/*******************************************************************************
** linear.c (linear envelope)
*******************************************************************************/

#include <stdlib.h>

#include "clock.h"
#include "linear.h"

#define LINEAR_TABLE_NUM_ROWS       8
#define LINEAR_TABLE_RATES_PER_ROW  12

#define LINEAR_TABLE_SIZE (LINEAR_TABLE_NUM_ROWS * LINEAR_TABLE_RATES_PER_ROW)

/* linear envelope wavetables             */
/* note that the 129th entry is used for  */
/* the "constant" and "hold at end" modes */
static short int S_linear_pitch_wavetable[9][129];
static short int S_linear_pulse_width_wavetable[9][129];

/* linearillator depth tables */
static int S_linear_pitch_depth_table[9] = 
            { (0 * 12 +  2) * 64, /* 1 second             */
              (0 * 12 +  4) * 64, /* 1 third              */
              (0 * 12 +  7) * 64, /* 1 fifth              */
              (0 * 12 + 10) * 64, /* 1 seventh            */
              (1 * 12 +  0) * 64, /* 1 octave             */
              (1 * 12 +  2) * 64, /* 1 octave + 1 second  */
              (1 * 12 +  4) * 64, /* 1 octave + 1 third   */
              (1 * 12 +  7) * 64, /* 1 octave + 1 fifth   */
              (1 * 12 + 10) * 64  /* 1 octave + 1 seventh */
            };

static int S_linear_pulse_depth_table[9] = 
            { 64,   /* 7/16 pulse width */
              96, 
              128,  /* 3/8 pulse width  */
              192, 
              256,  /* 1/4 pulse width  */
              320, 
              384,  /* 1/8 pulse width  */
              416, 
              448   /* 1/16 pulse width */
            };

/* linearillator phase increment table */
static unsigned int S_linear_phase_increment_table[LINEAR_TABLE_SIZE];

/* linearillator base frequency table */
static float S_linear_freq_table[12] = 
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

#if 0
static float S_linear_freq_table[12] = 
              { 2.0f,               /* 2              */
                2.118926188718591f, /* 2 * 2^(1/12)   */
                2.244924096618746f, /* 2 * 2^(2/12)   */
                2.378414230005442f, /* 2 * 2^(3/12)   */
                2.519842099789746f, /* 2 * 2^(4/12)   */
                2.669679708340069f, /* 2 * 2^(5/12)   */
                2.82842712474619f,  /* 2 * 2^(6/12)   */
                2.996614153753363f, /* 2 * 2^(7/12)   */
                3.174802103936399f, /* 2 * 2^(8/12)   */
                3.363585661014858f, /* 2 * 2^(9/12)   */
                3.563594872561357f, /* 2 * 2^(10/12)  */
                3.775497250726774f  /* 2 * 2^(11/12)  */
              };
#endif

/*******************************************************************************
** linear_setup()
*******************************************************************************/
short int linear_setup(linear* l, int type, 
                                  int mode, 
                                  int rate, 
                                  int keyscaling)
{
  if (l == NULL)
    return 1;

  /* set type */
  if ((type >= 0) && (type <= LINEAR_NUM_TYPES))
    l->type = type;
  else
    l->type = LINEAR_TYPE_PITCH;

  /* set mode */
  if ((mode >= 0) && (mode <= LINEAR_NUM_MODES))
    l->mode = mode;
  else
    l->mode = LINEAR_MODE_CONSTANT;

  /* set rate */
  if ((rate >= 1) && (rate <= 32))
    l->rate = rate;
  else
    l->rate = 1;

  /* set keyscaling */
  if ((keyscaling >= 1) && (keyscaling <= 8))
    l->keyscaling = keyscaling;
  else
    l->keyscaling = 1;

  /* initialize wavetable row & index */
  l->row = 0;
  l->index = 0;

  /* initialize alternate mode flag */
  l->flag = 0;

  /* set phase increment */
  l->increment = 0;

  /* initialize phase & level */
  l->phase = 0;
  l->level = 0;

  return 0;
}

/*******************************************************************************
** linear_trigger()
*******************************************************************************/
short int linear_trigger(linear* l, int note, int brightness)
{
  int keycode;
  int entry;

  if (l == NULL)
    return 1;

  /* make sure the note is valid */
  if ((note < 0) || (note >= 10 * 12))
    return 0;

  /* the keycode is based on the current  */
  /* note (bound to the range C0 to C8)   */
  keycode = note;

  if (keycode < 0)
    keycode = 0;
  else if (keycode > 96)
    keycode = 96;

  /* divide the keycode by 2, as the linear envelope table has  */
  /* half the number of octaves as the amplitude envelope table */
  keycode = keycode / 2;

  /* determine entry in phase increment table based on the rate */
  entry = 12 * ((l->rate - 1) / 4);

  if (((l->rate - 1) % 4) == 1)
    entry += 4;
  else if (((l->rate - 1) % 4) == 2)
    entry += 7;
  else if (((l->rate - 1) % 4) == 3)
    entry += 10;

  /* apply keyscaling */
  if ((l->keyscaling >= 1) && (l->keyscaling <= 8))
    entry += (l->keyscaling * keycode) / 8;
  else
    entry += keycode / 8;

  /* bound entry */
  if (entry < 0)
    entry = 0;
  else if (entry >= LINEAR_TABLE_SIZE)
    entry = LINEAR_TABLE_SIZE - 1;

  /* determine phase increment */
  l->increment = S_linear_phase_increment_table[entry];

  /* use the brightness value to set the linear envelope depth */
  if ((brightness >= 1) && (brightness <= 9))
    l->row = brightness - 1;
  else
    l->row = 0;

  /* reset phase */
  l->phase = 0;

  /* initialize wavetable index */
  if ((l->mode == LINEAR_MODE_OUTWARD_HOLD_AT_END)    || 
      (l->mode == LINEAR_MODE_OUTWARD_HOLD_AT_START)  || 
      (l->mode == LINEAR_MODE_OUTWARD_REPEAT)         || 
      (l->mode == LINEAR_MODE_OUTWARD_FLIP))
  {
    l->index = 0;
  }
  else if ( (l->mode == LINEAR_MODE_CONSTANT)             || 
            (l->mode == LINEAR_MODE_INWARD_HOLD_AT_END)   || 
            (l->mode == LINEAR_MODE_INWARD_HOLD_AT_START) || 
            (l->mode == LINEAR_MODE_INWARD_REPEAT)        || 
            (l->mode == LINEAR_MODE_INWARD_FLIP))
  {
    l->index = 128;
  }
  else
    l->index = 0;

  /* initialize alternate mode flag */
  l->flag = 0;

  /* initialize level */
  if (l->type == LINEAR_TYPE_PITCH)
    l->level = S_linear_pitch_wavetable[l->row][l->index];
  else if (l->type == LINEAR_TYPE_PULSE_WIDTH)
    l->level = S_linear_pulse_width_wavetable[l->row][l->index];
  else
    l->level = 0;

  return 0;
}

/*******************************************************************************
** linear_update()
*******************************************************************************/
short int linear_update(linear* l)
{
  if (l == NULL)
    return 1;

  /* if the linear envelope is constant, return */
  if (l->mode == LINEAR_MODE_CONSTANT)
    return 0;

  /* update phase */
  l->phase += l->increment;

  /* note that the phase register is 28 bits (overflows once per full period) */

  /* check if a level step was completed */
  if (l->phase > 0xFFFFFFF)
  {
    l->phase &= 0xFFFFFFF;

    /* update index */

    /* alternate modes */
    if (l->flag == 1)
    {
      if (l->mode == LINEAR_MODE_OUTWARD_FLIP)
        l->index -= 1;
      else if (l->mode == LINEAR_MODE_INWARD_FLIP)
        l->index += 1;
    }
    /* standard modes */
    else
    {
      if (l->mode == LINEAR_MODE_OUTWARD_HOLD_AT_END)
        l->index += 1;
      else if (l->mode == LINEAR_MODE_OUTWARD_HOLD_AT_START)
        l->index += 1;
      else if (l->mode == LINEAR_MODE_OUTWARD_REPEAT)
        l->index += 1;
      else if (l->mode == LINEAR_MODE_OUTWARD_FLIP)
        l->index += 1;
      else if (l->mode == LINEAR_MODE_INWARD_HOLD_AT_END)
        l->index -= 1;
      else if (l->mode == LINEAR_MODE_INWARD_HOLD_AT_START)
        l->index -= 1;
      else if (l->mode == LINEAR_MODE_INWARD_REPEAT)
        l->index -= 1;
      else if (l->mode == LINEAR_MODE_INWARD_FLIP)
        l->index -= 1;
    }

    /* check if a period was completed */
    if (l->index > 128)
    {
      if (l->mode == LINEAR_MODE_OUTWARD_HOLD_AT_END)
      {
        l->index = 128;
        l->flag = 1;
      }
      else if (l->mode == LINEAR_MODE_OUTWARD_HOLD_AT_START)
      {
        l->index = 0;
        l->flag = 1;
      }
      else if (l->mode == LINEAR_MODE_OUTWARD_REPEAT)
      {
        l->index = 0;
      }
      else if (l->mode == LINEAR_MODE_OUTWARD_FLIP)
      {
        l->index = 128;
        l->flag = 1;
      }
      else if (l->mode == LINEAR_MODE_INWARD_FLIP)
      {
        l->index = 128;
        l->flag = 0;
      }
    }
    else if (l->index < 0)
    {
      if (l->mode == LINEAR_MODE_INWARD_HOLD_AT_END)
      {
        l->index = 0;
        l->flag = 1;
      }
      else if (l->mode == LINEAR_MODE_INWARD_HOLD_AT_START)
      {
        l->index = 128;
        l->flag = 1;
      }
      else if (l->mode == LINEAR_MODE_INWARD_REPEAT)
      {
        l->index = 128;
      }
      else if (l->mode == LINEAR_MODE_INWARD_FLIP)
      {
        l->index = 0;
        l->flag = 1;
      }
      else if (l->mode == LINEAR_MODE_OUTWARD_FLIP)
      {
        l->index = 0;
        l->flag = 0;
      }
    }

    /* update level */
    if (l->type == LINEAR_TYPE_PITCH)
      l->level = S_linear_pitch_wavetable[l->row][l->index];
    else if (l->type == LINEAR_TYPE_PULSE_WIDTH)
      l->level = S_linear_pulse_width_wavetable[l->row][l->index];
    else
      l->level = 0;
  }

  return 0;
}

/*******************************************************************************
** linear_generate_tables()
*******************************************************************************/
short int linear_generate_tables()
{
  int m;
  int n;

  /* pitch & pulse wave tables */
  for (n = 0; n < 9; n++)
  {
    for (m = 0; m <= 128; m++)
    {
      S_linear_pitch_wavetable[n][m] = 
        (int) (((S_linear_pitch_depth_table[n] * m) / 128.0f) + 0.5f);

      S_linear_pulse_width_wavetable[n][m] = 
        (int) (((S_linear_pulse_depth_table[n] * m) / 128.0f) + 0.5f);
    }
  }

  /* phase increment table  */
  for (m = 0; m < LINEAR_TABLE_RATES_PER_ROW; m++)
  {
    S_linear_phase_increment_table[ 0 * LINEAR_TABLE_RATES_PER_ROW + m] = 
      (int) (((S_linear_freq_table[m] * 4) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_linear_phase_increment_table[ 1 * LINEAR_TABLE_RATES_PER_ROW + m] = 
      (int) (((S_linear_freq_table[m] * 8) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_linear_phase_increment_table[ 2 * LINEAR_TABLE_RATES_PER_ROW + m] = 
      (int) (((S_linear_freq_table[m] * 16) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_linear_phase_increment_table[ 3 * LINEAR_TABLE_RATES_PER_ROW + m] = 
      (int) (((S_linear_freq_table[m] * 32) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_linear_phase_increment_table[ 4 * LINEAR_TABLE_RATES_PER_ROW + m] = 
      (int) (((S_linear_freq_table[m] * 64) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_linear_phase_increment_table[ 5 * LINEAR_TABLE_RATES_PER_ROW + m] = 
      (int) (((S_linear_freq_table[m] * 128) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_linear_phase_increment_table[ 6 * LINEAR_TABLE_RATES_PER_ROW + m] = 
      (int) (((S_linear_freq_table[m] * 256) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_linear_phase_increment_table[ 7 * LINEAR_TABLE_RATES_PER_ROW + m] = 
      (int) (((S_linear_freq_table[m] * 512) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
  }

  /* testing: print tables */
#if 0
  printf("Linear Envelope Pitch Wavetable (Depth 9):\n");

  for (m = 0; m <= 128; m++)
    printf("  %d\n", S_linear_pitch_wavetable[8][m]);
#endif

#if 0
  printf("Linear Envelope Phase Increment Table:\n");

  for (m = 0; m < LINEAR_TABLE_SIZE; m++)
    printf("Sub-Oscillator Rate %d Phase Inc: %d\n", m, S_linear_phase_increment_table[m]);
#endif

  return 0;
}

