/*******************************************************************************
** pattern.c (pattern settings)
*******************************************************************************/

#include <stdio.h>  /* testing */
#include <stdlib.h>

#include "bank.h"
#include "pattern.h"

/* pattern bank */
pattern G_pattern_bank[BANK_NUM_PATTERNS];

/*******************************************************************************
** pattern_setup_all()
*******************************************************************************/
short int pattern_setup_all()
{
  int k;

  /* setup all patterns */
  for (k = 0; k < BANK_NUM_PATTERNS; k++)
    pattern_reset(k);

  return 0;
}

/*******************************************************************************
** pattern_reset()
*******************************************************************************/
short int pattern_reset(int pattern_index)
{
  int m;
  int n;

  pattern* p;

  drum_step*        dr_st;
  instrument_step*  in_st;

  /* make sure that the pattern index is valid */
  if (BANK_PATTERN_INDEX_IS_NOT_VALID(pattern_index))
    return 1;

  /* obtain pattern pointer */
  p = &G_pattern_bank[pattern_index];

  /* reset tempos */
  for (n = 0; n < PATTERN_NUM_BEATS; n++)
    p->tempo[n] = 120;

  /* reset swings */
  for (m = 0; m < PATTERN_NUM_MUSIC_SWINGS; m++)
  {
    for (n = 0; n < PATTERN_NUM_BEATS; n++)
      p->swings[m][n] = PATTERN_BLANK;
  }

  /* reset drum steps */
  for (m = 0; m < PATTERN_NUM_DRUM_VOICES; m++)
  {
    for (n = 0; n < PATTERN_NUM_STEPS; n++)
    {
      dr_st = &p->dr_steps[m][n];

      dr_st->note[0] = PATTERN_BLANK;
      dr_st->note[1] = PATTERN_BLANK;
      dr_st->note[2] = PATTERN_BLANK;
      dr_st->note[3] = PATTERN_BLANK;
      dr_st->note[4] = PATTERN_BLANK;
      dr_st->note[5] = PATTERN_BLANK;
    }
  }

  /* reset instrument steps */
  for (m = 0; m < PATTERN_NUM_INSTRUMENTS; m++)
  {
    for (n = 0; n < PATTERN_NUM_STEPS; n++)
    {
      in_st = &p->in_steps[m][n];

      in_st->note[0] = PATTERN_BLANK;
      in_st->note[1] = PATTERN_BLANK;
      in_st->note[2] = PATTERN_BLANK;
      in_st->note[3] = PATTERN_BLANK;
      in_st->key = PATTERN_BLANK;

      in_st->volume = PATTERN_BLANK;

      in_st->mod_wheel_amount = PATTERN_BLANK;
      in_st->aftertouch_amount = PATTERN_BLANK;

      in_st->arp_porta_mode = PATTERN_BLANK;
      in_st->arp_direction_or_porta_switch = PATTERN_BLANK;
      in_st->arp_speed_or_porta_speed = PATTERN_BLANK;
    }
  }

  p->num_bars = 0;

  return 0;
}

