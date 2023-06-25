/*******************************************************************************
** patch.h (patch settings)
*******************************************************************************/

#ifndef PATCH_H
#define PATCH_H

#include "bank.h"
#include "filter.h"
#include "voice.h"

enum
{
  PATCH_PARAM_ALGORITHM = 0, 
  PATCH_PARAM_OSC_FEEDBACK, 
  PATCH_PARAM_OSC_MULTIPLE, 
  PATCH_PARAM_OSC_DETUNE, 
  PATCH_PARAM_OSC_AMPLITUDE, 
  PATCH_PARAM_ENV_ATTACK, 
  PATCH_PARAM_ENV_DECAY_1, 
  PATCH_PARAM_ENV_DECAY_2, 
  PATCH_PARAM_ENV_RELEASE, 
  PATCH_PARAM_ENV_SUSTAIN, 
  PATCH_PARAM_ENV_RATE_KS, 
  PATCH_PARAM_ENV_LEVEL_KS, 
  PATCH_PARAM_LOWPASS_CUTOFF, 
  PATCH_PARAM_HIGHPASS_CUTOFF, 
  PATCH_NUM_PARAMS
};

#define PATCH_OSC_FEEDBACK_LOWER_BOUND  0
#define PATCH_OSC_FEEDBACK_UPPER_BOUND  7

#define PATCH_OSC_MULTIPLE_LOWER_BOUND  0
#define PATCH_OSC_MULTIPLE_UPPER_BOUND  15

#define PATCH_OSC_DETUNE_LOWER_BOUND    -8
#define PATCH_OSC_DETUNE_UPPER_BOUND    8

#define PATCH_OSC_AMPLITUDE_LOWER_BOUND 0
#define PATCH_OSC_AMPLITUDE_UPPER_BOUND 16

#define PATCH_ENV_ATTACK_LOWER_BOUND    1
#define PATCH_ENV_ATTACK_UPPER_BOUND    32

#define PATCH_ENV_DECAY_1_LOWER_BOUND   1
#define PATCH_ENV_DECAY_1_UPPER_BOUND   32

#define PATCH_ENV_DECAY_2_LOWER_BOUND   1
#define PATCH_ENV_DECAY_2_UPPER_BOUND   32

#define PATCH_ENV_RELEASE_LOWER_BOUND   1
#define PATCH_ENV_RELEASE_UPPER_BOUND   32

#define PATCH_ENV_SUSTAIN_LOWER_BOUND   0
#define PATCH_ENV_SUSTAIN_UPPER_BOUND   16

#define PATCH_ENV_RATE_KS_LOWER_BOUND   1
#define PATCH_ENV_RATE_KS_UPPER_BOUND   8

#define PATCH_ENV_LEVEL_KS_LOWER_BOUND  1
#define PATCH_ENV_LEVEL_KS_UPPER_BOUND  8

typedef struct patch
{
  /* algorithm */
  int algorithm;

  /* oscillators */
  int       osc_feedback[VOICE_NUM_OSCS_AND_ENVS];
  short int osc_multiple[VOICE_NUM_OSCS_AND_ENVS];
  short int osc_detune[VOICE_NUM_OSCS_AND_ENVS];
  short int osc_amplitude[VOICE_NUM_OSCS_AND_ENVS];

  /* envelopes */
  short int env_attack[VOICE_NUM_OSCS_AND_ENVS];
  short int env_decay_1[VOICE_NUM_OSCS_AND_ENVS];
  short int env_decay_2[VOICE_NUM_OSCS_AND_ENVS];
  short int env_release[VOICE_NUM_OSCS_AND_ENVS];
  short int env_sustain[VOICE_NUM_OSCS_AND_ENVS];
  short int env_rate_ks[VOICE_NUM_OSCS_AND_ENVS];
  short int env_level_ks[VOICE_NUM_OSCS_AND_ENVS];

  /* noise settings */

  /* filters */
  int lowpass_cutoff;
  int highpass_cutoff;
} patch;

/* patch bank */
extern patch G_patch_bank[BANK_NUM_PATCHES];

/* function declarations */
short int patch_setup_all();
short int patch_reset(int patch_index);

short int patch_adjust_parameter(int patch_index, int param, int num, int amount);

#endif
