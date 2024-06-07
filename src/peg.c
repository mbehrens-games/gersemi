/*******************************************************************************
** peg.c (pitch envelope)
*******************************************************************************/

#include <stdlib.h>
#include <math.h>

#include "bank.h"
#include "clock.h"
#include "patch.h"
#include "peg.h"
#include "tuning.h"

enum
{
  PEG_STAGE_ATTACK = 0, 
  PEG_STAGE_DECAY, 
  PEG_STAGE_SUSTAIN, 
  PEG_STAGE_RELEASE 
};

#define PEG_MAX_INDEX 4095

#define PEG_NUM_OCTAVES      13
#define PEG_RATES_PER_OCTAVE 12

#define PEG_NUM_RATES (PEG_NUM_OCTAVES * PEG_RATES_PER_OCTAVE)

#define PEG_SET_STAGE(name)                                                    \
  e->stage = PEG_STAGE_##name;                                                 \
                                                                               \
  if (e->stage == PEG_STAGE_ATTACK)                                            \
    e->rate = e->a_rate;                                                       \
  else if (e->stage == PEG_STAGE_DECAY)                                        \
    e->rate = e->d_rate;                                                       \
  else if (e->stage == PEG_STAGE_SUSTAIN)                                      \
    e->rate = S_peg_rate_table[0];                                             \
  else if (e->stage == PEG_STAGE_RELEASE)                                      \
    e->rate = e->r_rate;                                                       \
  else                                                                         \
    e->rate = S_peg_rate_table[0];                                             \
                                                                               \
  /* bound rate */                                                             \
  if (e->rate < 0)                                                             \
    e->rate = 0;                                                               \
  else if (e->rate > PEG_NUM_RATES - 1)                                        \
    e->rate = PEG_NUM_RATES - 1;                                               \
                                                                               \
  /* set the phase increment */                                                \
  if (e->stage == PEG_STAGE_ATTACK)                                            \
    e->increment = S_peg_attack_phase_increment_table[e->rate];                \
  else                                                                         \
    e->increment = S_peg_decay_phase_increment_table[e->rate];                 \
                                                                               \
  /* reset phase */                                                            \
  e->phase = 0;

/* peg phase increment tables */
static unsigned int S_peg_attack_phase_increment_table[PEG_NUM_RATES];
static unsigned int S_peg_decay_phase_increment_table[PEG_NUM_RATES];

/* rate table */
static short int  S_peg_rate_table[PATCH_PEG_TIME_NUM_VALUES];

/* maximum table */
static short int  S_peg_max_table[PATCH_PEG_LEVEL_NUM_VALUES];

/* peg bank */
peg G_peg_bank[BANK_NUM_PEGS];

/*******************************************************************************
** peg_reset_all()
*******************************************************************************/
short int peg_reset_all()
{
  int k;

  peg* e;

  /* reset all pegs */
  for (k = 0; k < BANK_NUM_PEGS; k++)
  {
    /* obtain peg pointer */
    e = &G_peg_bank[k];

    /* initialize peg variables */
    e->a_rate = S_peg_rate_table[PATCH_PEG_TIME_DEFAULT - PATCH_PEG_TIME_LOWER_BOUND];
    e->d_rate = S_peg_rate_table[PATCH_PEG_TIME_DEFAULT - PATCH_PEG_TIME_LOWER_BOUND];
    e->r_rate = S_peg_rate_table[PATCH_PEG_TIME_DEFAULT - PATCH_PEG_TIME_LOWER_BOUND];

    e->stage = PEG_STAGE_RELEASE;
    e->rate = S_peg_rate_table[PATCH_PEG_TIME_DEFAULT - PATCH_PEG_TIME_LOWER_BOUND];

    e->increment = 0;
    e->phase = 0;

    e->attack_max = S_peg_max_table[PATCH_PEG_LEVEL_DEFAULT - PATCH_PEG_LEVEL_LOWER_BOUND];
    e->finale_max = S_peg_max_table[PATCH_PEG_LEVEL_DEFAULT - PATCH_PEG_LEVEL_LOWER_BOUND];

    e->index = 0;

    e->level = 0;
  }

  return 0;
}

/*******************************************************************************
** peg_load_patch()
*******************************************************************************/
short int peg_load_patch( int voice_index, 
                          int cart_index, int patch_index)
{
  peg* e;

  cart* c;
  patch* p;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* make sure that the cart & patch indices are valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(cart_index))
    return 1;

  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain cart & patch pointers */
  c = &G_cart_bank[cart_index];
  p = &(c->patches[patch_index]);

  /* obtain peg pointer */
  e = &G_peg_bank[voice_index];

  /* attack rate */
  if ((p->peg_attack >= PATCH_PEG_TIME_LOWER_BOUND) && 
      (p->peg_attack <= PATCH_PEG_TIME_UPPER_BOUND))
  {
    e->a_rate = S_peg_rate_table[p->peg_attack - PATCH_PEG_TIME_LOWER_BOUND];
  }
  else
    e->a_rate = S_peg_rate_table[PATCH_PEG_TIME_DEFAULT - PATCH_PEG_TIME_LOWER_BOUND];

  /* decay rate */
  if ((p->peg_decay >= PATCH_PEG_TIME_LOWER_BOUND) && 
      (p->peg_decay <= PATCH_PEG_TIME_UPPER_BOUND))
  {
    e->d_rate = S_peg_rate_table[p->peg_decay - PATCH_PEG_TIME_LOWER_BOUND];
  }
  else
    e->d_rate = S_peg_rate_table[PATCH_PEG_TIME_DEFAULT - PATCH_PEG_TIME_LOWER_BOUND];

  /* release rate */
  if ((p->peg_release >= PATCH_PEG_TIME_LOWER_BOUND) && 
      (p->peg_release <= PATCH_PEG_TIME_UPPER_BOUND))
  {
    e->r_rate = S_peg_rate_table[p->peg_release - PATCH_PEG_TIME_LOWER_BOUND];
  }
  else
    e->r_rate = S_peg_rate_table[PATCH_PEG_TIME_DEFAULT - PATCH_PEG_TIME_LOWER_BOUND];

  /* maximum */
  if ((p->peg_maximum >= PATCH_PEG_LEVEL_LOWER_BOUND) && 
      (p->peg_maximum <= PATCH_PEG_LEVEL_UPPER_BOUND))
  {
    e->attack_max = S_peg_max_table[p->peg_maximum - PATCH_PEG_LEVEL_LOWER_BOUND];
  }
  else
    e->attack_max = S_peg_max_table[PATCH_PEG_LEVEL_DEFAULT - PATCH_PEG_LEVEL_LOWER_BOUND];

  /* finale */
  if ((p->peg_finale >= PATCH_PEG_LEVEL_LOWER_BOUND) && 
      (p->peg_finale <= PATCH_PEG_LEVEL_UPPER_BOUND))
  {
    e->finale_max = S_peg_max_table[p->peg_finale - PATCH_PEG_LEVEL_LOWER_BOUND];
  }
  else
    e->finale_max = S_peg_max_table[PATCH_PEG_LEVEL_DEFAULT - PATCH_PEG_LEVEL_LOWER_BOUND];

  return 0;
}

/*******************************************************************************
** peg_note_on()
*******************************************************************************/
short int peg_note_on(int voice_index)
{
  peg* e;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain peg pointer */
  e = &G_peg_bank[voice_index];

  /* set the peg to its initial stage */
  PEG_SET_STAGE(ATTACK)

  return 0;
}

/*******************************************************************************
** peg_note_off()
*******************************************************************************/
short int peg_note_off(int voice_index)
{
  peg* e;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain peg pointer */
  e = &G_peg_bank[voice_index];

  /* if this peg is already released, return */
  if (e->stage == PEG_STAGE_RELEASE)
    return 0;

  /* set peg to the release stage */
  PEG_SET_STAGE(RELEASE)

  return 0;
}

/*******************************************************************************
** peg_update_all()
*******************************************************************************/
short int peg_update_all()
{
  int k;

  peg* e;

  short int periods;

  /* update all pegs */
  for (k = 0; k < BANK_NUM_PEGS; k++)
  {
    /* obtain peg pointer */
    e = &G_peg_bank[k];

    /* update phase */
    e->phase += e->increment;

    /* check if a period was completed */
    if (e->phase > 0xFFFFFFF)
    {
      periods = (e->phase >> 28) & 0x0F;

      e->phase &= 0xFFFFFFF;
    }
    else
      periods = 0;

    /* if a period has elapsed, update the peg */
    while (periods > 0)
    {
      periods -= 1;

      /* the pitch envelope starts at internal index 0  */
      /* in the attack phase, the index goes up to MAX  */
      /* in the decay phase, the index goes back to 0   */
      /* in the sustain phase, it stays at 0            */
      /* in the release phase, it goes down to -MAX     */

      /* adjust index */
      if (e->stage == PEG_STAGE_ATTACK)
        e->index += 1;
      else if (e->stage == PEG_STAGE_DECAY)
        e->index -= 1;
      else if (e->stage == PEG_STAGE_SUSTAIN)
        e->index += 0;
      else if (e->stage == PEG_STAGE_RELEASE)
        e->index -= 1;

      /* bound index */
      if (e->index < -PEG_MAX_INDEX)
        e->index = -PEG_MAX_INDEX;
      else if (e->index > PEG_MAX_INDEX)
        e->index = PEG_MAX_INDEX;

      /* change stage if necessary */
      if ((e->stage == PEG_STAGE_ATTACK) && 
          (e->index >= PEG_MAX_INDEX))
      {
        e->index = PEG_MAX_INDEX;

        PEG_SET_STAGE(DECAY)
      }
      else if ( (e->stage == PEG_STAGE_DECAY) && 
                (e->index <= 0))
      {
        e->index = 0;

        PEG_SET_STAGE(SUSTAIN)
      }
    }

    /* update level */
    if (e->index >= 0)
      e->level = (e->index * e->attack_max) / PEG_MAX_INDEX;
    else if (e->index < 0)
      e->level = (-e->index * e->finale_max) / PEG_MAX_INDEX;
    else
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
  int n;

  int quotient;
  int remainder;

  float base;
  float freq;

  int center;
  int bound;

  /* rate table */
  for ( m = PATCH_PEG_TIME_LOWER_BOUND; 
        m <= PATCH_PEG_TIME_UPPER_BOUND; 
        m++)
  {
    /* there are 8 times for each octave.     */
    /* the octaves are numbered from 0 to 13. */

    /* we set up the calculation here so that the */
    /* times 1-8 map to the octave numbered 13.   */
    quotient =  ((PATCH_PEG_TIME_UPPER_BOUND - m + 4) / 8) + 1;
    remainder = (PATCH_PEG_TIME_UPPER_BOUND - m + 4) % 8;

    S_peg_rate_table[m - PATCH_PEG_TIME_LOWER_BOUND] = 12 * quotient;

    if (remainder == 0)
      S_peg_rate_table[m - PATCH_PEG_TIME_LOWER_BOUND] += 0;
    else if (remainder == 1)
      S_peg_rate_table[m - PATCH_PEG_TIME_LOWER_BOUND] += 2;
    else if (remainder == 2)
      S_peg_rate_table[m - PATCH_PEG_TIME_LOWER_BOUND] += 4;
    else if (remainder == 3)
      S_peg_rate_table[m - PATCH_PEG_TIME_LOWER_BOUND] += 6;
    else if (remainder == 4)
      S_peg_rate_table[m - PATCH_PEG_TIME_LOWER_BOUND] += 7;
    else if (remainder == 5)
      S_peg_rate_table[m - PATCH_PEG_TIME_LOWER_BOUND] += 8;
    else if (remainder == 6)
      S_peg_rate_table[m - PATCH_PEG_TIME_LOWER_BOUND] += 10;
    else
      S_peg_rate_table[m - PATCH_PEG_TIME_LOWER_BOUND] += 11;
  }

  /* phase increment tables  */

  /* for the decay stage, the fastest rate should have a fall time  */
  /* of ~8 ms. thus with 13 octaves, the lowest rate is ~64 s.      */
  /* so, the base frequency is (1 / 64) * 4095, where 4095 is the   */
  /* number of updates per fall time (with a 12 bit index).         */
  base = PEG_MAX_INDEX / 64.0f;

  for (n = 0; n < PEG_NUM_OCTAVES; n++)
  {
    for (m = 0; m < PEG_RATES_PER_OCTAVE; m++)
    {
      freq = base * pow(2, (12 * n + m) / 12.0f);

      S_peg_decay_phase_increment_table[ n * PEG_RATES_PER_OCTAVE + m] = 
        (int) ((freq * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
    }
  }

  /* for the attack stage, the fastest rate should have a rise time */
  /* of ~4 ms. thus with 13 octaves, the lowest rate is ~32 s.      */
  base = PEG_MAX_INDEX / 32.0f;

  for (n = 0; n < PEG_NUM_OCTAVES; n++)
  {
    for (m = 0; m < PEG_RATES_PER_OCTAVE; m++)
    {
      freq = base * pow(2, (12 * n + m) / 12.0f);

      S_peg_attack_phase_increment_table[ n * PEG_RATES_PER_OCTAVE + m] = 
        (int) ((freq * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
    }
  }

  /* max table */
  center = (PATCH_PEG_LEVEL_UPPER_BOUND + PATCH_PEG_LEVEL_LOWER_BOUND) / 2;
  bound = (PATCH_PEG_LEVEL_UPPER_BOUND - PATCH_PEG_LEVEL_LOWER_BOUND) / 2;

  S_peg_max_table[center - PATCH_PEG_LEVEL_LOWER_BOUND] = 0;

  for (m = 1; m <= bound; m++)
  {
    S_peg_max_table[center + m - PATCH_PEG_LEVEL_LOWER_BOUND] = m * ((4 * 12 * TUNING_NUM_SEMITONE_STEPS) / bound);

    S_peg_max_table[center - m - PATCH_PEG_LEVEL_LOWER_BOUND] = 
      -S_peg_max_table[center + m - PATCH_PEG_LEVEL_LOWER_BOUND];
  }

  return 0;
}

