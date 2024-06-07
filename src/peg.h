/*******************************************************************************
** peg.h (pitch envelope)
*******************************************************************************/

#ifndef PEG_H
#define PEG_H

#include "bank.h"

typedef struct peg
{
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

  /* max levels */
  short int attack_max;
  short int finale_max;

  /* current index */
  int index;

  /* level */
  int level;
} peg;

/* peg bank */
extern peg G_peg_bank[BANK_NUM_PEGS];

/* function declarations */
short int peg_reset_all();

short int peg_load_patch( int voice_index, 
                          int cart_index, int patch_index);

short int peg_note_on(int voice_index);
short int peg_note_off(int voice_index);

short int peg_update_all();

short int peg_generate_tables();

#endif
