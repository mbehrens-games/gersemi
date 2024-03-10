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
  ENVELOPE_STATE_ATTACK = 0, 
  ENVELOPE_STATE_DECAY_1, 
  ENVELOPE_STATE_DECAY_2, 
  ENVELOPE_STATE_RELEASE, 
  ENVELOPE_STATE_SUSTAINED 
};

#define ENVELOPE_AMPLITUDE_STEP  8
#define ENVELOPE_SUSTAIN_STEP   32

#define ENVELOPE_TABLE_NUM_ROWS       16
#define ENVELOPE_TABLE_RATES_PER_ROW  12

#define ENVELOPE_TABLE_SIZE (ENVELOPE_TABLE_NUM_ROWS * ENVELOPE_TABLE_RATES_PER_ROW)

#define ENVELOPE_MAX_RATE_ROW (12 * ((PATCH_ENV_RATE_UPPER_BOUND - PATCH_ENV_RATE_LOWER_BOUND) / 2) + 7)

#define ENVELOPE_SET_STATE(name)                                               \
  e->state = ENVELOPE_STATE_##name;                                            \
                                                                               \
  if (e->state == ENVELOPE_STATE_ATTACK)                                       \
    e->row = e->a_row;                                                         \
  else if (e->state == ENVELOPE_STATE_DECAY_1)                                 \
    e->row = e->d1_row;                                                        \
  else if (e->state == ENVELOPE_STATE_DECAY_2)                                 \
    e->row = e->d2_row;                                                        \
  else if (e->state == ENVELOPE_STATE_RELEASE)                                 \
    e->row = e->r_row;                                                         \
  else if (e->state == ENVELOPE_STATE_SUSTAINED)                               \
    e->row = e->pedal_row;                                                     \
  else                                                                         \
    e->row = ENVELOPE_MAX_RATE_ROW;                                            \
                                                                               \
  /* apply rate keyscaling */                                                  \
  e->row += e->rate_adjustment;                                                \
                                                                               \
  /* bound row index */                                                        \
  if (e->row < 0)                                                              \
    e->row = 0;                                                                \
  else if (e->row > ENVELOPE_TABLE_SIZE - 1)                                   \
    e->row = ENVELOPE_TABLE_SIZE - 1;                                          \
                                                                               \
  /* set the phase increment */                                                \
  e->increment = S_envelope_phase_increment_table[e->row];

/* envelope phase increment table */
static unsigned int S_envelope_phase_increment_table[ENVELOPE_TABLE_SIZE];

/* envelope base frequency table */
static float S_envelope_freq_table[12] = 
              { 16.0f,                /* 16             */
                16.951409509748724f,  /* 16 * 2^(1/12)  */
                17.959392772949968f,  /* 16 * 2^(2/12)  */
                19.027313840043537f,  /* 16 * 2^(3/12)  */
                20.158736798317971f,  /* 16 * 2^(4/12)  */
                21.357437666720550f,  /* 16 * 2^(5/12)  */
                22.627416997969521f,  /* 16 * 2^(6/12)  */
                23.972913230026904f,  /* 16 * 2^(7/12)  */
                25.398416831491192f,  /* 16 * 2^(8/12)  */
                26.908685288118865f,  /* 16 * 2^(9/12)  */
                28.508758980490858f,  /* 16 * 2^(10/12) */
                30.203978005814192f   /* 16 * 2^(11/12) */
              };

/* amplitude & sustain tables */
static short int  S_envelope_amplitude_table[PATCH_ENV_AMPLITUDE_NUM_VALUES];
static short int  S_envelope_sustain_table[PATCH_ENV_SUSTAIN_NUM_VALUES];

/* keyscaling depth table */

/* for the rate, the value is the number of semitones */
/* up from the base note at which the rate doubles    */
/* (i.e., the ADSR value is incremented by 2)         */

/* for the level, the value is the number of semitones  */
/* up from the break point at which the level halves    */
static short int  S_envelope_keyscaling_fraction_table[PATCH_ENV_KEYSCALING_NUM_VALUES] = 
                  { 144, 96, 72, 48, 36, 24, 18, 12 };

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

/* multiple table */

/* the values are in semitones    */
/* they form the harmonic series! */
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

/* pedal adjust table */
static short int  S_envelope_pedal_adjust_table[PATCH_PEDAL_ADJUST_NUM_VALUES] = 
                  {  0,  2,  4,  6,  8, 10, 12, 14, 16 };

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
    e->ks_break_note = 
      S_envelope_break_point_table[PATCH_ENV_BREAK_POINT_DEFAULT - PATCH_ENV_BREAK_POINT_LOWER_BOUND];

    e->ampl_adjustment = S_envelope_amplitude_table[PATCH_ENV_AMPLITUDE_DEFAULT - PATCH_ENV_AMPLITUDE_LOWER_BOUND];
    e->rate_adjustment = 0;
    e->level_adjustment = 0;

    e->transition_level = S_envelope_sustain_table[PATCH_ENV_SUSTAIN_DEFAULT - PATCH_ENV_SUSTAIN_LOWER_BOUND];

    e->sustain_pedal = MIDI_CONT_SUSTAIN_PEDAL_DEFAULT;

    e->offset = 0;

    e->a_row = 0;
    e->d1_row = 0;
    e->d2_row = 0;
    e->r_row = 0;

    e->pedal_row = 0;

    e->state = ENVELOPE_STATE_RELEASE;
    e->row = 0;

    e->increment = 0;
    e->phase = 0;

    e->attenuation = 1023;

    e->level = 1023;
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

  int shifted_rate;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain patch pointer */
  p = &G_patch_bank[patch_index];

  for (m = 0; m < BANK_OSCS_AND_ENVS_PER_VOICE; m++)
  {
    /* obtain envelope pointer */
    e = &G_envelope_bank[BANK_OSCS_AND_ENVS_PER_VOICE * voice_index + m];

    /* attack rate */
    if ((p->env_attack[m] >= PATCH_ENV_RATE_LOWER_BOUND) && 
        (p->env_attack[m] <= PATCH_ENV_RATE_UPPER_BOUND))
    {
      e->a_row =  12 * ((PATCH_ENV_RATE_UPPER_BOUND - p->env_attack[m]) / 2);
      e->a_row +=  7 * ((PATCH_ENV_RATE_UPPER_BOUND - p->env_attack[m]) % 2);
    }
    else
      e->a_row = 0;

    /* decay 1 rate */
    if ((p->env_decay_1[m] >= PATCH_ENV_RATE_LOWER_BOUND) && 
        (p->env_decay_1[m] <= PATCH_ENV_RATE_UPPER_BOUND))
    {
      e->d1_row =   12 * ((PATCH_ENV_RATE_UPPER_BOUND - p->env_decay_1[m]) / 2);
      e->d1_row +=   7 * ((PATCH_ENV_RATE_UPPER_BOUND - p->env_decay_1[m]) % 2);
    }
    else
      e->d1_row = 0;

    /* decay 2 rate */
    if ((p->env_decay_2[m] >= PATCH_ENV_RATE_LOWER_BOUND) && 
        (p->env_decay_2[m] <= PATCH_ENV_RATE_UPPER_BOUND))
    {
      e->d2_row =   12 * ((PATCH_ENV_RATE_UPPER_BOUND - p->env_decay_2[m]) / 2);
      e->d2_row +=   7 * ((PATCH_ENV_RATE_UPPER_BOUND - p->env_decay_2[m]) % 2);
    }
    else
      e->d2_row = 0;

    /* release rate */
    if ((p->env_release[m] >= PATCH_ENV_RATE_LOWER_BOUND) && 
        (p->env_release[m] <= PATCH_ENV_RATE_UPPER_BOUND))
    {
      e->r_row =  12 * ((PATCH_ENV_RATE_UPPER_BOUND - p->env_release[m]) / 2);
      e->r_row +=  7 * ((PATCH_ENV_RATE_UPPER_BOUND - p->env_release[m]) % 2);
    }
    else
      e->r_row = 0;

    /* sustain pedal adjust */
    if ((p->pedal_adjust >= PATCH_PEDAL_ADJUST_LOWER_BOUND) && 
        (p->pedal_adjust <= PATCH_PEDAL_ADJUST_UPPER_BOUND))
    {
      /* set sustained rate */
      shifted_rate = p->env_decay_2[m] + S_envelope_pedal_adjust_table[p->pedal_adjust - PATCH_PEDAL_ADJUST_LOWER_BOUND];

      if (shifted_rate < PATCH_ENV_RATE_LOWER_BOUND)
        shifted_rate = PATCH_ENV_RATE_LOWER_BOUND;
      else if (shifted_rate > PATCH_ENV_RATE_UPPER_BOUND)
        shifted_rate = PATCH_ENV_RATE_UPPER_BOUND;

      e->pedal_row =  12 * ((PATCH_ENV_RATE_UPPER_BOUND - shifted_rate) / 2);
      e->pedal_row +=  7 * ((PATCH_ENV_RATE_UPPER_BOUND - shifted_rate) % 2);
    }
    else
      e->pedal_row = e->d2_row;

    /* amplitude adjustment */
    if ((p->env_amplitude[m] >= PATCH_ENV_AMPLITUDE_LOWER_BOUND) && 
        (p->env_amplitude[m] <= PATCH_ENV_AMPLITUDE_UPPER_BOUND))
    {
      e->ampl_adjustment = S_envelope_amplitude_table[p->env_amplitude[m] - PATCH_ENV_AMPLITUDE_LOWER_BOUND];
    }
    else
      e->ampl_adjustment = S_envelope_amplitude_table[0];

    /* sustain level */
    if ((p->env_sustain[m] >= PATCH_ENV_SUSTAIN_LOWER_BOUND) && 
        (p->env_sustain[m] <= PATCH_ENV_SUSTAIN_UPPER_BOUND))
    {
      e->transition_level = S_envelope_sustain_table[p->env_sustain[m] - PATCH_ENV_SUSTAIN_LOWER_BOUND];
    }
    else
      e->transition_level = S_envelope_sustain_table[0];

    /* rate keyscaling depth */
    if ((p->env_rate_ks[m] >= PATCH_ENV_KEYSCALING_LOWER_BOUND) && 
        (p->env_rate_ks[m] <= PATCH_ENV_KEYSCALING_UPPER_BOUND))
    {
      e->ks_rate_fraction = S_envelope_keyscaling_fraction_table[p->env_rate_ks[m] - PATCH_ENV_KEYSCALING_LOWER_BOUND];
    }
    else
      e->ks_rate_fraction = S_envelope_keyscaling_fraction_table[0];

    /* level keyscaling depth */
    if ((p->env_level_ks[m] >= PATCH_ENV_KEYSCALING_LOWER_BOUND) && 
        (p->env_level_ks[m] <= PATCH_ENV_KEYSCALING_UPPER_BOUND))
    {
      e->ks_level_fraction = S_envelope_keyscaling_fraction_table[p->env_level_ks[m] - PATCH_ENV_KEYSCALING_LOWER_BOUND];
    }
    else
      e->ks_level_fraction = S_envelope_keyscaling_fraction_table[0];

    /* break point */
    if ((p->env_break_point[m] >= PATCH_ENV_BREAK_POINT_LOWER_BOUND) && 
        (p->env_break_point[m] <= PATCH_ENV_BREAK_POINT_UPPER_BOUND))
    {
      e->ks_break_note = S_envelope_break_point_table[p->env_break_point[m] - PATCH_ENV_BREAK_POINT_LOWER_BOUND];
    }
    else
      e->ks_break_note = S_envelope_break_point_table[0];

    /* note offset */
    e->offset = 0;

    if ((p->osc_multiple[m] >= PATCH_OSC_MULTIPLE_LOWER_BOUND) && 
        (p->osc_multiple[m] <= PATCH_OSC_MULTIPLE_UPPER_BOUND))
    {
      e->offset += S_envelope_multiple_table[p->osc_multiple[m] - PATCH_OSC_MULTIPLE_LOWER_BOUND];
    }

    if ((p->osc_divisor[m] >= PATCH_OSC_DIVISOR_LOWER_BOUND) && 
        (p->osc_divisor[m] <= PATCH_OSC_DIVISOR_UPPER_BOUND))
    {
      e->offset -= S_envelope_multiple_table[p->osc_divisor[m] - PATCH_OSC_DIVISOR_LOWER_BOUND];
    }
  }

  return 0;
}

/*******************************************************************************
** envelope_set_sustain_pedal()
*******************************************************************************/
short int envelope_set_sustain_pedal(int voice_index, int state)
{
  int m;

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

  for (m = 0; m < BANK_OSCS_AND_ENVS_PER_VOICE; m++)
  {
    /* obtain envelope pointer */
    e = &G_envelope_bank[BANK_OSCS_AND_ENVS_PER_VOICE * voice_index + m];

    /* set the sustain pedal */
    e->sustain_pedal = state;

    /* update envelope states */
    if (e->sustain_pedal == MIDI_CONT_PEDAL_STATE_DOWN)
    {
      if (e->state == ENVELOPE_STATE_DECAY_2)
      {
        ENVELOPE_SET_STATE(SUSTAINED)
      }
    }
    else if (e->sustain_pedal == MIDI_CONT_PEDAL_STATE_UP)
    {
      if (e->state == ENVELOPE_STATE_SUSTAINED)
      {
        ENVELOPE_SET_STATE(DECAY_2)
      }
    }
  }

  return 0;
}

/*******************************************************************************
** envelope_set_note()
*******************************************************************************/
short int envelope_set_note(int voice_index, int note)
{
  int m;

  envelope* e;

  short int adjusted_note;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* if note is out of range, ignore */
  if (TUNING_NOTE_IS_NOT_VALID(note))
    return 1;

  for (m = 0; m < BANK_OSCS_AND_ENVS_PER_VOICE; m++)
  {
    /* obtain envelope pointer */
    e = &G_envelope_bank[BANK_OSCS_AND_ENVS_PER_VOICE * voice_index + m];

    /* set the current note */
    if (note < 0)
      adjusted_note = 0;
    else if (note > TUNING_NUM_VALID_NOTES - 1)
      adjusted_note = TUNING_NUM_VALID_NOTES - 1;
    else
      adjusted_note = note;

    /* apply note offset */
    adjusted_note += e->offset;

    /* compute rate & level adjustments based on note */

    /* note that adding 64 to the base level is   */
    /* the same as multiplying it by 1/2 (once    */
    /* converted back to linear instead of log).  */
    e->rate_adjustment = (12 * (adjusted_note - TUNING_NOTE_C0)) / e->ks_rate_fraction;
    e->level_adjustment = (64 * (adjusted_note - e->ks_break_note)) / e->ks_level_fraction;
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

  for (m = 0; m < BANK_OSCS_AND_ENVS_PER_VOICE; m++)
  {
    /* obtain envelope pointer */
    e = &G_envelope_bank[BANK_OSCS_AND_ENVS_PER_VOICE * voice_index + m];

    /* set level */
    e->level = e->attenuation + e->ampl_adjustment + e->level_adjustment;

    /* bound level */
    if (e->level < 0)
      e->level = 0;
    else if (e->level > 1023)
      e->level = 1023;

    /* set the envelope to its initial state */
    ENVELOPE_SET_STATE(ATTACK)

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

  for (m = 0; m < BANK_OSCS_AND_ENVS_PER_VOICE; m++)
  {
    /* obtain envelope pointer */
    e = &G_envelope_bank[BANK_OSCS_AND_ENVS_PER_VOICE * voice_index + m];

    /* if this envelope is already released, continue */
    if (e->state == ENVELOPE_STATE_RELEASE)
      continue;

    /* set envelope to the release state */
    ENVELOPE_SET_STATE(RELEASE)
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
    if (periods > 0)
    {
      /* rising states */
      if (e->state == ENVELOPE_STATE_ATTACK)
      {
        e->attenuation += (~e->attenuation * periods) >> 4;
      }
      /* falling states */
      else if ( (e->state == ENVELOPE_STATE_DECAY_1)  || 
                (e->state == ENVELOPE_STATE_DECAY_2)  || 
                (e->state == ENVELOPE_STATE_RELEASE)  || 
                (e->state == ENVELOPE_STATE_SUSTAINED))
      {
        e->attenuation += periods;
      }
      else
        e->attenuation += periods;

      /* bound attenuation */
      if (e->attenuation < 0)
        e->attenuation = 0;
      else if (e->attenuation > 1023)
        e->attenuation = 1023;

      /* change state if necessary */
      if ((e->state == ENVELOPE_STATE_ATTACK) && 
          (e->attenuation == 0))
      {
        ENVELOPE_SET_STATE(DECAY_1)
      }
      else if ( (e->state == ENVELOPE_STATE_DECAY_1) && 
                (e->attenuation >= e->transition_level))
      {
        if (e->sustain_pedal == MIDI_CONT_PEDAL_STATE_DOWN)
        {
          ENVELOPE_SET_STATE(SUSTAINED)
        }
        else
        {
          ENVELOPE_SET_STATE(DECAY_2)
        }
      }
      else if ( (e->state == ENVELOPE_STATE_DECAY_2) || 
                (e->state == ENVELOPE_STATE_SUSTAINED))
      {
        if (e->attenuation == 1023)
        {
          ENVELOPE_SET_STATE(RELEASE)
        }
      }

      /* update level */
      e->level = e->attenuation + e->ampl_adjustment + e->level_adjustment;

      /* bound level */
      if (e->level < 0)
        e->level = 0;
      else if (e->level > 1023)
        e->level = 1023;
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

  /* amplitude table */
  S_envelope_amplitude_table[0] = 1023;

  for (m = 1; m < PATCH_ENV_AMPLITUDE_NUM_VALUES; m++)
  {
    S_envelope_amplitude_table[m] = 
      ENVELOPE_AMPLITUDE_STEP * (PATCH_ENV_AMPLITUDE_UPPER_BOUND - m);
  }

  /* sustain table */
  S_envelope_sustain_table[0] = 1023;

  for (m = 1; m < PATCH_ENV_SUSTAIN_NUM_VALUES; m++)
  {
    S_envelope_sustain_table[m] = 
      ENVELOPE_SUSTAIN_STEP * (PATCH_ENV_SUSTAIN_UPPER_BOUND - m);
  }

  /* phase increment table  */
  for (m = 0; m < ENVELOPE_TABLE_RATES_PER_ROW; m++)
  {
    S_envelope_phase_increment_table[ 0 * ENVELOPE_TABLE_RATES_PER_ROW + m] = 
      (int) (((S_envelope_freq_table[m] / 4) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_envelope_phase_increment_table[ 1 * ENVELOPE_TABLE_RATES_PER_ROW + m] = 
      (int) (((S_envelope_freq_table[m] / 2) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_envelope_phase_increment_table[ 2 * ENVELOPE_TABLE_RATES_PER_ROW + m] = 
      (int) ((S_envelope_freq_table[m] * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_envelope_phase_increment_table[ 3 * ENVELOPE_TABLE_RATES_PER_ROW + m] = 
      (int) (((S_envelope_freq_table[m] * 2) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_envelope_phase_increment_table[ 4 * ENVELOPE_TABLE_RATES_PER_ROW + m] = 
      (int) (((S_envelope_freq_table[m] * 4) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_envelope_phase_increment_table[ 5 * ENVELOPE_TABLE_RATES_PER_ROW + m] = 
      (int) (((S_envelope_freq_table[m] * 8) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_envelope_phase_increment_table[ 6 * ENVELOPE_TABLE_RATES_PER_ROW + m] = 
      (int) (((S_envelope_freq_table[m] * 16) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_envelope_phase_increment_table[ 7 * ENVELOPE_TABLE_RATES_PER_ROW + m] = 
      (int) (((S_envelope_freq_table[m] * 32) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_envelope_phase_increment_table[ 8 * ENVELOPE_TABLE_RATES_PER_ROW + m] = 
      (int) (((S_envelope_freq_table[m] * 64) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_envelope_phase_increment_table[ 9 * ENVELOPE_TABLE_RATES_PER_ROW + m] = 
      (int) (((S_envelope_freq_table[m] * 128) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_envelope_phase_increment_table[10 * ENVELOPE_TABLE_RATES_PER_ROW + m] = 
      (int) (((S_envelope_freq_table[m] * 256) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_envelope_phase_increment_table[11 * ENVELOPE_TABLE_RATES_PER_ROW + m] = 
      (int) (((S_envelope_freq_table[m] * 512) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_envelope_phase_increment_table[12 * ENVELOPE_TABLE_RATES_PER_ROW + m] = 
      (int) (((S_envelope_freq_table[m] * 1024) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_envelope_phase_increment_table[13 * ENVELOPE_TABLE_RATES_PER_ROW + m] = 
      (int) (((S_envelope_freq_table[m] * 2048) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_envelope_phase_increment_table[14 * ENVELOPE_TABLE_RATES_PER_ROW + m] = 
      (int) (((S_envelope_freq_table[m] * 4096) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_envelope_phase_increment_table[15 * ENVELOPE_TABLE_RATES_PER_ROW + m] = 
      (int) (((S_envelope_freq_table[m] * 8192) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
  }

#if 0
  printf("Envelope Phase Increment Table:\n");

  for (m = 0; m < ENVELOPE_TABLE_SIZE; m++)
    printf("Envelope Rate %d Phase Inc: %d\n", m, S_envelope_phase_increment_table[m]);
#endif

#if 0
  short int attenuation;
  short int count;

  attenuation = 1023;
  count = 0;

  printf("Attenuation: %d (%d)\n", attenuation, count);

  while (attenuation > 0)
  {
    attenuation += (~attenuation * 1) >> 4;
    count += 1;

    printf("Attenuation: %d (%d)\n", attenuation, count);
  }
#endif

  return 0;
}

