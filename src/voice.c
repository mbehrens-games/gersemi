/*******************************************************************************
** voice.c (synth voice)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "clock.h"
#include "midicont.h"
#include "patch.h"
#include "sweep.h"
#include "tuning.h"
#include "voice.h"

#define PI      3.14159265358979323846f
#define TWO_PI  6.28318530717958647693f

/* routing */
#define VOICE_ROUTING_CLEAR  0x00
#define VOICE_ROUTING_MASK   0xFF

#define VOICE_ROUTING_FLAG_MOD_WHEEL_VIBRATO    0x01
#define VOICE_ROUTING_FLAG_MOD_WHEEL_CHORUS     0x02
#define VOICE_ROUTING_FLAG_AFTERTOUCH_VIBRATO   0x04
#define VOICE_ROUTING_FLAG_AFTERTOUCH_CHORUS    0x08
#define VOICE_ROUTING_FLAG_EXP_PEDAL_VIBRATO    0x10
#define VOICE_ROUTING_FLAG_EXP_PEDAL_CHORUS     0x20

/* 12 bit db levels */
#define VOICE_DB_STEP_12_BIT 0.01171875f

#define VOICE_DB_TO_LINEAR_TABLE_SIZE 4096

#define VOICE_MAX_VOLUME_DB       0
#define VOICE_MAX_ATTENUATION_DB  (VOICE_DB_TO_LINEAR_TABLE_SIZE - 1)

#define VOICE_MAX_VOLUME_LINEAR       32767
#define VOICE_MAX_ATTENUATION_LINEAR  0

#define VOICE_WAVETABLE_SIZE_FULL     1024
#define VOICE_WAVETABLE_SIZE_HALF     (VOICE_WAVETABLE_SIZE_FULL / 2)
#define VOICE_WAVETABLE_SIZE_QUARTER  (VOICE_WAVETABLE_SIZE_FULL / 4)

#define VOICE_COMPUTE_PITCH_INDEX(num)                                         \
  v->osc_pitch_index[num] = v->base_note * TUNING_NUM_SEMITONE_STEPS;          \
                                                                               \
  v->osc_pitch_index[num] += v->osc_pitch_offset[num];                         \
  v->osc_pitch_index[num] +=                                                   \
    G_tuning_offset_table[(v->base_note - TUNING_NOTE_C0) % 12];               \
                                                                               \
  if (v->osc_pitch_index[num] < 0)                                             \
    v->osc_pitch_index[num] = 0;                                               \
  else if (v->osc_pitch_index[num] >= TUNING_NUM_INDICES)                      \
    v->osc_pitch_index[num] = TUNING_NUM_INDICES - 1;

#define VOICE_COMPUTE_COMBINED_POSITION(effect)                                \
  combined_pos = 0;                                                            \
                                                                               \
  if (v->routing & VOICE_ROUTING_FLAG_MOD_WHEEL_##effect)                      \
    combined_pos += v->mod_wheel_pos;                                          \
                                                                               \
  if (v->routing & VOICE_ROUTING_FLAG_AFTERTOUCH_##effect)                     \
    combined_pos += v->aftertouch_pos;                                         \
                                                                               \
  if (v->routing & VOICE_ROUTING_FLAG_EXP_PEDAL_##effect)                      \
    combined_pos += v->exp_pedal_pos;                                          \
                                                                               \
  if (combined_pos < MIDI_CONT_UNI_WHEEL_LOWER_BOUND)                          \
    MIDI_CONT_UNI_WHEEL_LOWER_BOUND;                                           \
  else if (combined_pos > MIDI_CONT_UNI_WHEEL_UPPER_BOUND)                     \
    MIDI_CONT_UNI_WHEEL_UPPER_BOUND;

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
  int m;
  int n;

  voice* v;

  /* reset all voices */
  for (k = 0; k < BANK_NUM_VOICES; k++)
  {
    /* obtain voice pointer */
    v = &G_voice_bank[k];

    /* algorithm, sync */
    v->algorithm = PATCH_ALGORITHM_DEFAULT;
    v->sync = PATCH_SYNC_DEFAULT;

    /* oscillators */
    for (m = 0; m < BANK_OSCILLATORS_PER_VOICE; m++)
    {
      v->osc_waveform[m] = PATCH_OSC_WAVEFORM_DEFAULT;
      v->osc_phi[m] = PATCH_OSC_PHI_DEFAULT;

      v->osc_pitch_offset[m] = 0;

      v->osc_pitch_offset[m] += S_voice_multiple_table[PATCH_OSC_MULTIPLE_DEFAULT - PATCH_OSC_MULTIPLE_LOWER_BOUND];
      v->osc_pitch_offset[m] -= S_voice_multiple_table[PATCH_OSC_DIVISOR_DEFAULT - PATCH_OSC_DIVISOR_LOWER_BOUND];
      v->osc_pitch_offset[m] += S_voice_detune_table[PATCH_OSC_DETUNE_DEFAULT - PATCH_OSC_DETUNE_LOWER_BOUND];
    }

    /* currently playing note, pitch indices */
    v->base_note = TUNING_NOTE_BLANK;

    for (m = 0; m < BANK_OSCILLATORS_PER_VOICE; m++)
      v->osc_pitch_index[m] = 0;

    /* phases */
    for (m = 0; m < BANK_OSCILLATORS_PER_VOICE; m++)
    {
      for (n = 0; n < VOICE_NUM_OSCILLATOR_SETS; n++)
        v->osc_phase[m][n] = 0;
    }

    /* noise lfsrs */
    for (m = 0; m < BANK_OSCILLATORS_PER_VOICE; m++)
      v->osc_lfsr[m] = 0x0001;

    /* envelope input levels */
    for (m = 0; m < BANK_ENVELOPES_PER_VOICE; m++)
      v->env_input[m] = VOICE_MAX_ATTENUATION_DB;

    /* vibrato, chorus */
    v->vibrato_base = 0;
    v->vibrato_extra = 0;

    v->chorus_base = 0;
    v->chorus_extra = 0;

    v->vibrato_adjustment = 0;
    v->chorus_adjustment = 0;

    /* pitch wheel, sweep */
    v->pitch_wheel_max = 0;

    v->sweep_input = 0;

    /* routing */
    v->routing = VOICE_ROUTING_CLEAR;

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
    v->algorithm = PATCH_ALGORITHM_DEFAULT;

  /* sync */
  if ((p->osc_sync >= PATCH_SYNC_LOWER_BOUND) && 
      (p->osc_sync <= PATCH_SYNC_UPPER_BOUND))
  {
    v->sync = p->osc_sync;
  }
  else
    v->sync = PATCH_SYNC_DEFAULT;

  for (m = 0; m < BANK_OSCILLATORS_PER_VOICE; m++)
  {
    /* oscillator waveform */
    if ((p->osc_waveform[m] >= PATCH_OSC_WAVEFORM_LOWER_BOUND) && 
        (p->osc_waveform[m] <= PATCH_OSC_WAVEFORM_UPPER_BOUND))
    {
      v->osc_waveform[m] = p->osc_waveform[m];
    }
    else
      v->osc_waveform[m] = PATCH_OSC_WAVEFORM_DEFAULT;

    /* oscillator phi */
    if ((p->osc_phi[m] >= PATCH_OSC_PHI_LOWER_BOUND) && 
        (p->osc_phi[m] <= PATCH_OSC_PHI_UPPER_BOUND))
    {
      v->osc_phi[m] = p->osc_phi[m];
    }
    else
      v->osc_phi[m] = PATCH_OSC_PHI_DEFAULT;

    /* oscillator pitch index */
    v->osc_pitch_offset[m] = 0;

    if ((p->osc_multiple[m] >= PATCH_OSC_MULTIPLE_LOWER_BOUND) && 
        (p->osc_multiple[m] <= PATCH_OSC_MULTIPLE_UPPER_BOUND))
    {
      v->osc_pitch_offset[m] += S_voice_multiple_table[p->osc_multiple[m] - PATCH_OSC_MULTIPLE_LOWER_BOUND];
    }
    else
      v->osc_pitch_offset[m] += S_voice_multiple_table[PATCH_OSC_MULTIPLE_DEFAULT - PATCH_OSC_MULTIPLE_LOWER_BOUND];

    if ((p->osc_divisor[m] >= PATCH_OSC_DIVISOR_LOWER_BOUND) && 
        (p->osc_divisor[m] <= PATCH_OSC_DIVISOR_UPPER_BOUND))
    {
      v->osc_pitch_offset[m] -= S_voice_multiple_table[p->osc_divisor[m] - PATCH_OSC_DIVISOR_LOWER_BOUND];
    }
    else
      v->osc_pitch_offset[m] -= S_voice_multiple_table[PATCH_OSC_DIVISOR_DEFAULT - PATCH_OSC_DIVISOR_LOWER_BOUND];

    if ((p->osc_detune[m] >= PATCH_OSC_DETUNE_LOWER_BOUND) && 
        (p->osc_detune[m] <= PATCH_OSC_DETUNE_UPPER_BOUND))
    {
      v->osc_pitch_offset[m] += S_voice_detune_table[p->osc_detune[m] - PATCH_OSC_DETUNE_LOWER_BOUND];
    }
    else
      v->osc_pitch_offset[m] += S_voice_detune_table[PATCH_OSC_DETUNE_DEFAULT - PATCH_OSC_DETUNE_LOWER_BOUND];

    /* for even periods only waveforms, apply an extra shift down by 1 octave */
    if ((p->osc_waveform[m] == PATCH_OSC_WAVEFORM_EPO_SINE)       || 
        (p->osc_waveform[m] == PATCH_OSC_WAVEFORM_EPO_FULL_RECT)  || 
        (p->osc_waveform[m] == PATCH_OSC_WAVEFORM_EPO_SQUARE)     || 
        (p->osc_waveform[m] == PATCH_OSC_WAVEFORM_EPO_SAW_DOWN)   || 
        (p->osc_waveform[m] == PATCH_OSC_WAVEFORM_EPO_SAW_UP)     || 
        (p->osc_waveform[m] == PATCH_OSC_WAVEFORM_EPO_TRIANGLE))
    {
      v->osc_pitch_offset[m] -= (1 * 12) * TUNING_NUM_SEMITONE_STEPS;
    }

    /* determine oscillator notes and pitch indices */
    VOICE_COMPUTE_PITCH_INDEX(m)

    /* routing */
    v->routing = VOICE_ROUTING_CLEAR;

    /* vibrato routing */
    if (p->mod_wheel_routing & PATCH_MIDI_CONT_ROUTING_FLAG_VIBRATO)
      v->routing |= VOICE_ROUTING_FLAG_MOD_WHEEL_VIBRATO;
    else
      v->routing &= ~VOICE_ROUTING_FLAG_MOD_WHEEL_VIBRATO;

    if (p->aftertouch_routing & PATCH_MIDI_CONT_ROUTING_FLAG_VIBRATO)
      v->routing |= VOICE_ROUTING_FLAG_AFTERTOUCH_VIBRATO;
    else
      v->routing &= ~VOICE_ROUTING_FLAG_AFTERTOUCH_VIBRATO;

    if (p->exp_pedal_routing & PATCH_MIDI_CONT_ROUTING_FLAG_VIBRATO)
      v->routing |= VOICE_ROUTING_FLAG_EXP_PEDAL_VIBRATO;
    else
      v->routing &= ~VOICE_ROUTING_FLAG_EXP_PEDAL_VIBRATO;

    /* chorus routing */
    if (p->mod_wheel_routing & PATCH_MIDI_CONT_ROUTING_FLAG_CHORUS)
      v->routing |= VOICE_ROUTING_FLAG_MOD_WHEEL_CHORUS;
    else
      v->routing &= ~VOICE_ROUTING_FLAG_MOD_WHEEL_CHORUS;

    if (p->aftertouch_routing & PATCH_MIDI_CONT_ROUTING_FLAG_CHORUS)
      v->routing |= VOICE_ROUTING_FLAG_AFTERTOUCH_CHORUS;
    else
      v->routing &= ~VOICE_ROUTING_FLAG_AFTERTOUCH_CHORUS;

    if (p->exp_pedal_routing & PATCH_MIDI_CONT_ROUTING_FLAG_CHORUS)
      v->routing |= VOICE_ROUTING_FLAG_EXP_PEDAL_CHORUS;
    else
      v->routing &= ~VOICE_ROUTING_FLAG_EXP_PEDAL_CHORUS;
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
  if (TUNING_NOTE_IS_NOT_VALID(note))
    return 0;

  /* set base note */
  v->base_note = note;

  /* determine notes & pitch indices */
  for (m = 0; m < BANK_OSCILLATORS_PER_VOICE; m++)
  {
    VOICE_COMPUTE_PITCH_INDEX(m)
  }

  return 0;
}

/*******************************************************************************
** voice_sync_to_key()
*******************************************************************************/
short int voice_sync_to_key(int voice_index)
{
  int m;
  int n;

  voice* v;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain voice pointer */
  v = &G_voice_bank[voice_index];

  /* reset phases if necessary */
  if (v->sync == PATCH_SYNC_ON)
  {
    for (m = 0; m < BANK_OSCILLATORS_PER_VOICE; m++)
    {
      for (n = 0; n < VOICE_NUM_OSCILLATOR_SETS; n++)
        v->osc_phase[m][n] = 0;

      v->osc_lfsr[m] = 0x0001;
    }
  }

  return 0;
}

/*******************************************************************************
** voice_update_all()
*******************************************************************************/
short int voice_update_all()
{
  int k;
  int m;
  int n;

  voice* v;

  int osc_level[BANK_OSCILLATORS_PER_VOICE][VOICE_NUM_OSCILLATOR_SETS];
  int osc_phase_mod[BANK_OSCILLATORS_PER_VOICE][VOICE_NUM_OSCILLATOR_SETS];

  short int combined_pos;

  int pitch_adjustment;

  int adjusted_pitch_index[VOICE_NUM_OSCILLATOR_SETS];

  unsigned int masked_phase;

  int final_index;

  /* update all voices */
  for (k = 0; k < BANK_NUM_VOICES; k++)
  {
    v = &G_voice_bank[k];

    /* compute vibrato & chorus adjustments */
    VOICE_COMPUTE_COMBINED_POSITION(VIBRATO)

    v->vibrato_adjustment = v->vibrato_base;
    v->vibrato_adjustment += 
      (v->vibrato_extra * combined_pos) / MIDI_CONT_UNI_WHEEL_DIVISOR;

    VOICE_COMPUTE_COMBINED_POSITION(CHORUS)

    v->chorus_adjustment = v->chorus_base;
    v->chorus_adjustment += 
      (v->chorus_extra * combined_pos) / MIDI_CONT_UNI_WHEEL_DIVISOR;

    /* update adjustments */
    pitch_adjustment = 0;

    pitch_adjustment += v->vibrato_adjustment;

#if 0
    pitch_adjustment += v->sweep_input;
    pitch_adjustment += v->pitch_wheel_pos;
#endif

    /* update phases */
    for (m = 0; m < BANK_OSCILLATORS_PER_VOICE; m++)
    {
      if ((v->osc_waveform[m] == PATCH_OSC_WAVEFORM_NOISE_SQUARE) || 
          (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_NOISE_SAW))
      {
        adjusted_pitch_index[0] = v->osc_pitch_index[m];
        adjusted_pitch_index[1] = v->osc_pitch_index[m];
      }
      else
      {
        adjusted_pitch_index[0] = v->osc_pitch_index[m] + pitch_adjustment + v->chorus_adjustment;
        adjusted_pitch_index[1] = v->osc_pitch_index[m] + pitch_adjustment - v->chorus_adjustment;
      }

      for (n = 0; n < VOICE_NUM_OSCILLATOR_SETS; n++)
      {
        if (adjusted_pitch_index[n] < 0)
          adjusted_pitch_index[n] = 0;
        else if (adjusted_pitch_index[n] >= TUNING_NUM_INDICES)
          adjusted_pitch_index[n] = TUNING_NUM_INDICES - 1;

        v->osc_phase[m][n] += 
          S_voice_wave_phase_increment_table[adjusted_pitch_index[n]];
      }

      if (v->osc_phase[m][0] > 0xFFFFFFF)
      {
        v->osc_phase[m][0] &= 0xFFFFFFF;

        if ((v->osc_waveform[m] == PATCH_OSC_WAVEFORM_NOISE_SQUARE) || 
            (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_NOISE_SAW))
        {
          /* update noise generator (nes) */
          /* 15-bit lfsr, taps on 1 and 2 */
          if ((v->osc_lfsr[m] & 0x0001) ^ ((v->osc_lfsr[m] & 0x0002) >> 1))
            v->osc_lfsr[m] = ((v->osc_lfsr[m] >> 1) & 0x3FFF) | 0x4000;
          else
            v->osc_lfsr[m] = (v->osc_lfsr[m] >> 1) & 0x3FFF;
        }
      }

      if (v->osc_phase[m][1] > 0xFFFFFFF)
        v->osc_phase[m][1] &= 0xFFFFFFF;
    }

    /* update oscillator levels */
    for (m = 0; m < BANK_OSCILLATORS_PER_VOICE; m++)
    {
      for (n = 0; n < VOICE_NUM_OSCILLATOR_SETS; n++)
      {
        /* determine masked phase */
        if ((v->osc_waveform[m] == PATCH_OSC_WAVEFORM_NOISE_SQUARE) || 
            (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_NOISE_SAW))
        {
          masked_phase = v->osc_lfsr[m] & 0x3FF;
        }
        else
          masked_phase = (v->osc_phase[m][n] >> 18) & 0x3FF;

        /* apply phase mod, if necessary */
        if (m == 1)
        {
          if (v->algorithm == PATCH_ALGORITHM_1_FM_2_FM_3)
            masked_phase = (masked_phase + osc_phase_mod[0][n]) & 0x3FF;
        }
        else if (m == 2)
        {
          if ((v->algorithm == PATCH_ALGORITHM_1_FM_2_FM_3)   || 
              (v->algorithm == PATCH_ALGORITHM_2_FM_3_ADD_1))
          {
            masked_phase = (masked_phase + osc_phase_mod[1][n]) & 0x3FF;
          }
          else if (v->algorithm == PATCH_ALGORITHM_1_ADD_2_FM_3)
            masked_phase = (masked_phase + osc_phase_mod[0][n] + osc_phase_mod[1][n]) & 0x3FF;
        }

        /* determine waveform level (db) */
        if ((v->osc_waveform[m] == PATCH_OSC_WAVEFORM_SINE) || 
            (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_FULL_RECT))
        {
          if (masked_phase < VOICE_WAVETABLE_SIZE_HALF)
            final_index = S_voice_wavetable_sine[masked_phase];
          else
            final_index = S_voice_wavetable_sine[masked_phase - VOICE_WAVETABLE_SIZE_HALF];
        }
        else if (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_HALF_RECT)
        {
          if (masked_phase < VOICE_WAVETABLE_SIZE_HALF)
            final_index = S_voice_wavetable_sine[masked_phase];
          else
            final_index = VOICE_MAX_ATTENUATION_DB;
        }
        else if (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_QUARTER_RECT)
        {
          if (masked_phase < VOICE_WAVETABLE_SIZE_QUARTER)
            final_index = S_voice_wavetable_sine[masked_phase];
          else if (masked_phase < VOICE_WAVETABLE_SIZE_HALF)
            final_index = VOICE_MAX_ATTENUATION_DB;
          else if (masked_phase < 3 * VOICE_WAVETABLE_SIZE_QUARTER)
            final_index = S_voice_wavetable_sine[masked_phase - VOICE_WAVETABLE_SIZE_HALF];
          else
            final_index = VOICE_MAX_ATTENUATION_DB;
        }
        else if ( (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_EPO_SINE) || 
                  (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_EPO_FULL_RECT))
        {
          if (masked_phase < VOICE_WAVETABLE_SIZE_QUARTER)
            final_index = S_voice_wavetable_sine[2 * masked_phase];
          else if (masked_phase < VOICE_WAVETABLE_SIZE_HALF)
            final_index = S_voice_wavetable_sine[2 * masked_phase - VOICE_WAVETABLE_SIZE_HALF];
          else
            final_index = VOICE_MAX_ATTENUATION_DB;
        }
        else if ( (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_SQUARE)  || 
                  (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_NOISE_SQUARE))
        {
          final_index = VOICE_MAX_VOLUME_DB;
        }
        else if ( (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_SAW_DOWN)  || 
                  (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_SAW_UP)    || 
                  (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_NOISE_SAW))
        {
          final_index = S_voice_wavetable_saw[masked_phase];
        }
        else if (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_TRIANGLE)
        {
          if (masked_phase < VOICE_WAVETABLE_SIZE_HALF)
            final_index = S_voice_wavetable_tri[masked_phase];
          else
            final_index = S_voice_wavetable_tri[masked_phase - VOICE_WAVETABLE_SIZE_HALF];
        }
        else if (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_EPO_SQUARE)
        {
          if (masked_phase < VOICE_WAVETABLE_SIZE_HALF)
            final_index = VOICE_MAX_VOLUME_DB;
          else
            final_index = VOICE_MAX_ATTENUATION_DB;
        }
        else if ( (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_EPO_SAW_DOWN) || 
                  (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_EPO_SAW_UP))
        {
          if (masked_phase < VOICE_WAVETABLE_SIZE_HALF)
            final_index = S_voice_wavetable_saw[2 * masked_phase];
          else
            final_index = VOICE_MAX_ATTENUATION_DB;
        }
        else if (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_EPO_TRIANGLE)
        {
          if (masked_phase < VOICE_WAVETABLE_SIZE_QUARTER)
            final_index = S_voice_wavetable_tri[2 * masked_phase];
          else if (masked_phase < VOICE_WAVETABLE_SIZE_HALF)
            final_index = S_voice_wavetable_tri[2 * masked_phase - VOICE_WAVETABLE_SIZE_HALF];
          else
            final_index = VOICE_MAX_ATTENUATION_DB;
        }
        else
          final_index = VOICE_MAX_ATTENUATION_DB;

        /* apply envelope */
        final_index += v->env_input[m];

        /* add an extra 256 to the envelope, which is the same  */
        /* as multiplying by 1/2. this is because we are adding */
        /* up 2 sets of oscillators for the chorus effect!      */
        final_index += 256;

        if (final_index < 0)
          final_index = 0;
        else if (final_index > VOICE_MAX_ATTENUATION_DB)
          final_index = VOICE_MAX_ATTENUATION_DB;

        /* determine waveform level (linear) */
        if ((v->osc_waveform[m] == PATCH_OSC_WAVEFORM_SINE)          || 
            (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_HALF_RECT)     || 
            (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_EPO_SINE)      || 
            (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_SQUARE)        || 
            (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_SAW_DOWN)      || 
            (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_TRIANGLE)      || 
            (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_NOISE_SQUARE)  || 
            (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_NOISE_SAW))
        {
          if (masked_phase < VOICE_WAVETABLE_SIZE_HALF)
            osc_level[m][n] = S_voice_db_to_linear_table[final_index];
          else
            osc_level[m][n] = -S_voice_db_to_linear_table[final_index];
        }
        else if ( (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_FULL_RECT)     || 
                  (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_QUARTER_RECT)  || 
                  (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_EPO_FULL_RECT))
        {
          osc_level[m][n] = S_voice_db_to_linear_table[final_index];
        }
        else if (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_SAW_UP)
        {
          if (masked_phase < VOICE_WAVETABLE_SIZE_HALF)
            osc_level[m][n] = -S_voice_db_to_linear_table[final_index];
          else
            osc_level[m][n] = S_voice_db_to_linear_table[final_index];
        }
        else if ( (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_EPO_SQUARE)    || 
                  (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_EPO_SAW_DOWN)  || 
                  (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_EPO_TRIANGLE))
        {
          if (masked_phase < VOICE_WAVETABLE_SIZE_QUARTER)
            osc_level[m][n] = S_voice_db_to_linear_table[final_index];
          else
            osc_level[m][n] = -S_voice_db_to_linear_table[final_index];
        }
        else if (v->osc_waveform[m] == PATCH_OSC_WAVEFORM_EPO_SAW_UP)
        {
          if (masked_phase < VOICE_WAVETABLE_SIZE_QUARTER)
            osc_level[m][n] = -S_voice_db_to_linear_table[final_index];
          else
            osc_level[m][n] = S_voice_db_to_linear_table[final_index];
        }
        else
          osc_level[m][n] = S_voice_db_to_linear_table[VOICE_MAX_ATTENUATION_DB];

        /* compute phase mod */
        osc_phase_mod[m][n] = (osc_level[m][n] >> 3) & 0x3FF;
      }
    }

    /* determine voice level */
    v->level = 0;

    if ((v->algorithm == PATCH_ALGORITHM_1_FM_2_FM_3) || 
        (v->algorithm == PATCH_ALGORITHM_1_ADD_2_FM_3))
    {
      for (n = 0; n < VOICE_NUM_OSCILLATOR_SETS; n++)
        v->level += osc_level[2][n];
    }
    else if (v->algorithm == PATCH_ALGORITHM_2_FM_3_ADD_1)
    {
      for (n = 0; n < VOICE_NUM_OSCILLATOR_SETS; n++)
        v->level += osc_level[0][n] + osc_level[2][n];
    }
    else
    {
      for (n = 0; n < VOICE_NUM_OSCILLATOR_SETS; n++)
        v->level += osc_level[0][n] + osc_level[1][n] + osc_level[2][n];
    }
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

