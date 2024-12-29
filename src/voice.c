/*******************************************************************************
** voice.c (synth voice)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "clock.h"
#include "cart.h"
#include "midicont.h"
#include "sweep.h"
#include "tuning.h"
#include "voice.h"

#define PI      3.14159265358979323846f
#define TWO_PI  6.28318530717958647693f

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
  if (v->osc_freq_mode[num] == 1)                                              \
  {                                                                            \
    v->osc_pitch_index[num] =                                                  \
      (TUNING_NOTE_C0 + v->osc_note_offset[num]) * TUNING_NUM_SEMITONE_STEPS;  \
  }                                                                            \
  else                                                                         \
  {                                                                            \
    v->osc_pitch_index[num] =                                                  \
      (v->base_note + v->osc_note_offset[num]) * TUNING_NUM_SEMITONE_STEPS;    \
  }                                                                            \
                                                                               \
  v->osc_pitch_index[num] += v->osc_detune_offset[num];                        \
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
  if (v->mod_wheel_routing & PATCH_MIDI_CONT_ROUTING_FLAG_##effect)            \
    combined_pos += v->mod_wheel_pos;                                          \
                                                                               \
  if (v->aftertouch_routing & PATCH_MIDI_CONT_ROUTING_FLAG_##effect)           \
    combined_pos += v->aftertouch_pos;                                         \
                                                                               \
  if (v->exp_pedal_routing & PATCH_MIDI_CONT_ROUTING_FLAG_##effect)            \
    combined_pos += v->exp_pedal_pos;                                          \
                                                                               \
  if (combined_pos < MIDI_CONT_UNI_WHEEL_LOWER_BOUND)                          \
    MIDI_CONT_UNI_WHEEL_LOWER_BOUND;                                           \
  else if (combined_pos > MIDI_CONT_UNI_WHEEL_UPPER_BOUND)                     \
    MIDI_CONT_UNI_WHEEL_UPPER_BOUND;

/* multiple table */
/* the values form the harmonic series! */
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

/* detune table */
static int  S_voice_detune_table[8] = 
            { ( 0  * TUNING_NUM_SEMITONE_STEPS) / 128, 
              (-12 * TUNING_NUM_SEMITONE_STEPS) / 128, 
              (-8  * TUNING_NUM_SEMITONE_STEPS) / 128, 
              (-4  * TUNING_NUM_SEMITONE_STEPS) / 128, 
              ( 0  * TUNING_NUM_SEMITONE_STEPS) / 128, 
              ( 4  * TUNING_NUM_SEMITONE_STEPS) / 128, 
              ( 8  * TUNING_NUM_SEMITONE_STEPS) / 128, 
              ( 12 * TUNING_NUM_SEMITONE_STEPS) / 128 
            };

/* pitch wheel range table */
static int  S_voice_pitch_wheel_max_table[12];

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

  voice* v;

  /* reset all voices */
  for (k = 0; k < BANK_NUM_VOICES; k++)
  {
    /* obtain voice pointer */
    v = &G_voice_bank[k];

    /* algorithm, sync */
    v->algorithm = 0;
    v->sync = 0;

    /* oscillators */
    for (m = 0; m < BANK_OSCILLATORS_PER_VOICE; m++)
    {
      v->osc_waveform[m] = 0;
      v->osc_phi[m] = 0;
      v->osc_freq_mode[m] = 0;

      v->osc_note_offset[m] = 0;

      v->osc_note_offset[m] += S_voice_multiple_table[0];
      v->osc_note_offset[m] -= S_voice_multiple_table[0];

      v->osc_detune_offset[m] = S_voice_detune_table[0];

      v->osc_routing[m] = 0x00;
    }

    /* currently playing note, pitch indices */
    v->base_note = TUNING_NOTE_BLANK;

    for (m = 0; m < BANK_OSCILLATORS_PER_VOICE; m++)
      v->osc_pitch_index[m] = 0;

    /* phases */
    for (m = 0; m < BANK_OSCILLATORS_PER_VOICE; m++)
      v->osc_phase[m] = 0;

    /* noise lfsr */
    v->noise_lfsr = 0x0001;

    /* envelope input levels */
    for (m = 0; m < BANK_ENVELOPES_PER_VOICE; m++)
      v->env_input[m] = VOICE_MAX_ATTENUATION_DB;

    /* vibrato */
    v->vibrato_base = 0;
    v->vibrato_extra = 0;

    /* pitch wheel */
    v->pitch_wheel_mode = 0;
    v->pitch_wheel_max = S_voice_pitch_wheel_max_table[0];

    /* pitch envelope, sweep */
    v->peg_input = 0;
    v->sweep_input = 0;

    /* midi controller routing */
    v->mod_wheel_routing = 0x00;
    v->aftertouch_routing = 0x00;
    v->exp_pedal_routing = 0x00;

    /* midi controller positions */
    v->mod_wheel_pos = MIDI_CONT_UNI_WHEEL_DEFAULT;
    v->aftertouch_pos = MIDI_CONT_UNI_WHEEL_DEFAULT;
    v->exp_pedal_pos = MIDI_CONT_UNI_WHEEL_DEFAULT;
    v->pitch_wheel_pos = MIDI_CONT_BI_WHEEL_DEFAULT;

    /* output level */
    v->level = 0;
  }

  return 0;
}

/*******************************************************************************
** voice_load_patch()
*******************************************************************************/
short int voice_load_patch( int voice_index, 
                            int cart_index, int patch_index)
{
  int m;

  voice* v;

  cart* c;
  patch* p;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* make sure that the cart & patch indices are valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(cart_index))
    return 1;

  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain cart & patch pointers */
  c = &G_cart_bank[cart_index];
  p = &(c->patches[patch_index]);

  /* obtain voice pointer */
  v = &G_voice_bank[voice_index];

  /* load patch parameters */
  if (PATCH_PARAM_IS_VALID_LOOKUP_BY_NAME(ALGORITHM))
    v->algorithm = p->values[PATCH_PARAM_ALGORITHM];
  else
    v->algorithm = 0;

  if (PATCH_PARAM_IS_VALID_LOOKUP_BY_NAME(OSC_SYNC))
    v->sync = p->values[PATCH_PARAM_OSC_SYNC];
  else
    v->sync = 0;

  for (m = 0; m < BANK_OSCILLATORS_PER_VOICE; m++)
  {
    if (PATCH_PARAM_IS_VALID_LOOKUP_OSC_PARAM(WAVEFORM, m + 1))
      v->osc_waveform[m] = p->values[PATCH_PARAM_COMPUTE_OSC_INDEX(WAVEFORM, m + 1)];
    else
      v->osc_waveform[m] = 0;

    if (PATCH_PARAM_IS_VALID_LOOKUP_OSC_PARAM(PHI, m + 1))
      v->osc_phi[m] = p->values[PATCH_PARAM_COMPUTE_OSC_INDEX(PHI, m + 1)];
    else
      v->osc_phi[m] = 0;

    if (PATCH_PARAM_IS_VALID_LOOKUP_OSC_PARAM(FREQ_MODE, m + 1))
      v->osc_freq_mode[m] = p->values[PATCH_PARAM_COMPUTE_OSC_INDEX(FREQ_MODE, m + 1)];
    else
      v->osc_freq_mode[m] = 0;

    if (v->osc_freq_mode[m] == 1)
    {
      v->osc_note_offset[m] = 0;

#if 0
      v->osc_note_offset[m] = TUNING_NOTE_C0;

      if ((p->osc_octave[m] >= PATCH_OSC_OCTAVE_LOWER_BOUND) && 
          (p->osc_octave[m] <= PATCH_OSC_OCTAVE_UPPER_BOUND))
      {
        v->osc_note_offset[m] += 12 * (p->osc_octave[m] - PATCH_OSC_OCTAVE_LOWER_BOUND);
      }
      else
        v->osc_note_offset[m] += 12 * (PATCH_OSC_OCTAVE_DEFAULT - PATCH_OSC_OCTAVE_LOWER_BOUND);

      if ((p->osc_note[m] >= PATCH_OSC_NOTE_LOWER_BOUND) && 
          (p->osc_note[m] <= PATCH_OSC_NOTE_UPPER_BOUND))
      {
        v->osc_note_offset[m] += p->osc_note[m] - PATCH_OSC_NOTE_LOWER_BOUND;
      }
      else
        v->osc_note_offset[m] += PATCH_OSC_NOTE_DEFAULT - PATCH_OSC_NOTE_LOWER_BOUND;
#endif
    }
    else
    {
      v->osc_note_offset[m] = 0;

      if (PATCH_PARAM_IS_VALID_LOOKUP_OSC_PARAM(MULTIPLE, m + 1))
        v->osc_note_offset[m] += S_voice_multiple_table[p->values[PATCH_PARAM_COMPUTE_OSC_INDEX(MULTIPLE, m + 1)]];
      else
        v->osc_note_offset[m] += S_voice_multiple_table[0];

      if (PATCH_PARAM_IS_VALID_LOOKUP_OSC_PARAM(DIVISOR, m + 1))
        v->osc_note_offset[m] -= S_voice_multiple_table[p->values[PATCH_PARAM_COMPUTE_OSC_INDEX(DIVISOR, m + 1)]];
      else
        v->osc_note_offset[m] -= S_voice_multiple_table[0];
    }

    if (PATCH_PARAM_IS_VALID_LOOKUP_OSC_PARAM(DETUNE, m + 1))
      v->osc_detune_offset[m] = S_voice_detune_table[p->values[PATCH_PARAM_COMPUTE_OSC_INDEX(DETUNE, m + 1)]];
    else
      v->osc_detune_offset[m] = S_voice_detune_table[0];

    /* determine oscillator notes and pitch indices */
    VOICE_COMPUTE_PITCH_INDEX(m)

    /* oscillator routing */
    v->osc_routing[m] = 0x00;
  }

  /* pitch wheel */
  if (PATCH_PARAM_IS_VALID_LOOKUP_BY_NAME(PITCH_WHEEL_MODE))
    v->pitch_wheel_mode = p->values[PATCH_PARAM_PITCH_WHEEL_MODE];
  else
    v->pitch_wheel_mode = 0;

  if (PATCH_PARAM_IS_VALID_LOOKUP_BY_NAME(PITCH_WHEEL_RANGE))
    v->pitch_wheel_max = S_voice_pitch_wheel_max_table[p->values[PATCH_PARAM_PITCH_WHEEL_RANGE]];
  else
    v->pitch_wheel_max = S_voice_pitch_wheel_max_table[0];

  /* midi controller routing */
  v->mod_wheel_routing = 0x00;
  v->aftertouch_routing = 0x00;
  v->exp_pedal_routing = 0x00;

  return 0;
}

/*******************************************************************************
** voice_note_on()
*******************************************************************************/
short int voice_note_on(int voice_index, int note)
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

  /* compute pitch indices & reset phases if necessary */
  for (m = 0; m < BANK_OSCILLATORS_PER_VOICE; m++)
  {
    VOICE_COMPUTE_PITCH_INDEX(m)

    if (v->sync == 1)
    {
      v->osc_phase[m] = 0;
      v->noise_lfsr = 0x0001;
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

  voice* v;

  int osc_level[BANK_OSCILLATORS_PER_VOICE];
  int osc_phase_mod[BANK_OSCILLATORS_PER_VOICE];

  short int combined_pos;

  int vibrato_adjustment;
  int pitch_env_adjustment;
  int pitch_wheel_adjustment;

  int adjusted_pitch_index;

  unsigned int masked_phase;

  int final_index;

  /* update all voices */
  for (k = 0; k < BANK_NUM_VOICES; k++)
  {
    v = &G_voice_bank[k];

    /* compute vibrato adjustment */
#if 0
    VOICE_COMPUTE_COMBINED_POSITION(VIBRATO)

    vibrato_adjustment = v->vibrato_base;
    vibrato_adjustment += 
      (v->vibrato_extra * combined_pos) / MIDI_CONT_UNI_WHEEL_DIVISOR;
#endif

    /* compute pitch envelope adjustment */
    pitch_env_adjustment = v->peg_input;

    /* compute pitch wheel adjustment */
    pitch_wheel_adjustment = 
      (v->pitch_wheel_max * v->pitch_wheel_pos) / MIDI_CONT_BI_WHEEL_RADIUS;

    if (v->pitch_wheel_mode == 1)
    {
      if (pitch_wheel_adjustment >= 0)
      {
        pitch_wheel_adjustment /= TUNING_NUM_SEMITONE_STEPS;
        pitch_wheel_adjustment *= TUNING_NUM_SEMITONE_STEPS;
      }
      else
      {
        pitch_wheel_adjustment *= -1;
        pitch_wheel_adjustment /= TUNING_NUM_SEMITONE_STEPS;
        pitch_wheel_adjustment *= -TUNING_NUM_SEMITONE_STEPS;
      }
    }

    /* update phases */
    for (m = 0; m < BANK_OSCILLATORS_PER_VOICE; m++)
    {
      /* determine adjusted pitch indices */
      adjusted_pitch_index = v->osc_pitch_index[m];

#if 0
      if (v->osc_routing[m] & PATCH_OSC_ROUTING_FLAG_VIBRATO)
        adjusted_pitch_index += vibrato_adjustment;

      if (v->osc_routing[m] & PATCH_OSC_ROUTING_FLAG_PITCH_ENV)
        adjusted_pitch_index += pitch_env_adjustment;

      if (v->osc_routing[m] & PATCH_OSC_ROUTING_FLAG_PITCH_WHEEL)
        adjusted_pitch_index += pitch_wheel_adjustment;
#endif

      if (v->osc_freq_mode[m] != 1)
        adjusted_pitch_index += v->sweep_input;

      if (adjusted_pitch_index < 0)
        adjusted_pitch_index = 0;
      else if (adjusted_pitch_index >= TUNING_NUM_INDICES)
        adjusted_pitch_index = TUNING_NUM_INDICES - 1;

      v->osc_phase[m] += 
        S_voice_wave_phase_increment_table[adjusted_pitch_index];

      if (v->osc_phase[m] > 0xFFFFFFF)
        v->osc_phase[m] &= 0xFFFFFFF;
    }

    /* update oscillator levels */
    for (m = 0; m < BANK_OSCILLATORS_PER_VOICE; m++)
    {
      /* determine masked phase */
      masked_phase = (v->osc_phase[m] >> 18) & 0x3FF;

      /* apply phase mod, if necessary */
      if (m == 1)
      {
        if ((v->algorithm == 0) || 
            (v->algorithm == 2) || 
            (v->algorithm == 3) || 
            (v->algorithm == 5) || 
            (v->algorithm == 6))
        {
          masked_phase = (masked_phase + osc_phase_mod[0]) & 0x3FF;
        }
      }
      else if (m == 2)
      {
        if ((v->algorithm == 0) || 
            (v->algorithm == 3))
        {
          masked_phase = (masked_phase + osc_phase_mod[1]) & 0x3FF;
        }
        else if (v->algorithm == 1)
          masked_phase = (masked_phase + osc_phase_mod[0] + osc_phase_mod[1]) & 0x3FF;
        else if ( (v->algorithm == 4) || 
                  (v->algorithm == 6))
        {
          masked_phase = (masked_phase + osc_phase_mod[0]) & 0x3FF;
        }
      }
      else if (m == 3)
      {
        if ((v->algorithm == 0) || 
            (v->algorithm == 1))
        {
          masked_phase = (masked_phase + osc_phase_mod[2]) & 0x3FF;
        }
        else if (v->algorithm == 2)
          masked_phase = (masked_phase + osc_phase_mod[1] + osc_phase_mod[2]) & 0x3FF;
        else if (v->algorithm == 4)
          masked_phase = (masked_phase + osc_phase_mod[1]) & 0x3FF;
        else if (v->algorithm == 6)
          masked_phase = (masked_phase + osc_phase_mod[0]) & 0x3FF;
      }

      /* determine waveform level (db) */
      if ((v->osc_waveform[m] == 0) || 
          (v->osc_waveform[m] == 2))
      {
        if (masked_phase < VOICE_WAVETABLE_SIZE_HALF)
          final_index = S_voice_wavetable_sine[masked_phase];
        else
          final_index = S_voice_wavetable_sine[masked_phase - VOICE_WAVETABLE_SIZE_HALF];
      }
      else if (v->osc_waveform[m] == 1)
      {
        if (masked_phase < VOICE_WAVETABLE_SIZE_HALF)
          final_index = S_voice_wavetable_sine[masked_phase];
        else
          final_index = VOICE_MAX_ATTENUATION_DB;
      }
      else if (v->osc_waveform[m] == 3)
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
      if (v->osc_waveform[m] == 0)
      {
        if (masked_phase < VOICE_WAVETABLE_SIZE_HALF)
          osc_level[m] = S_voice_db_to_linear_table[final_index];
        else
          osc_level[m] = -S_voice_db_to_linear_table[final_index];
      }
      else if ( (v->osc_waveform[m] == 1) || 
                (v->osc_waveform[m] == 2) || 
                (v->osc_waveform[m] == 3))
      {
        osc_level[m] = S_voice_db_to_linear_table[final_index];
      }
      else
        osc_level[m] = S_voice_db_to_linear_table[VOICE_MAX_ATTENUATION_DB];

      /* compute phase mod */
      osc_phase_mod[m] = (osc_level[m] >> 3) & 0x3FF;
    }

    /* determine voice level */
    v->level = 0;

    if ((v->algorithm == 0) || 
        (v->algorithm == 1) || 
        (v->algorithm == 2))
    {
      v->level += osc_level[3];
    }
    else if ( (v->algorithm == 3) || 
              (v->algorithm == 4))
    {
      v->level += osc_level[2] + osc_level[3];
    }
    else if ( (v->algorithm == 5) || 
              (v->algorithm == 6))
    {
      v->level += osc_level[1] + osc_level[2] + osc_level[3];
    }
    else
    {
      v->level += osc_level[0] + osc_level[1] + osc_level[2] + osc_level[3];
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

  /* pitch wheel range table */
  for (m = 0; m < 12; m++)
  {
    S_voice_pitch_wheel_max_table[m] = (m + 1) * TUNING_NUM_SEMITONE_STEPS;
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

