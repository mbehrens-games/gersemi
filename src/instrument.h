/*******************************************************************************
** instrument.h (synth instrument)
*******************************************************************************/

#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include "bank.h"
#include "tuning.h"

typedef struct instrument
{
  short int voice_index;
  short int last_offset;
  short int polyphony;

  char pressed_keys[TUNING_NUM_PLAYABLE_NOTES];
  char held_keys[TUNING_NUM_PLAYABLE_NOTES];

  int num_pressed;
  int num_held;

  short int volume;
  short int panning;

  short int note_velocity;

  short int mod_wheel_pos;
  short int aftertouch_pos;
  short int exp_pedal_pos;
  short int pitch_wheel_pos;

  short int port_arp_switch;
  short int sustain_pedal;
} instrument;

/* instrument bank */
extern instrument G_instrument_bank[BANK_NUM_INSTRUMENTS];

/* function declarations */
short int instrument_reset_all();

short int instrument_load_patch(int instrument_index, 
                                int cart_index, int patch_index);

short int instrument_note_on(int instrument_index, int note);
short int instrument_note_off(int instrument_index, int note);

short int instrument_key_pressed(int instrument_index, int note);
short int instrument_key_released(int instrument_index, int note);

short int instrument_set_note_velocity(int instrument_index, short int vel);

short int instrument_set_mod_wheel_position(int instrument_index, short int pos);
short int instrument_set_aftertouch_position(int instrument_index, short int pos);
short int instrument_set_exp_pedal_position(int instrument_index, short int pos);
short int instrument_set_pitch_wheel_position(int instrument_index, short int pos);

short int instrument_set_port_arp_switch(int instrument_index, int state);
short int instrument_set_sustain_pedal(int instrument_index, int state);

#endif
