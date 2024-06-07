/*******************************************************************************
** sweep.h (portamento / glissando)
*******************************************************************************/

#ifndef SWEEP_H
#define SWEEP_H

#include "bank.h"

typedef struct sweep
{
  /* sweep parameters */
  short int mode;
  short int legato;
  short int speed;

  /* tempo */
  short int tempo;

  /* phase, phase increment */
  unsigned int phase;
  unsigned int increment;

  /* note, offsets */
  short int note;
  short int target;
  short int offset;

  /* switch */
  short int on_switch;

  /* level */
  short int level;
} sweep;

/* sweep bank */
extern sweep G_sweep_bank[BANK_NUM_SWEEPS];

/* function declarations */
short int sweep_reset_all();

short int sweep_load_patch( int instrument_index, 
                            int cart_index, int patch_index);

short int sweep_set_speed(int instrument_index, short int speed);

short int sweep_set_switch(int instrument_index, short int state);
short int sweep_set_tempo(int instrument_index, short int tempo);

short int sweep_tempo_sync(int instrument_index);

short int sweep_key_pressed(int instrument_index, int note);
short int sweep_key_released(int instrument_index, int note);

short int sweep_update_all();

short int sweep_generate_tables();

#endif
