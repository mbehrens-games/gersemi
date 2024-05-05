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

#define LFO_TREMOLO_SENSITIVITY_STEP (1 * 32)

#define LFO_WAVE_AMPLITUDE 256

#define LFO_WAVETABLE_SIZE_FULL     256
#define LFO_WAVETABLE_SIZE_HALF     (LFO_WAVETABLE_SIZE_FULL / 2)
#define LFO_WAVETABLE_SIZE_QUARTER  (LFO_WAVETABLE_SIZE_FULL / 4)

/* wave frequencies (in cycles per beat)    */
/* at 120 bpm, the frequencies range from   */
/* 0.25 hz to 8 hz (counting up by 0.25 hz) */
#define LFO_WAVE_FREQUENCY_BASE 0.125f
#define LFO_WAVE_FREQUENCY_STEP 0.125f

/* vibrato depth table (in cents) */
static short int  S_lfo_vibrato_cents_table[PATCH_SENSITIVITY_NUM_VALUES] = 
                  {  4,   7,  10,  14,  17,  20,  25,  30, 
                    40,  50,  60,  70,  80,  90, 100, 200 
                  };

/* vibrato & tremolo amplitude tables */
static short int  S_lfo_vibrato_max_table[PATCH_SENSITIVITY_NUM_VALUES];
static short int  S_lfo_tremolo_max_table[PATCH_SENSITIVITY_NUM_VALUES];

/* noise note table */
static short int  S_lfo_noise_note_table[PATCH_LFO_SPEED_NUM_VALUES] = 
                  { TUNING_NOTE_A4 + 0 * 12 +  0, 
                    TUNING_NOTE_A4 + 0 * 12 +  4, 
                    TUNING_NOTE_A4 + 0 * 12 +  7, 
                    TUNING_NOTE_A4 + 0 * 12 + 10, 
                    TUNING_NOTE_A4 + 1 * 12 +  0, 
                    TUNING_NOTE_A4 + 1 * 12 +  4, 
                    TUNING_NOTE_A4 + 1 * 12 +  7, 
                    TUNING_NOTE_A4 + 1 * 12 + 10, 
                    TUNING_NOTE_A4 + 2 * 12 +  0, 
                    TUNING_NOTE_A4 + 2 * 12 +  2, 
                    TUNING_NOTE_A4 + 2 * 12 +  4, 
                    TUNING_NOTE_A4 + 2 * 12 +  6, 
                    TUNING_NOTE_A4 + 2 * 12 +  7, 
                    TUNING_NOTE_A4 + 2 * 12 +  8, 
                    TUNING_NOTE_A4 + 2 * 12 + 10, 
                    TUNING_NOTE_A4 + 2 * 12 + 11, 
                    TUNING_NOTE_A4 + 3 * 12 +  0, 
                    TUNING_NOTE_A4 + 3 * 12 +  2, 
                    TUNING_NOTE_A4 + 3 * 12 +  4, 
                    TUNING_NOTE_A4 + 3 * 12 +  6, 
                    TUNING_NOTE_A4 + 3 * 12 +  7, 
                    TUNING_NOTE_A4 + 3 * 12 +  8, 
                    TUNING_NOTE_A4 + 3 * 12 + 10, 
                    TUNING_NOTE_A4 + 3 * 12 + 11, 
                    TUNING_NOTE_A4 + 4 * 12 +  0, 
                    TUNING_NOTE_A4 + 4 * 12 +  2, 
                    TUNING_NOTE_A4 + 4 * 12 +  4, 
                    TUNING_NOTE_A4 + 4 * 12 +  6, 
                    TUNING_NOTE_A4 + 4 * 12 +  7, 
                    TUNING_NOTE_A4 + 4 * 12 +  8, 
                    TUNING_NOTE_A4 + 4 * 12 + 10, 
                    TUNING_NOTE_A4 + 4 * 12 + 11 
                  };

/* wavetables */
static short int S_lfo_wavetable_tri[LFO_WAVETABLE_SIZE_HALF];
static short int S_lfo_wavetable_saw[LFO_WAVETABLE_SIZE_FULL];

/* delay table */

/* at 120 bpm, the delays range from      */
/* 0.0 s to 0.5 s (counting up by 1/48 s) */
static int S_lfo_delay_period_table[TEMPO_NUM_VALUES][PATCH_LFO_DELAY_NUM_VALUES];

/* phase increment tables */
static unsigned int S_lfo_wave_phase_increment_table[TEMPO_NUM_VALUES][PATCH_LFO_SPEED_NUM_VALUES];
static unsigned int S_lfo_noise_phase_increment_table[PATCH_LFO_SPEED_NUM_VALUES];

/* lfo banks */
lfo G_vibrato_bank[BANK_NUM_VIBRATOS];
lfo G_tremolo_bank[BANK_NUM_TREMOLOS];

/*******************************************************************************
** lfo_reset_all()
*******************************************************************************/
short int lfo_reset_all()
{
  int k;

  lfo* l;

  /* reset all vibratos */
  for (k = 0; k < BANK_NUM_VIBRATOS; k++)
  {
    /* obtain vibrato pointer */
    l = &G_vibrato_bank[k];

    /* initialize vibrato variables */
    l->waveform = PATCH_VIBRATO_WAVEFORM_DEFAULT;
    l->frequency = PATCH_LFO_SPEED_DEFAULT;
    l->sync = PATCH_SYNC_DEFAULT;

    l->delay_cycles = 0;
    l->delay_period = 
      S_lfo_delay_period_table[TEMPO_DEFAULT - TEMPO_LOWER_BOUND][PATCH_LFO_DELAY_DEFAULT - PATCH_LFO_DELAY_LOWER_BOUND];

    l->phase = 0;
    l->increment = 
      S_lfo_wave_phase_increment_table[TEMPO_DEFAULT - TEMPO_LOWER_BOUND][PATCH_LFO_SPEED_DEFAULT - PATCH_LFO_SPEED_LOWER_BOUND];

    l->lfsr = 0x0001;

    l->tempo = TEMPO_DEFAULT;

    l->depth = PATCH_LFO_DEPTH_DEFAULT;
    l->max = 
      S_lfo_vibrato_max_table[PATCH_SENSITIVITY_DEFAULT - PATCH_SENSITIVITY_LOWER_BOUND];

    l->level_base = 0;
    l->level_extra = 0;
  }

  /* reset all tremolos */
  for (k = 0; k < BANK_NUM_TREMOLOS; k++)
  {
    /* obtain tremolo pointer */
    l = &G_tremolo_bank[k];

    /* initialize tremolo variables */
    l->waveform = PATCH_TREMOLO_WAVEFORM_DEFAULT;
    l->frequency = PATCH_LFO_SPEED_DEFAULT;
    l->sync = PATCH_SYNC_DEFAULT;

    l->delay_cycles = 0;
    l->delay_period = 
      S_lfo_delay_period_table[TEMPO_DEFAULT - TEMPO_LOWER_BOUND][PATCH_LFO_DELAY_DEFAULT - PATCH_LFO_DELAY_LOWER_BOUND];

    l->phase = 0;
    l->increment = 
      S_lfo_wave_phase_increment_table[TEMPO_DEFAULT - TEMPO_LOWER_BOUND][PATCH_LFO_SPEED_DEFAULT - PATCH_LFO_SPEED_LOWER_BOUND];

    l->lfsr = 0x0001;

    l->tempo = TEMPO_DEFAULT;

    l->depth = PATCH_LFO_DEPTH_DEFAULT;
    l->max = 
      S_lfo_tremolo_max_table[PATCH_SENSITIVITY_DEFAULT - PATCH_SENSITIVITY_LOWER_BOUND];

    l->level_base = 0;
    l->level_extra = 0;
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

  for (m = 0; m < BANK_VIBRATOS_PER_VOICE; m++)
  {
    /* obtain vibrato pointer */
    l = &G_vibrato_bank[voice_index];

#if 0
    /* waveform */
    if ((p->lfo_waveform[m] >= PATCH_LFO_WAVEFORM_LOWER_BOUND) && 
        (p->lfo_waveform[m] <= PATCH_LFO_WAVEFORM_UPPER_BOUND))
    {
      l->waveform = p->lfo_waveform[m];
    }
    else
      l->waveform = PATCH_LFO_WAVEFORM_LOWER_BOUND;

    /* frequency */
    if ((p->lfo_frequency[m] >= PATCH_LFO_FREQUENCY_LOWER_BOUND) && 
        (p->lfo_frequency[m] <= PATCH_LFO_FREQUENCY_UPPER_BOUND))
    {
      l->frequency = p->lfo_frequency[m];
    }
    else
      l->frequency = PATCH_LFO_FREQUENCY_LOWER_BOUND;

    /* set increment based on frequency */
    if ((p->lfo_waveform[m] == PATCH_LFO_WAVEFORM_NOISE_SQUARE) || 
        (p->lfo_waveform[m] == PATCH_LFO_WAVEFORM_NOISE_SAW))
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
    if ((p->lfo_delay[m] >= PATCH_LFO_DELAY_LOWER_BOUND) && 
        (p->lfo_delay[m] <= PATCH_LFO_DELAY_UPPER_BOUND))
    {
      l->delay_period = S_lfo_delay_period_table[l->tempo - TEMPO_LOWER_BOUND][p->lfo_delay[m] - PATCH_LFO_DELAY_LOWER_BOUND];
    }
    else
      l->delay_period = S_lfo_delay_period_table[l->tempo - TEMPO_LOWER_BOUND][0];

    /* depth */
    if ((p->lfo_base[m] >= PATCH_LFO_BASE_LOWER_BOUND) && 
        (p->lfo_base[m] <= PATCH_LFO_BASE_UPPER_BOUND))
    {
      l->depth = p->lfo_base[m];
    }
    else
      l->depth = PATCH_LFO_BASE_LOWER_BOUND;

    /* sensitivity */
    if ((p->lfo_depth[m] >= PATCH_LFO_DEPTH_LOWER_BOUND) && 
        (p->lfo_depth[m] <= PATCH_LFO_DEPTH_UPPER_BOUND))
    {
      l->max = 
        S_lfo_vibrato_max_table[p->lfo_depth[m] - PATCH_LFO_DEPTH_LOWER_BOUND];
    }
    else
    {
      l->max = 
        S_lfo_vibrato_max_table[PATCH_LFO_DEPTH_DEFAULT - PATCH_LFO_DEPTH_LOWER_BOUND];
    }
#endif
  }

  return 0;
}

/*******************************************************************************
** lfo_set_tempo()
*******************************************************************************/
short int lfo_set_tempo(int voice_index, short int tempo)
{
  int m;

  lfo* l;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  for (m = 0; m < BANK_VIBRATOS_PER_VOICE; m++)
  {
    /* obtain vibrato pointer */
    l = &G_vibrato_bank[voice_index];

#if 0
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
#endif
  }

  return 0;
}

/*******************************************************************************
** lfo_sync_to_key()
*******************************************************************************/
short int lfo_sync_to_key(int voice_index)
{
  int m;

  lfo* l;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  for (m = 0; m < BANK_VIBRATOS_PER_VOICE; m++)
  {
    /* obtain vibrato pointer */
    l = &G_vibrato_bank[voice_index];

    /* reset phase if necessary */
    if (l->sync == PATCH_SYNC_ON)
    {
      l->phase = 0;
      l->lfsr = 0x0001;
    }
  }

  return 0;
}

/*******************************************************************************
** lfo_sync_to_tempo()
*******************************************************************************/
short int lfo_sync_to_tempo(int voice_index)
{
  int m;

  lfo* l;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  for (m = 0; m < BANK_VIBRATOS_PER_VOICE; m++)
  {
    /* obtain vibrato pointer */
    l = &G_vibrato_bank[voice_index];

    /* reset phase if necessary */
    if (l->sync == PATCH_SYNC_OFF)
    {
      l->phase = 0;
      l->lfsr = 0x0001;
    }
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

  for (m = 0; m < BANK_VIBRATOS_PER_VOICE; m++)
  {
    /* obtain vibrato pointer */
    l = &G_vibrato_bank[voice_index];

#if 0
    /* set delay cycles */
    l->delay_cycles = l->delay_period;

    /* initialize wave values */
    l->vibrato_wave_value = 0;
    l->env_tremolo_wave_value = 0;
    l->peg_tremolo_wave_value = 0;

    /* initialize levels */
    l->vibrato_level = 0;
    l->env_tremolo_level = 0;
    l->peg_tremolo_level = 0;
#endif
  }

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

  /* update all vibratos */
  for (k = 0; k < BANK_NUM_VIBRATOS; k++)
  {
    /* obtain vibrato pointer */
    l = &G_vibrato_bank[k];

#if 0
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
#endif

#if 0
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

    /* determine vibrato & tremolo bounds */
    vibrato_bound = (l->vibrato_wave_value * S_lfo_vibrato_depth_table[l->vibrato_depth - PATCH_EFFECT_DEPTH_LOWER_BOUND]) / LFO_WAVE_AMPLITUDE;
    tremolo_bound = (l->tremolo_wave_value * S_lfo_tremolo_depth_table[l->tremolo_depth - PATCH_EFFECT_DEPTH_LOWER_BOUND]) / LFO_WAVE_AMPLITUDE;

    /* determine base vibrato & tremolo levels */
    l->vibrato_level = (vibrato_bound * l->vibrato_base) / PATCH_EFFECT_BASE_NUM_VALUES;
    l->tremolo_level = (tremolo_bound * l->tremolo_base) / PATCH_EFFECT_BASE_NUM_VALUES;

    /* apply modulation wheel effect */
    if ((l->mod_wheel_effect == PATCH_CONTROLLER_EFFECT_VIBRATO)        || 
        (l->mod_wheel_effect == PATCH_CONTROLLER_EFFECT_VIB_PLUS_TREM)  || 
        (l->mod_wheel_effect == PATCH_CONTROLLER_EFFECT_VIB_PLUS_BOOST) || 
        (l->mod_wheel_effect == PATCH_CONTROLLER_EFFECT_ALL_THREE))
    {
      l->vibrato_level += 
        (vibrato_bound * (PATCH_EFFECT_BASE_UPPER_BOUND - l->vibrato_base) * l->mod_wheel_input) / 
        (PATCH_EFFECT_BASE_NUM_VALUES * MIDI_CONT_MOD_WHEEL_UPPER_BOUND);
    }

    if ((l->mod_wheel_effect == PATCH_CONTROLLER_EFFECT_TREMOLO)          || 
        (l->mod_wheel_effect == PATCH_CONTROLLER_EFFECT_VIB_PLUS_TREM)    || 
        (l->mod_wheel_effect == PATCH_CONTROLLER_EFFECT_TREM_PLUS_BOOST)  || 
        (l->mod_wheel_effect == PATCH_CONTROLLER_EFFECT_ALL_THREE))
    {
      l->tremolo_level += 
        (tremolo_bound * (PATCH_EFFECT_BASE_UPPER_BOUND - l->tremolo_base) * l->mod_wheel_input) / 
        (PATCH_EFFECT_BASE_NUM_VALUES * MIDI_CONT_MOD_WHEEL_UPPER_BOUND);
    }

    /* apply aftertouch effect */
    if ((l->aftertouch_effect == PATCH_CONTROLLER_EFFECT_VIBRATO)         || 
        (l->aftertouch_effect == PATCH_CONTROLLER_EFFECT_VIB_PLUS_TREM)   || 
        (l->aftertouch_effect == PATCH_CONTROLLER_EFFECT_VIB_PLUS_BOOST)  || 
        (l->aftertouch_effect == PATCH_CONTROLLER_EFFECT_ALL_THREE))
    {
      l->vibrato_level += 
        (vibrato_bound * (PATCH_EFFECT_BASE_UPPER_BOUND - l->vibrato_base) * l->aftertouch_input) / 
        (PATCH_EFFECT_BASE_NUM_VALUES * MIDI_CONT_AFTERTOUCH_UPPER_BOUND);
    }

    if ((l->aftertouch_effect == PATCH_CONTROLLER_EFFECT_TREMOLO)         || 
        (l->aftertouch_effect == PATCH_CONTROLLER_EFFECT_VIB_PLUS_TREM)   || 
        (l->aftertouch_effect == PATCH_CONTROLLER_EFFECT_TREM_PLUS_BOOST) || 
        (l->aftertouch_effect == PATCH_CONTROLLER_EFFECT_ALL_THREE))
    {
      l->tremolo_level += 
        (tremolo_bound * (PATCH_EFFECT_BASE_UPPER_BOUND - l->tremolo_base) * l->aftertouch_input) / 
        (PATCH_EFFECT_BASE_NUM_VALUES * MIDI_CONT_AFTERTOUCH_UPPER_BOUND);
    }

    /* apply expression pedal effect */
    if ((l->exp_pedal_effect == PATCH_CONTROLLER_EFFECT_VIBRATO)         || 
        (l->exp_pedal_effect == PATCH_CONTROLLER_EFFECT_VIB_PLUS_TREM)   || 
        (l->exp_pedal_effect == PATCH_CONTROLLER_EFFECT_VIB_PLUS_BOOST)  || 
        (l->exp_pedal_effect == PATCH_CONTROLLER_EFFECT_ALL_THREE))
    {
      l->vibrato_level += 
        (vibrato_bound * (PATCH_EFFECT_BASE_UPPER_BOUND - l->vibrato_base) * l->exp_pedal_input) / 
        (PATCH_EFFECT_BASE_NUM_VALUES * MIDI_CONT_EXP_PEDAL_UPPER_BOUND);
    }

    if ((l->exp_pedal_effect == PATCH_CONTROLLER_EFFECT_TREMOLO)         || 
        (l->exp_pedal_effect == PATCH_CONTROLLER_EFFECT_VIB_PLUS_TREM)   || 
        (l->exp_pedal_effect == PATCH_CONTROLLER_EFFECT_TREM_PLUS_BOOST) || 
        (l->exp_pedal_effect == PATCH_CONTROLLER_EFFECT_ALL_THREE))
    {
      l->tremolo_level += 
        (tremolo_bound * (PATCH_EFFECT_BASE_UPPER_BOUND - l->tremolo_base) * l->exp_pedal_input) / 
        (PATCH_EFFECT_BASE_NUM_VALUES * MIDI_CONT_EXP_PEDAL_UPPER_BOUND);
    }

    /* bound vibrato & tremolo levels */
    if ((vibrato_bound > 0) && (l->vibrato_level > vibrato_bound))
      l->vibrato_level = vibrato_bound;
    else if ((vibrato_bound < 0) && (l->vibrato_level < vibrato_bound))
      l->vibrato_level = vibrato_bound;

    if (l->tremolo_level > tremolo_bound)
      l->tremolo_level = tremolo_bound;
#endif
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
  S_lfo_wavetable_tri[0] = 0;
  S_lfo_wavetable_tri[LFO_WAVETABLE_SIZE_QUARTER] = LFO_WAVE_AMPLITUDE;

  for (k = 1; k < LFO_WAVETABLE_SIZE_QUARTER; k++)
  {
    S_lfo_wavetable_tri[k] = (short int) (((LFO_WAVE_AMPLITUDE * k) / ((float) LFO_WAVETABLE_SIZE_QUARTER)) + 0.5f);
    S_lfo_wavetable_tri[LFO_WAVETABLE_SIZE_HALF - k] = S_lfo_wavetable_tri[k];
  }

  /* wavetable (sawtooth) */
  S_lfo_wavetable_saw[0] = LFO_WAVE_AMPLITUDE;
  S_lfo_wavetable_saw[LFO_WAVETABLE_SIZE_HALF] = 0;

  for (k = 1; k < LFO_WAVETABLE_SIZE_HALF; k++)
  {
    S_lfo_wavetable_saw[k] = (short int) (((LFO_WAVE_AMPLITUDE * (LFO_WAVETABLE_SIZE_HALF - k)) / ((float) LFO_WAVETABLE_SIZE_HALF)) + 0.5f);
    S_lfo_wavetable_saw[LFO_WAVETABLE_SIZE_HALF - k] = -S_lfo_wavetable_saw[k];
  }

  /* vibrato & tremolo tables */
  for (m = 0; m < PATCH_SENSITIVITY_NUM_VALUES; m++)
  {
    S_lfo_vibrato_max_table[m] = 
      (S_lfo_vibrato_cents_table[m] * TUNING_NUM_SEMITONE_STEPS) / 100;

    S_lfo_tremolo_max_table[m] = 
      LFO_TREMOLO_SENSITIVITY_STEP * (m + 1);
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
    for (m = 0; m < PATCH_LFO_SPEED_NUM_VALUES; m++)
    {
      S_lfo_wave_phase_increment_table[k][m] = 
        (int) ((TEMPO_COMPUTE_BEATS_PER_SECOND(k + TEMPO_LOWER_BOUND) * (LFO_WAVE_FREQUENCY_BASE + m * LFO_WAVE_FREQUENCY_STEP) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
    }
  }

#if 0
  for (m = 0; m < PATCH_LFO_FREQUENCY_NUM_VALUES; m++)
  {
    S_lfo_noise_phase_increment_table[m] = 
      (int) ((S_lfo_noise_frequency_table[m] * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
  }
#endif

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

  return 0;
}
