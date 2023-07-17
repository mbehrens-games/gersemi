/*******************************************************************************
** voice.c (synth voice)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "clock.h"
#include "dblinear.h"
#include "lfo.h"
#include "patch.h"
#include "sweep.h"
#include "tuning.h"
#include "voice.h"

#define PI      3.14159265358979323846f
#define TWO_PI  6.28318530717958647693f

#define VOICE_WAVETABLE_LOOKUP(num, phase)                                     \
  masked_phase = ((phase) & 0x3FF);                                            \
                                                                               \
  if (masked_phase < 512)                                                      \
    final_index = S_voice_wavetable_sine[masked_phase];                        \
  else                                                                         \
    final_index = S_voice_wavetable_sine[masked_phase - 512];                  \
                                                                               \
  final_index += (osc_env_index[num] << 2);                                    \
                                                                               \
  if (final_index < 0)                                                         \
    final_index = 0;                                                           \
  else if (final_index > 4095)                                                 \
    final_index = 4095;                                                        \
                                                                               \
  /* waveform 0: sine */                                                       \
  if (v->osc_waveform[num] == 0)                                               \
  {                                                                            \
    if (masked_phase < 512)                                                    \
      osc_level[num] = G_db_to_linear_table[final_index];                      \
    else                                                                       \
      osc_level[num] = -G_db_to_linear_table[final_index];                     \
  }                                                                            \
  /* waveform 1: half-rectified */                                             \
  else if (v->osc_waveform[num] == 1)                                          \
  {                                                                            \
    if (masked_phase < 512)                                                    \
      osc_level[num] = G_db_to_linear_table[final_index];                      \
    else                                                                       \
      osc_level[num] = G_db_to_linear_table[4095];                             \
  }                                                                            \
  /* waveform 2: full-rectified */                                             \
  else if (v->osc_waveform[num] == 2)                                          \
  {                                                                            \
    if (masked_phase < 512)                                                    \
      osc_level[num] = G_db_to_linear_table[final_index];                      \
    else                                                                       \
      osc_level[num] = G_db_to_linear_table[final_index];                      \
  }                                                                            \
  /* waveform 3: quarter-sine */                                               \
  else if (v->osc_waveform[num] == 3)                                          \
  {                                                                            \
    if (masked_phase < 256)                                                    \
      osc_level[num] = G_db_to_linear_table[final_index];                      \
    else if (masked_phase < 512)                                               \
      osc_level[num] = G_db_to_linear_table[4095];                             \
    else if (masked_phase < 768)                                               \
      osc_level[num] = G_db_to_linear_table[final_index];                      \
    else                                                                       \
      osc_level[num] = G_db_to_linear_table[4095];                             \
  }                                                                            \
  else                                                                         \
    osc_level[num] = G_db_to_linear_table[4095];

#define VOICE_UPDATE_OSCILLATOR_FEEDBACK(num)                                           \
  /* compute feedback phase mod                     */                                  \
  /* we take the average of the last two values,    */                                  \
  /* and then find the phase mod amount normally    */                                  \
  /* (right shift by another 3, and then mask it).  */                                  \
  osc_fb_mod[num] = ((v->feed_in[2 * num + 0] + v->feed_in[2 * num + 1]) >> 4) & 0x3FF; \
                                                                                        \
  /* update oscillator */                                                               \
  VOICE_WAVETABLE_LOOKUP(num, (v->osc_phase[num] >> 18) + osc_fb_mod[num])              \
                                                                                        \
  /* cycle feedback */                                                                  \
  v->feed_in[2 * num + 1] = v->feed_in[2 * num + 0];                                    \
  v->feed_in[2 * num + 0] = (osc_level[num] * v->osc_feedback_multiplier[num]) >> 6;

#define VOICE_UPDATE_OSCILLATOR_NO_MOD(num)                                    \
  VOICE_WAVETABLE_LOOKUP(num, (v->osc_phase[num] >> 18) + osc_fb_mod[num])     \
                                                                               \
  osc_phase_mod[num] = (osc_level[num] >> 3) & 0x3FF;

#define VOICE_UPDATE_OSCILLATOR_MOD_X1(num, mod1)                              \
  VOICE_WAVETABLE_LOOKUP(num, (v->osc_phase[num] >> 18) + osc_fb_mod[num]      \
                                                        + osc_phase_mod[mod1]) \
                                                                               \
  osc_phase_mod[num] = (osc_level[num] >> 3) & 0x3FF;

#define VOICE_UPDATE_OSCILLATOR_MOD_X2(num, mod1, mod2)                        \
  VOICE_WAVETABLE_LOOKUP(num, (v->osc_phase[num] >> 18) + osc_fb_mod[num]      \
                                                        + osc_phase_mod[mod1]  \
                                                        + osc_phase_mod[mod2]) \
                                                                               \
  osc_phase_mod[num] = (osc_level[num] >> 3) & 0x3FF;

#define VOICE_UPDATE_OSCILLATOR_MOD_X3(num, mod1, mod2, mod3)                  \
  VOICE_WAVETABLE_LOOKUP(num, (v->osc_phase[num] >> 18) + osc_fb_mod[num]      \
                                                        + osc_phase_mod[mod1]  \
                                                        + osc_phase_mod[mod2]  \
                                                        + osc_phase_mod[mod3]) \
                                                                               \
  osc_phase_mod[num] = (osc_level[num] >> 3) & 0x3FF;

/* feedback table */
static int  S_voice_feedback_table[8] = 
            { 0, 1, 2, 4, 8, 16, 32, 64};

/* multiple table */

/* the values are relative to the note played; they are   */
/* chosen to be approximately multiples of the base pitch */
static int  S_voice_multiple_table[16] = 
            {-1 * 12 + 0,   /* 0.5x               */
              0 * 12 + 0,   /*   1x               */
              1 * 12 + 0,   /*   2x (1 oct up)    */
              1 * 12 + 7,   /*   3x               */
              2 * 12 + 0,   /*   4x (2 octs up)   */
              2 * 12 + 4,   /*   5x               */
              2 * 12 + 7,   /*   6x               */
              2 * 12 + 10,  /*   7x               */
              3 * 12 + 0,   /*   8x (3 octs up)   */
              3 * 12 + 2,   /*   9x               */
              3 * 12 + 4,   /*  10x               */
              3 * 12 + 6,   /*  11x               */
              3 * 12 + 7,   /*  12x               */
              3 * 12 + 8,   /*  13x               */
              3 * 12 + 10,  /*  14x               */
              3 * 12 + 11   /*  15x               */
            };

/* detune coarse table */
static int  S_voice_detune_coarse_table[4] = 
            { 0 * 12 + 0,   /* 1        */
              0 * 12 + 6,   /* sqrt(2)  */
              0 * 12 + 8,   /* pi / 2   */
              0 * 12 + 10   /* sqrt(3)  */
            };

/* detune fine table */
static int  S_voice_detune_fine_table[17] = 
            { (-24 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              (-20 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              (-16 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              (-12 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              ( -8 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              ( -6 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              ( -4 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              ( -2 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              0, 
              (  2 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              (  4 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              (  6 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              (  8 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              ( 12 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              ( 16 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              ( 20 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              ( 24 * TUNING_NUM_SEMITONE_STEPS) / 64
            };

/* noise note table */
static int  S_voice_noise_note_table[16] = 
            { 3 * 12 + 9, /* A-3  */
              4 * 12 + 2, /* D-4  */
              4 * 12 + 9, /* A-4  */
              5 * 12 + 2, /* D-5  */
              5 * 12 + 9, /* A-5  */
              6 * 12 + 2, /* D-6  */
              6 * 12 + 9, /* A-6  */
              7 * 12 + 1, /* C#7  */
              7 * 12 + 5, /* F-7  */
              7 * 12 + 9, /* A-7  */
              8 * 12 + 1, /* C#8  */
              8 * 12 + 5, /* F-8  */
              8 * 12 + 9, /* A-8  */
              9 * 12 + 1, /* C#9  */
              9 * 12 + 5, /* F-9  */
              9 * 12 + 9  /* A-9  */
            };

/* sine wavetable */
static short int S_voice_wavetable_sine[512];

/* voice bank */
voice G_voice_bank[BANK_NUM_VOICES];

/*******************************************************************************
** voice_setup_all()
*******************************************************************************/
short int voice_setup_all()
{
  int k;

  /* setup all voices */
  for (k = 0; k < BANK_NUM_VOICES; k++)
    voice_reset(k);

  return 0;
}

/*******************************************************************************
** voice_reset()
*******************************************************************************/
short int voice_reset(int voice_index)
{
  int m;

  voice* v;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain voice pointer */
  v = &G_voice_bank[voice_index];

  /* algorithm */
  v->algorithm = PATCH_ALGORITHM_LOWER_BOUND;

  v->num_modulators = 3;

  /* base note */
  v->base_note = 0;

  /* currently playing notes, pitch table indices, phase, feedback levels, voice parameters */
  for (m = 0; m < VOICE_NUM_OSCS_AND_ENVS; m++)
  {
    v->osc_note[m] = 0;
    v->osc_pitch_index[m] = 0;

    v->osc_phase[m] = 0;

    v->feed_in[2 * m + 0] = 0;
    v->feed_in[2 * m + 1] = 0;

    v->osc_waveform[m] = 0;
    v->osc_feedback_multiplier[m] = 0;

    v->osc_multiple[m] = 0;
    v->osc_detune_coarse[m] = 0;
    v->osc_detune_fine[m] = 0;
  }

#if 0
  /* noise generator */
  v->noise_pitch_index = S_voice_noise_note_table[0] * TUNING_NUM_SEMITONE_STEPS;
  v->noise_mode = 0;

  v->noise_phase = 0;
  v->noise_lfsr = 0x0001;
#endif

  /* envelope levels */
  for (m = 0; m < VOICE_NUM_OSCS_AND_ENVS; m++)
    v->env_input[m] = 0;

  /* lfo levels */
  v->vibrato_input = 0;
  v->tremolo_input = 0;
  v->wobble_input = 0;

  /* output level */
  v->level = 0;

  return 0;
}

/*******************************************************************************
** voice_load_patch()
*******************************************************************************/
short int voice_load_patch(int voice_index, int patch_index)
{
  int m;

  voice* v;
  patch* p;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain voice and patch pointers */
  v = &G_voice_bank[voice_index];
  p = &G_patch_bank[patch_index];

  /* algorithm */
  if ((p->algorithm >= PATCH_ALGORITHM_LOWER_BOUND) && 
      (p->algorithm <= PATCH_ALGORITHM_UPPER_BOUND))
  {
    v->algorithm = p->algorithm;
  }
  else
    v->algorithm = PATCH_ALGORITHM_LOWER_BOUND;

  /* set carrier start index */
  if ((v->algorithm == 0) || (v->algorithm == 0) || (v->algorithm == 0))
    v->num_modulators = 3;
  else if ((v->algorithm == 3) || (v->algorithm == 4))
    v->num_modulators = 2;
  else if ((v->algorithm == 5) || (v->algorithm == 6))
    v->num_modulators = 1;
  else if (v->algorithm == 7)
    v->num_modulators = 0;
  else
    v->num_modulators = 3;

  /* voice parameters, pitch index */
  for (m = 0; m < VOICE_NUM_OSCS_AND_ENVS; m++)
  {
    /* waveform */
    if ((p->osc_waveform[m] >= PATCH_OSC_WAVEFORM_LOWER_BOUND) && 
        (p->osc_waveform[m] <= PATCH_OSC_WAVEFORM_UPPER_BOUND))
    {
      v->osc_waveform[m] = p->osc_waveform[m];
    }
    else
      v->osc_waveform[m] = 0;

    /* feedback */
    if ((p->osc_feedback[m] >= PATCH_OSC_FEEDBACK_LOWER_BOUND) && 
        (p->osc_feedback[m] <= PATCH_OSC_FEEDBACK_UPPER_BOUND))
    {
      v->osc_feedback_multiplier[m] = S_voice_feedback_table[p->osc_feedback[m] - PATCH_OSC_FEEDBACK_LOWER_BOUND];
    }
    else
      v->osc_feedback_multiplier[m] = S_voice_feedback_table[0];

    /* pitch offsets */
    if ((p->osc_multiple[m] >= PATCH_OSC_MULTIPLE_LOWER_BOUND) && 
        (p->osc_multiple[m] <= PATCH_OSC_MULTIPLE_UPPER_BOUND))
    {
      v->osc_multiple[m] = S_voice_multiple_table[p->osc_multiple[m] - PATCH_OSC_MULTIPLE_LOWER_BOUND];
    }
    else
      v->osc_multiple[m] = 0;

    if ((p->osc_detune_coarse[m] >= PATCH_OSC_DETUNE_COARSE_LOWER_BOUND) && 
        (p->osc_detune_coarse[m] <= PATCH_OSC_DETUNE_COARSE_UPPER_BOUND))
    {
      v->osc_detune_coarse[m] = S_voice_detune_coarse_table[p->osc_detune_coarse[m] - PATCH_OSC_DETUNE_COARSE_LOWER_BOUND];
    }
    else
      v->osc_detune_coarse[m] = 0;

    if ((p->osc_detune_fine[m] >= PATCH_OSC_DETUNE_FINE_LOWER_BOUND) && 
        (p->osc_detune_fine[m] <= PATCH_OSC_DETUNE_FINE_UPPER_BOUND))
    {
      v->osc_detune_fine[m] = S_voice_detune_fine_table[p->osc_detune_fine[m] - PATCH_OSC_DETUNE_FINE_LOWER_BOUND];
    }
    else
      v->osc_detune_fine[m] = 0;

    /* determine oscillator note and pitch index */
    v->osc_note[m] = v->base_note + v->osc_multiple[m] + v->osc_detune_coarse[m];

    v->osc_pitch_index[m] = 
      (v->osc_note[m] * TUNING_NUM_SEMITONE_STEPS) + v->osc_detune_fine[m];

    if (v->osc_pitch_index[m] < 0)
      v->osc_pitch_index[m] = 0;
    else if (v->osc_pitch_index[m] >= TUNING_TABLE_SIZE)
      v->osc_pitch_index[m] = TUNING_TABLE_SIZE - 1;
  }

#if 0
  /* noise generator */
  if ((p->noise_period >= PATCH_NOISE_PERIOD_LOWER_BOUND) && 
      (p->noise_period <= PATCH_NOISE_PERIOD_UPPER_BOUND))
  {
    v->noise_pitch_index = 
      S_voice_noise_note_table[p->noise_period - PATCH_NOISE_PERIOD_LOWER_BOUND] * TUNING_NUM_SEMITONE_STEPS;
  }
  else
    v->noise_pitch_index = S_voice_noise_note_table[0] * TUNING_NUM_SEMITONE_STEPS;

  if ((p->noise_mode >= PATCH_NOISE_MODE_LOWER_BOUND) && 
      (p->noise_mode <= PATCH_NOISE_MODE_UPPER_BOUND))
  {
    v->noise_mode = p->noise_mode - PATCH_NOISE_MODE_LOWER_BOUND;
  }
  else
    v->noise_mode = 0;
#endif

  return 0;
}

/*******************************************************************************
** voice_set_note()
*******************************************************************************/
short int voice_set_note(int voice_index, int note)
{
  int m;

  voice* v;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain voice pointer */
  v = &G_voice_bank[voice_index];

  /* if note is out of range, ignore */
  if ((note < TUNING_NOTE_A0) || (note > TUNING_NOTE_C8))
    return 0;

  /* set base note */
  v->base_note = note;

  /* determine notes & pitch indices, reset phases */
  for (m = 0; m < VOICE_NUM_OSCS_AND_ENVS; m++)
  {
    v->osc_note[m] = v->base_note + v->osc_multiple[m] + v->osc_detune_coarse[m];

    v->osc_pitch_index[m] = 
      (v->osc_note[m] * TUNING_NUM_SEMITONE_STEPS) + v->osc_detune_fine[m];

    if (v->osc_pitch_index[m] < 0)
      v->osc_pitch_index[m] = 0;
    else if (v->osc_pitch_index[m] >= TUNING_TABLE_SIZE)
      v->osc_pitch_index[m] = TUNING_TABLE_SIZE - 1;

    v->osc_phase[m] = 0;
  }

#if 0
  /* reset noise generator */
  v->noise_phase = 0;
  v->noise_lfsr = 0x0001;
#endif

  return 0;
}

/*******************************************************************************
** voice_update_all()
*******************************************************************************/
short int voice_update_all()
{
  short int osc_env_index[VOICE_NUM_OSCS_AND_ENVS];

  int osc_fb_mod[VOICE_NUM_OSCS_AND_ENVS];
  int osc_phase_mod[VOICE_NUM_OSCS_AND_ENVS];

  int osc_adjusted_pitch_index[VOICE_NUM_OSCS_AND_ENVS];
  int osc_level[VOICE_NUM_OSCS_AND_ENVS];

  unsigned int masked_phase;
  int final_index;

  int level;

  int k;
  int m;

  voice* v;

  /* update all voices */
  for (k = 0; k < BANK_NUM_VOICES; k++)
  {
    v = &G_voice_bank[k];

    /* update envelopes */
    for (m = 0; m < VOICE_NUM_OSCS_AND_ENVS; m++)
    {
      osc_env_index[m] = v->env_input[m];

      if (m < v->num_modulators)
        osc_env_index[m] += v->wobble_input;
      else
        osc_env_index[m] += v->tremolo_input;

      if (osc_env_index[m] < 0)
        osc_env_index[m] = 0;
      else if (osc_env_index[m] > 1023)
        osc_env_index[m] = 1023;
    }

    /* update pitches & phases */
    for (m = 0; m < VOICE_NUM_OSCS_AND_ENVS; m++)
    {
      osc_adjusted_pitch_index[m] = v->osc_pitch_index[m];

      osc_adjusted_pitch_index[m] += v->vibrato_input;

      if (osc_adjusted_pitch_index[m] < 0)
        osc_adjusted_pitch_index[m] = 0;
      else if (osc_adjusted_pitch_index[m] >= TUNING_TABLE_SIZE)
        osc_adjusted_pitch_index[m] = TUNING_TABLE_SIZE - 1;

      v->osc_phase[m] += G_phase_increment_table[osc_adjusted_pitch_index[m]];

      /* wraparound phase register (28 bits) */
      if (v->osc_phase[m] > 0xFFFFFFF)
        v->osc_phase[m] &= 0xFFFFFFF;
    }

    /* update oscillator feedback */
    VOICE_UPDATE_OSCILLATOR_FEEDBACK(0)
    VOICE_UPDATE_OSCILLATOR_FEEDBACK(1)
    VOICE_UPDATE_OSCILLATOR_FEEDBACK(2)
    VOICE_UPDATE_OSCILLATOR_FEEDBACK(3)

    /* update oscillator levels based on current algorithm */

    /* 1 carrier - chain  */
    /*   1 -> 2 -> 3 -> 4 */
    if (v->algorithm == 0)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(1, 0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(2, 1)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(3, 2)

      level = osc_level[3];
    }
    /* 1 carrier - Y        */
    /*   (1 + 2) -> 3 -> 4  */
    else if (v->algorithm == 1)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(1)
      VOICE_UPDATE_OSCILLATOR_MOD_X2(2, 0, 1)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(3, 2)

      level = osc_level[3];
    }
    /* 1 carrier - crab claw  */
    /*   1 -> 2, (2 + 3) -> 4 */
    else if (v->algorithm == 2)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(1, 0)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(2)
      VOICE_UPDATE_OSCILLATOR_MOD_X2(3, 1, 2)

      level = osc_level[3];
    }
    /* 2 carriers - twin                  */
    /*   1 -> 3, 2 -> 4, output is 3 + 4  */
    else if (v->algorithm == 3)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(1)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(2, 0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(3, 1)

      level = osc_level[2] + osc_level[3];
    }
    /* 2 carriers - stacked           */
    /*   1 -> 2 -> 3, output is 3 + 4 */
    else if (v->algorithm == 4)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(1, 0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(2, 1)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(3)

      level = osc_level[2] + osc_level[3];
    }
    /* 3 carriers - one to three                      */
    /*   1 -> 2, 1 -> 3, 1 -> 4, output is 2 + 3 + 4  */
    else if (v->algorithm == 5)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(1, 0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(2, 0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(3, 0)

      level = osc_level[1] + osc_level[2] + osc_level[3];
    }
    /* 3 carriers - one to one        */
    /*   1 -> 2, output is 2 + 3 + 4  */
    else if (v->algorithm == 6)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(1, 0)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(2)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(3)

      level = osc_level[1] + osc_level[2] + osc_level[3];
    }
    /* 4 carriers - pipes         */
    /*   output is 1 + 2 + 3 + 4  */
    else if (v->algorithm == 7)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(1)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(2)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(3)

      level = osc_level[0] + osc_level[1] + osc_level[2] + osc_level[3];
    }
    else
      level = 0;

#if 0
    /* update noise phase */
    v->noise_phase += G_phase_increment_table[v->noise_pitch_index];

    /* if the noise generator has completed a period, update the lfsr */
    if (v->noise_phase > 0xFFFFFFF)
    {
      v->noise_phase &= 0xFFFFFFF;

      /* update noise generator (nes) */
      /* 15-bit lfsr, taps on 1 and 2 */
      if ((v->noise_lfsr & 0x0001) ^ ((v->noise_lfsr & 0x0002) >> 1))
        v->noise_lfsr = ((v->noise_lfsr >> 1) & 0x3FFF) | 0x4000;
      else
        v->noise_lfsr = (v->noise_lfsr >> 1) & 0x3FFF;
    }

    /* mix in noise if necessary */
    if (v->noise_mode != 0)
    {
      final_index = (osc_env_index[3] << 2);

      if (final_index < 0)
        final_index = 0;
      else if (final_index > 4095)
        final_index = 4095;

      /* replace oscillator 4's waveform with noise */
      level -= osc_level[3];

      if (v->noise_lfsr & 0x0001)
        level += G_db_to_linear_table[final_index];
      else
        level -= G_db_to_linear_table[final_index];
    }
#endif

    /* set voice level */
    v->level = level;
  }

  return 0;
}

/*******************************************************************************
** voice_generate_tables()
*******************************************************************************/
short int voice_generate_tables()
{
  int     i;
  double  val;

  /* the voice wavetables have 1024 entries per period */

  /* wavetable (sine) */
  S_voice_wavetable_sine[0] = 4095;
  S_voice_wavetable_sine[256] = 0;

  for (i = 1; i < 256; i++)
  {
    val = sin(TWO_PI * (i / 1024.0f));
    S_voice_wavetable_sine[i] = (short int) ((10 * (log(1 / val) / log(10)) / DB_STEP_12_BIT) + 0.5f);
    S_voice_wavetable_sine[512 - i] = S_voice_wavetable_sine[i];
  }

#if 0
  /* print out sine wavetable values */
  for (i = 0; i < 256; i++)
  {
    val = sin(TWO_PI * (i / 1024.0f));
    printf("Sine Wavetable Index %d: %f, %d (DB: %d)\n", 
            i, val, G_db_to_linear_table[S_voice_wavetable_sine[i]], 
                    S_voice_wavetable_sine[i]);
  }
#endif

  return 0;
}

