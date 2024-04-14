/*******************************************************************************
** boost.h (envelope boost)
*******************************************************************************/

#ifndef BOOST_H
#define BOOST_H

#include "bank.h"

typedef struct boost
{
  /* boost parameters */
  short int amount;

  /* controller inputs */
  short int mod_wheel_input;
  short int aftertouch_input;
  short int exp_pedal_input;

  /* level */
  short int level;
} boost;

/* boost bank */
extern boost G_boost_bank[BANK_NUM_BOOSTS];

/* function declarations */
short int boost_reset_all();

short int boost_load_patch(int instrument_index, int patch_index);

short int boost_update_all();

#endif
