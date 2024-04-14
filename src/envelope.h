/*******************************************************************************
** envelope.h (envelope)
*******************************************************************************/

#ifndef ENVELOPE_H
#define ENVELOPE_H

#include "bank.h"

typedef struct envelope
{
  /* keyscaling settings */
  short int ks_rate_fraction;
  short int ks_level_fraction;
  short int ks_break_note;

  /* adjustments */
  short int rate_adjustment;
  short int level_adjustment;

  /* decay 1 to decay 2 switch level */
  short int transition_level;

  /* sustain pedal state */
  short int sustain_pedal;

  /* rates */
  int a_rate;
  int d1_rate;
  int d2_rate;
  int r_rate;

  /* alternate rate (with sustain pedal) */
  int pedal_rate;

  /* current stage, rate */
  int stage;
  int rate;

  /* phase increment, phase */
  unsigned int increment;
  unsigned int phase;

  /* attenuation */
  short int attenuation;

  /* level (current attenuation + adjustments) */
  short int level;
} envelope;

/* envelope bank */
extern envelope G_envelope_bank[BANK_NUM_ENVELOPES];

/* function declarations */
short int envelope_reset_all();

short int envelope_load_patch(int voice_index, int patch_index);

short int envelope_set_sustain_pedal(int voice_index, int state);

short int envelope_set_note(int voice_index, int note);

short int envelope_trigger(int voice_index);
short int envelope_release(int voice_index);

short int envelope_update_all();

short int envelope_generate_tables();

#endif
