/*******************************************************************************
** lfo.c (vibrato / tremolo)
*******************************************************************************/

#include <stdio.h>    /* testing */
#include <stdlib.h>
#include <math.h>

#include "clock.h"
#include "lfo.h"
#include "pi.h"
#include "tempo.h"

/* vibrato / tremolo wave tables  */
/* rows: 9 depths                 */
/* index: 128 steps in the wave   */
static short int S_vibrato_tri_wavetable[9][128];
static short int S_vibrato_saw_wavetable[9][128];

static short int S_tremolo_tri_wavetable[9][128];
static short int S_tremolo_saw_wavetable[9][128];

static short int S_wobble_tri_wavetable[9][128];
static short int S_wobble_saw_wavetable[9][128];

/* vibrato depth table          */
/* assuming each semitone is    */
/*   divided into 1/64ths:      */
/* depth 1:   4 -> 6.25 cents   */
/* depth 2:   8 -> 12.5 cents   */
/* depth 3:  12 -> 18.75 cents  */
/* depth 4:  16 -> 25 cents     */
/* depth 5:  32 -> 50 cents     */
/* depth 6:  32 -> 75 cents     */
/* depth 7:  64 -> 1 semitone   */
/* depth 8:  96 -> 150 cents    */
/* depth 9: 128 -> 2 semitones  */

/* the depth values above give the peak-to-peak swing   */
/* of the sine wave. note that the values in the table  */
/* below are amplitudes, which are half of these.       */
static short int S_vibrato_depth_table[9] = 
                  {2, 4, 6, 8, 16, 24, 32, 48, 64};

/* tremolo depth table                                  */
/* the values are found using the formula:              */
/*   (10 * (log(1 / val) / log(10)) / DB_STEP_10_BIT,   */
/*   where DB_STEP_10_BIT = 0.046875                    */
/* the increment of 12 in db is equivalent to           */
/* multiplying by 7/8 in terms of linear amplitudes.    */
/* note that these values are the peak-to-peak swing    */
static short int S_tremolo_depth_table[9] = 
                  {12, 24, 36, 48, 60, 72, 84, 96, 108};

/* wobble depth table                                 */
/* the values in the table are for modulating the     */
/* pitch of the sync or ring oscillators; for pulse   */
/* width modulation, we divide these by two.          */
/* note that these values are the peak-to-peak swing  */
static short int S_wobble_depth_table[9] = 
                  {64, 128, 192, 256, 320, 384, 448, 512, 576};

/* lfo phase increment table */
static unsigned int S_lfo_phase_increment_table[TEMPO_NUM_BPMS][9];

/* lfo speed table */

/* the speeds give the number of periods per quarter note */
/* at 120 bpm, they are: 2 hz, 3 hz, 4 hz, ..., 10 hz     */
static float S_lfo_speed_table[9] = 
              {1.0f, 1.5f, 2.0f, 2.5f, 3.0f, 3.5f, 4.0f, 4.5f, 5.0f};

/*******************************************************************************
** lfo_setup()
*******************************************************************************/
short int lfo_setup(lfo* l, int type, int mode)
{
  if (l == NULL)
    return 1;

  /* set type */
  if ((type >= 0) && (type <= LFO_NUM_TYPES))
    l->type = type;
  else
    l->type = LFO_TYPE_VIBRATO;

  /* set mode */
  if ((mode >= 0) && (mode <= LFO_NUM_MODES))
    l->mode = mode;
  else
    l->mode = LFO_MODE_TRIANGLE;

  /* initialize depth & speed */
  l->depth = 0;
  l->speed = 1;

  /* initialize wavetable row & index */
  l->row = 0;
  l->index = 0;

  /* initialize noise lfsr */
  l->lfsr = 0x0001;

  /* initialize phase increment */
  l->increment = 0;

  /* initialize phase & level */
  l->phase = 0;
  l->level = 0;

  return 0;
}

/*******************************************************************************
** lfo_set_depth()
*******************************************************************************/
short int lfo_set_depth(lfo* l, int depth)
{
  if (l == NULL)
    return 1;

  if ((depth >= 1) && (depth <= 9))
  {
    l->row = depth - 1;
    l->depth = depth;
  }
  else
  {
    l->row = 0;
    l->depth = 0;
  }

  return 0;
}

/*******************************************************************************
** lfo_set_speed()
*******************************************************************************/
short int lfo_set_speed(lfo* l, int tempo, int speed)
{
  if (l == NULL)
    return 1;

  /* make sure tempo is valid */
  if ((tempo < 0) || (tempo >= TEMPO_NUM_BPMS))
    return 0;

  /* set the speed */
  if ((speed >= 1) && (speed <= 9))
    l->speed = speed;
  else
    l->speed = 1;

  /* set phase increment */
  l->increment = 
    S_lfo_phase_increment_table[TEMPO_COMPUTE_INDEX(tempo)][l->speed - 1];

  return 0;
}

/*******************************************************************************
** lfo_adjust_to_tempo()
*******************************************************************************/
short int lfo_adjust_to_tempo(lfo* l, int tempo)
{
  if (l == NULL)
    return 1;

  /* make sure tempo is valid */
  if ((tempo < 0) || (tempo >= TEMPO_NUM_BPMS))
    return 0;

  /* set phase increment */
  l->increment = 
    S_lfo_phase_increment_table[TEMPO_COMPUTE_INDEX(tempo)][l->speed - 1];

  return 0;
}

/*******************************************************************************
** lfo_trigger()
*******************************************************************************/
short int lfo_trigger(lfo* l)
{
  if (l == NULL)
    return 1;

  /* reset phase */
  l->phase = 0;

  /* initialize wavetable index */

  /* note that the alternate vibrato table index  */
  /* starts at the lowest point of the waveform   */
  if (l->type == LFO_TYPE_VIBRATO_ALTERNATE)
  {
    if (l->mode == LFO_MODE_TRIANGLE)
      l->index = 96;
    else if (l->mode == LFO_MODE_SAW_UP)
      l->index = 64;
    else if (l->mode == LFO_MODE_SAW_DOWN)
      l->index = 64;
    else
      l->index = 0;
  }
  else
    l->index = 0;

  /* initialize noise lfsr */
  l->lfsr = 0x0001;

  /* initialize level */
  l->level = 0;

  return 0;
}

/*******************************************************************************
** lfo_update()
*******************************************************************************/
short int lfo_update(lfo* l)
{
  if (l == NULL)
    return 1;

  /* if lfo depth is 0, return */
  if (l->depth == 0)
  {
    l->level = 0;

    return 0;
  }

  /* update phase */
  l->phase += l->increment;

  /* note that the phase register is 28 bits (overflows once per full period) */
  /* the mantissa is 21 bits (overflows once per wavetable index step)        */

  /* check if a wavetable index step was completed */
  if (l->phase > 0x1FFFFF)
  {
    l->phase &= 0x1FFFFF;

    /* update index */
    if ((l->mode == LFO_MODE_TRIANGLE)  || 
        (l->mode == LFO_MODE_SQUARE)    || 
        (l->mode == LFO_MODE_SAW_UP)    || 
        (l->mode == LFO_MODE_SAW_DOWN))
    {
      l->index += 1;

      if (l->index >= 128)
        l->index = 0;
    }
    /* update lfsr */
    else if ( (l->mode == LFO_MODE_RANDOM_SQUARE) || 
              (l->mode == LFO_MODE_RANDOM_SAW))
    {
      /* 15-bit lfsr, taps on 1 and 2 */
      if ((l->lfsr & 0x0001) ^ ((l->lfsr & 0x0002) >> 1))
        l->lfsr = ((l->lfsr >> 1) & 0x3FFF) | 0x4000;
      else
        l->lfsr = (l->lfsr >> 1) & 0x3FFF;
    }

    /* update vibrato level */
    if ((l->type == LFO_TYPE_VIBRATO) || 
        (l->type == LFO_TYPE_VIBRATO_ALTERNATE))
    {
      if (l->mode == LFO_MODE_TRIANGLE)
        l->level = S_vibrato_tri_wavetable[l->row][l->index];
      else if (l->mode == LFO_MODE_SQUARE)
      {
        if (l->index < 64)
          l->level = S_vibrato_depth_table[l->row];
        else
          l->level = -S_vibrato_depth_table[l->row];
      }
      else if (l->mode == LFO_MODE_SAW_UP)
        l->level = S_vibrato_saw_wavetable[l->row][l->index];
      else if (l->mode == LFO_MODE_SAW_DOWN)
        l->level = S_vibrato_saw_wavetable[l->row][127 - l->index];
      else if (l->mode == LFO_MODE_RANDOM_SQUARE)
      {
        if ((l->lfsr & 0x7F) < 64)
          l->level = S_vibrato_depth_table[l->row];
        else
          l->level = -S_vibrato_depth_table[l->row];
      }
      else if (l->mode == LFO_MODE_RANDOM_SAW)
        l->level = S_vibrato_saw_wavetable[l->row][l->lfsr & 0x7F];

      /* alternate vibrato is upwards only */
      if (l->type == LFO_TYPE_VIBRATO_ALTERNATE)
        l->level += S_vibrato_depth_table[l->row];
    }
    /* update tremolo / fm modulator wobble level */
    else if ( (l->type == LFO_TYPE_TREMOLO) || 
              (l->type == LFO_TYPE_WOBBLE_AMPLITUDE))
    {
      if (l->mode == LFO_MODE_TRIANGLE)
        l->level = S_tremolo_tri_wavetable[l->row][l->index];
      else if (l->mode == LFO_MODE_SQUARE)
      {
        if (l->index < 64)
          l->level = 0;
        else
          l->level = S_tremolo_depth_table[l->row];
      }
      else if (l->mode == LFO_MODE_SAW_UP)
        l->level = S_tremolo_saw_wavetable[l->row][127 - l->index];
      else if (l->mode == LFO_MODE_SAW_DOWN)
        l->level = S_tremolo_saw_wavetable[l->row][l->index];
      else if (l->mode == LFO_MODE_RANDOM_SQUARE)
      {
        if ((l->lfsr & 0x7F) < 64)
          l->level = 0;
        else
          l->level = S_tremolo_depth_table[l->row];
      }
      else if (l->mode == LFO_MODE_RANDOM_SAW)
        l->level = S_tremolo_saw_wavetable[l->row][l->lfsr & 0x7F];
    }
    /* update sync / ring mod / pulse width wobble level */
    else if ( (l->type == LFO_TYPE_WOBBLE_PITCH) || 
              (l->type == LFO_TYPE_WOBBLE_PULSE_WIDTH))
    {
      if (l->mode == LFO_MODE_TRIANGLE)
        l->level = S_wobble_tri_wavetable[l->row][l->index];
      else if (l->mode == LFO_MODE_SQUARE)
      {
        if (l->index < 64)
          l->level = 0;
        else
          l->level = S_wobble_depth_table[l->row];
      }
      else if (l->mode == LFO_MODE_SAW_UP)
        l->level = S_wobble_saw_wavetable[l->row][127 - l->index];
      else if (l->mode == LFO_MODE_SAW_DOWN)
        l->level = S_wobble_saw_wavetable[l->row][l->index];
      else if (l->mode == LFO_MODE_RANDOM_SQUARE)
      {
        if ((l->lfsr & 0x7F) < 64)
          l->level = 0;
        else
          l->level = S_wobble_depth_table[l->row];
      }
      else if (l->mode == LFO_MODE_RANDOM_SAW)
        l->level = S_wobble_saw_wavetable[l->row][l->lfsr & 0x7F];

      /* for pulse width, divide the level by 2 */
      if (l->type == LFO_TYPE_WOBBLE_PULSE_WIDTH)
        l->level = l->level / 2;
    }
  }

  return 0;
}

/*******************************************************************************
** lfo_generate_tables()
*******************************************************************************/
short int lfo_generate_tables()
{
  int m;
  int n;

  /* triangle wavetables */
  for (n = 0; n < 9; n++)
  {
    for (m = 0; m < 32; m++)
    {
      S_vibrato_tri_wavetable[n][m] = (S_vibrato_depth_table[n] * m) / 32;
      S_vibrato_tri_wavetable[n][m + 32] = S_vibrato_depth_table[n] - S_vibrato_tri_wavetable[n][m];

      S_vibrato_tri_wavetable[n][m + 64] = S_vibrato_tri_wavetable[n][m];
      S_vibrato_tri_wavetable[n][m + 96] = S_vibrato_tri_wavetable[n][m + 32];
    }

    for (m = 0; m < 64; m++)
    {
      S_tremolo_tri_wavetable[n][m] = (S_tremolo_depth_table[n] * m) / 64;
      S_tremolo_tri_wavetable[n][m + 64] = S_tremolo_depth_table[n] - S_tremolo_tri_wavetable[n][m];

      S_wobble_tri_wavetable[n][m] = (S_wobble_depth_table[n] * m) / 64;
      S_wobble_tri_wavetable[n][m + 64] = S_wobble_depth_table[n] - S_wobble_tri_wavetable[n][m];
    }
  }

  /* saw wavetables */
  for (n = 0; n < 9; n++)
  {
    for (m = 0; m < 64; m++)
    {
      S_vibrato_saw_wavetable[n][m] = (S_vibrato_depth_table[n] * m) / 64;
      S_vibrato_saw_wavetable[n][m + 64] = S_vibrato_saw_wavetable[n][m] - S_vibrato_depth_table[n];
    }

    for (m = 0; m < 128; m++)
    {
      S_tremolo_saw_wavetable[n][m] = (S_tremolo_depth_table[n] * m) / 128;

      S_wobble_saw_wavetable[n][m] = (S_wobble_depth_table[n] * m) / 128;
    }
  }

  /* phase increment table  */
  for (m = TEMPO_LOWER_BOUND; m <= TEMPO_UPPER_BOUND; m++)
  {
    for (n = 0; n < 9; n++)
    {
      S_lfo_phase_increment_table[TEMPO_COMPUTE_INDEX(m)][n] = 
        (int) ((TEMPO_COMPUTE_BEAT_FREQUENCY(m) * S_lfo_speed_table[n] * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
    }
  }

  /* testing: print tables */
#if 0
  printf("Vibrato Triangle Table (Depth 9):\n");

  for (m = 0; m < 128; m++)
    printf("  %d\n", S_vibrato_tri_wavetable[8][m]);
#endif

#if 0
  printf("Vibrato Saw Table (Depth 9):\n");

  for (m = 0; m < 128; m++)
    printf("  %d\n", S_vibrato_saw_wavetable[8][m]);
#endif

#if 0
  printf("Tremolo Triangle Table (Depth 9):\n");

  for (m = 0; m < 128; m++)
    printf("  %d\n", S_tremolo_tri_wavetable[8][m]);
#endif

#if 0
  printf("Tremolo Saw Table (Depth 9):\n");

  for (m = 0; m < 128; m++)
    printf("  %d\n", S_tremolo_saw_wavetable[8][m]);
#endif

#if 0
  printf("LFO Phase Increment Table (at 120 BPM):\n");

  for (n = 0; n < 9; n++)
  {
    printf("Speed: %d, Phase Inc: %d, %d\n", k, 
              S_lfo_phase_increment_table[TEMPO_COMPUTE_INDEX(120)][n], 
              S_lfo_phase_increment_table[TEMPO_COMPUTE_INDEX(120)][n]);
  }
#endif

  return 0;
}
