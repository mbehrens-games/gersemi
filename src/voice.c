/*******************************************************************************
** voice.c (synth voice)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "clock.h"
#include "dbstep.h"
#include "lfo.h"
#include "midicont.h"
#include "patch.h"
#include "sweep.h"
#include "tuning.h"
#include "voice.h"

#define PI      3.14159265358979323846f
#define TWO_PI  6.28318530717958647693f

#define VOICE_BASE_NOISE_FREQUENCY 440.0f /* A-4 */

#define VOICE_VELOCITY_LEVEL_STEP 32

#define VOICE_WAVETABLE_LOOKUP(num, phase)                                     \
  masked_phase = ((phase) & 0x3FF);                                            \
                                                                               \
  /* determine waveform level (db) */                                          \
  if ((v->osc_waveform[num] == PATCH_OSC_WAVEFORM_SINE) ||                     \
      (v->osc_waveform[num] == PATCH_OSC_WAVEFORM_FULL))                       \
  {                                                                            \
    if (masked_phase < 512)                                                    \
      final_index = S_voice_wavetable_sine[masked_phase];                      \
    else                                                                       \
      final_index = S_voice_wavetable_sine[masked_phase - 512];                \
  }                                                                            \
  else if (v->osc_waveform[num] == PATCH_OSC_WAVEFORM_HALF)                    \
  {                                                                            \
    if (masked_phase < 512)                                                    \
      final_index = S_voice_wavetable_sine[masked_phase];                      \
    else                                                                       \
      final_index = 4095;                                                      \
  }                                                                            \
  else if (v->osc_waveform[num] == PATCH_OSC_WAVEFORM_QUARTER)                 \
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
  else if ( (v->osc_waveform[num] == PATCH_OSC_WAVEFORM_ALTERNATING) ||        \
            (v->osc_waveform[num] == PATCH_OSC_WAVEFORM_CAMEL))                \
  {                                                                            \
    if (masked_phase < 256)                                                    \
      final_index = S_voice_wavetable_sine[2 * masked_phase];                  \
    else if (masked_phase < 512)                                               \
      final_index = S_voice_wavetable_sine[2 * (masked_phase - 256)];          \
    else                                                                       \
      final_index = 4095;                                                      \
  }                                                                            \
  else if (v->osc_waveform[num] == PATCH_OSC_WAVEFORM_SQUARE)                  \
    final_index = 0;                                                           \
  else if (v->osc_waveform[num] == PATCH_OSC_WAVEFORM_LOG_SAW)                 \
  {                                                                            \
    if (masked_phase < 512)                                                    \
      final_index = ((511 - masked_phase) * 4095) / 512;                       \
    else                                                                       \
      final_index = ((masked_phase - 512) * 4095) / 512;                       \
  }                                                                            \
  else                                                                         \
    final_index = 4095;                                                        \
                                                                               \
  /* apply envelope */                                                         \
  final_index += (osc_env_index[num] << 2);                                    \
                                                                               \
  if (final_index < 0)                                                         \
    final_index = 0;                                                           \
  else if (final_index > 4095)                                                 \
    final_index = 4095;                                                        \
                                                                               \
  /* determine waveform level (linear) */                                      \
  if ((v->osc_waveform[num] == PATCH_OSC_WAVEFORM_SINE)   ||                   \
      (v->osc_waveform[num] == PATCH_OSC_WAVEFORM_SQUARE) ||                   \
      (v->osc_waveform[num] == PATCH_OSC_WAVEFORM_LOG_SAW))                    \
  {                                                                            \
    if (masked_phase < 512)                                                    \
      osc_level[num] = S_voice_db_to_linear_table[final_index];                \
    else                                                                       \
      osc_level[num] = -S_voice_db_to_linear_table[final_index];               \
  }                                                                            \
  else if ( (v->osc_waveform[num] == PATCH_OSC_WAVEFORM_HALF)     ||           \
            (v->osc_waveform[num] == PATCH_OSC_WAVEFORM_FULL)     ||           \
            (v->osc_waveform[num] == PATCH_OSC_WAVEFORM_QUARTER)  ||           \
            (v->osc_waveform[num] == PATCH_OSC_WAVEFORM_CAMEL))                \
  {                                                                            \
    osc_level[num] = S_voice_db_to_linear_table[final_index];                  \
  }                                                                            \
  else if (v->osc_waveform[num] == PATCH_OSC_WAVEFORM_ALTERNATING)             \
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
static int  S_voice_feedback_table[PATCH_OSC_FEEDBACK_NUM_VALUES] = 
            { 0, 1, 2, 4, 8, 16, 32, 64};

/* phase shift table */
static int  S_voice_phase_shift_table[PATCH_OSC_PHI_NUM_VALUES] = 
            { 0x00000000, 0x01555555, 0x02000000, 0x02AAAAAA, /*   0,  30,  45,  60 */
              0x04000000, 0x05555555, 0x06000000, 0x06AAAAAA, /*  90, 120, 135, 150 */
              0x08000000, 0x09555555, 0x0A000000, 0x0AAAAAAA, /* 180, 210, 225, 240 */
              0x0C000000, 0x0D555555, 0x0E000000, 0x0EAAAAAA  /* 270, 300, 315, 330 */
            };

/* multiple table */

/* the values are in semitones    */
/* they form the harmonic series! */
static int  S_voice_multiple_table[PATCH_OSC_MULTIPLE_NUM_VALUES] = 
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

/* octave table */
static int  S_voice_octave_table[PATCH_OSC_OCTAVE_NUM_VALUES] = 
            { -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

/* detune table */
static int  S_voice_detune_table[PATCH_OSC_DETUNE_NUM_VALUES];

/* noise frequency table (in lfsr updates per second) */
static float  S_voice_noise_frequency_table[PATCH_NOISE_FREQUENCY_NUM_VALUES] = 
              { VOICE_BASE_NOISE_FREQUENCY * 1, 
                VOICE_BASE_NOISE_FREQUENCY * 1 * 1.25f, 
                VOICE_BASE_NOISE_FREQUENCY * 1 * 1.50f, 
                VOICE_BASE_NOISE_FREQUENCY * 1 * 1.75f, 
                VOICE_BASE_NOISE_FREQUENCY * 2, 
                VOICE_BASE_NOISE_FREQUENCY * 2 * 1.25f, 
                VOICE_BASE_NOISE_FREQUENCY * 2 * 1.50f, 
                VOICE_BASE_NOISE_FREQUENCY * 2 * 1.75f, 
                VOICE_BASE_NOISE_FREQUENCY * 4, 
                VOICE_BASE_NOISE_FREQUENCY * 4 * 1.125f, 
                VOICE_BASE_NOISE_FREQUENCY * 4 * 1.250f, 
                VOICE_BASE_NOISE_FREQUENCY * 4 * 1.375f, 
                VOICE_BASE_NOISE_FREQUENCY * 4 * 1.500f, 
                VOICE_BASE_NOISE_FREQUENCY * 4 * 1.625f, 
                VOICE_BASE_NOISE_FREQUENCY * 4 * 1.750f, 
                VOICE_BASE_NOISE_FREQUENCY * 4 * 1.875f, 
                VOICE_BASE_NOISE_FREQUENCY * 8, 
                VOICE_BASE_NOISE_FREQUENCY * 8 * 1.125f, 
                VOICE_BASE_NOISE_FREQUENCY * 8 * 1.250f, 
                VOICE_BASE_NOISE_FREQUENCY * 8 * 1.375f, 
                VOICE_BASE_NOISE_FREQUENCY * 8 * 1.500f, 
                VOICE_BASE_NOISE_FREQUENCY * 8 * 1.625f, 
                VOICE_BASE_NOISE_FREQUENCY * 8 * 1.750f, 
                VOICE_BASE_NOISE_FREQUENCY * 8 * 1.875f, 
                VOICE_BASE_NOISE_FREQUENCY * 16, 
                VOICE_BASE_NOISE_FREQUENCY * 16 * 1.125f, 
                VOICE_BASE_NOISE_FREQUENCY * 16 * 1.250f, 
                VOICE_BASE_NOISE_FREQUENCY * 16 * 1.375f, 
                VOICE_BASE_NOISE_FREQUENCY * 16 * 1.500f, 
                VOICE_BASE_NOISE_FREQUENCY * 16 * 1.625f, 
                VOICE_BASE_NOISE_FREQUENCY * 16 * 1.750f, 
                VOICE_BASE_NOISE_FREQUENCY * 16 * 1.875f 
              };

/* velocity scaling table */
static short int  S_voice_velocity_scaling_table[PATCH_VELOCITY_SCALING_NUM_VALUES] = 
                  { 0,  
                    VOICE_VELOCITY_LEVEL_STEP * 1, 
                    VOICE_VELOCITY_LEVEL_STEP * 2, 
                    VOICE_VELOCITY_LEVEL_STEP * 3, 
                    VOICE_VELOCITY_LEVEL_STEP * 4, 
                    VOICE_VELOCITY_LEVEL_STEP * 5, 
                    VOICE_VELOCITY_LEVEL_STEP * 6, 
                    VOICE_VELOCITY_LEVEL_STEP * 7, 
                    VOICE_VELOCITY_LEVEL_STEP * 8 
                  };

/* db to linear table */
static short int S_voice_db_to_linear_table[4096];

/* sine wavetable */
static short int S_voice_wavetable_sine[512];

/* noise phase increment table */
static unsigned int S_voice_noise_phase_increment_table[PATCH_NOISE_FREQUENCY_NUM_VALUES];

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
  v->algorithm = PATCH_ALGORITHM_DEFAULT;

  /* sync */
  v->sync = PATCH_SYNC_DEFAULT;

  /* base note */
  v->base_note = TUNING_NOTE_BLANK;

  /* currently playing notes, pitch table indices, phase, feedback levels, voice parameters */
  for (m = 0; m < BANK_OSCS_AND_ENVS_PER_VOICE; m++)
  {
    v->osc_note[m] = 0;
    v->osc_pitch_index[m] = 0;

    v->osc_phase[m] = 0;

    v->feed_in[2 * m + 0] = 0;
    v->feed_in[2 * m + 1] = 0;

    v->osc_waveform[m] = PATCH_OSC_WAVEFORM_DEFAULT;
    v->osc_feedback_multiplier[m] = 0;
    v->osc_phi[m] = PATCH_OSC_PHI_DEFAULT;

    v->osc_freq_mode[m] = PATCH_OSC_FREQ_MODE_DEFAULT;
    v->osc_offset[m] = 0;
    v->osc_detune[m] = 0;
  }

  /* noise */
  v->noise_lfsr = 0x0001;
  v->noise_mode = PATCH_NOISE_MODE_DEFAULT;

  v->noise_phase = 0;
  v->noise_increment = S_voice_noise_phase_increment_table[PATCH_NOISE_FREQUENCY_DEFAULT - PATCH_NOISE_FREQUENCY_LOWER_BOUND];

  /* amplitude effect modes */
  v->tremolo_mode = PATCH_TREMOLO_MODE_DEFAULT;
  v->boost_mode = PATCH_BOOST_MODE_DEFAULT;
  v->velocity_mode = PATCH_VELOCITY_MODE_DEFAULT;

  /* velocity scaling, adjustment */
  v->velocity_scaling_amount = 
    S_voice_velocity_scaling_table[PATCH_VELOCITY_SCALING_DEFAULT - PATCH_VELOCITY_SCALING_LOWER_BOUND];

  v->velocity_adjustment = 0;

  /* note velocity */
  v->note_velocity = MIDI_CONT_NOTE_VELOCITY_DEFAULT;

  /* envelope levels */
  for (m = 0; m < BANK_OSCS_AND_ENVS_PER_VOICE; m++)
    v->env_input[m] = 0;

  /* lfo levels */
  v->lfo_input_vibrato = 0;
  v->lfo_input_tremolo = 0;

  /* boost level */
  v->boost_input = 0;

  /* sweep level */
  v->sweep_input = 0;

  /* bender level */
  v->bender_input = 0;

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

  /* sync */
  if ((p->sync_osc >= PATCH_SYNC_LOWER_BOUND) && 
      (p->sync_osc <= PATCH_SYNC_UPPER_BOUND))
  {
    v->sync = p->sync_osc;
  }
  else
    v->sync = PATCH_SYNC_LOWER_BOUND;

  /* voice parameters, pitch index */
  for (m = 0; m < BANK_OSCS_AND_ENVS_PER_VOICE; m++)
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

    /* phi */
    if ((p->osc_phi[m] >= PATCH_OSC_PHI_LOWER_BOUND) && 
        (p->osc_phi[m] <= PATCH_OSC_PHI_UPPER_BOUND))
    {
      v->osc_phi[m] = p->osc_phi[m];
    }
    else
      v->osc_phi[m] = PATCH_OSC_PHI_LOWER_BOUND;

   /* frequency mode */
    if ((p->osc_freq_mode[m] >= PATCH_OSC_FREQ_MODE_LOWER_BOUND) && 
        (p->osc_freq_mode[m] <= PATCH_OSC_FREQ_MODE_UPPER_BOUND))
    {
      v->osc_freq_mode[m] = p->osc_freq_mode[m];
    }
    else
      v->osc_freq_mode[m] = PATCH_OSC_FREQ_MODE_LOWER_BOUND; 

    /* note offset */
    if (p->osc_freq_mode[m] == PATCH_OSC_FREQ_MODE_RATIO)
    {
      v->osc_offset[m] = 0;

      if ((p->osc_multiple[m] >= PATCH_OSC_MULTIPLE_LOWER_BOUND) && 
          (p->osc_multiple[m] <= PATCH_OSC_MULTIPLE_UPPER_BOUND))
      {
        v->osc_offset[m] += S_voice_multiple_table[p->osc_multiple[m] - PATCH_OSC_MULTIPLE_LOWER_BOUND];
      }

      if ((p->osc_divisor[m] >= PATCH_OSC_DIVISOR_LOWER_BOUND) && 
          (p->osc_divisor[m] <= PATCH_OSC_DIVISOR_UPPER_BOUND))
      {
        v->osc_offset[m] -= S_voice_multiple_table[p->osc_divisor[m] - PATCH_OSC_DIVISOR_LOWER_BOUND];
      }
    }
    else if (p->osc_freq_mode[m] == PATCH_OSC_FREQ_MODE_FIXED)
    {
      v->osc_offset[m] = 0;

      if ((p->osc_note[m] >= PATCH_OSC_NOTE_LOWER_BOUND) && 
          (p->osc_note[m] <= PATCH_OSC_NOTE_UPPER_BOUND))
      {
        v->osc_offset[m] += p->osc_note[m] - PATCH_OSC_NOTE_LOWER_BOUND;
      }

      if ((p->osc_octave[m] >= PATCH_OSC_OCTAVE_LOWER_BOUND) && 
          (p->osc_octave[m] <= PATCH_OSC_OCTAVE_UPPER_BOUND))
      {
        v->osc_offset[m] += 12 * S_voice_octave_table[p->osc_octave[m] - PATCH_OSC_OCTAVE_LOWER_BOUND];
      }
    }
    else
      v->osc_offset[m] = 0;

    /* if the waveform is sine (even periods only)  */
    /* or full-rectified sine (even periods only),  */
    /* then add an additional division by 2.        */
    if ((p->osc_waveform[m] == PATCH_OSC_WAVEFORM_ALTERNATING) || 
        (p->osc_waveform[m] == PATCH_OSC_WAVEFORM_CAMEL))
    {
      v->osc_offset[m] -= S_voice_multiple_table[1];
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
    if (v->osc_freq_mode[m] == PATCH_OSC_FREQ_MODE_RATIO)
      v->osc_note[m] = v->base_note + v->osc_offset[m];
    else if (v->osc_freq_mode[m] == PATCH_OSC_FREQ_MODE_FIXED)
      v->osc_note[m] = v->osc_offset[m];
    else
      v->osc_note[m] = v->base_note;

    v->osc_pitch_index[m] = 
      (v->osc_note[m] * TUNING_NUM_SEMITONE_STEPS) + v->osc_detune[m];

    if (v->osc_pitch_index[m] < 0)
      v->osc_pitch_index[m] = 0;
    else if (v->osc_pitch_index[m] >= TUNING_NUM_INDICES)
      v->osc_pitch_index[m] = TUNING_NUM_INDICES - 1;
  }

  /* tremolo mode */
  if ((p->tremolo_mode >= PATCH_TREMOLO_MODE_LOWER_BOUND) && 
      (p->tremolo_mode <= PATCH_TREMOLO_MODE_UPPER_BOUND))
  {
    v->tremolo_mode = p->tremolo_mode;
  }
  else
    v->tremolo_mode = PATCH_TREMOLO_MODE_LOWER_BOUND;

  /* boost mode */
  if ((p->boost_mode >= PATCH_BOOST_MODE_LOWER_BOUND) && 
      (p->boost_mode <= PATCH_BOOST_MODE_UPPER_BOUND))
  {
    v->boost_mode = p->boost_mode;
  }
  else
    v->boost_mode = PATCH_BOOST_MODE_LOWER_BOUND;

  /* velocity mode */
  if ((p->velocity_mode >= PATCH_VELOCITY_MODE_LOWER_BOUND) && 
      (p->velocity_mode <= PATCH_VELOCITY_MODE_UPPER_BOUND))
  {
    v->velocity_mode = p->velocity_mode;
  }
  else
    v->velocity_mode = PATCH_VELOCITY_MODE_LOWER_BOUND;

  /* velocity scaling */
  if ((p->velocity_scaling >= PATCH_VELOCITY_SCALING_LOWER_BOUND) && 
      (p->velocity_scaling <= PATCH_VELOCITY_SCALING_UPPER_BOUND))
  {
    v->velocity_scaling_amount = 
      S_voice_velocity_scaling_table[p->velocity_scaling - PATCH_VELOCITY_SCALING_LOWER_BOUND];
  }
  else
  {
    v->velocity_scaling_amount = 
      S_voice_velocity_scaling_table[PATCH_VELOCITY_SCALING_DEFAULT - PATCH_VELOCITY_SCALING_LOWER_BOUND];
  }

  /* noise mode */
  if ((p->noise_mode >= PATCH_NOISE_MODE_LOWER_BOUND) && 
      (p->noise_mode <= PATCH_NOISE_MODE_UPPER_BOUND))
  {
    v->noise_mode = p->noise_mode;
  }
  else
    v->noise_mode = PATCH_NOISE_MODE_LOWER_BOUND;

  /* noise frequency */
  if ((p->noise_frequency >= PATCH_NOISE_FREQUENCY_LOWER_BOUND) && 
      (p->noise_frequency <= PATCH_NOISE_FREQUENCY_UPPER_BOUND))
  {
    v->noise_increment = S_voice_noise_phase_increment_table[p->noise_frequency - PATCH_NOISE_FREQUENCY_LOWER_BOUND];
  }
  else
    v->noise_increment = S_voice_noise_phase_increment_table[0];

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

  /* set note velocity adjustment */
  if ((v->note_velocity >=  MIDI_CONT_NOTE_VELOCITY_LOWER_BOUND) && 
      (v->note_velocity <=  MIDI_CONT_NOTE_VELOCITY_UPPER_BOUND))
  {
    v->velocity_adjustment = 
      (v->velocity_scaling_amount * (MIDI_CONT_NOTE_VELOCITY_BREAK_POINT - v->note_velocity)) / MIDI_CONT_NOTE_VELOCITY_DIVISOR;
  }
  else
    v->velocity_adjustment = 0;

  /* determine notes & pitch indices */
  for (m = 0; m < BANK_OSCS_AND_ENVS_PER_VOICE; m++)
  {
    if (v->osc_freq_mode[m] == PATCH_OSC_FREQ_MODE_RATIO)
      v->osc_note[m] = v->base_note + v->osc_offset[m];
    else if (v->osc_freq_mode[m] == PATCH_OSC_FREQ_MODE_FIXED)
      v->osc_note[m] = v->osc_offset[m];
    else
      v->osc_note[m] = v->base_note;

    v->osc_pitch_index[m] = 
      (v->osc_note[m] * TUNING_NUM_SEMITONE_STEPS) + v->osc_detune[m];

    if (v->osc_pitch_index[m] < 0)
      v->osc_pitch_index[m] = 0;
    else if (v->osc_pitch_index[m] >= TUNING_NUM_INDICES)
      v->osc_pitch_index[m] = TUNING_NUM_INDICES - 1;
  }

  return 0;
}

/*******************************************************************************
** voice_sync_phases()
*******************************************************************************/
short int voice_sync_phases(int voice_index)
{
  int m;

  voice* v;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain voice pointer */
  v = &G_voice_bank[voice_index];

  /* sync (reset phases) */
  if (v->sync == PATCH_SYNC_ON)
  {
    /* oscillators */
    for (m = 0; m < BANK_OSCS_AND_ENVS_PER_VOICE; m++)
    {
      if ((v->osc_phi[m] >= PATCH_OSC_PHI_LOWER_BOUND) && 
          (v->osc_phi[m] <= PATCH_OSC_PHI_UPPER_BOUND))
      {
        v->osc_phase[m] = S_voice_phase_shift_table[v->osc_phi[m] - PATCH_OSC_PHI_LOWER_BOUND];
      }
      else
        v->osc_phase[m] = S_voice_phase_shift_table[0];
    }

    /* noise */
    v->noise_lfsr = 0x0001;
    v->noise_phase = 0;
  }

  return 0;
}

/*******************************************************************************
** voice_update_all()
*******************************************************************************/
short int voice_update_all()
{
  short int osc_env_index[BANK_OSCS_AND_ENVS_PER_VOICE];

  int osc_fb_mod[BANK_OSCS_AND_ENVS_PER_VOICE];
  int osc_phase_mod[BANK_OSCS_AND_ENVS_PER_VOICE];

  int osc_level[BANK_OSCS_AND_ENVS_PER_VOICE];

  int pitch_adjustment;
  int amplitude_adjustment_carriers;
  int amplitude_adjustment_modulators;

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

    /* update adjustments */
    pitch_adjustment = 0;
    amplitude_adjustment_carriers = 0;
    amplitude_adjustment_modulators = 0;

    pitch_adjustment += v->lfo_input_vibrato;
    pitch_adjustment += v->sweep_input;
    pitch_adjustment += v->bender_input;

    if (v->tremolo_mode == PATCH_TREMOLO_MODE_CARRIERS)
      amplitude_adjustment_carriers += v->lfo_input_tremolo;
    else if (v->tremolo_mode == PATCH_TREMOLO_MODE_MODULATORS)
      amplitude_adjustment_modulators += v->lfo_input_tremolo;

    if (v->boost_mode == PATCH_BOOST_MODE_CARRIERS)
      amplitude_adjustment_carriers -= v->boost_input;
    else if (v->boost_mode == PATCH_BOOST_MODE_MODULATORS)
      amplitude_adjustment_modulators -= v->boost_input;

    if (v->velocity_mode == PATCH_VELOCITY_MODE_CARRIERS)
      amplitude_adjustment_carriers += v->velocity_adjustment;
    else if (v->velocity_mode == PATCH_VELOCITY_MODE_MODULATORS)
      amplitude_adjustment_modulators += v->velocity_adjustment;

    /* update envelopes */
    for (m = 0; m < BANK_OSCS_AND_ENVS_PER_VOICE; m++)
    {
      osc_env_index[m] = v->env_input[m];
    }

    /* apply tremolo, boost, and note velocity */
    if (v->algorithm == PATCH_ALGORITHM_1C_CHAIN)
    {
      osc_env_index[2] += amplitude_adjustment_modulators;
      osc_env_index[3] += amplitude_adjustment_carriers;
    }
    else if (v->algorithm == PATCH_ALGORITHM_1C_THE_Y)
    {
      osc_env_index[2] += amplitude_adjustment_modulators;
      osc_env_index[3] += amplitude_adjustment_carriers;
    }
    else if (v->algorithm == PATCH_ALGORITHM_1C_CRAB)
    {
      osc_env_index[1] += amplitude_adjustment_modulators;
      osc_env_index[2] += amplitude_adjustment_modulators;
      osc_env_index[3] += amplitude_adjustment_carriers;
    }
    else if (v->algorithm == PATCH_ALGORITHM_2C_TWIN)
    {
      osc_env_index[0] += amplitude_adjustment_modulators;
      osc_env_index[1] += amplitude_adjustment_modulators;
      osc_env_index[2] += amplitude_adjustment_carriers;
      osc_env_index[3] += amplitude_adjustment_carriers;
    }
    else if (v->algorithm == PATCH_ALGORITHM_2C_STACKED)
    {
      osc_env_index[1] += amplitude_adjustment_modulators;
      osc_env_index[2] += amplitude_adjustment_carriers;
      osc_env_index[3] += amplitude_adjustment_carriers;
    }
    else if (v->algorithm == PATCH_ALGORITHM_3C_1_TO_3)
    {
      osc_env_index[0] += amplitude_adjustment_modulators;
      osc_env_index[1] += amplitude_adjustment_carriers;
      osc_env_index[2] += amplitude_adjustment_carriers;
      osc_env_index[3] += amplitude_adjustment_carriers;
    }
    else if (v->algorithm == PATCH_ALGORITHM_3C_1_TO_1)
    {
      osc_env_index[0] += amplitude_adjustment_modulators;
      osc_env_index[1] += amplitude_adjustment_carriers;
      osc_env_index[2] += amplitude_adjustment_carriers;
      osc_env_index[3] += amplitude_adjustment_carriers;
    }
    else if (v->algorithm == PATCH_ALGORITHM_4C_PIPES)
    {
      osc_env_index[0] += amplitude_adjustment_carriers;
      osc_env_index[1] += amplitude_adjustment_carriers;
      osc_env_index[2] += amplitude_adjustment_carriers;
      osc_env_index[3] += amplitude_adjustment_carriers;
    }

    /* bound envelopes */
    for (m = 0; m < BANK_OSCS_AND_ENVS_PER_VOICE; m++)
    {
      if (osc_env_index[m] < 0)
        osc_env_index[m] = 0;
      else if (osc_env_index[m] > 1023)
        osc_env_index[m] = 1023;
    }

    /* update pitches & phases */
    for (m = 0; m < BANK_OSCS_AND_ENVS_PER_VOICE; m++)
    {
      adjusted_pitch_index = v->osc_pitch_index[m] + pitch_adjustment;

      if (adjusted_pitch_index < 0)
        adjusted_pitch_index = 0;
      else if (adjusted_pitch_index >= TUNING_NUM_INDICES)
        adjusted_pitch_index = TUNING_NUM_INDICES - 1;

      octave = adjusted_pitch_index / TUNING_TABLE_SIZE;
      increment_index = adjusted_pitch_index % TUNING_TABLE_SIZE;

      if (octave < TUNING_HIGHEST_OCTAVE)
        v->osc_phase[m] += G_tuning_phase_increment_table[increment_index] >> (TUNING_HIGHEST_OCTAVE - octave);
      else
        v->osc_phase[m] += G_tuning_phase_increment_table[increment_index];

      /* wraparound phase register (28 bits) */
      if (v->osc_phase[m] > 0xFFFFFFF)
        v->osc_phase[m] &= 0xFFFFFFF;
    }

    /* update oscillator feedback */
    VOICE_UPDATE_OSCILLATOR_FEEDBACK(0)
    VOICE_UPDATE_OSCILLATOR_FEEDBACK(1)
    VOICE_UPDATE_OSCILLATOR_FEEDBACK(2)
    VOICE_UPDATE_OSCILLATOR_FEEDBACK(3)

    /* update noise */
    if ((v->noise_mode == PATCH_NOISE_MODE_SQUARE) || 
        (v->noise_mode == PATCH_NOISE_MODE_SAW))
    {
      v->noise_phase += v->noise_increment;

      /* wraparound noise phase register (28 bits) */
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
    }

    /* update oscillator levels based on current algorithm */

    /* 1 carrier - chain  */
    /*   1 -> 2 -> 3 -> 4 */
    if (v->algorithm == PATCH_ALGORITHM_1C_CHAIN)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(1, 0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(2, 1)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(3, 2)

      level = osc_level[3];
    }
    /* 1 carrier - Y        */
    /*   (1 + 2) -> 3 -> 4  */
    else if (v->algorithm == PATCH_ALGORITHM_1C_THE_Y)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(1)
      VOICE_UPDATE_OSCILLATOR_MOD_X2(2, 0, 1)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(3, 2)

      level = osc_level[3];
    }
    /* 1 carrier - crab claw  */
    /*   1 -> 2, (2 + 3) -> 4 */
    else if (v->algorithm == PATCH_ALGORITHM_1C_CRAB)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(1, 0)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(2)
      VOICE_UPDATE_OSCILLATOR_MOD_X2(3, 1, 2)

      level = osc_level[3];
    }
    /* 2 carriers - twin                  */
    /*   1 -> 3, 2 -> 4, output is 3 + 4  */
    else if (v->algorithm == PATCH_ALGORITHM_2C_TWIN)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(1)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(2, 0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(3, 1)

      level = osc_level[2] + osc_level[3];
    }
    /* 2 carriers - stacked           */
    /*   1 -> 2 -> 3, output is 3 + 4 */
    else if (v->algorithm == PATCH_ALGORITHM_2C_STACKED)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(1, 0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(2, 1)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(3)

      level = osc_level[2] + osc_level[3];
    }
    /* 3 carriers - one to three                      */
    /*   1 -> 2, 1 -> 3, 1 -> 4, output is 2 + 3 + 4  */
    else if (v->algorithm == PATCH_ALGORITHM_3C_1_TO_3)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(1, 0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(2, 0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(3, 0)

      level = osc_level[1] + osc_level[2] + osc_level[3];
    }
    /* 3 carriers - one to one        */
    /*   1 -> 2, output is 2 + 3 + 4  */
    else if (v->algorithm == PATCH_ALGORITHM_3C_1_TO_1)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_MOD_X1(1, 0)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(2)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(3)

      level = osc_level[1] + osc_level[2] + osc_level[3];
    }
    /* 4 carriers - pipes         */
    /*   output is 1 + 2 + 3 + 4  */
    else if (v->algorithm == PATCH_ALGORITHM_4C_PIPES)
    {
      VOICE_UPDATE_OSCILLATOR_NO_MOD(0)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(1)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(2)
      VOICE_UPDATE_OSCILLATOR_NO_MOD(3)

      level = osc_level[0] + osc_level[1] + osc_level[2] + osc_level[3];
    }
    else
      level = 0;

    /* swap in noise on oscillator 4 if necessary */
    if ((v->noise_mode == PATCH_NOISE_MODE_SQUARE) || 
        (v->noise_mode == PATCH_NOISE_MODE_SAW))
    {
      level -= osc_level[3];

      masked_phase = (v->noise_lfsr & 0x3FF);

      if (v->noise_mode == PATCH_NOISE_MODE_SQUARE)
        final_index = 0;
      else if (v->noise_mode == PATCH_NOISE_MODE_SAW)
      {
        if (masked_phase < 512)
          final_index = ((511 - masked_phase) * 4095) / 512;
        else
          final_index = ((masked_phase - 512) * 4095) / 512;
      }
      else
        final_index = 4095;

      final_index += (osc_env_index[3] << 2);

      if (final_index < 0)
        final_index = 0;
      else if (final_index > 4095)
        final_index = 4095;

      if (masked_phase < 512)
        level += S_voice_db_to_linear_table[final_index];
      else
        level -= S_voice_db_to_linear_table[final_index];
    }

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
  int     m;
  double  val;

  /* ym2612 - 10 bit envelope (shifted to 12 bit), 12 bit sine, 13 bit sum    */
  /* 10 bit db: 24, 12, 6, 3, 1.5, 0.75, 0.375, 0.1875, 0.09375, 0.046875     */
  /* 12 bit db: adds on 0.0234375, 0.01171875 in back                         */
  /* 13 bit db: adds on 48 in front                                           */

  /* db to linear scale conversion */
  S_voice_db_to_linear_table[0] = 32767;

  for (m = 1; m < 4095; m++)
  {
    S_voice_db_to_linear_table[m] = 
      (short int) ((32767.0f * exp(-log(10) * (DB_STEP_12_BIT / 10) * m)) + 0.5f);
  }

  S_voice_db_to_linear_table[4095] = 0;

  /* the voice wavetables have 1024 entries per period */

  /* wavetable (sine) */
  S_voice_wavetable_sine[0] = 4095;
  S_voice_wavetable_sine[256] = 0;

  for (m = 1; m < 256; m++)
  {
    val = sin(TWO_PI * (m / 1024.0f));
    S_voice_wavetable_sine[m] = (short int) ((10 * (log(1 / val) / log(10)) / DB_STEP_12_BIT) + 0.5f);
    S_voice_wavetable_sine[512 - m] = S_voice_wavetable_sine[m];
  }

  /* detune table */
  for (m = 0; m < PATCH_OSC_DETUNE_NUM_VALUES; m++)
  {
    S_voice_detune_table[m] = 
      ((m - (PATCH_OSC_DETUNE_UPPER_BOUND / 2)) * TUNING_NUM_SEMITONE_STEPS) / PATCH_OSC_DETUNE_UPPER_BOUND;
  }

  /* noise phase increment table */
  for (m = 0; m < PATCH_NOISE_FREQUENCY_NUM_VALUES; m++)
  {
    S_voice_noise_phase_increment_table[m] = 
      (int) ((S_voice_noise_frequency_table[m] * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
  }

#if 0
  /* print out db to linear table values */
  for (m = 0; m < 4096; m += 4)
  {
    printf("DB to Linear Table Index %d: %d\n", m, S_voice_db_to_linear_table[m]);
  }
#endif

#if 0
  /* print out sine wavetable values */
  for (m = 0; m < 256; m++)
  {
    val = sin(TWO_PI * (m / 1024.0f));
    printf("Sine Wavetable Index %d: %f, %d (DB: %d)\n", 
            m, val, S_voice_db_to_linear_table[S_voice_wavetable_sine[m]], 
                    S_voice_wavetable_sine[m]);
  }
#endif

#if 0
  /* print out noise phase increments */
  for (m = 0; m < PATCH_NOISE_FREQUENCY_NUM_VALUES; m++)
  {
    printf( "Noise Freq %d (%f hz): %d \n", 
            m, S_voice_noise_frequency_table[m], S_voice_noise_phase_increment_table[m]);
  }
#endif

  return 0;
}

