/*******************************************************************************
** voice.c (synth voice)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "clock.h"
#include "cart.h"
#include "midicont.h"
#include "tempo.h"
#include "tuning.h"
#include "voice.h"

#define PI      3.14159265358979323846f
#define TWO_PI  6.28318530717958647693f

enum
{
  VOICE_ENV_STAGE_ATTACK = 1, 
  VOICE_ENV_STAGE_DECAY, 
  VOICE_ENV_STAGE_SUSTAIN, 
  VOICE_ENV_STAGE_RELEASE 
};

/* 12 bit db levels */
#define VOICE_DB_STEP_12_BIT 0.01171875f

#define VOICE_DB_TO_LINEAR_TABLE_SIZE 4096

#define VOICE_MAX_VOLUME_DB       0
#define VOICE_MAX_ATTENUATION_DB  (VOICE_DB_TO_LINEAR_TABLE_SIZE - 1)

#define VOICE_MAX_VOLUME_LINEAR       32767
#define VOICE_MAX_ATTENUATION_LINEAR  0

/* oscillators */
#define VOICE_WAVETABLE_SIZE_FULL     1024
#define VOICE_WAVETABLE_SIZE_HALF     (VOICE_WAVETABLE_SIZE_FULL / 2)
#define VOICE_WAVETABLE_SIZE_QUARTER  (VOICE_WAVETABLE_SIZE_FULL / 4)

/* envelopes */
#define VOICE_ENV_NUM_OCTAVES 13

#define VOICE_ENV_NUM_RATES (VOICE_ENV_NUM_OCTAVES * 12)

/* lfo */
#define VOICE_LFO_WAVE_AMPLITUDE 512

#define VOICE_LFO_WAVE_PERIOD_FULL    256
#define VOICE_LFO_WAVE_PERIOD_HALF    (VOICE_LFO_WAVE_PERIOD_FULL / 2)
#define VOICE_LFO_WAVE_PERIOD_QUARTER (VOICE_LFO_WAVE_PERIOD_FULL / 4)

/* multiple table */
/* the values form the harmonic series! */
static int  S_voice_multiple_table[PATCH_NUM_OSC_MULTIPLE_VALS] = 
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
static int  S_voice_detune_table[PATCH_NUM_OSC_DETUNE_VALS] = 
            { (-12 * TUNING_NUM_SEMITONE_STEPS) / 128, 
              (-8  * TUNING_NUM_SEMITONE_STEPS) / 128, 
              (-4  * TUNING_NUM_SEMITONE_STEPS) / 128, 
              ( 0  * TUNING_NUM_SEMITONE_STEPS) / 128, 
              ( 4  * TUNING_NUM_SEMITONE_STEPS) / 128, 
              ( 8  * TUNING_NUM_SEMITONE_STEPS) / 128, 
              ( 12 * TUNING_NUM_SEMITONE_STEPS) / 128 
            };

/* sensitivity tables */
static short int S_voice_vibrato_max_table[PATCH_NUM_SENSITIVITY_VALS] = 
                  { (  2 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    (  4 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    (  6 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    (  8 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    ( 12 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    ( 16 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    ( 20 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    ( 24 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    ( 28 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    ( 32 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    ( 48 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    ( 64 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    (128 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    (256 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    (512 * TUNING_NUM_SEMITONE_STEPS) / 128, 
                    (896 * TUNING_NUM_SEMITONE_STEPS) / 128 
                  };

static short int S_voice_tremolo_max_table[PATCH_NUM_SENSITIVITY_VALS];
static short int S_voice_boost_max_table[PATCH_NUM_SENSITIVITY_VALS];

/* oscillator phase increment table */
static unsigned int S_voice_wave_phase_increment_table[TUNING_NUM_INDICES];

/* db to linear table */
static short int S_voice_db_to_linear_table[VOICE_DB_TO_LINEAR_TABLE_SIZE];

/* wavetables */
static short int S_voice_wavetable_sine[VOICE_WAVETABLE_SIZE_HALF];

/* envelope phase increment tables */
static unsigned int S_voice_env_attack_increment_table[VOICE_ENV_NUM_RATES];
static unsigned int S_voice_env_decay_increment_table[VOICE_ENV_NUM_RATES];

/* envelope level, time tables */
static short int S_voice_env_max_level_table[PATCH_NUM_ENV_LEVEL_VALS];
static short int S_voice_env_hold_level_table[PATCH_NUM_ENV_LEVEL_VALS];
static short int S_voice_env_time_table[PATCH_NUM_ENV_TIME_VALS];

/* keyscaling tables */
static short int S_voice_env_rate_keyscale_table[TUNING_NUM_PLAYABLE_NOTES];
static short int S_voice_env_level_keyscale_table[TUNING_NUM_PLAYABLE_NOTES];

/* lfo wavetables */
static short int S_voice_lfo_bi_wavetable_tri[VOICE_LFO_WAVE_PERIOD_FULL];
static short int S_voice_lfo_bi_wavetable_saw[VOICE_LFO_WAVE_PERIOD_FULL];

static short int S_voice_lfo_uni_wavetable_tri[VOICE_LFO_WAVE_PERIOD_FULL];
static short int S_voice_lfo_uni_wavetable_saw[VOICE_LFO_WAVE_PERIOD_FULL];

/* delay table */
static int S_voice_lfo_delay_period_table[TEMPO_NUM_VALUES];

/* phase increment table */
static unsigned int S_voice_lfo_phase_increment_table[TEMPO_NUM_VALUES];

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

    /* cart & patch indices */
    v->cart_index = 0;
    v->patch_index = 0;

    /* base note */
    v->base_note = TUNING_NOTE_BLANK;

    /* feedback */
    v->feedin[0] = 0;
    v->feedin[1] = 0;

    /* oscillators */
    for (m = 0; m < VOICE_NUM_OSCS; m++)
    {
      v->osc_pitch_index[m] = 0;
      v->osc_phase[m] = 0;
    }

    /* envelopes */
    for (m = 0; m < VOICE_NUM_OSCS; m++)
    {
      v->env_stage[m] = VOICE_ENV_STAGE_RELEASE;

      v->env_increment[m] = 0;
      v->env_phase[m] = 0;

      v->env_attenuation[m] = VOICE_MAX_ATTENUATION_DB;

      v->env_ks_rate_adjustment[m] = 0;
      v->env_ks_level_adjustment[m] = 0;

      v->env_volume_adjustment[m] = 0;
      v->env_amplitude_adjustment[m] = 0;
      v->env_velocity_adjustment[m] = 0;
    }

    /* lfo */
    v->tempo = TEMPO_DEFAULT;

    v->lfo_delay_cycles = 0;

    v->lfo_phase = 0;
    v->lfo_increment = S_voice_lfo_phase_increment_table[v->tempo] * 0;

    /* midi controller positions */
    v->vibrato_wheel_pos = MIDI_CONT_UNI_WHEEL_DEFAULT;
    v->tremolo_wheel_pos = MIDI_CONT_UNI_WHEEL_DEFAULT;
    v->boost_wheel_pos = MIDI_CONT_UNI_WHEEL_DEFAULT;

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
  voice* v;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* make sure that the cart & patch indices are valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(cart_index))
    return 1;

  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain voice pointer */
  v = &G_voice_bank[voice_index];

  /* set cart and patch indices */
  v->cart_index = cart_index;
  v->patch_index = patch_index;

  return 0;
}

/*******************************************************************************
** voice_note_on()
*******************************************************************************/
short int voice_note_on(int voice_index, int note)
{
  int m;

  voice* v;

  cart* c;
  patch* p;

  short int shifted_note;
  short int time;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* if note is out of range, ignore */
  if (TUNING_NOTE_IS_NOT_VALID(note))
    return 0;

  /* obtain voice pointer */
  v = &G_voice_bank[voice_index];

  /* obtain cart & patch pointers */
  c = &G_cart_bank[v->cart_index];
  p = &(c->patches[v->patch_index]);

  /* set base note */
  v->base_note = note;

  /* lfo */
  v->lfo_delay_cycles = S_voice_lfo_delay_period_table[v->tempo];
  v->lfo_delay_cycles *= p->values[PATCH_PARAM_LFO_DELAY];

  /* compute phase increment */
  v->lfo_increment = S_voice_lfo_phase_increment_table[v->tempo];
  v->lfo_increment *= p->values[PATCH_PARAM_LFO_SPEED];

  /* reset phase if necessary */
  if (p->values[PATCH_PARAM_LFO_SYNC] == 1)
    v->lfo_phase = 0;

  /* oscillators & envelopes */
  for (m = 0; m < VOICE_NUM_OSCS; m++)
  {
    /* shift note based on multiple/divisor or fixed frequency mode */
    if (p->values[PATCH_PARAM_OSC_1_FREQ_MODE + m * PATCH_PARAM_OSC_SHIFT] == PATCH_OSC_FREQ_MODE_VAL_FIXED)
    {
      shifted_note = TUNING_NOTE_C0;
      shifted_note += 12 * p->values[PATCH_PARAM_OSC_1_OCTAVE + m * PATCH_PARAM_OSC_SHIFT];
      shifted_note += p->values[PATCH_PARAM_OSC_1_NOTE + m * PATCH_PARAM_OSC_SHIFT];
    }
    else
    {
      shifted_note = note;
      shifted_note += S_voice_multiple_table[p->values[PATCH_PARAM_OSC_1_MULTIPLE + m * PATCH_PARAM_OSC_SHIFT]];
      shifted_note -= S_voice_multiple_table[p->values[PATCH_PARAM_OSC_1_DIVISOR + m * PATCH_PARAM_OSC_SHIFT]];
    }

    if (shifted_note < TUNING_NOTE_C0)
      shifted_note = TUNING_NOTE_C0;
    else if (shifted_note > TUNING_NOTE_B9)
      shifted_note = TUNING_NOTE_B9;

    /* compute keyscaling adjustments based on shifted note */
    if (p->values[PATCH_PARAM_LEGACY_KEYSCALE] == PATCH_LEGACY_KEYSCALE_VAL_KEY)
    {
      v->env_ks_rate_adjustment[m] = S_voice_env_rate_keyscale_table[note - TUNING_NOTE_A0];
      v->env_ks_level_adjustment[m] = S_voice_env_level_keyscale_table[note - TUNING_NOTE_A0];
    }
    else
    {
      v->env_ks_rate_adjustment[m] = S_voice_env_rate_keyscale_table[shifted_note - TUNING_NOTE_A0];
      v->env_ks_level_adjustment[m] = S_voice_env_level_keyscale_table[shifted_note - TUNING_NOTE_A0];
    }

    if (p->values[PATCH_PARAM_ENV_1_RATE_KS + m * PATCH_PARAM_ENV_SHIFT] < 3)
    {
      v->env_ks_rate_adjustment[m] = 
        v->env_ks_rate_adjustment[m] >> (3 - p->values[PATCH_PARAM_ENV_1_RATE_KS + m * PATCH_PARAM_ENV_SHIFT]);
    }

    if (p->values[PATCH_PARAM_ENV_1_LEVEL_KS + m * PATCH_PARAM_ENV_SHIFT] < 3)
    {
      v->env_ks_level_adjustment[m] = 
        v->env_ks_level_adjustment[m] >> (3 - p->values[PATCH_PARAM_ENV_1_LEVEL_KS + m * PATCH_PARAM_ENV_SHIFT]);
    }

    /* set the amplitude adjustment */
    v->env_amplitude_adjustment[m] = 
      S_voice_env_max_level_table[p->values[PATCH_PARAM_ENV_1_MAX_LEVEL + m * PATCH_PARAM_ENV_SHIFT]];

    /* set to attack stage */
    v->env_stage[m] = VOICE_ENV_STAGE_ATTACK;

    time = S_voice_env_time_table[p->values[PATCH_PARAM_ENV_1_ATTACK + m * PATCH_PARAM_ENV_SHIFT]];
    time += v->env_ks_rate_adjustment[m];

    if (time < 0)
      time = 0;
    else if (time > VOICE_ENV_NUM_RATES - 1)
      time = VOICE_ENV_NUM_RATES - 1;

    v->env_increment[m] = S_voice_env_attack_increment_table[time];
    v->env_phase[m] = 0;

    /* compute pitch indices */
    v->osc_pitch_index[m] = shifted_note * TUNING_NUM_SEMITONE_STEPS;
    v->osc_pitch_index[m] += G_tuning_offset_table[(shifted_note - TUNING_NOTE_C0) % 12];

    v->osc_pitch_index[m] += 
      S_voice_detune_table[p->values[PATCH_PARAM_OSC_1_DETUNE + m * PATCH_PARAM_OSC_SHIFT]];

    /* bound pitch indices */
    if (v->osc_pitch_index[m] < 0)
      v->osc_pitch_index[m] = 0;
    else if (v->osc_pitch_index[m] >= TUNING_NUM_INDICES)
      v->osc_pitch_index[m] = TUNING_NUM_INDICES - 1;

    /* reset phases if necessary */
    if (p->values[PATCH_PARAM_OSC_SYNC] == 1)
      v->osc_phase[m] = 0;
  }

  return 0;
}

/*******************************************************************************
** voice_note_off()
*******************************************************************************/
short int voice_note_off(int voice_index)
{
  int m;

  voice* v;

  cart* c;
  patch* p;

  short int time;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain voice pointer */
  v = &G_voice_bank[voice_index];

  /* obtain cart & patch pointers */
  c = &G_cart_bank[v->cart_index];
  p = &(c->patches[v->patch_index]);

  /* oscillators & envelopes */
  for (m = 0; m < VOICE_NUM_OSCS; m++)
  {
    /* if this envelope is already note_offd, continue */
    if (v->env_stage[m] == VOICE_ENV_STAGE_RELEASE)
      continue;

    /* set to release stage */
    v->env_stage[m] = VOICE_ENV_STAGE_RELEASE;

    time = S_voice_env_time_table[2 * p->values[PATCH_PARAM_ENV_1_RELEASE + m * PATCH_PARAM_ENV_SHIFT]];
    time += v->env_ks_rate_adjustment[m];

    if (time < 0)
      time = 0;
    else if (time > VOICE_ENV_NUM_RATES - 1)
      time = VOICE_ENV_NUM_RATES - 1;

    v->env_increment[m] = S_voice_env_decay_increment_table[time];
    v->env_phase[m] = 0;
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

  cart* c;
  patch* p;

  int lfo_bi_level;
  int lfo_uni_level;

  int wheel_base;
  int wheel_extra;

  int vibrato_adjustment;
  int carrier_adjustment;
  int modulator_adjustment;

  int env_level[VOICE_NUM_OSCS];

  short int time;
  short int periods;

  int osc_level[VOICE_NUM_OSCS];
  int osc_phase_mod[VOICE_NUM_OSCS];

  int adjusted_pitch_index;
  int fb_phase_mod;

  unsigned int masked_phase;

  int final_index;

  /* update all voices */
  for (k = 0; k < BANK_NUM_VOICES; k++)
  {
    v = &G_voice_bank[k];

    /* obtain cart & patch pointers */
    c = &G_cart_bank[v->cart_index];
    p = &(c->patches[v->patch_index]);

    /* compute feedback */
    fb_phase_mod = (v->feedin[0] + v->feedin[1]) >> 1;

    /* update lfo */
    v->lfo_phase += v->lfo_increment;

    /* wraparound phase register (28 bits) */
    if (v->lfo_phase > 0xFFFFFFF)
      v->lfo_phase &= 0xFFFFFFF;

    /* update delay cycles if necessary */
    if (v->lfo_delay_cycles > 0)
    {
      v->lfo_delay_cycles -= 1;

      if ((v->lfo_delay_cycles == 0) && (p->values[PATCH_PARAM_LFO_SYNC] == 1))
        v->lfo_phase = 0;
    }

    /* determine base wave indices */
    masked_phase = ((v->lfo_phase >> 20) & 0xFF);

    /* determine lfo wave level */
    lfo_bi_level = 0;
    lfo_uni_level = 0;

    if (p->values[PATCH_PARAM_LFO_WAVEFORM] == PATCH_LFO_WAVEFORM_VAL_TRIANGLE)
    {
      lfo_bi_level = S_voice_lfo_bi_wavetable_tri[masked_phase];
      lfo_uni_level = S_voice_lfo_uni_wavetable_tri[masked_phase];
    }
    else if (p->values[PATCH_PARAM_LFO_WAVEFORM] == PATCH_LFO_WAVEFORM_VAL_SQUARE)
    {
      if (masked_phase < VOICE_LFO_WAVE_PERIOD_HALF)
      {
        lfo_bi_level = VOICE_LFO_WAVE_AMPLITUDE;
        lfo_uni_level = 0;
      }
      else
      {
        lfo_bi_level = -VOICE_LFO_WAVE_AMPLITUDE;
        lfo_uni_level = VOICE_LFO_WAVE_AMPLITUDE;
      }
    }
    else if (p->values[PATCH_PARAM_LFO_WAVEFORM] == PATCH_LFO_WAVEFORM_VAL_SAW_UP)
    {
      lfo_bi_level = VOICE_LFO_WAVE_AMPLITUDE - S_voice_lfo_bi_wavetable_saw[masked_phase];
      lfo_uni_level = VOICE_LFO_WAVE_AMPLITUDE - S_voice_lfo_uni_wavetable_saw[masked_phase];
    }
    else if (p->values[PATCH_PARAM_LFO_WAVEFORM] == PATCH_LFO_WAVEFORM_VAL_SAW_DOWN)
    {
      lfo_bi_level = S_voice_lfo_bi_wavetable_saw[masked_phase];
      lfo_uni_level = S_voice_lfo_uni_wavetable_saw[masked_phase];
    }

    /* make sure the levels are zeroed out during a delay */
    if (v->lfo_delay_cycles > 0)
    {
      lfo_bi_level = 0;
      lfo_uni_level = 0;
    }

    /* determine vibrato adjustment */
    if (p->values[PATCH_PARAM_VIBRATO_POLARITY] == PATCH_POLARITY_VAL_UNI)
      wheel_extra = lfo_uni_level;
    else
      wheel_extra = lfo_bi_level;

    wheel_extra *= S_voice_vibrato_max_table[p->values[PATCH_PARAM_VIBRATO_SENSITIVITY]];
    wheel_extra /= VOICE_LFO_WAVE_AMPLITUDE;

    wheel_base = wheel_extra;
    wheel_base *= p->values[PATCH_PARAM_VIBRATO_DEPTH];
    wheel_base /= G_patch_param_bounds[PATCH_PARAM_VIBRATO_DEPTH];

    wheel_extra -= wheel_base;

    vibrato_adjustment = wheel_base;
    vibrato_adjustment += 
      (wheel_extra * v->vibrato_wheel_pos) / MIDI_CONT_UNI_WHEEL_DIVISOR;

    /* determine tremolo adjustment */
    wheel_extra = lfo_uni_level;
    wheel_extra *= S_voice_tremolo_max_table[p->values[PATCH_PARAM_TREMOLO_SENSITIVITY]];
    wheel_extra /= VOICE_LFO_WAVE_AMPLITUDE;

    wheel_base = wheel_extra;
    wheel_base *= p->values[PATCH_PARAM_TREMOLO_DEPTH];
    wheel_base /= G_patch_param_bounds[PATCH_PARAM_TREMOLO_DEPTH];

    wheel_extra -= wheel_base;

    if (p->values[PATCH_PARAM_TREMOLO_MODE] == PATCH_AM_MODE_VAL_MODULATORS)
    {
      carrier_adjustment = 0;

      modulator_adjustment = wheel_base;
      modulator_adjustment += 
        (wheel_extra * v->tremolo_wheel_pos) / MIDI_CONT_UNI_WHEEL_DIVISOR;
    }
    else
    {
      carrier_adjustment = wheel_base;
      carrier_adjustment += 
        (wheel_extra * v->tremolo_wheel_pos) / MIDI_CONT_UNI_WHEEL_DIVISOR;

      modulator_adjustment = 0;
    }

    /* add on boost adjustment */
    wheel_base = 0;
    wheel_extra = S_voice_boost_max_table[p->values[PATCH_PARAM_BOOST_SENSITIVITY]];

    if (p->values[PATCH_PARAM_BOOST_MODE] == PATCH_AM_MODE_VAL_MODULATORS)
    {
      modulator_adjustment -= 
        (wheel_extra * v->boost_wheel_pos) / MIDI_CONT_UNI_WHEEL_DIVISOR;
    }
    else
    {
      carrier_adjustment -= 
        (wheel_extra * v->boost_wheel_pos) / MIDI_CONT_UNI_WHEEL_DIVISOR;
    }

    /* update envelopes */
    for (m = 0; m < VOICE_NUM_OSCS; m++)
    {
      /* update phase */
      v->env_phase[m] += v->env_increment[m];

      /* check if a period was completed */
      if (v->env_phase[m] > 0xFFFFFFF)
      {
        periods = (v->env_phase[m] >> 28) & 0x0F;

        v->env_phase[m] &= 0xFFFFFFF;
      }
      else
        periods = 0;

      /* if a period has elapsed, update the envelope */
      while (periods > 0)
      {
        periods -= 1;

        if (v->env_stage[m] == VOICE_ENV_STAGE_ATTACK)
          v->env_attenuation[m] = (127 * v->env_attenuation[m]) / 128;
        else
          v->env_attenuation[m] += 1;

        /* bound attenuation */
        if (v->env_attenuation[m] < 0)
          v->env_attenuation[m] = 0;
        else if (v->env_attenuation[m] > VOICE_MAX_ATTENUATION_DB)
          v->env_attenuation[m] = VOICE_MAX_ATTENUATION_DB;

        /* change stage if necessary */
        if ((v->env_stage[m] == VOICE_ENV_STAGE_ATTACK) && 
            (v->env_attenuation[m] <= 0))
        {
          v->env_stage[m] = VOICE_ENV_STAGE_DECAY;

          time = S_voice_env_time_table[p->values[PATCH_PARAM_ENV_1_DECAY + m * PATCH_PARAM_ENV_SHIFT]];
          time += v->env_ks_rate_adjustment[m];

          if (time < 0)
            time = 0;
          else if (time > VOICE_ENV_NUM_RATES - 1)
            time = VOICE_ENV_NUM_RATES - 1;

          v->env_increment[m] = S_voice_env_decay_increment_table[time];
          v->env_phase[m] = 0;
        }
        else if ( (v->env_stage[m] == VOICE_ENV_STAGE_DECAY) && 
                  (v->env_attenuation[m] >= S_voice_env_hold_level_table[p->values[PATCH_PARAM_ENV_1_HOLD_LEVEL + m * PATCH_PARAM_ENV_SHIFT]]))
        {
          v->env_stage[m] = VOICE_ENV_STAGE_SUSTAIN;

          time = S_voice_env_time_table[p->values[PATCH_PARAM_ENV_1_SUSTAIN + m * PATCH_PARAM_ENV_SHIFT]];
          time += v->env_ks_rate_adjustment[m];

          if (time < 0)
            time = 0;
          else if (time > VOICE_ENV_NUM_RATES - 1)
            time = VOICE_ENV_NUM_RATES - 1;

          v->env_increment[m] = S_voice_env_decay_increment_table[time];
          v->env_phase[m] = 0;
        }
        else if ( (v->env_stage[m] == VOICE_ENV_STAGE_SUSTAIN) && 
                  (v->env_attenuation[m] >= VOICE_MAX_ATTENUATION_DB))
        {
          v->env_stage[m] = VOICE_ENV_STAGE_RELEASE;

          time = S_voice_env_time_table[2 * p->values[PATCH_PARAM_ENV_1_RELEASE + m * PATCH_PARAM_ENV_SHIFT]];
          time += v->env_ks_rate_adjustment[m];

          if (time < 0)
            time = 0;
          else if (time > VOICE_ENV_NUM_RATES - 1)
            time = VOICE_ENV_NUM_RATES - 1;

          v->env_increment[m] = S_voice_env_decay_increment_table[time];
          v->env_phase[m] = 0;
        }
      }

      /* update level */
      env_level[m] = v->env_attenuation[m];

      if (p->values[PATCH_PARAM_ALGORITHM] == PATCH_ALGORITHM_VAL_1C_CHAIN)
      {
        if (m == 1)
          env_level[m] += modulator_adjustment;
        else if (m == 2)
          env_level[m] += carrier_adjustment;
      }
      else if (p->values[PATCH_PARAM_ALGORITHM] == PATCH_ALGORITHM_VAL_1C_THE_V)
      {
        if ((m == 0) || (m == 1))
          env_level[m] += modulator_adjustment;
        else if (m == 2)
          env_level[m] += carrier_adjustment;
      }
      else if ( (p->values[PATCH_PARAM_ALGORITHM] == PATCH_ALGORITHM_VAL_2C_1_TO_1) || 
                (p->values[PATCH_PARAM_ALGORITHM] == PATCH_ALGORITHM_VAL_2C_1_TO_2))
      {
        if (m == 0)
          env_level[m] += modulator_adjustment;
        else if ((m == 1) || (m == 2))
          env_level[m] += carrier_adjustment;
      }
      else if (p->values[PATCH_PARAM_ALGORITHM] == PATCH_ALGORITHM_VAL_3C_PIPES)
      {
        if ((m == 0) || (m == 1) || (m == 2))
          env_level[m] += carrier_adjustment;
      }

      env_level[m] += v->env_ks_level_adjustment[m];

      env_level[m] += v->env_volume_adjustment[m];
      env_level[m] += v->env_amplitude_adjustment[m];
      env_level[m] += v->env_velocity_adjustment[m];

      /* bound level */
      if (env_level[m] < 0)
        env_level[m] = 0;
      else if (env_level[m] > VOICE_MAX_ATTENUATION_DB)
        env_level[m] = VOICE_MAX_ATTENUATION_DB;
    }

    /* update oscillators */
    for (m = 0; m < VOICE_NUM_OSCS; m++)
    {
      /* determine adjusted pitch indices */
      adjusted_pitch_index = v->osc_pitch_index[m];

      adjusted_pitch_index += vibrato_adjustment;

      if (adjusted_pitch_index < 0)
        adjusted_pitch_index = 0;
      else if (adjusted_pitch_index >= TUNING_NUM_INDICES)
        adjusted_pitch_index = TUNING_NUM_INDICES - 1;

      v->osc_phase[m] += 
        S_voice_wave_phase_increment_table[adjusted_pitch_index];

      if (v->osc_phase[m] > 0xFFFFFFF)
        v->osc_phase[m] &= 0xFFFFFFF;

      /* determine masked phase */
      masked_phase = (v->osc_phase[m] >> 18) & 0x3FF;

      /* apply phase mod, if necessary */
      if (m == 0)
      {
        if (p->values[PATCH_PARAM_FEEDBACK] > 0)
        {
          masked_phase = 
            (masked_phase + (fb_phase_mod >> (7 - p->values[PATCH_PARAM_FEEDBACK]))) & 0x3FF;
        }
      }
      else if (m == 1)
      {
        if ((p->values[PATCH_PARAM_ALGORITHM] == PATCH_ALGORITHM_VAL_1C_CHAIN)  || 
            (p->values[PATCH_PARAM_ALGORITHM] == PATCH_ALGORITHM_VAL_2C_1_TO_1) || 
            (p->values[PATCH_PARAM_ALGORITHM] == PATCH_ALGORITHM_VAL_2C_1_TO_2))
        {
          masked_phase = (masked_phase + osc_phase_mod[0]) & 0x3FF;
        }
      }
      else if (m == 2)
      {
        if (p->values[PATCH_PARAM_ALGORITHM] == PATCH_ALGORITHM_VAL_1C_CHAIN)
          masked_phase = (masked_phase + osc_phase_mod[1]) & 0x3FF;
        else if (p->values[PATCH_PARAM_ALGORITHM] == PATCH_ALGORITHM_VAL_1C_THE_V)
          masked_phase = (masked_phase + osc_phase_mod[0] + osc_phase_mod[1]) & 0x3FF;
        else if (p->values[PATCH_PARAM_ALGORITHM] == PATCH_ALGORITHM_VAL_2C_1_TO_2)
          masked_phase = (masked_phase + osc_phase_mod[0]) & 0x3FF;
      }

      /* determine waveform level (db) */
      if (masked_phase < VOICE_WAVETABLE_SIZE_HALF)
        final_index = S_voice_wavetable_sine[masked_phase];
      else
        final_index = S_voice_wavetable_sine[masked_phase - VOICE_WAVETABLE_SIZE_HALF];

      /* half rectified */
      if (p->values[PATCH_PARAM_OSC_1_WAVEFORM + m * PATCH_PARAM_OSC_SHIFT] == 1)
      {
        if (masked_phase >= VOICE_WAVETABLE_SIZE_HALF)
          final_index = VOICE_MAX_ATTENUATION_DB;
      }

      /* quarter rectified */
      if (p->values[PATCH_PARAM_OSC_1_WAVEFORM + m * PATCH_PARAM_OSC_SHIFT] == 3)
      {
        if ((masked_phase >= VOICE_WAVETABLE_SIZE_QUARTER) && (masked_phase < VOICE_WAVETABLE_SIZE_HALF))
          final_index = VOICE_MAX_ATTENUATION_DB;
        else if (masked_phase >= 3 * VOICE_WAVETABLE_SIZE_QUARTER)
          final_index = VOICE_MAX_ATTENUATION_DB;
      }

      /* apply envelope */
      final_index += env_level[m];

      if (final_index < 0)
        final_index = 0;
      else if (final_index > VOICE_MAX_ATTENUATION_DB)
        final_index = VOICE_MAX_ATTENUATION_DB;

      /* determine waveform level (linear) */
      osc_level[m] = S_voice_db_to_linear_table[final_index];

      /* non-rectified */
      if (p->values[PATCH_PARAM_OSC_1_WAVEFORM + m * PATCH_PARAM_OSC_SHIFT] == 0)
      {
        if (masked_phase >= VOICE_WAVETABLE_SIZE_HALF)
          osc_level[m] *= -1;
      }

      /* compute phase mod */
      osc_phase_mod[m] = (osc_level[m] >> 3) & 0x3FF;
    }

    /* cycle feedback */
    v->feedin[1] = v->feedin[0];
    v->feedin[0] = osc_level[0] >> 3;

    /* determine voice level */
    v->level = 0;

    if ((p->values[PATCH_PARAM_ALGORITHM] == PATCH_ALGORITHM_VAL_1C_CHAIN) || 
        (p->values[PATCH_PARAM_ALGORITHM] == PATCH_ALGORITHM_VAL_1C_THE_V))
    {
      v->level += osc_level[2];
    }
    else if ( (p->values[PATCH_PARAM_ALGORITHM] == PATCH_ALGORITHM_VAL_2C_1_TO_1) || 
              (p->values[PATCH_PARAM_ALGORITHM] == PATCH_ALGORITHM_VAL_2C_1_TO_2))
    {
      v->level += osc_level[1] + osc_level[2];
    }
    else
      v->level += osc_level[0] + osc_level[1] + osc_level[2];
  }

  return 0;
}

/*******************************************************************************
** voice_generate_tables()
*******************************************************************************/
short int voice_generate_tables()
{
  int m;
  int n;

  double val;

  float base;
  float freq;

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

  /* wavetable (sine) */
  S_voice_wavetable_sine[0] = VOICE_MAX_ATTENUATION_DB;
  S_voice_wavetable_sine[VOICE_WAVETABLE_SIZE_QUARTER] = VOICE_MAX_VOLUME_DB;

  for (m = 1; m < VOICE_WAVETABLE_SIZE_QUARTER; m++)
  {
    val = sin(TWO_PI * (m / ((float) VOICE_WAVETABLE_SIZE_FULL)));
    S_voice_wavetable_sine[m] = (short int) ((10 * (log(1 / val) / log(10)) / VOICE_DB_STEP_12_BIT) + 0.5f);
    S_voice_wavetable_sine[VOICE_WAVETABLE_SIZE_HALF - m] = S_voice_wavetable_sine[m];
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

  /* note that adding 32 to the 12-bit envelope is the  */
  /* same as adding 1 to a left shifted 7-bit envelope. */

  /* env level tables */
  S_voice_env_max_level_table[0] = VOICE_MAX_ATTENUATION_DB;

  for (m = 1; m < PATCH_NUM_ENV_LEVEL_VALS; m++)
    S_voice_env_max_level_table[m] = (PATCH_NUM_ENV_LEVEL_VALS - 1 - m) * 16;

  S_voice_env_hold_level_table[0] = VOICE_MAX_ATTENUATION_DB;

  for (m = 1; m < PATCH_NUM_ENV_LEVEL_VALS; m++)
    S_voice_env_hold_level_table[m] = (PATCH_NUM_ENV_LEVEL_VALS - 1 - m) * 8;

  /* keyscaling tables */
  for (m = 0; m < TUNING_NUM_PLAYABLE_NOTES; m++)
  {
    /* adding 12 to the rate is the same as going up one octave */
    S_voice_env_rate_keyscale_table[m] = 
      (12 * (m + TUNING_NOTE_A0 - TUNING_NOTE_C0)) / 12;

    /* adding 256 to the level (in db) is the */
    /* same as multiplying by 1/2 (in linear) */
    S_voice_env_level_keyscale_table[m] = 
      (256 * (m + TUNING_NOTE_A0 - TUNING_NOTE_A2)) / 12;
  }

  /* time table */
  S_voice_env_time_table[0] = (12 * (VOICE_ENV_NUM_OCTAVES - 1)) + 11;
  S_voice_env_time_table[1] = (12 * (VOICE_ENV_NUM_OCTAVES - 1)) + 10;
  S_voice_env_time_table[2] = (12 * (VOICE_ENV_NUM_OCTAVES - 1)) +  8;
  S_voice_env_time_table[3] = (12 * (VOICE_ENV_NUM_OCTAVES - 1)) +  7;
  S_voice_env_time_table[4] = (12 * (VOICE_ENV_NUM_OCTAVES - 1)) +  5;
  S_voice_env_time_table[5] = (12 * (VOICE_ENV_NUM_OCTAVES - 1)) +  4;
  S_voice_env_time_table[6] = (12 * (VOICE_ENV_NUM_OCTAVES - 1)) +  2;
  S_voice_env_time_table[7] = (12 * (VOICE_ENV_NUM_OCTAVES - 1)) +  0;

  for (m = 8; m < PATCH_NUM_ENV_TIME_VALS; m++)
    S_voice_env_time_table[m] = S_voice_env_time_table[m - 8] - 12;

  /* phase increment tables  */

  /* for the decay stage, the fastest rate should have a fall time  */
  /* of ~8 ms. thus with 13 octaves, the lowest rate is ~64 s.      */
  /* so, the base frequency is (1 / 64) * 4095, where 4095 is the   */
  /* number of updates per fall time (with a 12 bit envelope).      */
  base = VOICE_MAX_ATTENUATION_DB / 64.0f;

  for (n = 0; n < VOICE_ENV_NUM_OCTAVES; n++)
  {
    for (m = 0; m < 12; m++)
    {
      freq = base * pow(2, (12 * n + m) / 12.0f);

      S_voice_env_decay_increment_table[12 * n + m] = 
        (int) ((freq * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
    }
  }

  /* for the attack stage, the fastest rate should have a rise time */
  /* of ~4 ms. thus with 13 octaves, the lowest rate is ~32 s.      */
  /* the attack phase has 518 updates per rise time.                */
  base = 518.0f / 32.0f;

  for (n = 0; n < VOICE_ENV_NUM_OCTAVES; n++)
  {
    for (m = 0; m < 12; m++)
    {
      freq = base * pow(2, (12 * n + m) / 12.0f);

      S_voice_env_attack_increment_table[12 * n + m] = 
        (int) ((freq * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
    }
  }

  /* the lfo wavetables have 256 entries per period */

  /* vibrato wavetable (triangle) */
  S_voice_lfo_bi_wavetable_tri[0] = 0;
  S_voice_lfo_bi_wavetable_tri[VOICE_LFO_WAVE_PERIOD_QUARTER] = VOICE_LFO_WAVE_AMPLITUDE;
  S_voice_lfo_bi_wavetable_tri[VOICE_LFO_WAVE_PERIOD_HALF] = 0;
  S_voice_lfo_bi_wavetable_tri[3 * VOICE_LFO_WAVE_PERIOD_QUARTER] = -VOICE_LFO_WAVE_AMPLITUDE;

  for (m = 1; m < VOICE_LFO_WAVE_PERIOD_QUARTER; m++)
  {
    S_voice_lfo_bi_wavetable_tri[m] = (short int) (((VOICE_LFO_WAVE_AMPLITUDE * m) / ((float) VOICE_LFO_WAVE_PERIOD_QUARTER)) + 0.5f);
    S_voice_lfo_bi_wavetable_tri[VOICE_LFO_WAVE_PERIOD_HALF - m] = S_voice_lfo_bi_wavetable_tri[m];

    S_voice_lfo_bi_wavetable_tri[VOICE_LFO_WAVE_PERIOD_HALF + m] = -S_voice_lfo_bi_wavetable_tri[m];
    S_voice_lfo_bi_wavetable_tri[VOICE_LFO_WAVE_PERIOD_FULL - m] = -S_voice_lfo_bi_wavetable_tri[m];
  }

  /* vibrato wavetable (sawtooth) */
  S_voice_lfo_bi_wavetable_saw[0] = 0;
  S_voice_lfo_bi_wavetable_saw[VOICE_LFO_WAVE_PERIOD_HALF] = VOICE_LFO_WAVE_AMPLITUDE;

  for (m = 1; m < VOICE_LFO_WAVE_PERIOD_HALF; m++)
  {
    S_voice_lfo_bi_wavetable_saw[m] = (short int) (((VOICE_LFO_WAVE_AMPLITUDE * m) / ((float) VOICE_LFO_WAVE_PERIOD_HALF)) + 0.5f);
    S_voice_lfo_bi_wavetable_saw[VOICE_LFO_WAVE_PERIOD_FULL - m] = -S_voice_lfo_bi_wavetable_saw[m];
  }

  /* tremolo wavetable (triangle) */
  S_voice_lfo_uni_wavetable_tri[0] = 0;
  S_voice_lfo_uni_wavetable_tri[VOICE_LFO_WAVE_PERIOD_HALF] = VOICE_LFO_WAVE_AMPLITUDE;

  for (m = 1; m < VOICE_LFO_WAVE_PERIOD_HALF; m++)
  {
    S_voice_lfo_uni_wavetable_tri[m] = (short int) (((VOICE_LFO_WAVE_AMPLITUDE * m) / ((float) VOICE_LFO_WAVE_PERIOD_HALF)) + 0.5f);
    S_voice_lfo_uni_wavetable_tri[VOICE_LFO_WAVE_PERIOD_FULL - m] = S_voice_lfo_uni_wavetable_tri[m];
  }

  /* tremolo wavetable (sawtooth) */
  S_voice_lfo_uni_wavetable_saw[0] = 0;

  for (m = 1; m < VOICE_LFO_WAVE_PERIOD_FULL; m++)
  {
    S_voice_lfo_uni_wavetable_saw[m] = (short int) (((VOICE_LFO_WAVE_AMPLITUDE * m) / ((float) VOICE_LFO_WAVE_PERIOD_FULL)) + 0.5f);
  }

  /* delay period table */

  /* at 120 bpm, the delays range from        */
  /* 0.0 s to 0.5 s (counting up by 1/48 s)   */
  for (m = 0; m < TEMPO_NUM_VALUES; m++)
  {
    S_voice_lfo_delay_period_table[m] = 
      (int) ((0.020833333333333f * TEMPO_COMPUTE_SECONDS_PER_BEAT(m) * CLOCK_SAMPLING_RATE) + 0.5f);
  }

  /* phase increment table */

  /* at 120 bpm, the frequencies range from   */
  /* 0.5 hz to 8 hz (counting up by 0.5 hz)   */
  for (m = 0; m < TEMPO_NUM_VALUES; m++)
  {
    S_voice_lfo_phase_increment_table[m] = 
      (int) ((0.25f * TEMPO_COMPUTE_BEATS_PER_SECOND(m) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
  }

  /* sensitivity tables */
  for (m = 0; m < PATCH_NUM_SENSITIVITY_VALS; m++)
    S_voice_tremolo_max_table[m] = m * 4 * 32;

  for (m = 0; m < PATCH_NUM_SENSITIVITY_VALS; m++)
    S_voice_boost_max_table[m] = m * 1 * 32;

  return 0;
}

