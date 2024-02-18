/*******************************************************************************
** instrument.h (synth instrument)
*******************************************************************************/

#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include "bank.h"

enum
{
  INSTRUMENT_TYPE_POLY = 0, 
  INSTRUMENT_TYPE_MONO, 
  INSTRUMENT_TYPE_DRUMS, 
  INSTRUMENT_NUM_TYPES
};

#define INSTRUMENT_POLY_INDEX         0

#define INSTRUMENT_MONO_START_INDEX   1
#define INSTRUMENT_MONO_END_INDEX     8

#define INSTRUMENT_DRUMS_INDEX        9

#define INSTRUMENT_NUM_PRESSED_NOTES  4
#define INSTRUMENT_NUM_SWEEP_NOTES    2

typedef struct instrument
{
  int type;

  int voice_index;

  int pressed_notes[INSTRUMENT_NUM_PRESSED_NOTES];
  int sweep_notes[INSTRUMENT_NUM_SWEEP_NOTES];

  short int volume;
  short int panning;

  short int note_velocity;

  short int mod_wheel_pos;
  short int aftertouch_pos;
  short int pitch_wheel_pos;

  short int portamento_switch;
  short int arpeggio_switch;
  short int sustain_pedal;

  short int legato;
} instrument;

/* instrument bank */
extern instrument G_instrument_bank[BANK_NUM_INSTRUMENTS];

/* function declarations */
short int instrument_setup_all();
short int instrument_reset(int instrument_index);

short int instrument_load_patch(int instrument_index, 
                                int cart_number, int patch_number);

short int instrument_key_on(int instrument_index, int note);
short int instrument_key_off(int instrument_index, int note);

short int instrument_set_note_velocity(int instrument_index, short int vel);

short int instrument_set_mod_wheel_position(int instrument_index, short int pos);
short int instrument_set_aftertouch_position(int instrument_index, short int pos);
short int instrument_set_pitch_wheel_position(int instrument_index, short int pos);

short int instrument_set_portamento_switch(int instrument_index, int state);
short int instrument_set_arpeggio_switch(int instrument_index, int state);
short int instrument_set_sustain_pedal(int instrument_index, int state);

#endif
