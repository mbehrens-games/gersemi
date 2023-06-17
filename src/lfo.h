/*******************************************************************************
** lfo.h (low frequency oscillator)
*******************************************************************************/

#ifndef LFO_H
#define LFO_H

#include "bank.h"

enum
{
  LFO_TYPE_VIBRATO = 0,
  LFO_TYPE_VIBRATO_ALTERNATE,
  LFO_TYPE_TREMOLO,
  LFO_TYPE_WOBBLE_AMPLITUDE,
  LFO_TYPE_WOBBLE_PITCH,
  LFO_TYPE_WOBBLE_PULSE_WIDTH,
  LFO_NUM_TYPES
};

enum
{
  LFO_MODE_TRIANGLE = 0,
  LFO_MODE_SQUARE,
  LFO_MODE_SAW_UP,
  LFO_MODE_SAW_DOWN,
  LFO_MODE_RANDOM_SQUARE,
  LFO_MODE_RANDOM_SAW,
  LFO_NUM_MODES
};

typedef struct lfo
{
  /* type, mode */
  int type;
  int mode;

  /* depth, speed */
  int depth;
  int speed;

  /* wavetable row & index */
  int row;
  int index;

  /* noise lfsr */
  unsigned int lfsr;

  /* phase increment */
  unsigned int increment;

  /* phase */
  unsigned int phase;

  /* level */
  short int level;
} lfo;

/* lfo bank */
extern lfo G_lfo_bank[BANK_NUM_LFOS];

/* function declarations */
short int lfo_setup_all();
short int lfo_reset(int voice_index);

short int lfo_set_type_and_mode(int voice_index, int type, int mode);
short int lfo_set_depth(int voice_index, int depth);
short int lfo_set_speed(int voice_index, int tempo, int speed);

short int lfo_adjust_to_tempo(int voice_index, int tempo);
short int lfo_trigger(int voice_index);

short int lfo_update_all();

short int lfo_generate_tables();

#endif
