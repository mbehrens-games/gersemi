/*******************************************************************************
** patch.c (patch settings)
*******************************************************************************/

#include <stdio.h>

#include "patch.h"

#define PATCH_SET_PARAMETER(param)                                             \
  if (mode == PATCH_ADJUST_MODE_DIRECT)                                        \
    param = value;                                                             \
  else if (mode == PATCH_ADJUST_MODE_RELATIVE)                                 \
    param += value;

#define PATCH_BOUND_PARAMETER(param, lower, upper)                             \
  if (param < lower)                                                           \
    param = lower;                                                             \
  else if (param > upper)                                                      \
    param = upper;

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
  p->algorithm = PATCH_ALGORITHM_LOWER_BOUND;

  /* filters */
  p->highpass_cutoff = PATCH_HIGHPASS_CUTOFF_LOWER_BOUND;
  p->lowpass_cutoff = PATCH_LOWPASS_CUTOFF_UPPER_BOUND;

  /* noise */
  p->noise_period = PATCH_NOISE_PERIOD_LOWER_BOUND;
  p->noise_mix = PATCH_NOISE_MIX_LOWER_BOUND;

  /* oscillators and envelopes */
  for (m = 0; m < VOICE_NUM_OSCS_AND_ENVS; m++)
  {
    p->osc_waveform[m] = 0;
    p->osc_feedback[m] = 0;
    p->osc_freq_mode[m] = 0;
    p->osc_multiple[m] = 1;
    p->osc_divisor[m] = 1;
    p->osc_detune[m] = 0;

    p->env_attack[m] = 32;
    p->env_decay_1[m] = 32;
    p->env_decay_2[m] = 32;
    p->env_release[m] = 32;
    p->env_amplitude[m] = 0;
    p->env_sustain[m] = 0;
    p->env_rate_ks[m] = 1;
    p->env_level_ks[m] = 1;
    p->env_special_mode[m] = 0;
  }

  return 0;
}

/*******************************************************************************
** patch_adjust_parameter()
*******************************************************************************/
short int patch_adjust_parameter( int patch_index, int param, int num, 
                                  int mode, int value)
{
  patch* p;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain patch pointer */
  p = &G_patch_bank[patch_index];

  /* make sure the oscillator / envelope number is valid */
  if ((num < 0) || (num >= VOICE_NUM_OSCS_AND_ENVS))
    return 1;

  /* adjust the parameter by the value */

  /* algorithm */
  if (param == PATCH_PARAM_ALGORITHM)
  {
    PATCH_SET_PARAMETER(p->algorithm)
    PATCH_BOUND_PARAMETER(p->algorithm, 
                          PATCH_ALGORITHM_LOWER_BOUND, 
                          PATCH_ALGORITHM_UPPER_BOUND)
  }
  /* filters */
  else if (param == PATCH_PARAM_HIGHPASS_CUTOFF)
  {
    PATCH_SET_PARAMETER(p->highpass_cutoff)
    PATCH_BOUND_PARAMETER(p->highpass_cutoff, 
                          PATCH_HIGHPASS_CUTOFF_LOWER_BOUND, 
                          PATCH_HIGHPASS_CUTOFF_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_LOWPASS_CUTOFF)
  {
    PATCH_SET_PARAMETER(p->lowpass_cutoff)
    PATCH_BOUND_PARAMETER(p->lowpass_cutoff, 
                          PATCH_LOWPASS_CUTOFF_LOWER_BOUND, 
                          PATCH_LOWPASS_CUTOFF_UPPER_BOUND)
  }
  /* noise */
  else if (param == PATCH_PARAM_NOISE_PERIOD)
  {
    PATCH_SET_PARAMETER(p->noise_period)
    PATCH_BOUND_PARAMETER(p->noise_period, 
                          PATCH_NOISE_PERIOD_LOWER_BOUND, 
                          PATCH_NOISE_PERIOD_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_NOISE_MIX)
  {
    PATCH_SET_PARAMETER(p->noise_mix)
    PATCH_BOUND_PARAMETER(p->noise_mix, 
                          PATCH_NOISE_MIX_LOWER_BOUND, 
                          PATCH_NOISE_MIX_UPPER_BOUND)
  }
  /* oscillator */
  else if (param == PATCH_PARAM_OSC_WAVEFORM)
  {
    PATCH_SET_PARAMETER(p->osc_waveform[num])
    PATCH_BOUND_PARAMETER(p->osc_waveform[num], 
                          PATCH_OSC_WAVEFORM_LOWER_BOUND, 
                          PATCH_OSC_WAVEFORM_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_OSC_FEEDBACK)
  {
    PATCH_SET_PARAMETER(p->osc_feedback[num])
    PATCH_BOUND_PARAMETER(p->osc_feedback[num], 
                          PATCH_OSC_FEEDBACK_LOWER_BOUND, 
                          PATCH_OSC_FEEDBACK_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_OSC_FREQ_MODE)
  {
    PATCH_SET_PARAMETER(p->osc_freq_mode[num])
    PATCH_BOUND_PARAMETER(p->osc_freq_mode[num], 
                          PATCH_OSC_FREQ_MODE_LOWER_BOUND, 
                          PATCH_OSC_FREQ_MODE_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_OSC_MULTIPLE)
  {
    PATCH_SET_PARAMETER(p->osc_multiple[num])
    PATCH_BOUND_PARAMETER(p->osc_multiple[num], 
                          PATCH_OSC_MULTIPLE_LOWER_BOUND, 
                          PATCH_OSC_MULTIPLE_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_OSC_DIVISOR)
  {
    PATCH_SET_PARAMETER(p->osc_divisor[num])
    PATCH_BOUND_PARAMETER(p->osc_divisor[num], 
                          PATCH_OSC_DIVISOR_LOWER_BOUND, 
                          PATCH_OSC_DIVISOR_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_OSC_DETUNE)
  {
    PATCH_SET_PARAMETER(p->osc_detune[num])
    PATCH_BOUND_PARAMETER(p->osc_detune[num], 
                          PATCH_OSC_DETUNE_LOWER_BOUND, 
                          PATCH_OSC_DETUNE_UPPER_BOUND)
  }
  /* envelope */
  else if (param == PATCH_PARAM_ENV_ATTACK)
  {
    PATCH_SET_PARAMETER(p->env_attack[num])
    PATCH_BOUND_PARAMETER(p->env_attack[num], 
                          PATCH_ENV_RATE_LOWER_BOUND, 
                          PATCH_ENV_RATE_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_ENV_DECAY_1)
  {
    PATCH_SET_PARAMETER(p->env_decay_1[num])
    PATCH_BOUND_PARAMETER(p->env_decay_1[num], 
                          PATCH_ENV_RATE_LOWER_BOUND, 
                          PATCH_ENV_RATE_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_ENV_DECAY_2)
  {
    PATCH_SET_PARAMETER(p->env_decay_2[num])
    PATCH_BOUND_PARAMETER(p->env_decay_2[num], 
                          PATCH_ENV_RATE_LOWER_BOUND, 
                          PATCH_ENV_RATE_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_ENV_RELEASE)
  {
    PATCH_SET_PARAMETER(p->env_release[num])
    PATCH_BOUND_PARAMETER(p->env_release[num], 
                          PATCH_ENV_RATE_LOWER_BOUND, 
                          PATCH_ENV_RATE_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_ENV_AMPLITUDE)
  {
    PATCH_SET_PARAMETER(p->env_amplitude[num])
    PATCH_BOUND_PARAMETER(p->env_amplitude[num], 
                          PATCH_ENV_AMPLITUDE_LOWER_BOUND, 
                          PATCH_ENV_AMPLITUDE_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_ENV_SUSTAIN)
  {
    PATCH_SET_PARAMETER(p->env_sustain[num])
    PATCH_BOUND_PARAMETER(p->env_sustain[num], 
                          PATCH_ENV_SUSTAIN_LOWER_BOUND, 
                          PATCH_ENV_SUSTAIN_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_ENV_RATE_KS)
  {
    PATCH_SET_PARAMETER(p->env_rate_ks[num])
    PATCH_BOUND_PARAMETER(p->env_rate_ks[num], 
                          PATCH_ENV_KEYSCALE_LOWER_BOUND, 
                          PATCH_ENV_KEYSCALE_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_ENV_LEVEL_KS)
  {
    PATCH_SET_PARAMETER(p->env_level_ks[num])
    PATCH_BOUND_PARAMETER(p->env_level_ks[num], 
                          PATCH_ENV_KEYSCALE_LOWER_BOUND, 
                          PATCH_ENV_KEYSCALE_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_ENV_SPECIAL_MODE)
  {
    PATCH_SET_PARAMETER(p->env_special_mode[num])
    PATCH_BOUND_PARAMETER(p->env_special_mode[num], 
                          PATCH_ENV_SPECIAL_MODE_LOWER_BOUND, 
                          PATCH_ENV_SPECIAL_MODE_UPPER_BOUND)
  }

  return 0;
}

