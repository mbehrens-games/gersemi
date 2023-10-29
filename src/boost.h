/*******************************************************************************
** boost.h (envelope boost)
*******************************************************************************/

#ifndef BOOST_H
#define BOOST_H

#include "bank.h"

typedef struct boost
{
  /* boost parameters */
  short int depth;

  /* controller effects */
  short int mod_wheel_effect;
  short int aftertouch_effect;

  /* controller inputs */
  short int mod_wheel_input;
  short int aftertouch_input;

  /* level */
  short int level;
} boost;

/* boost bank */
extern boost G_boost_bank[BANK_NUM_BOOSTS];

/* function declarations */
short int boost_setup_all();
short int boost_reset(int voice_index);

short int boost_load_patch(int voice_index, int patch_index);

short int boost_update_all();

#endif
