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
  short int sync;

  /* delay */
  int delay_period;
  int delay_cycles;

  /* phase, phase increment */
  unsigned int phase;
  unsigned int increment;

  /* noise lfsr */
  unsigned int lfsr;

  /* vibrato */
  short int vibrato_depth;
  short int vibrato_base;
  short int vibrato_mode;

  /* tremolo */
  short int tremolo_depth;
  short int tremolo_base;

  /* controller effects */
  short int mod_wheel_effect;
  short int aftertouch_effect;

  /* controller inputs */
  short int mod_wheel_input;
  short int aftertouch_input;

  /* levels */
  short int vibrato_level;
  short int tremolo_level;
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
