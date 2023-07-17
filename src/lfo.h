/*******************************************************************************
** lfo.h (low frequency oscillator)
*******************************************************************************/

#ifndef LFO_H
#define LFO_H

#include "bank.h"

typedef struct lfo
{
  /* lfo parameters */
  short int waveform;
  short int octave;
  short int note;
  short int delay;
  short int sync;
  short int vibrato_mode;

  short int base_vibrato;
  short int base_tremolo;
  short int base_wobble;

  /* mod wheel sensitivity */
  short int mod_wheel_vibrato;
  short int mod_wheel_tremolo;
  short int mod_wheel_wobble;

  /* aftertouch sensitivity */
  short int aftertouch_vibrato;
  short int aftertouch_tremolo;
  short int aftertouch_wobble;

  /* phase, phase increment */
  unsigned int phase;
  unsigned int increment;

  /* noise lfsr */
  unsigned int lfsr;

  /* mod wheel and aftertouch inputs */
  short int mod_wheel_input;
  short int aftertouch_input;

  /* levels */
  short int vibrato_level;
  short int tremolo_level;
  short int wobble_level;
} lfo;

/* lfo bank */
extern lfo G_lfo_bank[BANK_NUM_LFOS];

/* function declarations */
short int lfo_setup_all();
short int lfo_reset(int voice_index);

short int lfo_load_patch(int voice_index, int patch_index);

short int lfo_trigger(int voice_index);

short int lfo_update_all();

short int lfo_generate_tables();

#endif
