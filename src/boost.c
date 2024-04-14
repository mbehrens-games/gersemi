/*******************************************************************************
** boost.c (envelope boost)
*******************************************************************************/

#include <stdio.h>    /* testing */
#include <stdlib.h>
#include <math.h>

#include "bank.h"
#include "boost.h"
#include "midicont.h"
#include "patch.h"

#define BOOST_AMOUNT_LEVEL_STEP (1 * 32)

/* boost amount table */
static short int  S_boost_amount_table[PATCH_BOOST_DEPTH_NUM_VALUES] = 
                  { BOOST_AMOUNT_LEVEL_STEP * 1, 
                    BOOST_AMOUNT_LEVEL_STEP * 2, 
                    BOOST_AMOUNT_LEVEL_STEP * 3, 
                    BOOST_AMOUNT_LEVEL_STEP * 4, 
                    BOOST_AMOUNT_LEVEL_STEP * 5, 
                    BOOST_AMOUNT_LEVEL_STEP * 6, 
                    BOOST_AMOUNT_LEVEL_STEP * 7, 
                    BOOST_AMOUNT_LEVEL_STEP * 8, 
                    BOOST_AMOUNT_LEVEL_STEP * 9, 
                    BOOST_AMOUNT_LEVEL_STEP * 10, 
                    BOOST_AMOUNT_LEVEL_STEP * 11, 
                    BOOST_AMOUNT_LEVEL_STEP * 12, 
                    BOOST_AMOUNT_LEVEL_STEP * 13, 
                    BOOST_AMOUNT_LEVEL_STEP * 14, 
                    BOOST_AMOUNT_LEVEL_STEP * 15, 
                    BOOST_AMOUNT_LEVEL_STEP * 16 
                  };

/* boost bank */
boost G_boost_bank[BANK_NUM_BOOSTS];

/*******************************************************************************
** boost_reset_all()
*******************************************************************************/
short int boost_reset_all()
{
  int k;

  boost* b;

  /* reset all boosts */
  for (k = 0; k < BANK_NUM_BOOSTS; k++)
  {
    /* obtain boost pointer */
    b = &G_boost_bank[k];

    /* initialize boost variables */
    b->amount = 
      S_boost_amount_table[PATCH_BOOST_DEPTH_DEFAULT - PATCH_BOOST_DEPTH_LOWER_BOUND];

    b->mod_wheel_input = 0;
    b->aftertouch_input = 0;
    b->exp_pedal_input = 0;

    b->level = 0;
  }

  return 0;
}

/*******************************************************************************
** boost_load_patch()
*******************************************************************************/
short int boost_load_patch(int instrument_index, int patch_index)
{
  boost* b;
  patch* p;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain patch pointer */
  p = &G_patch_bank[patch_index];

  /* obtain boost pointer */
  b = &G_boost_bank[instrument_index];

  /* depth */
  if ((p->boost_depth >= PATCH_BOOST_DEPTH_LOWER_BOUND) && 
      (p->boost_depth <= PATCH_BOOST_DEPTH_UPPER_BOUND))
  {
    b->amount = S_boost_amount_table[p->boost_depth - PATCH_BOOST_DEPTH_LOWER_BOUND];
  }
  else
    b->amount = S_boost_amount_table[PATCH_BOOST_DEPTH_DEFAULT - PATCH_BOOST_DEPTH_LOWER_BOUND];

  return 0;
}

/*******************************************************************************
** boost_update_all()
*******************************************************************************/
short int boost_update_all()
{
  int k;

  boost* b;

  /* update all boosts */
  for (k = 0; k < BANK_NUM_BOOSTS; k++)
  {
    /* obtain boost pointer */
    b = &G_boost_bank[k];

    /* initialize level */
    b->level = 0;

#if 0
    /* apply mod wheel effect */
    if ((b->mod_wheel_effect == PATCH_CONTROLLER_EFFECT_BOOST)            || 
        (b->mod_wheel_effect == PATCH_CONTROLLER_EFFECT_VIB_PLUS_BOOST)   || 
        (b->mod_wheel_effect == PATCH_CONTROLLER_EFFECT_TREM_PLUS_BOOST)  || 
        (b->mod_wheel_effect == PATCH_CONTROLLER_EFFECT_ALL_THREE))
    {
      b->level += 
        (b->amount * b->mod_wheel_input) / MIDI_CONT_MOD_WHEEL_UPPER_BOUND;
    }

    /* apply aftertouch effect */
    if ((b->aftertouch_effect == PATCH_CONTROLLER_EFFECT_BOOST)           || 
        (b->aftertouch_effect == PATCH_CONTROLLER_EFFECT_VIB_PLUS_BOOST)  || 
        (b->aftertouch_effect == PATCH_CONTROLLER_EFFECT_TREM_PLUS_BOOST) || 
        (b->aftertouch_effect == PATCH_CONTROLLER_EFFECT_ALL_THREE))
    {
      b->level += 
        (b->amount * b->aftertouch_input) / MIDI_CONT_AFTERTOUCH_UPPER_BOUND;
    }

    /* apply expression pedal effect */
    if ((b->exp_pedal_effect == PATCH_CONTROLLER_EFFECT_BOOST)           || 
        (b->exp_pedal_effect == PATCH_CONTROLLER_EFFECT_VIB_PLUS_BOOST)  || 
        (b->exp_pedal_effect == PATCH_CONTROLLER_EFFECT_TREM_PLUS_BOOST) || 
        (b->exp_pedal_effect == PATCH_CONTROLLER_EFFECT_ALL_THREE))
    {
      b->level += 
        (b->amount * b->exp_pedal_input) / MIDI_CONT_EXP_PEDAL_UPPER_BOUND;
    }
#endif

    /* bound level */
    if (b->level > b->amount)
      b->level = b->amount;
  }

  return 0;
}

