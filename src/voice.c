/*******************************************************************************
** voice.c (synth voice)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "clock.h"
#include "lfo.h"
#include "midicont.h"
#include "patch.h"
#include "sweep.h"
#include "tuning.h"
#include "voice.h"

#define PI      3.14159265358979323846f
#define TWO_PI  6.28318530717958647693f

#define VOICE_DB_STEP_12_BIT 0.01171875f

#define VOICE_DB_TO_LINEAR_TABLE_SIZE 4096

#define VOICE_MAX_VOLUME_DB       0
#define VOICE_MAX_ATTENUATION_DB  (VOICE_DB_TO_LINEAR_TABLE_SIZE - 1)

#define VOICE_MAX_VOLUME_LINEAR       32767
#define VOICE_MAX_ATTENUATION_LINEAR  0

#define VOICE_WAVETABLE_SIZE_FULL     1024
#define VOICE_WAVETABLE_SIZE_HALF     (VOICE_WAVETABLE_SIZE_FULL / 2)
#define VOICE_WAVETABLE_SIZE_QUARTER  (VOICE_WAVETABLE_SIZE_FULL / 4)

#define VOICE_WAVETABLE_LOOKUP(set, num, phase_mod)                                       \
  if ((v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_NOISE_SQUARE) ||                     \
      (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_NOISE_SAW))                          \
  {                                                                                       \
    masked_phase = v->osc##_##num##_lfsr & 0x3FF;                                         \
  }                                                                                       \
  else                                                                                    \
    masked_phase = (v->set##_##num##_phase >> 18) & 0x3FF;                                \
                                                                                          \
  masked_phase = (masked_phase + phase_mod) & 0x3FF;                                      \
                                                                                          \
  /* determine waveform level (db) */                                                     \
  if ((v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_SINE) ||                             \
      (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_FULL_RECT))                          \
  {                                                                                       \
    if (masked_phase < VOICE_WAVETABLE_SIZE_HALF)                                         \
      final_index = S_voice_wavetable_sine[masked_phase];                                 \
    else                                                                                  \
      final_index = S_voice_wavetable_sine[masked_phase - VOICE_WAVETABLE_SIZE_HALF];     \
  }                                                                                       \
  else if (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_HALF_RECT)                       \
  {                                                                                       \
    if (masked_phase < VOICE_WAVETABLE_SIZE_HALF)                                         \
      final_index = S_voice_wavetable_sine[masked_phase];                                 \
    else                                                                                  \
      final_index = VOICE_MAX_ATTENUATION_DB;                                             \
  }                                                                                       \
  else if (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_QUARTER_RECT)                    \
  {                                                                                       \
    if (masked_phase < VOICE_WAVETABLE_SIZE_QUARTER)                                      \
      final_index = S_voice_wavetable_sine[masked_phase];                                 \
    else if (masked_phase < VOICE_WAVETABLE_SIZE_HALF)                                    \
      final_index = VOICE_MAX_ATTENUATION_DB;                                             \
    else if (masked_phase < 3 * VOICE_WAVETABLE_SIZE_QUARTER)                             \
      final_index = S_voice_wavetable_sine[masked_phase - VOICE_WAVETABLE_SIZE_HALF];     \
    else                                                                                  \
      final_index = VOICE_MAX_ATTENUATION_DB;                                             \
  }                                                                                       \
  else if ( (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_EPO_SINE) ||                   \
            (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_EPO_FULL_RECT))                \
  {                                                                                       \
    if (masked_phase < VOICE_WAVETABLE_SIZE_QUARTER)                                      \
      final_index = S_voice_wavetable_sine[2 * masked_phase];                             \
    else if (masked_phase < VOICE_WAVETABLE_SIZE_HALF)                                    \
      final_index = S_voice_wavetable_sine[2 * masked_phase - VOICE_WAVETABLE_SIZE_HALF]; \
    else                                                                                  \
      final_index = VOICE_MAX_ATTENUATION_DB;                                             \
  }                                                                                       \
  else if ( (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_SQUARE)  ||                    \
            (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_NOISE_SQUARE))                 \
  {                                                                                       \
    final_index = VOICE_MAX_VOLUME_DB;                                                    \
  }                                                                                       \
  else if ( (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_SAW_DOWN)  ||                  \
            (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_SAW_UP)    ||                  \
            (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_NOISE_SAW))                    \
  {                                                                                       \
    final_index = S_voice_wavetable_saw[masked_phase];                                    \
  }                                                                                       \
  else if (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_TRIANGLE)                        \
  {                                                                                       \
    if (masked_phase < VOICE_WAVETABLE_SIZE_HALF)                                         \
      final_index = S_voice_wavetable_tri[masked_phase];                                  \
    else                                                                                  \
      final_index = S_voice_wavetable_tri[masked_phase - VOICE_WAVETABLE_SIZE_HALF];      \
  }                                                                                       \
  else if (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_EPO_SQUARE)                      \
  {                                                                                       \
    if (masked_phase < VOICE_WAVETABLE_SIZE_HALF)                                         \
      final_index = VOICE_MAX_VOLUME_DB;                                                  \
    else                                                                                  \
      final_index = VOICE_MAX_ATTENUATION_DB;                                             \
  }                                                                                       \
  else if ( (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_EPO_SAW_DOWN) ||               \
            (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_EPO_SAW_UP))                   \
  {                                                                                       \
    if (masked_phase < VOICE_WAVETABLE_SIZE_HALF)                                         \
      final_index = S_voice_wavetable_saw[2 * masked_phase];                              \
    else                                                                                  \
      final_index = VOICE_MAX_ATTENUATION_DB;                                             \
  }                                                                                       \
  else if (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_EPO_TRIANGLE)                    \
  {                                                                                       \
    if (masked_phase < VOICE_WAVETABLE_SIZE_QUARTER)                                      \
      final_index = S_voice_wavetable_tri[2 * masked_phase];                              \
    else if (masked_phase < VOICE_WAVETABLE_SIZE_HALF)                                    \
      final_index = S_voice_wavetable_tri[2 * masked_phase - VOICE_WAVETABLE_SIZE_HALF];  \
    else                                                                                  \
      final_index = VOICE_MAX_ATTENUATION_DB;                                             \
  }                                                                                       \
  else                                                                                    \
    final_index = VOICE_MAX_ATTENUATION_DB;                                               \
                                                                                          \
  /* apply envelope */                                                                    \
  final_index += set##_##num##_env_index;                                                 \
                                                                                          \
  if (final_index < 0)                                                                    \
    final_index = 0;                                                                      \
  else if (final_index > VOICE_MAX_ATTENUATION_DB)                                        \
    final_index = VOICE_MAX_ATTENUATION_DB;                                               \
                                                                                          \
  /* determine waveform level (linear) */                                                 \
  if ((v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_SINE)          ||                    \
      (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_HALF_RECT)     ||                    \
      (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_EPO_SINE)      ||                    \
      (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_SQUARE)        ||                    \
      (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_SAW_DOWN)      ||                    \
      (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_TRIANGLE)      ||                    \
      (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_NOISE_SQUARE)  ||                    \
      (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_NOISE_SAW))                          \
  {                                                                                       \
    if (masked_phase < VOICE_WAVETABLE_SIZE_HALF)                                         \
      set##_##num##_level = S_voice_db_to_linear_table[final_index];                      \
    else                                                                                  \
      set##_##num##_level = -S_voice_db_to_linear_table[final_index];                     \
  }                                                                                       \
  else if ( (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_FULL_RECT)     ||              \
            (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_QUARTER_RECT)  ||              \
            (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_EPO_FULL_RECT))                \
  {                                                                                       \
    set##_##num##_level = S_voice_db_to_linear_table[final_index];                        \
  }                                                                                       \
  else if (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_SAW_UP)                          \
  {                                                                                       \
    if (masked_phase < VOICE_WAVETABLE_SIZE_HALF)                                         \
      set##_##num##_level = -S_voice_db_to_linear_table[final_index];                     \
    else                                                                                  \
      set##_##num##_level = S_voice_db_to_linear_table[final_index];                      \
  }                                                                                       \
  else if ( (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_EPO_SQUARE)    ||              \
            (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_EPO_SAW_DOWN)  ||              \
            (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_EPO_TRIANGLE))                 \
  {                                                                                       \
    if (masked_phase < VOICE_WAVETABLE_SIZE_QUARTER)                                      \
      set##_##num##_level = S_voice_db_to_linear_table[final_index];                      \
    else                                                                                  \
      set##_##num##_level = -S_voice_db_to_linear_table[final_index];                     \
  }                                                                                       \
  else if (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_EPO_SAW_UP)                      \
  {                                                                                       \
    if (masked_phase < VOICE_WAVETABLE_SIZE_QUARTER)                                      \
      set##_##num##_level = -S_voice_db_to_linear_table[final_index];                     \
    else                                                                                  \
      set##_##num##_level = S_voice_db_to_linear_table[final_index];                      \
  }                                                                                       \
  else                                                                                    \
    set##_##num##_level = S_voice_db_to_linear_table[VOICE_MAX_ATTENUATION_DB];

#define VOICE_COMPUTE_PHASE_MOD(set, num)                                      \
  osc_##num##_phase_mod = (set##_##num##_level >> 3) & 0x3FF;

#define VOICE_BOUND_ENVELOPE(num)                                              \
  if (osc_##num##_env_index < 0)                                               \
    osc_##num##_env_index = 0;                                                 \
  else if (osc_##num##_env_index > VOICE_MAX_ATTENUATION_DB)                   \
    osc_##num##_env_index = VOICE_MAX_ATTENUATION_DB;

#define VOICE_BOUND_PITCH_INDEX(index)                                         \
  if (index < 0)                                                               \
    index = 0;                                                                 \
  else if (index >= TUNING_NUM_INDICES)                                        \
    index = TUNING_NUM_INDICES - 1;

#define VOICE_COMPUTE_PITCH_INDEX(num)                                         \
  v->osc_##num##_pitch_index = v->base_note * TUNING_NUM_SEMITONE_STEPS;       \
                                                                               \
  v->osc_##num##_pitch_index += v->osc_##num##_pitch_offset;                   \
  v->osc_##num##_pitch_index +=                                                \
    G_tuning_offset_table[(v->base_note - TUNING_NOTE_C0) % 12];               \
                                                                               \
  VOICE_BOUND_PITCH_INDEX(v->osc_##num##_pitch_index)

#define VOICE_UPDATE_PHASE(num)                                                   \
  adjusted_pitch_index = v->osc_##num##_pitch_index + pitch_adjustment;           \
                                                                                  \
  VOICE_BOUND_PITCH_INDEX(adjusted_pitch_index)                                   \
                                                                                  \
  v->osc_##num##_phase +=                                                         \
    S_voice_wave_phase_increment_table[adjusted_pitch_index];                     \
                                                                                  \
  if (v->osc_##num##_phase > 0xFFFFFFF)                                           \
  {                                                                               \
    v->osc_##num##_phase &= 0xFFFFFFF;                                            \
                                                                                  \
    if ((v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_NOISE_SQUARE) ||           \
        (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_NOISE_SAW))                \
    {                                                                             \
      /* update noise generator (nes) */                                          \
      /* 15-bit lfsr, taps on 1 and 2 */                                          \
      if ((v->osc_##num##_lfsr & 0x0001) ^ ((v->osc_##num##_lfsr & 0x0002) >> 1)) \
        v->osc_##num##_lfsr = ((v->osc_##num##_lfsr >> 1) & 0x3FFF) | 0x4000;     \
      else                                                                        \
        v->osc_##num##_lfsr = (v->osc_##num##_lfsr >> 1) & 0x3FFF;                \
    }                                                                             \
  }

/* multiple table */
/* the values form the harmonic series! */
static int  S_voice_multiple_table[PATCH_OSC_MULTIPLE_NUM_VALUES] = 
            { (0 * 12 + 0)  * TUNING_NUM_SEMITONE_STEPS,  /*  1x  */
              (1 * 12 + 0)  * TUNING_NUM_SEMITONE_STEPS,  /*  2x  */
              (1 * 12 + 7)  * TUNING_NUM_SEMITONE_STEPS,  /*  3x  */
              (2 * 12 + 0)  * TUNING_NUM_SEMITONE_STEPS,  /*  4x  */
              (2 * 12 + 4)  * TUNING_NUM_SEMITONE_STEPS,  /*  5x  */
              (2 * 12 + 7)  * TUNING_NUM_SEMITONE_STEPS,  /*  6x  */
              (2 * 12 + 10) * TUNING_NUM_SEMITONE_STEPS,  /*  7x  */
              (3 * 12 + 0)  * TUNING_NUM_SEMITONE_STEPS,  /*  8x  */
              (3 * 12 + 2)  * TUNING_NUM_SEMITONE_STEPS,  /*  9x  */
              (3 * 12 + 4)  * TUNING_NUM_SEMITONE_STEPS,  /* 10x  */
              (3 * 12 + 6)  * TUNING_NUM_SEMITONE_STEPS,  /* 11x  */
              (3 * 12 + 7)  * TUNING_NUM_SEMITONE_STEPS,  /* 12x  */
              (3 * 12 + 8)  * TUNING_NUM_SEMITONE_STEPS,  /* 13x  */
              (3 * 12 + 10) * TUNING_NUM_SEMITONE_STEPS,  /* 14x  */
              (3 * 12 + 11) * TUNING_NUM_SEMITONE_STEPS,  /* 15x  */
              (4 * 12 + 0)  * TUNING_NUM_SEMITONE_STEPS   /* 16x  */
            };

/* detune table */
static int  S_voice_detune_table[PATCH_OSC_DETUNE_NUM_VALUES] = 
            {-(9  * TUNING_NUM_SEMITONE_STEPS) / 100, 
             -(6  * TUNING_NUM_SEMITONE_STEPS) / 100, 
             -(3  * TUNING_NUM_SEMITONE_STEPS) / 100, 
              (0  * TUNING_NUM_SEMITONE_STEPS) / 100, 
              (3  * TUNING_NUM_SEMITONE_STEPS) / 100, 
              (6  * TUNING_NUM_SEMITONE_STEPS) / 100, 
              (9  * TUNING_NUM_SEMITONE_STEPS) / 100 
            };

/* phase increment table */
static unsigned int S_voice_wave_phase_increment_table[TUNING_NUM_INDICES];

/* db to linear table */
static short int  S_voice_db_to_linear_table[VOICE_DB_TO_LINEAR_TABLE_SIZE];

/* wavetables */
static short int  S_voice_wavetable_sine[VOICE_WAVETABLE_SIZE_HALF];
static short int  S_voice_wavetable_tri[VOICE_WAVETABLE_SIZE_HALF];
static short int  S_voice_wavetable_saw[VOICE_WAVETABLE_SIZE_FULL];

/* voice bank */
voice G_voice_bank[BANK_NUM_VOICES];

/*******************************************************************************
** voice_reset_all()
*******************************************************************************/
short int voice_reset_all()
{
  int k;

  voice* v;

  /* reset all voices */
  for (k = 0; k < BANK_NUM_VOICES; k++)
  {
    /* obtain voice pointer */
    v = &G_voice_bank[k];

    /* algorithm, sync */
    v->algorithm = PATCH_ALGORITHM_DEFAULT;
    v->sync = PATCH_SYNC_DEFAULT;

    /* oscillator 1 */
    v->osc_1_waveform = PATCH_OSC_WAVEFORM_DEFAULT;
    v->osc_1_phi = PATCH_OSC_PHI_DEFAULT;

    v->osc_1_pitch_offset = 0;

    v->osc_1_pitch_offset += S_voice_multiple_table[PATCH_OSC_MULTIPLE_DEFAULT - PATCH_OSC_MULTIPLE_LOWER_BOUND];
    v->osc_1_pitch_offset -= S_voice_multiple_table[PATCH_OSC_DIVISOR_DEFAULT - PATCH_OSC_DIVISOR_LOWER_BOUND];
    v->osc_1_pitch_offset += S_voice_detune_table[PATCH_OSC_DETUNE_DEFAULT - PATCH_OSC_DETUNE_LOWER_BOUND];

    /* oscillator 2 */
    v->osc_2_waveform = PATCH_OSC_WAVEFORM_DEFAULT;
    v->osc_2_phi = PATCH_OSC_PHI_DEFAULT;

    v->osc_2_pitch_offset = 0;

    v->osc_2_pitch_offset += S_voice_multiple_table[PATCH_OSC_MULTIPLE_DEFAULT - PATCH_OSC_MULTIPLE_LOWER_BOUND];
    v->osc_2_pitch_offset -= S_voice_multiple_table[PATCH_OSC_DIVISOR_DEFAULT - PATCH_OSC_DIVISOR_LOWER_BOUND];
    v->osc_2_pitch_offset += S_voice_detune_table[PATCH_OSC_DETUNE_DEFAULT - PATCH_OSC_DETUNE_LOWER_BOUND];

    /* oscillator 3 */
    v->osc_3_waveform = PATCH_OSC_WAVEFORM_DEFAULT;
    v->osc_3_phi = PATCH_OSC_PHI_DEFAULT;

    v->osc_3_pitch_offset = 0;

    v->osc_3_pitch_offset += S_voice_detune_table[PATCH_OSC_DETUNE_DEFAULT - PATCH_OSC_DETUNE_LOWER_BOUND];

    /* currently playing note, pitch indices */
    v->base_note = TUNING_NOTE_BLANK;

    v->osc_1_pitch_index = 0;
    v->osc_2_pitch_index = 0;
    v->osc_3_pitch_index = 0;

    /* phases */
    v->osc_1_phase = 0;
    v->osc_2_phase = 0;
    v->osc_3_phase = 0;

    v->chorus_1_phase = 0;
    v->chorus_2_phase = 0;
    v->chorus_3_phase = 0;

    /* noise lfsrs */
    v->osc_1_lfsr = 0x0001;
    v->osc_2_lfsr = 0x0001;
    v->osc_3_lfsr = 0x0001;

    /* envelope input levels */
    v->env_1_input = VOICE_MAX_ATTENUATION_DB;
    v->env_2_input = VOICE_MAX_ATTENUATION_DB;
    v->env_2_input = VOICE_MAX_ATTENUATION_DB;

    /* vibrato, sweep */
    v->vibrato_input = 0;
    v->chorus_input = 0;
    v->sweep_input = 0;

    v->vibrato_adjustment = 0;

    /* midi controller positions */
    v->mod_wheel_pos = 0;
    v->aftertouch_pos = 0;
    v->exp_pedal_pos = 0;
    v->pitch_wheel_pos = 0;

    /* output level */
    v->level = 0;
  }

  return 0;
}

/*******************************************************************************
** voice_load_patch()
*******************************************************************************/
short int voice_load_patch(int voice_index, int patch_index)
{
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
    v->algorithm = PATCH_ALGORITHM_DEFAULT;

  /* sync */
  if ((p->sync_osc >= PATCH_SYNC_LOWER_BOUND) && 
      (p->sync_osc <= PATCH_SYNC_UPPER_BOUND))
  {
    v->sync = p->sync_osc;
  }
  else
    v->sync = PATCH_SYNC_DEFAULT;

  /* oscillator 1 waveform */
  if ((p->osc_1_waveform >= PATCH_OSC_WAVEFORM_LOWER_BOUND) && 
      (p->osc_1_waveform <= PATCH_OSC_WAVEFORM_UPPER_BOUND))
  {
    v->osc_1_waveform = p->osc_1_waveform;
  }
  else
    v->osc_1_waveform = PATCH_OSC_WAVEFORM_DEFAULT;

  /* oscillator 1 phi */
  if ((p->osc_1_phi >= PATCH_OSC_PHI_LOWER_BOUND) && 
      (p->osc_1_phi <= PATCH_OSC_PHI_UPPER_BOUND))
  {
    v->osc_1_phi = p->osc_1_phi;
  }
  else
    v->osc_1_phi = PATCH_OSC_PHI_DEFAULT;

  /* oscillator 1 pitch index */
  v->osc_1_pitch_offset = 0;

  if ((p->osc_1_multiple >= PATCH_OSC_MULTIPLE_LOWER_BOUND) && 
      (p->osc_1_multiple <= PATCH_OSC_MULTIPLE_UPPER_BOUND))
  {
    v->osc_1_pitch_offset += S_voice_multiple_table[p->osc_1_multiple - PATCH_OSC_MULTIPLE_LOWER_BOUND];
  }
  else
    v->osc_1_pitch_offset += S_voice_multiple_table[PATCH_OSC_MULTIPLE_DEFAULT - PATCH_OSC_MULTIPLE_LOWER_BOUND];

  if ((p->osc_1_divisor >= PATCH_OSC_DIVISOR_LOWER_BOUND) && 
      (p->osc_1_divisor <= PATCH_OSC_DIVISOR_UPPER_BOUND))
  {
    v->osc_1_pitch_offset -= S_voice_multiple_table[p->osc_1_divisor - PATCH_OSC_DIVISOR_LOWER_BOUND];
  }
  else
    v->osc_1_pitch_offset -= S_voice_multiple_table[PATCH_OSC_DIVISOR_DEFAULT - PATCH_OSC_DIVISOR_LOWER_BOUND];

  if ((p->osc_1_detune >= PATCH_OSC_DETUNE_LOWER_BOUND) && 
      (p->osc_1_detune <= PATCH_OSC_DETUNE_UPPER_BOUND))
  {
    v->osc_1_pitch_offset += S_voice_detune_table[p->osc_1_detune - PATCH_OSC_DETUNE_LOWER_BOUND];
  }
  else
    v->osc_1_pitch_offset += S_voice_detune_table[PATCH_OSC_DETUNE_DEFAULT - PATCH_OSC_DETUNE_LOWER_BOUND];

  /* oscillator 2 waveform */
  if ((p->osc_2_waveform >= PATCH_OSC_WAVEFORM_LOWER_BOUND) && 
      (p->osc_2_waveform <= PATCH_OSC_WAVEFORM_UPPER_BOUND))
  {
    v->osc_2_waveform = p->osc_2_waveform;
  }
  else
    v->osc_2_waveform = PATCH_OSC_WAVEFORM_DEFAULT;

  /* oscillator 2 phi */
  if ((p->osc_2_phi >= PATCH_OSC_PHI_LOWER_BOUND) && 
      (p->osc_2_phi <= PATCH_OSC_PHI_UPPER_BOUND))
  {
    v->osc_2_phi = p->osc_2_phi;
  }
  else
    v->osc_2_phi = PATCH_OSC_PHI_DEFAULT;

  /* oscillator 2 pitch index */
  v->osc_2_pitch_offset = 0;

  if ((p->osc_2_multiple >= PATCH_OSC_MULTIPLE_LOWER_BOUND) && 
      (p->osc_2_multiple <= PATCH_OSC_MULTIPLE_UPPER_BOUND))
  {
    v->osc_2_pitch_offset += S_voice_multiple_table[p->osc_2_multiple - PATCH_OSC_MULTIPLE_LOWER_BOUND];
  }
  else
    v->osc_2_pitch_offset += S_voice_multiple_table[PATCH_OSC_MULTIPLE_DEFAULT - PATCH_OSC_MULTIPLE_LOWER_BOUND];

  if ((p->osc_2_divisor >= PATCH_OSC_DIVISOR_LOWER_BOUND) && 
      (p->osc_2_divisor <= PATCH_OSC_DIVISOR_UPPER_BOUND))
  {
    v->osc_2_pitch_offset -= S_voice_multiple_table[p->osc_2_divisor - PATCH_OSC_DIVISOR_LOWER_BOUND];
  }
  else
    v->osc_2_pitch_offset -= S_voice_multiple_table[PATCH_OSC_DIVISOR_DEFAULT - PATCH_OSC_DIVISOR_LOWER_BOUND];

  if ((p->osc_2_detune >= PATCH_OSC_DETUNE_LOWER_BOUND) && 
      (p->osc_2_detune <= PATCH_OSC_DETUNE_UPPER_BOUND))
  {
    v->osc_2_pitch_offset += S_voice_detune_table[p->osc_2_detune - PATCH_OSC_DETUNE_LOWER_BOUND];
  }
  else
    v->osc_2_pitch_offset += S_voice_detune_table[PATCH_OSC_DETUNE_DEFAULT - PATCH_OSC_DETUNE_LOWER_BOUND];

  /* oscillator 3 waveform */
  if ((p->osc_3_waveform >= PATCH_OSC_WAVEFORM_LOWER_BOUND) && 
      (p->osc_3_waveform <= PATCH_OSC_WAVEFORM_UPPER_BOUND))
  {
    v->osc_3_waveform = p->osc_3_waveform;
  }
  else
    v->osc_3_waveform = PATCH_OSC_WAVEFORM_DEFAULT;

  /* oscillator 3 phi */
  if ((p->osc_3_phi >= PATCH_OSC_PHI_LOWER_BOUND) && 
      (p->osc_3_phi <= PATCH_OSC_PHI_UPPER_BOUND))
  {
    v->osc_3_phi = p->osc_3_phi;
  }
  else
    v->osc_3_phi = PATCH_OSC_PHI_DEFAULT;

  /* oscillator 3 pitch index */
  v->osc_3_pitch_offset = 0;

  if ((p->osc_3_detune >= PATCH_OSC_DETUNE_LOWER_BOUND) && 
      (p->osc_3_detune <= PATCH_OSC_DETUNE_UPPER_BOUND))
  {
    v->osc_3_pitch_offset += S_voice_detune_table[p->osc_3_detune - PATCH_OSC_DETUNE_LOWER_BOUND];
  }
  else
    v->osc_3_pitch_offset += S_voice_detune_table[PATCH_OSC_DETUNE_DEFAULT - PATCH_OSC_DETUNE_LOWER_BOUND];

  /* determine oscillator notes and pitch indices */
  VOICE_COMPUTE_PITCH_INDEX(1)
  VOICE_COMPUTE_PITCH_INDEX(2)
  VOICE_COMPUTE_PITCH_INDEX(3)

  return 0;
}

/*******************************************************************************
** voice_set_note()
*******************************************************************************/
short int voice_set_note(int voice_index, int note)
{
  voice* v;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain voice pointer */
  v = &G_voice_bank[voice_index];

  /* if note is out of range, ignore */
  if (TUNING_NOTE_IS_NOT_VALID(note))
    return 0;

  /* set base note */
  v->base_note = note;

  /* determine notes & pitch indices */
  VOICE_COMPUTE_PITCH_INDEX(1)
  VOICE_COMPUTE_PITCH_INDEX(2)
  VOICE_COMPUTE_PITCH_INDEX(3)

  return 0;
}

/*******************************************************************************
** voice_sync_to_key()
*******************************************************************************/
short int voice_sync_to_key(int voice_index)
{
  voice* v;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain voice pointer */
  v = &G_voice_bank[voice_index];

  /* reset phases if necessary */
  if (v->sync == PATCH_SYNC_ON)
  {
    v->osc_1_phase = 0;
    v->osc_2_phase = 0;
    v->osc_3_phase = 0;

    v->chorus_1_phase = 0;
    v->chorus_2_phase = 0;
    v->chorus_3_phase = 0;

    v->osc_1_lfsr = 0x0001;
    v->osc_2_lfsr = 0x0001;
    v->osc_3_lfsr = 0x0001;
  }

  return 0;
}

/*******************************************************************************
** voice_update_all()
*******************************************************************************/
short int voice_update_all()
{
  int osc_1_env_index;
  int osc_2_env_index;
  int osc_3_env_index;

  int osc_1_level;
  int osc_2_level;
  int osc_3_level;

  int chorus_1_level;
  int chorus_2_level;
  int chorus_3_level;

  int pitch_adjustment;

  int adjusted_pitch_index;

  unsigned int masked_phase;

  int final_index;

  int osc_1_phase_mod;
  int osc_2_phase_mod;

  int k;

  voice* v;

  /* update all voices */
  for (k = 0; k < BANK_NUM_VOICES; k++)
  {
    v = &G_voice_bank[k];

    /* update adjustments */
    pitch_adjustment = 0;

    pitch_adjustment += v->vibrato_input;
    pitch_adjustment += v->sweep_input;
    pitch_adjustment += v->pitch_wheel_pos;

    /* update envelopes */
    osc_1_env_index = v->env_1_input;
    osc_2_env_index = v->env_2_input;
    osc_3_env_index = v->env_3_input;

    /* bound envelopes */
    VOICE_BOUND_ENVELOPE(1)
    VOICE_BOUND_ENVELOPE(2)
    VOICE_BOUND_ENVELOPE(3)

    /* update phases */
    VOICE_UPDATE_PHASE(1)
    VOICE_UPDATE_PHASE(2)
    VOICE_UPDATE_PHASE(3)

    /* update oscillator 1 level */
    VOICE_WAVETABLE_LOOKUP(osc, 1, 0)
    VOICE_COMPUTE_PHASE_MOD(osc, 1)

    /* update oscillator 2 level */
    if (v->algorithm == PATCH_ALGORITHM_1_FM_2_FM_3)
    {
      VOICE_WAVETABLE_LOOKUP(osc, 2, osc_1_phase_mod)
    }
    else
    {
      VOICE_WAVETABLE_LOOKUP(osc, 2, 0)
    }

    VOICE_COMPUTE_PHASE_MOD(osc, 2)

    /* update oscillator 3 level */
    if ((v->algorithm == PATCH_ALGORITHM_1_FM_2_FM_3)   || 
        (v->algorithm == PATCH_ALGORITHM_2_FM_3_ADD_1))
    {
      VOICE_WAVETABLE_LOOKUP(osc, 3, osc_2_phase_mod)
    }
    else if (v->algorithm == PATCH_ALGORITHM_1_ADD_2_FM_3)
    {
      VOICE_WAVETABLE_LOOKUP(osc, 3, (osc_1_phase_mod + osc_2_phase_mod))
    }
    else
    {
      VOICE_WAVETABLE_LOOKUP(osc, 3, 0)
    }

    /* determine level */
    if ((v->algorithm == PATCH_ALGORITHM_1_FM_2_FM_3) || 
        (v->algorithm == PATCH_ALGORITHM_1_ADD_2_FM_3))
    {
      v->level = osc_3_level;
    }
    else if (v->algorithm == PATCH_ALGORITHM_2_FM_3_ADD_1)
    {
      v->level = osc_1_level + osc_3_level;
    }
    else
      v->level = osc_1_level + osc_2_level + osc_3_level;
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

  /* 12 bit envelope & waveform values for each bit, in db:                 */
  /* 3(8), 3(4), 3(2), 3(1), 3/2, 3/4, 3/8, 3/16, 3/32, 3/64, 3/128, 3/256  */

  /* db to linear scale conversion */
  S_voice_db_to_linear_table[0] = VOICE_MAX_VOLUME_LINEAR;
  S_voice_db_to_linear_table[VOICE_DB_TO_LINEAR_TABLE_SIZE - 1] = VOICE_MAX_ATTENUATION_LINEAR;

  for (m = 1; m < VOICE_DB_TO_LINEAR_TABLE_SIZE - 1; m++)
  {
    S_voice_db_to_linear_table[m] = 
      (short int) ((VOICE_MAX_VOLUME_LINEAR * exp(-log(10) * (VOICE_DB_STEP_12_BIT / 10) * m)) + 0.5f);
  }

  /* the wavetables have 1024 entries per period */

  /* wavetable (sine) */
  S_voice_wavetable_sine[0] = VOICE_MAX_ATTENUATION_DB;
  S_voice_wavetable_sine[VOICE_WAVETABLE_SIZE_QUARTER] = VOICE_MAX_VOLUME_DB;

  for (m = 1; m < VOICE_WAVETABLE_SIZE_QUARTER; m++)
  {
    val = sin(TWO_PI * (m / ((float) VOICE_WAVETABLE_SIZE_FULL)));
    S_voice_wavetable_sine[m] = (short int) ((10 * (log(1 / val) / log(10)) / VOICE_DB_STEP_12_BIT) + 0.5f);
    S_voice_wavetable_sine[VOICE_WAVETABLE_SIZE_HALF - m] = S_voice_wavetable_sine[m];
  }

  /* wavetable (triangle) */
  S_voice_wavetable_tri[0] = VOICE_MAX_ATTENUATION_DB;
  S_voice_wavetable_tri[VOICE_WAVETABLE_SIZE_QUARTER] = VOICE_MAX_VOLUME_DB;

  for (m = 1; m < VOICE_WAVETABLE_SIZE_QUARTER; m++)
  {
    val = m / ((float) VOICE_WAVETABLE_SIZE_QUARTER);
    S_voice_wavetable_tri[m] = (short int) ((10 * (log(1 / val) / log(10)) / VOICE_DB_STEP_12_BIT) + 0.5f);
    S_voice_wavetable_tri[VOICE_WAVETABLE_SIZE_HALF - m] = S_voice_wavetable_tri[m];
  }

  /* wavetable (sawtooth) */
  S_voice_wavetable_saw[0] = VOICE_MAX_VOLUME_DB;
  S_voice_wavetable_saw[VOICE_WAVETABLE_SIZE_HALF] = VOICE_MAX_ATTENUATION_DB;

  for (m = 1; m < VOICE_WAVETABLE_SIZE_HALF; m++)
  {
    val = (VOICE_WAVETABLE_SIZE_HALF - m) / ((float) VOICE_WAVETABLE_SIZE_HALF);
    S_voice_wavetable_saw[m] = (short int) ((10 * (log(1 / val) / log(10)) / VOICE_DB_STEP_12_BIT) + 0.5f);
    S_voice_wavetable_saw[VOICE_WAVETABLE_SIZE_FULL - m] = S_voice_wavetable_saw[m];
  }

  /* wave phase increment table */
  for (m = 0; m < TUNING_NUM_INDICES; m++)
  {
    val = 440.0f * exp(log(2) * ((m - TUNING_INDEX_A4) / (12.0f * TUNING_NUM_SEMITONE_STEPS)));

    S_voice_wave_phase_increment_table[m] = 
      (unsigned int) ((val * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
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

  return 0;
}

