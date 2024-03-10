/*******************************************************************************
** instrument.h (synth instrument)
*******************************************************************************/

#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include "bank.h"
#include "tuning.h"

enum
{
  INSTRUMENT_TYPE_POLY = 0, 
  INSTRUMENT_TYPE_MONO, 
  INSTRUMENT_NUM_TYPES
};

typedef struct instrument
{
  int type;

  int voice_index;

  char pressed_keys[TUNING_NUM_PLAYABLE_NOTES];
  char held_keys[TUNING_NUM_PLAYABLE_NOTES];

  int num_pressed;
  int num_held;

  short int volume;
  short int panning;

  short int note_velocity;

  short int mod_wheel_pos;
  short int aftertouch_pos;
  short int pitch_wheel_pos;

  short int portamento_switch;
  short int arpeggio_switch;
  short int sustain_pedal;
} instrument;

/* instrument bank */
extern instrument G_instrument_bank[BANK_NUM_INSTRUMENTS];

/* function declarations */
short int instrument_reset_all();

short int instrument_load_patch(int instrument_index, 
                                int cart_number, int patch_number);

short int instrument_note_on(int instrument_index, int note);
short int instrument_note_off(int instrument_index, int note);

short int instrument_key_pressed(int instrument_index, int note);
short int instrument_key_released(int instrument_index, int note);

short int instrument_set_note_velocity(int instrument_index, short int vel);

short int instrument_set_mod_wheel_position(int instrument_index, short int pos);
short int instrument_set_aftertouch_position(int instrument_index, short int pos);
short int instrument_set_pitch_wheel_position(int instrument_index, short int pos);

short int instrument_set_portamento_switch(int instrument_index, int state);
short int instrument_set_arpeggio_switch(int instrument_index, int state);
short int instrument_set_sustain_pedal(int instrument_index, int state);

#endif
