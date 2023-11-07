/*******************************************************************************
** patch.c (patch settings)
*******************************************************************************/

#include <stdio.h>

#include "patch.h"

#define PATCH_RESET_PARAMETER(param, name)                                     \
  param = PATCH_##name##_DEFAULT;

#define PATCH_BOUND_PARAMETER(param, name)                                     \
  if (param < PATCH_##name##_LOWER_BOUND)                                      \
    param = PATCH_##name##_LOWER_BOUND;                                        \
  else if (param > PATCH_##name##_UPPER_BOUND)                                 \
    param = PATCH_##name##_UPPER_BOUND;

/* patch bank */
patch G_patch_bank[BANK_NUM_PATCHES];

/*******************************************************************************
** patch_setup_all()
*******************************************************************************/
short int patch_setup_all()
{
  int k;

  /* setup all patches */
  for (k = 0; k < BANK_NUM_PATCHES; k++)
    patch_reset(k);

  return 0;
}

/*******************************************************************************
** patch_reset()
*******************************************************************************/
short int patch_reset(int patch_index)
{
  int m;

  patch* p;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain patch pointer */
  p = &G_patch_bank[patch_index];

  /* algorithm */
  PATCH_RESET_PARAMETER(p->algorithm, ALGORITHM)

  /* filters */
  PATCH_RESET_PARAMETER(p->highpass_cutoff, HIGHPASS_CUTOFF)
  PATCH_RESET_PARAMETER(p->lowpass_cutoff,  LOWPASS_CUTOFF)

  /* pedal */
  PATCH_RESET_PARAMETER(p->pedal_shift, PEDAL_SHIFT)

  /* oscillators and envelopes */
  for (m = 0; m < BANK_OSCS_AND_ENVS_PER_VOICE; m++)
  {
    PATCH_RESET_PARAMETER(p->osc_waveform[m],   OSC_WAVEFORM)
    PATCH_RESET_PARAMETER(p->osc_feedback[m],   OSC_FEEDBACK)
    PATCH_RESET_PARAMETER(p->osc_sync[m],       OSC_SYNC)
    PATCH_RESET_PARAMETER(p->osc_freq_mode[m],  OSC_FREQ_MODE)
    PATCH_RESET_PARAMETER(p->osc_multiple[m],   OSC_MULTIPLE)
    PATCH_RESET_PARAMETER(p->osc_divisor[m],    OSC_DIVISOR)
    PATCH_RESET_PARAMETER(p->osc_note[m],       OSC_NOTE)
    PATCH_RESET_PARAMETER(p->osc_octave[m],     OSC_OCTAVE)
    PATCH_RESET_PARAMETER(p->osc_detune[m],     OSC_DETUNE)

    PATCH_RESET_PARAMETER(p->env_attack[m],     ENV_RATE)
    PATCH_RESET_PARAMETER(p->env_decay_1[m],    ENV_RATE)
    PATCH_RESET_PARAMETER(p->env_decay_2[m],    ENV_RATE)
    PATCH_RESET_PARAMETER(p->env_release[m],    ENV_RATE)
    PATCH_RESET_PARAMETER(p->env_amplitude[m],  ENV_AMPLITUDE)
    PATCH_RESET_PARAMETER(p->env_sustain[m],    ENV_SUSTAIN)

    PATCH_RESET_PARAMETER(p->ks_mode[m],        KEYSCALING_MODE)
    PATCH_RESET_PARAMETER(p->ks_rate_depth[m],  KEYSCALING_DEPTH)
    PATCH_RESET_PARAMETER(p->ks_level_depth[m], KEYSCALING_DEPTH)
    PATCH_RESET_PARAMETER(p->ks_break_point[m], KEYSCALING_BREAK_POINT)
  }

  /* lfo */
  PATCH_RESET_PARAMETER(p->lfo_waveform,  LFO_WAVEFORM)
  PATCH_RESET_PARAMETER(p->lfo_frequency, LFO_FREQUENCY)
  PATCH_RESET_PARAMETER(p->lfo_delay,     LFO_DELAY)
  PATCH_RESET_PARAMETER(p->lfo_sync,      LFO_SYNC)
  PATCH_RESET_PARAMETER(p->lfo_quantize,  LFO_QUANTIZE)

  /* vibrato / tremolo / boost */
  PATCH_RESET_PARAMETER(p->vibrato_depth, EFFECT_DEPTH)
  PATCH_RESET_PARAMETER(p->vibrato_base,  EFFECT_BASE)
  PATCH_RESET_PARAMETER(p->vibrato_mode,  VIBRATO_MODE)

  PATCH_RESET_PARAMETER(p->tremolo_depth, EFFECT_DEPTH)
  PATCH_RESET_PARAMETER(p->tremolo_base,  EFFECT_BASE)
  PATCH_RESET_PARAMETER(p->tremolo_mode,  TREMOLO_MODE)

  PATCH_RESET_PARAMETER(p->boost_depth,   EFFECT_DEPTH)
  PATCH_RESET_PARAMETER(p->boost_mode,    BOOST_MODE)

  /* mod wheel / aftertouch */
  PATCH_RESET_PARAMETER(p->mod_wheel_effect,  CONTROLLER_EFFECT)
  PATCH_RESET_PARAMETER(p->aftertouch_effect, CONTROLLER_EFFECT)

  /* portamento */
  PATCH_RESET_PARAMETER(p->portamento_mode,   PORTAMENTO_MODE)
  PATCH_RESET_PARAMETER(p->portamento_legato, PORTAMENTO_LEGATO)
  PATCH_RESET_PARAMETER(p->portamento_speed,  PORTAMENTO_SPEED)

  /* pitch wheel */
  PATCH_RESET_PARAMETER(p->pitch_wheel_mode,  PITCH_WHEEL_MODE)
  PATCH_RESET_PARAMETER(p->pitch_wheel_range, PITCH_WHEEL_RANGE)

  return 0;
}

/*******************************************************************************
** patch_validate()
*******************************************************************************/
short int patch_validate(int patch_index)
{
  int m;

  patch* p;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain patch pointer */
  p = &G_patch_bank[patch_index];

  /* algorithm */
  PATCH_BOUND_PARAMETER(p->algorithm, ALGORITHM)

  /* filters */
  PATCH_BOUND_PARAMETER(p->highpass_cutoff, HIGHPASS_CUTOFF)
  PATCH_BOUND_PARAMETER(p->lowpass_cutoff,  LOWPASS_CUTOFF)

  /* pedal */
  PATCH_BOUND_PARAMETER(p->pedal_shift, PEDAL_SHIFT)

  /* oscillators and envelopes */
  for (m = 0; m < BANK_OSCS_AND_ENVS_PER_VOICE; m++)
  {
    PATCH_BOUND_PARAMETER(p->osc_waveform[m],     OSC_WAVEFORM)
    PATCH_BOUND_PARAMETER(p->osc_feedback[m],     OSC_FEEDBACK)
    PATCH_BOUND_PARAMETER(p->osc_sync[m],         OSC_SYNC)
    PATCH_BOUND_PARAMETER(p->osc_freq_mode[m],    OSC_FREQ_MODE)
    PATCH_BOUND_PARAMETER(p->osc_multiple[m],     OSC_MULTIPLE)
    PATCH_BOUND_PARAMETER(p->osc_divisor[m],      OSC_DIVISOR)
    PATCH_BOUND_PARAMETER(p->osc_note[m],         OSC_NOTE)
    PATCH_BOUND_PARAMETER(p->osc_octave[m],       OSC_OCTAVE)
    PATCH_BOUND_PARAMETER(p->osc_detune[m],       OSC_DETUNE)

    PATCH_BOUND_PARAMETER(p->env_attack[m],       ENV_RATE)
    PATCH_BOUND_PARAMETER(p->env_decay_1[m],      ENV_RATE)
    PATCH_BOUND_PARAMETER(p->env_decay_2[m],      ENV_RATE)
    PATCH_BOUND_PARAMETER(p->env_release[m],      ENV_RATE)
    PATCH_BOUND_PARAMETER(p->env_amplitude[m],    ENV_AMPLITUDE)
    PATCH_BOUND_PARAMETER(p->env_sustain[m],      ENV_SUSTAIN)

    PATCH_BOUND_PARAMETER(p->ks_mode[m],        KEYSCALING_MODE)
    PATCH_BOUND_PARAMETER(p->ks_rate_depth[m],  KEYSCALING_DEPTH)
    PATCH_BOUND_PARAMETER(p->ks_level_depth[m], KEYSCALING_DEPTH)
    PATCH_BOUND_PARAMETER(p->ks_break_point[m], KEYSCALING_BREAK_POINT)
  }

  /* lfo */
  PATCH_BOUND_PARAMETER(p->lfo_waveform,  LFO_WAVEFORM)
  PATCH_BOUND_PARAMETER(p->lfo_frequency, LFO_FREQUENCY)
  PATCH_BOUND_PARAMETER(p->lfo_delay,     LFO_DELAY)
  PATCH_BOUND_PARAMETER(p->lfo_sync,      LFO_SYNC)
  PATCH_BOUND_PARAMETER(p->lfo_quantize,  LFO_QUANTIZE)

  /* vibrato / tremolo / boost */
  PATCH_BOUND_PARAMETER(p->vibrato_depth, EFFECT_DEPTH)
  PATCH_BOUND_PARAMETER(p->vibrato_base,  EFFECT_BASE)
  PATCH_BOUND_PARAMETER(p->vibrato_mode,  VIBRATO_MODE)

  PATCH_BOUND_PARAMETER(p->tremolo_depth, EFFECT_DEPTH)
  PATCH_BOUND_PARAMETER(p->tremolo_base,  EFFECT_BASE)
  PATCH_BOUND_PARAMETER(p->tremolo_mode,  TREMOLO_MODE)

  PATCH_BOUND_PARAMETER(p->boost_depth,   EFFECT_DEPTH)
  PATCH_BOUND_PARAMETER(p->boost_mode,    BOOST_MODE)

  /* mod wheel / aftertouch */
  PATCH_BOUND_PARAMETER(p->mod_wheel_effect,  CONTROLLER_EFFECT)
  PATCH_BOUND_PARAMETER(p->aftertouch_effect, CONTROLLER_EFFECT)

  /* portamento */
  PATCH_BOUND_PARAMETER(p->portamento_mode,   PORTAMENTO_MODE)
  PATCH_BOUND_PARAMETER(p->portamento_legato, PORTAMENTO_LEGATO)
  PATCH_BOUND_PARAMETER(p->portamento_speed,  PORTAMENTO_SPEED)

  /* pitch wheel */
  PATCH_BOUND_PARAMETER(p->pitch_wheel_mode,  PITCH_WHEEL_MODE)
  PATCH_BOUND_PARAMETER(p->pitch_wheel_range, PITCH_WHEEL_RANGE)

  return 0;
}

