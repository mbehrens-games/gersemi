/*******************************************************************************
** patch.c (patch settings)
*******************************************************************************/

#include <stdio.h>

#include "envelope.h"
#include "filter.h"
#include "lfo.h"
#include "linear.h"
#include "patch.h"
#include "voice.h"
#include "waveform.h"

/*******************************************************************************
** patch_reset()
*******************************************************************************/
short int patch_reset(patch* p)
{
  if (p == NULL)
    return 1;

  /* program */
  p->program = VOICE_PROGRAM_SYNC_SQUARE;

  /* oscillators */
  p->osc_1_numerator = 1;
  p->osc_1_denominator = 1;
  p->osc_1_detune = 8;
  p->osc_1_amplitude = 16;

  p->osc_2_numerator = 1;
  p->osc_2_denominator = 1;
  p->osc_2_detune = 8;
  p->osc_2_amplitude = 16;

  p->osc_3_numerator = 1;
  p->osc_3_denominator = 1;
  p->osc_3_detune = 8;
  p->osc_3_amplitude = 16;

  p->osc_4_numerator = 1;
  p->osc_4_denominator = 1;
  p->osc_4_detune = 8;
  p->osc_4_amplitude = 16;

  /* feedback */
  p->feedback = 0;

  /* noise generator */
  p->noise_period = 0;
  p->noise_amplitude = 0;
  p->noise_alternate = 0;

  /* carrier envelope */
  p->carr_attack = 1;
  p->carr_decay_1 = 1;
  p->carr_decay_2 = 1;
  p->carr_release = 1;
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

  /* linear envelope settings */
  p->extra_mode = LINEAR_MODE_CONSTANT;
  p->extra_rate = 1;
  p->extra_keyscaling = 1;

  /* lfo settings */
  p->vibrato_mode = LFO_MODE_TRIANGLE;
  p->vibrato_alternate = 0;

  p->tremolo_mode = LFO_MODE_TRIANGLE;
  p->wobble_mode = LFO_MODE_TRIANGLE;

  /* filters */
  p->lowpass_cutoff = FILTER_LOWPASS_CUTOFF_C8;
  p->highpass_cutoff = FILTER_HIGHPASS_CUTOFF_A0;

  return 0;
}

