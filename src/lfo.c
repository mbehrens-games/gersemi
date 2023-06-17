/*******************************************************************************
** lfo.c (low frequency oscillator)
*******************************************************************************/

#include <stdio.h>    /* testing */
#include <stdlib.h>
#include <math.h>

#include "bank.h"
#include "clock.h"
#include "lfo.h"
#include "tempo.h"

#define PI      3.14159265358979323846f
#define TWO_PI  6.28318530717958647693f

#define LFO_NUM_DEPTHS  16
#define LFO_NUM_SPEEDS  16

#define LFO_WAVETABLE_SIZE      128

#define LFO_WAVETABLE_SIZE_1_4  (LFO_WAVETABLE_SIZE / 4)
#define LFO_WAVETABLE_SIZE_1_2  (LFO_WAVETABLE_SIZE / 2)
#define LFO_WAVETABLE_SIZE_3_4  ((3 * LFO_WAVETABLE_SIZE) / 4)

/* vibrato / tremolo wave tables  */
/* rows: 16 depths                */
/* index: 128 steps in the wave   */
static short int S_vibrato_tri_wavetable[LFO_NUM_DEPTHS][LFO_WAVETABLE_SIZE];
static short int S_vibrato_saw_wavetable[LFO_NUM_DEPTHS][LFO_WAVETABLE_SIZE];

static short int S_tremolo_tri_wavetable[LFO_NUM_DEPTHS][LFO_WAVETABLE_SIZE];
static short int S_tremolo_saw_wavetable[LFO_NUM_DEPTHS][LFO_WAVETABLE_SIZE];

static short int S_wobble_tri_wavetable[LFO_NUM_DEPTHS][LFO_WAVETABLE_SIZE];
static short int S_wobble_saw_wavetable[LFO_NUM_DEPTHS][LFO_WAVETABLE_SIZE];

/* vibrato depth table                                */
/* assuming each semitone is divided into 1/64ths.    */
/* note that the values in the table are amplitudes,  */
/* which are half of the peak-to-peak swing.          */
static short int S_vibrato_depth_table[16] = 
                  {  1, /*   2 (3.125 cents)  */
                     2, /*   4 (6.25 cents)   */
                     3, /*   6 (9.375 cents)  */
                     4, /*   8 (12.5 cents)   */
                     5, /*  10 (15.625 cents) */
                     6, /*  12 (18.75 cents)  */
                     7, /*  14 (21.875 cents) */
                     8, /*  16 (25 cents)     */
                    12, /*  24 (37.5 cents)   */
                    16, /*  32 (50 cents)     */
                    24, /*  48 (75 cents)     */
                    32, /*  64 (1 semitone)   */
                    40, /*  80 (125 cents)    */
                    48, /*  96 (150 cents)    */
                    56, /* 112 (175 cents)    */
                    64  /* 128 (2 semitones)  */
                  };

/* tremolo depth table                                  */
/* the values are found using the formula:              */
/*   (10 * (log(1 / val) / log(10)) / DB_STEP_10_BIT,   */
/*   where DB_STEP_10_BIT = 0.046875                    */
/* note that these values are the peak-to-peak swing    */
static short int S_tremolo_depth_table[16] = 
                  { 10,  /* (9/10)^1  */
                    20,  /* (9/10)^2  */
                    29,  /* (9/10)^3  */
                    39,  /* (9/10)^4  */
                    49,  /* (9/10)^5  */
                    58,  /* (9/10)^6  */
                    68,  /* (9/10)^7  */
                    78,  /* (9/10)^8  */
                    88,  /* (9/10)^9  */
                    98,  /* (9/10)^10 */
                   107,  /* (9/10)^11 */
                   117,  /* (9/10)^12 */
                   127,  /* (9/10)^13 */
                   137,  /* (9/10)^14 */
                   146,  /* (9/10)^15 */
                   156   /* (9/10)^16 */
                  };

/* wobble depth table                                 */
/* the values in the table are for modulating the     */
/* pitch of the sync or ring oscillators; for pulse   */
/* width modulation, we divide these by two.          */
/* note that these values are the peak-to-peak swing  */
static short int S_wobble_depth_table[16] = 
                  { 64, 
                   128, 
                   192, 
                   256, 
                   320, 
                   384, 
                   448, 
                   512, 
                   576, 
                   576, 
                   576, 
                   576, 
                   576, 
                   576, 
                   576, 
                   576
                  };

/* lfo phase increment table */
static unsigned int S_lfo_phase_increment_table[TEMPO_NUM_BPMS][LFO_NUM_SPEEDS];

/* lfo speed table */

/* the speeds give the number of periods per quarter note */
/* the frequencies given are obtained at 120 bpm          */
static float S_lfo_speed_table[16] = 
              { 1.0f,   /* 2 hz     */
                1.5f,   /* 3 hz     */
                2.0f,   /* 4 hz     */
                2.25f,  /* 4.5 hz   */
                2.5f,   /* 5 hz     */
                2.625f, /* 5.25 hz  */
                2.75f,  /* 5.5 hz   */
                2.875f, /* 5.75 hz  */
                3.0f,   /* 6 hz     */
                3.125f, /* 6.25 hz  */
                3.25f,  /* 6.5 hz   */
                3.375f, /* 6.75 hz  */
                3.5f,   /* 7 hz     */
                3.75f,  /* 7.5 hz   */
                4.0f,   /* 8 hz     */
                4.5f    /* 9 hz     */
              };

/* lfo bank */
lfo G_lfo_bank[BANK_NUM_LFOS];

/*******************************************************************************
** lfo_setup_all()
*******************************************************************************/
short int lfo_setup_all()
{
  int k;

  /* setup all lfos */
  for (k = 0; k < BANK_NUM_VOICES; k++)
    lfo_reset(k);

  return 0;
}

/*******************************************************************************
** lfo_reset()
*******************************************************************************/
short int lfo_reset(int voice_index)
{
  lfo* l;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain lfo pointer */
  l = &G_lfo_bank[voice_index];

  /* reset type and mode */
  l->type = LFO_TYPE_VIBRATO;
  l->mode = LFO_MODE_TRIANGLE;

  /* reset depth & speed */
  l->depth = 0;
  l->speed = 1;

  /* reset wavetable row & index */
  l->row = 0;
  l->index = 0;

  /* reset noise lfsr */
  l->lfsr = 0x0001;

  /* reset phase increment */
  l->increment = 0;

  /* reset phase & level */
  l->phase = 0;
  l->level = 0;

  return 0;
}

/*******************************************************************************
** lfo_set_type_and_mode()
*******************************************************************************/
short int lfo_set_type_and_mode(int voice_index, int type, int mode)
{
  lfo* l;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain lfo pointer */
  l = &G_lfo_bank[voice_index];

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

  return 0;
}

/*******************************************************************************
** lfo_set_depth()
*******************************************************************************/
short int lfo_set_depth(int voice_index, int depth)
{
  lfo* l;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain lfo pointer */
  l = &G_lfo_bank[voice_index];

  /* set depth */
  if ((depth >= 1) && (depth <= LFO_NUM_DEPTHS))
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
short int lfo_set_speed(int voice_index, int tempo, int speed)
{
  lfo* l;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain lfo pointer */
  l = &G_lfo_bank[voice_index];

  /* make sure tempo is valid */
  if (TEMPO_IS_NOT_VALID(tempo))
    return 0;

  /* set the speed */
  if ((speed >= 1) && (speed <= LFO_NUM_SPEEDS))
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
short int lfo_adjust_to_tempo(int voice_index, int tempo)
{
  lfo* l;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain lfo pointer */
  l = &G_lfo_bank[voice_index];

  /* make sure tempo is valid */
  if ((tempo < TEMPO_LOWER_BOUND) || (tempo > TEMPO_UPPER_BOUND))
    return 0;

  /* set phase increment */
  l->increment = 
    S_lfo_phase_increment_table[TEMPO_COMPUTE_INDEX(tempo)][l->speed - 1];

  return 0;
}

/*******************************************************************************
** lfo_trigger()
*******************************************************************************/
short int lfo_trigger(int voice_index)
{
  lfo* l;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain lfo pointer */
  l = &G_lfo_bank[voice_index];

  /* reset phase */
  l->phase = 0;

  /* initialize wavetable index */

  /* note that the alternate vibrato table index  */
  /* starts at the lowest point of the waveform   */
  if (l->type == LFO_TYPE_VIBRATO_ALTERNATE)
  {
    if (l->mode == LFO_MODE_TRIANGLE)
      l->index = LFO_WAVETABLE_SIZE_3_4;
    else if (l->mode == LFO_MODE_SAW_UP)
      l->index = LFO_WAVETABLE_SIZE_1_2;
    else if (l->mode == LFO_MODE_SAW_DOWN)
      l->index = LFO_WAVETABLE_SIZE_1_2;
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
** lfo_update_all()
*******************************************************************************/
short int lfo_update_all()
{
  int k;

  lfo* l;

  /* update all lfos */
  for (k = 0; k < BANK_NUM_VOICES; k++)
  {
    l = &G_lfo_bank[k];

    /* if lfo depth is 0, continue */
    if (l->depth == 0)
    {
      l->level = 0;

      continue;
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

        if (l->index >= LFO_WAVETABLE_SIZE)
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
          if (l->index < LFO_WAVETABLE_SIZE_1_2)
            l->level = S_vibrato_depth_table[l->row];
          else
            l->level = -S_vibrato_depth_table[l->row];
        }
        else if (l->mode == LFO_MODE_SAW_UP)
          l->level = S_vibrato_saw_wavetable[l->row][l->index];
        else if (l->mode == LFO_MODE_SAW_DOWN)
          l->level = S_vibrato_saw_wavetable[l->row][LFO_WAVETABLE_SIZE - 1 - l->index];
        else if (l->mode == LFO_MODE_RANDOM_SQUARE)
        {
          if ((l->lfsr & 0x7F) < LFO_WAVETABLE_SIZE_1_2)
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
          if (l->index < LFO_WAVETABLE_SIZE_1_2)
            l->level = 0;
          else
            l->level = S_tremolo_depth_table[l->row];
        }
        else if (l->mode == LFO_MODE_SAW_UP)
          l->level = S_tremolo_saw_wavetable[l->row][LFO_WAVETABLE_SIZE - 1 - l->index];
        else if (l->mode == LFO_MODE_SAW_DOWN)
          l->level = S_tremolo_saw_wavetable[l->row][l->index];
        else if (l->mode == LFO_MODE_RANDOM_SQUARE)
        {
          if ((l->lfsr & 0x7F) < LFO_WAVETABLE_SIZE_1_2)
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
          if (l->index < LFO_WAVETABLE_SIZE_1_2)
            l->level = 0;
          else
            l->level = S_wobble_depth_table[l->row];
        }
        else if (l->mode == LFO_MODE_SAW_UP)
          l->level = S_wobble_saw_wavetable[l->row][LFO_WAVETABLE_SIZE - 1 - l->index];
        else if (l->mode == LFO_MODE_SAW_DOWN)
          l->level = S_wobble_saw_wavetable[l->row][l->index];
        else if (l->mode == LFO_MODE_RANDOM_SQUARE)
        {
          if ((l->lfsr & 0x7F) < LFO_WAVETABLE_SIZE_1_2)
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
  for (n = 0; n < LFO_NUM_DEPTHS; n++)
  {
    for (m = 0; m < 32; m++)
    {
      S_vibrato_tri_wavetable[n][m] = (S_vibrato_depth_table[n] * m) / LFO_WAVETABLE_SIZE_1_4;
      S_vibrato_tri_wavetable[n][m + LFO_WAVETABLE_SIZE_1_4] = S_vibrato_depth_table[n] - S_vibrato_tri_wavetable[n][m];

      S_vibrato_tri_wavetable[n][m + LFO_WAVETABLE_SIZE_1_2] = -S_vibrato_tri_wavetable[n][m];
      S_vibrato_tri_wavetable[n][m + LFO_WAVETABLE_SIZE_3_4] = S_vibrato_tri_wavetable[n][m] - S_vibrato_depth_table[n];
    }

    for (m = 0; m < LFO_WAVETABLE_SIZE_1_2; m++)
    {
      S_tremolo_tri_wavetable[n][m] = (S_tremolo_depth_table[n] * m) / LFO_WAVETABLE_SIZE_1_2;
      S_tremolo_tri_wavetable[n][m + LFO_WAVETABLE_SIZE_1_2] = S_tremolo_depth_table[n] - S_tremolo_tri_wavetable[n][m];

      S_wobble_tri_wavetable[n][m] = (S_wobble_depth_table[n] * m) / LFO_WAVETABLE_SIZE_1_2;
      S_wobble_tri_wavetable[n][m + LFO_WAVETABLE_SIZE_1_2] = S_wobble_depth_table[n] - S_wobble_tri_wavetable[n][m];
    }
  }

  /* saw wavetables */
  for (n = 0; n < LFO_NUM_DEPTHS; n++)
  {
    for (m = 0; m < LFO_WAVETABLE_SIZE_1_2; m++)
    {
      S_vibrato_saw_wavetable[n][m] = (S_vibrato_depth_table[n] * m) / LFO_WAVETABLE_SIZE_1_2;
      S_vibrato_saw_wavetable[n][m + LFO_WAVETABLE_SIZE_1_2] = S_vibrato_saw_wavetable[n][m] - S_vibrato_depth_table[n];
    }

    for (m = 0; m < LFO_WAVETABLE_SIZE; m++)
    {
      S_tremolo_saw_wavetable[n][m] = (S_tremolo_depth_table[n] * m) / LFO_WAVETABLE_SIZE;

      S_wobble_saw_wavetable[n][m] = (S_wobble_depth_table[n] * m) / LFO_WAVETABLE_SIZE;
    }
  }

  /* phase increment table  */
  for (m = TEMPO_LOWER_BOUND; m <= TEMPO_UPPER_BOUND; m++)
  {
    for (n = 0; n < LFO_NUM_SPEEDS; n++)
    {
      S_lfo_phase_increment_table[TEMPO_COMPUTE_INDEX(m)][n] = 
        (int) ((TEMPO_COMPUTE_BEAT_FREQUENCY(m) * S_lfo_speed_table[n] * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
    }
  }

  /* testing: print tables */
#if 0
  printf("Vibrato Triangle Table (Depth 16):\n");

  for (m = 0; m < LFO_WAVETABLE_SIZE; m++)
    printf("  %d\n", S_vibrato_tri_wavetable[15][m]);
#endif

#if 0
  printf("Vibrato Saw Table (Depth 16):\n");

  for (m = 0; m < LFO_WAVETABLE_SIZE; m++)
    printf("  %d\n", S_vibrato_saw_wavetable[15][m]);
#endif

#if 0
  printf("Tremolo Triangle Table (Depth 16):\n");

  for (m = 0; m < LFO_WAVETABLE_SIZE; m++)
    printf("  %d\n", S_tremolo_tri_wavetable[15][m]);
#endif

#if 0
  printf("Tremolo Saw Table (Depth 16):\n");

  for (m = 0; m < LFO_WAVETABLE_SIZE; m++)
    printf("  %d\n", S_tremolo_saw_wavetable[15][m]);
#endif

#if 0
  printf("LFO Phase Increment Table (at 120 BPM):\n");

  for (n = 0; n < LFO_NUM_SPEEDS; n++)
  {
    printf("Speed: %d, Phase Inc: %d (120 BPM), %d (150 BPM)\n", n, 
              S_lfo_phase_increment_table[TEMPO_COMPUTE_INDEX(120)][n], 
              S_lfo_phase_increment_table[TEMPO_COMPUTE_INDEX(150)][n]);
  }
#endif

  return 0;
}
