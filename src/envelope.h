/*******************************************************************************
** envelope.h (envelope)
*******************************************************************************/

#ifndef ENVELOPE_H
#define ENVELOPE_H

#include "bank.h"

enum
{
  ENVELOPE_TYPE_CARRIER = 0, 
  ENVELOPE_TYPE_MODULATOR
};

enum
{
  ENVELOPE_STATE_ATTACK = 0,
  ENVELOPE_STATE_DECAY_1,
  ENVELOPE_STATE_DECAY_2,
  ENVELOPE_STATE_RELEASE
};

typedef struct envelope
{
  /* type */
  int type;

  /* envelope settings */
  short int attack;
  short int decay_1;
  short int decay_2;
  short int release;
  short int sustain;
  short int rate_ks;
  short int level_ks;

  /* adjustments */
  short int rate_adjustment;
  short int level_adjustment;
  short int ampl_adjustment;
  short int volume_adjustment;

  /* rows */
  int       a_row;
  int       d1_row;
  int       d2_row;
  int       r_row;

  /* decay 1 to decay 2 switch level */
  short int transition_level;

  /* current state, keycode, row */
  int       state;
  short int keycode;
  int       row;

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
short int envelope_setup_all();
short int envelope_reset(int voice_index, int num);

short int envelope_load_patch(int voice_index, int num, 
                              int patch_index, int type);

short int envelope_trigger(int voice_index, int num, int note, int volume, int brightness);
short int envelope_release(int voice_index, int num);

short int envelope_update_all();

short int envelope_generate_tables();

#endif
