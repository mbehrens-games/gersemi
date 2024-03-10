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

  /* phase, phase increment */
  unsigned int phase;
  unsigned int increment;

  /* note, offset */
  short int note;
  short int offset;

  /* tempo */
  short int tempo;

  /* level */
  short int level;
} sweep;

/* sweep bank */
extern sweep G_sweep_bank[BANK_NUM_SWEEPS];

/* function declarations */
short int sweep_reset_all();

short int sweep_load_patch(int instrument_index, int patch_index);

short int sweep_set_tempo(int instrument_index, short int tempo);

short int sweep_set_note(int instrument_index, int note);
short int sweep_trigger(int instrument_index, int note);

short int sweep_update_all();

short int sweep_generate_tables();

#endif
