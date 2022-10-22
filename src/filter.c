/*******************************************************************************
** filter.c (filters)
*******************************************************************************/

#include <stdlib.h>
#include <math.h>

#include "filter.h"
#include "tuning.h"

/*******************************************************************************
** filter_setup()
*******************************************************************************/
short int filter_setup(filter* fltr, int mode, int fc_index)
{
  int i;

  if (fltr == NULL)
    return 1;

  /* set mode & cutoff frequency index */
  if (mode == FILTER_MODE_LOWPASS)
  {
    fltr->mode = mode;

    if ((fc_index >= 0) && (fc_index < FILTER_NUM_LOWPASS_CUTOFFS))
      fltr->fc_index = fc_index;
    else
      fltr->fc_index = FILTER_LOWPASS_CUTOFF_C8;
  }
  else if (mode == FILTER_MODE_HIGHPASS)
  {
    fltr->mode = mode;

    if ((fc_index >= 0) && (fc_index < FILTER_NUM_HIGHPASS_CUTOFFS))
      fltr->fc_index = fc_index;
    else
      fltr->fc_index = FILTER_HIGHPASS_CUTOFF_A0;
  }
  else
  {
    fltr->mode = FILTER_MODE_LOWPASS;
    fltr->fc_index = FILTER_LOWPASS_CUTOFF_C8;
  }

  /* reset the filter */
  for (i = 0; i < FILTER_NUM_STAGES; i++)
  {
    fltr->s[i] = 0;
    fltr->v[i] = 0;
    fltr->y[i] = 0;
  }

  fltr->level = 0;

  return 0;
}

/*******************************************************************************
** filter_set_index()
*******************************************************************************/
short int filter_set_index(filter* fltr, int fc_index)
{
  if (fltr == NULL)
    return 1;

  if (fltr->mode == FILTER_MODE_LOWPASS)
  {
    if ((fc_index >= 0) && (fc_index < FILTER_NUM_LOWPASS_CUTOFFS))
      fltr->fc_index = fc_index;
    else
      fltr->fc_index = FILTER_LOWPASS_CUTOFF_C8;
  }
  else if (fltr->mode == FILTER_MODE_HIGHPASS)
  {
    if ((fc_index >= 0) && (fc_index < FILTER_NUM_HIGHPASS_CUTOFFS))
      fltr->fc_index = fc_index;
    else
      fltr->fc_index = FILTER_HIGHPASS_CUTOFF_A0;
  }

  return 0;
}

/*******************************************************************************
** filter_reset()
*******************************************************************************/
short int filter_reset(filter* fltr)
{
  int i;

  if (fltr == NULL)
    return 1;

  for (i = 0; i < FILTER_NUM_STAGES; i++)
  {
    fltr->s[i] = 0;
    fltr->v[i] = 0;
    fltr->y[i] = 0;
  }

  fltr->level = 0;

  return 0;
}

/*******************************************************************************
** filter_update()
*******************************************************************************/
short int filter_update(filter* fltr, int input)
{
  int stage_multiplier;

  if (fltr == NULL)
    return 1;

  /* see Vadim Zavalishin's "The Art of VA Filter Design" (p. 77) */
  if (fltr->mode == FILTER_MODE_LOWPASS)
  {
    /* obtain multiplier from table */
    stage_multiplier = 
      G_lowpass_filter_stage_multiplier_table[fltr->fc_index];

    /* integrator 1 */
    fltr->v[0] = ((input - fltr->s[0]) * stage_multiplier) / 32768;
    fltr->y[0] = fltr->v[0] + fltr->s[0];
    fltr->s[0] = fltr->y[0] + fltr->v[0];

    /* integrator 2 */
    fltr->v[1] = ((fltr->y[0] - fltr->s[1]) * stage_multiplier) / 32768;
    fltr->y[1] = fltr->v[1] + fltr->s[1];
    fltr->s[1] = fltr->y[1] + fltr->v[1];

    /* set output level */
    fltr->level = fltr->y[1];
  }
  else if (fltr->mode == FILTER_MODE_HIGHPASS)
  {
    /* obtain multiplier from table */
    stage_multiplier = 
      G_highpass_filter_stage_multiplier_table[fltr->fc_index];

    /* integrator 1 */
    fltr->v[0] = ((input - fltr->s[0]) * stage_multiplier) / 32768;
    fltr->y[0] = fltr->v[0] + fltr->s[0];
    fltr->s[0] = fltr->y[0] + fltr->v[0];

    /* integrator 2 */
    fltr->v[1] = ((input - fltr->y[0] - fltr->s[1]) * stage_multiplier) / 32768;
    fltr->y[1] = fltr->v[1] + fltr->s[1];
    fltr->s[1] = fltr->y[1] + fltr->v[1];

    /* set output level */
    fltr->level = input - fltr->y[0] - fltr->y[1];
  }

  return 0;
}

