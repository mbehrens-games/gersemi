/*******************************************************************************
** peg.h (pitch envelope)
*******************************************************************************/

#ifndef PEG_H
#define PEG_H

#include "bank.h"

typedef struct peg
{
  /* attack, hold, finale levels */
  short int turn_level;
  short int hold_level;
  short int finale_level;

  /* rates */
  int a_rate;
  int d_rate;
  int r_rate;

  /* current stage, rate */
  int stage;
  int rate;

  /* phase increment, phase */
  unsigned int increment;
  unsigned int phase;

  /* level */
  short int level;
} peg;

/* peg bank */
extern peg G_peg_bank[BANK_NUM_PEGS];

/* function declarations */
short int peg_reset_all();

short int peg_load_patch(int voice_index, int patch_index);

short int peg_trigger(int voice_index);
short int peg_release(int voice_index);

short int peg_update_all();

short int peg_generate_tables();

#endif
