/*******************************************************************************
** patch.c (patch settings)
*******************************************************************************/

#include <stdio.h>

#include "filter.h"
#include "patch.h"
#include "voice.h"

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
  p->algorithm = VOICE_ALGORITHM_1_CAR_CHAIN;

  /* oscillators */
  for (m = 0; m < VOICE_NUM_OSCS_AND_ENVS; m++)
  {
    p->osc_feedback[m] = 0;
    p->osc_multiple[m] = 1;
    p->osc_detune[m] = 0;
    p->osc_amplitude[m] = 16;
  }

  /* carrier envelope */
  p->car_attack = 32;
  p->car_decay_1 = 16;
  p->car_decay_2 = 16;
  p->car_release = 16;
  p->car_sustain = 0;
  p->car_rate_keyscaling = 1;
  p->car_level_keyscaling = 1;

  /* modulator envelope */
  p->mod_attack = 32;
  p->mod_decay_1 = 16;
  p->mod_decay_2 = 16;
  p->mod_release = 16;
  p->mod_sustain = 0;
  p->mod_rate_keyscaling = 1;
  p->mod_level_keyscaling = 1;

  /* lfo settings */

  /* filters */
  p->lowpass_cutoff = FILTER_LOWPASS_CUTOFF_C8;
  p->highpass_cutoff = FILTER_HIGHPASS_CUTOFF_A0;

  return 0;
}

/*******************************************************************************
** patch_adjust_parameter()
*******************************************************************************/
short int patch_adjust_parameter(int patch_index, int param, int amount)
{
  patch* p;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain patch pointer */
  p = &G_patch_bank[patch_index];

  /* adjust the parameter by the amount */

  /* algorithm */
  if (param == PATCH_PARAM_ALGORITHM)
  {
    p->algorithm += amount;

    PATCH_BOUND_PARAMETER(p->algorithm, 0, VOICE_NUM_ALGORITHMS - 1)
  }
  /* carrier envelope */
  else if (param == PATCH_PARAM_CAR_ENV_ATTACK)
  {
    p->car_attack += amount;

    PATCH_BOUND_PARAMETER(p->car_attack, 
                          PATCH_ENV_ATTACK_LOWER_BOUND, 
                          PATCH_ENV_ATTACK_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_CAR_ENV_DECAY_1)
  {
    p->car_decay_1 += amount;

    PATCH_BOUND_PARAMETER(p->car_decay_1, 
                          PATCH_ENV_DECAY_1_LOWER_BOUND, 
                          PATCH_ENV_DECAY_1_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_CAR_ENV_DECAY_2)
  {
    p->car_decay_2 += amount;

    PATCH_BOUND_PARAMETER(p->car_decay_2, 
                          PATCH_ENV_DECAY_2_LOWER_BOUND, 
                          PATCH_ENV_DECAY_2_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_CAR_ENV_RELEASE)
  {
    p->car_release += amount;

    PATCH_BOUND_PARAMETER(p->car_release, 
                          PATCH_ENV_RELEASE_LOWER_BOUND, 
                          PATCH_ENV_RELEASE_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_CAR_ENV_SUSTAIN)
  {
    p->car_sustain += amount;

    PATCH_BOUND_PARAMETER(p->car_sustain, 
                          PATCH_ENV_SUSTAIN_LOWER_BOUND, 
                          PATCH_ENV_SUSTAIN_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_CAR_ENV_RATE_KS)
  {
    p->car_rate_keyscaling += amount;

    PATCH_BOUND_PARAMETER(p->car_rate_keyscaling, 
                          PATCH_ENV_RATE_KS_LOWER_BOUND, 
                          PATCH_ENV_RATE_KS_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_CAR_ENV_LEVEL_KS)
  {
    p->car_level_keyscaling += amount;

    PATCH_BOUND_PARAMETER(p->car_level_keyscaling, 
                          PATCH_ENV_LEVEL_KS_LOWER_BOUND, 
                          PATCH_ENV_LEVEL_KS_UPPER_BOUND)
  }
  /* modulator envelope */
  else if (param == PATCH_PARAM_MOD_ENV_ATTACK)
  {
    p->mod_attack += amount;

    PATCH_BOUND_PARAMETER(p->mod_attack, 
                          PATCH_ENV_ATTACK_LOWER_BOUND, 
                          PATCH_ENV_ATTACK_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_MOD_ENV_DECAY_1)
  {
    p->mod_decay_1 += amount;

    PATCH_BOUND_PARAMETER(p->mod_decay_1, 
                          PATCH_ENV_DECAY_1_LOWER_BOUND, 
                          PATCH_ENV_DECAY_1_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_MOD_ENV_DECAY_2)
  {
    p->mod_decay_2 += amount;

    PATCH_BOUND_PARAMETER(p->mod_decay_2, 
                          PATCH_ENV_DECAY_2_LOWER_BOUND, 
                          PATCH_ENV_DECAY_2_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_MOD_ENV_RELEASE)
  {
    p->mod_release += amount;

    PATCH_BOUND_PARAMETER(p->mod_release, 
                          PATCH_ENV_RELEASE_LOWER_BOUND, 
                          PATCH_ENV_RELEASE_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_MOD_ENV_SUSTAIN)
  {
    p->mod_sustain += amount;

    PATCH_BOUND_PARAMETER(p->mod_sustain, 
                          PATCH_ENV_SUSTAIN_LOWER_BOUND, 
                          PATCH_ENV_SUSTAIN_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_MOD_ENV_RATE_KS)
  {
    p->mod_rate_keyscaling += amount;

    PATCH_BOUND_PARAMETER(p->mod_rate_keyscaling, 
                          PATCH_ENV_RATE_KS_LOWER_BOUND, 
                          PATCH_ENV_RATE_KS_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_MOD_ENV_LEVEL_KS)
  {
    p->mod_level_keyscaling += amount;

    PATCH_BOUND_PARAMETER(p->mod_level_keyscaling, 
                          PATCH_ENV_LEVEL_KS_LOWER_BOUND, 
                          PATCH_ENV_LEVEL_KS_UPPER_BOUND)
  }
  /* oscillator 1 */
  else if (param == PATCH_PARAM_OSC_1_FEEDBACK)
  {
    p->osc_feedback[0] += amount;

    PATCH_BOUND_PARAMETER(p->osc_feedback[0], 
                          PATCH_OSC_FEEDBACK_LOWER_BOUND, 
                          PATCH_OSC_FEEDBACK_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_OSC_1_MULTIPLE)
  {
    p->osc_multiple[0] += amount;

    PATCH_BOUND_PARAMETER(p->osc_multiple[0], 
                          PATCH_OSC_MULTIPLE_LOWER_BOUND, 
                          PATCH_OSC_MULTIPLE_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_OSC_1_DETUNE)
  {
    p->osc_detune[0] += amount;

    PATCH_BOUND_PARAMETER(p->osc_detune[0], 
                          PATCH_OSC_DETUNE_LOWER_BOUND, 
                          PATCH_OSC_DETUNE_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_OSC_1_AMPLITUDE)
  {
    p->osc_amplitude[0] += amount;

    PATCH_BOUND_PARAMETER(p->osc_amplitude[0], 
                          PATCH_OSC_AMPLITUDE_LOWER_BOUND, 
                          PATCH_OSC_AMPLITUDE_UPPER_BOUND)
  }
  /* oscillator 2 */
  else if (param == PATCH_PARAM_OSC_2_FEEDBACK)
  {
    p->osc_feedback[1] += amount;

    PATCH_BOUND_PARAMETER(p->osc_feedback[1], 
                          PATCH_OSC_FEEDBACK_LOWER_BOUND, 
                          PATCH_OSC_FEEDBACK_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_OSC_2_MULTIPLE)
  {
    p->osc_multiple[1] += amount;

    PATCH_BOUND_PARAMETER(p->osc_multiple[1], 
                          PATCH_OSC_MULTIPLE_LOWER_BOUND, 
                          PATCH_OSC_MULTIPLE_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_OSC_2_DETUNE)
  {
    p->osc_detune[1] += amount;

    PATCH_BOUND_PARAMETER(p->osc_detune[1], 
                          PATCH_OSC_DETUNE_LOWER_BOUND, 
                          PATCH_OSC_DETUNE_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_OSC_2_AMPLITUDE)
  {
    p->osc_amplitude[1] += amount;

    PATCH_BOUND_PARAMETER(p->osc_amplitude[1], 
                          PATCH_OSC_AMPLITUDE_LOWER_BOUND, 
                          PATCH_OSC_AMPLITUDE_UPPER_BOUND)
  }
  /* oscillator 3 */
  else if (param == PATCH_PARAM_OSC_3_FEEDBACK)
  {
    p->osc_feedback[2] += amount;

    PATCH_BOUND_PARAMETER(p->osc_feedback[2], 
                          PATCH_OSC_FEEDBACK_LOWER_BOUND, 
                          PATCH_OSC_FEEDBACK_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_OSC_3_MULTIPLE)
  {
    p->osc_multiple[2] += amount;

    PATCH_BOUND_PARAMETER(p->osc_multiple[2], 
                          PATCH_OSC_MULTIPLE_LOWER_BOUND, 
                          PATCH_OSC_MULTIPLE_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_OSC_3_DETUNE)
  {
    p->osc_detune[2] += amount;

    PATCH_BOUND_PARAMETER(p->osc_detune[2], 
                          PATCH_OSC_DETUNE_LOWER_BOUND, 
                          PATCH_OSC_DETUNE_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_OSC_3_AMPLITUDE)
  {
    p->osc_amplitude[2] += amount;

    PATCH_BOUND_PARAMETER(p->osc_amplitude[2], 
                          PATCH_OSC_AMPLITUDE_LOWER_BOUND, 
                          PATCH_OSC_AMPLITUDE_UPPER_BOUND)
  }
  /* oscillator 4 */
  else if (param == PATCH_PARAM_OSC_4_FEEDBACK)
  {
    p->osc_feedback[3] += amount;

    PATCH_BOUND_PARAMETER(p->osc_feedback[3], 
                          PATCH_OSC_FEEDBACK_LOWER_BOUND, 
                          PATCH_OSC_FEEDBACK_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_OSC_4_MULTIPLE)
  {
    p->osc_multiple[3] += amount;

    PATCH_BOUND_PARAMETER(p->osc_multiple[3], 
                          PATCH_OSC_MULTIPLE_LOWER_BOUND, 
                          PATCH_OSC_MULTIPLE_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_OSC_4_DETUNE)
  {
    p->osc_detune[3] += amount;

    PATCH_BOUND_PARAMETER(p->osc_detune[3], 
                          PATCH_OSC_DETUNE_LOWER_BOUND, 
                          PATCH_OSC_DETUNE_UPPER_BOUND)
  }
  else if (param == PATCH_PARAM_OSC_4_AMPLITUDE)
  {
    p->osc_amplitude[3] += amount;

    PATCH_BOUND_PARAMETER(p->osc_amplitude[3], 
                          PATCH_OSC_AMPLITUDE_LOWER_BOUND, 
                          PATCH_OSC_AMPLITUDE_UPPER_BOUND)
  }
  /* filters */
  else if (param == PATCH_PARAM_LOWPASS_CUTOFF)
  {
    p->lowpass_cutoff += amount;

    PATCH_BOUND_PARAMETER(p->lowpass_cutoff, 0, FILTER_NUM_LOWPASS_CUTOFFS - 1)
  }
  else if (param == PATCH_PARAM_HIGHPASS_CUTOFF)
  {
    p->highpass_cutoff += amount;

    PATCH_BOUND_PARAMETER(p->highpass_cutoff, 0, FILTER_NUM_HIGHPASS_CUTOFFS - 1)
  }

  return 0;
}

