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
#include "wheel.h"

#define PI      3.14159265358979323846f
#define TWO_PI  6.28318530717958647693f

#define LFO_WAVE_AMPLITUDE      128
#define LFO_WAVE_AMPLITUDE_HALF (LFO_WAVE_AMPLITUDE / 2)

#define LFO_BASE_NOISE_FREQUENCY  293.333333333333333f /* D-4 */

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
              { 0.25f, 0.50f, 0.75f, 1.00f, 
                1.25f, 1.50f, 1.75f, 2.00f, 
                2.25f, 2.50f, 2.75f, 3.00f, 
                3.25f, 3.50f, 3.75f, 4.00f, 
                4.25f, 4.50f, 4.75f, 5.00f, 
                5.25f, 5.50f, 5.75f, 6.00f, 
                6.25f, 6.50f, 6.75f, 7.00f, 
                7.25f, 7.50f, 7.75f, 8.00f 
              };

/* note that the noise frequencies describe the frequency at which the lfsr is    */
/* updated, as opposed to the frequency of completing one "period" (256 updates). */
static float  S_lfo_noise_frequency_table[PATCH_LFO_FREQUENCY_NUM_VALUES] = 
              { (LFO_BASE_NOISE_FREQUENCY / 8.0f) * 1.75f,  /* C-2  */
                LFO_BASE_NOISE_FREQUENCY / 4.0f,            /* D-2  */
                (LFO_BASE_NOISE_FREQUENCY / 4.0f) * 1.25f,  /* F-2  */
                (LFO_BASE_NOISE_FREQUENCY / 4.0f) * 1.5f,   /* A-2  */
                (LFO_BASE_NOISE_FREQUENCY / 4.0f) * 1.75f,  /* C-3  */
                LFO_BASE_NOISE_FREQUENCY / 2.0f,            /* D-3  */
                (LFO_BASE_NOISE_FREQUENCY / 2.0f) * 1.25f,  /* F-3  */
                (LFO_BASE_NOISE_FREQUENCY / 2.0f) * 1.5f,   /* A-3  */
                (LFO_BASE_NOISE_FREQUENCY / 2.0f) * 1.75f,  /* C-4  */
                LFO_BASE_NOISE_FREQUENCY * 1.0f,            /* D-4  */
                (LFO_BASE_NOISE_FREQUENCY * 1.0f) * 1.25f,  /* F-4  */
                (LFO_BASE_NOISE_FREQUENCY * 1.0f) * 1.5f,   /* A-4  */
                (LFO_BASE_NOISE_FREQUENCY * 1.0f) * 1.75f,  /* C-5  */
                LFO_BASE_NOISE_FREQUENCY * 2.0f,            /* D-5  */
                (LFO_BASE_NOISE_FREQUENCY * 2.0f) * 1.25f,  /* F-5  */
                (LFO_BASE_NOISE_FREQUENCY * 2.0f) * 1.5f,   /* A-5  */
                (LFO_BASE_NOISE_FREQUENCY * 2.0f) * 1.75f,  /* C-6  */
                LFO_BASE_NOISE_FREQUENCY * 4.0f,            /* D-6  */
                (LFO_BASE_NOISE_FREQUENCY * 4.0f) * 1.25f,  /* F-6  */
                (LFO_BASE_NOISE_FREQUENCY * 4.0f) * 1.5f,   /* A-6  */
                (LFO_BASE_NOISE_FREQUENCY * 4.0f) * 1.75f,  /* C-7  */
                LFO_BASE_NOISE_FREQUENCY * 8.0f,            /* D-7  */
                (LFO_BASE_NOISE_FREQUENCY * 8.0f) * 1.25f,  /* F-7  */
                (LFO_BASE_NOISE_FREQUENCY * 8.0f) * 1.5f,   /* A-7  */
                (LFO_BASE_NOISE_FREQUENCY * 8.0f) * 1.75f,  /* C-8  */
                LFO_BASE_NOISE_FREQUENCY * 16.0f,           /* D-8  */
                (LFO_BASE_NOISE_FREQUENCY * 16.0f) * 1.25f, /* F-8  */
                (LFO_BASE_NOISE_FREQUENCY * 16.0f) * 1.5f,  /* A-8  */
                (LFO_BASE_NOISE_FREQUENCY * 16.0f) * 1.75f, /* C-9  */
                LFO_BASE_NOISE_FREQUENCY * 32.0f,           /* D-9  */
                (LFO_BASE_NOISE_FREQUENCY * 32.0f) * 1.25f, /* F-9  */
                (LFO_BASE_NOISE_FREQUENCY * 32.0f) * 1.5f   /* A-9  */
              };

/* delay table */
static float  S_lfo_delay_seconds_table[PATCH_LFO_DELAY_NUM_VALUES] = 
              { 0.0f,               /*  0/48  */
                0.020833333333333f, /*  1/48  */
                0.041666666666667f, /*  2/48  */
                0.0625f,            /*  3/48  */
                0.083333333333333f, /*  4/48  */
                0.104166666666667f, /*  5/48  */
                0.125f,             /*  6/48  */
                0.145833333333333f, /*  7/48  */
                0.166666666666667f, /*  8/48  */
                0.1875f,            /*  9/48  */
                0.208333333333333f, /* 10/48  */
                0.229166666666667f, /* 11/48  */
                0.25f,              /* 12/48  */
                0.270833333333333f, /* 13/48  */
                0.291666666666667f, /* 14/48  */
                0.3125f,            /* 15/48  */
                0.333333333333333f, /* 16/48  */
                0.354166666666667f, /* 17/48  */
                0.375f,             /* 18/48  */
                0.395833333333333f, /* 19/48  */
                0.416666666666667f, /* 20/48  */
                0.4375f,            /* 21/48  */
                0.458333333333333f, /* 22/48  */
                0.479166666666667f, /* 23/48  */
                0.5f                /* 24/48  */
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
  int m;

  lfo* l;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  for (m = 0; m < BANK_LFOS_PER_VOICE; m++)
  {
    /* obtain lfo pointer */
    l = &G_lfo_bank[BANK_LFOS_PER_VOICE * voice_index + m];

    /* initialize lfo variables */
    l->waveform = 1;
    l->sync = 1;

    l->base_vibrato = 0;
    l->base_tremolo = 0;

    l->vibrato_depth = S_lfo_vibrato_depth_table[0];
    l->tremolo_depth = S_lfo_tremolo_depth_table[0];

    l->mod_wheel_vibrato = 0;
    l->mod_wheel_tremolo = 0;

    l->aftertouch_vibrato = 0;
    l->aftertouch_tremolo = 0;

    l->delay_period = 0;
    l->delay_cycles = 0;

    l->phase = 0;
    l->increment = 0;

    l->lfsr = 0x0001;

    l->mod_wheel_input = 0;
    l->aftertouch_input = 0;

    l->vibrato_level = 0;
    l->tremolo_level = 0;
  }

  return 0;
}

/*******************************************************************************
** lfo_load_patch()
*******************************************************************************/
short int lfo_load_patch(int voice_index, int patch_index)
{
  int m;

  lfo* l;
  patch* p;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain patch pointer */
  p = &G_patch_bank[patch_index];

  for (m = 0; m < BANK_LFOS_PER_VOICE; m++)
  {
    /* obtain lfo pointer */
    l = &G_lfo_bank[BANK_LFOS_PER_VOICE * voice_index + m];

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

    /* delay */
    if ((p->lfo_delay >= PATCH_LFO_DELAY_LOWER_BOUND) && 
        (p->lfo_delay <= PATCH_LFO_DELAY_UPPER_BOUND))
    {
      l->delay_period = S_lfo_delay_period_table[p->lfo_delay - PATCH_LFO_DELAY_LOWER_BOUND];
    }
    else
      l->delay_period = S_lfo_delay_period_table[0];

    /* vibrato mode */
    if ((p->lfo_vibrato_mode >= PATCH_LFO_VIBRATO_MODE_LOWER_BOUND) && 
        (p->lfo_vibrato_mode <= PATCH_LFO_VIBRATO_MODE_UPPER_BOUND))
    {
      l->vibrato_mode = p->lfo_vibrato_mode;
    }
    else
      l->vibrato_mode = PATCH_LFO_VIBRATO_MODE_LOWER_BOUND;

    /* sync */
    if ((p->lfo_sync >= PATCH_LFO_SYNC_LOWER_BOUND) && 
        (p->lfo_sync <= PATCH_LFO_SYNC_UPPER_BOUND))
    {
      l->sync = p->lfo_sync;
    }
    else
      l->sync = PATCH_LFO_SYNC_LOWER_BOUND;

    /* tempo */
    if ((p->lfo_tempo >= PATCH_LFO_TEMPO_LOWER_BOUND) && 
        (p->lfo_tempo <= PATCH_LFO_TEMPO_UPPER_BOUND))
    {
      l->tempo = p->lfo_tempo;
    }
    else
      l->tempo = PATCH_LFO_TEMPO_LOWER_BOUND;

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
  }

  return 0;
}

/*******************************************************************************
** lfo_trigger()
*******************************************************************************/
short int lfo_trigger(int voice_index)
{
  int m;

  lfo* l;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  for (m = 0; m < BANK_LFOS_PER_VOICE; m++)
  {
    /* obtain lfo pointer */
    l = &G_lfo_bank[BANK_LFOS_PER_VOICE * voice_index + m];

    /* set delay cycles */
    l->delay_cycles = l->delay_period;

    /* reset phase if necessary */
    if (l->sync == 1)
    {
      l->phase = 0;
      l->lfsr = 0x0001;
    }

    /* initialize levels */
    l->vibrato_level = 0;
    l->tremolo_level = 0;
  }

  return 0;
}

/*******************************************************************************
** lfo_update_all()
*******************************************************************************/
short int lfo_update_all()
{
  int k;
  int m;

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
    for (m = 0; m < BANK_LFOS_PER_VOICE; m++)
    {
      /* obtain lfo pointer */
      l = &G_lfo_bank[BANK_LFOS_PER_VOICE * k + m];

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
        if (l->vibrato_mode == 1)
          wave_value_vibrato = (S_lfo_wavetable_triangle[(masked_phase + 192) % 256] + LFO_WAVE_AMPLITUDE) / 2;
        else
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

        if (l->vibrato_mode == 1)
          wave_value_vibrato = (wave_value_vibrato + LFO_WAVE_AMPLITUDE) / 2;
      }
      /* waveform 3: sawtooth up */
      else if (l->waveform == 3)
      {
        if (l->vibrato_mode == 1)
          wave_value_vibrato = (S_lfo_wavetable_sawtooth[(masked_phase + 128) % 256] + LFO_WAVE_AMPLITUDE) / 2;
        else
          wave_value_vibrato = S_lfo_wavetable_sawtooth[masked_phase];

        wave_value_tremolo = S_lfo_wavetable_sawtooth[(masked_phase + 128) % 256] + LFO_WAVE_AMPLITUDE;
      }
      /* waveform 4: sawtooth down */
      else if (l->waveform == 4)
      {
        if (l->vibrato_mode == 1)
          wave_value_vibrato = (-S_lfo_wavetable_sawtooth[(masked_phase + 128) % 256] + LFO_WAVE_AMPLITUDE) / 2;
        else
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

        if (l->vibrato_mode == 1)
          wave_value_vibrato = (wave_value_vibrato + LFO_WAVE_AMPLITUDE) / 2;
      }
      else
      {
        wave_value_vibrato = 0;
        wave_value_tremolo = 0;
      }

      /* update vibrato */
      l->vibrato_level = 
        (wave_value_vibrato * l->vibrato_depth * l->base_vibrato) / (LFO_WAVE_AMPLITUDE * PATCH_MOD_BASE_NUM_VALUES);

      vibrato_bound = 
        (wave_value_vibrato * l->vibrato_depth * (PATCH_MOD_BASE_UPPER_BOUND - l->base_vibrato)) / (LFO_WAVE_AMPLITUDE * PATCH_MOD_BASE_NUM_VALUES);
      vibrato_extra = 0;

      vibrato_extra += 
        (vibrato_bound * l->mod_wheel_vibrato * l->mod_wheel_input) / (PATCH_MOD_CONTROLLER_NUM_VALUES * WHEEL_MOD_WHEEL_NUM_STEPS);
      vibrato_extra += 
        (vibrato_bound * l->aftertouch_vibrato * l->aftertouch_input) / (PATCH_MOD_CONTROLLER_NUM_VALUES * WHEEL_AFTERTOUCH_NUM_STEPS);

      if ((vibrato_bound > 0) && (vibrato_extra > vibrato_bound))
        vibrato_extra = vibrato_bound;
      else if ((vibrato_bound < 0) && (vibrato_extra < vibrato_bound))
        vibrato_extra = vibrato_bound;

      l->vibrato_level += vibrato_extra;

      /* update tremolo */
      l->tremolo_level = 
        (wave_value_tremolo * l->tremolo_depth * l->base_tremolo) / (LFO_WAVE_AMPLITUDE * PATCH_MOD_BASE_NUM_VALUES);

      tremolo_bound = 
        (wave_value_tremolo * l->tremolo_depth * (PATCH_MOD_BASE_UPPER_BOUND - l->base_tremolo)) / (LFO_WAVE_AMPLITUDE * PATCH_MOD_BASE_NUM_VALUES);
      tremolo_extra = 0;

      tremolo_extra += 
        (tremolo_bound * l->mod_wheel_tremolo * l->mod_wheel_input) / (PATCH_MOD_CONTROLLER_NUM_VALUES * WHEEL_MOD_WHEEL_NUM_STEPS);
      tremolo_extra += 
        (tremolo_bound * l->aftertouch_tremolo * l->aftertouch_input) / (PATCH_MOD_CONTROLLER_NUM_VALUES * WHEEL_AFTERTOUCH_NUM_STEPS);

      if ((tremolo_bound > 0) && (tremolo_extra > tremolo_bound))
        tremolo_extra = tremolo_bound;
      else if ((tremolo_bound < 0) && (tremolo_extra < tremolo_bound))
        tremolo_extra = tremolo_bound;

      l->tremolo_level += tremolo_extra;
    }
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

