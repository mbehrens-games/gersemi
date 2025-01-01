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

  short int vibrato_wheel_pos;
  short int tremolo_wheel_pos;
  short int boost_wheel_pos;
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
short int instrument_set_vibrato_wheel_position(int instrument_index, short int pos);
short int instrument_set_tremolo_wheel_position(int instrument_index, short int pos);
short int instrument_set_boost_wheel_position(int instrument_index, short int pos);

#endif
