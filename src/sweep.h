/*******************************************************************************
** sweep.h (portamento / glissando)
*******************************************************************************/

#ifndef SWEEP_H
#define SWEEP_H

#include "bank.h"

typedef struct sweep
{
  /* sweep parameters */
  short int portamento_mode;
  short int portamento_speed;
  short int portamento_switch;

  /* phase, phase increment */
  unsigned int phase;
  unsigned int increment;

  /* note, offset */
  short int note;
  short int offset;

  /* level */
  short int level;
} sweep;

/* sweep bank */
extern sweep G_sweep_bank[BANK_NUM_SWEEPS];

/* function declarations */
short int sweep_setup_all();
short int sweep_reset(int voice_index);

short int sweep_load_patch(int voice_index, int patch_index);

short int sweep_trigger(int voice_index, int new_note);

short int sweep_update_all();

short int sweep_generate_tables();

#endif
