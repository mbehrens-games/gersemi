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
short int sweep_setup_all();
short int sweep_reset(int voice_index);

short int sweep_load_patch(int voice_index, int patch_index);

short int sweep_set_tempo(int voice_index, short int tempo);

short int sweep_set_note(int voice_index, int note);
short int sweep_trigger(int voice_index, int note);

short int sweep_update_all();

short int sweep_generate_tables();

#endif
