/*******************************************************************************
** bender.c (pitch wheel)
*******************************************************************************/

#include <stdio.h>    /* testing */
#include <stdlib.h>
#include <math.h>

#include "bank.h"
#include "bender.h"
#include "midicont.h"
#include "patch.h"
#include "tuning.h"

/* bender range table */
static short int  S_bender_range_table[PATCH_PITCH_WHEEL_RANGE_NUM_VALUES] = 
                  {  1 * TUNING_NUM_SEMITONE_STEPS, 
                     2 * TUNING_NUM_SEMITONE_STEPS, 
                     3 * TUNING_NUM_SEMITONE_STEPS, 
                     4 * TUNING_NUM_SEMITONE_STEPS, 
                     5 * TUNING_NUM_SEMITONE_STEPS, 
                     6 * TUNING_NUM_SEMITONE_STEPS, 
                     7 * TUNING_NUM_SEMITONE_STEPS, 
                     8 * TUNING_NUM_SEMITONE_STEPS, 
                     9 * TUNING_NUM_SEMITONE_STEPS, 
                    10 * TUNING_NUM_SEMITONE_STEPS, 
                    11 * TUNING_NUM_SEMITONE_STEPS, 
                    12 * TUNING_NUM_SEMITONE_STEPS 
                  };

/* bender bank */
bender G_bender_bank[BANK_NUM_BENDERS];

/*******************************************************************************
** bender_setup_all()
*******************************************************************************/
short int bender_setup_all()
{
  int k;

  /* setup all benders */
  for (k = 0; k < BANK_NUM_VOICES; k++)
    bender_reset(k);

  return 0;
}

/*******************************************************************************
** bender_reset()
*******************************************************************************/
short int bender_reset(int voice_index)
{
  int m;

  bender* b;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  for (m = 0; m < BANK_BOOSTS_PER_VOICE; m++)
  {
    /* obtain bender pointer */
    b = &G_bender_bank[BANK_BOOSTS_PER_VOICE * voice_index + m];

    /* initialize bender variables */
    b->range = 1;

    b->pitch_wheel_input = 0;

    b->level = 0;
  }

  return 0;
}

/*******************************************************************************
** bender_load_patch()
*******************************************************************************/
short int bender_load_patch(int voice_index, int patch_index)
{
  bender* b;
  patch* p;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain patch pointer */
  p = &G_patch_bank[patch_index];

  /* obtain bender pointer */
  b = &G_bender_bank[voice_index];

  /* range */
  if ((p->pitch_wheel_range >= PATCH_PITCH_WHEEL_RANGE_LOWER_BOUND) && 
      (p->pitch_wheel_range <= PATCH_PITCH_WHEEL_RANGE_UPPER_BOUND))
  {
    b->range = p->pitch_wheel_range;
  }
  else
    b->range = PATCH_PITCH_WHEEL_RANGE_LOWER_BOUND;

  return 0;
}

/*******************************************************************************
** bender_update_all()
*******************************************************************************/
short int bender_update_all()
{
  int k;

  bender* b;

  int bender_bound;

  /* update all benders */
  for (k = 0; k < BANK_NUM_VOICES; k++)
  {
    /* obtain bender pointer */
    b = &G_bender_bank[k];

    /* obtain bender upper bound */
    bender_bound = S_bender_range_table[b->range - PATCH_PITCH_WHEEL_RANGE_LOWER_BOUND];

    /* initialize level */
    b->level = 0;

    /* apply pitch wheel */
    b->level += 
      (bender_bound * b->pitch_wheel_input) / MIDI_CONT_PITCH_WHEEL_UPPER_BOUND;

    /* bound level */
    if ((b->pitch_wheel_input > 0) && (b->level > bender_bound))
      b->level = bender_bound;
    else if ((b->pitch_wheel_input < 0) && (b->level < -bender_bound))
      b->level = -bender_bound;
  }

  return 0;
}

