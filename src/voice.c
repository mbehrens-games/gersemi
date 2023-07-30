/*******************************************************************************
** voice.c (synth voice)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "clock.h"
#include "lfo.h"
#include "patch.h"
#include "sweep.h"
#include "tuning.h"
#include "voice.h"

#define PI      3.14159265358979323846f
#define TWO_PI  6.28318530717958647693f

#define DB_STEP_12_BIT 0.01171875f

#define VOICE_WAVETABLE_LOOKUP(num, phase)                                     \
  masked_phase = ((phase) & 0x3FF);                                            \
                                                                               \
  /* waveforms 1, 3: sine, full-rectified sine */                              \
  if ((v->osc_waveform[num] == 1) || (v->osc_waveform[num] == 3))              \
  {                                                                            \
    if (masked_phase < 512)                                                    \
      final_index = S_voice_wavetable_sine[masked_phase];                      \
    else                                                                       \
      final_index = S_voice_wavetable_sine[masked_phase - 512];                \
  }                                                                            \
  /* waveform 2: half-rectified sine */                                        \
  else if (v->osc_waveform[num] == 2)                                          \
  {                                                                            \
    if (masked_phase < 512)                                                    \
      final_index = S_voice_wavetable_sine[masked_phase];                      \
    else                                                                       \
      final_index = 4095;                                                      \
  }                                                                            \
  /* waveform 4: quarter sine */                                               \
  else if (v->osc_waveform[num] == 4)                                          \
  {                                                                            \
    if (masked_phase < 256)                                                    \
      final_index = S_voice_wavetable_sine[masked_phase];                      \
    else if (masked_phase < 512)                                               \
      final_index = 4095;                                                      \
    else if (masked_phase < 768)                                               \
      final_index = S_voice_wavetable_sine[masked_phase - 512];                \
    else                                                                       \
      final_index = 4095;                                                      \
  }                                                                            \
  /* waveforms 5, 6: sine, full-rectified sine (even periods only) */          \
  else if ((v->osc_waveform[num] == 5) || (v->osc_waveform[num] == 6))         \
  {                                                                            \
    if (masked_phase < 256)                                                    \
      final_index = S_voice_wavetable_sine[2 * masked_phase];                  \
    else if (masked_phase < 512)                                               \
      final_index = S_voice_wavetable_sine[2 * (masked_phase - 256)];          \
    else                                                                       \
      final_index = 4095;                                                      \
  }                                                                            \
  /* waveform 7: square */                                                     \
  else if (v->osc_waveform[num] == 7)                                          \
    final_index = 0;                                                           \
  /* waveform 8: logarithmic sawtooth */                                       \
  else if (v->osc_waveform[num] == 8)                                          \
  {                                                                            \
    if (masked_phase < 512)                                                    \
      final_index = ((511 - masked_phase) * 4095) / 512;                       \
    else                                                                       \
      final_index = ((masked_phase - 512) * 4095) / 512;                       \
  }                                                                            \
  else                                                                         \
    final_index = 4095;                                                        \
                                                                               \
  final_index += (osc_env_index[num] << 2);                                    \
                                                                               \
  if (final_index < 0)                                                         \
    final_index = 0;                                                           \
  else if (final_index > 4095)                                                 \
    final_index = 4095;                                                        \
                                                                               \
  /* waveforms 1, 7, 8: sine, square, logarithmic sawtooth */                  \
  if ((v->osc_waveform[num] == 1) ||                                           \
      (v->osc_waveform[num] == 7) ||                                           \
      (v->osc_waveform[num] == 8))                                             \
  {                                                                            \
    if (masked_phase < 512)                                                    \
      osc_level[num] = S_voice_db_to_linear_table[final_index];                \
    else                                                                       \
      osc_level[num] = -S_voice_db_to_linear_table[final_index];               \
  }                                                                            \
  /* waveforms 2, 3, 4, 6: rectified sine waves */                             \
  else if ( (v->osc_waveform[num] == 2) ||                                     \
            (v->osc_waveform[num] == 3) ||                                     \
            (v->osc_waveform[num] == 4) ||                                     \
            (v->osc_waveform[num] == 6))                                       \
  {                                                                            \
    osc_level[num] = S_voice_db_to_linear_table[final_index];                  \
  }                                                                            \
  /* waveform 5: sine (even periods only) */                                   \
  else if (v->osc_waveform[num] == 5)                                          \
  {                                                                            \
    if (masked_phase < 256)                                                    \
      osc_level[num] = S_voice_db_to_linear_table[final_index];                \
    else if (masked_phase < 512)                                               \
      osc_level[num] = -S_voice_db_to_linear_table[final_index];               \
    else                                                                       \
      osc_level[num] = S_voice_db_to_linear_table[final_index];                \
  }                                                                            \
  else                                                                         \
    osc_level[num] = S_voice_db_to_linear_table[4095];

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

/* phase shift table */
static int  S_voice_phase_shift_table[4] = 
            { 0x00000000, 0x04000000, 0x08000000, 0x0A000000};

/* multiple table */

/* the values are relative to the note played; they are   */
/* chosen to be approximately multiples of the base pitch */
static int  S_voice_multiple_table[16] = 
            { 0 * 12 + 0,   /*  1x  */
              1 * 12 + 0,   /*  2x  */
              1 * 12 + 7,   /*  3x  */
              2 * 12 + 0,   /*  4x  */
              2 * 12 + 4,   /*  5x  */
              2 * 12 + 7,   /*  6x  */
              2 * 12 + 10,  /*  7x  */
              3 * 12 + 0,   /*  8x  */
              3 * 12 + 2,   /*  9x  */
              3 * 12 + 4,   /* 10x  */
              3 * 12 + 6,   /* 11x  */
              3 * 12 + 7,   /* 12x  */
              3 * 12 + 8,   /* 13x  */
              3 * 12 + 10,  /* 14x  */
              3 * 12 + 11,  /* 15x  */
              4 * 12 + 0    /* 16x  */
            };

/* divisor table */
static int  S_voice_divisor_table[8] = 
            { 0 * 12 + 0,   /*  /1  */
              1 * 12 + 0,   /*  /2  */
              1 * 12 + 7,   /*  /3  */
              2 * 12 + 0,   /*  /4  */
              2 * 12 + 4,   /*  /5  */
              2 * 12 + 7,   /*  /6  */
              2 * 12 + 10,  /*  /7  */
              3 * 12 + 0    /*  /8  */
            };

/* detune (fine) table */
static int  S_voice_detune_table[33] = 
            { (-16 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              (-15 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              (-14 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              (-13 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              (-12 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              (-11 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              (-10 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              ( -9 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              ( -8 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              ( -7 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              ( -6 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              ( -5 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              ( -4 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              ( -3 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              ( -2 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              ( -1 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              0, 
              (  1 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              (  2 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              (  3 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              (  4 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              (  5 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              (  6 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              (  7 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              (  8 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              (  9 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              ( 10 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              ( 11 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              ( 12 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              ( 13 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              ( 14 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              ( 15 * TUNING_NUM_SEMITONE_STEPS) / 32, 
              ( 16 * TUNING_NUM_SEMITONE_STEPS) / 32
            };

/* noise mix table */

/* the values are found using the formula:            */
/*   (10 * (log(1 / val) / log(10)) / DB_STEP_10_BIT, */
/*   where DB_STEP_10_BIT = 0.046875                  */
static int  S_voice_noise_mix_table[17] = 
            {1023,  /*  0/16  */
              257,  /*  1/16  */
              193,  /*  2/16  */
              155,  /*  3/16  */
              128,  /*  4/16  */
              108,  /*  5/16  */
               91,  /*  6/16  */
               76,  /*  7/16  */
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

/* db to linear table */
static short int S_voice_db_to_linear_table[4096];

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

    v->osc_waveform[m] = 1;
    v->osc_feedback_multiplier[m] = 0;
    v->osc_sync[m] = 1;

    v->osc_freq_mode[m] = 0;
    v->osc_offset[m] = 0;
    v->osc_detune[m] = 0;

    v->vibrato_enable[m] = 1;
    v->tremolo_enable[m] = 1;
    v->boost_enable[m] = 1;
  }

  /* noise generator */
  v->noise_mix_adjustment = 1023;
  v->wave_mix_adjustment = 0;

  v->noise_pitch_index = 0;

  v->noise_phase = 0;
  v->noise_lfsr = 0x0001;

  /* envelope levels */
  for (m = 0; m < VOICE_NUM_OSCS_AND_ENVS; m++)
    v->env_input[m] = 0;

  /* lfo levels */
  v->vibrato_input = 0;
  v->tremolo_input = 0;

  /* sweep level */
  v->sweep_input = 0;

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
      v->osc_waveform[m] = PATCH_OSC_WAVEFORM_LOWER_BOUND;

    /* feedback */
    if ((p->osc_feedback[m] >= PATCH_OSC_FEEDBACK_LOWER_BOUND) && 
        (p->osc_feedback[m] <= PATCH_OSC_FEEDBACK_UPPER_BOUND))
    {
      v->osc_feedback_multiplier[m] = S_voice_feedback_table[p->osc_feedback[m] - PATCH_OSC_FEEDBACK_LOWER_BOUND];
    }
    else
      v->osc_feedback_multiplier[m] = S_voice_feedback_table[0];

    /* sync */
    if ((p->osc_sync[m] >= PATCH_OSC_SYNC_LOWER_BOUND) && 
        (p->osc_sync[m] <= PATCH_OSC_SYNC_UPPER_BOUND))
    {
      v->osc_sync[m] = p->osc_sync[m];
    }
    else
      v->osc_sync[m] = PATCH_OSC_SYNC_LOWER_BOUND;

    /* frequency mode */
    if ((p->osc_freq_mode[m] >= PATCH_OSC_FREQ_MODE_LOWER_BOUND) && 
        (p->osc_freq_mode[m] <= PATCH_OSC_FREQ_MODE_UPPER_BOUND))
    {
      v->osc_freq_mode[m] = p->osc_freq_mode[m];
    }
    else
      v->osc_freq_mode[m] = PATCH_OSC_FREQ_MODE_LOWER_BOUND;

    /* note offset */

    /* mode 0: ratio (multiple and divisor) */
    if (p->osc_freq_mode[m] == 0)
    {
      v->osc_offset[m] = 0;

      if ((p->osc_multiple_or_octave[m] >= PATCH_OSC_MULTIPLE_LOWER_BOUND) && 
          (p->osc_multiple_or_octave[m] <= PATCH_OSC_MULTIPLE_UPPER_BOUND))
      {
        v->osc_offset[m] += S_voice_multiple_table[p->osc_multiple_or_octave[m] - PATCH_OSC_MULTIPLE_LOWER_BOUND];
      }

      if ((p->osc_divisor_or_note[m] >= PATCH_OSC_DIVISOR_LOWER_BOUND) && 
          (p->osc_divisor_or_note[m] <= PATCH_OSC_DIVISOR_UPPER_BOUND))
      {
        v->osc_offset[m] -= S_voice_divisor_table[p->osc_divisor_or_note[m] - PATCH_OSC_DIVISOR_LOWER_BOUND];
      }
    }
    /* mode 1: fixed (octave and note) */
    else if (p->osc_freq_mode[m] == 1)
    {
      v->osc_offset[m] = 0;

      if ((p->osc_multiple_or_octave[m] >= PATCH_OSC_OCTAVE_LOWER_BOUND) && 
          (p->osc_multiple_or_octave[m] <= PATCH_OSC_OCTAVE_UPPER_BOUND))
      {
        v->osc_offset[m] += 12 * (p->osc_multiple_or_octave[m] - PATCH_OSC_OCTAVE_LOWER_BOUND);
      }

      if ((p->osc_divisor_or_note[m] >= PATCH_OSC_NOTE_LOWER_BOUND) && 
          (p->osc_divisor_or_note[m] <= PATCH_OSC_NOTE_UPPER_BOUND))
      {
        v->osc_offset[m] += p->osc_divisor_or_note[m] - PATCH_OSC_NOTE_LOWER_BOUND;
      }
    }
    else
      v->osc_offset[m] = 0;

    /* if the waveform is sine (even periods only)  */
    /* or full-rectified sine (even periods only),  */
    /* then add an additional division by 2.        */
    if ((p->osc_waveform[m] == 5) || (p->osc_waveform[m] == 6))
    {
      v->osc_offset[m] -= S_voice_divisor_table[1];
    }

    /* detune */
    if ((p->osc_detune[m] >= PATCH_OSC_DETUNE_LOWER_BOUND) && 
        (p->osc_detune[m] <= PATCH_OSC_DETUNE_UPPER_BOUND))
    {
      v->osc_detune[m] = S_voice_detune_table[p->osc_detune[m] - PATCH_OSC_DETUNE_LOWER_BOUND];
    }
    else
      v->osc_detune[m] = 0;

    /* determine oscillator note and pitch index */
    if (v->osc_freq_mode[m] == 0)
      v->osc_note[m] = v->base_note + v->osc_offset[m];
    else if (v->osc_freq_mode[m] == 1)
      v->osc_note[m] = v->osc_offset[m];
    else
      v->osc_note[m] = v->base_note;

    v->osc_pitch_index[m] = 
      (v->osc_note[m] * TUNING_NUM_SEMITONE_STEPS) + v->osc_detune[m];

    if (v->osc_pitch_index[m] < 0)
      v->osc_pitch_index[m] = 0;
    else if (v->osc_pitch_index[m] >= TUNING_NUM_INDICES)
      v->osc_pitch_index[m] = TUNING_NUM_INDICES - 1;

    /* vibrato enable */
    if ((p->vibrato_enable[m] >= PATCH_MOD_ENABLE_LOWER_BOUND) && 
        (p->vibrato_enable[m] <= PATCH_MOD_ENABLE_UPPER_BOUND))
    {
      v->vibrato_enable[m] = p->vibrato_enable[m];
    }
    else
      v->vibrato_enable[m] = PATCH_MOD_ENABLE_LOWER_BOUND;

    /* tremolo enable */
    if ((p->tremolo_enable[m] >= PATCH_MOD_ENABLE_LOWER_BOUND) && 
        (p->tremolo_enable[m] <= PATCH_MOD_ENABLE_UPPER_BOUND))
    {
      v->tremolo_enable[m] = p->tremolo_enable[m];
    }
    else
      v->tremolo_enable[m] = PATCH_MOD_ENABLE_LOWER_BOUND;

    /* boost enable */
    if ((p->boost_enable[m] >= PATCH_MOD_ENABLE_LOWER_BOUND) && 
        (p->boost_enable[m] <= PATCH_MOD_ENABLE_UPPER_BOUND))
    {
      v->boost_enable[m] = p->boost_enable[m];
    }
    else
      v->boost_enable[m] = PATCH_MOD_ENABLE_LOWER_BOUND;
  }

  /* noise mix */
  if ((p->noise_mix >= PATCH_NOISE_MIX_LOWER_BOUND) && 
      (p->noise_mix <= PATCH_NOISE_MIX_UPPER_BOUND))
  {
    v->noise_mix_adjustment = S_voice_noise_mix_table[p->noise_mix - PATCH_NOISE_MIX_LOWER_BOUND];
    v->wave_mix_adjustment = S_voice_noise_mix_table[16 - p->noise_mix + PATCH_NOISE_MIX_LOWER_BOUND];
  }
  else
  {
    v->noise_mix_adjustment = S_voice_noise_mix_table[0];
    v->wave_mix_adjustment = S_voice_noise_mix_table[16];
  }

  /* noise frequency */
  if ((p->noise_frequency >= PATCH_NOISE_FREQUENCY_LOWER_BOUND) && 
      (p->noise_frequency <= PATCH_NOISE_FREQUENCY_UPPER_BOUND))
  {
    v->noise_pitch_index = 
      S_voice_noise_note_table[p->noise_frequency - PATCH_NOISE_FREQUENCY_LOWER_BOUND] * TUNING_NUM_SEMITONE_STEPS;
  }
  else
    v->noise_pitch_index = S_voice_noise_note_table[0] * TUNING_NUM_SEMITONE_STEPS;

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
    if (v->osc_freq_mode[m] == 0)
      v->osc_note[m] = v->base_note + v->osc_offset[m];
    else if (v->osc_freq_mode[m] == 1)
      v->osc_note[m] = v->osc_offset[m];
    else
      v->osc_note[m] = v->base_note;

    v->osc_pitch_index[m] = 
      (v->osc_note[m] * TUNING_NUM_SEMITONE_STEPS) + v->osc_detune[m];

    if (v->osc_pitch_index[m] < 0)
      v->osc_pitch_index[m] = 0;
    else if (v->osc_pitch_index[m] >= TUNING_NUM_INDICES)
      v->osc_pitch_index[m] = TUNING_NUM_INDICES - 1;

    if ((v->osc_sync[m] >  PATCH_OSC_SYNC_LOWER_BOUND) && 
        (v->osc_sync[m] <= PATCH_OSC_SYNC_UPPER_BOUND))
    {
      v->osc_phase[m] = S_voice_phase_shift_table[v->osc_sync[m] - PATCH_OSC_SYNC_LOWER_BOUND - 1];
    }
  }

  /* reset noise generator */
  v->noise_phase = 0;
  v->noise_lfsr = 0x0001;

  return 0;
}

/*******************************************************************************
** voice_update_all()
*******************************************************************************/
short int voice_update_all()
{
  short int osc_env_index[VOICE_NUM_OSCS_AND_ENVS];
  short int noise_env_index;

  int osc_fb_mod[VOICE_NUM_OSCS_AND_ENVS];
  int osc_phase_mod[VOICE_NUM_OSCS_AND_ENVS];

  int osc_level[VOICE_NUM_OSCS_AND_ENVS];

  int adjusted_pitch_index;

  int octave;
  int increment_index;

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

      if (v->tremolo_enable[m] == 1)
        osc_env_index[m] += v->tremolo_input;

      if (osc_env_index[m] < 0)
        osc_env_index[m] = 0;
      else if (osc_env_index[m] > 1023)
        osc_env_index[m] = 1023;
    }

    /* update pitches & phases */
    for (m = 0; m < VOICE_NUM_OSCS_AND_ENVS; m++)
    {
      adjusted_pitch_index = v->osc_pitch_index[m];

      if (v->vibrato_enable[m] == 1)
        adjusted_pitch_index += v->vibrato_input;

      adjusted_pitch_index += v->sweep_input;

      if (adjusted_pitch_index < 0)
        adjusted_pitch_index = 0;
      else if (adjusted_pitch_index >= TUNING_NUM_INDICES)
        adjusted_pitch_index = TUNING_NUM_INDICES - 1;

      octave = adjusted_pitch_index / TUNING_TABLE_SIZE;
      increment_index = adjusted_pitch_index % TUNING_TABLE_SIZE;

      if (octave < 9)
        v->osc_phase[m] += G_tuning_phase_increment_table[increment_index] >> (9 - octave);
      else
        v->osc_phase[m] += G_tuning_phase_increment_table[increment_index];

      /* wraparound phase register (28 bits) */
      if (v->osc_phase[m] > 0xFFFFFFF)
        v->osc_phase[m] &= 0xFFFFFFF;
    }

    /* update noise phase */
    octave = v->noise_pitch_index / TUNING_TABLE_SIZE;
    increment_index = v->noise_pitch_index % TUNING_TABLE_SIZE;

    if (octave < 9)
      v->noise_phase += G_tuning_phase_increment_table[increment_index] >> (9 - octave);
    else
      v->noise_phase += G_tuning_phase_increment_table[increment_index];

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

    /* add in noise mix adjustments to envelopes */
    noise_env_index = osc_env_index[3];

    noise_env_index += v->noise_mix_adjustment;
    osc_env_index[3] += v->wave_mix_adjustment;

    if (noise_env_index < 0)
      noise_env_index = 0;
    else if (noise_env_index > 1023)
      noise_env_index = 1023;

    if (osc_env_index[3] < 0)
      osc_env_index[3] = 0;
    else if (osc_env_index[3] > 1023)
      osc_env_index[3] = 1023;

    /* update oscillator feedback */
    VOICE_UPDATE_OSCILLATOR_FEEDBACK(0)
    VOICE_UPDATE_OSCILLATOR_FEEDBACK(1)
    VOICE_UPDATE_OSCILLATOR_FEEDBACK(2)
    VOICE_UPDATE_OSCILLATOR_FEEDBACK(3)

    /* update oscillator levels based on current algorithm */

    /* 1 carrier - chain  */
    /*   1 -> 2 -> 3 -> 4 */
    if (v->algorithm == 1)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(1, 0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(2, 1)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(3, 2)

      level = osc_level[3];
    }
    /* 1 carrier - Y        */
    /*   (1 + 2) -> 3 -> 4  */
    else if (v->algorithm == 2)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(1)
      VOICE_UPDATE_OSCILLATOR_MOD_X2(2, 0, 1)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(3, 2)

      level = osc_level[3];
    }
    /* 1 carrier - crab claw  */
    /*   1 -> 2, (2 + 3) -> 4 */
    else if (v->algorithm == 3)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(1, 0)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(2)
      VOICE_UPDATE_OSCILLATOR_MOD_X2(3, 1, 2)

      level = osc_level[3];
    }
    /* 2 carriers - twin                  */
    /*   1 -> 3, 2 -> 4, output is 3 + 4  */
    else if (v->algorithm == 4)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(1)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(2, 0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(3, 1)

      level = osc_level[2] + osc_level[3];
    }
    /* 2 carriers - stacked           */
    /*   1 -> 2 -> 3, output is 3 + 4 */
    else if (v->algorithm == 5)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(1, 0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(2, 1)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(3)

      level = osc_level[2] + osc_level[3];
    }
    /* 3 carriers - one to three                      */
    /*   1 -> 2, 1 -> 3, 1 -> 4, output is 2 + 3 + 4  */
    else if (v->algorithm == 6)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(1, 0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(2, 0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(3, 0)

      level = osc_level[1] + osc_level[2] + osc_level[3];
    }
    /* 3 carriers - one to one        */
    /*   1 -> 2, output is 2 + 3 + 4  */
    else if (v->algorithm == 7)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(1, 0)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(2)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(3)

      level = osc_level[1] + osc_level[2] + osc_level[3];
    }
    /* 4 carriers - pipes         */
    /*   output is 1 + 2 + 3 + 4  */
    else if (v->algorithm == 8)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(1)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(2)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(3)

      level = osc_level[0] + osc_level[1] + osc_level[2] + osc_level[3];
    }
    else
      level = 0;

    /* mix in noise */
    if (v->noise_lfsr & 0x0001)
      level += S_voice_db_to_linear_table[noise_env_index << 2];
    else
      level -= S_voice_db_to_linear_table[noise_env_index << 2];

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
  /* print out db to linear table values */
  for (i = 0; i < 4096; i += 4)
  {
    printf("DB to Linear Table Index %d: %d\n", i, S_voice_db_to_linear_table[i]);
  }
#endif

#if 0
  /* print out sine wavetable values */
  for (i = 0; i < 256; i++)
  {
    val = sin(TWO_PI * (i / 1024.0f));
    printf("Sine Wavetable Index %d: %f, %d (DB: %d)\n", 
            i, val, S_voice_db_to_linear_table[S_voice_wavetable_sine[i]], 
                    S_voice_wavetable_sine[i]);
  }
#endif

  return 0;
}

