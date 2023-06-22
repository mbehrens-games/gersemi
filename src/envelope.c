/*******************************************************************************
** envelope.c (envelope)
*******************************************************************************/

#include <stdlib.h>

#include "bank.h"
#include "clock.h"
#include "envelope.h"
#include "patch.h"
#include "voice.h"

#define ENVELOPE_TABLE_NUM_ROWS       16
#define ENVELOPE_TABLE_RATES_PER_ROW  12

#define ENVELOPE_TABLE_SIZE (ENVELOPE_TABLE_NUM_ROWS * ENVELOPE_TABLE_RATES_PER_ROW)

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

/* carrier envelope level table */
static short int S_envelope_carrier_level_table[17] = 
  { 213,  /* 10^(-16/16)  */
    200,  /* 10^(-15/16)  */
    187,  /* 10^(-14/16)  */
    173,  /* 10^(-13/16)  */
    160,  /* 10^(-12/16)  */
    147,  /* 10^(-11/16)  */
    133,  /* 10^(-10/16)  */
    120,  /*  10^(-9/16)  */
    107,  /*  10^(-8/16)  */
     93,  /*  10^(-7/16)  */
     80,  /*  10^(-6/16)  */
     67,  /*  10^(-5/16)  */
     53,  /*  10^(-4/16)  */
     40,  /*  10^(-3/16)  */
     27,  /*  10^(-2/16)  */
     13,  /*  10^(-1/16)  */
      0   /*   1          */
  };

/* modulator envelope level table */
static short int S_envelope_modulator_level_table[17] = 
  { 108,  /*  8/24  */
     91,  /*  9/24  */
     81,  /* 10/24  */
     72,  /* 11/24  */
     64,  /* 12/24  */
     57,  /* 13/24  */
     50,  /* 14/24  */
     44,  /* 15/24  */
     38,  /* 16/24  */
     32,  /* 17/24  */
     27,  /* 18/24  */
     22,  /* 19/24  */
     17,  /* 20/24  */
     12,  /* 21/24  */
      8,  /* 22/24  */
      4,  /* 23/24  */
      0   /* 24/24  */
  };

/* volume table */
static short int S_envelope_volume_table[17] = 
  {1023,  /*  0/16  */
    257,  /*  1/16  */
    193,  /*  2/16  */
    155,  /*  3/16  */
    128,  /*  4/16  */
    108,  /*  5/16  */
     91,  /*  6/16  */
     76,  /*  7/16  */
     64,  /*  8/16  */
     53,  /*  9/16  */
     44,  /* 10/16  */
     35,  /* 11/16  */
     27,  /* 12/16  */
     19,  /* 13/16  */
     12,  /* 14/16  */
      6,  /* 15/16  */
      0   /* 16/16  */
  };

/* brightness table */
static short int S_envelope_brightness_table[17] = 
  {  64,  /*  8/16  */
     53,  /*  9/16  */
     44,  /* 10/16  */
     35,  /* 11/16  */
     27,  /* 12/16  */
     19,  /* 13/16  */
     12,  /* 14/16  */
      6,  /* 15/16  */
      0,  /* 16/16  */
     -6,  /* 17/16  */
    -11,  /* 18/16  */
    -16,  /* 19/16  */
    -21,  /* 20/16  */
    -25,  /* 21/16  */
    -30,  /* 22/16  */
    -34,  /* 23/16  */
    -38   /* 24/16  */
  };

/* sustain table */
static short int S_envelope_sustain_table[17] = 
  {1023,  /*  0/16  */
    257,  /*  1/16  */
    193,  /*  2/16  */
    155,  /*  3/16  */
    128,  /*  4/16  */
    108,  /*  5/16  */
     91,  /*  6/16  */
     76,  /*  7/16  */
     64,  /*  8/16  */
     53,  /*  9/16  */
     44,  /* 10/16  */
     35,  /* 11/16  */
     27,  /* 12/16  */
     19,  /* 13/16  */
     12,  /* 14/16  */
      6,  /* 15/16  */
      0   /* 16/16  */
  };

/* envelope bank */
envelope G_envelope_bank[BANK_NUM_ENVELOPES];

/*******************************************************************************
** envelope_setup_all()
*******************************************************************************/
short int envelope_setup_all()
{
  int k;
  int m;

  /* setup all envelopes */
  for (k = 0; k < BANK_NUM_VOICES; k++)
  {
    for (m = 0; m < VOICE_NUM_OSCS_AND_ENVS; m++)
      envelope_reset(k, m);
  }

  return 0;
}

/*******************************************************************************
** envelope_reset()
*******************************************************************************/
short int envelope_reset(int voice_index, int num)
{
  envelope* e;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* make sure the envelope number is valid */
  if ((num < 0) || (num >= VOICE_NUM_OSCS_AND_ENVS))
    return 1;

  /* obtain envelope pointer */
  e = &G_envelope_bank[4 * voice_index + num];

  /* initialize envelope variables */
  e->type = ENVELOPE_TYPE_CARRIER;

  e->attack_row = 0;
  e->decay_1_row = 0;
  e->decay_2_row = 0;
  e->release_row = 0;

  e->rate_keyscaling = 0;
  e->level_keyscaling = 0;

  e->state = ENVELOPE_STATE_RELEASE;

  e->keycode = 0;

  e->row = 0;

  e->increment = 0;
  e->phase = 0;

  e->sustain_level = S_envelope_sustain_table[0];

  e->attenuation = 1023;

  e->volume_adjustment = 0;
  e->amp_adjustment = S_envelope_carrier_level_table[16];

  e->level = 1023;

  return 0;
}

/*******************************************************************************
** envelope_change_state()
*******************************************************************************/
short int envelope_change_state(int voice_index, int num, int state)
{
  short int rate_adjustment;

  envelope* e;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* make sure the envelope number is valid */
  if ((num < 0) || (num >= VOICE_NUM_OSCS_AND_ENVS))
    return 1;

  /* obtain envelope pointer */
  e = &G_envelope_bank[4 * voice_index + num];

  /* change the envelope to the new state, and set  */
  /* the row index based on the appropriate rate    */
  if (state == ENVELOPE_STATE_ATTACK)
  {
    e->state = ENVELOPE_STATE_ATTACK;
    e->row = e->attack_row;
  }
  else if (state == ENVELOPE_STATE_DECAY_1)
  {
    e->state = ENVELOPE_STATE_DECAY_1;
    e->row = e->decay_1_row;
  }
  else if (state == ENVELOPE_STATE_DECAY_2)
  {
    e->state = ENVELOPE_STATE_DECAY_2;
    e->row = e->decay_2_row;
  }
  else if (state == ENVELOPE_STATE_RELEASE)
  {
    e->state = ENVELOPE_STATE_RELEASE;
    e->row = e->release_row;
  }
  else
    return 0;

  /* apply rate keyscaling */

  /* description of the settings:         */
  /*   1: the rate is multiplied by 2     */
  /*      for every increase by 8 octaves */
  /*   2: the rate is multiplied by 2     */
  /*      for every increase by 4 octaves */
  /*   4: the rate is multiplied by 2     */
  /*      for every increase by 2 octaves */
  /*   8: the rate is multiplied by 2     */
  /*      for every increase by 1 octave  */
  if ((e->rate_keyscaling >= 1) && (e->rate_keyscaling <= 8))
    rate_adjustment = (e->rate_keyscaling * e->keycode) / 8;
  else
    rate_adjustment = e->keycode / 8;

  e->row += rate_adjustment;

  /* bound row index */
  if (e->row < 0)
    e->row = 0;
  else if (e->row >= ENVELOPE_TABLE_SIZE)
    e->row = ENVELOPE_TABLE_SIZE - 1;

  /* set the phase increment */
  e->increment = S_envelope_phase_increment_table[e->row];

  return 0;
}

/*******************************************************************************
** envelope_load_patch()
*******************************************************************************/
short int envelope_load_patch(int voice_index, int num, 
                              int patch_index, int type)
{
  envelope* e;
  patch* p;

  short int attack;
  short int decay_1;
  short int decay_2;
  short int release;
  short int sustain;
  short int rate_keyscaling;
  short int level_keyscaling;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* make sure the envelope number is valid */
  if ((num < 0) || (num >= VOICE_NUM_OSCS_AND_ENVS))
    return 1;

  /* obtain envelope pointer */
  e = &G_envelope_bank[4 * voice_index + num];

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain patch pointer */
  p = &G_patch_bank[patch_index];

  /* set type */
  if ((type == ENVELOPE_TYPE_CARRIER) || 
      (type == ENVELOPE_TYPE_MODULATOR))
  {
    e->type = type;
  }
  else
    e->type = ENVELOPE_TYPE_CARRIER;

  /* determine which set of parameters to use */
  if (e->type == ENVELOPE_TYPE_CARRIER)
  {
    attack = p->car_attack;
    decay_1 = p->car_decay_1;
    decay_2 = p->car_decay_2;
    release = p->car_release;
    sustain = p->car_sustain;
    rate_keyscaling = p->car_rate_keyscaling;
    level_keyscaling = p->car_level_keyscaling;

    if ((p->osc_amplitude[num] >= 0) && (p->osc_amplitude[num] <= 16))
      e->amp_adjustment = S_envelope_carrier_level_table[p->osc_amplitude[num]];
    else
      e->amp_adjustment = S_envelope_carrier_level_table[16];
  }
  else if (e->type == ENVELOPE_TYPE_MODULATOR)
  {
    attack = p->mod_attack;
    decay_1 = p->mod_decay_1;
    decay_2 = p->mod_decay_2;
    release = p->mod_release;
    sustain = p->mod_sustain;
    rate_keyscaling = p->mod_rate_keyscaling;
    level_keyscaling = p->mod_level_keyscaling;

    if ((p->osc_amplitude[num] >= 0) && (p->osc_amplitude[num] <= 16))
      e->amp_adjustment = S_envelope_modulator_level_table[p->osc_amplitude[num]];
    else
      e->amp_adjustment = S_envelope_modulator_level_table[16];
  }
  else
  {
    attack = p->car_attack;
    decay_1 = p->car_decay_1;
    decay_2 = p->car_decay_2;
    release = p->car_release;
    sustain = p->car_sustain;
    rate_keyscaling = p->car_rate_keyscaling;
    level_keyscaling = p->car_level_keyscaling;

    if ((p->osc_amplitude[num] >= 0) && (p->osc_amplitude[num] <= 16))
      e->amp_adjustment = S_envelope_carrier_level_table[p->osc_amplitude[num]];
    else
      e->amp_adjustment = S_envelope_carrier_level_table[16];
  }

  /* set attack rate */
  if ((attack >= 1) && (attack <= 32))
  {
    if ((attack % 2) == 1)
      e->attack_row = 12 * ((attack - 1) / 2);
    else
      e->attack_row = 12 * ((attack - 1) / 2) + 7;
  }
  else
    e->attack_row = 0;

  /* set decay 1 rate */
  if ((decay_1 >= 1) && (decay_1 <= 32))
  {
    if ((decay_1 % 2) == 1)
      e->decay_1_row = 12 * ((decay_1 - 1) / 2);
    else
      e->decay_1_row = 12 * ((decay_1 - 1) / 2) + 7;
  }
  else
    e->decay_1_row = 0;

  /* set decay 2 rate */
  if ((decay_2 >= 1) && (decay_2 <= 32))
  {
    if ((decay_2 % 2) == 1)
      e->decay_2_row = 12 * ((decay_2 - 1) / 2);
    else
      e->decay_2_row = 12 * ((decay_2 - 1) / 2) + 7;
  }
  else
    e->decay_2_row = 0;

  /* set release rate */
  if ((release >= 1) && (release <= 32))
  {
    if ((release % 2) == 1)
      e->release_row = 12 * ((release - 1) / 2);
    else
      e->release_row = 12 * ((release - 1) / 2) + 7;
  }
  else
    e->release_row = 0;

  /* set sustain level */
  if ((sustain >= 0) && (sustain <= 16))
    e->sustain_level = S_envelope_sustain_table[sustain];
  else
    e->sustain_level = S_envelope_sustain_table[0];

  /* set keyscaling */
  if ((rate_keyscaling >= 1) && (rate_keyscaling <= 8))
    e->rate_keyscaling = rate_keyscaling;
  else
    e->rate_keyscaling = 1;

  if ((level_keyscaling >= 1) && (level_keyscaling <= 8))
    e->level_keyscaling = level_keyscaling;
  else
    e->level_keyscaling = 1;

  /* initialize other envelope variables */
#if 0
  e->state = ENVELOPE_STATE_RELEASE;

  e->keycode = 0;

  e->row = 0;

  e->increment = 0;
  e->phase = 0;

  e->attenuation = 1023;

  e->volume_adjustment = 0;

  e->level = 1023;
#endif

  return 0;
}

/*******************************************************************************
** envelope_trigger()
*******************************************************************************/
short int envelope_trigger(int voice_index, int num, int note, int volume, int brightness)
{
  short int level_adjustment;

  envelope* e;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* make sure the envelope number is valid */
  if ((num < 0) || (num >= VOICE_NUM_OSCS_AND_ENVS))
    return 1;

  /* obtain envelope pointer */
  e = &G_envelope_bank[4 * voice_index + num];

  /* make sure the note is valid */
  if ((note < 0) || (note >= 10 * 12))
    return 0;

  /* the keycode is based on the current  */
  /* note (bound to the range C0 to C8)   */
  e->keycode = note;

  if (e->keycode < 0)
    e->keycode = 0;
  else if (e->keycode > 96)
    e->keycode = 96;

  /* set volume adjustment */
  if (e->type == ENVELOPE_TYPE_CARRIER)
  {
    if ((volume >= 0) && (volume <= 16))
      e->volume_adjustment = S_envelope_volume_table[volume];
    else
      e->volume_adjustment = S_envelope_volume_table[8];
  }
  else if (e->type == ENVELOPE_TYPE_MODULATOR)
  {
    if ((brightness >= 0) && (brightness <= 16))
      e->volume_adjustment = S_envelope_brightness_table[brightness];
    else
      e->volume_adjustment = S_envelope_brightness_table[8];
  }
  else
  {
    if ((volume >= 0) && (volume <= 16))
      e->volume_adjustment = S_envelope_volume_table[volume];
    else
      e->volume_adjustment = S_envelope_volume_table[8];
  }

  /* determine level adjustment from keyscaling */

  /* description of the settings:         */
  /*   1: the level is multiplied by 1/2  */
  /*      for every increase by 4 octaves */
  /*   2: the level is multiplied by 1/2  */
  /*      for every increase by 2 octaves */
  /*   4: the level is multiplied by 1/2  */
  /*      for every increase by 1 octave  */
  /*   8: the level is multiplied by 1/4  */
  /*      for every increase by 1 octave  */

  /* note that adding 64 to the base level is   */
  /* the same as multiplying it by 1/2 (once    */
  /* converted back to linear instead of log).  */

  /* additional tweak to the level scaling:     */
  /* the adjustment is shifted so that 0 occurs */
  /* at G2 (which is at keycode 31)             */
  if ((e->level_keyscaling >= 1) && (e->level_keyscaling <= 8))
    level_adjustment = (16 * e->level_keyscaling * (e->keycode - 31)) / 12;
  else
    level_adjustment = (16 * (e->keycode - 31)) / 12;

  /* apply level keyscaling */
  e->volume_adjustment += level_adjustment;

  /* set level */
  e->level = e->attenuation + e->volume_adjustment + e->amp_adjustment;

  /* bound level */
  if (e->level < 0)
    e->level = 0;
  else if (e->level > 1023)
    e->level = 1023;

  /* set the envelope to attack state */
  envelope_change_state(voice_index, num, ENVELOPE_STATE_ATTACK);

  /* reset phase */
  e->phase = 0;

  return 0;
}

/*******************************************************************************
** envelope_release()
*******************************************************************************/
short int envelope_release(int voice_index, int num)
{
  envelope* e;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* make sure the envelope number is valid */
  if ((num < 0) || (num >= VOICE_NUM_OSCS_AND_ENVS))
    return 1;

  /* obtain envelope pointer */
  e = &G_envelope_bank[4 * voice_index + num];

  /* if this envelope is already released, return */
  if (e->state == ENVELOPE_STATE_RELEASE)
    return 0;

  /* set the envelope to release state */
  envelope_change_state(voice_index, num, ENVELOPE_STATE_RELEASE);

  return 0;
}

/*******************************************************************************
** envelope_update_all()
*******************************************************************************/
short int envelope_update_all()
{
  short int periods;

  int k;
  int m;

  envelope* e;

  /* update all envelopes */
  for (k = 0; k < BANK_NUM_VOICES; k++)
  {
    for (m = 0; m < VOICE_NUM_OSCS_AND_ENVS; m++)
    {
      /* obtain envelope pointer */
      e = &G_envelope_bank[4 * k + m];

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
        /* attack */
        if (e->state == ENVELOPE_STATE_ATTACK)
        {
          e->attenuation += (~e->attenuation * periods) >> 4;

          if (e->attenuation <= 0)
          {
            e->attenuation = 0;
            envelope_change_state(k, m, ENVELOPE_STATE_DECAY_1);
          }
        }
        /* decay 1 */
        else if (e->state == ENVELOPE_STATE_DECAY_1)
        {
          e->attenuation += periods;

          if (e->attenuation >= e->sustain_level)
            envelope_change_state(k, m, ENVELOPE_STATE_DECAY_2);
        }
        /* decay 2 */
        else if (e->state == ENVELOPE_STATE_DECAY_2)
        {
          e->attenuation += periods;

          if (e->attenuation >= 1023)
          {
            e->attenuation = 1023;
            envelope_change_state(k, m, ENVELOPE_STATE_RELEASE);
          }
        }
        /* release */
        else if (e->state == ENVELOPE_STATE_RELEASE)
        {
          e->attenuation += periods;

          if (e->attenuation >= 1023)
            e->attenuation = 1023;
        }

        /* update level */
        e->level = e->attenuation + e->volume_adjustment + e->amp_adjustment;

        if (e->level >= 1023)
          e->level = 1023;
      }
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

  return 0;
}

