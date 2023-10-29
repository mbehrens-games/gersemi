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

/* boost depth table */
static short int  S_boost_depth_table[PATCH_EFFECT_DEPTH_NUM_VALUES] = 
                  { 12 * 1, 
                    12 * 2, 
                    12 * 3, 
                    12 * 4, 
                    12 * 5, 
                    12 * 6, 
                    12 * 7, 
                    12 * 8, 
                    12 * 9, 
                    12 * 10, 
                    12 * 11, 
                    12 * 12, 
                    12 * 13, 
                    12 * 14, 
                    12 * 15, 
                    12 * 16 
                  };

/* boost bank */
boost G_boost_bank[BANK_NUM_BOOSTS];

/*******************************************************************************
** boost_setup_all()
*******************************************************************************/
short int boost_setup_all()
{
  int k;

  /* setup all boosts */
  for (k = 0; k < BANK_NUM_VOICES; k++)
    boost_reset(k);

  return 0;
}

/*******************************************************************************
** boost_reset()
*******************************************************************************/
short int boost_reset(int voice_index)
{
  int m;

  boost* b;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  for (m = 0; m < BANK_BOOSTS_PER_VOICE; m++)
  {
    /* obtain boost pointer */
    b = &G_boost_bank[BANK_BOOSTS_PER_VOICE * voice_index + m];

    /* initialize boost variables */
    b->depth = 1;

    b->mod_wheel_effect = PATCH_CONTROLLER_EFFECT_VIBRATO;
    b->aftertouch_effect = PATCH_CONTROLLER_EFFECT_VIBRATO;

    b->mod_wheel_input = 0;
    b->aftertouch_input = 0;

    b->level = 0;
  }

  return 0;
}

/*******************************************************************************
** boost_load_patch()
*******************************************************************************/
short int boost_load_patch(int voice_index, int patch_index)
{
  boost* b;
  patch* p;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain patch pointer */
  p = &G_patch_bank[patch_index];

  /* obtain boost pointer */
  b = &G_boost_bank[voice_index];

  /* depth */
  if ((p->boost_depth >= PATCH_EFFECT_DEPTH_LOWER_BOUND) && 
      (p->boost_depth <= PATCH_EFFECT_DEPTH_UPPER_BOUND))
  {
    b->depth = p->boost_depth;
  }
  else
    b->depth = PATCH_EFFECT_DEPTH_LOWER_BOUND;

  /* controller effects */
  if ((p->mod_wheel_effect >= PATCH_CONTROLLER_EFFECT_LOWER_BOUND) && 
      (p->mod_wheel_effect <= PATCH_CONTROLLER_EFFECT_UPPER_BOUND))
  {
    b->mod_wheel_effect = p->mod_wheel_effect;
  }
  else
    b->mod_wheel_effect = PATCH_CONTROLLER_EFFECT_LOWER_BOUND;

  if ((p->aftertouch_effect >= PATCH_CONTROLLER_EFFECT_LOWER_BOUND) && 
      (p->aftertouch_effect <= PATCH_CONTROLLER_EFFECT_UPPER_BOUND))
  {
    b->aftertouch_effect = p->aftertouch_effect;
  }
  else
    b->aftertouch_effect = PATCH_CONTROLLER_EFFECT_LOWER_BOUND;

  return 0;
}

/*******************************************************************************
** boost_update_all()
*******************************************************************************/
short int boost_update_all()
{
  int k;

  boost* b;

  int boost_bound;

  /* update all boosts */
  for (k = 0; k < BANK_NUM_VOICES; k++)
  {
    /* obtain boost pointer */
    b = &G_boost_bank[k];

    /* obtain boost upper bound */
    boost_bound = S_boost_depth_table[b->depth - PATCH_EFFECT_DEPTH_LOWER_BOUND];

    /* initialize level */
    b->level = 0;

    /* apply mod wheel effect */
    if (b->mod_wheel_effect == PATCH_CONTROLLER_EFFECT_BOOST)
    {
      b->level += 
        (boost_bound * b->mod_wheel_input) / MIDI_CONT_MOD_WHEEL_UPPER_BOUND;
    }

    /* apply aftertouch effect */
    if (b->aftertouch_effect == PATCH_CONTROLLER_EFFECT_BOOST)
    {
      b->level += 
        (boost_bound * b->aftertouch_input) / MIDI_CONT_AFTERTOUCH_UPPER_BOUND;
    }

    /* bound level */
    if (b->level > boost_bound)
      b->level = boost_bound;
  }

  return 0;
}

