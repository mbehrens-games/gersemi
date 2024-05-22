/*******************************************************************************
** lfo.h (low frequency oscillator)
*******************************************************************************/

#ifndef LFO_H
#define LFO_H

#include "bank.h"

typedef struct lfo
{
  /* lfo parameters */
  short int type;
  short int waveform;
  short int speed;
  short int sync;
  short int polarity;

  /* delay */
  int delay_cycles;
  int delay_period;

  /* phase, phase increment */
  unsigned int phase;
  unsigned int increment;

  /* tempo */
  short int tempo;

  /* depth, sensitivity */
  short int depth;
  short int max;

  /* levels */
  short int level_base;
  short int level_extra;
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
