/*******************************************************************************
** patch.c (patch settings)
*******************************************************************************/

#include <stdio.h>

#include "patch.h"

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

  /* oscillators and envelopes */
  for (m = 0; m < VOICE_NUM_OSCS_AND_ENVS; m++)
  {
    p->osc_waveform[m] = 1;
    p->osc_feedback[m] = 0;
    p->osc_sync[m] = 1;
    p->osc_freq_mode[m] = 0;
    p->osc_multiple[m] = 1;
    p->osc_divisor[m] = 1;
    p->osc_note[m] = 1;
    p->osc_octave[m] = 4;
    p->osc_detune[m] = 0;

    p->env_attack[m] = 32;
    p->env_decay_1[m] = 32;
    p->env_decay_2[m] = 32;
    p->env_release[m] = 32;
    p->env_amplitude[m] = 0;
    p->env_sustain[m] = 0;
    p->env_rate_ks[m] = 1;
    p->env_level_ks[m] = 1;
    p->env_trigger[m] = 1;

    p->vibrato_enable[m] = 1;
    p->tremolo_enable[m] = 1;
    p->boost_enable[m] = 1;
  }

  /* lfo */
  p->lfo_waveform = 1;
  p->lfo_frequency = 1;
  p->lfo_delay = 0;
  p->lfo_vibrato_mode = 0;
  p->lfo_sync = 1;
  p->lfo_tempo = 0;

  p->lfo_base_vibrato = 0;
  p->lfo_base_tremolo = 0;

  /* sweep */
  p->portamento_mode = 0;
  p->portamento_speed = 1;

  /* filters */
  p->highpass_cutoff = PATCH_HIGHPASS_CUTOFF_LOWER_BOUND;
  p->lowpass_cutoff = PATCH_LOWPASS_CUTOFF_UPPER_BOUND;

  /* depths */
  p->vibrato_depth = 1;
  p->tremolo_depth = 1;
  p->boost_depth = 1;

  /* mod wheel */
  p->mod_wheel_vibrato = 0;
  p->mod_wheel_tremolo = 0;
  p->mod_wheel_boost = 0;

  /* aftertouch */
  p->aftertouch_vibrato = 0;
  p->aftertouch_tremolo = 0;
  p->aftertouch_boost = 0;

  return 0;
}

