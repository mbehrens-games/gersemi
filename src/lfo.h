/*******************************************************************************
** lfo.h (vibrato / tremolo)
*******************************************************************************/

#ifndef LFO_H
#define LFO_H

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

/* function declarations */
short int lfo_setup(lfo* l, int type, int mode);

short int lfo_set_depth(lfo* l, int depth);
short int lfo_set_speed(lfo* l, int tempo, int speed);

short int lfo_adjust_to_tempo(lfo* l, int tempo);

short int lfo_trigger(lfo* l);

short int lfo_update(lfo* l);

short int lfo_generate_tables();

#endif
