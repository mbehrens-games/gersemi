/*******************************************************************************
** lfo.c (low frequency oscillator)
*******************************************************************************/

#include <stdio.h>    /* testing */
#include <stdlib.h>
#include <math.h>

#include "bank.h"
#include "dblinear.h"
#include "lfo.h"
#include "patch.h"
#include "tuning.h"

#define PI      3.14159265358979323846f
#define TWO_PI  6.28318530717958647693f

#define LFO_VIBRATO_DEPTHS  17
#define LFO_TREMOLO_DEPTHS  17

#define LFO_MOD_WHEEL_STEPS 17

#define LFO_WAVETABLE_LOOKUP(phase)                                            \
  masked_phase = ((phase) & 0x0FF);                                            \
                                                                               \
  /* waveform 0: triangle */                                                   \
  if (l->waveform == 0)                                                        \
  {                                                                            \
    if (masked_phase < 128)                                                    \
      base_wave_index = S_lfo_wavetable_triangle[masked_phase];                \
    else                                                                       \
      base_wave_index = S_lfo_wavetable_triangle[masked_phase - 128];          \
  }                                                                            \
  /* waveform 1: square */                                                     \
  else if (l->waveform == 1)                                                   \
    base_wave_index = 0;                                                       \
  /* waveform 2-3: sawtooth */                                                 \
  else if ((l->waveform == 2) || (l->waveform == 3))                           \
    base_wave_index = S_lfo_wavetable_sawtooth[masked_phase];                  \
  else                                                                         \
    base_wave_index = 4095;

#define LFO_UPDATE_VIBRATO()                                                            \
  base_vibrato_index = base_wave_index + (S_vibrato_depth_table[l->base_vibrato] << 2); \
                                                                                        \
  if (base_vibrato_index < 0)                                                           \
    base_vibrato_index = 0;                                                             \
  else if (base_vibrato_index > 4095)                                                   \
    base_vibrato_index = 4095;                                                          \
                                                                                        \
  /* waveform 0-2: non-inverted */                                                      \
  if ((l->waveform == 0) || (l->waveform == 1) || (l->waveform == 2))                   \
  {                                                                                     \
    if (masked_phase < 128)                                                             \
      l->vibrato_level = G_db_to_linear_table[base_vibrato_index];                      \
    else                                                                                \
      l->vibrato_level = -G_db_to_linear_table[base_vibrato_index];                     \
  }                                                                                     \
  /* waveform 3: inverted */                                                            \
  else if (l->waveform == 3)                                                            \
  {                                                                                     \
    if (masked_phase < 128)                                                             \
      l->vibrato_level = -G_db_to_linear_table[base_vibrato_index];                     \
    else                                                                                \
      l->vibrato_level = G_db_to_linear_table[base_vibrato_index];                      \
  }                                                                                     \
  else                                                                                  \
    l->vibrato_level = G_db_to_linear_table[4095];

#define LFO_UPDATE_TREMOLO()                                                            \
  base_tremolo_index = base_wave_index + (S_tremolo_depth_table[l->base_tremolo] << 2); \
                                                                                        \
  if (base_tremolo_index < 0)                                                           \
    base_tremolo_index = 0;                                                             \
  else if (base_tremolo_index > 4095)                                                   \
    base_tremolo_index = 4095;                                                          \
                                                                                        \
  /* waveform 0-2: non-inverted */                                                      \
  if ((l->waveform == 0) || (l->waveform == 1) || (l->waveform == 2))                   \
  {                                                                                     \
    if (masked_phase < 128)                                                             \
      l->tremolo_level = G_db_to_linear_table[base_tremolo_index];                      \
    else                                                                                \
      l->tremolo_level = -G_db_to_linear_table[base_tremolo_index];                     \
  }                                                                                     \
  /* waveform 3: inverted */                                                            \
  else if (l->waveform == 3)                                                            \
  {                                                                                     \
    if (masked_phase < 128)                                                             \
      l->tremolo_level = -G_db_to_linear_table[base_tremolo_index];                     \
    else                                                                                \
      l->tremolo_level = G_db_to_linear_table[base_tremolo_index];                      \
  }                                                                                     \
  else                                                                                  \
    l->tremolo_level = G_db_to_linear_table[4095];

/* for the following tables, the values are found using the formula:  */
/*   (10 * (log(1 / val) / log(10)) / DB_STEP_10_BIT,                 */
/*   where DB_STEP_10_BIT = 0.046875                                  */

/* the values in the tables are attenuations added onto the waveform  */
/* so that the amplitude scales down to the desired amount, starting  */
/* from the original amplitude of 32767.                              */

/* so, if we want to scale the amplitude down to "x", */
/* then the "val" in the formula should be "x"/32767. */

/* vibrato depth table */
static short int  S_vibrato_depth_table[LFO_VIBRATO_DEPTHS] = 
                  {1023, /*   0 (off)         */
                    899, /*   2 (3.1 cents)   */
                    862, /*   3 (4.7 cents)   */
                    835, /*   4 (6.25 cents)  */
                    797, /*   6 (9.4 cents)   */
                    760, /*   9 (14.1 cents)  */
                    733, /*  12 (18.8 cents)  */
                    706, /*  16 (25 cents)    */
                    669, /*  24 (37.5 cents)  */
                    642, /*  32 (50 cents)    */
                    622, /*  40 (62.5 cents)  */
                    605, /*  48 (75 cents)    */
                    578, /*  64 (1 semitone)  */
                    514, /* 128 (2 semitones) */
                    476, /* 192 (3 semitones) */
                    450, /* 256 (4 semitones) */
                    398  /* 448 (7 semitones) */
                  };

/* tremolo depth table */
static short int  S_tremolo_depth_table[LFO_TREMOLO_DEPTHS] = 
                  {1023,  /*      0  (off)          */
                    750,  /* 10 * 1  (10^(-3/64))   */
                    686,  /* 10 * 2  (...)          */
                    648,  /* 10 * 3  (...)          */
                    622,  /* 10 * 4  (...)          */
                    601,  /* 10 * 5  (...)          */
                    584,  /* 10 * 6  (...)          */
                    570,  /* 10 * 7  (...)          */
                    557,  /* 10 * 8  (10^(-24/64))  */
                    546,  /* 10 * 9  (...)          */
                    537,  /* 10 * 10 (...)          */
                    528,  /* 10 * 11 (...)          */
                    520,  /* 10 * 12 (...)          */
                    512,  /* 10 * 13 (...)          */
                    505,  /* 10 * 14 (...)          */
                    499,  /* 10 * 15 (...)          */
                    493   /* 10 * 16 (10^(-48/64))  */
                  };

/* mod wheel table */
static short int  S_lfo_mod_wheel_step_table[17] = 
                  {1023,  /*  0/16  */
                    257,  /*  1/16  */
                    193,  /*  2/16  */
                    155,  /*  3/16  */
                    128,  /*  4/16  */
                    108,  /*  5/16  */
                     91,  /*  6/16  */
                     77,  /*  7/16  */
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

/* wavetables */
static short int S_lfo_wavetable_triangle[128];
static short int S_lfo_wavetable_sawtooth[256];

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
  l->waveform = 0;
  l->octave = 0;
  l->note = 0;
  l->delay = 0;
  l->sync = 1;
  l->vibrato_mode = 0;

  l->base_vibrato = 0;
  l->base_tremolo = 0;
  l->base_wobble = 0;

  /* mod wheel sensitivity */
  l->mod_wheel_vibrato = 0;
  l->mod_wheel_tremolo = 0;
  l->mod_wheel_wobble = 0;

  /* aftertouch sensitivity */
  l->aftertouch_vibrato = 0;
  l->aftertouch_tremolo = 0;
  l->aftertouch_wobble = 0;

  /* phase, phase increment */
  l->phase = 0;
  l->increment = 0;

  /* noise lfsr */
  l->lfsr = 0x0001;

  /* mod wheel and aftertouch inputs */
  l->mod_wheel_input = 0;
  l->aftertouch_input = 0;

  /* levels */
  l->vibrato_level = 0;
  l->tremolo_level = 0;
  l->wobble_level = 0;

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

  int pitch_index;

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

  /* octave */
  if ((p->lfo_octave >= PATCH_LFO_OCTAVE_LOWER_BOUND) && 
      (p->lfo_octave <= PATCH_LFO_OCTAVE_UPPER_BOUND))
  {
    l->octave = p->lfo_octave;
  }
  else
    l->octave = PATCH_LFO_OCTAVE_LOWER_BOUND;

  /* note */
  if ((p->lfo_note >= PATCH_LFO_NOTE_LOWER_BOUND) && 
      (p->lfo_note <= PATCH_LFO_NOTE_UPPER_BOUND))
  {
    l->note = p->lfo_note;
  }
  else
    l->note = PATCH_LFO_NOTE_LOWER_BOUND;

  /* delay */
  if ((p->lfo_delay >= PATCH_LFO_DELAY_LOWER_BOUND) && 
      (p->lfo_delay <= PATCH_LFO_DELAY_UPPER_BOUND))
  {
    l->delay = p->lfo_delay;
  }
  else
    l->delay = PATCH_LFO_DELAY_LOWER_BOUND;

  /* sync */
  if ((p->lfo_sync >= PATCH_LFO_SYNC_LOWER_BOUND) && 
      (p->lfo_sync <= PATCH_LFO_SYNC_UPPER_BOUND))
  {
    l->sync = p->lfo_sync;
  }
  else
    l->sync = PATCH_LFO_SYNC_LOWER_BOUND;

  /* vibrato */
  if ((p->lfo_vibrato >= PATCH_VIBRATO_LOWER_BOUND) && 
      (p->lfo_vibrato <= PATCH_VIBRATO_UPPER_BOUND))
  {
    l->base_vibrato = p->lfo_vibrato;
  }
  else
    l->base_vibrato = PATCH_VIBRATO_LOWER_BOUND;

  /* tremolo */
  if ((p->lfo_tremolo >= PATCH_TREMOLO_LOWER_BOUND) && 
      (p->lfo_tremolo <= PATCH_TREMOLO_UPPER_BOUND))
  {
    l->base_tremolo = p->lfo_tremolo;
  }
  else
    l->base_tremolo = PATCH_TREMOLO_LOWER_BOUND;

  /* wobble */
  if ((p->lfo_wobble >= PATCH_WOBBLE_LOWER_BOUND) && 
      (p->lfo_wobble <= PATCH_WOBBLE_UPPER_BOUND))
  {
    l->base_wobble = p->lfo_wobble;
  }
  else
    l->base_wobble = PATCH_WOBBLE_LOWER_BOUND;

  /* mod wheel vibrato */
  if ((p->mod_wheel_vibrato >= PATCH_VIBRATO_LOWER_BOUND) && 
      (p->mod_wheel_vibrato <= PATCH_VIBRATO_UPPER_BOUND))
  {
    l->mod_wheel_vibrato = p->mod_wheel_vibrato;
  }
  else
    l->mod_wheel_vibrato = PATCH_VIBRATO_LOWER_BOUND;

  /* mod wheel tremolo */
  if ((p->mod_wheel_tremolo >= PATCH_TREMOLO_LOWER_BOUND) && 
      (p->mod_wheel_tremolo <= PATCH_TREMOLO_UPPER_BOUND))
  {
    l->mod_wheel_tremolo = p->mod_wheel_tremolo;
  }
  else
    l->mod_wheel_tremolo = PATCH_TREMOLO_LOWER_BOUND;

  /* mod wheel wobble */
  if ((p->mod_wheel_wobble >= PATCH_WOBBLE_LOWER_BOUND) && 
      (p->mod_wheel_wobble <= PATCH_WOBBLE_UPPER_BOUND))
  {
    l->mod_wheel_wobble = p->mod_wheel_wobble;
  }
  else
    l->mod_wheel_wobble = PATCH_WOBBLE_LOWER_BOUND;

  /* aftertouch vibrato */
  if ((p->aftertouch_vibrato >= PATCH_VIBRATO_LOWER_BOUND) && 
      (p->aftertouch_vibrato <= PATCH_VIBRATO_UPPER_BOUND))
  {
    l->aftertouch_vibrato = p->aftertouch_vibrato;
  }
  else
    l->aftertouch_vibrato = PATCH_VIBRATO_LOWER_BOUND;

  /* aftertouch tremolo */
  if ((p->aftertouch_tremolo >= PATCH_TREMOLO_LOWER_BOUND) && 
      (p->aftertouch_tremolo <= PATCH_TREMOLO_UPPER_BOUND))
  {
    l->aftertouch_tremolo = p->aftertouch_tremolo;
  }
  else
    l->aftertouch_tremolo = PATCH_TREMOLO_LOWER_BOUND;

  /* aftertouch wobble */
  if ((p->aftertouch_wobble >= PATCH_WOBBLE_LOWER_BOUND) && 
      (p->aftertouch_wobble <= PATCH_WOBBLE_UPPER_BOUND))
  {
    l->aftertouch_wobble = p->aftertouch_wobble;
  }
  else
    l->aftertouch_wobble = PATCH_WOBBLE_LOWER_BOUND;

  /* determine phase increment based on octave & note */
  pitch_index = 
    (12 * l->octave + l->note) * TUNING_NUM_SEMITONE_STEPS;

  if (pitch_index < 0)
    pitch_index = 0;
  else if (pitch_index >= TUNING_TABLE_SIZE)
    pitch_index = TUNING_TABLE_SIZE - 1;

  l->increment = G_phase_increment_table[pitch_index];

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

  /* reset phase and lfsr if necessary */
  if (l->sync == 1)
  {
    l->phase = 0;
    l->lfsr = 0x0001;
  }

  /* initialize levels */
  l->vibrato_level = 0;
  l->tremolo_level = 0;
  l->wobble_level = 0;

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
  int base_wave_index;

  int base_vibrato_index;
  int base_tremolo_index;
  int base_wobble_index;

  /* update all lfos */
  for (k = 0; k < BANK_NUM_VOICES; k++)
  {
    l = &G_lfo_bank[k];

    /* update phase */
    l->phase += l->increment;

    /* wraparound phase register (28 bits) */
    if (l->phase > 0xFFFFFFF)
    {
      /* 15-bit lfsr, taps on 1 and 2 */
      if ((l->lfsr & 0x0001) ^ ((l->lfsr & 0x0002) >> 1))
        l->lfsr = ((l->lfsr >> 1) & 0x3FFF) | 0x4000;
      else
        l->lfsr = (l->lfsr >> 1) & 0x3FFF;

      l->phase &= 0xFFFFFFF;
    }

    /* determine base wave index */
    LFO_WAVETABLE_LOOKUP(l->phase >> 20)

    /* update levels */
    LFO_UPDATE_VIBRATO()
  }

  return 0;
}

/*******************************************************************************
** lfo_generate_tables()
*******************************************************************************/
short int lfo_generate_tables()
{
  int     i;
  double  val;

  /* the lfo wavetables have 256 entries per period */

  /* wavetable (triangle) */
  S_lfo_wavetable_triangle[0] = 4095;
  S_lfo_wavetable_triangle[64] = 0;

  for (i = 1; i < 64; i++)
  {
    val = i / 64.0f;
    S_lfo_wavetable_triangle[i] = (short int) ((10 * (log(1 / val) / log(10)) / DB_STEP_12_BIT) + 0.5f);
    S_lfo_wavetable_triangle[128 - i] = S_lfo_wavetable_triangle[i];
  }

  /* wavetable (sawtooth) */
  S_lfo_wavetable_sawtooth[0] = 4095;
  S_lfo_wavetable_sawtooth[128] = 0;

  for (i = 1; i < 128; i++)
  {
    val = i / 128.0f;
    S_lfo_wavetable_sawtooth[i] = (short int) ((10 * (log(1 / val) / log(10)) / DB_STEP_12_BIT) + 0.5f);
    S_lfo_wavetable_sawtooth[256 - i] = S_lfo_wavetable_sawtooth[i];
  }

#if 0
  for (i = 0; i < 128; i++)
  {
    printf( "LFO Triangle Wavetable Index %d: %d (%d)\n", 
            i, S_lfo_wavetable_triangle[i], G_db_to_linear_table[S_lfo_wavetable_triangle[i]]);
  }
#endif

#if 0
  for (i = 0; i < LFO_VIBRATO_DEPTHS; i++)
  {
    printf( "LFO Vibrato Triangle Max at Depth %d: %d\n", 
            i, G_db_to_linear_table[S_lfo_wavetable_triangle[64] + (S_vibrato_depth_table[i] << 2)]);
  }
#endif

#if 0
  for (i = 0; i < LFO_TREMOLO_DEPTHS; i++)
  {
    printf( "LFO Tremolo Triangle Max at Depth %d: %d\n", 
            i, G_db_to_linear_table[S_lfo_wavetable_triangle[64] + (S_tremolo_depth_table[i] << 2)]);
  }
#endif

  return 0;
}

