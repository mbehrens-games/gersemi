/*******************************************************************************
** envelope.h (envelope)
*******************************************************************************/

#ifndef ENVELOPE_H
#define ENVELOPE_H

#include "bank.h"

enum
{
  ENVELOPE_STATE_ATTACK = 0, 
  ENVELOPE_STATE_DECAY_1, 
  ENVELOPE_STATE_DECAY_2, 
  ENVELOPE_STATE_RELEASE, 
  ENVELOPE_STATE_ALTERNATE_DECAY_2, 
  ENVELOPE_STATE_ALTERNATE_RELEASE 
};

typedef struct envelope
{
  /* envelope settings */
  short int rate_ks;
  short int level_ks;

  /* adjustments */
  short int ampl_adjustment;
  short int rate_adjustment;
  short int level_adjustment;

  /* decay 1 to decay 2 switch level */
  short int transition_level;

  /* rows */
  int a_row;
  int d1_row;
  int d2_row;
  int r_row;

  /* alternate rows (with sustain pedal) */
  int alt_d2_row;
  int alt_r_row;

  /* current state, row */
  int state;
  int row;

  /* phase increment, phase */
  unsigned int increment;
  unsigned int phase;

  /* note input (from oscillator) */
  short int note_input;

  /* sustain pedal input (from instrument) */
  short int pedal_input;

  /* attenuation */
  short int attenuation;

  /* level (current attenuation + adjustments) */
  short int level;
} envelope;

/* envelope bank */
extern envelope G_envelope_bank[BANK_NUM_ENVELOPES];

/* function declarations */
short int envelope_setup_all();
short int envelope_reset(int voice_index);

short int envelope_load_patch(int voice_index, int patch_index);

short int envelope_trigger(int voice_index);
short int envelope_release(int voice_index);

short int envelope_update_all();

short int envelope_generate_tables();

#endif
