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

  /* rate settings */
  short int attack_row;
  short int decay_1_row;
  short int decay_2_row;
  short int release_row;

  /* key scaling settings */
  short int rate_keyscaling;
  short int level_keyscaling;

  /* current state */
  int state;

  /* current keycode */
  short int keycode;

  /* phase increment table row */
  int row;

  /* phase increment */
  unsigned int increment;

  /* phase */
  unsigned int phase;

  /* decay 1 to decay 2 switch level */
  short int sustain_level;

  /* attenuation */
  short int attenuation;

  /* adjustments */
  short int volume_adjustment;
  short int amp_adjustment;

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
