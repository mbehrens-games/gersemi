/*******************************************************************************
** envelope.c (envelope)
*******************************************************************************/

#include <stdlib.h>
#include <math.h>

#include "bank.h"
#include "clock.h"
#include "envelope.h"
#include "midicont.h"
#include "patch.h"
#include "tuning.h"

enum
{
  ENVELOPE_STAGE_ATTACK = 0, 
  ENVELOPE_STAGE_DECAY, 
  ENVELOPE_STAGE_SUSTAIN, 
  ENVELOPE_STAGE_RELEASE 
};

/* routing */
#define ENVELOPE_ROUTING_CLEAR  0x00
#define ENVELOPE_ROUTING_MASK   0xFF

#define ENVELOPE_ROUTING_FLAG_MOD_WHEEL_BOOST     0x01
#define ENVELOPE_ROUTING_FLAG_MOD_WHEEL_TREMOLO   0x02
#define ENVELOPE_ROUTING_FLAG_AFTERTOUCH_BOOST    0x04
#define ENVELOPE_ROUTING_FLAG_AFTERTOUCH_TREMOLO  0x08
#define ENVELOPE_ROUTING_FLAG_EXP_PEDAL_BOOST     0x10
#define ENVELOPE_ROUTING_FLAG_EXP_PEDAL_TREMOLO   0x20
#define ENVELOPE_ROUTING_FLAG_NOTE_VELOCITY       0x40

/* 12 bit envelopes */
#define ENVELOPE_MAX_ATTENUATION  4095

/* note that 16 = 32 / 2, and adding 32 to the 12-bit envelope  */
/* is the same as adding 1 to a left shifted 7-bit envelope.    */
#define ENVELOPE_AMPLITUDE_STEP 16
#define ENVELOPE_SUSTAIN_STEP   16

/* note that adding 256 to the 12-bit envelope  */
/* is the same as multiplying the level by 1/2. */
#define ENVELOPE_SHIFT_STEP    256

#define ENVELOPE_BOOST_MAX_STEP       (1 * 32)
#define ENVELOPE_VELOCITY_MAX_STEP    (4 * 32)

#define ENVELOPE_NUM_OCTAVES      15
#define ENVELOPE_RATES_PER_OCTAVE 12

#define ENVELOPE_NUM_RATES (ENVELOPE_NUM_OCTAVES * ENVELOPE_RATES_PER_OCTAVE)

#define ENVELOPE_SET_STAGE(name)                                               \
  e->stage = ENVELOPE_STAGE_##name;                                            \
                                                                               \
  if (e->stage == ENVELOPE_STAGE_ATTACK)                                       \
    e->rate = e->a_rate;                                                       \
  else if (e->stage == ENVELOPE_STAGE_DECAY)                                   \
    e->rate = e->d_rate;                                                       \
  else if (e->stage == ENVELOPE_STAGE_RELEASE)                                 \
    e->rate = e->r_rate;                                                       \
  else                                                                         \
    e->rate = S_envelope_rate_table[0];                                        \
                                                                               \
  /* apply rate keyscaling */                                                  \
  e->rate += e->ks_rate_adjustment;                                               \
                                                                               \
  /* bound rate */                                                             \
  if (e->rate < 0)                                                             \
    e->rate = 0;                                                               \
  else if (e->rate > ENVELOPE_NUM_RATES - 1)                                   \
    e->rate = ENVELOPE_NUM_RATES - 1;                                          \
                                                                               \
  /* set the phase increment */                                                \
  e->increment = S_envelope_phase_increment_table[e->rate];

#define ENVELOPE_COMPUTE_COMBINED_POSITION(effect)                             \
  combined_pos = 0;                                                            \
                                                                               \
  if (e->routing & ENVELOPE_ROUTING_FLAG_MOD_WHEEL_##effect)                   \
    combined_pos += e->mod_wheel_pos;                                          \
                                                                               \
  if (e->routing & ENVELOPE_ROUTING_FLAG_AFTERTOUCH_##effect)                  \
    combined_pos += e->aftertouch_pos;                                         \
                                                                               \
  if (e->routing & ENVELOPE_ROUTING_FLAG_EXP_PEDAL_##effect)                   \
    combined_pos += e->exp_pedal_pos;                                          \
                                                                               \
  if (combined_pos < MIDI_CONT_UNI_WHEEL_LOWER_BOUND)                          \
    MIDI_CONT_UNI_WHEEL_LOWER_BOUND;                                           \
  else if (combined_pos > MIDI_CONT_UNI_WHEEL_UPPER_BOUND)                     \
    MIDI_CONT_UNI_WHEEL_UPPER_BOUND;

#define ENVELOPE_COMPUTE_TREMOLO_ADJUSTMENT()                                  \
  ENVELOPE_COMPUTE_COMBINED_POSITION(TREMOLO)                                  \
                                                                               \
  e->tremolo_adjustment =                                                      \
    (e->tremolo_input * combined_pos) / MIDI_CONT_UNI_WHEEL_DIVISOR;

#define ENVELOPE_COMPUTE_BOOST_ADJUSTMENT()                                    \
  ENVELOPE_COMPUTE_COMBINED_POSITION(BOOST)                                    \
                                                                               \
  e->boost_adjustment =                                                        \
    -((e->boost_max * combined_pos) / MIDI_CONT_UNI_WHEEL_DIVISOR);

/* envelope phase increment table */
static unsigned int S_envelope_phase_increment_table[ENVELOPE_NUM_RATES];

/* amplitude, sustain table */
static short int  S_envelope_amplitude_table[PATCH_ENV_LEVEL_NUM_VALUES];
static short int  S_envelope_sustain_table[PATCH_ENV_LEVEL_NUM_VALUES];

/* rate table */
static short int  S_envelope_rate_table[PATCH_ENV_TIME_NUM_VALUES];

/* keyscaling depth table */

/* for the rate, the value is the number of semitones */
/* up from the base note at which the rate doubles    */
/* (i.e., the ADSR value is incremented by 2)         */

/* for the level, the value is the number of semitones  */
/* up from the break point at which the level halves    */
static short int  S_envelope_keyscaling_fraction_table[PATCH_ENV_KEYSCALING_NUM_VALUES] = 
                  { 32767, 144, 96, 72, 48, 36, 24, 18, 12 };

/* boost & velocity tables */
static short int  S_envelope_boost_max_table[PATCH_SENSITIVITY_NUM_VALUES];
static short int  S_envelope_velocity_max_table[PATCH_SENSITIVITY_NUM_VALUES];

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

    /* initialize envelope variables */
    e->ks_rate_fraction = 
      S_envelope_keyscaling_fraction_table[PATCH_ENV_KEYSCALING_DEFAULT - PATCH_ENV_KEYSCALING_LOWER_BOUND];
    e->ks_level_fraction = 
      S_envelope_keyscaling_fraction_table[PATCH_ENV_KEYSCALING_DEFAULT - PATCH_ENV_KEYSCALING_LOWER_BOUND];

    e->ks_rate_adjustment = 0;
    e->ks_level_adjustment = 0;

    e->sustain_level = S_envelope_sustain_table[PATCH_ENV_LEVEL_DEFAULT - PATCH_ENV_LEVEL_LOWER_BOUND];

    e->a_rate = S_envelope_rate_table[PATCH_ENV_TIME_DEFAULT - PATCH_ENV_TIME_LOWER_BOUND];
    e->d_rate = S_envelope_rate_table[PATCH_ENV_TIME_DEFAULT - PATCH_ENV_TIME_LOWER_BOUND];
    e->r_rate = S_envelope_rate_table[PATCH_ENV_TIME_DEFAULT - PATCH_ENV_TIME_LOWER_BOUND];

    e->stage = ENVELOPE_STAGE_RELEASE;
    e->rate = S_envelope_rate_table[PATCH_ENV_TIME_DEFAULT - PATCH_ENV_TIME_LOWER_BOUND];

    e->increment = 0;
    e->phase = 0;

    e->attenuation = ENVELOPE_MAX_ATTENUATION;

    e->tremolo_input = 0;

    e->boost_max = S_envelope_boost_max_table[PATCH_SENSITIVITY_DEFAULT - PATCH_SENSITIVITY_LOWER_BOUND];
    e->velocity_max = S_envelope_velocity_max_table[PATCH_SENSITIVITY_DEFAULT - PATCH_SENSITIVITY_LOWER_BOUND];

    e->tremolo_adjustment = 0;
    e->boost_adjustment = 0;
    e->velocity_adjustment = 0;

    e->routing = ENVELOPE_ROUTING_CLEAR;

    e->mod_wheel_pos = 0;
    e->aftertouch_pos = 0;
    e->exp_pedal_pos = 0;

    e->volume_adjustment = 0;
    e->amplitude_adjustment = 0;

    e->level = ENVELOPE_MAX_ATTENUATION;
  }

  return 0;
}

/*******************************************************************************
** envelope_load_patch()
*******************************************************************************/
short int envelope_load_patch(int voice_index, int patch_index)
{
  int m;

  envelope* e;
  patch* p;

  short int combined_pos;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain patch pointer */
  p = &G_patch_bank[patch_index];

  for (m = 0; m < BANK_ENVELOPES_PER_VOICE; m++)
  {
    /* obtain envelope pointer */
    e = &G_envelope_bank[voice_index * BANK_ENVELOPES_PER_VOICE + m];

    /* attack rate */
    if ((p->env_attack[m] >= PATCH_ENV_TIME_LOWER_BOUND) && 
        (p->env_attack[m] <= PATCH_ENV_TIME_UPPER_BOUND))
    {
      e->a_rate = S_envelope_rate_table[p->env_attack[m] - PATCH_ENV_TIME_LOWER_BOUND];
    }
    else
      e->a_rate = S_envelope_rate_table[PATCH_ENV_TIME_DEFAULT - PATCH_ENV_TIME_LOWER_BOUND];

    /* decay rate */
    if ((p->env_decay[m] >= PATCH_ENV_TIME_LOWER_BOUND) && 
        (p->env_decay[m] <= PATCH_ENV_TIME_UPPER_BOUND))
    {
      e->d_rate = S_envelope_rate_table[p->env_decay[m] - PATCH_ENV_TIME_LOWER_BOUND];
    }
    else
      e->d_rate = S_envelope_rate_table[PATCH_ENV_TIME_DEFAULT - PATCH_ENV_TIME_LOWER_BOUND];

    /* release rate */
    if ((p->env_release[m] >= PATCH_ENV_TIME_LOWER_BOUND) && 
        (p->env_release[m] <= PATCH_ENV_TIME_UPPER_BOUND))
    {
      e->r_rate = S_envelope_rate_table[p->env_release[m] - PATCH_ENV_TIME_LOWER_BOUND];
    }
    else
      e->r_rate = S_envelope_rate_table[PATCH_ENV_TIME_DEFAULT - PATCH_ENV_TIME_LOWER_BOUND];

    /* amplitude */
    if ((p->env_amplitude[m] >= PATCH_ENV_LEVEL_LOWER_BOUND) && 
        (p->env_amplitude[m] <= PATCH_ENV_LEVEL_UPPER_BOUND))
    {
      e->amplitude_adjustment = S_envelope_amplitude_table[p->env_amplitude[m] - PATCH_ENV_LEVEL_LOWER_BOUND];
    }
    else
      e->amplitude_adjustment = S_envelope_amplitude_table[PATCH_ENV_LEVEL_DEFAULT - PATCH_ENV_LEVEL_LOWER_BOUND];

    /* sustain level */
    if ((p->env_sustain[m] >= PATCH_ENV_LEVEL_LOWER_BOUND) && 
        (p->env_sustain[m] <= PATCH_ENV_LEVEL_UPPER_BOUND))
    {
      e->sustain_level = S_envelope_sustain_table[p->env_sustain[m] - PATCH_ENV_LEVEL_LOWER_BOUND];
    }
    else
      e->sustain_level = S_envelope_sustain_table[PATCH_ENV_LEVEL_DEFAULT - PATCH_ENV_LEVEL_LOWER_BOUND];

    /* rate keyscaling */
    if ((p->env_rate_ks[m] >= PATCH_ENV_KEYSCALING_LOWER_BOUND) && 
        (p->env_rate_ks[m] <= PATCH_ENV_KEYSCALING_UPPER_BOUND))
    {
      e->ks_rate_fraction = S_envelope_keyscaling_fraction_table[p->env_rate_ks[m] - PATCH_ENV_KEYSCALING_LOWER_BOUND];
    }
    else
      e->ks_rate_fraction = S_envelope_keyscaling_fraction_table[PATCH_ENV_KEYSCALING_DEFAULT - PATCH_ENV_KEYSCALING_LOWER_BOUND];

    /* level keyscaling */
    if ((p->env_level_ks[m] >= PATCH_ENV_KEYSCALING_LOWER_BOUND) && 
        (p->env_level_ks[m] <= PATCH_ENV_KEYSCALING_UPPER_BOUND))
    {
      e->ks_level_fraction = S_envelope_keyscaling_fraction_table[p->env_level_ks[m] - PATCH_ENV_KEYSCALING_LOWER_BOUND];
    }
    else
      e->ks_level_fraction = S_envelope_keyscaling_fraction_table[PATCH_ENV_KEYSCALING_DEFAULT - PATCH_ENV_KEYSCALING_LOWER_BOUND];

    /* boost sensitivity */
    if ((p->sensitivity_boost >= PATCH_SENSITIVITY_LOWER_BOUND) && 
        (p->sensitivity_boost <= PATCH_SENSITIVITY_UPPER_BOUND))
    {
      e->boost_max = S_envelope_boost_max_table[p->sensitivity_boost - PATCH_SENSITIVITY_LOWER_BOUND];
    }
    else
      e->boost_max = S_envelope_boost_max_table[PATCH_SENSITIVITY_DEFAULT - PATCH_SENSITIVITY_LOWER_BOUND];

    /* velocity sensitivity */
    if ((p->sensitivity_velocity >= PATCH_SENSITIVITY_LOWER_BOUND) && 
        (p->sensitivity_velocity <= PATCH_SENSITIVITY_UPPER_BOUND))
    {
      e->velocity_max = S_envelope_velocity_max_table[p->sensitivity_velocity - PATCH_SENSITIVITY_LOWER_BOUND];
    }
    else
      e->velocity_max = S_envelope_velocity_max_table[PATCH_SENSITIVITY_DEFAULT - PATCH_SENSITIVITY_LOWER_BOUND];

    /* routing */
    e->routing = ENVELOPE_ROUTING_CLEAR;

    /* tremolo routing */
    if (((m == 0) && (p->tremolo_routing & PATCH_ENV_ADJUST_ROUTING_FLAG_1))  || 
        ((m == 1) && (p->tremolo_routing & PATCH_ENV_ADJUST_ROUTING_FLAG_2))  || 
        ((m == 2) && (p->tremolo_routing & PATCH_ENV_ADJUST_ROUTING_FLAG_3)))
    {
      if (p->mod_wheel_routing & PATCH_MIDI_CONT_ROUTING_FLAG_TREMOLO)
        e->routing |= ENVELOPE_ROUTING_FLAG_MOD_WHEEL_TREMOLO;
      else
        e->routing &= ~ENVELOPE_ROUTING_FLAG_MOD_WHEEL_TREMOLO;

      if (p->aftertouch_routing & PATCH_MIDI_CONT_ROUTING_FLAG_TREMOLO)
        e->routing |= ENVELOPE_ROUTING_FLAG_AFTERTOUCH_TREMOLO;
      else
        e->routing &= ~ENVELOPE_ROUTING_FLAG_AFTERTOUCH_TREMOLO;

      if (p->exp_pedal_routing & PATCH_MIDI_CONT_ROUTING_FLAG_TREMOLO)
        e->routing |= ENVELOPE_ROUTING_FLAG_EXP_PEDAL_TREMOLO;
      else
        e->routing &= ~ENVELOPE_ROUTING_FLAG_EXP_PEDAL_TREMOLO;
    }
    else
    {
      e->routing &= ~ENVELOPE_ROUTING_FLAG_MOD_WHEEL_TREMOLO;
      e->routing &= ~ENVELOPE_ROUTING_FLAG_AFTERTOUCH_TREMOLO;
      e->routing &= ~ENVELOPE_ROUTING_FLAG_EXP_PEDAL_TREMOLO;
    }

    /* boost routing */
    if (((m == 0) && (p->boost_routing & PATCH_ENV_ADJUST_ROUTING_FLAG_1))  || 
        ((m == 1) && (p->boost_routing & PATCH_ENV_ADJUST_ROUTING_FLAG_2))  || 
        ((m == 2) && (p->boost_routing & PATCH_ENV_ADJUST_ROUTING_FLAG_3)))
    {
      if (p->mod_wheel_routing & PATCH_MIDI_CONT_ROUTING_FLAG_BOOST)
        e->routing |= ENVELOPE_ROUTING_FLAG_MOD_WHEEL_BOOST;
      else
        e->routing &= ~ENVELOPE_ROUTING_FLAG_MOD_WHEEL_BOOST;

      if (p->aftertouch_routing & PATCH_MIDI_CONT_ROUTING_FLAG_BOOST)
        e->routing |= ENVELOPE_ROUTING_FLAG_AFTERTOUCH_BOOST;
      else
        e->routing &= ~ENVELOPE_ROUTING_FLAG_AFTERTOUCH_BOOST;

      if (p->exp_pedal_routing & PATCH_MIDI_CONT_ROUTING_FLAG_BOOST)
        e->routing |= ENVELOPE_ROUTING_FLAG_EXP_PEDAL_BOOST;
      else
        e->routing &= ~ENVELOPE_ROUTING_FLAG_EXP_PEDAL_BOOST;
    }
    else
    {
      e->routing &= ~ENVELOPE_ROUTING_FLAG_MOD_WHEEL_BOOST;
      e->routing &= ~ENVELOPE_ROUTING_FLAG_AFTERTOUCH_BOOST;
      e->routing &= ~ENVELOPE_ROUTING_FLAG_EXP_PEDAL_BOOST;
    }

    /* velocity routing */
    if (((m == 0) && (p->velocity_routing & PATCH_ENV_ADJUST_ROUTING_FLAG_1)) || 
        ((m == 1) && (p->velocity_routing & PATCH_ENV_ADJUST_ROUTING_FLAG_2)) || 
        ((m == 2) && (p->velocity_routing & PATCH_ENV_ADJUST_ROUTING_FLAG_3)))
    {
      e->routing |= ENVELOPE_ROUTING_FLAG_NOTE_VELOCITY;
    }
    else
      e->routing &= ~ENVELOPE_ROUTING_FLAG_NOTE_VELOCITY;

    /* determine the tremolo and boost adjustments */
    ENVELOPE_COMPUTE_TREMOLO_ADJUSTMENT()
    ENVELOPE_COMPUTE_BOOST_ADJUSTMENT()
  }

  return 0;
}

/*******************************************************************************
** envelope_set_mod_wheel_position()
*******************************************************************************/
short int envelope_set_mod_wheel_position(int voice_index, int pos)
{
  int m;

  envelope* e;

  short int combined_pos;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* if position is out of range, ignore */
  if ((pos < MIDI_CONT_UNI_WHEEL_LOWER_BOUND) || 
      (pos > MIDI_CONT_UNI_WHEEL_UPPER_BOUND))
  {
    return 1;
  }

  for (m = 0; m < BANK_ENVELOPES_PER_VOICE; m++)
  {
    /* obtain envelope pointer */
    e = &G_envelope_bank[voice_index * BANK_ENVELOPES_PER_VOICE + m];

    /* set the mod wheel position */
    e->mod_wheel_pos = pos;

    /* determine the tremolo and boost adjustments */
    ENVELOPE_COMPUTE_TREMOLO_ADJUSTMENT()
    ENVELOPE_COMPUTE_BOOST_ADJUSTMENT()
  }

  return 0;
}

/*******************************************************************************
** envelope_set_aftertouch_position()
*******************************************************************************/
short int envelope_set_aftertouch_position(int voice_index, int pos)
{
  int m;

  envelope* e;

  short int combined_pos;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* if position is out of range, ignore */
  if ((pos < MIDI_CONT_UNI_WHEEL_LOWER_BOUND) || 
      (pos > MIDI_CONT_UNI_WHEEL_UPPER_BOUND))
  {
    return 1;
  }

  for (m = 0; m < BANK_ENVELOPES_PER_VOICE; m++)
  {
    /* obtain envelope pointer */
    e = &G_envelope_bank[voice_index * BANK_ENVELOPES_PER_VOICE + m];

    /* set the aftertouch position */
    e->aftertouch_pos = pos;

    /* determine the tremolo and boost adjustments */
    ENVELOPE_COMPUTE_TREMOLO_ADJUSTMENT()
    ENVELOPE_COMPUTE_BOOST_ADJUSTMENT()
  }

  return 0;
}

/*******************************************************************************
** envelope_set_exp_pedal_position()
*******************************************************************************/
short int envelope_set_exp_pedal_position(int voice_index, int pos)
{
  int m;

  envelope* e;

  short int combined_pos;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* if position is out of range, ignore */
  if ((pos < MIDI_CONT_UNI_WHEEL_LOWER_BOUND) || 
      (pos > MIDI_CONT_UNI_WHEEL_UPPER_BOUND))
  {
    return 1;
  }

  for (m = 0; m < BANK_ENVELOPES_PER_VOICE; m++)
  {
    /* obtain envelope pointer */
    e = &G_envelope_bank[voice_index * BANK_ENVELOPES_PER_VOICE + m];

    /* set the exp pedal position */
    e->exp_pedal_pos = pos;

    /* determine the tremolo and boost adjustments */
    ENVELOPE_COMPUTE_TREMOLO_ADJUSTMENT()
    ENVELOPE_COMPUTE_BOOST_ADJUSTMENT()
  }

  return 0;
}

/*******************************************************************************
** envelope_set_note()
*******************************************************************************/
short int envelope_set_note(int voice_index, int note, int vel)
{
  int m;

  envelope* e;

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

    /* compute rate & level adjustments based on note */

    /* note that adding 256 to the base level is  */
    /* the same as multiplying it by 1/2 (once    */
    /* converted back to linear instead of db).   */
    e->ks_rate_adjustment = (12 * (note - TUNING_NOTE_C0)) / e->ks_rate_fraction;
    e->ks_level_adjustment = (256 * (note - TUNING_NOTE_A2)) / e->ks_level_fraction;

    /* set the velocity adjustment */
    if (e->routing & ENVELOPE_ROUTING_FLAG_NOTE_VELOCITY)
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
  }

  return 0;
}

/*******************************************************************************
** envelope_trigger()
*******************************************************************************/
short int envelope_trigger(int voice_index)
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

    /* set level */
    e->level = e->attenuation + e->ks_level_adjustment;

    /* bound level */
    if (e->level < 0)
      e->level = 0;
    else if (e->level > ENVELOPE_MAX_ATTENUATION)
      e->level = ENVELOPE_MAX_ATTENUATION;

    /* set the envelope to its initial stage */
    ENVELOPE_SET_STAGE(ATTACK)

    /* reset phase */
    e->phase = 0;
  }

  return 0;
}

/*******************************************************************************
** envelope_release()
*******************************************************************************/
short int envelope_release(int voice_index)
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

    /* if this envelope is already released, return */
    if (e->stage == ENVELOPE_STAGE_RELEASE)
      return 0;

    /* set envelope to the release stage */
    ENVELOPE_SET_STAGE(RELEASE)
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

        e->attenuation = (11 * e->attenuation) / 12;
      }
      /* falling stages */
      else if (e->stage == ENVELOPE_STAGE_DECAY)
        e->attenuation += 4;
      else if (e->stage == ENVELOPE_STAGE_SUSTAIN)
        e->attenuation += 0;
      else if (e->stage == ENVELOPE_STAGE_RELEASE)
        e->attenuation += 4;

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
                (e->attenuation >= e->sustain_level))
      {
        ENVELOPE_SET_STAGE(SUSTAIN)
      }

      /* update level */
      e->level = e->attenuation;
      e->level += e->ks_level_adjustment;

      e->level += e->tremolo_adjustment;
      e->level += e->boost_adjustment;
      e->level += e->velocity_adjustment;

      e->level += e->volume_adjustment;
      e->level += e->amplitude_adjustment;

      /* bound level */
      if (e->level < 0)
        e->level = 0;
      else if (e->level > ENVELOPE_MAX_ATTENUATION)
        e->level = ENVELOPE_MAX_ATTENUATION;
    }
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

  int quotient;
  int remainder;

  /* amplitude table */
  S_envelope_amplitude_table[0] = ENVELOPE_MAX_ATTENUATION;

  for ( m = PATCH_ENV_LEVEL_LOWER_BOUND + 1; 
        m <= PATCH_ENV_LEVEL_UPPER_BOUND; 
        m++)
  {
    S_envelope_amplitude_table[m - PATCH_ENV_LEVEL_LOWER_BOUND] = 
      ENVELOPE_AMPLITUDE_STEP * (PATCH_ENV_LEVEL_UPPER_BOUND - m);
  }

  /* sustain table */
  S_envelope_sustain_table[0] = ENVELOPE_MAX_ATTENUATION;

  for ( m = PATCH_ENV_LEVEL_LOWER_BOUND + 1; 
        m <= PATCH_ENV_LEVEL_UPPER_BOUND; 
        m++)
  {
    S_envelope_sustain_table[m - PATCH_ENV_LEVEL_LOWER_BOUND] = 
      ENVELOPE_SUSTAIN_STEP * (PATCH_ENV_LEVEL_UPPER_BOUND - m);
  }

  /* rate table */
  for ( m = PATCH_ENV_TIME_LOWER_BOUND; 
        m <= PATCH_ENV_TIME_UPPER_BOUND; 
        m++)
  {
    /* there are 7 times for each octave.     */
    /* the octaves are numbered from 0 to 14. */

    /* we set up the calculation here so that   */
    /* times 1-7 map to the highest octave, 14. */
    quotient =  (PATCH_ENV_TIME_UPPER_BOUND - m + 5) / 7;
    remainder = (PATCH_ENV_TIME_UPPER_BOUND - m + 5) % 7;

    S_envelope_rate_table[m - PATCH_ENV_TIME_LOWER_BOUND] = 12 * quotient;

    if (remainder == 0)
      S_envelope_rate_table[m - PATCH_ENV_TIME_LOWER_BOUND] += 0;
    else if (remainder == 1)
      S_envelope_rate_table[m - PATCH_ENV_TIME_LOWER_BOUND] += 2;
    else if (remainder == 2)
      S_envelope_rate_table[m - PATCH_ENV_TIME_LOWER_BOUND] += 4;
    else if (remainder == 3)
      S_envelope_rate_table[m - PATCH_ENV_TIME_LOWER_BOUND] += 5;
    else if (remainder == 4)
      S_envelope_rate_table[m - PATCH_ENV_TIME_LOWER_BOUND] += 7;
    else if (remainder == 5)
      S_envelope_rate_table[m - PATCH_ENV_TIME_LOWER_BOUND] += 9;
    else
      S_envelope_rate_table[m - PATCH_ENV_TIME_LOWER_BOUND] += 11;
  }

  /* phase increment table  */
  for (n = 0; n < ENVELOPE_NUM_OCTAVES; n++)
  {
    for (m = 0; m < ENVELOPE_RATES_PER_OCTAVE; m++)
    {
      S_envelope_phase_increment_table[ n * ENVELOPE_RATES_PER_OCTAVE + m] = 
        (int) ((8.0f * pow(2, (12 * n + m) / 12.0f) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
    }
  }

  /* boost max table */
  for (m = 0; m < PATCH_SENSITIVITY_NUM_VALUES; m++)
  {
    S_envelope_boost_max_table[m] = m * ENVELOPE_BOOST_MAX_STEP;
  }

  /* velocity max table */
  for (m = 0; m < PATCH_SENSITIVITY_NUM_VALUES; m++)
  {
    S_envelope_velocity_max_table[m] = m * ENVELOPE_VELOCITY_MAX_STEP;
  }

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

