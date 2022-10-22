/*******************************************************************************
** linear.h (linear envelope)
*******************************************************************************/

#ifndef LINEAR_H
#define LINEAR_H

enum
{
  LINEAR_TYPE_PITCH = 0,
  LINEAR_TYPE_PULSE_WIDTH,
  LINEAR_NUM_TYPES
};

enum
{
  LINEAR_MODE_CONSTANT = 0,
  LINEAR_MODE_OUTWARD_HOLD_AT_END,
  LINEAR_MODE_OUTWARD_HOLD_AT_START,
  LINEAR_MODE_OUTWARD_REPEAT,
  LINEAR_MODE_OUTWARD_FLIP,
  LINEAR_MODE_INWARD_HOLD_AT_END,
  LINEAR_MODE_INWARD_HOLD_AT_START,
  LINEAR_MODE_INWARD_REPEAT,
  LINEAR_MODE_INWARD_FLIP,
  LINEAR_NUM_MODES
};

typedef struct linear
{
  /* type, mode, rate */
  int type;
  int mode;
  int rate;

  /* keyscaling */
  int keyscaling;

  /* wavetable row & index */
  int row;
  int index;

  /* alternate mode flag */
  int flag;

  /* phase increment */
  unsigned int increment;

  /* phase */
  unsigned int phase;

  /* level */
  int level;
} linear;

/* function declarations */
short int linear_setup(linear* l, int type, 
                                  int mode, 
                                  int rate, 
                                  int keyscaling);

short int linear_trigger(linear* l, int note, int brightness);

short int linear_update(linear* l);

short int linear_generate_tables();

#endif
