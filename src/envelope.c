/*******************************************************************************
** envelope.c (envelope)
*******************************************************************************/

#include <stdlib.h>

#include "bank.h"
#include "clock.h"
#include "envelope.h"
#include "midicont.h"
#include "patch.h"
#include "tuning.h"

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
  else if (e->state == ENVELOPE_STATE_ALTERNATE_DECAY_2)                       \
    e->row = e->alt_d2_row;                                                    \
  else if (e->state == ENVELOPE_STATE_ALTERNATE_RELEASE)                       \
    e->row = e->alt_r_row;                                                     \
  else                                                                         \
    e->row = ENVELOPE_MAX_RATE_ROW;                                            \
                                                                               \
  /* apply rate keyscaling */                                                  \
  e->row += e->rate_adjustment;                                                \
                                                                               \
  /* bound row index */                                                        \
  if (e->row < 0)                                                              \
    e->row = 0;                                                                \
  else if (e->row >= ENVELOPE_TABLE_SIZE)                                      \
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

/* for the following tables, the values are found using the formula:  */
/*   (10 * (log(1 / val) / log(10)) / DB_STEP_10_BIT,                 */
/*   where DB_STEP_10_BIT = 0.046875                                  */

/* amplitude table */
static short int S_envelope_amplitude_table[PATCH_ENV_AMPLITUDE_NUM_VALUES] = 
  { 1023,     /*  0             */
    12 * 31,  /* 10^(-277/160)  */
    12 * 30,  /* ...            */
    12 * 29,  /* ...            */
    12 * 28,  /* ...            */
    12 * 27,  /* ...            */
    12 * 26,  /* ...            */
    12 * 25,  /* ...            */
    12 * 24,  /* ...            */
    12 * 23,  /* ...            */
    12 * 22,  /* ...            */
    12 * 21,  /* ...            */
    12 * 20,  /* ...            */
    12 * 19,  /* ...            */
    12 * 18,  /* ...            */
    12 * 17,  /* ...            */
    12 * 16,  /* 10^(-144/160)  */
    12 * 15,  /* ...            */
    12 * 14,  /* ...            */
    12 * 13,  /* ...            */
    12 * 12,  /* ...            */
    12 * 11,  /* ...            */
    12 * 10,  /* ...            */
    12 * 9,   /* ...            */
    12 * 8,   /* ...            */
    12 * 7,   /* ...            */
    12 * 6,   /* ...            */
    12 * 5,   /* ...            */
    12 * 4,   /* ...            */
    12 * 3,   /* ...            */
    12 * 2,   /* ...            */
    12 * 1,   /* 10^(-9/160)    */
         0    /*  1             */
  };

/* sustain table */
static short int S_envelope_sustain_table[PATCH_ENV_SUSTAIN_NUM_VALUES] = 
  { 1023,     /*  0           */
    30 * 15,  /* 10^(-135/64) */
    30 * 14,  /* ...          */
    30 * 13,  /* ...          */
    30 * 12,  /* ...          */
    30 * 11,  /* ...          */
    30 * 10,  /* ...          */
    30 * 9,   /* ...          */
    30 * 8,   /* 10^(-72/64)  */
    30 * 7,   /* ...          */
    30 * 6,   /* ...          */
    30 * 5,   /* ...          */
    30 * 4,   /* ...          */
    30 * 3,   /* ...          */
    30 * 2,   /* ...          */
    30 * 1,   /* 10^(-9/64)   */
         0    /*  1           */
  };

/* keyscaling table */
static short int  S_envelope_keyscale_table[PATCH_ENV_KEYSCALE_NUM_VALUES] = 
                  { 1, 2, 4, 8 };

/* sustain pedal shift table */
static short int  S_envelope_pedal_shift_table[PATCH_PEDAL_SHIFT_NUM_VALUES] = 
                  { -8, -7, -6, -5, -4, -3, -2, -1, 
                     1,  2,  3,  4,  5,  6,  7,  8 };

/* envelope bank */
envelope G_envelope_bank[BANK_NUM_ENVELOPES];

/*******************************************************************************
** envelope_setup_all()
*******************************************************************************/
short int envelope_setup_all()
{
  int k;

  /* setup all envelopes */
  for (k = 0; k < BANK_NUM_VOICES; k++)
    envelope_reset(k);

  return 0;
}

/*******************************************************************************
** envelope_reset()
*******************************************************************************/
short int envelope_reset(int voice_index)
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

    /* initialize envelope variables */
    e->rate_ks = 1;
    e->level_ks = 1;

    e->ampl_adjustment = S_envelope_amplitude_table[0];
    e->rate_adjustment = 0;
    e->level_adjustment = 0;

    e->transition_level = S_envelope_sustain_table[0];

    e->a_row = 0;
    e->d1_row = 0;
    e->d2_row = 0;
    e->r_row = 0;

    e->alt_d2_row = 0;
    e->alt_r_row = 0;

    e->state = ENVELOPE_STATE_RELEASE;
    e->row = 0;

    e->increment = 0;
    e->phase = 0;

    e->note_input = 0;

    e->pedal_input = 0;

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
      e->a_row = 12 * ((p->env_attack[m] - PATCH_ENV_RATE_LOWER_BOUND) / 2);

      if (((p->env_attack[m] - PATCH_ENV_RATE_LOWER_BOUND) % 2) == 1)
        e->a_row += 7;
    }
    else
      e->a_row = 0;

    /* decay 1 rate */
    if ((p->env_decay_1[m] >= PATCH_ENV_RATE_LOWER_BOUND) && 
        (p->env_decay_1[m] <= PATCH_ENV_RATE_UPPER_BOUND))
    {
      e->d1_row = 12 * ((p->env_decay_1[m] - PATCH_ENV_RATE_LOWER_BOUND) / 2);

      if (((p->env_decay_1[m] - PATCH_ENV_RATE_LOWER_BOUND) % 2) == 1)
        e->d1_row += 7;
    }
    else
      e->d1_row = 0;

    /* decay 2 rate */
    if ((p->env_decay_2[m] >= PATCH_ENV_RATE_LOWER_BOUND) && 
        (p->env_decay_2[m] <= PATCH_ENV_RATE_UPPER_BOUND))
    {
      e->d2_row = 12 * ((p->env_decay_2[m] - PATCH_ENV_RATE_LOWER_BOUND) / 2);

      if (((p->env_decay_2[m] - PATCH_ENV_RATE_LOWER_BOUND) % 2) == 1)
        e->d2_row += 7;
    }
    else
      e->d2_row = 0;

    /* release rate */
    if ((p->env_release[m] >= PATCH_ENV_RATE_LOWER_BOUND) && 
        (p->env_release[m] <= PATCH_ENV_RATE_UPPER_BOUND))
    {
      e->r_row = 12 * ((p->env_release[m] - PATCH_ENV_RATE_LOWER_BOUND) / 2);

      if (((p->env_release[m] - PATCH_ENV_RATE_LOWER_BOUND) % 2) == 1)
        e->r_row += 7;
    }
    else
      e->r_row = 0;

    /* sustain pedal shift */
    if ((p->pedal_shift >= PATCH_PEDAL_SHIFT_LOWER_BOUND) && 
        (p->pedal_shift <= PATCH_PEDAL_SHIFT_UPPER_BOUND))
    {
      /* set alternate decay 2 rate */
      shifted_rate = p->env_decay_2[m] - PATCH_ENV_RATE_LOWER_BOUND;
      shifted_rate += S_envelope_pedal_shift_table[p->pedal_shift - PATCH_PEDAL_SHIFT_LOWER_BOUND];

      if (shifted_rate < PATCH_ENV_RATE_LOWER_BOUND)
        shifted_rate = PATCH_ENV_RATE_LOWER_BOUND;
      else if (shifted_rate > PATCH_ENV_RATE_UPPER_BOUND)
        shifted_rate = PATCH_ENV_RATE_UPPER_BOUND;

      e->alt_d2_row = 12 * (shifted_rate / 2);

      if ((shifted_rate % 2) == 1)
        e->alt_d2_row += 7;

      /* set alternate release rate */
      shifted_rate = p->env_release[m] - PATCH_ENV_RATE_LOWER_BOUND;
      shifted_rate += S_envelope_pedal_shift_table[p->pedal_shift - PATCH_PEDAL_SHIFT_LOWER_BOUND];

      if (shifted_rate < PATCH_ENV_RATE_LOWER_BOUND)
        shifted_rate = PATCH_ENV_RATE_LOWER_BOUND;
      else if (shifted_rate > PATCH_ENV_RATE_UPPER_BOUND)
        shifted_rate = PATCH_ENV_RATE_UPPER_BOUND;

      e->alt_r_row = 12 * (shifted_rate / 2);

      if ((shifted_rate % 2) == 1)
        e->alt_r_row += 7;
    }
    else
    {
      e->alt_d2_row = e->d2_row;
      e->alt_r_row = e->r_row;
    }

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

    /* rate keyscaling */

    /* description of the settings:         */
    /*   1: the rate is multiplied by 2     */
    /*      for every increase by 8 octaves */
    /*   2: the rate is multiplied by 2     */
    /*      for every increase by 4 octaves */
    /*   4: the rate is multiplied by 2     */
    /*      for every increase by 2 octaves */
    /*   8: the rate is multiplied by 2     */
    /*      for every increase by 1 octave  */
    if ((p->env_rate_ks[m] >= PATCH_ENV_KEYSCALE_LOWER_BOUND) && 
        (p->env_rate_ks[m] <= PATCH_ENV_KEYSCALE_UPPER_BOUND))
    {
      e->rate_ks = S_envelope_keyscale_table[p->env_rate_ks[m] - PATCH_ENV_KEYSCALE_LOWER_BOUND];
    }
    else
      e->rate_ks = S_envelope_keyscale_table[0];

    /* level keyscaling */

    /* description of the settings:         */
    /*   1: the level is multiplied by 1/2  */
    /*      for every increase by 4 octaves */
    /*   2: the level is multiplied by 1/2  */
    /*      for every increase by 2 octaves */
    /*   4: the level is multiplied by 1/2  */
    /*      for every increase by 1 octave  */
    /*   8: the level is multiplied by 1/4  */
    /*      for every increase by 1 octave  */
    if ((p->env_level_ks[m] >= PATCH_ENV_KEYSCALE_LOWER_BOUND) && 
        (p->env_level_ks[m] <= PATCH_ENV_KEYSCALE_UPPER_BOUND))
    {
      e->level_ks = S_envelope_keyscale_table[p->env_level_ks[m] - PATCH_ENV_KEYSCALE_LOWER_BOUND];
    }
    else
      e->level_ks = S_envelope_keyscale_table[0];
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

  int keycode;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  for (m = 0; m < BANK_OSCS_AND_ENVS_PER_VOICE; m++)
  {
    /* obtain envelope pointer */
    e = &G_envelope_bank[BANK_OSCS_AND_ENVS_PER_VOICE * voice_index + m];

    /* the keycode is based on the current  */
    /* note (bound to the range C0 to C8)   */
    keycode = e->note_input;

    if (keycode < TUNING_NOTE_C0)
      keycode = TUNING_NOTE_C0;
    else if (keycode > TUNING_NOTE_C8)
      keycode = TUNING_NOTE_C8;

    keycode -= TUNING_NOTE_C0;

    /* compute rate & level adjustments based on keycode */

    /* note that adding 64 to the base level is   */
    /* the same as multiplying it by 1/2 (once    */
    /* converted back to linear instead of log).  */

    /* additional tweak to the level scaling:     */
    /* the adjustment is shifted so that 0 occurs */
    /* at G2 (which is at keycode 31)             */
    e->rate_adjustment = (e->rate_ks * keycode) / 8;
    e->level_adjustment = (16 * e->level_ks * (keycode - 31)) / 12;

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

    /* set the envelope to release state */
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
  int m;

  envelope* e;

  short int periods;

  /* update all envelopes */
  for (k = 0; k < BANK_NUM_VOICES; k++)
  {
    for (m = 0; m < BANK_OSCS_AND_ENVS_PER_VOICE; m++)
    {
      /* obtain envelope pointer */
      e = &G_envelope_bank[BANK_OSCS_AND_ENVS_PER_VOICE * k + m];

      /* check if the sustain pedal should change the envelope state */
      if (e->pedal_input == 1)
      {
        if (e->state == ENVELOPE_STATE_DECAY_2)
        {
          ENVELOPE_SET_STATE(ALTERNATE_DECAY_2)
        }
        else if (e->state == ENVELOPE_STATE_RELEASE)
        {
          ENVELOPE_SET_STATE(ALTERNATE_RELEASE)
        }
      }
      else
      {
        if (e->state == ENVELOPE_STATE_ALTERNATE_DECAY_2)
        {
          ENVELOPE_SET_STATE(DECAY_2)
        }
        else if (e->state == ENVELOPE_STATE_ALTERNATE_RELEASE)
        {
          ENVELOPE_SET_STATE(RELEASE)
        }
      }

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
        else if ( (e->state == ENVELOPE_STATE_DECAY_1)            || 
                  (e->state == ENVELOPE_STATE_DECAY_2)            || 
                  (e->state == ENVELOPE_STATE_RELEASE)            || 
                  (e->state == ENVELOPE_STATE_ALTERNATE_DECAY_2)  || 
                  (e->state == ENVELOPE_STATE_ALTERNATE_RELEASE))
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
          if (e->pedal_input == 1)
          {
            ENVELOPE_SET_STATE(ALTERNATE_DECAY_2)
          }
          else
          {
            ENVELOPE_SET_STATE(DECAY_2)
          }
        }
        else if ( (e->state == ENVELOPE_STATE_DECAY_2) && 
                  (e->attenuation == 1023))
        {
          ENVELOPE_SET_STATE(RELEASE)
        }
        else if ( (e->state == ENVELOPE_STATE_ALTERNATE_DECAY_2) && 
                  (e->attenuation == 1023))
        {
          ENVELOPE_SET_STATE(ALTERNATE_RELEASE)
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
  printf("Phase Increment Table:\n");

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

