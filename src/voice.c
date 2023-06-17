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
#include "patch.h"
#include "sweep.h"
#include "tuning.h"
#include "voice.h"
#include "waveform.h"

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

  /* program */
  v->program = VOICE_PROGRAM_SYNC_SQUARE;

  /* notes, base pitch table indices, phases, offsets */
  for (m = 0; m < VOICE_NUM_OSCS_AND_ENVS; m++)
  {
    v->osc_note[m] = 0;
    v->osc_base_pitch_index[m] = 0;
    v->osc_phase[m] = 0;
    v->osc_offset_coarse[m] = 0;
    v->osc_offset_fine[m] = 0;
  }

  /* feedback */
  v->feedback = 0;

  v->feed_in[0] = 0;
  v->feed_in[1] = 0;

  /* noise generator */
  v->noise_period = 0;
  v->noise_mix = 0;

  v->noise_lfsr = 0x0001;

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

  /* program */
  if ((p->program >= 0) && (p->program < VOICE_NUM_PROGRAMS))
    v->program = p->program;
  else
    v->program = VOICE_PROGRAM_SYNC_SQUARE;

  /* pitch offsets */
  for (m = 0; m < VOICE_NUM_OSCS_AND_ENVS; m++)
  {
    /* determine base pitch indices */
    v->osc_offset_coarse[m] = 0;
    v->osc_offset_fine[m] = 0;

    if ((p->osc_numerator[m] >= 1) && (p->osc_numerator[m] <= 16))
      v->osc_offset_coarse[m] += S_voice_multiple_table[p->osc_numerator[m] - 1];

    if ((p->osc_denominator[m] >= 1) && (p->osc_denominator[m] <= 16))
      v->osc_offset_coarse[m] -= S_voice_multiple_table[p->osc_denominator[m] - 1];

    if ((p->osc_detune[m] >= 0) && (p->osc_detune[m] <= 16))
      v->osc_offset_fine[m] = S_voice_detune_table[p->osc_detune[m]];
  }

  /* feedback */
  if ((p->feedback >= 0) && (p->feedback <= 8))
    v->feedback = p->feedback;
  else
    v->feedback = 0;

  /* noise generator */
  if ((p->noise_period >= 0) && (p->noise_period < 16))
    v->noise_period = p->noise_period;
  else
    v->noise_period = 0;

  if ((p->noise_mix >= 0) && (p->noise_mix <= 16))
    v->noise_mix = p->noise_mix;
  else
    v->noise_mix = 0;

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

  /* determine notes & base pitch indices, reset phases */
  for (m = 0; m < VOICE_NUM_OSCS_AND_ENVS; m++)
  {
    v->osc_note[m] = note + v->osc_offset_coarse[m];

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

  int osc_pitch_index[VOICE_NUM_OSCS_AND_ENVS];

  int fb_phase_mod;
  int osc_1_phase_mod;
  int osc_2_phase_mod;
  int osc_3_phase_mod;

  int osc_level[VOICE_NUM_OSCS_AND_ENVS];

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

    /* update pitches, phases */
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
    }

    /* check if oscillators have completed a period */

    /* sync on */
    if ((v->program == VOICE_PROGRAM_SYNC_SQUARE)   || 
        (v->program == VOICE_PROGRAM_SYNC_TRIANGLE) || 
        (v->program == VOICE_PROGRAM_SYNC_SAW)      || 
        (v->program == VOICE_PROGRAM_SYNC_PHAT_SAW))
    {
      /* oscillator 3 syncs with oscillator 1 */
      if (v->osc_phase[0] > 0xFFFFFFF)
      {
        v->osc_phase[0] &= 0xFFFFFFF;
        v->osc_phase[2] = v->osc_phase[0];
      }

      /* oscillator 4 syncs with oscillator 2 */
      if (v->osc_phase[1] > 0xFFFFFFF)
      {
        v->osc_phase[1] &= 0xFFFFFFF;
        v->osc_phase[3] = v->osc_phase[1];
      }

      if (v->osc_phase[2] > 0xFFFFFFF)
        v->osc_phase[2] &= 0xFFFFFFF;

      if (v->osc_phase[3] > 0xFFFFFFF)
        v->osc_phase[3] &= 0xFFFFFFF;
    }
    /* sync off */
    else
    {
      if (v->osc_phase[0] > 0xFFFFFFF)
        v->osc_phase[0] &= 0xFFFFFFF;

      if (v->osc_phase[1] > 0xFFFFFFF)
        v->osc_phase[1] &= 0xFFFFFFF;

      if (v->osc_phase[2] > 0xFFFFFFF)
        v->osc_phase[2] &= 0xFFFFFFF;

      if (v->osc_phase[3] > 0xFFFFFFF)
        v->osc_phase[3] &= 0xFFFFFFF;
    }

    /* update oscillator levels based on current program */

    osc_level[2] = waveform_wave_lookup(WAVEFORM_SQUARE, 
                                        (v->osc_phase[2] >> 18), 
                                        osc_env_index[2]);

    osc_level[3] = waveform_wave_lookup(WAVEFORM_SQUARE, 
                                        (v->osc_phase[3] >> 18), 
                                        osc_env_index[3]);

    level = osc_level[2] + osc_level[3];

#if 0
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
      osc_3_level = waveform_pulse_lookup(v->extra_env.level, 
                                          (v->osc_3_phase >> 18), 
                                          osc_3_env_index);

      osc_4_level = waveform_pulse_lookup(v->extra_env.level, 
                                          (v->osc_4_phase >> 18), 
                                          osc_4_env_index);

      level = osc_3_level + osc_4_level;

      level = 0;
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
#endif

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
    noise_level = waveform_noise_lookup(v->noise_alternate, v->noise_lfsr, osc_1_env_index);

    /* mix in noise */
    level += noise_level;
#endif

    /* set voice level */
    v->level = level;
  }

  return 0;
}

