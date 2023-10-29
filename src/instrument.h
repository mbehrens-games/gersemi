/*******************************************************************************
** instrument.h (synth instrument)
*******************************************************************************/

#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include "bank.h"

enum
{
  INSTRUMENT_LAYOUT_1_POLY_8_MONO = 0,
  INSTRUMENT_LAYOUT_2_POLY_4_MONO, 
  INSTRUMENT_NUM_LAYOUTS
};

enum
{
  INSTRUMENT_TYPE_POLY = 0, 
  INSTRUMENT_TYPE_MONO, 
  INSTRUMENT_TYPE_DRUMS, 
  INSTRUMENT_TYPE_INACTIVE, 
  INSTRUMENT_NUM_TYPES
};

#define INSTRUMENT_NUM_PRESSED_NOTES  4
#define INSTRUMENT_NUM_RELEASED_NOTES 4

typedef struct instrument
{
  int type;

  int voice_index;

  int pressed_notes[INSTRUMENT_NUM_PRESSED_NOTES];
  int released_notes[INSTRUMENT_NUM_RELEASED_NOTES];

  short int volume;
  short int panning;

  short int velocity;

  short int mod_wheel_pos;
  short int aftertouch_pos;
  short int pitch_wheel_pos;

  short int port_arp_switch;
  short int sustain_pedal;
} instrument;

/* instrument bank */
extern instrument G_instrument_bank[BANK_NUM_INSTRUMENTS];

/* function declarations */
short int instrument_setup_all();
short int instrument_reset(int instrument_index);

short int instrument_set_layout(int layout);

short int instrument_load_patch(int instrument_index, int patch_index);

short int instrument_key_on(int instrument_index, int note);
short int instrument_key_off(int instrument_index, int note);

short int instrument_set_mod_wheel_position(int instrument_index, short int pos);
short int instrument_set_aftertouch_position(int instrument_index, short int pos);
short int instrument_set_pitch_wheel_position(int instrument_index, short int pos);

short int instrument_set_port_arp_switch(int instrument_index, short int state);
short int instrument_set_sustain_pedal(int instrument_index, short int state);

#endif
