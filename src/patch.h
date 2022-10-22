/*******************************************************************************
** patch.h (patch settings)
*******************************************************************************/

#ifndef PATCH_H
#define PATCH_H

typedef struct patch
{
  /* program */
  int program;

  /* oscillators */
  short int osc_1_numerator;
  short int osc_1_denominator;
  short int osc_1_detune;
  short int osc_1_amplitude;

  short int osc_2_numerator;
  short int osc_2_denominator;
  short int osc_2_detune;
  short int osc_2_amplitude;

  short int osc_3_numerator;
  short int osc_3_denominator;
  short int osc_3_detune;
  short int osc_3_amplitude;

  short int osc_4_numerator;
  short int osc_4_denominator;
  short int osc_4_detune;
  short int osc_4_amplitude;

  /* feedback */
  short int feedback;

  /* noise generator */
  short int noise_period;
  short int noise_amplitude;
  short int noise_alternate;

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

  /* linear envelope settings */
  int       extra_mode;
  short int extra_rate;
  short int extra_keyscaling;

  /* lfo settings */
  int       vibrato_mode;
  short int vibrato_alternate;

  int tremolo_mode;
  int wobble_mode;

  /* filters */
  int lowpass_cutoff;
  int highpass_cutoff;
} patch;

/* function declarations */
short int patch_reset(patch* p);

#endif
