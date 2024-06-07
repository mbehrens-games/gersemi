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

enum
{
  LFO_TYPE_VIBRATO = 1, 
  LFO_TYPE_TREMOLO, 
  LFO_TYPE_CHORUS 
};

#define LFO_TREMOLO_SENSITIVITY_STEP  (1 * 32)

#define LFO_WAVE_AMPLITUDE  512

#define LFO_WAVETABLE_SIZE_FULL     256
#define LFO_WAVETABLE_SIZE_HALF     (LFO_WAVETABLE_SIZE_FULL / 2)
#define LFO_WAVETABLE_SIZE_QUARTER  (LFO_WAVETABLE_SIZE_FULL / 4)

/* delay times (in fractions of a beat)     */
/* at 120 bpm, the delays range from        */
/* 0.0 s to 0.5 s (counting up by 1/48 s)   */
#define LFO_DELAY_STEP 0.020833333333333f

/* vib/trem speeds (in cycles per beat)     */
/* at 120 bpm, the frequencies range from   */
/* 0.5 hz to 8 hz (counting up by 0.5 hz)   */
#define LFO_VIB_TREM_SPEED_STEP 0.25f

/* chorus speeds (in cycles per beat)       */
/* at 120 bpm, the frequencies range from   */
/* 0.1 hz to 1.6 hz (counting up by 0.1 hz) */
#define LFO_CHORUS_SPEED_STEP 0.05f

/* amplitude tables */
static short int  S_lfo_vibrato_max_table[PATCH_SENSITIVITY_NUM_VALUES] = 
                  { (  2 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    (  4 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    (  6 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    (  8 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    ( 10 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    ( 12 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    ( 14 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    ( 16 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    ( 20 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    ( 24 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    ( 28 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    ( 32 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    ( 64 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    (128 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    (192 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    (256 * TUNING_NUM_SEMITONE_STEPS) / 128 
                  };

static short int  S_lfo_tremolo_max_table[PATCH_SENSITIVITY_NUM_VALUES] = 
                  { LFO_TREMOLO_SENSITIVITY_STEP *  1, 
                    LFO_TREMOLO_SENSITIVITY_STEP *  2, 
                    LFO_TREMOLO_SENSITIVITY_STEP *  3, 
                    LFO_TREMOLO_SENSITIVITY_STEP *  4, 
                    LFO_TREMOLO_SENSITIVITY_STEP *  5, 
                    LFO_TREMOLO_SENSITIVITY_STEP *  6, 
                    LFO_TREMOLO_SENSITIVITY_STEP *  7, 
                    LFO_TREMOLO_SENSITIVITY_STEP *  8, 
                    LFO_TREMOLO_SENSITIVITY_STEP *  9, 
                    LFO_TREMOLO_SENSITIVITY_STEP * 10, 
                    LFO_TREMOLO_SENSITIVITY_STEP * 11, 
                    LFO_TREMOLO_SENSITIVITY_STEP * 12, 
                    LFO_TREMOLO_SENSITIVITY_STEP * 13, 
                    LFO_TREMOLO_SENSITIVITY_STEP * 14, 
                    LFO_TREMOLO_SENSITIVITY_STEP * 15, 
                    LFO_TREMOLO_SENSITIVITY_STEP * 16 
                  };

static short int  S_lfo_chorus_max_table[PATCH_SENSITIVITY_NUM_VALUES] = 
                  { ( 2 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    ( 4 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    ( 6 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    ( 8 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    (10 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    (12 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    (14 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    (16 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    (18 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    (20 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    (22 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    (24 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    (26 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    (28 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    (30 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    (32 * TUNING_NUM_SEMITONE_STEPS) / 128 
                  };

/* wavetables */
static short int  S_lfo_bi_wavetable_tri[LFO_WAVETABLE_SIZE_FULL];
static short int  S_lfo_bi_wavetable_saw[LFO_WAVETABLE_SIZE_FULL];

static short int  S_lfo_uni_wavetable_tri[LFO_WAVETABLE_SIZE_FULL];
static short int  S_lfo_uni_wavetable_saw[LFO_WAVETABLE_SIZE_FULL];

/* delay table */
static int S_lfo_delay_period_table[TEMPO_NUM_VALUES];

/* phase increment tables */
static unsigned int S_lfo_vib_trem_phase_increment_table[TEMPO_NUM_VALUES];
static unsigned int S_lfo_chorus_phase_increment_table[TEMPO_NUM_VALUES];

/* lfo bank */
lfo G_lfo_bank[BANK_NUM_LFOS];

/*******************************************************************************
** lfo_reset_all()
*******************************************************************************/
short int lfo_reset_all()
{
  int k;

  lfo* l;

  /* reset all lfos */
  for (k = 0; k < BANK_NUM_LFOS; k++)
  {
    /* obtain lfo pointer */
    l = &G_lfo_bank[k];

    /* initialize lfo variables */
    if ((k % BANK_LFOS_PER_VOICE) == 0)
      l->type = LFO_TYPE_VIBRATO; 
    else if ((k % BANK_LFOS_PER_VOICE) == 1)
      l->type = LFO_TYPE_TREMOLO;
    else if ((k % BANK_LFOS_PER_VOICE) == 2)
      l->type = LFO_TYPE_CHORUS;
    else
      l->type = LFO_TYPE_VIBRATO;

    l->waveform = PATCH_LFO_WAVEFORM_DEFAULT;
    l->speed = PATCH_LFO_SPEED_DEFAULT;
    l->sync = PATCH_SYNC_DEFAULT;

    if (l->type == LFO_TYPE_VIBRATO)
      l->polarity = PATCH_LFO_POLARITY_DEFAULT;
    else if (l->type == LFO_TYPE_TREMOLO)
      l->polarity = PATCH_LFO_POLARITY_UNI;
    else if (l->type == LFO_TYPE_CHORUS)
      l->polarity = PATCH_LFO_POLARITY_BI;
    else
      l->polarity = PATCH_LFO_POLARITY_DEFAULT;

    l->delay_cycles = 0;
    l->delay_period = 
      S_lfo_delay_period_table[TEMPO_DEFAULT - TEMPO_LOWER_BOUND] * PATCH_LFO_DELAY_DEFAULT;

    l->phase = 0;

    if ((l->type == LFO_TYPE_VIBRATO) || 
        (l->type == LFO_TYPE_TREMOLO))
    {
      l->increment = 
        S_lfo_vib_trem_phase_increment_table[TEMPO_DEFAULT - TEMPO_LOWER_BOUND] * PATCH_LFO_SPEED_DEFAULT;
    }
    else if (l->type == LFO_TYPE_CHORUS)
    {
      l->increment = 
        S_lfo_chorus_phase_increment_table[TEMPO_DEFAULT - TEMPO_LOWER_BOUND] * PATCH_LFO_SPEED_DEFAULT;
    }
    else
      l->increment = 0;

    l->tempo = TEMPO_DEFAULT;

    l->depth = PATCH_LFO_DEPTH_DEFAULT;

    if (l->type == LFO_TYPE_VIBRATO)
      l->max = S_lfo_vibrato_max_table[PATCH_SENSITIVITY_DEFAULT - PATCH_SENSITIVITY_LOWER_BOUND];
    else if (l->type == LFO_TYPE_TREMOLO)
      l->max = S_lfo_tremolo_max_table[PATCH_SENSITIVITY_DEFAULT - PATCH_SENSITIVITY_LOWER_BOUND];
    else if (l->type == LFO_TYPE_CHORUS)
      l->max = S_lfo_chorus_max_table[PATCH_SENSITIVITY_DEFAULT - PATCH_SENSITIVITY_LOWER_BOUND];
    else
      l->max = 0;

    l->level_base = 0;
    l->level_extra = 0;
  }

  return 0;
}

/*******************************************************************************
** lfo_load_patch()
*******************************************************************************/
short int lfo_load_patch( int voice_index, 
                          int cart_index, int patch_index)
{
  int m;

  lfo* l;

  cart* c;
  patch* p;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* make sure that the cart & patch indices are valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(cart_index))
    return 1;

  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain cart & patch pointers */
  c = &G_cart_bank[cart_index];
  p = &(c->patches[patch_index]);

  for (m = 0; m < BANK_LFOS_PER_VOICE; m++)
  {
    /* obtain lfo pointer */
    l = &G_lfo_bank[voice_index * BANK_LFOS_PER_VOICE + m];

    /* waveform */
    if ((p->lfo_waveform[m] >= PATCH_LFO_WAVEFORM_LOWER_BOUND) && 
        (p->lfo_waveform[m] <= PATCH_LFO_WAVEFORM_UPPER_BOUND))
    {
      l->waveform = p->lfo_waveform[m];
    }
    else
      l->waveform = PATCH_LFO_WAVEFORM_DEFAULT;

    /* delay */
    if ((p->lfo_delay[m] >= PATCH_LFO_DELAY_LOWER_BOUND) && 
        (p->lfo_delay[m] <= PATCH_LFO_DELAY_UPPER_BOUND))
    {
      l->delay_period = S_lfo_delay_period_table[l->tempo - TEMPO_LOWER_BOUND] * p->lfo_delay[m];
    }
    else
      l->delay_period = S_lfo_delay_period_table[l->tempo - TEMPO_LOWER_BOUND] * PATCH_LFO_DELAY_DEFAULT;

    /* speed */
    if ((p->lfo_speed[m] >= PATCH_LFO_SPEED_LOWER_BOUND) && 
        (p->lfo_speed[m] <= PATCH_LFO_SPEED_UPPER_BOUND))
    {
      l->speed = p->lfo_speed[m];
    }
    else
      l->speed = PATCH_LFO_SPEED_DEFAULT;

    /* depth */
    if ((p->lfo_depth[m] >= PATCH_LFO_DEPTH_LOWER_BOUND) && 
        (p->lfo_depth[m] <= PATCH_LFO_DEPTH_UPPER_BOUND))
    {
      l->depth = p->lfo_depth[m];
    }
    else
      l->depth = PATCH_LFO_DEPTH_DEFAULT;

    /* sensitivity */
    if ((p->lfo_sensitivity[m] >= PATCH_SENSITIVITY_LOWER_BOUND) && 
        (p->lfo_sensitivity[m] <= PATCH_SENSITIVITY_UPPER_BOUND))
    {
      if (l->type == LFO_TYPE_VIBRATO)
        l->max = S_lfo_vibrato_max_table[p->lfo_sensitivity[m] - PATCH_SENSITIVITY_LOWER_BOUND];
      else if (l->type == LFO_TYPE_TREMOLO)
        l->max = S_lfo_tremolo_max_table[p->lfo_sensitivity[m] - PATCH_SENSITIVITY_LOWER_BOUND];
      else if (l->type == LFO_TYPE_CHORUS)
        l->max = S_lfo_chorus_max_table[p->lfo_sensitivity[m] - PATCH_SENSITIVITY_LOWER_BOUND];
      else
        l->max = S_lfo_vibrato_max_table[p->lfo_sensitivity[m] - PATCH_SENSITIVITY_LOWER_BOUND];
    }
    else
    {
      if (l->type == LFO_TYPE_VIBRATO)
        l->max = S_lfo_vibrato_max_table[PATCH_SENSITIVITY_DEFAULT - PATCH_SENSITIVITY_LOWER_BOUND];
      else if (l->type == LFO_TYPE_TREMOLO)
        l->max = S_lfo_tremolo_max_table[PATCH_SENSITIVITY_DEFAULT - PATCH_SENSITIVITY_LOWER_BOUND];
      else if (l->type == LFO_TYPE_CHORUS)
        l->max = S_lfo_chorus_max_table[PATCH_SENSITIVITY_DEFAULT - PATCH_SENSITIVITY_LOWER_BOUND];
      else
        l->max = S_lfo_vibrato_max_table[PATCH_SENSITIVITY_DEFAULT - PATCH_SENSITIVITY_LOWER_BOUND];
    }

    /* sync */
    if ((p->lfo_sync[m] >= PATCH_SYNC_LOWER_BOUND) && 
        (p->lfo_sync[m] <= PATCH_SYNC_UPPER_BOUND))
    {
      l->sync = p->lfo_sync[m];
    }
    else
      l->sync = PATCH_SYNC_DEFAULT;

    /* polarity */
    if (l->type == LFO_TYPE_VIBRATO)
    {
      if ((p->lfo_polarity[m] >= PATCH_LFO_POLARITY_LOWER_BOUND) && 
          (p->lfo_polarity[m] <= PATCH_LFO_POLARITY_UPPER_BOUND))
      {
        l->polarity = p->lfo_polarity[m];
      }
      else
        l->polarity = PATCH_SYNC_DEFAULT;
    }

    /* set increment based on speed */
    if ((l->type == LFO_TYPE_VIBRATO) || 
        (l->type == LFO_TYPE_TREMOLO))
    {
      l->increment = 
        S_lfo_vib_trem_phase_increment_table[l->tempo - TEMPO_LOWER_BOUND] * l->speed;
    }
    else if (l->type == LFO_TYPE_CHORUS)
    {
      l->increment = 
        S_lfo_chorus_phase_increment_table[l->tempo - TEMPO_LOWER_BOUND] * l->speed;
    }
    else
      l->increment = 0;
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

  for (m = 0; m < BANK_LFOS_PER_VOICE; m++)
  {
    /* obtain lfo pointer */
    l = &G_lfo_bank[voice_index * BANK_LFOS_PER_VOICE + m];

    /* set tempo */
    if (tempo < TEMPO_LOWER_BOUND)
      l->tempo = TEMPO_LOWER_BOUND;
    else if (tempo > TEMPO_UPPER_BOUND)
      l->tempo = TEMPO_UPPER_BOUND;
    else
      l->tempo = tempo;

    /* adjust phase increment based on tempo */
    if ((l->type == LFO_TYPE_VIBRATO) || 
        (l->type == LFO_TYPE_TREMOLO))
    {
      l->increment = 
        S_lfo_vib_trem_phase_increment_table[l->tempo - TEMPO_LOWER_BOUND] * l->speed;
    }
    else if (l->type == LFO_TYPE_CHORUS)
    {
      l->increment = 
        S_lfo_chorus_phase_increment_table[l->tempo - TEMPO_LOWER_BOUND] * l->speed;
    }
    else
      l->increment = 0;
  }

  return 0;
}

/*******************************************************************************
** lfo_tempo_sync()
*******************************************************************************/
short int lfo_tempo_sync(int voice_index)
{
  int m;

  lfo* l;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  for (m = 0; m < BANK_LFOS_PER_VOICE; m++)
  {
    /* obtain lfo pointer */
    l = &G_lfo_bank[voice_index * BANK_LFOS_PER_VOICE + m];

    /* reset phase if necessary */
    if (l->sync == PATCH_SYNC_OFF)
      l->phase = 0;
  }

  return 0;
}

/*******************************************************************************
** lfo_note_on()
*******************************************************************************/
short int lfo_note_on(int voice_index)
{
  int m;

  lfo* l;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  for (m = 0; m < BANK_LFOS_PER_VOICE; m++)
  {
    /* obtain lfo pointer */
    l = &G_lfo_bank[voice_index * BANK_LFOS_PER_VOICE + m];

    /* set delay cycles */
    l->delay_cycles = l->delay_period;

    /* reset levels during the delay */
    if (l->delay_cycles > 0)
    {
      l->level_base = 0;
      l->level_extra = 0;
    }

    /* reset phase if necessary */
    if (l->sync == PATCH_SYNC_ON)
      l->phase = 0;
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

  unsigned int masked_phase;

  int wave_level;

  /* update all lfos */
  for (k = 0; k < BANK_NUM_LFOS; k++)
  {
    /* obtain lfo pointer */
    l = &G_lfo_bank[k];

    /* update phase */
    l->phase += l->increment;

    /* wraparound phase register (28 bits) */
    if (l->phase > 0xFFFFFFF)
      l->phase &= 0xFFFFFFF;

    /* update delay cycles if necessary */
    if (l->delay_cycles > 0)
    {
      l->delay_cycles -= 1;

      if ((l->delay_cycles == 0) && (l->sync == PATCH_SYNC_ON))
        l->phase = 0;

      continue;
    }

    /* determine base wave indices */
    masked_phase = ((l->phase >> 20) & 0xFF);

    /* determine wave level */
    if (l->polarity == PATCH_LFO_POLARITY_BI)
    {
      if (l->waveform == PATCH_LFO_WAVEFORM_TRIANGLE)
        wave_level = S_lfo_bi_wavetable_tri[masked_phase];
      else if (l->waveform == PATCH_LFO_WAVEFORM_SQUARE)
      {
        if (masked_phase < LFO_WAVETABLE_SIZE_HALF)
          wave_level = LFO_WAVE_AMPLITUDE;
        else
          wave_level = -LFO_WAVE_AMPLITUDE;
      }
      else if (l->waveform == PATCH_LFO_WAVEFORM_SAW_UP)
        wave_level = LFO_WAVE_AMPLITUDE - S_lfo_bi_wavetable_saw[masked_phase];
      else if (l->waveform == PATCH_LFO_WAVEFORM_SAW_DOWN)
        wave_level = S_lfo_bi_wavetable_saw[masked_phase];
      else
        wave_level = 0;
    }
    else if (l->polarity == PATCH_LFO_POLARITY_UNI)
    {
      if (l->waveform == PATCH_LFO_WAVEFORM_TRIANGLE)
        wave_level = S_lfo_uni_wavetable_tri[masked_phase];
      else if (l->waveform == PATCH_LFO_WAVEFORM_SQUARE)
      {
        if (masked_phase < LFO_WAVETABLE_SIZE_HALF)
          wave_level = 0;
        else
          wave_level = LFO_WAVE_AMPLITUDE;
      }
      else if (l->waveform == PATCH_LFO_WAVEFORM_SAW_UP)
        wave_level = LFO_WAVE_AMPLITUDE - S_lfo_uni_wavetable_saw[masked_phase];
      else if (l->waveform == PATCH_LFO_WAVEFORM_SAW_DOWN)
        wave_level = S_lfo_uni_wavetable_saw[masked_phase];
      else
        wave_level = 0;
    }
    else
      wave_level = 0;

    /* apply sensitivty */
    wave_level = (wave_level * l->max) / LFO_WAVE_AMPLITUDE;

    /* determine base level */
    l->level_base = (wave_level * l->depth) / PATCH_LFO_DEPTH_UPPER_BOUND;

    /* determine extra level */
    l->level_extra = wave_level - l->level_base;
  }

  return 0;
}

/*******************************************************************************
** lfo_generate_tables()
*******************************************************************************/
short int lfo_generate_tables()
{
  int m;

  /* the lfo wavetables have 256 entries per period */

  /* vibrato wavetable (triangle) */
  S_lfo_bi_wavetable_tri[0] = 0;
  S_lfo_bi_wavetable_tri[LFO_WAVETABLE_SIZE_QUARTER] = LFO_WAVE_AMPLITUDE;
  S_lfo_bi_wavetable_tri[LFO_WAVETABLE_SIZE_HALF] = 0;
  S_lfo_bi_wavetable_tri[3 * LFO_WAVETABLE_SIZE_QUARTER] = -LFO_WAVE_AMPLITUDE;

  for (m = 1; m < LFO_WAVETABLE_SIZE_QUARTER; m++)
  {
    S_lfo_bi_wavetable_tri[m] = (short int) (((LFO_WAVE_AMPLITUDE * m) / ((float) LFO_WAVETABLE_SIZE_QUARTER)) + 0.5f);
    S_lfo_bi_wavetable_tri[LFO_WAVETABLE_SIZE_HALF - m] = S_lfo_bi_wavetable_tri[m];

    S_lfo_bi_wavetable_tri[LFO_WAVETABLE_SIZE_HALF + m] = -S_lfo_bi_wavetable_tri[m];
    S_lfo_bi_wavetable_tri[LFO_WAVETABLE_SIZE_FULL - m] = -S_lfo_bi_wavetable_tri[m];
  }

  /* vibrato wavetable (sawtooth) */
  S_lfo_bi_wavetable_saw[0] = 0;
  S_lfo_bi_wavetable_saw[LFO_WAVETABLE_SIZE_HALF] = LFO_WAVE_AMPLITUDE;

  for (m = 1; m < LFO_WAVETABLE_SIZE_HALF; m++)
  {
    S_lfo_bi_wavetable_saw[m] = (short int) (((LFO_WAVE_AMPLITUDE * m) / ((float) LFO_WAVETABLE_SIZE_HALF)) + 0.5f);
    S_lfo_bi_wavetable_saw[LFO_WAVETABLE_SIZE_FULL - m] = -S_lfo_bi_wavetable_saw[m];
  }

  /* tremolo wavetable (triangle) */
  S_lfo_uni_wavetable_tri[0] = 0;
  S_lfo_uni_wavetable_tri[LFO_WAVETABLE_SIZE_HALF] = LFO_WAVE_AMPLITUDE;

  for (m = 1; m < LFO_WAVETABLE_SIZE_HALF; m++)
  {
    S_lfo_uni_wavetable_tri[m] = (short int) (((LFO_WAVE_AMPLITUDE * m) / ((float) LFO_WAVETABLE_SIZE_HALF)) + 0.5f);
    S_lfo_uni_wavetable_tri[LFO_WAVETABLE_SIZE_FULL - m] = S_lfo_uni_wavetable_tri[m];
  }

  /* tremolo wavetable (sawtooth) */
  S_lfo_uni_wavetable_saw[0] = 0;

  for (m = 1; m < LFO_WAVETABLE_SIZE_FULL; m++)
  {
    S_lfo_uni_wavetable_saw[m] = (short int) (((LFO_WAVE_AMPLITUDE * m) / ((float) LFO_WAVETABLE_SIZE_FULL)) + 0.5f);
  }

  /* delay period table */
  for (m = 0; m < TEMPO_NUM_VALUES; m++)
  {
    S_lfo_delay_period_table[m] = 
      (int) ((LFO_DELAY_STEP * TEMPO_COMPUTE_SECONDS_PER_BEAT(m + TEMPO_LOWER_BOUND) * CLOCK_SAMPLING_RATE) + 0.5f);
  }

  /* phase increment tables */
  for (m = 0; m < TEMPO_NUM_VALUES; m++)
  {
    S_lfo_vib_trem_phase_increment_table[m] = 
      (int) ((LFO_VIB_TREM_SPEED_STEP * TEMPO_COMPUTE_BEATS_PER_SECOND(m + TEMPO_LOWER_BOUND) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_lfo_chorus_phase_increment_table[m] = 
      (int) ((LFO_CHORUS_SPEED_STEP * TEMPO_COMPUTE_BEATS_PER_SECOND(m + TEMPO_LOWER_BOUND) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
  }

#if 0
  /* print out vibrato triangle wavetable */
  for (m = 0; m < LFO_VIBRATO_WAVETABLE_SIZE_FULL; m++)
  {
    printf( "Vibrato Triangle Wavetable Index %d: %d \n", 
            m, S_lfo_bi_wavetable_tri[m]);
  }

  /* print out vibrato sawtooth wavetable */
  for (m = 0; m < LFO_VIBRATO_WAVETABLE_SIZE_FULL; m++)
  {
    printf( "Vibrato Sawtooth Wavetable Index %d: %d \n", 
            m, S_lfo_bi_wavetable_saw[m]);
  }
#endif

#if 0
  /* print out phase increments */
  for (m = 0; m < PATCH_LFO_SPEED_NUM_VALUES; m++)
  {
    printf( "Vibrato/Tremolo Phase Increment (at 120 BPM) Index %d: %d \n", 
            m, S_lfo_vib_trem_phase_increment_table[120 - TEMPO_LOWER_BOUND] * (m + 1));
  }

  for (m = 0; m < PATCH_LFO_SPEED_NUM_VALUES; m++)
  {
    printf( "Chorus Phase Increment (at 120 BPM) Index %d: %d \n", 
            m, S_lfo_chorus_phase_increment_table[120 - TEMPO_LOWER_BOUND] * (m + 1));
  }
#endif

  return 0;
}
