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
  ENVELOPE_STAGE_DECAY_1, 
  ENVELOPE_STAGE_DECAY_2, 
  ENVELOPE_STAGE_RELEASE, 
  ENVELOPE_STAGE_SUSTAINED 
};

/* 12 bit envelopes */
#define ENVELOPE_MAX_ATTENUATION  4095

#define ENVELOPE_SUSTAIN_STEP     16

#define ENVELOPE_NUM_OCTAVES       17
#define ENVELOPE_RATES_PER_OCTAVE  12

#define ENVELOPE_NUM_RATES (ENVELOPE_NUM_OCTAVES * ENVELOPE_RATES_PER_OCTAVE)

#define ENVELOPE_SET_STAGE(name)                                               \
  e->stage = ENVELOPE_STAGE_##name;                                            \
                                                                               \
  if (e->stage == ENVELOPE_STAGE_ATTACK)                                       \
    e->rate = e->a_rate;                                                       \
  else if (e->stage == ENVELOPE_STAGE_DECAY_1)                                 \
    e->rate = e->d1_rate;                                                      \
  else if (e->stage == ENVELOPE_STAGE_DECAY_2)                                 \
    e->rate = e->d2_rate;                                                      \
  else if (e->stage == ENVELOPE_STAGE_RELEASE)                                 \
    e->rate = e->r_rate;                                                       \
  else if (e->stage == ENVELOPE_STAGE_SUSTAINED)                               \
    e->rate = e->pedal_rate;                                                   \
  else                                                                         \
    e->rate = S_envelope_rate_table[0];                                        \
                                                                               \
  /* apply rate keyscaling */                                                  \
  e->rate += e->rate_adjustment;                                               \
                                                                               \
  /* bound rate */                                                             \
  if (e->rate < 0)                                                             \
    e->rate = 0;                                                               \
  else if (e->rate > ENVELOPE_NUM_RATES - 1)                                   \
    e->rate = ENVELOPE_NUM_RATES - 1;                                          \
                                                                               \
  /* set the phase increment */                                                \
  e->increment = S_envelope_phase_increment_table[e->rate];

/* envelope phase increment table */
static unsigned int S_envelope_phase_increment_table[ENVELOPE_NUM_RATES];

/* sustain table */
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
                  { 192, 144, 96, 72, 48, 36, 24, 18, 12 };

/* break point table */
static short int  S_envelope_break_point_table[PATCH_ENV_BREAK_POINT_NUM_VALUES] = 
                  { TUNING_NOTE_C0 + (0 * 12 + 9), /* A-0 */
                    TUNING_NOTE_C0 + (1 * 12 + 2), /* D-1 */
                    TUNING_NOTE_C0 + (1 * 12 + 9), /* A-1 */
                    TUNING_NOTE_C0 + (2 * 12 + 2), /* D-2 */
                    TUNING_NOTE_C0 + (2 * 12 + 9), /* A-2 */
                    TUNING_NOTE_C0 + (3 * 12 + 2), /* D-3 */
                    TUNING_NOTE_C0 + (3 * 12 + 9), /* A-3 */
                    TUNING_NOTE_C0 + (4 * 12 + 2), /* D-4 */
                    TUNING_NOTE_C0 + (4 * 12 + 9), /* A-4 */
                    TUNING_NOTE_C0 + (5 * 12 + 2), /* D-5 */
                    TUNING_NOTE_C0 + (5 * 12 + 9), /* A-5 */
                    TUNING_NOTE_C0 + (6 * 12 + 2), /* D-6 */
                    TUNING_NOTE_C0 + (6 * 12 + 9), /* A-6 */
                    TUNING_NOTE_C0 + (7 * 12 + 2), /* D-7 */
                    TUNING_NOTE_C0 + (7 * 12 + 9), /* A-7 */
                    TUNING_NOTE_C0 + (8 * 12 + 0)  /* C-8 */
                  };

/* pedal adjust table */
static short int  S_envelope_pedal_adjust_table[PATCH_PEDAL_ADJUST_NUM_VALUES] = 
                  {  0,  6, 12, 18, 24, 30, 36, 42, 48 };

/* envelope bank */
envelope G_envelope_bank[BANK_NUM_AMPLITUDE_ENVELOPES];

/*******************************************************************************
** envelope_reset_all()
*******************************************************************************/
short int envelope_reset_all()
{
  int k;

  envelope* e;

  /* reset all envelopes */
  for (k = 0; k < BANK_NUM_AMPLITUDE_ENVELOPES; k++)
  {
    /* obtain envelope pointer */
    e = &G_envelope_bank[k];

    /* initialize envelope variables */
    e->ks_rate_fraction = 
      S_envelope_keyscaling_fraction_table[PATCH_ENV_KEYSCALING_DEFAULT - PATCH_ENV_KEYSCALING_LOWER_BOUND];
    e->ks_left_fraction = 
      S_envelope_keyscaling_fraction_table[PATCH_ENV_KEYSCALING_DEFAULT - PATCH_ENV_KEYSCALING_LOWER_BOUND];
    e->ks_right_fraction = 
      S_envelope_keyscaling_fraction_table[PATCH_ENV_KEYSCALING_DEFAULT - PATCH_ENV_KEYSCALING_LOWER_BOUND];
    e->ks_break_note = 
      S_envelope_break_point_table[PATCH_ENV_BREAK_POINT_DEFAULT - PATCH_ENV_BREAK_POINT_LOWER_BOUND];

    e->rate_adjustment = 0;
    e->level_adjustment = 0;

    e->transition_level = S_envelope_sustain_table[PATCH_ENV_LEVEL_DEFAULT - PATCH_ENV_LEVEL_LOWER_BOUND];

    e->sustain_pedal = MIDI_CONT_SUSTAIN_PEDAL_DEFAULT;

    e->a_rate = S_envelope_rate_table[PATCH_ENV_TIME_DEFAULT - PATCH_ENV_TIME_LOWER_BOUND];
    e->d1_rate = S_envelope_rate_table[PATCH_ENV_TIME_DEFAULT - PATCH_ENV_TIME_LOWER_BOUND];
    e->d2_rate = S_envelope_rate_table[PATCH_ENV_TIME_DEFAULT - PATCH_ENV_TIME_LOWER_BOUND];
    e->r_rate = S_envelope_rate_table[PATCH_ENV_TIME_DEFAULT - PATCH_ENV_TIME_LOWER_BOUND];

    e->pedal_rate = S_envelope_rate_table[PATCH_ENV_TIME_DEFAULT - PATCH_ENV_TIME_LOWER_BOUND];

    e->stage = ENVELOPE_STAGE_RELEASE;
    e->rate = S_envelope_rate_table[PATCH_ENV_TIME_DEFAULT - PATCH_ENV_TIME_LOWER_BOUND];

    e->increment = 0;
    e->phase = 0;

    e->attenuation = ENVELOPE_MAX_ATTENUATION;

    e->level = ENVELOPE_MAX_ATTENUATION;
  }

  return 0;
}

/*******************************************************************************
** envelope_load_patch()
*******************************************************************************/
short int envelope_load_patch(int voice_index, int patch_index)
{
  envelope* e;
  patch* p;

  int shifted_time;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain patch pointer */
  p = &G_patch_bank[patch_index];

  /* obtain envelope pointer */
  e = &G_envelope_bank[voice_index];

  /* attack rate */
  if ((p->amp_env_attack >= PATCH_ENV_TIME_LOWER_BOUND) && 
      (p->amp_env_attack <= PATCH_ENV_TIME_UPPER_BOUND))
  {
    e->a_rate = S_envelope_rate_table[p->amp_env_attack - PATCH_ENV_TIME_LOWER_BOUND];
  }
  else
    e->a_rate = S_envelope_rate_table[PATCH_ENV_TIME_DEFAULT - PATCH_ENV_TIME_LOWER_BOUND];

  /* decay 1 rate */
  if ((p->amp_env_decay_1 >= PATCH_ENV_TIME_LOWER_BOUND) && 
      (p->amp_env_decay_1 <= PATCH_ENV_TIME_UPPER_BOUND))
  {
    e->d1_rate = S_envelope_rate_table[p->amp_env_decay_1 - PATCH_ENV_TIME_LOWER_BOUND];
  }
  else
    e->d1_rate = S_envelope_rate_table[PATCH_ENV_TIME_DEFAULT - PATCH_ENV_TIME_LOWER_BOUND];

  /* decay 2 rate */
  if ((p->amp_env_decay_2 >= PATCH_ENV_TIME_LOWER_BOUND) && 
      (p->amp_env_decay_2 <= PATCH_ENV_TIME_UPPER_BOUND))
  {
    e->d2_rate = S_envelope_rate_table[p->amp_env_decay_2 - PATCH_ENV_TIME_LOWER_BOUND];
  }
  else
    e->d2_rate = S_envelope_rate_table[PATCH_ENV_TIME_DEFAULT - PATCH_ENV_TIME_LOWER_BOUND];

  /* release rate */
  if ((p->amp_env_release >= PATCH_ENV_TIME_LOWER_BOUND) && 
      (p->amp_env_release <= PATCH_ENV_TIME_UPPER_BOUND))
  {
    e->r_rate = S_envelope_rate_table[p->amp_env_release - PATCH_ENV_TIME_LOWER_BOUND];
  }
  else
    e->r_rate = S_envelope_rate_table[PATCH_ENV_TIME_DEFAULT - PATCH_ENV_TIME_LOWER_BOUND];

  /* sustain pedal adjust */
  if ((p->pedal_adjust >= PATCH_PEDAL_ADJUST_LOWER_BOUND) && 
      (p->pedal_adjust <= PATCH_PEDAL_ADJUST_UPPER_BOUND))
  {
    /* set sustained rate */
    shifted_time = p->amp_env_decay_2 + S_envelope_pedal_adjust_table[p->pedal_adjust - PATCH_PEDAL_ADJUST_LOWER_BOUND];

    if (shifted_time < PATCH_ENV_TIME_LOWER_BOUND)
      shifted_time = PATCH_ENV_TIME_LOWER_BOUND;
    else if (shifted_time > PATCH_ENV_TIME_UPPER_BOUND)
      shifted_time = PATCH_ENV_TIME_UPPER_BOUND;

    e->pedal_rate = S_envelope_rate_table[shifted_time - PATCH_ENV_TIME_LOWER_BOUND];
  }
  else
    e->pedal_rate = e->d2_rate;

  /* sustain level */
  if ((p->amp_env_sustain >= PATCH_ENV_LEVEL_LOWER_BOUND) && 
      (p->amp_env_sustain <= PATCH_ENV_LEVEL_UPPER_BOUND))
  {
    e->transition_level = S_envelope_sustain_table[p->amp_env_sustain - PATCH_ENV_LEVEL_LOWER_BOUND];
  }
  else
    e->transition_level = S_envelope_sustain_table[PATCH_ENV_LEVEL_DEFAULT - PATCH_ENV_LEVEL_LOWER_BOUND];

  /* rate keyscaling depth */
  if ((p->amp_env_rate_ks >= PATCH_ENV_KEYSCALING_LOWER_BOUND) && 
      (p->amp_env_rate_ks <= PATCH_ENV_KEYSCALING_UPPER_BOUND))
  {
    e->ks_rate_fraction = S_envelope_keyscaling_fraction_table[p->amp_env_rate_ks - PATCH_ENV_KEYSCALING_LOWER_BOUND];
  }
  else
    e->ks_rate_fraction = S_envelope_keyscaling_fraction_table[PATCH_ENV_KEYSCALING_DEFAULT - PATCH_ENV_KEYSCALING_LOWER_BOUND];

  /* left level keyscaling depth */
  if ((p->amp_env_left_ks >= PATCH_ENV_KEYSCALING_LOWER_BOUND) && 
      (p->amp_env_left_ks <= PATCH_ENV_KEYSCALING_UPPER_BOUND))
  {
    e->ks_left_fraction = S_envelope_keyscaling_fraction_table[p->amp_env_left_ks - PATCH_ENV_KEYSCALING_LOWER_BOUND];
  }
  else
    e->ks_left_fraction = S_envelope_keyscaling_fraction_table[PATCH_ENV_KEYSCALING_DEFAULT - PATCH_ENV_KEYSCALING_LOWER_BOUND];

  /* right level keyscaling depth */
  if ((p->amp_env_right_ks >= PATCH_ENV_KEYSCALING_LOWER_BOUND) && 
      (p->amp_env_right_ks <= PATCH_ENV_KEYSCALING_UPPER_BOUND))
  {
    e->ks_right_fraction = S_envelope_keyscaling_fraction_table[p->amp_env_right_ks - PATCH_ENV_KEYSCALING_LOWER_BOUND];
  }
  else
    e->ks_right_fraction = S_envelope_keyscaling_fraction_table[PATCH_ENV_KEYSCALING_DEFAULT - PATCH_ENV_KEYSCALING_LOWER_BOUND];

  /* break point */
  if ((p->amp_env_break_point >= PATCH_ENV_BREAK_POINT_LOWER_BOUND) && 
      (p->amp_env_break_point <= PATCH_ENV_BREAK_POINT_UPPER_BOUND))
  {
    e->ks_break_note = S_envelope_break_point_table[p->amp_env_break_point - PATCH_ENV_BREAK_POINT_LOWER_BOUND];
  }
  else
    e->ks_break_note = S_envelope_break_point_table[PATCH_ENV_BREAK_POINT_DEFAULT - PATCH_ENV_BREAK_POINT_LOWER_BOUND];

  return 0;
}

/*******************************************************************************
** envelope_set_sustain_pedal()
*******************************************************************************/
short int envelope_set_sustain_pedal(int voice_index, int state)
{
  envelope* e;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* make sure the pedal state is valid */
  if ((state < MIDI_CONT_SUSTAIN_PEDAL_LOWER_BOUND) || 
      (state > MIDI_CONT_SUSTAIN_PEDAL_UPPER_BOUND))
  {
    return 0;
  }

  /* obtain envelope pointer */
  e = &G_envelope_bank[voice_index];

  /* set the sustain pedal */
  e->sustain_pedal = state;

  /* update envelope stages */
  if (e->sustain_pedal == MIDI_CONT_PEDAL_STATE_DOWN)
  {
    if (e->stage == ENVELOPE_STAGE_DECAY_2)
    {
      ENVELOPE_SET_STAGE(SUSTAINED)
    }
  }
  else if (e->sustain_pedal == MIDI_CONT_PEDAL_STATE_UP)
  {
    if (e->stage == ENVELOPE_STAGE_SUSTAINED)
    {
      ENVELOPE_SET_STAGE(DECAY_2)
    }
  }

  return 0;
}

/*******************************************************************************
** envelope_set_note()
*******************************************************************************/
short int envelope_set_note(int voice_index, int note)
{
  envelope* e;

  short int adjusted_note;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* if note is out of range, ignore */
  if (TUNING_NOTE_IS_NOT_VALID(note))
    return 1;

  /* obtain envelope pointer */
  e = &G_envelope_bank[voice_index];

  /* set the current note */
  if (note < 0)
    adjusted_note = 0;
  else if (note > TUNING_NUM_VALID_NOTES - 1)
    adjusted_note = TUNING_NUM_VALID_NOTES - 1;
  else
    adjusted_note = note;

  /* compute rate & level adjustments based on note */

  /* note that adding 256 to the base level is  */
  /* the same as multiplying it by 1/2 (once    */
  /* converted back to linear instead of db).   */
  e->rate_adjustment = (12 * (adjusted_note - TUNING_NOTE_C0)) / e->ks_rate_fraction;

  if (adjusted_note < e->ks_break_note)
    e->level_adjustment = (256 * (adjusted_note - e->ks_break_note)) / e->ks_left_fraction;
  else
    e->level_adjustment = (256 * (adjusted_note - e->ks_break_note)) / e->ks_right_fraction;

  return 0;
}

/*******************************************************************************
** envelope_trigger()
*******************************************************************************/
short int envelope_trigger(int voice_index)
{
  envelope* e;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain envelope pointer */
  e = &G_envelope_bank[voice_index];

  /* set level */
  e->level = e->attenuation + e->level_adjustment;

  /* bound level */
  if (e->level < 0)
    e->level = 0;
  else if (e->level > ENVELOPE_MAX_ATTENUATION)
    e->level = ENVELOPE_MAX_ATTENUATION;

  /* set the envelope to its initial stage */
  ENVELOPE_SET_STAGE(ATTACK)

  /* reset phase */
  e->phase = 0;

  return 0;
}

/*******************************************************************************
** envelope_release()
*******************************************************************************/
short int envelope_release(int voice_index)
{
  envelope* e;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain envelope pointer */
  e = &G_envelope_bank[voice_index];

  /* if this envelope is already released, return */
  if (e->stage == ENVELOPE_STAGE_RELEASE)
    return 0;

  /* set envelope to the release stage */
  ENVELOPE_SET_STAGE(RELEASE)

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
  for (k = 0; k < BANK_NUM_AMPLITUDE_ENVELOPES; k++)
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
      else if ( (e->stage == ENVELOPE_STAGE_DECAY_1)  || 
                (e->stage == ENVELOPE_STAGE_DECAY_2)  || 
                (e->stage == ENVELOPE_STAGE_RELEASE)  || 
                (e->stage == ENVELOPE_STAGE_SUSTAINED))
      {
        e->attenuation += 4;
      }
      else
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
        ENVELOPE_SET_STAGE(DECAY_1)
      }
      else if ( (e->stage == ENVELOPE_STAGE_DECAY_1) && 
                (e->attenuation >= e->transition_level))
      {
        if (e->sustain_pedal == MIDI_CONT_PEDAL_STATE_DOWN)
        {
          ENVELOPE_SET_STAGE(SUSTAINED)
        }
        else
        {
          ENVELOPE_SET_STAGE(DECAY_2)
        }
      }
      else if ( (e->stage == ENVELOPE_STAGE_DECAY_2) || 
                (e->stage == ENVELOPE_STAGE_SUSTAINED))
      {
        if (e->attenuation == ENVELOPE_MAX_ATTENUATION)
        {
          ENVELOPE_SET_STAGE(RELEASE)
        }
      }

      /* update level */
      e->level = e->attenuation + e->level_adjustment;

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
    /* there are 6 times for each octave.     */
    /* the octaves are numbered from 0 to 16. */

    /* we set up the calculation here so that   */
    /* times 1-6 map to the highest octave, 16. */
    quotient =  (PATCH_ENV_TIME_UPPER_BOUND - m + 2) / 6;
    remainder = (PATCH_ENV_TIME_UPPER_BOUND - m + 2) % 6;

    S_envelope_rate_table[m - PATCH_ENV_TIME_LOWER_BOUND] = 12 * quotient;

    if (remainder == 0)
      S_envelope_rate_table[m - PATCH_ENV_TIME_LOWER_BOUND] += 0;
    else if (remainder == 1)
      S_envelope_rate_table[m - PATCH_ENV_TIME_LOWER_BOUND] += 3;
    else if (remainder == 2)
      S_envelope_rate_table[m - PATCH_ENV_TIME_LOWER_BOUND] += 5;
    else if (remainder == 3)
      S_envelope_rate_table[m - PATCH_ENV_TIME_LOWER_BOUND] += 7;
    else if (remainder == 4)
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
        (int) ((2 * pow(2, (12 * n + m) / 12.0f) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
    }
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

