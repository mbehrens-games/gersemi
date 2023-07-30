/*******************************************************************************
** lfo.c (low frequency oscillator)
*******************************************************************************/

#include <stdio.h>    /* testing */
#include <stdlib.h>
#include <math.h>

#include "bank.h"
#include "clock.h"
#include "lfo.h"
#include "patch.h"

#define PI      3.14159265358979323846f
#define TWO_PI  6.28318530717958647693f

#define LFO_WAVE_AMPLITUDE      128
#define LFO_WAVE_AMPLITUDE_HALF (LFO_WAVE_AMPLITUDE / 2)

#define LFO_MOD_WHEEL_STEPS   16
#define LFO_AFTERTOUCH_STEPS  16

/* vibrato depth table */
/* assuming 128 steps per semitone */
static short int  S_lfo_vibrato_depth_table[PATCH_MOD_DEPTH_NUM_VALUES] = 
                  {   6,  /* 4.7 cents    */
                      9,  /* 7 cents      */
                     12,  /* 9.4 cents    */
                     18,  /* 14.1 cents   */
                     26,  /* 20.3 cents   */
                     38,  /* 29.7 cents   */
                     51,  /* 39.8 cents   */
                     64,  /* 50 cents     */
                     77,  /* 60.2 cents   */
                     90,  /* 70.3 cents   */
                    102,  /* 79.6 cents   */
                    115,  /* 89.8 cents   */
                    128,  /* 1 semitones  */
                    256,  /* 2 semitones  */
                    512,  /* 4 semitones  */
                    896   /* 7 semitones  */
                  };

/* tremolo depth table */
static short int  S_lfo_tremolo_depth_table[PATCH_MOD_DEPTH_NUM_VALUES] = 
                  { 6 * 1, 
                    6 * 2, 
                    6 * 3, 
                    6 * 4, 
                    6 * 5, 
                    6 * 6, 
                    6 * 7, 
                    6 * 8, 
                    6 * 9, 
                    6 * 10, 
                    6 * 11, 
                    6 * 12, 
                    6 * 13, 
                    6 * 14, 
                    6 * 15, 
                    6 * 16 
                  };

/* frequency tables */
static float  S_lfo_wave_frequency_table[PATCH_LFO_FREQUENCY_NUM_VALUES] = 
              { 0.333333333333333f, 0.666666666666667f, 1.0f, 
                1.333333333333333f, 1.666666666666667f, 2.0f, 
                2.333333333333333f, 2.666666666666667f, 3.0f, 
                3.333333333333333f, 3.666666666666667f, 4.0f, 
                4.333333333333333f, 4.666666666666667f, 5.0f, 
                5.25f,        5.5f,       5.75f,        6.0f, 
                6.25f,        6.5f,       6.75f,        7.0f, 
                7.333333333333333f, 7.666666666666667f, 8.0f, 
                8.333333333333333f, 8.666666666666667f, 9.0f, 
                9.333333333333333f, 9.666666666666667f, 10.0f 
              };

/* note that the noise frequencies describe the frequency at which the lfsr is    */
/* updated, as opposed to the frequency of completing one "period" (256 updates). */
static float  S_lfo_noise_frequency_table[PATCH_LFO_FREQUENCY_NUM_VALUES] = 
              {  16.0f,   20.0f,   24.0f,   28.0f, 
                 32.0f,   40.0f,   48.0f,   56.0f, 
                 64.0f,   80.0f,   96.0f,  112.0f, 
                128.0f,  160.0f,  192.0f,  224.0f, 
                256.0f,  320.0f,  384.0f,  448.0f, 
                512.0f,  640.0f,  768.0f,  896.0f, 
               1024.0f, 1280.0f, 1536.0f, 1792.0f, 
               2048.0f, 2560.0f, 3072.0f, 3584.0f 
              };

/* delay table */
static float  S_lfo_delay_seconds_table[PATCH_LFO_DELAY_NUM_VALUES] = 
              { 0.0f,               /*  0/24  */
                0.041666666666667f, /*  1/24  */
                0.083333333333333f, /*  2/24  */
                0.125f,             /*  3/24  */
                0.166666666666667f, /*  4/24  */
                0.208333333333333f, /*  5/24  */
                0.25f,              /*  6/24  */
                0.291666666666667f, /*  7/24  */
                0.333333333333333f, /*  8/24  */
                0.375f,             /*  9/24  */
                0.416666666666667f, /* 10/24  */
                0.458333333333333f, /* 11/24  */
                0.5f,               /* 12/24  */
                0.541666666666667f, /* 13/24  */
                0.583333333333333f, /* 14/24  */
                0.625f,             /* 15/24  */
                0.666666666666667f, /* 16/24  */
                0.708333333333333f, /* 17/24  */
                0.75f,              /* 18/24  */
                0.791666666666667f, /* 19/24  */
                0.833333333333333f, /* 20/24  */
                0.875f,             /* 21/24  */
                0.916666666666667f, /* 22/24  */
                0.958333333333333f, /* 23/24  */
                1.0f                /* 24/24  */
              };

/* wavetables */
static short int S_lfo_wavetable_triangle[256];
static short int S_lfo_wavetable_sawtooth[256];

/* delay cycles table */
static int S_lfo_delay_period_table[PATCH_LFO_DELAY_NUM_VALUES];

/* phase increment tables */
static unsigned int S_lfo_wave_phase_increment_table[PATCH_LFO_FREQUENCY_NUM_VALUES];
static unsigned int S_lfo_noise_phase_increment_table[PATCH_LFO_FREQUENCY_NUM_VALUES];

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

  /* lfo parameters */
  l->waveform = 1;
  l->sync = 1;

  l->base_vibrato = 0;
  l->base_tremolo = 0;

  /* depths */
  l->vibrato_depth = S_lfo_vibrato_depth_table[0];
  l->tremolo_depth = S_lfo_tremolo_depth_table[0];

  /* mod wheel */
  l->mod_wheel_vibrato = 0;
  l->mod_wheel_tremolo = 0;

  /* aftertouch */
  l->aftertouch_vibrato = 0;
  l->aftertouch_tremolo = 0;

  /* delay */
  l->delay_period = 0;
  l->delay_cycles = 0;

  /* phase, phase increment */
  l->phase = 0;
  l->increment = 0;

  /* noise lfsr */
  l->lfsr = 0x0001;

  /* controller inputs */
  l->mod_wheel_input = 0;
  l->aftertouch_input = 0;

  /* levels */
  l->vibrato_level = 0;
  l->tremolo_level = 0;

  return 0;
}

/*******************************************************************************
** lfo_load_patch()
*******************************************************************************/
short int lfo_load_patch(int voice_index, int patch_index)
{
  lfo* l;
  patch* p;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain lfo and patch pointers */
  l = &G_lfo_bank[voice_index];
  p = &G_patch_bank[patch_index];

  /* waveform */
  if ((p->lfo_waveform >= PATCH_LFO_WAVEFORM_LOWER_BOUND) && 
      (p->lfo_waveform <= PATCH_LFO_WAVEFORM_UPPER_BOUND))
  {
    l->waveform = p->lfo_waveform;
  }
  else
    l->waveform = PATCH_LFO_WAVEFORM_LOWER_BOUND;

  /* frequency */
  if ((p->lfo_frequency >= PATCH_LFO_FREQUENCY_LOWER_BOUND) && 
      (p->lfo_frequency <= PATCH_LFO_FREQUENCY_UPPER_BOUND))
  {
    if (l->waveform == 5)
      l->increment = S_lfo_noise_phase_increment_table[p->lfo_frequency - PATCH_LFO_FREQUENCY_LOWER_BOUND];
    else
      l->increment = S_lfo_wave_phase_increment_table[p->lfo_frequency - PATCH_LFO_FREQUENCY_LOWER_BOUND];
  }
  else
  {
    if (l->waveform == 5)
      l->increment = S_lfo_noise_phase_increment_table[0];
    else
      l->increment = S_lfo_wave_phase_increment_table[0];
  }

  /* sync */
  if ((p->lfo_sync >= PATCH_LFO_SYNC_LOWER_BOUND) && 
      (p->lfo_sync <= PATCH_LFO_SYNC_UPPER_BOUND))
  {
    l->sync = p->lfo_sync;
  }
  else
    l->sync = PATCH_LFO_SYNC_LOWER_BOUND;

  /* delay */
  if ((p->lfo_delay >= PATCH_LFO_DELAY_LOWER_BOUND) && 
      (p->lfo_delay <= PATCH_LFO_DELAY_UPPER_BOUND))
  {
    l->delay_period = S_lfo_delay_period_table[p->lfo_delay - PATCH_LFO_DELAY_LOWER_BOUND];
  }
  else
    l->delay_period = S_lfo_delay_period_table[0];

  /* base vibrato */
  if ((p->lfo_base_vibrato >= PATCH_MOD_BASE_LOWER_BOUND) && 
      (p->lfo_base_vibrato <= PATCH_MOD_BASE_UPPER_BOUND))
  {
    l->base_vibrato = p->lfo_base_vibrato;
  }
  else
    l->base_vibrato = PATCH_MOD_BASE_LOWER_BOUND;

  /* base tremolo */
  if ((p->lfo_base_tremolo >= PATCH_MOD_BASE_LOWER_BOUND) && 
      (p->lfo_base_tremolo <= PATCH_MOD_BASE_UPPER_BOUND))
  {
    l->base_tremolo = p->lfo_base_tremolo;
  }
  else
    l->base_tremolo = PATCH_MOD_BASE_LOWER_BOUND;

  /* vibrato depth */
  if ((p->vibrato_depth >= PATCH_MOD_DEPTH_LOWER_BOUND) && 
      (p->vibrato_depth <= PATCH_MOD_DEPTH_UPPER_BOUND))
  {
    l->vibrato_depth = S_lfo_vibrato_depth_table[p->vibrato_depth - PATCH_MOD_DEPTH_LOWER_BOUND];
  }
  else
    l->vibrato_depth = S_lfo_vibrato_depth_table[0];

  /* tremolo depth */
  if ((p->tremolo_depth >= PATCH_MOD_DEPTH_LOWER_BOUND) && 
      (p->tremolo_depth <= PATCH_MOD_DEPTH_UPPER_BOUND))
  {
    l->tremolo_depth = S_lfo_tremolo_depth_table[p->tremolo_depth - PATCH_MOD_DEPTH_LOWER_BOUND];
  }
  else
    l->tremolo_depth = S_lfo_tremolo_depth_table[0];

  /* mod wheel vibrato */
  if ((p->mod_wheel_vibrato >= PATCH_MOD_CONTROLLER_LOWER_BOUND) && 
      (p->mod_wheel_vibrato <= PATCH_MOD_CONTROLLER_UPPER_BOUND))
  {
    l->mod_wheel_vibrato = p->mod_wheel_vibrato;
  }
  else
    l->mod_wheel_vibrato = PATCH_MOD_CONTROLLER_LOWER_BOUND;

  /* mod wheel tremolo */
  if ((p->mod_wheel_tremolo >= PATCH_MOD_CONTROLLER_LOWER_BOUND) && 
      (p->mod_wheel_tremolo <= PATCH_MOD_CONTROLLER_UPPER_BOUND))
  {
    l->mod_wheel_tremolo = p->mod_wheel_tremolo;
  }
  else
    l->mod_wheel_tremolo = PATCH_MOD_CONTROLLER_LOWER_BOUND;

  /* aftertouch vibrato */
  if ((p->aftertouch_vibrato >= PATCH_MOD_CONTROLLER_LOWER_BOUND) && 
      (p->aftertouch_vibrato <= PATCH_MOD_CONTROLLER_UPPER_BOUND))
  {
    l->aftertouch_vibrato = p->aftertouch_vibrato;
  }
  else
    l->aftertouch_vibrato = PATCH_MOD_CONTROLLER_LOWER_BOUND;

  /* aftertouch tremolo */
  if ((p->aftertouch_tremolo >= PATCH_MOD_CONTROLLER_LOWER_BOUND) && 
      (p->aftertouch_tremolo <= PATCH_MOD_CONTROLLER_UPPER_BOUND))
  {
    l->aftertouch_tremolo = p->aftertouch_tremolo;
  }
  else
    l->aftertouch_tremolo = PATCH_MOD_CONTROLLER_LOWER_BOUND;

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

  /* set delay cycles */
  l->delay_cycles = l->delay_period;

  /* reset phase if necessary */
  if (l->sync == 1)
    l->phase = 0;

  l->lfsr = 0x0001;

  /* initialize levels */
  l->vibrato_level = 0;
  l->tremolo_level = 0;

  return 0;
}

/*******************************************************************************
** lfo_update_all()
*******************************************************************************/
short int lfo_update_all()
{
  int k;

  lfo* l;

  unsigned int masked_phase;

  int wave_value_vibrato;
  int wave_value_tremolo;

  int vibrato_bound;
  int tremolo_bound;

  int vibrato_extra;
  int tremolo_extra;

  /* update all lfos */
  for (k = 0; k < BANK_NUM_VOICES; k++)
  {
    l = &G_lfo_bank[k];

    /* update delay cycles if necessary */
    if (l->delay_cycles > 0)
    {
      l->delay_cycles -= 1;

      l->vibrato_level = 0;
      l->tremolo_level = 0;

      continue;
    }

    /* update phase */
    l->phase += l->increment;

    /* wraparound phase register (28 bits) */
    if (l->phase > 0xFFFFFFF)
    {
      l->phase &= 0xFFFFFFF;

      if (l->waveform == 5)
      {
        /* update noise generator (nes) */
        /* 15-bit lfsr, taps on 1 and 2 */
        if ((l->lfsr & 0x0001) ^ ((l->lfsr & 0x0002) >> 1))
          l->lfsr = ((l->lfsr >> 1) & 0x3FFF) | 0x4000;
        else
          l->lfsr = (l->lfsr >> 1) & 0x3FFF;
      }
    }

    /* determine base wave index */
    masked_phase = ((l->phase >> 20) & 0xFF);

    /* waveform 1: triangle */
    if (l->waveform == 1)
    {
      wave_value_vibrato = S_lfo_wavetable_triangle[masked_phase];
      wave_value_tremolo = S_lfo_wavetable_triangle[(masked_phase + 192) % 256] + LFO_WAVE_AMPLITUDE;
    }
    /* waveform 2: square */
    else if (l->waveform == 2)
    {
      if (masked_phase < 128)
      {
        wave_value_vibrato = LFO_WAVE_AMPLITUDE;
        wave_value_tremolo = 2 * LFO_WAVE_AMPLITUDE;
      }
      else
      {
        wave_value_vibrato = -LFO_WAVE_AMPLITUDE;
        wave_value_tremolo = 0;
      }
    }
    /* waveform 3: sawtooth up */
    else if (l->waveform == 3)
    {
      wave_value_vibrato = S_lfo_wavetable_sawtooth[masked_phase];
      wave_value_tremolo = S_lfo_wavetable_sawtooth[(masked_phase + 128) % 256] + LFO_WAVE_AMPLITUDE;
    }
    /* waveform 4: sawtooth down */
    else if (l->waveform == 4)
    {
      wave_value_vibrato = -S_lfo_wavetable_sawtooth[masked_phase];
      wave_value_tremolo = -S_lfo_wavetable_sawtooth[(masked_phase + 128) % 256] + LFO_WAVE_AMPLITUDE;
    }
    /* waveform 5: noise */
    else if (l->waveform == 5)
    {
      if (l->lfsr & 0x0001)
      {
        wave_value_vibrato = LFO_WAVE_AMPLITUDE;
        wave_value_tremolo = 2 * LFO_WAVE_AMPLITUDE;
      }
      else
      {
        wave_value_vibrato = -LFO_WAVE_AMPLITUDE;
        wave_value_tremolo = 0;
      }
    }
    else
    {
      wave_value_vibrato = 0;
      wave_value_tremolo = 0;
    }

    /* update vibrato */
    l->vibrato_level = 
      (wave_value_vibrato * l->vibrato_depth * l->base_vibrato) / (LFO_WAVE_AMPLITUDE * PATCH_MOD_BASE_NUM_VALUES);

    vibrato_bound = (wave_value_vibrato * l->vibrato_depth) / LFO_WAVE_AMPLITUDE;
    vibrato_extra = 0;

    vibrato_extra += 
      (wave_value_vibrato * l->vibrato_depth * l->mod_wheel_vibrato * l->mod_wheel_input) / (LFO_WAVE_AMPLITUDE * PATCH_MOD_CONTROLLER_NUM_VALUES * LFO_MOD_WHEEL_STEPS);
    vibrato_extra += 
      (wave_value_vibrato * l->vibrato_depth * l->aftertouch_vibrato * l->aftertouch_input) / (LFO_WAVE_AMPLITUDE * PATCH_MOD_CONTROLLER_NUM_VALUES * LFO_AFTERTOUCH_STEPS);

    if ((vibrato_bound > 0) && (vibrato_extra > vibrato_bound))
      vibrato_extra = vibrato_bound;
    else if ((vibrato_bound < 0) && (vibrato_extra < vibrato_bound))
      vibrato_extra = vibrato_bound;

    l->vibrato_level += vibrato_extra;

    /* update tremolo */
    l->tremolo_level = 
      (wave_value_tremolo * l->tremolo_depth * l->base_tremolo) / (LFO_WAVE_AMPLITUDE * PATCH_MOD_BASE_NUM_VALUES);

    tremolo_bound = (wave_value_tremolo * l->tremolo_depth) / LFO_WAVE_AMPLITUDE;
    tremolo_extra = 0;

    tremolo_extra += 
      (wave_value_tremolo * l->tremolo_depth * l->mod_wheel_tremolo * l->mod_wheel_input) / (LFO_WAVE_AMPLITUDE * PATCH_MOD_CONTROLLER_NUM_VALUES * LFO_MOD_WHEEL_STEPS);
    tremolo_extra += 
      (wave_value_tremolo * l->tremolo_depth * l->aftertouch_tremolo * l->aftertouch_input) / (LFO_WAVE_AMPLITUDE * PATCH_MOD_CONTROLLER_NUM_VALUES * LFO_AFTERTOUCH_STEPS);

    if ((tremolo_bound > 0) && (tremolo_extra > tremolo_bound))
      tremolo_extra = tremolo_bound;
    else if ((tremolo_bound < 0) && (tremolo_extra < tremolo_bound))
      tremolo_extra = tremolo_bound;

    l->tremolo_level += tremolo_extra;
  }

  return 0;
}

/*******************************************************************************
** lfo_generate_tables()
*******************************************************************************/
short int lfo_generate_tables()
{
  int i;

  /* the lfo wavetables have 256 entries per period */

  /* wavetable (triangle) */
  S_lfo_wavetable_triangle[0] = 0;
  S_lfo_wavetable_triangle[64] = LFO_WAVE_AMPLITUDE;
  S_lfo_wavetable_triangle[128] = 0;
  S_lfo_wavetable_triangle[192] = -LFO_WAVE_AMPLITUDE;

  for (i = 1; i < 64; i++)
  {
    S_lfo_wavetable_triangle[i] = (short int) (((LFO_WAVE_AMPLITUDE * i) / 64.0f) + 0.5f);
    S_lfo_wavetable_triangle[128 - i] = S_lfo_wavetable_triangle[i];

    S_lfo_wavetable_triangle[128 + i] = -S_lfo_wavetable_triangle[i];
    S_lfo_wavetable_triangle[256 - i] = -S_lfo_wavetable_triangle[128 - i];
  }

  /* wavetable (sawtooth) */
  S_lfo_wavetable_sawtooth[0] = 0;
  S_lfo_wavetable_sawtooth[128] = -LFO_WAVE_AMPLITUDE;

  for (i = 1; i < 128; i++)
  {
    S_lfo_wavetable_sawtooth[i] = (short int) (((LFO_WAVE_AMPLITUDE * i) / 128.0f) + 0.5f);
    S_lfo_wavetable_sawtooth[256 - i] = -S_lfo_wavetable_sawtooth[i];
  }

  /* delay period table */
  for (i = 0; i < PATCH_LFO_DELAY_NUM_VALUES; i++)
  {
    S_lfo_delay_period_table[i] = 
      (int) ((S_lfo_delay_seconds_table[i] * CLOCK_SAMPLING_RATE) + 0.5f);
  }

  /* phase increment tables */
  for (i = 0; i < PATCH_LFO_FREQUENCY_NUM_VALUES; i++)
  {
    S_lfo_wave_phase_increment_table[i] = 
      (int) ((S_lfo_wave_frequency_table[i] * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_lfo_noise_phase_increment_table[i] = 
      (int) ((S_lfo_noise_frequency_table[i] * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
  }

#if 0
  /* print out triangle wavetable */
  for (i = 0; i < 256; i++)
  {
    printf( "LFO Triangle Wavetable Index %d: %d \n", 
            i, S_lfo_wavetable_triangle[i]);
  }

  /* print out sawtooth wavetable */
  for (i = 0; i < 256; i++)
  {
    printf( "LFO Sawtooth Wavetable Index %d: %d \n", 
            i, S_lfo_wavetable_sawtooth[i]);
  }
#endif

  return 0;
}

