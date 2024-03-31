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

  /* quantize (sample and hold) */
  unsigned int quantize_phase;
  unsigned int quantize_overflow;

  /* phase, phase increment */
  unsigned int phase;
  unsigned int increment;

  /* noise lfsr */
  unsigned int lfsr;

  /* tempo */
  short int tempo;

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
  short int exp_pedal_effect;

  /* controller inputs */
  short int mod_wheel_input;
  short int aftertouch_input;
  short int exp_pedal_input;

  /* wave values (unadjusted levels) */
  short int vibrato_wave_value;
  short int tremolo_wave_value;

  /* levels */
  short int vibrato_level;
  short int tremolo_level;
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
