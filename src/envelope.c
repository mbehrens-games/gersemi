/*******************************************************************************
** envelope.c (envelope)
*******************************************************************************/

#include <stdlib.h>
#include <math.h>

#include "bank.h"
#include "cart.h"
#include "clock.h"
#include "envelope.h"
#include "midicont.h"
#include "tuning.h"

enum
{
  ENVELOPE_STAGE_ATTACK = 1, 
  ENVELOPE_STAGE_DECAY, 
  ENVELOPE_STAGE_SUSTAIN, 
  ENVELOPE_STAGE_RELEASE 
};

/* 12 bit envelopes */
#define ENVELOPE_MAX_ATTENUATION  4095

#define ENVELOPE_NUM_OCTAVES      16
#define ENVELOPE_RATES_PER_OCTAVE 12

#define ENVELOPE_NUM_RATES (ENVELOPE_NUM_OCTAVES * ENVELOPE_RATES_PER_OCTAVE)

#define ENVELOPE_SET_STAGE(name)                                               \
  e->stage = ENVELOPE_STAGE_##name;                                            \
                                                                               \
  if (e->stage == ENVELOPE_STAGE_ATTACK)                                       \
    e->rate = e->a_rate;                                                       \
  else if (e->stage == ENVELOPE_STAGE_DECAY)                                   \
    e->rate = e->d_rate;                                                       \
  else if (e->stage == ENVELOPE_STAGE_SUSTAIN)                                 \
    e->rate = e->s_rate;                                                       \
  else if (e->stage == ENVELOPE_STAGE_RELEASE)                                 \
    e->rate = e->r_rate;                                                       \
  else                                                                         \
    e->rate = S_envelope_rate_table[0];                                        \
                                                                               \
  /* apply rate keyscaling */                                                  \
  e->rate += e->ks_rate_adjustment;                                            \
                                                                               \
  /* bound rate */                                                             \
  if (e->rate < 0)                                                             \
    e->rate = 0;                                                               \
  else if (e->rate > ENVELOPE_NUM_RATES - 1)                                   \
    e->rate = ENVELOPE_NUM_RATES - 1;                                          \
                                                                               \
  /* set the phase increment */                                                \
  if (e->stage == ENVELOPE_STAGE_ATTACK)                                       \
    e->increment = S_envelope_attack_phase_increment_table[e->rate];           \
  else                                                                         \
    e->increment = S_envelope_decay_phase_increment_table[e->rate];            \
                                                                               \
  /* reset phase */                                                            \
  e->phase = 0;

#define ENVELOPE_BOUND_LEVEL()                                                 \
  if (e->level < 0)                                                            \
    e->level = 0;                                                              \
  else if (e->level > ENVELOPE_MAX_ATTENUATION)                                \
    e->level = ENVELOPE_MAX_ATTENUATION;

#define ENVELOPE_COMPUTE_COMBINED_POSITION(effect)                             \
  combined_pos = 0;                                                            \
                                                                               \
  if (e->mod_wheel_routing & PATCH_MIDI_CONT_ROUTING_FLAG_##effect)            \
    combined_pos += e->mod_wheel_pos;                                          \
                                                                               \
  if (e->aftertouch_routing & PATCH_MIDI_CONT_ROUTING_FLAG_##effect)           \
    combined_pos += e->aftertouch_pos;                                         \
                                                                               \
  if (e->exp_pedal_routing & PATCH_MIDI_CONT_ROUTING_FLAG_##effect)            \
    combined_pos += e->exp_pedal_pos;                                          \
                                                                               \
  if (combined_pos < MIDI_CONT_UNI_WHEEL_LOWER_BOUND)                          \
    MIDI_CONT_UNI_WHEEL_LOWER_BOUND;                                           \
  else if (combined_pos > MIDI_CONT_UNI_WHEEL_UPPER_BOUND)                     \
    MIDI_CONT_UNI_WHEEL_UPPER_BOUND;

/* multiple table */
/* the values form the harmonic series! */
static int  S_envelope_multiple_table[16] = 
            { 0 * 12 + 0,   /*  1x  */
              1 * 12 + 0,   /*  2x  */
              1 * 12 + 7,   /*  3x  */
              2 * 12 + 0,   /*  4x  */
              2 * 12 + 4,   /*  5x  */
              2 * 12 + 7,   /*  6x  */
              2 * 12 + 10,  /*  7x  */
              3 * 12 + 0,   /*  8x  */
              3 * 12 + 2,   /*  9x  */
              3 * 12 + 4,   /* 10x  */
              3 * 12 + 6,   /* 11x  */
              3 * 12 + 7,   /* 12x  */
              3 * 12 + 8,   /* 13x  */
              3 * 12 + 10,  /* 14x  */
              3 * 12 + 11,  /* 15x  */
              4 * 12 + 0    /* 16x  */
            };

/* envelope phase increment tables */
static unsigned int S_envelope_attack_phase_increment_table[ENVELOPE_NUM_RATES];
static unsigned int S_envelope_decay_phase_increment_table[ENVELOPE_NUM_RATES];

/* amplitude, hold level table */
static short int  S_envelope_max_level_table[128];
static short int  S_envelope_hold_level_table[16];

/* rate table */
static short int  S_envelope_rate_table[32];

/* keyscaling fraction tables */

/* for the rate, the value is the number of semitones */
/* up from the base note at which the rate doubles    */
/* (i.e., the ADSR value is incremented by 2)         */

/* for the level, the value is the number of semitones  */
/* up from the break point at which the level halves    */
static short int  S_envelope_keyscaling_fraction_table[4] = 
                  { 8 * (12 + 0), 4 * (12 + 0), 
                    2 * (12 + 0), 1 * (12 + 0) 
                  };

/* boost & velocity tables */
static short int  S_envelope_boost_max_table[8];
static short int  S_envelope_velocity_max_table[8];

/* envelope bank */
envelope G_envelope_bank[BANK_NUM_ENVELOPES];

/*******************************************************************************
** envelope_reset_all()
*******************************************************************************/
short int envelope_reset_all()
{
  int k;

  envelope* e;

  /* reset all envelopes */
  for (k = 0; k < BANK_NUM_ENVELOPES; k++)
  {
    /* obtain envelope pointer */
    e = &G_envelope_bank[k];

    /* keyscaling */
    e->ks_rate_fraction = S_envelope_keyscaling_fraction_table[0];
    e->ks_level_fraction = S_envelope_keyscaling_fraction_table[0];

    e->ks_rate_adjustment = 0;
    e->ks_level_adjustment = 0;

    /* frequency mode, note offset */
    e->freq_mode = 0;
    e->note_offset = 0;

    /* hold parameters */
    e->hold_level = S_envelope_hold_level_table[0];
    e->hold_mode = 0;
    e->hold_switch = MIDI_CONT_TOGGLE_SWITCH_OFF;

    /* routing */
    e->env_routing = 0x00;

    /* rates */
    e->a_rate = S_envelope_rate_table[0];
    e->d_rate = S_envelope_rate_table[0];
    e->s_rate = S_envelope_rate_table[0];
    e->r_rate = S_envelope_rate_table[0];

    /* current stage, rate */
    e->stage = ENVELOPE_STAGE_RELEASE;
    e->rate = S_envelope_rate_table[0];

    /* phase increment, phase */
    e->increment = 0;
    e->phase = 0;

    /* attenuation */
    e->attenuation = ENVELOPE_MAX_ATTENUATION;

    /* tremolo, boost, velocity */
    e->tremolo_base = 0;
    e->tremolo_extra = 0;

    e->boost_max = S_envelope_boost_max_table[0];
    e->velocity_max = S_envelope_velocity_max_table[0];

    /* midi controller routing */
    e->mod_wheel_routing = 0x00;
    e->aftertouch_routing = 0x00;
    e->exp_pedal_routing = 0x00;

    /* midi controller positions */
    e->mod_wheel_pos = MIDI_CONT_UNI_WHEEL_DEFAULT;
    e->aftertouch_pos = MIDI_CONT_UNI_WHEEL_DEFAULT;
    e->exp_pedal_pos = MIDI_CONT_UNI_WHEEL_DEFAULT;

    /* volume, amplitude, velocity */
    e->volume_adjustment = 0;
    e->amplitude_adjustment = 0;
    e->velocity_adjustment = 0;

    /* level (current attenuation + adjustments) */
    e->level = ENVELOPE_MAX_ATTENUATION;
  }

  return 0;
}

/*******************************************************************************
** envelope_load_patch()
*******************************************************************************/
short int envelope_load_patch(int voice_index, 
                              int cart_index, int patch_index)
{
  int m;

  envelope* e;

  cart* c;
  patch* p;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* make sure that the cart & patch indices are valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(cart_index))
    return 1;

  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain cart & patch pointers */
  c = &G_cart_bank[cart_index];
  p = &(c->patches[patch_index]);

  for (m = 0; m < BANK_ENVELOPES_PER_VOICE; m++)
  {
    /* obtain envelope pointer */
    e = &G_envelope_bank[voice_index * BANK_ENVELOPES_PER_VOICE + m];

    /* load patch parameters */
    if (PATCH_PARAM_IS_VALID_LOOKUP_ENV_PARAM(ATTACK, m + 1))
      e->a_rate = S_envelope_rate_table[p->values[PATCH_PARAM_COMPUTE_ENV_INDEX(ATTACK, m + 1)]];
    else
      e->a_rate = S_envelope_rate_table[0];

    if (PATCH_PARAM_IS_VALID_LOOKUP_ENV_PARAM(DECAY, m + 1))
      e->d_rate = S_envelope_rate_table[p->values[PATCH_PARAM_COMPUTE_ENV_INDEX(DECAY, m + 1)]];
    else
      e->d_rate = S_envelope_rate_table[0];

    if (PATCH_PARAM_IS_VALID_LOOKUP_ENV_PARAM(SUSTAIN, m + 1))
      e->s_rate = S_envelope_rate_table[p->values[PATCH_PARAM_COMPUTE_ENV_INDEX(SUSTAIN, m + 1)]];
    else
      e->s_rate = S_envelope_rate_table[0];

    if (PATCH_PARAM_IS_VALID_LOOKUP_ENV_PARAM(RELEASE, m + 1))
      e->r_rate = S_envelope_rate_table[2 * p->values[PATCH_PARAM_COMPUTE_ENV_INDEX(RELEASE, m + 1)]];
    else
      e->r_rate = S_envelope_rate_table[0];

    if (PATCH_PARAM_IS_VALID_LOOKUP_ENV_PARAM(MAX_LEVEL, m + 1))
      e->amplitude_adjustment = S_envelope_max_level_table[p->values[PATCH_PARAM_COMPUTE_ENV_INDEX(MAX_LEVEL, m + 1)]];
    else
      e->amplitude_adjustment = S_envelope_max_level_table[0];

    if (PATCH_PARAM_IS_VALID_LOOKUP_ENV_PARAM(HOLD_LEVEL, m + 1))
      e->hold_level = S_envelope_hold_level_table[p->values[PATCH_PARAM_COMPUTE_ENV_INDEX(HOLD_LEVEL, m + 1)]];
    else
      e->hold_level = S_envelope_hold_level_table[0];

    if (PATCH_PARAM_IS_VALID_LOOKUP_ENV_PARAM(HOLD_MODE, m + 1))
      e->hold_mode = p->values[PATCH_PARAM_COMPUTE_ENV_INDEX(HOLD_MODE, m + 1)];
    else
      e->hold_mode = 0;

    if (PATCH_PARAM_IS_VALID_LOOKUP_ENV_PARAM(RATE_KEYSCALING, m + 1))
      e->ks_rate_fraction = S_envelope_keyscaling_fraction_table[p->values[PATCH_PARAM_COMPUTE_ENV_INDEX(RATE_KEYSCALING, m + 1)]];
    else
      e->ks_rate_fraction = S_envelope_keyscaling_fraction_table[0];

    if (PATCH_PARAM_IS_VALID_LOOKUP_ENV_PARAM(LEVEL_KEYSCALING, m + 1))
      e->ks_level_fraction = S_envelope_keyscaling_fraction_table[p->values[PATCH_PARAM_COMPUTE_ENV_INDEX(LEVEL_KEYSCALING, m + 1)]];
    else
      e->ks_level_fraction = S_envelope_keyscaling_fraction_table[0];

    if (PATCH_PARAM_IS_VALID_LOOKUP_OSC_PARAM(FREQ_MODE, m + 1))
      e->freq_mode = p->values[PATCH_PARAM_COMPUTE_OSC_INDEX(FREQ_MODE, m + 1)];
    else
      e->freq_mode = 0;

    /* fixed */
    if (e->freq_mode == 1)
    {
      e->note_offset = 0;

#if 0
      e->note_offset = TUNING_NOTE_C0;

      if ((p->osc_octave[m] >= PATCH_OSC_OCTAVE_LOWER_BOUND) && 
          (p->osc_octave[m] <= PATCH_OSC_OCTAVE_UPPER_BOUND))
      {
        e->note_offset += 12 * (p->osc_octave[m] - PATCH_OSC_OCTAVE_LOWER_BOUND);
      }
      else
        e->note_offset += 12 * (PATCH_OSC_OCTAVE_DEFAULT - PATCH_OSC_OCTAVE_LOWER_BOUND);

      if ((p->osc_note[m] >= PATCH_OSC_NOTE_LOWER_BOUND) && 
          (p->osc_note[m] <= PATCH_OSC_NOTE_UPPER_BOUND))
      {
        e->note_offset += p->osc_note[m] - PATCH_OSC_NOTE_LOWER_BOUND;
      }
      else
        e->note_offset += PATCH_OSC_NOTE_DEFAULT - PATCH_OSC_NOTE_LOWER_BOUND;
#endif
    }
    /* ratio */
    else
    {
      e->note_offset = 0;

      if (PATCH_PARAM_IS_VALID_LOOKUP_OSC_PARAM(MULTIPLE, m + 1))
        e->note_offset += S_envelope_multiple_table[p->values[PATCH_PARAM_COMPUTE_OSC_INDEX(MULTIPLE, m + 1)]];
      else
        e->note_offset += S_envelope_multiple_table[0];

      if (PATCH_PARAM_IS_VALID_LOOKUP_OSC_PARAM(DIVISOR, m + 1))
        e->note_offset -= S_envelope_multiple_table[p->values[PATCH_PARAM_COMPUTE_OSC_INDEX(DIVISOR, m + 1)]];
      else
        e->note_offset -= S_envelope_multiple_table[0];
    }

    if (PATCH_PARAM_IS_VALID_LOOKUP_BY_NAME(BOOST_SENSITIVITY))
      e->boost_max = S_envelope_boost_max_table[p->values[PATCH_PARAM_BOOST_SENSITIVITY]];
    else
      e->boost_max = S_envelope_boost_max_table[0];

    if (PATCH_PARAM_IS_VALID_LOOKUP_BY_NAME(VELOCITY_SENSITIVITY))
      e->velocity_max = S_envelope_velocity_max_table[p->values[PATCH_PARAM_VELOCITY_SENSITIVITY]];
    else
      e->velocity_max = S_envelope_velocity_max_table[0];

    /* routing */
    e->env_routing = 0x00;

    /* midi controller routing */
    e->mod_wheel_routing = 0x00;
    e->aftertouch_routing = 0x00;
    e->exp_pedal_routing = 0x00;
  }

  return 0;
}

/*******************************************************************************
** envelope_note_on()
*******************************************************************************/
short int envelope_note_on(int voice_index, int note, int vel, int pedal_state)
{
  int m;

  envelope* e;

  short int shifted_note;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* if note is out of range, ignore */
  if (TUNING_NOTE_IS_NOT_VALID(note))
    return 1;

  /* if velocity is out of range, ignore */
  if ((vel < MIDI_CONT_NOTE_VELOCITY_LOWER_BOUND) || 
      (vel > MIDI_CONT_NOTE_VELOCITY_UPPER_BOUND))
  {
    return 1;
  }

  for (m = 0; m < BANK_ENVELOPES_PER_VOICE; m++)
  {
    /* obtain envelope pointer */
    e = &G_envelope_bank[voice_index * BANK_ENVELOPES_PER_VOICE + m];

    /* compute shifted note */
    if (e->freq_mode == 1)
      shifted_note = e->note_offset;
    else
      shifted_note = note + e->note_offset;

    if (shifted_note < TUNING_NOTE_C0)
      shifted_note = TUNING_NOTE_C0;
    else if (shifted_note > TUNING_NOTE_B9)
      shifted_note = TUNING_NOTE_B9;

    /* compute rate & level adjustments based on note */

    /* note that adding 256 to the base level is  */
    /* the same as multiplying it by 1/2 (once    */
    /* converted back to linear instead of db).   */
    e->ks_rate_adjustment = (12 * (shifted_note - TUNING_NOTE_C0)) / e->ks_rate_fraction;
    e->ks_level_adjustment = (256 * (shifted_note - TUNING_NOTE_A2)) / e->ks_level_fraction;

    /* set the velocity adjustment */
#if 0
    if (e->env_routing & PATCH_ENV_ROUTING_FLAG_VELOCITY)
    {
      if ((vel >= MIDI_CONT_NOTE_VELOCITY_LOWER_BOUND) && 
          (vel <= MIDI_CONT_NOTE_VELOCITY_SPLIT_POINT))
      {
        e->velocity_adjustment = (e->velocity_max * (MIDI_CONT_NOTE_VELOCITY_SPLIT_POINT - vel)) / MIDI_CONT_NOTE_VELOCITY_DIVISOR;
      }
      else if ( (vel >  MIDI_CONT_NOTE_VELOCITY_SPLIT_POINT) && 
                (vel <= MIDI_CONT_NOTE_VELOCITY_UPPER_BOUND))
      {
        e->velocity_adjustment = -((e->velocity_max * (vel - MIDI_CONT_NOTE_VELOCITY_SPLIT_POINT)) / MIDI_CONT_NOTE_VELOCITY_DIVISOR);
      }
      else
        e->velocity_adjustment = 0;
    }
    else
      e->velocity_adjustment = 0;
#endif

    /* set the envelope to its initial stage */
    ENVELOPE_SET_STAGE(ATTACK)

    /* set the hold switch */
    if (pedal_state == MIDI_CONT_SUSTAIN_PEDAL_DOWN)
      e->hold_switch = MIDI_CONT_TOGGLE_SWITCH_ON;
    else
      e->hold_switch = MIDI_CONT_TOGGLE_SWITCH_OFF;
  }

  return 0;
}

/*******************************************************************************
** envelope_note_off()
*******************************************************************************/
short int envelope_note_off(int voice_index)
{
  int m;

  envelope* e;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  for (m = 0; m < BANK_ENVELOPES_PER_VOICE; m++)
  {
    /* obtain envelope pointer */
    e = &G_envelope_bank[voice_index * BANK_ENVELOPES_PER_VOICE + m];

    /* if this envelope is already note_offd, continue */
    if (e->stage == ENVELOPE_STAGE_RELEASE)
      continue;

    /* set envelope to the note_off stage */
    ENVELOPE_SET_STAGE(RELEASE)

    /* reset the hold switch */
    e->hold_switch = MIDI_CONT_TOGGLE_SWITCH_OFF;
  }

  return 0;
}

/*******************************************************************************
** envelope_update_all()
*******************************************************************************/
short int envelope_update_all()
{
  int k;

  envelope* e;

  int tremolo_adjustment;
  int boost_adjustment;

  short int combined_pos;
  short int periods;

  /* update all envelopes */
  for (k = 0; k < BANK_NUM_ENVELOPES; k++)
  {
    /* obtain envelope pointer */
    e = &G_envelope_bank[k];

    /* update phase */
    e->phase += e->increment;

    /* check if a period was completed */
    if (e->phase > 0xFFFFFFF)
    {
      periods = (e->phase >> 28) & 0x0F;

      e->phase &= 0xFFFFFFF;
    }
    else
      periods = 0;

    /* if a period has elapsed, update the envelope */
    while (periods > 0)
    {
      periods -= 1;

      /* rising stages */
      if (e->stage == ENVELOPE_STAGE_ATTACK)
      {
        /*e->attenuation = (15 * e->attenuation - 1) / 16;*/

        e->attenuation = (127 * e->attenuation) / 128;
      }
      /* falling stages */
      else if (e->stage == ENVELOPE_STAGE_DECAY)
        e->attenuation += 1;
      else if (e->stage == ENVELOPE_STAGE_SUSTAIN)
      {
        if (e->hold_mode == 0)
          e->attenuation += 1;
        else if (e->hold_mode == 1)
        {
          if (e->hold_switch == MIDI_CONT_TOGGLE_SWITCH_ON)
            e->attenuation += 0;
          else
            e->attenuation += 1;
        }
        else if (e->hold_mode == 2)
        {
          if (e->hold_switch == MIDI_CONT_TOGGLE_SWITCH_OFF)
            e->attenuation += 0;
          else
            e->attenuation += 1;
        }
        else if (e->hold_mode == 3)
          e->attenuation += 0;
        else
          e->attenuation += 1;
      }
      else if (e->stage == ENVELOPE_STAGE_RELEASE)
        e->attenuation += 1;

      /* bound attenuation */
      if (e->attenuation < 0)
        e->attenuation = 0;
      else if (e->attenuation > ENVELOPE_MAX_ATTENUATION)
        e->attenuation = ENVELOPE_MAX_ATTENUATION;

      /* change stage if necessary */
      if ((e->stage == ENVELOPE_STAGE_ATTACK) && 
          (e->attenuation == 0))
      {
        ENVELOPE_SET_STAGE(DECAY)
      }
      else if ( (e->stage == ENVELOPE_STAGE_DECAY) && 
                (e->attenuation >= e->hold_level))
      {
        ENVELOPE_SET_STAGE(SUSTAIN)
      }
      else if ( (e->stage == ENVELOPE_STAGE_SUSTAIN) && 
                (e->attenuation >= ENVELOPE_MAX_ATTENUATION))
      {
        ENVELOPE_SET_STAGE(RELEASE)
      }
    }

#if 0
    /* compute tremolo adjustment */
    if (e->env_routing & PATCH_ENV_ROUTING_FLAG_TREMOLO)
    {
      ENVELOPE_COMPUTE_COMBINED_POSITION(TREMOLO)

      tremolo_adjustment = e->tremolo_base;
      tremolo_adjustment += 
        (e->tremolo_extra * combined_pos) / MIDI_CONT_UNI_WHEEL_DIVISOR;
    }
    else
      tremolo_adjustment = 0;

    /* compute boost adjustment */
    if (e->env_routing & PATCH_ENV_ROUTING_FLAG_BOOST)
    {
      ENVELOPE_COMPUTE_COMBINED_POSITION(BOOST)

      boost_adjustment = 
        -((e->boost_max * combined_pos) / MIDI_CONT_UNI_WHEEL_DIVISOR);
    }
    else
      boost_adjustment = 0;
#endif

    /* update level */
    e->level = e->attenuation;

#if 0
    e->level += tremolo_adjustment;
    e->level += boost_adjustment;
#endif

    e->level += e->ks_level_adjustment;

    e->level += e->volume_adjustment;
    e->level += e->amplitude_adjustment;
    e->level += e->velocity_adjustment;

    /* bound level */
    ENVELOPE_BOUND_LEVEL()
  }

  return 0;
}

/*******************************************************************************
** envelope_generate_tables()
*******************************************************************************/
short int envelope_generate_tables()
{
  int m;
  int n;

  float base;
  float freq;

  /* note that adding 32 to the 12-bit envelope is the  */
  /* same as adding 1 to a left shifted 7-bit envelope. */

  /* amplitude table */
  S_envelope_max_level_table[0] = ENVELOPE_MAX_ATTENUATION;

  for (m = 1; m < 128; m++)
    S_envelope_max_level_table[m] = (127 - m) * (1 * 32);

  /* hold level table */
  S_envelope_hold_level_table[0] = ENVELOPE_MAX_ATTENUATION;

  for (m = 1; m < 16; m++)
    S_envelope_hold_level_table[m] = (15 - m) * (4 * 32);

  /* boost max table */
  for (m = 0; m < 8; m++)
    S_envelope_boost_max_table[m] = m * (1 * 32);

  /* velocity max table */
  for (m = 0; m < 8; m++)
    S_envelope_velocity_max_table[m] = m * (4 * 32);

  /* rate table */
  for (m = 0; m < 32; m++)
  {
    S_envelope_rate_table[m] = 12 * ((31 - m) / 2);

    if ((31 - m) % 2 == 0)
      S_envelope_rate_table[m] += 0;
    else
      S_envelope_rate_table[m] += 7;
  }

  /* phase increment tables  */

  /* for the decay stage, the fastest rate should have a fall time  */
  /* of ~8 ms. thus with 16 octaves, the lowest rate is ~512 s.     */
  /* so, the base frequency is (1 / 512) * 4095, where 4095 is the  */
  /* number of updates per fall time (with a 12 bit envelope).      */
  base = ENVELOPE_MAX_ATTENUATION / 512.0f;

  for (n = 0; n < ENVELOPE_NUM_OCTAVES; n++)
  {
    for (m = 0; m < ENVELOPE_RATES_PER_OCTAVE; m++)
    {
      freq = base * pow(2, (12 * n + m) / 12.0f);

      S_envelope_decay_phase_increment_table[ n * ENVELOPE_RATES_PER_OCTAVE + m] = 
        (int) ((freq * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

#if 0
      printf( "Decay Time %d: %f, %f (%u)\n", 
              (n * ENVELOPE_RATES_PER_OCTAVE + m), freq, (ENVELOPE_MAX_ATTENUATION / freq), 
              S_envelope_decay_phase_increment_table[ n * ENVELOPE_RATES_PER_OCTAVE + m]);
#endif
    }
  }

  /* for the attack stage, the fastest rate should have a rise time */
  /* of ~4 ms. thus with 16 octaves, the lowest rate is ~256 s.     */
  /* the attack phase has 518 updates per rise time.                */
  base = 518.0f / 256.0f;

  for (n = 0; n < ENVELOPE_NUM_OCTAVES; n++)
  {
    for (m = 0; m < ENVELOPE_RATES_PER_OCTAVE; m++)
    {
      freq = base * pow(2, (12 * n + m) / 12.0f);

      S_envelope_attack_phase_increment_table[ n * ENVELOPE_RATES_PER_OCTAVE + m] = 
        (int) ((freq * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

#if 0
      printf( "Attack Time %d: %f, %f (%u)\n", 
              (n * ENVELOPE_RATES_PER_OCTAVE + m), freq, (518 / freq), 
              S_envelope_attack_phase_increment_table[ n * ENVELOPE_RATES_PER_OCTAVE + m]);
#endif
    }
  }

#if 0
  /* testing */
  int count;
  int att;

  count = 0;
  att = ENVELOPE_MAX_ATTENUATION;

  while (att > 0)
  {
    att = (127 * att) / 128;

    count += 1;
  }

  printf("Envelope Attack Stage Number of Updates: %d\n", count);
#endif

#if 0
  printf("Envelope Sustain Table:\n");

  for (m = 0; m < PATCH_ENV_LEVEL_NUM_VALUES; m++)
    printf("Envelope Sustain %d: %d\n", m, S_envelope_sustain_table[m]);
#endif

#if 0
  printf("Envelope Rate Table:\n");

  for (m = 0; m < PATCH_ENV_TIME_NUM_VALUES; m++)
    printf("Envelope Rate %d: %d\n", m + 1, S_envelope_rate_table[m]);
#endif

#if 0
  printf("Envelope Frequency Table:\n");

  for (n = 0; n < ENVELOPE_NUM_OCTAVES; n++)
  {
    for (m = 0; m < ENVELOPE_RATES_PER_OCTAVE; m++)
    {
      printf("Envelope Frequency %d: %f\n", (n * ENVELOPE_RATES_PER_OCTAVE) + m, 2 * pow(2, (12 * n + m) / 12.0f));
    }
  }
#endif

  return 0;
}

