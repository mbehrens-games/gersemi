/*******************************************************************************
** sweep.h (pitch sweep, duty cycle sweep)
*******************************************************************************/

#ifndef SWEEP_H
#define SWEEP_H

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

/* function declarations */
short int sweep_setup(sweep* sw);

short int sweep_set_mode(sweep* sw, int mode);
short int sweep_set_speed(sweep* sw, int tempo, int speed);

short int sweep_adjust_to_tempo(sweep* sw, int tempo);

short int sweep_trigger(sweep* sw, int old_note, int new_note);

short int sweep_update(sweep* sw);

short int sweep_generate_tables();

#endif
