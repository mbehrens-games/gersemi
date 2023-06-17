/*******************************************************************************
** patch.c (patch settings)
*******************************************************************************/

#include <stdio.h>

#include "filter.h"
#include "patch.h"
#include "voice.h"

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

  /* program */
  p->program = VOICE_PROGRAM_SYNC_SQUARE;

  /* oscillators */
  for (m = 0; m < VOICE_NUM_OSCS_AND_ENVS; m++)
  {
    p->osc_numerator[m] = 1;
    p->osc_denominator[m] = 1;
    p->osc_detune[m] = 8;
    p->osc_amplitude[m] = 16;
  }

  /* feedback */
  p->feedback = 0;

  /* noise generator */
  p->noise_period = 0;
  p->noise_mix = 0;

  /* carrier envelope */
  p->carr_attack = 32;
  p->carr_decay_1 = 16;
  p->carr_decay_2 = 16;
  p->carr_release = 16;
  p->carr_sustain = 0;
  p->carr_rate_keyscaling = 1;
  p->carr_level_keyscaling = 1;

  /* modulator envelope */
  p->mod_attack = 1;
  p->mod_decay_1 = 1;
  p->mod_decay_2 = 1;
  p->mod_release = 1;
  p->mod_sustain = 0;
  p->mod_rate_keyscaling = 1;
  p->mod_level_keyscaling = 1;

  /* lfo settings */

  /* filters */
  p->lowpass_cutoff = FILTER_LOWPASS_CUTOFF_C8;
  p->highpass_cutoff = FILTER_HIGHPASS_CUTOFF_A0;

  return 0;
}

