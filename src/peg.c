/*******************************************************************************
** peg.c (pitch envelope)
*******************************************************************************/

#include <stdio.h>    /* testing */
#include <stdlib.h>
#include <math.h>

#include "bank.h"
#include "clock.h"
#include "midicont.h"
#include "patch.h"
#include "peg.h"
#include "tuning.h"

enum
{
  PEG_STAGE_ATTACK = 0, 
  PEG_STAGE_DECAY, 
  PEG_STAGE_RELEASE, 
  PEG_STAGE_SUSTAINED 
};

#define PEG_MAX_ATTENUATION   4095

#define PEG_SUSTAIN_STEP      16

#define PEG_NUM_OCTAVES       17
#define PEG_RATES_PER_OCTAVE  12

#define PEG_NUM_RATES (PEG_NUM_OCTAVES * PEG_RATES_PER_OCTAVE)

#define PEG_SET_STAGE(name)                                                    \
  e->stage = PEG_STAGE_##name;                                                 \
                                                                               \
  if (e->stage == PEG_STAGE_ATTACK)                                            \
    e->rate = e->a_rate;                                                       \
  else if (e->stage == PEG_STAGE_DECAY)                                        \
    e->rate = e->d_rate;                                                       \
  else if (e->stage == PEG_STAGE_RELEASE)                                      \
    e->rate = e->r_rate;                                                       \
  else if (e->stage == PEG_STAGE_SUSTAINED)                                    \
    e->rate = e->pedal_rate;                                                   \
  else                                                                         \
    e->rate = S_peg_rate_table[0];                                             \
                                                                               \
  /* apply rate keyscaling */                                                  \
  e->rate += e->rate_adjustment;                                               \
                                                                               \
  /* bound rate */                                                             \
  if (e->rate < 0)                                                             \
    e->rate = 0;                                                               \
  else if (e->rate > PEG_NUM_RATES - 1)                                        \
    e->rate = PEG_NUM_RATES - 1;                                               \
                                                                               \
  /* set the phase increment */                                                \
  e->increment = S_envelope_phase_increment_table[e->rate];

/* peg phase increment table */
static unsigned int S_peg_phase_increment_table[PEG_NUM_RATES];

/* rate table */
static short int  S_peg_rate_table[PATCH_PEG_TIME_NUM_VALUES];

/* peg bank */
peg G_peg_bank[BANK_NUM_PEGS];

/*******************************************************************************
** peg_reset_all()
*******************************************************************************/
short int peg_reset_all()
{
  int k;

  peg* e;

  /* setup all pegs */
  for (k = 0; k < BANK_NUM_PEGS; k++)
  {
    /* obtain peg pointer */
    e = &G_peg_bank[k];

    /* initialize peg variables */
    e->turn_level = PATCH_PEG_LEVEL_DEFAULT - PATCH_PEG_LEVEL_LOWER_BOUND;
    e->hold_level = PATCH_PEG_LEVEL_DEFAULT - PATCH_PEG_LEVEL_LOWER_BOUND;
    e->finale_level = PATCH_PEG_LEVEL_DEFAULT - PATCH_PEG_LEVEL_LOWER_BOUND;

    e->a_rate = S_peg_rate_table[PATCH_PEG_TIME_DEFAULT - PATCH_PEG_TIME_LOWER_BOUND];
    e->d_rate = S_peg_rate_table[PATCH_PEG_TIME_DEFAULT - PATCH_PEG_TIME_LOWER_BOUND];
    e->r_rate = S_peg_rate_table[PATCH_PEG_TIME_DEFAULT - PATCH_PEG_TIME_LOWER_BOUND];

    e->stage = PEG_STAGE_RELEASE;
    e->rate = S_peg_rate_table[PATCH_PEG_TIME_DEFAULT - PATCH_PEG_TIME_LOWER_BOUND];

    e->increment = 0;
    e->phase = 0;

    e->level = 0;
  }

  return 0;
}

/*******************************************************************************
** peg_load_patch()
*******************************************************************************/
short int peg_load_patch(int voice_index, int patch_index)
{
  peg* e;
  patch* p;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain patch pointer */
  p = &G_patch_bank[patch_index];

  /* obtain peg pointer */
  e = &G_peg_bank[voice_index];

  /* attack */
  if ((p->peg_attack >= PATCH_PEG_TIME_LOWER_BOUND) && 
      (p->peg_attack <= PATCH_PEG_TIME_UPPER_BOUND))
  {
    e->a_rate = S_peg_rate_table[p->peg_attack - PATCH_PEG_TIME_LOWER_BOUND];
  }
  else
    e->a_rate = S_peg_rate_table[PATCH_PEG_TIME_DEFAULT - PATCH_PEG_TIME_LOWER_BOUND];

  /* decay */
  if ((p->peg_decay >= PATCH_PEG_TIME_LOWER_BOUND) && 
      (p->peg_decay <= PATCH_PEG_TIME_UPPER_BOUND))
  {
    e->a_rate = S_peg_rate_table[p->peg_decay - PATCH_PEG_TIME_LOWER_BOUND];
  }
  else
    e->a_rate = S_peg_rate_table[PATCH_PEG_TIME_DEFAULT - PATCH_PEG_TIME_LOWER_BOUND];

  /* release */
  if ((p->peg_release >= PATCH_PEG_TIME_LOWER_BOUND) && 
      (p->peg_release <= PATCH_PEG_TIME_UPPER_BOUND))
  {
    e->a_rate = S_peg_rate_table[p->peg_release - PATCH_PEG_TIME_LOWER_BOUND];
  }
  else
    e->a_rate = S_peg_rate_table[PATCH_PEG_TIME_DEFAULT - PATCH_PEG_TIME_LOWER_BOUND];

  return 0;
}

/*******************************************************************************
** peg_trigger()
*******************************************************************************/
short int peg_trigger(int voice_index)
{
  peg* e;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain peg pointer */
  e = &G_peg_bank[voice_index];

  e->phase = 0;

  return 0;
}

/*******************************************************************************
** peg_release()
*******************************************************************************/
short int peg_release(int voice_index)
{
  peg* e;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain peg pointer */
  e = &G_peg_bank[voice_index];

  return 0;
}

/*******************************************************************************
** peg_update_all()
*******************************************************************************/
short int peg_update_all()
{
  int k;

  peg* e;

  /* update all pegs */
  for (k = 0; k < BANK_NUM_PEGS; k++)
  {
    /* obtain peg pointer */
    e = &G_peg_bank[k];

    /* update phase */
    e->phase += e->increment;

    /* check if a period was completed */

    /* wraparound phase register (28 bits) */
    if (e->phase > 0xFFFFFFF)
    {

      e->phase &= 0xFFFFFFF;
    }

    e->level = 0;
  }

  return 0;
}

/*******************************************************************************
** peg_generate_tables()
*******************************************************************************/
short int peg_generate_tables()
{
  int m;

  /* phase increment table */
  for (m = 0; m < PEG_NUM_RATES; m++)
  {
    S_peg_phase_increment_table[m] = 
      (int) ((440.0f * TUNING_NUM_SEMITONE_STEPS * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
  }

  return 0;
}

