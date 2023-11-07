/*******************************************************************************
** bender.h (pitch wheel)
*******************************************************************************/

#ifndef BENDER_H
#define BENDER_H

#include "bank.h"

typedef struct bender
{
  /* bender parameters */
  short int mode;
  short int range;

  /* controller inputs */
  short int pitch_wheel_input;

  /* level */
  short int level;
} bender;

/* bender bank */
extern bender G_bender_bank[BANK_NUM_BENDERS];

/* function declarations */
short int bender_setup_all();
short int bender_reset(int voice_index);

short int bender_load_patch(int voice_index, int patch_index);

short int bender_update_all();

#endif
