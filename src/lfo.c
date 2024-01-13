/*******************************************************************************
** lfo.c (low frequency oscillator)
*******************************************************************************/

#include <stdio.h>    /* testing */
#include <stdlib.h>
#include <math.h>

#include "bank.h"
#include "clock.h"
#include "lfo.h"
#include "midicont.h"
#include "patch.h"
#include "tempo.h"
#include "tuning.h"

#define PI      3.14159265358979323846f
#define TWO_PI  6.28318530717958647693f

#define LFO_WAVE_AMPLITUDE 128

#define LFO_BASE_NOISE_FREQUENCY 440.0f /* A-4 */

#define LFO_TREMOLO_DEPTH_STEP 12

/* vibrato depth table */
/* assuming 128 steps per semitone */
static short int  S_lfo_vibrato_depth_table[PATCH_EFFECT_DEPTH_NUM_VALUES] = 
                  { (  5.0f / 100.0f) * TUNING_NUM_SEMITONE_STEPS, 
                    (  7.5f / 100.0f) * TUNING_NUM_SEMITONE_STEPS, 
                    ( 10.0f / 100.0f) * TUNING_NUM_SEMITONE_STEPS, 
                    ( 15.0f / 100.0f) * TUNING_NUM_SEMITONE_STEPS, 
                    ( 20.0f / 100.0f) * TUNING_NUM_SEMITONE_STEPS, 
                    ( 30.0f / 100.0f) * TUNING_NUM_SEMITONE_STEPS, 
                    ( 40.0f / 100.0f) * TUNING_NUM_SEMITONE_STEPS, 
                    ( 50.0f / 100.0f) * TUNING_NUM_SEMITONE_STEPS, 
                    ( 60.0f / 100.0f) * TUNING_NUM_SEMITONE_STEPS, 
                    ( 70.0f / 100.0f) * TUNING_NUM_SEMITONE_STEPS, 
                    ( 80.0f / 100.0f) * TUNING_NUM_SEMITONE_STEPS, 
                    ( 90.0f / 100.0f) * TUNING_NUM_SEMITONE_STEPS, 
                    (100.0f / 100.0f) * TUNING_NUM_SEMITONE_STEPS, 
                    (200.0f / 100.0f) * TUNING_NUM_SEMITONE_STEPS, 
                    (400.0f / 100.0f) * TUNING_NUM_SEMITONE_STEPS, 
                    (700.0f / 100.0f) * TUNING_NUM_SEMITONE_STEPS 
                  };

/* tremolo depth table */
static short int  S_lfo_tremolo_depth_table[PATCH_EFFECT_DEPTH_NUM_VALUES] = 
                  { LFO_TREMOLO_DEPTH_STEP *  1, 
                    LFO_TREMOLO_DEPTH_STEP *  2, 
                    LFO_TREMOLO_DEPTH_STEP *  3, 
                    LFO_TREMOLO_DEPTH_STEP *  4, 
                    LFO_TREMOLO_DEPTH_STEP *  5, 
                    LFO_TREMOLO_DEPTH_STEP *  6, 
                    LFO_TREMOLO_DEPTH_STEP *  7, 
                    LFO_TREMOLO_DEPTH_STEP *  8, 
                    LFO_TREMOLO_DEPTH_STEP *  9, 
                    LFO_TREMOLO_DEPTH_STEP * 10, 
                    LFO_TREMOLO_DEPTH_STEP * 11, 
                    LFO_TREMOLO_DEPTH_STEP * 12, 
                    LFO_TREMOLO_DEPTH_STEP * 13, 
                    LFO_TREMOLO_DEPTH_STEP * 14, 
                    LFO_TREMOLO_DEPTH_STEP * 15, 
                    LFO_TREMOLO_DEPTH_STEP * 16 
                  };

/* wave frequency table (in cycles per beat)    */
/* at 120 bpm, the wave frequencies range from  */
/* 0.25 hz to 8 hz (counting up by 0.25 hz)     */
static float  S_lfo_wave_frequency_table[PATCH_LFO_FREQUENCY_NUM_VALUES] = 
              { 0.125f, 0.25f, 0.375f, 0.5f, 
                0.625f, 0.75f, 0.875f, 1.0f, 
                1.125f, 1.25f, 1.375f, 1.5f, 
                1.625f, 1.75f, 1.875f, 2.0f, 
                2.125f, 2.25f, 2.375f, 2.5f, 
                2.625f, 2.75f, 2.875f, 3.0f, 
                3.125f, 3.25f, 3.375f, 3.5f, 
                3.625f, 3.75f, 3.875f, 4.0f 
              };

/* noise frequency table (in periods per second) */
static float  S_lfo_noise_frequency_table[PATCH_LFO_FREQUENCY_NUM_VALUES] = 
              { LFO_BASE_NOISE_FREQUENCY * 1, 
                LFO_BASE_NOISE_FREQUENCY * 1 * 1.25f, 
                LFO_BASE_NOISE_FREQUENCY * 1 * 1.50f, 
                LFO_BASE_NOISE_FREQUENCY * 1 * 1.75f, 
                LFO_BASE_NOISE_FREQUENCY * 2, 
                LFO_BASE_NOISE_FREQUENCY * 2 * 1.25f, 
                LFO_BASE_NOISE_FREQUENCY * 2 * 1.50f, 
                LFO_BASE_NOISE_FREQUENCY * 2 * 1.75f, 
                LFO_BASE_NOISE_FREQUENCY * 4, 
                LFO_BASE_NOISE_FREQUENCY * 4 * 1.125f, 
                LFO_BASE_NOISE_FREQUENCY * 4 * 1.250f, 
                LFO_BASE_NOISE_FREQUENCY * 4 * 1.375f, 
                LFO_BASE_NOISE_FREQUENCY * 4 * 1.500f, 
                LFO_BASE_NOISE_FREQUENCY * 4 * 1.625f, 
                LFO_BASE_NOISE_FREQUENCY * 4 * 1.750f, 
                LFO_BASE_NOISE_FREQUENCY * 4 * 1.875f, 
                LFO_BASE_NOISE_FREQUENCY * 8, 
                LFO_BASE_NOISE_FREQUENCY * 8 * 1.125f, 
                LFO_BASE_NOISE_FREQUENCY * 8 * 1.250f, 
                LFO_BASE_NOISE_FREQUENCY * 8 * 1.375f, 
                LFO_BASE_NOISE_FREQUENCY * 8 * 1.500f, 
                LFO_BASE_NOISE_FREQUENCY * 8 * 1.625f, 
                LFO_BASE_NOISE_FREQUENCY * 8 * 1.750f, 
                LFO_BASE_NOISE_FREQUENCY * 8 * 1.875f, 
                LFO_BASE_NOISE_FREQUENCY * 16, 
                LFO_BASE_NOISE_FREQUENCY * 16 * 1.125f, 
                LFO_BASE_NOISE_FREQUENCY * 16 * 1.250f, 
                LFO_BASE_NOISE_FREQUENCY * 16 * 1.375f, 
                LFO_BASE_NOISE_FREQUENCY * 16 * 1.500f, 
                LFO_BASE_NOISE_FREQUENCY * 16 * 1.625f, 
                LFO_BASE_NOISE_FREQUENCY * 16 * 1.750f, 
                LFO_BASE_NOISE_FREQUENCY * 16 * 1.875f 
              };

/* quantize (sample and hold) frequency table (in samples per wave period) */
static float  S_lfo_quantize_frequency_table[PATCH_LFO_QUANTIZE_NUM_VALUES] = 
              {  0.5625f, 0.625f, 0.6875f, 0.75f, 0.8125f, 0.875f, 0.9375f, 
                  1.125f,  1.25f,  1.375f,  1.5f,  1.625f,  1.75f,  1.875f, 2.0f, 
                   2.25f,  2.50f,   2.75f,  3.0f,   3.25f,  3.50f,   3.75f, 4.0f, 
                    4.5f,   5.0f,    5.5f,  6.0f,    6.5f,   7.0f,    7.5f, 8.0f, 
                  256.0f 
              };

/* wavetables */
static short int S_lfo_wavetable_triangle[256];
static short int S_lfo_wavetable_sawtooth[256];

/* delay table */

/* at 120 bpm, the delays range from      */
/* 0.0 s to 0.5 s (counting up by 1/48 s) */
static int S_lfo_delay_period_table[TEMPO_NUM_VALUES][PATCH_LFO_DELAY_NUM_VALUES];

/* phase increment tables */
static unsigned int S_lfo_wave_phase_increment_table[TEMPO_NUM_VALUES][PATCH_LFO_FREQUENCY_NUM_VALUES];
static unsigned int S_lfo_noise_phase_increment_table[PATCH_LFO_FREQUENCY_NUM_VALUES];

/* quantize phase overflow table */
static unsigned int S_lfo_quantize_overflow_table[PATCH_LFO_QUANTIZE_NUM_VALUES];

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

  /* initialize lfo variables */
  l->waveform = PATCH_LFO_WAVEFORM_DEFAULT;
  l->frequency = PATCH_LFO_FREQUENCY_DEFAULT;
  l->sync = PATCH_SYNC_DEFAULT;

  l->delay_cycles = 0;
  l->delay_period = 
    S_lfo_delay_period_table[TEMPO_DEFAULT - TEMPO_LOWER_BOUND][PATCH_LFO_DELAY_DEFAULT - PATCH_LFO_DELAY_LOWER_BOUND];

  l->quantize_phase = 0;
  l->quantize_overflow = 
    S_lfo_quantize_overflow_table[PATCH_LFO_QUANTIZE_DEFAULT - PATCH_LFO_QUANTIZE_LOWER_BOUND];

  l->phase = 0;
  l->increment = 
    S_lfo_wave_phase_increment_table[TEMPO_DEFAULT - TEMPO_LOWER_BOUND][PATCH_LFO_FREQUENCY_DEFAULT - PATCH_LFO_FREQUENCY_LOWER_BOUND];

  l->lfsr = 0x0001;

  l->tempo = TEMPO_DEFAULT;

  l->vibrato_depth = PATCH_EFFECT_DEPTH_DEFAULT;
  l->vibrato_base = PATCH_EFFECT_BASE_DEFAULT;
  l->vibrato_mode = PATCH_VIBRATO_MODE_DEFAULT;

  l->tremolo_depth = PATCH_EFFECT_DEPTH_DEFAULT;
  l->tremolo_base = PATCH_EFFECT_BASE_DEFAULT;

  l->mod_wheel_effect = PATCH_CONTROLLER_EFFECT_VIBRATO;
  l->aftertouch_effect = PATCH_CONTROLLER_EFFECT_VIBRATO;

  l->mod_wheel_input = 0;
  l->aftertouch_input = 0;

  l->vibrato_wave_value = 0;
  l->tremolo_wave_value = 0;

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

  /* obtain patch pointer */
  p = &G_patch_bank[patch_index];

  /* obtain lfo pointer */
  l = &G_lfo_bank[voice_index];

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
    l->frequency = p->lfo_frequency;
  }
  else
    l->frequency = PATCH_LFO_FREQUENCY_LOWER_BOUND;

  /* set increment based on frequency */
  if ((p->lfo_waveform == PATCH_LFO_WAVEFORM_NOISE_SQUARE) || 
      (p->lfo_waveform == PATCH_LFO_WAVEFORM_NOISE_SAW))
  {
    l->increment = S_lfo_noise_phase_increment_table[l->frequency - PATCH_LFO_FREQUENCY_LOWER_BOUND];
  }
  else
    l->increment = S_lfo_wave_phase_increment_table[l->tempo - TEMPO_LOWER_BOUND][l->frequency - PATCH_LFO_FREQUENCY_LOWER_BOUND];

  /* sync */
  if ((p->sync_lfo >= PATCH_SYNC_LOWER_BOUND) && 
      (p->sync_lfo <= PATCH_SYNC_UPPER_BOUND))
  {
    l->sync = p->sync_lfo;
  }
  else
    l->sync = PATCH_SYNC_LOWER_BOUND;

  /* delay */
  if ((p->lfo_delay >= PATCH_LFO_DELAY_LOWER_BOUND) && 
      (p->lfo_delay <= PATCH_LFO_DELAY_UPPER_BOUND))
  {
    l->delay_period = S_lfo_delay_period_table[l->tempo - TEMPO_LOWER_BOUND][p->lfo_delay - PATCH_LFO_DELAY_LOWER_BOUND];
  }
  else
    l->delay_period = S_lfo_delay_period_table[l->tempo - TEMPO_LOWER_BOUND][0];

  /* quantize */
  if ((p->lfo_quantize >= PATCH_LFO_QUANTIZE_LOWER_BOUND) && 
      (p->lfo_quantize <= PATCH_LFO_QUANTIZE_UPPER_BOUND))
  {
    l->quantize_overflow = S_lfo_quantize_overflow_table[p->lfo_quantize - PATCH_LFO_QUANTIZE_LOWER_BOUND];
  }
  else
    l->quantize_overflow = S_lfo_quantize_overflow_table[PATCH_LFO_QUANTIZE_UPPER_BOUND - PATCH_LFO_QUANTIZE_LOWER_BOUND];

  /* vibrato */
  if ((p->vibrato_depth >= PATCH_EFFECT_DEPTH_LOWER_BOUND) && 
      (p->vibrato_depth <= PATCH_EFFECT_DEPTH_UPPER_BOUND))
  {
    l->vibrato_depth = p->vibrato_depth;
  }
  else
    l->vibrato_depth = PATCH_EFFECT_DEPTH_LOWER_BOUND;

  if ((p->vibrato_base >= PATCH_EFFECT_BASE_LOWER_BOUND) && 
      (p->vibrato_base <= PATCH_EFFECT_BASE_UPPER_BOUND))
  {
    l->vibrato_base = p->vibrato_base;
  }
  else
    l->vibrato_base = PATCH_EFFECT_BASE_LOWER_BOUND;

  if ((p->vibrato_mode >= PATCH_VIBRATO_MODE_LOWER_BOUND) && 
      (p->vibrato_mode <= PATCH_VIBRATO_MODE_UPPER_BOUND))
  {
    l->vibrato_mode = p->vibrato_mode;
  }
  else
    l->vibrato_mode = PATCH_VIBRATO_MODE_LOWER_BOUND;

  /* tremolo */
  if ((p->tremolo_depth >= PATCH_EFFECT_DEPTH_LOWER_BOUND) && 
      (p->tremolo_depth <= PATCH_EFFECT_DEPTH_UPPER_BOUND))
  {
    l->tremolo_depth = p->tremolo_depth;
  }
  else
    l->tremolo_depth = PATCH_EFFECT_DEPTH_LOWER_BOUND;

  if ((p->tremolo_base >= PATCH_EFFECT_BASE_LOWER_BOUND) && 
      (p->tremolo_base <= PATCH_EFFECT_BASE_UPPER_BOUND))
  {
    l->tremolo_base = p->tremolo_base;
  }
  else
    l->tremolo_base = PATCH_EFFECT_BASE_LOWER_BOUND;

  /* controller effects */
  if ((p->mod_wheel_effect >= PATCH_CONTROLLER_EFFECT_LOWER_BOUND) && 
      (p->mod_wheel_effect <= PATCH_CONTROLLER_EFFECT_UPPER_BOUND))
  {
    l->mod_wheel_effect = p->mod_wheel_effect;
  }
  else
    l->mod_wheel_effect = PATCH_CONTROLLER_EFFECT_LOWER_BOUND;

  if ((p->aftertouch_effect >= PATCH_CONTROLLER_EFFECT_LOWER_BOUND) && 
      (p->aftertouch_effect <= PATCH_CONTROLLER_EFFECT_UPPER_BOUND))
  {
    l->aftertouch_effect = p->aftertouch_effect;
  }
  else
    l->aftertouch_effect = PATCH_CONTROLLER_EFFECT_LOWER_BOUND;

  return 0;
}

/*******************************************************************************
** lfo_set_tempo()
*******************************************************************************/
short int lfo_set_tempo(int voice_index, short int tempo)
{
  lfo* l;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain lfo pointer */
  l = &G_lfo_bank[voice_index];

  /* set tempo */
  if (tempo < TEMPO_LOWER_BOUND)
    l->tempo = TEMPO_LOWER_BOUND;
  else if (tempo > TEMPO_UPPER_BOUND)
    l->tempo = TEMPO_UPPER_BOUND;
  else
    l->tempo = tempo;

  /* adjust phase increment based on tempo */
  if ((l->waveform != PATCH_LFO_WAVEFORM_NOISE_SQUARE) && 
      (l->waveform != PATCH_LFO_WAVEFORM_NOISE_SAW))
  {
    l->increment = S_lfo_wave_phase_increment_table[l->tempo - TEMPO_LOWER_BOUND][l->frequency - PATCH_LFO_FREQUENCY_LOWER_BOUND];
  }

  return 0;
}

/*******************************************************************************
** lfo_sync_phase()
*******************************************************************************/
short int lfo_sync_phase(int voice_index)
{
  lfo* l;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain lfo pointer */
  l = &G_lfo_bank[voice_index];

  /* reset phase if necessary */
  if (l->sync == PATCH_SYNC_ON)
  {
    l->phase = 0;
    l->quantize_phase = 0;
    l->lfsr = 0x0001;
  }

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

  /* initialize wave values */
  l->vibrato_wave_value = 0;
  l->tremolo_wave_value = 0;

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

  unsigned int vibrato_phase;
  unsigned int tremolo_phase;

  int vibrato_bound;
  int tremolo_bound;

  /* update all lfos */
  for (k = 0; k < BANK_NUM_VOICES; k++)
  {
    /* obtain lfo pointer */
    l = &G_lfo_bank[k];

    /* update delay cycles if necessary */
    if (l->delay_cycles > 0)
    {
      l->delay_cycles -= 1;

      continue;
    }

    /* update phase */
    l->phase += l->increment;

    /* wraparound phase register (28 bits) */
    if (l->phase > 0xFFFFFFF)
    {
      l->phase &= 0xFFFFFFF;

      /* update noise if necessary */
      if ((l->waveform == PATCH_LFO_WAVEFORM_NOISE_SQUARE) || 
          (l->waveform == PATCH_LFO_WAVEFORM_NOISE_SAW))
      {
        /* update noise generator (nes) */
        /* 15-bit lfsr, taps on 1 and 2 */
        if ((l->lfsr & 0x0001) ^ ((l->lfsr & 0x0002) >> 1))
          l->lfsr = ((l->lfsr >> 1) & 0x3FFF) | 0x4000;
        else
          l->lfsr = (l->lfsr >> 1) & 0x3FFF;
      }
    }

    /* check if the new output bounds should be computed */
    l->quantize_phase += l->increment;

    if (l->quantize_phase >= l->quantize_overflow)
    {
      l->quantize_phase -= l->quantize_overflow;

      /* determine base wave indices */
      vibrato_phase = ((l->phase >> 20) & 0xFF);
      tremolo_phase = (vibrato_phase + 192) % 256;

      /* determine wave values */
      if (l->waveform == PATCH_LFO_WAVEFORM_TRIANGLE)
      {
        l->vibrato_wave_value = S_lfo_wavetable_triangle[vibrato_phase];
        l->tremolo_wave_value = S_lfo_wavetable_triangle[tremolo_phase];
      }
      else if (l->waveform == PATCH_LFO_WAVEFORM_SQUARE)
      {
        if (vibrato_phase < 128)
          l->vibrato_wave_value = LFO_WAVE_AMPLITUDE;
        else
          l->vibrato_wave_value = -LFO_WAVE_AMPLITUDE;

        if (tremolo_phase < 128)
          l->tremolo_wave_value = -LFO_WAVE_AMPLITUDE;
        else
          l->tremolo_wave_value = LFO_WAVE_AMPLITUDE;
      }
      else if (l->waveform == PATCH_LFO_WAVEFORM_SAW_UP)
      {
        l->vibrato_wave_value = S_lfo_wavetable_sawtooth[vibrato_phase];
        l->tremolo_wave_value = S_lfo_wavetable_sawtooth[tremolo_phase];
      }
      else if (l->waveform == PATCH_LFO_WAVEFORM_SAW_DOWN)
      {
        l->vibrato_wave_value = -S_lfo_wavetable_sawtooth[vibrato_phase];
        l->tremolo_wave_value = -S_lfo_wavetable_sawtooth[tremolo_phase];
      }
      else if (l->waveform == PATCH_LFO_WAVEFORM_NOISE_SQUARE)
      {
        if ((l->lfsr & 0x00FF) < 128)
        {
          l->vibrato_wave_value = LFO_WAVE_AMPLITUDE;
          l->tremolo_wave_value = -LFO_WAVE_AMPLITUDE;
        }
        else
        {
          l->vibrato_wave_value = -LFO_WAVE_AMPLITUDE;
          l->tremolo_wave_value = LFO_WAVE_AMPLITUDE;
        }
      }
      else if (l->waveform == PATCH_LFO_WAVEFORM_NOISE_SAW)
      {
        l->vibrato_wave_value = S_lfo_wavetable_sawtooth[l->lfsr & 0x00FF];
        l->tremolo_wave_value = S_lfo_wavetable_sawtooth[l->lfsr & 0x00FF];
      }
      else
      {
        l->vibrato_wave_value = 0;
        l->tremolo_wave_value = -LFO_WAVE_AMPLITUDE;
      }

      /* apply amplitude shift and scaling to vibrato wave based on mode */
      if (l->vibrato_mode == PATCH_VIBRATO_MODE_UP_ONLY)
        l->vibrato_wave_value = (l->vibrato_wave_value + LFO_WAVE_AMPLITUDE) / 2;

      /* apply amplitude shift and scaling to tremolo wave */
      l->tremolo_wave_value = (l->tremolo_wave_value + LFO_WAVE_AMPLITUDE) / 2;
    }

    /* determine vibrato & tremolo bounds */
    vibrato_bound = (l->vibrato_wave_value * S_lfo_vibrato_depth_table[l->vibrato_depth - PATCH_EFFECT_DEPTH_LOWER_BOUND]) / LFO_WAVE_AMPLITUDE;
    tremolo_bound = (l->tremolo_wave_value * S_lfo_tremolo_depth_table[l->tremolo_depth - PATCH_EFFECT_DEPTH_LOWER_BOUND]) / LFO_WAVE_AMPLITUDE;

    /* determine base vibrato & tremolo levels */
    l->vibrato_level = (vibrato_bound * l->vibrato_base) / PATCH_EFFECT_BASE_NUM_VALUES;
    l->tremolo_level = (tremolo_bound * l->tremolo_base) / PATCH_EFFECT_BASE_NUM_VALUES;

    /* apply mod wheel effect */
    if (l->mod_wheel_effect == PATCH_CONTROLLER_EFFECT_VIBRATO)
    {
      l->vibrato_level += 
        (vibrato_bound * (PATCH_EFFECT_BASE_UPPER_BOUND - l->vibrato_base) * l->mod_wheel_input) / 
        (PATCH_EFFECT_BASE_NUM_VALUES * MIDI_CONT_MOD_WHEEL_UPPER_BOUND);
    }
    else if (l->mod_wheel_effect == PATCH_CONTROLLER_EFFECT_TREMOLO)
    {
      l->tremolo_level += 
        (tremolo_bound * (PATCH_EFFECT_BASE_UPPER_BOUND - l->tremolo_base) * l->mod_wheel_input) / 
        (PATCH_EFFECT_BASE_NUM_VALUES * MIDI_CONT_MOD_WHEEL_UPPER_BOUND);
    }

    /* apply aftertouch effect */
    if (l->aftertouch_effect == PATCH_CONTROLLER_EFFECT_VIBRATO)
    {
      l->vibrato_level += 
        (vibrato_bound * (PATCH_EFFECT_BASE_UPPER_BOUND - l->vibrato_base) * l->aftertouch_input) / 
        (PATCH_EFFECT_BASE_NUM_VALUES * MIDI_CONT_AFTERTOUCH_UPPER_BOUND);
    }
    else if (l->aftertouch_effect == PATCH_CONTROLLER_EFFECT_TREMOLO)
    {
      l->tremolo_level += 
        (tremolo_bound * (PATCH_EFFECT_BASE_UPPER_BOUND - l->tremolo_base) * l->aftertouch_input) / 
        (PATCH_EFFECT_BASE_NUM_VALUES * MIDI_CONT_AFTERTOUCH_UPPER_BOUND);
    }

    /* bound vibrato & tremolo levels */
    if ((vibrato_bound > 0) && (l->vibrato_level > vibrato_bound))
      l->vibrato_level = vibrato_bound;
    else if ((vibrato_bound < 0) && (l->vibrato_level < vibrato_bound))
      l->vibrato_level = vibrato_bound;

    if (l->tremolo_level > tremolo_bound)
      l->tremolo_level = tremolo_bound;
  }

  return 0;
}

/*******************************************************************************
** lfo_generate_tables()
*******************************************************************************/
short int lfo_generate_tables()
{
  int k;
  int m;

  /* the lfo wavetables have 256 entries per period */

  /* wavetable (triangle) */
  S_lfo_wavetable_triangle[0] = 0;
  S_lfo_wavetable_triangle[64] = LFO_WAVE_AMPLITUDE;
  S_lfo_wavetable_triangle[128] = 0;
  S_lfo_wavetable_triangle[192] = -LFO_WAVE_AMPLITUDE;

  for (k = 1; k < 64; k++)
  {
    S_lfo_wavetable_triangle[k] = (short int) (((LFO_WAVE_AMPLITUDE * k) / 64.0f) + 0.5f);
    S_lfo_wavetable_triangle[128 - k] = S_lfo_wavetable_triangle[k];

    S_lfo_wavetable_triangle[128 + k] = -S_lfo_wavetable_triangle[k];
    S_lfo_wavetable_triangle[256 - k] = -S_lfo_wavetable_triangle[128 - k];
  }

  /* wavetable (sawtooth) */
  S_lfo_wavetable_sawtooth[0] = 0;
  S_lfo_wavetable_sawtooth[128] = -LFO_WAVE_AMPLITUDE;

  for (k = 1; k < 128; k++)
  {
    S_lfo_wavetable_sawtooth[k] = (short int) (((LFO_WAVE_AMPLITUDE * k) / 128.0f) + 0.5f);
    S_lfo_wavetable_sawtooth[256 - k] = -S_lfo_wavetable_sawtooth[k];
  }

  /* delay period table */
  for (k = 0; k < TEMPO_NUM_VALUES; k++)
  {
    for (m = 0; m < PATCH_LFO_DELAY_NUM_VALUES; m++)
    {
      S_lfo_delay_period_table[k][m] = 
        (int) ((TEMPO_COMPUTE_SECONDS_PER_BEAT(k + TEMPO_LOWER_BOUND) * (m / 24.0f) * CLOCK_SAMPLING_RATE) + 0.5f);
    }
  }

  /* phase increment tables */
  for (k = 0; k < TEMPO_NUM_VALUES; k++)
  {
    for (m = 0; m < PATCH_LFO_FREQUENCY_NUM_VALUES; m++)
    {
      S_lfo_wave_phase_increment_table[k][m] = 
        (int) ((TEMPO_COMPUTE_BEATS_PER_SECOND(k + TEMPO_LOWER_BOUND) * S_lfo_wave_frequency_table[m] * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
    }
  }

  for (m = 0; m < PATCH_LFO_FREQUENCY_NUM_VALUES; m++)
  {
    S_lfo_noise_phase_increment_table[m] = 
      (int) ((S_lfo_noise_frequency_table[m] * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
  }

  /* quantize phase overflow table */
  for (k = 0; k < PATCH_LFO_QUANTIZE_NUM_VALUES; k++)
  {
    S_lfo_quantize_overflow_table[k] = 
      (int) (((1 << 28) / S_lfo_quantize_frequency_table[k]) + 0.5f);
  }

#if 0
  /* print out triangle wavetable */
  for (k = 0; k < 256; k++)
  {
    printf( "LFO Triangle Wavetable Index %d: %d \n", 
            k, S_lfo_wavetable_triangle[k]);
  }

  /* print out sawtooth wavetable */
  for (k = 0; k < 256; k++)
  {
    printf( "LFO Sawtooth Wavetable Index %d: %d \n", 
            k, S_lfo_wavetable_sawtooth[k]);
  }
#endif

#if 0
  /* print out delay periods for various tempos */
  printf("Delay Periods for Tempo 120 BPM:\n");

  for (m = 0; m < PATCH_LFO_DELAY_NUM_VALUES; m++)
  {
    printf( "  Delay %d: %d \n", 
            m, S_lfo_delay_period_table[120 - TEMPO_LOWER_BOUND][m]);
  }

  printf("Delay Periods for Tempo 180 BPM:\n");

  for (m = 0; m < PATCH_LFO_DELAY_NUM_VALUES; m++)
  {
    printf( "  Delay %d: %d \n", 
            m, S_lfo_delay_period_table[180 - TEMPO_LOWER_BOUND][m]);
  }

#endif

#if 0
  /* print out phase increments for various tempos */
  printf("Wave Frequencies for Tempo 120 BPM:\n");

  for (m = 0; m < PATCH_LFO_FREQUENCY_NUM_VALUES; m++)
  {
    printf( "  Freq %d: %d \n", 
            m, S_lfo_wave_phase_increment_table[120 - TEMPO_LOWER_BOUND][m]);
  }

  printf("Wave Frequencies for Tempo 180 BPM:\n");

  for (m = 0; m < PATCH_LFO_FREQUENCY_NUM_VALUES; m++)
  {
    printf( "  Freq %d: %d \n", 
            m, S_lfo_wave_phase_increment_table[180 - TEMPO_LOWER_BOUND][m]);
  }

#endif

#if 0
  /* print out quantize overflow table */
  for (k = 0; k < PATCH_LFO_QUANTIZE_NUM_VALUES; k++)
  {
    printf( "LFO Quantize Overflow Index %d: %d \n", 
            k, S_lfo_quantize_overflow_table[k]);
  }
#endif

  return 0;
}

