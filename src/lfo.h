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
  short int frequency;
  short int sync;

  /* delay */
  int delay_cycles;
  int delay_period;

  /* phase, phase increment */
  unsigned int phase;
  unsigned int increment;

  /* noise lfsr */
  unsigned int lfsr;

  /* tempo */
  short int tempo;

  /* depth, base */
  short int depth;
  short int base;

  /* controller inputs */
  short int mod_wheel_input;
  short int aftertouch_input;
  short int exp_pedal_input;

  /* wave values (unadjusted levels) */
  short int vibrato_wave_value;
  short int env_tremolo_wave_value;
  short int peg_tremolo_wave_value;

  /* levels */
  short int vibrato_level;
  short int env_tremolo_level;
  short int peg_tremolo_level;
} lfo;

/* lfo bank */
extern lfo G_lfo_bank[BANK_NUM_LFOS];

/* function declarations */
short int lfo_reset_all();

short int lfo_load_patch(int voice_index, int patch_index);

short int lfo_set_tempo(int voice_index, short int tempo);

short int lfo_sync_to_key(int voice_index);
short int lfo_sync_to_tempo(int voice_index);

short int lfo_trigger(int voice_index);

short int lfo_update_all();

short int lfo_generate_tables();

#endif
