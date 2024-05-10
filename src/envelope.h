/*******************************************************************************
** envelope.h (envelope)
*******************************************************************************/

#ifndef ENVELOPE_H
#define ENVELOPE_H

#include "bank.h"

typedef struct envelope
{
  /* keyscaling */
  short int ks_rate_fraction;
  short int ks_level_fraction;

  short int ks_rate_adjustment;
  short int ks_level_adjustment;

  /* note offset */
  short int note_offset;

  /* sustain level */
  short int sustain_level;

  /* rates */
  int a_rate;
  int d_rate;
  int r_rate;

  /* current stage, rate */
  int stage;
  int rate;

  /* phase increment, phase */
  unsigned int increment;
  unsigned int phase;

  /* attenuation */
  short int attenuation;

  /* tremolo, boost, velocity */
  short int tremolo_base;
  short int tremolo_extra;

  short int boost_max;
  short int velocity_max;

  short int tremolo_adjustment;
  short int boost_adjustment;
  short int velocity_adjustment;

  /* routing */
  unsigned char routing;

  /* midi controller positions */
  short int mod_wheel_pos;
  short int aftertouch_pos;
  short int exp_pedal_pos;

  /* volume, amplitude */
  short int volume_adjustment;
  short int amplitude_adjustment;

  /* level (current attenuation + adjustments) */
  short int level;
} envelope;

/* envelope bank */
extern envelope G_envelope_bank[BANK_NUM_ENVELOPES];

/* function declarations */
short int envelope_reset_all();

short int envelope_load_patch(int voice_index, int patch_index);

short int envelope_set_note(int voice_index, int note, int vel);

short int envelope_trigger(int voice_index);
short int envelope_release(int voice_index);

short int envelope_update_all();

short int envelope_generate_tables();

#endif
