/*******************************************************************************
** voice.c (synth voice)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "clock.h"
#include "dbstep.h"
#include "envelope.h"
#include "filter.h"
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
  {                                                                            \
    final_index = S_voice_wavetable_sine[masked_phase];                        \
    final_index += (osc_env_index[num] << 2);                                  \
                                                                               \
    if (final_index < 0)                                                       \
      final_index = 0;                                                         \
    else if (final_index > 4095)                                               \
      final_index = 4095;                                                      \
                                                                               \
    osc_level[num] = S_voice_db_to_linear_table[final_index];                  \
  }                                                                            \
  else                                                                         \
  {                                                                            \
    final_index = S_voice_wavetable_sine[masked_phase - 512];                  \
    final_index += (osc_env_index[num] << 2);                                  \
                                                                               \
    if (final_index < 0)                                                       \
      final_index = 0;                                                         \
    else if (final_index > 4095)                                               \
      final_index = 4095;                                                      \
                                                                               \
    osc_level[num] = -S_voice_db_to_linear_table[final_index];                 \
  }

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
  /* cycle feedback                                             */                      \
  /* note that the feedback values from 0-8 are divided by 16,  */                      \
  /* so the max feedback is 8/16 = 1/2 of the oscillator level. */                      \
  v->feed_in[2 * num + 1] = v->feed_in[2 * num + 0];                                    \
  v->feed_in[2 * num + 0] = (osc_level[num] * v->osc_feedback[num]) >> 4;

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

#if 0
/* phase offset table     */
/* entries (in degrees):  */
/*     0,  30,  45,  60   */
/*    90, 120, 135, 150   */
/*   180, 210, 225, 240   */
/*   270, 300, 315, 330   */
static int  S_phi_table[16] = 
            { 0x00000000, 0x01555555, 0x02000000, 0x02AAAAAA, 
              0x04000000, 0x05555555, 0x06000000, 0x06AAAAAA, 
              0x08000000, 0x09555555, 0x0A000000, 0x0AAAAAAA, 
              0x0C000000, 0x0D555555, 0x0E000000, 0x0EAAAAAA 
            };
#endif

/* multiple table */

/* the values are relative to the note played; they are   */
/* chosen to be approximately multiples of the base pitch */
static int  S_voice_multiple_table[16] = 
            { 0 * 12 + 0,   /* note:       1x */
              1 * 12 + 0,   /* 1 octave:   2x */
              1 * 12 + 7,   /*             3x */
              2 * 12 + 0,   /* 2 octaves:  4x */
              2 * 12 + 4,   /*             5x */
              2 * 12 + 7,   /*             6x */
              2 * 12 + 10,  /*             7x */
              3 * 12 + 0,   /* 3 octaves:  8x */
              3 * 12 + 2,   /*             9x */
              3 * 12 + 4,   /*            10x */
              3 * 12 + 6,   /*            11x */
              3 * 12 + 7,   /*            12x */
              3 * 12 + 8,   /*            13x */
              3 * 12 + 10,  /*            14x */
              3 * 12 + 11,  /*            15x */
              4 * 12 + 0    /* 4 octaves: 16x */
            };

/* detune table */
static int  S_voice_detune_table[17] = 
            { (-24 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              (-21 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              (-18 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              (-15 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              (-12 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              ( -9 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              ( -6 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              ( -3 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              0, 
              (  3 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              (  6 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              (  9 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              ( 12 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              ( 15 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              ( 18 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              ( 21 * TUNING_NUM_SEMITONE_STEPS) / 64, 
              ( 24 * TUNING_NUM_SEMITONE_STEPS) / 64
            };

#if 0
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
#endif

/* tables */
static short int S_voice_db_to_linear_table[4096];

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
  v->algorithm = VOICE_ALGORITHM_1_CAR_CHAIN;

  /* base note */
  v->base_note = 0;

  /* notes, base pitch table indices, phases, feedback, offsets */
  for (m = 0; m < VOICE_NUM_OSCS_AND_ENVS; m++)
  {
    v->osc_note[m] = 0;
    v->osc_base_pitch_index[m] = 0;
    v->osc_phase[m] = 0;
    v->osc_offset_coarse[m] = 0;
    v->osc_offset_fine[m] = 0;
    v->osc_feedback[m] = 0;

    v->feed_in[2 * m + 0] = 0;
    v->feed_in[2 * m + 1] = 0;
  }

#if 0
  /* noise generator */
  v->noise_period = S_voice_noise_note_table[0];
  v->noise_lfsr = 0x0001;
#endif

  /* envelope levels */
  for (m = 0; m < VOICE_NUM_OSCS_AND_ENVS; m++)
    v->env_input[m] = 0;

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
  if ((p->algorithm >= 0) && (p->algorithm < VOICE_NUM_ALGORITHMS))
    v->algorithm = p->algorithm;
  else
    v->algorithm = VOICE_ALGORITHM_1_CAR_CHAIN;

  /* feedback, pitch offsets, current notes */
  for (m = 0; m < VOICE_NUM_OSCS_AND_ENVS; m++)
  {
    /* feedback */
    if ((p->osc_feedback[m] >= PATCH_OSC_FEEDBACK_LOWER_BOUND) && 
        (p->osc_feedback[m] <= PATCH_OSC_FEEDBACK_UPPER_BOUND))
    {
      v->osc_feedback[m] = p->osc_feedback[m];
    }
    else
      v->osc_feedback[m] = PATCH_OSC_FEEDBACK_LOWER_BOUND;

    /* pitch offsets */
    if ((p->osc_multiple[m] >= PATCH_OSC_MULTIPLE_LOWER_BOUND) && 
        (p->osc_multiple[m] <= PATCH_OSC_MULTIPLE_UPPER_BOUND))
    {
      v->osc_offset_coarse[m] = S_voice_multiple_table[p->osc_multiple[m] - 1];
    }
    else
      v->osc_offset_coarse[m] = S_voice_multiple_table[0];

    if ((p->osc_detune[m] >= PATCH_OSC_DETUNE_LOWER_BOUND) && 
        (p->osc_detune[m] <= PATCH_OSC_DETUNE_UPPER_BOUND))
    {
      v->osc_offset_fine[m] = S_voice_detune_table[8 + p->osc_detune[m]];
    }
    else
      v->osc_offset_fine[m] = 0;

    /* current notes */
    v->osc_note[m] = v->base_note + v->osc_offset_coarse[m];

    v->osc_base_pitch_index[m] = 
      (v->osc_note[m] * TUNING_NUM_SEMITONE_STEPS) + v->osc_offset_fine[m];

    if (v->osc_base_pitch_index[m] < 0)
      v->osc_base_pitch_index[m] = 0;
    else if (v->osc_base_pitch_index[m] >= TUNING_TABLE_SIZE)
      v->osc_base_pitch_index[m] = TUNING_TABLE_SIZE - 1;
  }

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
  if ((note < 0) || (note >= TUNING_NUM_NOTES))
    return 0;

  /* set base note */
  v->base_note = note;

  /* determine notes & base pitch indices, reset phases */
  for (m = 0; m < VOICE_NUM_OSCS_AND_ENVS; m++)
  {
    v->osc_note[m] = v->base_note + v->osc_offset_coarse[m];

    v->osc_base_pitch_index[m] = 
      (v->osc_note[m] * TUNING_NUM_SEMITONE_STEPS) + v->osc_offset_fine[m];

    if (v->osc_base_pitch_index[m] < 0)
      v->osc_base_pitch_index[m] = 0;
    else if (v->osc_base_pitch_index[m] >= TUNING_TABLE_SIZE)
      v->osc_base_pitch_index[m] = TUNING_TABLE_SIZE - 1;

    v->osc_phase[m] = 0;
  }

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

  int osc_pitch_index[VOICE_NUM_OSCS_AND_ENVS];
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

      /*if (v->osc_env[m].type == ENVELOPE_TYPE_CARRIER)
        osc_env_index[m] += v->tremolo.level;
      else if (v->osc_env[m].type == ENVELOPE_TYPE_MODULATOR)
        osc_env_index[m] += v->wobble.level;*/

      if (osc_env_index[m] < 0)
        osc_env_index[m] = 0;
      else if (osc_env_index[m] > 1023)
        osc_env_index[m] = 1023;
    }

    /* update pitches & phases */
    for (m = 0; m < VOICE_NUM_OSCS_AND_ENVS; m++)
    {
      osc_pitch_index[m] = v->osc_base_pitch_index[m];
      /*osc_pitch_index[m] += v->vibrato.level;*/
      /*osc_pitch_index[m] += v->pitch_sweep.level;*/

      if (osc_pitch_index[m] < 0)
        osc_pitch_index[m] = 0;
      else if (osc_pitch_index[m] >= TUNING_TABLE_SIZE)
        osc_pitch_index[m] = TUNING_TABLE_SIZE - 1;

      v->osc_phase[m] += G_phase_increment_table[osc_pitch_index[m]];

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
    if (v->algorithm == VOICE_ALGORITHM_1_CAR_CHAIN)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(1, 0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(2, 1)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(3, 2)

      level = osc_level[3];
    }
    /* 1 carrier - Y        */
    /*   (1 + 2) -> 3 -> 4  */
    else if (v->algorithm == VOICE_ALGORITHM_1_CAR_Y)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(1)
      VOICE_UPDATE_OSCILLATOR_MOD_X2(2, 0, 1)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(3, 2)

      level = osc_level[3];
    }
    /* 1 carrier - crab claw  */
    /*   1 -> 2, (2 + 3) -> 4 */
    else if (v->algorithm == VOICE_ALGORITHM_1_CAR_CRAB_CLAW)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(1, 0)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(2)
      VOICE_UPDATE_OSCILLATOR_MOD_X2(3, 1, 2)

      level = osc_level[3];
    }
    /* 2 carriers - twin                  */
    /*   1 -> 3, 2 -> 4, output is 3 + 4  */
    else if (v->algorithm == VOICE_ALGORITHM_2_CAR_TWIN)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(1)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(2, 0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(3, 1)

      level = osc_level[2] + osc_level[3];
    }
    /* 2 carriers - stacked           */
    /*   1 -> 2 -> 3, output is 3 + 4 */
    else if (v->algorithm == VOICE_ALGORITHM_2_CAR_STACKED)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(1, 0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(2, 1)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(3)

      level = osc_level[2] + osc_level[3];
    }
    /* 3 carriers - one to three                      */
    /*   1 -> 2, 1 -> 3, 1 -> 4, output is 2 + 3 + 4  */
    else if (v->algorithm == VOICE_ALGORITHM_3_CAR_ONE_TO_THREE)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(1, 0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(2, 0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(3, 0)

      level = osc_level[1] + osc_level[2] + osc_level[3];
    }
    /* 3 carriers - one to one        */
    /*   1 -> 2, output is 2 + 3 + 4  */
    else if (v->algorithm == VOICE_ALGORITHM_3_CAR_ONE_TO_ONE)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(1, 0)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(2)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(3)

      level = osc_level[1] + osc_level[2] + osc_level[3];
    }
    /* 4 carriers - pipes         */
    /*   output is 1 + 2 + 3 + 4  */
    else if (v->algorithm == VOICE_ALGORITHM_4_CAR_PIPES)
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
    v->noise_phase += G_phase_increment_table[v->noise_base_pitch_index];

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

    /* compute noise level */
    noise_level = waveform_noise_lookup(v->noise_lfsr, osc_env_index[0]);

    /* mix in noise */
    level += noise_level;
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

  /* ym2612 - 10 bit envelope (shifted to 12 bit), 12 bit sine, 13 bit sum    */
  /* 10 bit db: 24, 12, 6, 3, 1.5, 0.75, 0.375, 0.1875, 0.09375, 0.046875     */
  /* 12 bit db: adds on 0.0234375, 0.01171875 in back                         */
  /* 13 bit db: adds on 48 in front                                           */

  /* db to linear scale conversion */
  S_voice_db_to_linear_table[0] = 32767;

  for (i = 1; i < 4095; i++)
  {
    S_voice_db_to_linear_table[i] = 
      (short int) ((32767.0f * exp(-log(10) * (DB_STEP_12_BIT / 10) * i)) + 0.5f);
  }

  S_voice_db_to_linear_table[4095] = 0;

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
  /* wavetable (triangle) */
  S_voice_wavetable_tri[0] = 4095;
  S_voice_wavetable_tri[256] = 0;

  for (i = 1; i < 256; i++)
  {
    val = i / 256.0f;
    S_voice_wavetable_tri[i] = (short int) ((10 * (log(1 / val) / log(10)) / DB_STEP_12_BIT) + 0.5f);
    S_voice_wavetable_tri[512 - i] = S_voice_wavetable_tri[i];
  }

  /* wavetable (sawtooth) */
  S_voice_wavetable_saw[0] = 4095;
  S_voice_wavetable_saw[512] = 0;

  for (i = 1; i < 512; i++)
  {
    val = i / 512.0f;
    S_voice_wavetable_saw[i] = (short int) ((10 * (log(1 / val) / log(10)) / DB_STEP_12_BIT) + 0.5f);
    S_voice_wavetable_saw[1024 - i] = S_voice_wavetable_saw[i];
  }
#endif

  return 0;
}

