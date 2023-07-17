/*******************************************************************************
** sweep.h (portamento / glissando)
*******************************************************************************/

#ifndef SWEEP_H
#define SWEEP_H

#include "bank.h"

enum
{
  SWEEP_MODE_OFF = 0, 
  SWEEP_MODE_PORTAMENTO_UP, 
  SWEEP_MODE_HALF_STEP_GLISSANDO_UP, 
  SWEEP_MODE_WHOLE_STEP_GLISSANDO_UP, 
  SWEEP_MODE_PORTAMENTO_DOWN, 
  SWEEP_MODE_HALF_STEP_GLISSANDO_DOWN, 
  SWEEP_MODE_WHOLE_STEP_GLISSANDO_DOWN, 
  SWEEP_MODE_PORTAMENTO_TO_NOTE, 
  SWEEP_MODE_HALF_STEP_GLISSANDO_TO_NOTE, 
  SWEEP_MODE_WHOLE_STEP_GLISSANDO_TO_NOTE, 
  SWEEP_NUM_MODES
};

typedef struct sweep
{
  /* mode, speed */
  int mode;
  int speed;

  /* phase increment */
  unsigned int increment;

  /* phase */
  unsigned int phase;

  /* start position, target position */
  short int start;
  short int target;

  /* steps */
  short int step_coarse;
  short int step_fine;

  /* level */
  short int level;
} sweep;

/* sweep bank */
extern sweep G_sweep_bank[BANK_NUM_SWEEPS];

/* function declarations */
short int sweep_setup_all();
short int sweep_reset(int voice_index);

short int sweep_trigger(int voice_index, int old_note, int new_note);

short int sweep_update_all();

#endif
