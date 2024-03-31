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

#define VOICE_VELOCITY_LEVEL_STEP (4 * 32)

#define VOICE_OSC_WAVETABLE_LOOKUP(num, phase)                                 \
  masked_phase = ((phase) & 0x3FF);                                            \
                                                                               \
  /* determine waveform level (db) */                                          \
  if (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_TRIANGLE)                  \
  {                                                                            \
    if (masked_phase < 512)                                                    \
      final_index = S_voice_wavetable_saw[2 * masked_phase];                   \
    else                                                                       \
      final_index = S_voice_wavetable_saw[2 * (masked_phase - 512)];           \
  }                                                                            \
  else if (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_SQUARE)               \
    final_index = 0;                                                           \
  else if (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_SAW)                  \
    final_index = S_voice_wavetable_saw[masked_phase];                         \
  else                                                                         \
    final_index = 4095;                                                        \
                                                                               \
  /* apply envelope */                                                         \
  final_index += osc_##num##_env_index;                                        \
                                                                               \
  if (final_index < 0)                                                         \
    final_index = 0;                                                           \
  else if (final_index > 4095)                                                 \
    final_index = 4095;                                                        \
                                                                               \
  /* determine waveform level (linear) */                                      \
  if ((v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_TRIANGLE)  ||             \
      (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_SQUARE)    ||             \
      (v->osc_##num##_waveform == PATCH_OSC_WAVEFORM_SAW))                     \
  {                                                                            \
    if (masked_phase < 512)                                                    \
      osc_##num##_level = S_voice_db_to_linear_table[final_index];             \
    else                                                                       \
      osc_##num##_level = -S_voice_db_to_linear_table[final_index];            \
  }                                                                            \
  else                                                                         \
    osc_##num##_level = S_voice_db_to_linear_table[4095];

#if 0
#define VOICE_FM_WAVETABLE_LOOKUP(num, phase)                                  \
  masked_phase = ((phase) & 0x3FF);                                            \
                                                                               \
  /* determine waveform level (db) */                                          \
  if ((v->osc_##num##_waveform == PATCH_FM_WAVEFORM_SINE) ||                   \
      (v->osc_##num##_waveform == PATCH_FM_WAVEFORM_FULL))                     \
  {                                                                            \
    if (masked_phase < 512)                                                    \
      final_index = S_voice_wavetable_sine[masked_phase];                      \
    else                                                                       \
      final_index = S_voice_wavetable_sine[masked_phase - 512];                \
  }                                                                            \
  else if (v->osc_##num##_waveform == PATCH_FM_WAVEFORM_HALF)                  \
  {                                                                            \
    if (masked_phase < 512)                                                    \
      final_index = S_voice_wavetable_sine[masked_phase];                      \
    else                                                                       \
      final_index = 4095;                                                      \
  }                                                                            \
  else if (v->osc_##num##_waveform == PATCH_FM_WAVEFORM_QUARTER)               \
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
  else if ( (v->osc_##num##_waveform == PATCH_FM_WAVEFORM_ALTERNATING) ||      \
            (v->osc_##num##_waveform == PATCH_FM_WAVEFORM_CAMEL))              \
  {                                                                            \
    if (masked_phase < 256)                                                    \
      final_index = S_voice_wavetable_sine[2 * masked_phase];                  \
    else if (masked_phase < 512)                                               \
      final_index = S_voice_wavetable_sine[2 * (masked_phase - 256)];          \
    else                                                                       \
      final_index = 4095;                                                      \
  }                                                                            \
  else if (v->osc_##num##_waveform == PATCH_FM_WAVEFORM_SQUARE)                \
    final_index = 0;                                                           \
  else if (v->osc_##num##_waveform == PATCH_FM_WAVEFORM_LOG_SAW)               \
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
  final_index += osc_##num##_env_index;                                        \
                                                                               \
  if (final_index < 0)                                                         \
    final_index = 0;                                                           \
  else if (final_index > 4095)                                                 \
    final_index = 4095;                                                        \
                                                                               \
  /* determine waveform level (linear) */                                      \
  if ((v->osc_##num##_waveform == PATCH_FM_WAVEFORM_SINE)   ||                 \
      (v->osc_##num##_waveform == PATCH_FM_WAVEFORM_SQUARE) ||                 \
      (v->osc_##num##_waveform == PATCH_FM_WAVEFORM_LOG_SAW))                  \
  {                                                                            \
    if (masked_phase < 512)                                                    \
      osc_level[num] = S_voice_db_to_linear_table[final_index];                \
    else                                                                       \
      osc_level[num] = -S_voice_db_to_linear_table[final_index];               \
  }                                                                            \
  else if ( (v->osc_##num##_waveform == PATCH_FM_WAVEFORM_HALF)     ||         \
            (v->osc_##num##_waveform == PATCH_FM_WAVEFORM_FULL)     ||         \
            (v->osc_##num##_waveform == PATCH_FM_WAVEFORM_QUARTER)  ||         \
            (v->osc_##num##_waveform == PATCH_FM_WAVEFORM_CAMEL))              \
  {                                                                            \
    osc_level[num] = S_voice_db_to_linear_table[final_index];                  \
  }                                                                            \
  else if (v->osc_##num##_waveform == PATCH_FM_WAVEFORM_ALTERNATING)           \
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

#define VOICE_UPDATE_FM_MODULATOR_FEEDBACK()                                   \
  /* compute feedback phase mod                     */                         \
  /* we take the average of the last two values,    */                         \
  /* and then find the phase mod amount normally    */                         \
  /* (right shift by another 3, and then mask it).  */                         \
  fm_fb_mod = ((v->feed_in[0] + v->feed_in[1]) >> 4) & 0x3FF;                  \
                                                                               \
  /* update oscillator */                                                      \
  VOICE_FM_WAVETABLE_LOOKUP(2, (v->osc_2_phase >> 18) + fm_fb_mod)             \
                                                                               \
  /* cycle feedback */                                                         \
  v->feed_in[1] = v->feed_in[0];                                               \
  v->feed_in[0] = (osc_2_level * v->fm_feedback_multiplier) >> 6;

#define VOICE_UPDATE_FM_MODULATOR()                                            \
  VOICE_FM_WAVETABLE_LOOKUP(2, (v->osc_2_phase >> 18) + fm_fb_mod)             \
                                                                               \
  fm_phase_mod = (osc_2_level >> 3) & 0x3FF;

#define VOICE_UPDATE_FM_CARRIER()                                              \
  VOICE_FM_WAVETABLE_LOOKUP(1, (v->osc_1_phase >> 18) + fm_phase_mod)
#endif

#define VOICE_BOUND_PITCH_INDEX(pitch_index)                                   \
  if (pitch_index < 0)                                                         \
    pitch_index = 0;                                                           \
  else if (pitch_index >= TUNING_NUM_INDICES)                                  \
    pitch_index = TUNING_NUM_INDICES - 1;

#if 0
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
#endif

/* multiple table */

/* the values are in steps (cents)  */
/* they form the harmonic series!   */
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
static int  S_voice_detune_table[PATCH_OSC_DETUNE_NUM_VALUES];

#if 0
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
#endif

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

/* wavetables */
static short int S_voice_wavetable_sine[512];
static short int S_voice_wavetable_saw[1024];

/* phase increment table */
static unsigned int S_voice_wave_phase_increment_table[TUNING_NUM_INDICES];

#if 0
/* noise phase increment table */
static unsigned int S_voice_noise_phase_increment_table[PATCH_NOISE_FREQUENCY_NUM_VALUES];
#endif

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

    /* program */
    v->program = PATCH_PROGRAM_DEFAULT;

    /* sync */
    v->sync = PATCH_SYNC_DEFAULT;

    /* currently playing note, pitch indices */
    v->base_note = TUNING_NOTE_BLANK;

    v->osc_1_pitch_index = 0;
    v->osc_2_pitch_index = 0;

    /* phases */
    v->osc_1_phase = 0;
    v->osc_2_phase = 0;

    /* voice parameters */
    v->osc_1_waveform = PATCH_OSC_WAVEFORM_DEFAULT;
    v->osc_2_waveform = PATCH_OSC_WAVEFORM_DEFAULT;

    v->osc_1_offset = 0;
    v->osc_2_offset = 0;

    /* amplitude effect modes */
    v->tremolo_mode = PATCH_TREMOLO_MODE_DEFAULT;
    v->boost_mode = PATCH_BOOST_MODE_DEFAULT;
    v->velocity_mode = PATCH_VELOCITY_MODE_DEFAULT;

    /* velocity scaling, adjustment */
    v->velocity_scaling_amount = 
      S_voice_velocity_scaling_table[PATCH_VELOCITY_SCALING_DEFAULT - PATCH_VELOCITY_SCALING_LOWER_BOUND];

    v->velocity_adjustment = 0;

    /* envelope level */
    v->env_input = 4095;

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

  /* program */
  if ((p->program >= PATCH_PROGRAM_LOWER_BOUND) && 
      (p->program <= PATCH_PROGRAM_UPPER_BOUND))
  {
    v->program = p->program;
  }
  else
    v->program = PATCH_PROGRAM_LOWER_BOUND;

  /* sync */
  if ((p->sync_osc >= PATCH_SYNC_LOWER_BOUND) && 
      (p->sync_osc <= PATCH_SYNC_UPPER_BOUND))
  {
    v->sync = p->sync_osc;
  }
  else
    v->sync = PATCH_SYNC_LOWER_BOUND;

  /* waveforms */
  if ((p->osc_1_waveform >= PATCH_OSC_WAVEFORM_LOWER_BOUND) && 
      (p->osc_1_waveform <= PATCH_OSC_WAVEFORM_UPPER_BOUND))
  {
    v->osc_1_waveform = p->osc_1_waveform;
  }
  else
    v->osc_1_waveform = PATCH_OSC_WAVEFORM_LOWER_BOUND;

  if ((p->osc_2_waveform >= PATCH_OSC_WAVEFORM_LOWER_BOUND) && 
      (p->osc_2_waveform <= PATCH_OSC_WAVEFORM_UPPER_BOUND))
  {
    v->osc_2_waveform = p->osc_2_waveform;
  }
  else
    v->osc_2_waveform = PATCH_OSC_WAVEFORM_LOWER_BOUND;

  /* oscillator 1 offset (detune) */
  if ((p->osc_1_detune >= PATCH_OSC_DETUNE_LOWER_BOUND) && 
      (p->osc_1_detune <= PATCH_OSC_DETUNE_UPPER_BOUND))
  {
    v->osc_1_offset = S_voice_detune_table[p->osc_1_detune - PATCH_OSC_DETUNE_LOWER_BOUND];
  }
  else
    v->osc_1_offset = 0;

  /* oscillator 2 offset (detune, multiple, divisor) */
  if ((p->osc_2_detune >= PATCH_OSC_DETUNE_LOWER_BOUND) && 
      (p->osc_2_detune <= PATCH_OSC_DETUNE_UPPER_BOUND))
  {
    v->osc_2_offset = S_voice_detune_table[p->osc_2_detune - PATCH_OSC_DETUNE_LOWER_BOUND];
  }
  else
    v->osc_2_offset = 0;

  if ((p->osc_2_multiple >= PATCH_OSC_MULTIPLE_LOWER_BOUND) && 
      (p->osc_2_multiple <= PATCH_OSC_MULTIPLE_UPPER_BOUND))
  {
    v->osc_2_offset += S_voice_multiple_table[p->osc_2_multiple - PATCH_OSC_MULTIPLE_LOWER_BOUND];
  }

  if ((p->osc_2_divisor >= PATCH_OSC_DIVISOR_LOWER_BOUND) && 
      (p->osc_2_divisor <= PATCH_OSC_DIVISOR_UPPER_BOUND))
  {
    v->osc_2_offset -= S_voice_multiple_table[p->osc_2_divisor - PATCH_OSC_DIVISOR_LOWER_BOUND];
  }

  /* determine oscillator notes and pitch indices */
  v->osc_1_pitch_index = 
    (v->base_note * TUNING_NUM_SEMITONE_STEPS) + v->osc_1_offset;

  v->osc_2_pitch_index = 
    (v->base_note * TUNING_NUM_SEMITONE_STEPS) + v->osc_2_offset;

  VOICE_BOUND_PITCH_INDEX(v->osc_1_pitch_index)
  VOICE_BOUND_PITCH_INDEX(v->osc_2_pitch_index)

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

  return 0;
}

/*******************************************************************************
** voice_set_note()
*******************************************************************************/
short int voice_set_note(int voice_index, int note, int vel)
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

  /* set note velocity adjustment */
  if ((vel >=  MIDI_CONT_NOTE_VELOCITY_LOWER_BOUND) && 
      (vel <=  MIDI_CONT_NOTE_VELOCITY_UPPER_BOUND))
  {
    v->velocity_adjustment = 
      (v->velocity_scaling_amount * (MIDI_CONT_NOTE_VELOCITY_BREAK_POINT - vel)) / MIDI_CONT_NOTE_VELOCITY_DIVISOR;
  }
  else
    v->velocity_adjustment = 0;

  /* determine notes & pitch indices */
  v->osc_1_pitch_index = 
    (v->base_note * TUNING_NUM_SEMITONE_STEPS) + v->osc_1_offset;

  v->osc_2_pitch_index = 
    (v->base_note * TUNING_NUM_SEMITONE_STEPS) + v->osc_2_offset;

  VOICE_BOUND_PITCH_INDEX(v->osc_1_pitch_index)
  VOICE_BOUND_PITCH_INDEX(v->osc_2_pitch_index)

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

#if 0
    v->noise_lfsr = 0x0001;
    v->noise_phase = 0;
#endif
  }

  return 0;
}

/*******************************************************************************
** voice_update_all()
*******************************************************************************/
short int voice_update_all()
{
  short int osc_1_env_index;
  short int osc_2_env_index;

  int osc_1_level;
  int osc_2_level;

  int pitch_adjustment;

  int amplitude_adjustment;
  int cutoff_adjustment;
  int extra_adjustment;

  int adjusted_pitch_index;

  unsigned int masked_phase;
  int final_index;

  int k;
  int m;

  voice* v;

  /* update all voices */
  for (k = 0; k < BANK_NUM_VOICES; k++)
  {
    v = &G_voice_bank[k];

    /* update adjustments */
    pitch_adjustment = 0;
    amplitude_adjustment = 0;
    cutoff_adjustment = 0;
    extra_adjustment = 0;

    pitch_adjustment += v->lfo_input_vibrato;
    pitch_adjustment += v->sweep_input;
    pitch_adjustment += v->bender_input;

    if (v->tremolo_mode == PATCH_TREMOLO_MODE_CARRIERS)
      amplitude_adjustment += v->lfo_input_tremolo;
    else if (v->tremolo_mode == PATCH_TREMOLO_MODE_MODULATORS)
      cutoff_adjustment += v->lfo_input_tremolo;

    if (v->boost_mode == PATCH_BOOST_MODE_CARRIERS)
      amplitude_adjustment -= v->boost_input;
    else if (v->boost_mode == PATCH_BOOST_MODE_MODULATORS)
      cutoff_adjustment -= v->boost_input;

    if (v->velocity_mode == PATCH_VELOCITY_MODE_CARRIERS)
      amplitude_adjustment += v->velocity_adjustment;
    else if (v->velocity_mode == PATCH_VELOCITY_MODE_MODULATORS)
      cutoff_adjustment += v->velocity_adjustment;

    /* update envelopes */
    osc_1_env_index = v->env_input;
    osc_2_env_index = v->env_input;

    /* apply tremolo, boost, and note velocity */
    osc_1_env_index += amplitude_adjustment;
    osc_2_env_index += amplitude_adjustment;

    /* bound envelopes */
    if (osc_1_env_index < 0)
      osc_1_env_index = 0;
    else if (osc_1_env_index > 4095)
      osc_1_env_index = 4095;

    if (osc_2_env_index < 0)
      osc_2_env_index = 0;
    else if (osc_2_env_index > 4095)
      osc_2_env_index = 4095;

    /* update pitches & phases */
    adjusted_pitch_index = v->osc_1_pitch_index + pitch_adjustment;

    VOICE_BOUND_PITCH_INDEX(adjusted_pitch_index)

    v->osc_1_phase += S_voice_wave_phase_increment_table[adjusted_pitch_index];

    adjusted_pitch_index = v->osc_2_pitch_index + pitch_adjustment;

    VOICE_BOUND_PITCH_INDEX(adjusted_pitch_index)

    v->osc_2_phase += S_voice_wave_phase_increment_table[adjusted_pitch_index];

    /* wraparound phase register (28 bits) */
    if (v->osc_1_phase > 0xFFFFFFF)
      v->osc_1_phase &= 0xFFFFFFF;

    if (v->osc_2_phase > 0xFFFFFFF)
      v->osc_2_phase &= 0xFFFFFFF;

#if 0
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
#endif

    /* update oscillator levels based on current algorithm */
    if (v->program == PATCH_PROGRAM_WAVE_MIX)
    {
      VOICE_OSC_WAVETABLE_LOOKUP(1, (v->osc_1_phase >> 18))

      v->level = osc_1_level;
    }
    else
      v->level = 0;

#if 0
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
#endif
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

  /* wavetable (sawtooth) */
  S_voice_wavetable_saw[0] = 4095;
  S_voice_wavetable_saw[512] = 0;

  for (m = 1; m < 512; m++)
  {
    val = m / 512.0f;
    S_voice_wavetable_saw[m] = (short int) ((10 * (log(1 / val) / log(10)) / DB_STEP_12_BIT) + 0.5f);
    S_voice_wavetable_saw[1024 - m] = S_voice_wavetable_saw[m];
  }

  /* detune table */
  for (m = 0; m < PATCH_OSC_DETUNE_NUM_VALUES; m++)
  {
    S_voice_detune_table[m] = PATCH_OSC_DETUNE_LOWER_BOUND + m;
  }

  /* wave phase increment table */
  for (m = 0; m < TUNING_NUM_INDICES; m++)
  {
    val = 440.0f * exp(log(2) * ((m - TUNING_INDEX_A4) / 1200.0f));

    S_voice_wave_phase_increment_table[m] = 
      (unsigned int) ((val * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
  }

#if 0
  /* noise phase increment table */
  for (m = 0; m < PATCH_NOISE_FREQUENCY_NUM_VALUES; m++)
  {
    S_voice_noise_phase_increment_table[m] = 
      (int) ((S_voice_noise_frequency_table[m] * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
  }
#endif

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

