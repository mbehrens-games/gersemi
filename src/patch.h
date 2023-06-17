/*******************************************************************************
** patch.h (patch settings)
*******************************************************************************/

#ifndef PATCH_H
#define PATCH_H

#include "bank.h"
#include "voice.h"

typedef struct patch
{
  /* program */
  int program;

  /* oscillators */
  short int osc_numerator[VOICE_NUM_OSCS_AND_ENVS];
  short int osc_denominator[VOICE_NUM_OSCS_AND_ENVS];
  short int osc_detune[VOICE_NUM_OSCS_AND_ENVS];
  short int osc_amplitude[VOICE_NUM_OSCS_AND_ENVS];

  /* feedback */
  short int feedback;

  /* noise generator */
  short int noise_period;
  short int noise_mix;

  /* carrier envelope */
  short int carr_attack;
  short int carr_decay_1;
  short int carr_decay_2;
  short int carr_release;
  short int carr_sustain;
  short int carr_rate_keyscaling;
  short int carr_level_keyscaling;

  /* modulator envelope */
  short int mod_attack;
  short int mod_decay_1;
  short int mod_decay_2;
  short int mod_release;
  short int mod_sustain;
  short int mod_rate_keyscaling;
  short int mod_level_keyscaling;

  /* lfo settings */

  /* filters */
  int lowpass_cutoff;
  int highpass_cutoff;
} patch;

/* patch bank */
extern patch G_patch_bank[BANK_NUM_PATCHES];

/* function declarations */
short int patch_setup_all();
short int patch_reset(int patch_index);

#endif
