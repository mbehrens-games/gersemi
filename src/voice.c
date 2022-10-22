/*******************************************************************************
** voice.c (synth voice)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "clock.h"
#include "envelope.h"
#include "filter.h"
#include "lfo.h"
#include "linear.h"
#include "patch.h"
#include "sweep.h"
#include "tuning.h"
#include "voice.h"
#include "waveform.h"

#define VOICE_LOAD_PATCH_SETUP_PITCH_OFFSET(num)                                      \
  v->osc_##num##_offset_coarse = 0;                                                   \
  v->osc_##num##_offset_fine = 0;                                                     \
                                                                                      \
  if ((p->osc_##num##_numerator >= 1) && (p->osc_##num##_numerator <= 16))            \
    v->osc_##num##_offset_coarse += S_multiple_table[p->osc_##num##_numerator - 1];   \
                                                                                      \
  if ((p->osc_##num##_denominator >= 1) && (p->osc_##num##_denominator <= 16))        \
    v->osc_##num##_offset_coarse -= S_multiple_table[p->osc_##num##_denominator - 1]; \
                                                                                      \
  if ((p->osc_##num##_detune >= 0) && (p->osc_##num##_detune <= 16))                  \
    v->osc_##num##_offset_fine = S_detune_table[p->osc_##num##_detune];

#define VOICE_LOAD_PATCH_SETUP_CARRIER_ENVELOPE(num)                           \
  envelope_setup( &v->osc_##num##_env, ENVELOPE_TYPE_CARRIER,                  \
                  p->osc_##num##_amplitude,                                    \
                  p->carr_attack, p->carr_decay_1,                             \
                  p->carr_decay_2, p->carr_release,                            \
                  p->carr_sustain,                                             \
                  p->carr_rate_keyscaling, p->carr_level_keyscaling);

#define VOICE_LOAD_PATCH_SETUP_MODULATOR_ENVELOPE(num)                         \
  envelope_setup( &v->osc_##num##_env, ENVELOPE_TYPE_MODULATOR,                \
                  p->osc_##num##_amplitude,                                    \
                  p->mod_attack, p->mod_decay_1,                               \
                  p->mod_decay_2, p->mod_release,                              \
                  p->mod_sustain,                                              \
                  p->mod_rate_keyscaling, p->mod_level_keyscaling);

#define VOICE_LOAD_PATCH_SETUP_NOISE_ENVELOPE()                                \
  envelope_setup( &v->osc_5_env, ENVELOPE_TYPE_CARRIER,                        \
                  p->noise_amplitude,                                          \
                  p->carr_attack, p->carr_decay_1,                             \
                  p->carr_decay_2, p->carr_release,                            \
                  p->carr_sustain,                                             \
                  p->carr_rate_keyscaling, p->carr_level_keyscaling);

#define VOICE_KEY_ON_DETERMINE_NOTE_AND_BASE_PITCH(num)                           \
  osc_##num##_note = note + v->osc_##num##_offset_coarse;                         \
                                                                                  \
  v->osc_##num##_base_pitch_index =                                               \
    (osc_##num##_note * TUNING_NUM_SEMITONE_STEPS) + v->osc_##num##_offset_fine;  \
                                                                                  \
  if (v->osc_##num##_base_pitch_index < 0)                                        \
    v->osc_##num##_base_pitch_index = 0;                                          \
  else if (v->osc_##num##_base_pitch_index >= TUNING_TABLE_SIZE)                  \
    v->osc_##num##_base_pitch_index = TUNING_TABLE_SIZE - 1;

#define VOICE_UPDATE_ENVELOPE(num)                                             \
  envelope_update(&v->osc_##num##_env);                                        \
                                                                               \
  osc_##num##_env_index = v->osc_##num##_env.level;                            \
                                                                               \
  if (v->osc_##num##_env.type == ENVELOPE_TYPE_CARRIER)                        \
    osc_##num##_env_index += v->tremolo.level;                                 \
  else if (v->osc_##num##_env.type == ENVELOPE_TYPE_MODULATOR)                 \
    osc_##num##_env_index += v->wobble.level;                               \
                                                                               \
  if (osc_##num##_env_index < 0)                                               \
    osc_##num##_env_index = 0;                                                 \
  else if (osc_##num##_env_index > 1023)                                       \
    osc_##num##_env_index = 1023;

#define VOICE_UPDATE_PITCH(num)                                                \
  osc_##num##_pitch_index = v->osc_##num##_base_pitch_index;                   \
  osc_##num##_pitch_index += v->vibrato.level;                                 \
  osc_##num##_pitch_index += v->pitch_sweep.level;                             \
                                                                               \
  if (osc_##num##_pitch_index < 0)                                             \
    osc_##num##_pitch_index = 0;                                               \
  else if (osc_##num##_pitch_index >= TUNING_TABLE_SIZE)                       \
    osc_##num##_pitch_index = TUNING_TABLE_SIZE - 1;

#define VOICE_APPLY_SUBOSC_SYNC_ADJUSTMENT(num)                                \
  if (v->wobble.level < v->extra_env.level)                                    \
  {                                                                            \
    osc_##num##_pitch_index -= v->extra_env.level;                             \
    osc_##num##_pitch_index += v->wobble.level;                                \
  }                                                                            \
                                                                               \
  if (osc_##num##_pitch_index < 0)                                             \
    osc_##num##_pitch_index = 0;                                               \
  else if (osc_##num##_pitch_index >= TUNING_TABLE_SIZE)                       \
    osc_##num##_pitch_index = TUNING_TABLE_SIZE - 1;

#define VOICE_APPLY_SUBOSC_RING_ADJUSTMENT(num)                                \
  if (v->wobble.level < v->extra_env.level)                                    \
  {                                                                            \
    osc_##num##_pitch_index += v->extra_env.level;                             \
    osc_##num##_pitch_index -= v->wobble.level;                                \
  }                                                                            \
                                                                               \
  if (osc_##num##_pitch_index < 0)                                             \
    osc_##num##_pitch_index = 0;                                               \
  else if (osc_##num##_pitch_index >= TUNING_TABLE_SIZE)                       \
    osc_##num##_pitch_index = TUNING_TABLE_SIZE - 1;

#define VOICE_UPDATE_FM_OSCILLATOR_1()                                         \
  /* compute feedback phase mod                     */                         \
  /* we take the average of the last two values,    */                         \
  /* and then find the phase mod amount normally    */                         \
  /* (right shift by another 3, and then mask it).  */                         \
  fb_phase_mod = ((v->feed_in[0] + v->feed_in[1]) >> 4) & 0x3FF;               \
                                                                               \
  /* oscillator 1 */                                                           \
  osc_1_level = waveform_wave_lookup( WAVEFORM_SINE,                           \
                                      (v->osc_1_phase >> 18) + fb_phase_mod,   \
                                      osc_1_env_index);                        \
                                                                               \
  osc_1_phase_mod = (osc_1_level >> 3) & 0x3FF;                                \
                                                                               \
  /* cycle feedback                                             */             \
  /* note that the feedback values from 0-8 are divided by 16,  */             \
  /* so the max feedback is 8/16 = 1/2 of the oscillator level. */             \
  v->feed_in[1] = v->feed_in[0];                                               \
  v->feed_in[0] = (osc_1_level * v->feedback) >> 4;

#define VOICE_UPDATE_FM_OSCILLATOR_2_NO_MOD()                                  \
  osc_2_level =                                                                \
    waveform_wave_lookup( WAVEFORM_SINE,                                       \
                          (v->osc_2_phase >> 18),                              \
                          osc_2_env_index);                                    \
                                                                               \
  osc_2_phase_mod = (osc_2_level >> 3) & 0x3FF;

#define VOICE_UPDATE_FM_OSCILLATOR_2_MOD_1()                                   \
  osc_2_level =                                                                \
    waveform_wave_lookup( WAVEFORM_SINE,                                       \
                          (v->osc_2_phase >> 18) + osc_1_phase_mod,            \
                          osc_2_env_index);                                    \
                                                                               \
  osc_2_phase_mod = (osc_2_level >> 3) & 0x3FF;

#define VOICE_UPDATE_FM_OSCILLATOR_3_NO_MOD()                                  \
  osc_3_level =                                                                \
    waveform_wave_lookup( WAVEFORM_SINE,                                       \
                          (v->osc_3_phase >> 18),                              \
                          osc_3_env_index);                                    \
                                                                               \
  osc_3_phase_mod = (osc_3_level >> 3) & 0x3FF;

#define VOICE_UPDATE_FM_OSCILLATOR_3_MOD_1()                                   \
  osc_3_level =                                                                \
    waveform_wave_lookup( WAVEFORM_SINE,                                       \
                          (v->osc_3_phase >> 18) + osc_1_phase_mod,            \
                          osc_3_env_index);                                    \
                                                                               \
  osc_3_phase_mod = (osc_3_level >> 3) & 0x3FF;

#define VOICE_UPDATE_FM_OSCILLATOR_3_MOD_2()                                   \
  osc_3_level =                                                                \
    waveform_wave_lookup( WAVEFORM_SINE,                                       \
                          (v->osc_3_phase >> 18) + osc_2_phase_mod,            \
                          osc_3_env_index);                                    \
                                                                               \
  osc_3_phase_mod = (osc_3_level >> 3) & 0x3FF;

#define VOICE_UPDATE_FM_OSCILLATOR_3_MOD_1_AND_2()                             \
  osc_3_level =                                                                \
    waveform_wave_lookup( WAVEFORM_SINE,                                       \
                          (v->osc_3_phase >> 18)  + osc_1_phase_mod            \
                                                  + osc_2_phase_mod,           \
                          osc_3_env_index);                                    \
                                                                               \
  osc_3_phase_mod = (osc_3_level >> 3) & 0x3FF;

#define VOICE_UPDATE_FM_OSCILLATOR_4_NO_MOD()                                  \
  osc_4_level =                                                                \
    waveform_wave_lookup( WAVEFORM_SINE,                                       \
                          (v->osc_4_phase >> 18),                              \
                          osc_4_env_index);

#define VOICE_UPDATE_FM_OSCILLATOR_4_MOD_1()                                   \
  osc_4_level =                                                                \
    waveform_wave_lookup( WAVEFORM_SINE,                                       \
                          (v->osc_4_phase >> 18) + osc_1_phase_mod,            \
                          osc_4_env_index);

#define VOICE_UPDATE_FM_OSCILLATOR_4_MOD_2()                                   \
  osc_4_level =                                                                \
    waveform_wave_lookup( WAVEFORM_SINE,                                       \
                          (v->osc_4_phase >> 18) + osc_2_phase_mod,            \
                          osc_4_env_index);

#define VOICE_UPDATE_FM_OSCILLATOR_4_MOD_3()                                   \
  osc_4_level =                                                                \
    waveform_wave_lookup( WAVEFORM_SINE,                                       \
                          (v->osc_4_phase >> 18) + osc_3_phase_mod,            \
                          osc_4_env_index);

#define VOICE_UPDATE_FM_OSCILLATOR_4_MOD_1_AND_2()                             \
  osc_4_level =                                                                \
    waveform_wave_lookup( WAVEFORM_SINE,                                       \
                          (v->osc_4_phase >> 18)  + osc_1_phase_mod            \
                                                  + osc_2_phase_mod,           \
                          osc_4_env_index);

#define VOICE_UPDATE_FM_OSCILLATOR_4_MOD_1_AND_3()                             \
  osc_4_level =                                                                \
    waveform_wave_lookup( WAVEFORM_SINE,                                       \
                          (v->osc_4_phase >> 18)  + osc_1_phase_mod            \
                                                  + osc_3_phase_mod,           \
                          osc_4_env_index);

#define VOICE_UPDATE_FM_OSCILLATOR_4_MOD_2_AND_3()                             \
  osc_4_level =                                                                \
    waveform_wave_lookup( WAVEFORM_SINE,                                       \
                          (v->osc_4_phase >> 18)  + osc_2_phase_mod            \
                                                  + osc_3_phase_mod,           \
                          osc_4_env_index);

#define VOICE_UPDATE_FM_OSCILLATOR_4_MOD_1_2_AND_3()                           \
  osc_4_level =                                                                \
    waveform_wave_lookup( WAVEFORM_SINE,                                       \
                          (v->osc_4_phase >> 18)  + osc_1_phase_mod            \
                                                  + osc_2_phase_mod            \
                                                  + osc_3_phase_mod,           \
                          osc_4_env_index);

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
static int  S_multiple_table[16] = 
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
static int  S_detune_table[17] = 
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
static int  S_noise_note_table[16] = 
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

/*******************************************************************************
** voice_setup()
*******************************************************************************/
short int voice_setup(voice* v)
{
  if (v == NULL)
    return 1;

  /* current note */
  v->note = 0;

  /* base pitch table indices */
  v->osc_1_base_pitch_index = 0;
  v->osc_2_base_pitch_index = 0;
  v->osc_3_base_pitch_index = 0;
  v->osc_4_base_pitch_index = 0;
  v->osc_5_base_pitch_index = 0;

  /* phases */
  v->osc_1_phase = 0;
  v->osc_2_phase = 0;
  v->osc_3_phase = 0;
  v->osc_4_phase = 0;
  v->osc_5_phase = 0;

  /* program */
  v->program = VOICE_PROGRAM_SYNC_SQUARE;

  /* pitch offsets */
  v->osc_1_offset_coarse = 0;
  v->osc_1_offset_fine = 0;

  v->osc_2_offset_coarse = 0;
  v->osc_2_offset_fine = 0;

  v->osc_3_offset_coarse = 0;
  v->osc_3_offset_fine = 0;

  v->osc_4_offset_coarse = 0;
  v->osc_4_offset_fine = 0;

  /* feedback */
  v->feedback = 0;

  /* noise generator */
  v->noise_period = 0;
  v->noise_alternate = 0;

  v->noise_lfsr = 0x0001;

  /* envelopes */
  envelope_setup( &v->osc_1_env, ENVELOPE_TYPE_CARRIER, 
                  0, 1, 1, 1, 1, 0, 1, 1);

  envelope_setup( &v->osc_2_env, ENVELOPE_TYPE_CARRIER, 
                  0, 1, 1, 1, 1, 0, 1, 1);

  envelope_setup( &v->osc_3_env, ENVELOPE_TYPE_CARRIER, 
                  0, 1, 1, 1, 1, 0, 1, 1);

  envelope_setup( &v->osc_4_env, ENVELOPE_TYPE_CARRIER, 
                  0, 1, 1, 1, 1, 0, 1, 1);

  envelope_setup( &v->osc_5_env, ENVELOPE_TYPE_CARRIER, 
                  0, 1, 1, 1, 1, 0, 1, 1);

  /* linear envelope */
  linear_setup(&v->extra_env, LINEAR_TYPE_PITCH, 
                              LINEAR_MODE_CONSTANT, 1, 1);

  /* lfos */
  lfo_setup(&v->vibrato, LFO_TYPE_VIBRATO, LFO_MODE_TRIANGLE);
  lfo_setup(&v->tremolo, LFO_TYPE_TREMOLO, LFO_MODE_TRIANGLE);
  lfo_setup(&v->wobble, LFO_TYPE_WOBBLE_PITCH, LFO_MODE_TRIANGLE);

  /* sweep */
  sweep_setup(&v->pitch_sweep);

  /* filters */
  filter_setup(&v->lowpass, FILTER_MODE_LOWPASS, FILTER_LOWPASS_CUTOFF_C8);
  filter_setup(&v->highpass, FILTER_MODE_HIGHPASS, FILTER_HIGHPASS_CUTOFF_A0);

  /* feedback values */
  v->feed_in[0] = 0;
  v->feed_in[1] = 0;

  /* output level */
  v->level = 0;

  return 0;
}

/*******************************************************************************
** voice_load_patch()
*******************************************************************************/
short int voice_load_patch(voice* v, patch* p)
{
  if (v == NULL)
    return 1;

  if (p == NULL)
    return 1;

  /* current note */
  v->note = 0;

  /* base pitch table indices */
  v->osc_1_base_pitch_index = 0;
  v->osc_2_base_pitch_index = 0;
  v->osc_3_base_pitch_index = 0;
  v->osc_4_base_pitch_index = 0;
  v->osc_5_base_pitch_index = 0;

  /* phases */
  v->osc_1_phase = 0;
  v->osc_2_phase = 0;
  v->osc_3_phase = 0;
  v->osc_4_phase = 0;
  v->osc_5_phase = 0;

  /* program */
  if ((p->program >= 0) && (p->program < VOICE_NUM_PROGRAMS))
    v->program = p->program;
  else
    v->program = VOICE_PROGRAM_SYNC_SQUARE;

  /* pitch offsets */
  VOICE_LOAD_PATCH_SETUP_PITCH_OFFSET(1)
  VOICE_LOAD_PATCH_SETUP_PITCH_OFFSET(2)
  VOICE_LOAD_PATCH_SETUP_PITCH_OFFSET(3)
  VOICE_LOAD_PATCH_SETUP_PITCH_OFFSET(4)

  /* feedback */
  if ((p->feedback >= 0) && (p->feedback <= 8))
    v->feedback = p->feedback;
  else
    v->feedback = 0;

  /* noise generator & envelope */
  if ((p->noise_period >= 0) && (p->noise_period < 16))
    v->noise_period = p->noise_period;
  else
    v->noise_period = 0;

  if ((p->noise_alternate >= 0) && (p->noise_alternate <= 1))
    v->noise_alternate = p->noise_alternate;
  else
    v->noise_alternate = 0;

  VOICE_LOAD_PATCH_SETUP_NOISE_ENVELOPE()

  /* oscillator envelopes */
  if ((v->program == VOICE_PROGRAM_SYNC_SQUARE)   || 
      (v->program == VOICE_PROGRAM_SYNC_TRIANGLE) || 
      (v->program == VOICE_PROGRAM_SYNC_SAW)      || 
      (v->program == VOICE_PROGRAM_SYNC_PHAT_SAW) || 
      (v->program == VOICE_PROGRAM_RING_SQUARE)   || 
      (v->program == VOICE_PROGRAM_RING_TRIANGLE) || 
      (v->program == VOICE_PROGRAM_RING_SAW)      || 
      (v->program == VOICE_PROGRAM_RING_PHAT_SAW) || 
      (v->program == VOICE_PROGRAM_PULSE_WAVES))
  {
    VOICE_LOAD_PATCH_SETUP_MODULATOR_ENVELOPE(1)
    VOICE_LOAD_PATCH_SETUP_MODULATOR_ENVELOPE(2)
    VOICE_LOAD_PATCH_SETUP_CARRIER_ENVELOPE(3)
    VOICE_LOAD_PATCH_SETUP_CARRIER_ENVELOPE(4)
  }
  else if ( (v->program == VOICE_PROGRAM_FM_1_CARRIER_CHAIN)            || 
            (v->program == VOICE_PROGRAM_FM_1_CARRIER_Y)                || 
            (v->program == VOICE_PROGRAM_FM_1_CARRIER_LEFT_CRAB_CLAW)   || 
            (v->program == VOICE_PROGRAM_FM_1_CARRIER_RIGHT_CRAB_CLAW)  || 
            (v->program == VOICE_PROGRAM_FM_1_CARRIER_DIAMOND)          || 
            (v->program == VOICE_PROGRAM_FM_1_CARRIER_THREE_TO_ONE))
  {
    VOICE_LOAD_PATCH_SETUP_MODULATOR_ENVELOPE(1)
    VOICE_LOAD_PATCH_SETUP_MODULATOR_ENVELOPE(2)
    VOICE_LOAD_PATCH_SETUP_MODULATOR_ENVELOPE(3)
    VOICE_LOAD_PATCH_SETUP_CARRIER_ENVELOPE(4)
  }
  else if ( (v->program == VOICE_PROGRAM_FM_2_CARRIERS_TWIN)      || 
            (v->program == VOICE_PROGRAM_FM_2_CARRIERS_STACK)     || 
            (v->program == VOICE_PROGRAM_FM_2_CARRIERS_SHARED))
  {
    VOICE_LOAD_PATCH_SETUP_MODULATOR_ENVELOPE(1)
    VOICE_LOAD_PATCH_SETUP_MODULATOR_ENVELOPE(2)
    VOICE_LOAD_PATCH_SETUP_CARRIER_ENVELOPE(3)
    VOICE_LOAD_PATCH_SETUP_CARRIER_ENVELOPE(4)
  }
  else if (v->program == VOICE_PROGRAM_FM_2_CARRIERS_STACK_ALT)
  {
    VOICE_LOAD_PATCH_SETUP_CARRIER_ENVELOPE(1)
    VOICE_LOAD_PATCH_SETUP_MODULATOR_ENVELOPE(2)
    VOICE_LOAD_PATCH_SETUP_MODULATOR_ENVELOPE(3)
    VOICE_LOAD_PATCH_SETUP_CARRIER_ENVELOPE(4)
  }
  else if ( (v->program == VOICE_PROGRAM_FM_3_CARRIERS_ONE_TO_THREE)  || 
            (v->program == VOICE_PROGRAM_FM_3_CARRIERS_ONE_TO_TWO)    || 
            (v->program == VOICE_PROGRAM_FM_3_CARRIERS_ONE_TO_ONE))
  {
    VOICE_LOAD_PATCH_SETUP_MODULATOR_ENVELOPE(1)
    VOICE_LOAD_PATCH_SETUP_CARRIER_ENVELOPE(2)
    VOICE_LOAD_PATCH_SETUP_CARRIER_ENVELOPE(3)
    VOICE_LOAD_PATCH_SETUP_CARRIER_ENVELOPE(4)
  }
  else if (v->program == VOICE_PROGRAM_FM_3_CARRIERS_ONE_TO_ONE_ALT)
  {
    VOICE_LOAD_PATCH_SETUP_CARRIER_ENVELOPE(1)
    VOICE_LOAD_PATCH_SETUP_MODULATOR_ENVELOPE(2)
    VOICE_LOAD_PATCH_SETUP_CARRIER_ENVELOPE(3)
    VOICE_LOAD_PATCH_SETUP_CARRIER_ENVELOPE(4)
  }
  else if (v->program == VOICE_PROGRAM_FM_4_CARRIERS_PIPES)
  {
    VOICE_LOAD_PATCH_SETUP_CARRIER_ENVELOPE(1)
    VOICE_LOAD_PATCH_SETUP_CARRIER_ENVELOPE(2)
    VOICE_LOAD_PATCH_SETUP_CARRIER_ENVELOPE(3)
    VOICE_LOAD_PATCH_SETUP_CARRIER_ENVELOPE(4)
  }

  /* linear envelope */
  if ((v->program == VOICE_PROGRAM_SYNC_SQUARE)   || 
      (v->program == VOICE_PROGRAM_SYNC_TRIANGLE) || 
      (v->program == VOICE_PROGRAM_SYNC_SAW)      || 
      (v->program == VOICE_PROGRAM_SYNC_PHAT_SAW) || 
      (v->program == VOICE_PROGRAM_RING_SQUARE)   || 
      (v->program == VOICE_PROGRAM_RING_TRIANGLE) || 
      (v->program == VOICE_PROGRAM_RING_SAW)      || 
      (v->program == VOICE_PROGRAM_RING_PHAT_SAW))
  {
    linear_setup(&v->extra_env, LINEAR_TYPE_PITCH, 
                                p->extra_mode, 
                                p->extra_rate, 
                                p->extra_keyscaling);
  }
  else if (v->program == VOICE_PROGRAM_PULSE_WAVES)
  {
    linear_setup(&v->extra_env, LINEAR_TYPE_PULSE_WIDTH, 
                                p->extra_mode, 
                                p->extra_rate, 
                                p->extra_keyscaling);
  }
  else
  {
    linear_setup(&v->extra_env, LINEAR_TYPE_PITCH, 
                                LINEAR_MODE_CONSTANT, 1, 1);
  }

  /* lfos */
  if (p->vibrato_alternate == 1)
    lfo_setup(&v->vibrato, LFO_TYPE_VIBRATO_ALTERNATE, p->vibrato_mode);
  else
    lfo_setup(&v->vibrato, LFO_TYPE_VIBRATO, p->vibrato_mode);

  lfo_setup(&v->tremolo, LFO_TYPE_TREMOLO, p->tremolo_mode);

  if ((v->program == VOICE_PROGRAM_SYNC_SQUARE)   || 
      (v->program == VOICE_PROGRAM_SYNC_TRIANGLE) || 
      (v->program == VOICE_PROGRAM_SYNC_SAW)      || 
      (v->program == VOICE_PROGRAM_SYNC_PHAT_SAW) || 
      (v->program == VOICE_PROGRAM_RING_SQUARE)   || 
      (v->program == VOICE_PROGRAM_RING_TRIANGLE) || 
      (v->program == VOICE_PROGRAM_RING_SAW)      || 
      (v->program == VOICE_PROGRAM_RING_PHAT_SAW))
  {
    lfo_setup(&v->wobble, LFO_TYPE_WOBBLE_PITCH, p->wobble_mode);
  }
  else if (v->program == VOICE_PROGRAM_PULSE_WAVES)
    lfo_setup(&v->wobble, LFO_TYPE_WOBBLE_PULSE_WIDTH, p->wobble_mode);
  else
    lfo_setup(&v->wobble, LFO_TYPE_WOBBLE_AMPLITUDE, p->wobble_mode);

  /* filters */
  filter_setup(&v->lowpass, FILTER_MODE_LOWPASS, p->lowpass_cutoff);
  filter_setup(&v->highpass, FILTER_MODE_HIGHPASS, p->highpass_cutoff);

  /* feedback values */
  v->feed_in[0] = 0;
  v->feed_in[1] = 0;

  /* output level */
  v->level = 0;

  return 0;
}

/*******************************************************************************
** voice_set_vibrato()
*******************************************************************************/
short int voice_set_vibrato(voice* v, int depth, int tempo, int speed)
{
  if (v == NULL)
    return 1;

  lfo_set_depth(&v->vibrato, depth);
  lfo_set_speed(&v->vibrato, tempo, speed);

  return 0;
}

/*******************************************************************************
** voice_set_tremolo()
*******************************************************************************/
short int voice_set_tremolo(voice* v, int depth, int tempo, int speed)
{
  if (v == NULL)
    return 1;

  lfo_set_depth(&v->tremolo, depth);
  lfo_set_speed(&v->tremolo, tempo, speed);

  return 0;
}

/*******************************************************************************
** voice_set_wobble()
*******************************************************************************/
short int voice_set_wobble(voice* v, int depth, int tempo, int speed)
{
  if (v == NULL)
    return 1;

  lfo_set_depth(&v->wobble, depth);
  lfo_set_speed(&v->wobble, tempo, speed);

  return 0;
}

/*******************************************************************************
** voice_set_pitch_sweep()
*******************************************************************************/
short int voice_set_pitch_sweep(voice* v, int mode, 
                                          int tempo, 
                                          int speed)
{
  if (v == NULL)
    return 1;

  sweep_set_mode(&v->pitch_sweep, mode);
  sweep_set_speed(&v->pitch_sweep, tempo, speed);

  return 0;
}

/*******************************************************************************
** voice_key_on()
*******************************************************************************/
short int voice_key_on(voice* v, int note, int volume, int brightness)
{
  int osc_1_note;
  int osc_2_note;
  int osc_3_note;
  int osc_4_note;
  int osc_5_note;

  if (v == NULL)
    return 1;

  /* if note is out of range, ignore */
  if ((note < 0) || (note >= TUNING_NUM_NOTES))
    return 0;

  /* make sure the volume is valid */
  if ((volume < 1) || (volume > 9))
    volume = 5;

  /* make sure the brightness is valid */
  if ((brightness < 1) || (brightness > 9))
    brightness = 5;

  /* determine notes & base pitch indices */
  VOICE_KEY_ON_DETERMINE_NOTE_AND_BASE_PITCH(1)
  VOICE_KEY_ON_DETERMINE_NOTE_AND_BASE_PITCH(2)
  VOICE_KEY_ON_DETERMINE_NOTE_AND_BASE_PITCH(3)
  VOICE_KEY_ON_DETERMINE_NOTE_AND_BASE_PITCH(4)

  osc_5_note = S_noise_note_table[v->noise_period];

  v->osc_5_base_pitch_index = osc_5_note * TUNING_NUM_SEMITONE_STEPS;

  /* reset phases */
  v->osc_1_phase = 0;
  v->osc_2_phase = 0;
  v->osc_3_phase = 0;
  v->osc_4_phase = 0;
  v->osc_5_phase = 0;

  /* reset the highpass filter */
  filter_reset(&v->highpass);

  /* trigger envelopes */
  envelope_trigger(&v->osc_1_env, osc_1_note, volume, brightness);
  envelope_trigger(&v->osc_2_env, osc_2_note, volume, brightness);
  envelope_trigger(&v->osc_3_env, osc_3_note, volume, brightness);
  envelope_trigger(&v->osc_4_env, osc_4_note, volume, brightness);
  envelope_trigger(&v->osc_5_env, osc_5_note, volume, brightness);

  /* trigger linear envelope */
  linear_trigger(&v->extra_env, note, brightness);

  /* trigger lfos */
  lfo_trigger(&v->vibrato);
  lfo_trigger(&v->tremolo);
  lfo_trigger(&v->wobble);

  /* trigger sweep */
  sweep_trigger(&v->pitch_sweep, v->note, note);

  /* store current note */
  v->note = note;

  return 0;
}

/*******************************************************************************
** voice_key_off()
*******************************************************************************/
short int voice_key_off(voice* v)
{
  if (v == NULL)
    return 1;

  /* release envelopes */
  envelope_release(&v->osc_1_env);
  envelope_release(&v->osc_2_env);
  envelope_release(&v->osc_3_env);
  envelope_release(&v->osc_4_env);
  envelope_release(&v->osc_5_env);

  return 0;
}

/*******************************************************************************
** voice_update()
*******************************************************************************/
short int voice_update(voice* v)
{
  short int osc_1_env_index;
  short int osc_2_env_index;
  short int osc_3_env_index;
  short int osc_4_env_index;
  short int osc_5_env_index;

  int       osc_1_pitch_index;
  int       osc_2_pitch_index;
  int       osc_3_pitch_index;
  int       osc_4_pitch_index;

  int       fb_phase_mod;
  int       osc_1_phase_mod;
  int       osc_2_phase_mod;
  int       osc_3_phase_mod;

  int       osc_1_level;
  int       osc_2_level;
  int       osc_3_level;
  int       osc_4_level;
  int       osc_5_level;

  int       level;

  if (v == NULL)
    return 1;

  /* update lfos */
  lfo_update(&v->vibrato);
  lfo_update(&v->tremolo);
  lfo_update(&v->wobble);

  /* update sweep */
  sweep_update(&v->pitch_sweep);

  /* update linear envelope */
  linear_update(&v->extra_env);

  /* update envelopes */
  VOICE_UPDATE_ENVELOPE(1)
  VOICE_UPDATE_ENVELOPE(2)
  VOICE_UPDATE_ENVELOPE(3)
  VOICE_UPDATE_ENVELOPE(4)
  VOICE_UPDATE_ENVELOPE(5)

  /* update pitches */
  VOICE_UPDATE_PITCH(1)
  VOICE_UPDATE_PITCH(2)
  VOICE_UPDATE_PITCH(3)
  VOICE_UPDATE_PITCH(4)

  /* apply pitch adjustment from suboscillator if necessary */
  if ((v->program == VOICE_PROGRAM_SYNC_SQUARE)   || 
      (v->program == VOICE_PROGRAM_SYNC_TRIANGLE) || 
      (v->program == VOICE_PROGRAM_SYNC_SAW)      || 
      (v->program == VOICE_PROGRAM_SYNC_PHAT_SAW))
  {
    VOICE_APPLY_SUBOSC_SYNC_ADJUSTMENT(1)
    VOICE_APPLY_SUBOSC_SYNC_ADJUSTMENT(2)
  }
  else if ( (v->program == VOICE_PROGRAM_RING_SQUARE)   || 
            (v->program == VOICE_PROGRAM_RING_TRIANGLE) || 
            (v->program == VOICE_PROGRAM_RING_SAW)      || 
            (v->program == VOICE_PROGRAM_RING_PHAT_SAW))
  {
    VOICE_APPLY_SUBOSC_RING_ADJUSTMENT(1)
    VOICE_APPLY_SUBOSC_RING_ADJUSTMENT(2)
  }

  /* update oscillator phases */
  v->osc_1_phase += G_phase_increment_table[osc_1_pitch_index];
  v->osc_2_phase += G_phase_increment_table[osc_2_pitch_index];
  v->osc_3_phase += G_phase_increment_table[osc_3_pitch_index];
  v->osc_4_phase += G_phase_increment_table[osc_4_pitch_index];

  /* check if oscillators have completed a period */

  /* sync on */
  if ((v->program == VOICE_PROGRAM_SYNC_SQUARE)   || 
      (v->program == VOICE_PROGRAM_SYNC_TRIANGLE) || 
      (v->program == VOICE_PROGRAM_SYNC_SAW)      || 
      (v->program == VOICE_PROGRAM_SYNC_PHAT_SAW))
  {
    /* oscillator 3 syncs with oscillator 1 */
    if (v->osc_1_phase > 0xFFFFFFF)
    {
      v->osc_1_phase &= 0xFFFFFFF;
      v->osc_3_phase = v->osc_1_phase;
    }

    /* oscillator 4 syncs with oscillator 2 */
    if (v->osc_2_phase > 0xFFFFFFF)
    {
      v->osc_2_phase &= 0xFFFFFFF;
      v->osc_4_phase = v->osc_2_phase;
    }

    if (v->osc_3_phase > 0xFFFFFFF)
      v->osc_3_phase &= 0xFFFFFFF;

    if (v->osc_4_phase > 0xFFFFFFF)
      v->osc_4_phase &= 0xFFFFFFF;
  }
  /* sync off */
  else
  {
    if (v->osc_1_phase > 0xFFFFFFF)
      v->osc_1_phase &= 0xFFFFFFF;

    if (v->osc_2_phase > 0xFFFFFFF)
      v->osc_2_phase &= 0xFFFFFFF;

    if (v->osc_3_phase > 0xFFFFFFF)
      v->osc_3_phase &= 0xFFFFFFF;

    if (v->osc_4_phase > 0xFFFFFFF)
      v->osc_4_phase &= 0xFFFFFFF;
  }

  /* update oscillator levels based on current program */

  /* sync square */
  if (v->program == VOICE_PROGRAM_SYNC_SQUARE)
  {
    osc_3_level = waveform_wave_lookup( WAVEFORM_SQUARE, 
                                        (v->osc_3_phase >> 18), 
                                        osc_3_env_index);

    osc_4_level = waveform_wave_lookup( WAVEFORM_SQUARE, 
                                        (v->osc_4_phase >> 18), 
                                        osc_4_env_index);

    level = osc_3_level + osc_4_level;
  }
  /* sync triangle */
  else if (v->program == VOICE_PROGRAM_SYNC_TRIANGLE)
  {
    osc_3_level = waveform_wave_lookup( WAVEFORM_TRIANGLE, 
                                        (v->osc_3_phase >> 18), 
                                        osc_3_env_index);

    osc_4_level = waveform_wave_lookup( WAVEFORM_TRIANGLE, 
                                        (v->osc_4_phase >> 18), 
                                        osc_4_env_index);

    level = osc_3_level + osc_4_level;
  }
  /* sync saw */
  else if (v->program == VOICE_PROGRAM_SYNC_SAW)
  {
    osc_3_level = waveform_wave_lookup( WAVEFORM_SAW, 
                                        (v->osc_3_phase >> 18), 
                                        osc_3_env_index);

    osc_4_level = waveform_wave_lookup( WAVEFORM_SAW, 
                                        (v->osc_4_phase >> 18), 
                                        osc_4_env_index);

    level = osc_3_level + osc_4_level;
  }
  /* sync phat saw */
  else if (v->program == VOICE_PROGRAM_SYNC_PHAT_SAW)
  {
    osc_3_level = waveform_wave_lookup( WAVEFORM_PHAT_SAW, 
                                        (v->osc_3_phase >> 18), 
                                        osc_3_env_index);

    osc_4_level = waveform_wave_lookup( WAVEFORM_PHAT_SAW, 
                                        (v->osc_4_phase >> 18), 
                                        osc_4_env_index);

    level = osc_3_level + osc_4_level;
  }
  /* ring mod square */
  else if (v->program == VOICE_PROGRAM_RING_SQUARE)
  {
    osc_3_level = waveform_wave_lookup( WAVEFORM_SQUARE, 
                                        (v->osc_3_phase >> 18), 
                                        osc_3_env_index);

    osc_4_level = waveform_wave_lookup( WAVEFORM_SQUARE, 
                                        (v->osc_4_phase >> 18), 
                                        osc_4_env_index);

    if (v->osc_1_phase >= 0x8000000)
      osc_3_level = -osc_3_level;

    if (v->osc_2_phase >= 0x8000000)
      osc_4_level = -osc_4_level;

    level = osc_3_level + osc_4_level;
  }
  /* ring mod triangle */
  else if (v->program == VOICE_PROGRAM_RING_TRIANGLE)
  {
    osc_3_level = waveform_wave_lookup( WAVEFORM_TRIANGLE, 
                                        (v->osc_3_phase >> 18), 
                                        osc_3_env_index);

    osc_4_level = waveform_wave_lookup( WAVEFORM_TRIANGLE, 
                                        (v->osc_4_phase >> 18), 
                                        osc_4_env_index);

    if (v->osc_1_phase >= 0x8000000)
      osc_3_level = -osc_3_level;

    if (v->osc_2_phase >= 0x8000000)
      osc_4_level = -osc_4_level;

    level = osc_3_level + osc_4_level;
  }
  /* ring mod saw */
  else if (v->program == VOICE_PROGRAM_RING_SAW)
  {
    osc_3_level = waveform_wave_lookup( WAVEFORM_SAW, 
                                        (v->osc_3_phase >> 18), 
                                        osc_3_env_index);

    osc_4_level = waveform_wave_lookup( WAVEFORM_SAW, 
                                        (v->osc_4_phase >> 18), 
                                        osc_4_env_index);

    if (v->osc_1_phase >= 0x8000000)
      osc_3_level = -osc_3_level;

    if (v->osc_2_phase >= 0x8000000)
      osc_4_level = -osc_4_level;

    level = osc_3_level + osc_4_level;
  }
  /* ring mod phat saw */
  else if (v->program == VOICE_PROGRAM_RING_PHAT_SAW)
  {
    osc_3_level = waveform_wave_lookup( WAVEFORM_PHAT_SAW, 
                                        (v->osc_3_phase >> 18), 
                                        osc_3_env_index);

    osc_4_level = waveform_wave_lookup( WAVEFORM_PHAT_SAW, 
                                        (v->osc_4_phase >> 18), 
                                        osc_4_env_index);

    if (v->osc_1_phase >= 0x8000000)
      osc_3_level = -osc_3_level;

    if (v->osc_2_phase >= 0x8000000)
      osc_4_level = -osc_4_level;

    level = osc_3_level + osc_4_level;
  }
  /* pulse waves */
  else if (v->program == VOICE_PROGRAM_PULSE_WAVES)
  {
    osc_3_level = waveform_pulse_lookup(v->extra_env.level - v->wobble.level, 
                                        (v->osc_3_phase >> 18), 
                                        osc_3_env_index);

    osc_4_level = waveform_pulse_lookup(v->extra_env.level - v->wobble.level, 
                                        (v->osc_4_phase >> 18), 
                                        osc_4_env_index);

    level = osc_3_level + osc_4_level;
  }
  /* 1 carrier - chain  */
  /*   1 -> 2 -> 3 -> 4 */
  else if (v->program == VOICE_PROGRAM_FM_1_CARRIER_CHAIN)
  {
    VOICE_UPDATE_FM_OSCILLATOR_1()
    VOICE_UPDATE_FM_OSCILLATOR_2_MOD_1()
    VOICE_UPDATE_FM_OSCILLATOR_3_MOD_2()
    VOICE_UPDATE_FM_OSCILLATOR_4_MOD_3()

    level = osc_4_level;
  }
  /* 1 carrier - Y        */
  /*   (1 + 2) -> 3 -> 4  */
  else if (v->program == VOICE_PROGRAM_FM_1_CARRIER_Y)
  {
    VOICE_UPDATE_FM_OSCILLATOR_1()
    VOICE_UPDATE_FM_OSCILLATOR_2_NO_MOD()
    VOICE_UPDATE_FM_OSCILLATOR_3_MOD_1_AND_2()
    VOICE_UPDATE_FM_OSCILLATOR_4_MOD_3()

    level = osc_4_level;
  }
  /* 1 carrier - left crab claw */
  /*   1 -> 2, (2 + 3) -> 4     */
  else if (v->program == VOICE_PROGRAM_FM_1_CARRIER_LEFT_CRAB_CLAW)
  {
    VOICE_UPDATE_FM_OSCILLATOR_1()
    VOICE_UPDATE_FM_OSCILLATOR_2_MOD_1()
    VOICE_UPDATE_FM_OSCILLATOR_3_NO_MOD()
    VOICE_UPDATE_FM_OSCILLATOR_4_MOD_2_AND_3()

    level = osc_4_level;
  }
  /* 1 carrier - right crab claw  */
  /*   2 -> 3, (1 + 3) -> 4       */
  else if (v->program == VOICE_PROGRAM_FM_1_CARRIER_RIGHT_CRAB_CLAW)
  {
    VOICE_UPDATE_FM_OSCILLATOR_1()
    VOICE_UPDATE_FM_OSCILLATOR_2_NO_MOD()
    VOICE_UPDATE_FM_OSCILLATOR_3_MOD_2()
    VOICE_UPDATE_FM_OSCILLATOR_4_MOD_1_AND_3()

    level = osc_4_level;
  }
  /* 1 carrier - diamond            */
  /*   1 -> 2, 1 -> 3, (2 + 3) -> 4 */
  else if (v->program == VOICE_PROGRAM_FM_1_CARRIER_DIAMOND)
  {
    VOICE_UPDATE_FM_OSCILLATOR_1()
    VOICE_UPDATE_FM_OSCILLATOR_2_MOD_1()
    VOICE_UPDATE_FM_OSCILLATOR_3_MOD_1()
    VOICE_UPDATE_FM_OSCILLATOR_4_MOD_2_AND_3()

    level = osc_4_level;
  }
  /* 1 carrier - three to one */
  /*   (1 + 2 + 3) -> 4       */
  else if (v->program == VOICE_PROGRAM_FM_1_CARRIER_THREE_TO_ONE)
  {
    VOICE_UPDATE_FM_OSCILLATOR_1()
    VOICE_UPDATE_FM_OSCILLATOR_2_NO_MOD()
    VOICE_UPDATE_FM_OSCILLATOR_3_NO_MOD()
    VOICE_UPDATE_FM_OSCILLATOR_4_MOD_1_2_AND_3()

    level = osc_4_level;
  }
  /* 2 carriers - twin                  */
  /*   1 -> 3, 2 -> 4, output is 3 + 4  */
  else if (v->program == VOICE_PROGRAM_FM_2_CARRIERS_TWIN)
  {
    VOICE_UPDATE_FM_OSCILLATOR_1()
    VOICE_UPDATE_FM_OSCILLATOR_2_NO_MOD()
    VOICE_UPDATE_FM_OSCILLATOR_3_MOD_1()
    VOICE_UPDATE_FM_OSCILLATOR_4_MOD_2()

    level = osc_3_level + osc_4_level;
  }
  /* 2 carriers - stack             */
  /*   1 -> 2 -> 3, output is 3 + 4 */
  else if (v->program == VOICE_PROGRAM_FM_2_CARRIERS_STACK)
  {
    VOICE_UPDATE_FM_OSCILLATOR_1()
    VOICE_UPDATE_FM_OSCILLATOR_2_MOD_1()
    VOICE_UPDATE_FM_OSCILLATOR_3_MOD_2()
    VOICE_UPDATE_FM_OSCILLATOR_4_NO_MOD()

    level = osc_3_level + osc_4_level;
  }
  /* 2 carriers - stack alternate   */
  /*   2 -> 3 -> 4, output is 1 + 4 */
  else if (v->program == VOICE_PROGRAM_FM_2_CARRIERS_STACK_ALT)
  {
    VOICE_UPDATE_FM_OSCILLATOR_1()
    VOICE_UPDATE_FM_OSCILLATOR_2_NO_MOD()
    VOICE_UPDATE_FM_OSCILLATOR_3_MOD_2()
    VOICE_UPDATE_FM_OSCILLATOR_4_MOD_3()

    level = osc_1_level + osc_4_level;
  }
  /* 2 carriers - shared                          */
  /*   1 -> 2 -> 3, 1 -> 2 -> 4, output is 3 + 4  */
  else if (v->program == VOICE_PROGRAM_FM_2_CARRIERS_SHARED)
  {
    VOICE_UPDATE_FM_OSCILLATOR_1()
    VOICE_UPDATE_FM_OSCILLATOR_2_MOD_1()
    VOICE_UPDATE_FM_OSCILLATOR_3_MOD_2()
    VOICE_UPDATE_FM_OSCILLATOR_4_MOD_2()

    level = osc_3_level + osc_4_level;
  }
  /* 3 carriers - one to three                      */
  /*   1 -> 2, 1 -> 3, 1 -> 4, output is 2 + 3 + 4  */
  else if (v->program == VOICE_PROGRAM_FM_3_CARRIERS_ONE_TO_THREE)
  {
    VOICE_UPDATE_FM_OSCILLATOR_1()
    VOICE_UPDATE_FM_OSCILLATOR_2_MOD_1()
    VOICE_UPDATE_FM_OSCILLATOR_3_MOD_1()
    VOICE_UPDATE_FM_OSCILLATOR_4_MOD_1()

    level = osc_2_level + osc_3_level + osc_4_level;
  }
  /* 3 carriers - one to two                */
  /*   1 -> 2, 1 -> 3, output is 2 + 3 + 4  */
  else if (v->program == VOICE_PROGRAM_FM_3_CARRIERS_ONE_TO_TWO)
  {
    VOICE_UPDATE_FM_OSCILLATOR_1()
    VOICE_UPDATE_FM_OSCILLATOR_2_MOD_1()
    VOICE_UPDATE_FM_OSCILLATOR_3_MOD_1()
    VOICE_UPDATE_FM_OSCILLATOR_4_NO_MOD()

    level = osc_2_level + osc_3_level + osc_4_level;
  }
  /* 3 carriers - one to one        */
  /*   1 -> 2, output is 2 + 3 + 4  */
  else if (v->program == VOICE_PROGRAM_FM_3_CARRIERS_ONE_TO_ONE)
  {
    VOICE_UPDATE_FM_OSCILLATOR_1()
    VOICE_UPDATE_FM_OSCILLATOR_2_MOD_1()
    VOICE_UPDATE_FM_OSCILLATOR_3_NO_MOD()
    VOICE_UPDATE_FM_OSCILLATOR_4_NO_MOD()

    level = osc_2_level + osc_3_level + osc_4_level;
  }
  /* 3 carriers - one to one alternate  */
  /*   2 -> 3, output is 1 + 3 + 4      */
  else if (v->program == VOICE_PROGRAM_FM_3_CARRIERS_ONE_TO_ONE_ALT)
  {
    VOICE_UPDATE_FM_OSCILLATOR_1()
    VOICE_UPDATE_FM_OSCILLATOR_2_NO_MOD()
    VOICE_UPDATE_FM_OSCILLATOR_3_MOD_2()
    VOICE_UPDATE_FM_OSCILLATOR_4_NO_MOD()

    level = osc_1_level + osc_3_level + osc_4_level;
  }
  /* 4 carriers - pipes         */
  /*   output is 1 + 2 + 3 + 4  */
  else if (v->program == VOICE_PROGRAM_FM_4_CARRIERS_PIPES)
  {
    VOICE_UPDATE_FM_OSCILLATOR_1()
    VOICE_UPDATE_FM_OSCILLATOR_2_NO_MOD()
    VOICE_UPDATE_FM_OSCILLATOR_3_NO_MOD()
    VOICE_UPDATE_FM_OSCILLATOR_4_NO_MOD()

    level = osc_1_level + osc_2_level + osc_3_level + osc_4_level;
  }
  else
    level = 0;

  /* update noise phase */
  v->osc_5_phase += G_phase_increment_table[v->osc_5_base_pitch_index];

  /* if the noise generator has completed a period, update the lfsr */
  if (v->osc_5_phase > 0xFFFFFFF)
  {
    v->osc_5_phase &= 0xFFFFFFF;

    /* update noise generator (nes) */
    /* 15-bit lfsr, taps on 1 and 2 */
    if ((v->noise_lfsr & 0x0001) ^ ((v->noise_lfsr & 0x0002) >> 1))
      v->noise_lfsr = ((v->noise_lfsr >> 1) & 0x3FFF) | 0x4000;
    else
      v->noise_lfsr = (v->noise_lfsr >> 1) & 0x3FFF;
  }

  /* compute noise level */
  osc_5_level = waveform_noise_lookup(v->noise_alternate, v->noise_lfsr, osc_5_env_index);

  /* mix in noise */
  level += osc_5_level;

  /* apply lowpass filter */
  filter_update(&v->lowpass, level);
  level = v->lowpass.level;

  /* apply highpass filter */
  filter_update(&v->highpass, level);
  level = v->highpass.level;

  /* clipping */
  if (level > 32767)
    level = 32767;
  else if (level < -32768)
    level = -32768;

  /* set voice level */
  v->level = level;

  return 0;
}

