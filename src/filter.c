/*******************************************************************************
** filter.c (filters)
*******************************************************************************/

#include <stdlib.h>
#include <math.h>

#include "bank.h"
#include "filter.h"
#include "tuning.h"

/* filter bank */
filter G_filter_bank[BANK_NUM_FILTERS];

/*******************************************************************************
** filter_setup_all()
*******************************************************************************/
short int filter_setup_all()
{
  int k;

  /* setup all filters */
  for (k = 0; k < BANK_NUM_VOICES; k++)
    filter_reset(k);

  return 0;
}

/*******************************************************************************
** filter_reset()
*******************************************************************************/
short int filter_reset(int voice_index)
{
  int k;

  filter* lpf;
  filter* hpf;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain filter pointers */
  lpf = &G_filter_bank[2 * voice_index + 0];
  hpf = &G_filter_bank[2 * voice_index + 1];

  /* set cutoffs */
  lpf->cutoff = FILTER_LOWPASS_CUTOFF_C8;
  hpf->cutoff = FILTER_HIGHPASS_CUTOFF_A0;

  /* reset state */
  lpf->input = 0;
  hpf->input = 0;

  for (k = 0; k < FILTER_NUM_STAGES; k++)
  {
    lpf->s[k] = 0;
    lpf->v[k] = 0;
    lpf->y[k] = 0;

    hpf->s[k] = 0;
    hpf->v[k] = 0;
    hpf->y[k] = 0;
  }

  lpf->level = 0;
  hpf->level = 0;

  return 0;
}

/*******************************************************************************
** filter_set_lowpass_cutoff()
*******************************************************************************/
short int filter_set_lowpass_cutoff(int voice_index, int cutoff)
{
  filter* lpf;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain filter pointer */
  lpf = &G_filter_bank[2 * voice_index + 0];

  if ((cutoff >= 0) && (cutoff < FILTER_NUM_LOWPASS_CUTOFFS))
    lpf->cutoff = cutoff;
  else
    lpf->cutoff = FILTER_LOWPASS_CUTOFF_C8;

  return 0;
}

/*******************************************************************************
** filter_set_highpass_cutoff()
*******************************************************************************/
short int filter_set_highpass_cutoff(int voice_index, int cutoff)
{
  filter* hpf;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain filter pointer */
  hpf = &G_filter_bank[2 * voice_index + 1];

  if ((cutoff >= 0) && (cutoff < FILTER_NUM_HIGHPASS_CUTOFFS))
    hpf->cutoff = cutoff;
  else
    hpf->cutoff = FILTER_HIGHPASS_CUTOFF_A0;

  return 0;
}

/*******************************************************************************
** filter_update_all()
*******************************************************************************/
short int filter_update_all()
{
  int k;

  filter* lpf;
  filter* hpf;

  int stage_multiplier;

  for (k = 0; k < BANK_NUM_VOICES; k++)
  {
    /* obtain filter pointers */
    lpf = &G_filter_bank[2 * k + 0];
    hpf = &G_filter_bank[2 * k + 1];

    /* see Vadim Zavalishin's "The Art of VA Filter Design" (p. 77) */

    /* lowpass filter */

    /* obtain multiplier from table */
    stage_multiplier = 
      G_lowpass_filter_stage_multiplier_table[lpf->cutoff];

    /* integrator 1 */
    lpf->v[0] = ((lpf->input - lpf->s[0]) * stage_multiplier) / 32768;
    lpf->y[0] = lpf->v[0] + lpf->s[0];
    lpf->s[0] = lpf->y[0] + lpf->v[0];

    /* integrator 2 */
    lpf->v[1] = ((lpf->y[0] - lpf->s[1]) * stage_multiplier) / 32768;
    lpf->y[1] = lpf->v[1] + lpf->s[1];
    lpf->s[1] = lpf->y[1] + lpf->v[1];

    /* set output level */
    lpf->level = lpf->y[1];

    /* highpass filter */
    hpf->input = lpf->level;

    /* obtain multiplier from table */
    stage_multiplier = 
      G_highpass_filter_stage_multiplier_table[hpf->cutoff];

    /* integrator 1 */
    hpf->v[0] = ((hpf->input - hpf->s[0]) * stage_multiplier) / 32768;
    hpf->y[0] = hpf->v[0] + hpf->s[0];
    hpf->s[0] = hpf->y[0] + hpf->v[0];

    /* integrator 2 */
    hpf->v[1] = ((hpf->input - hpf->y[0] - hpf->s[1]) * stage_multiplier) / 32768;
    hpf->y[1] = hpf->v[1] + hpf->s[1];
    hpf->s[1] = hpf->y[1] + hpf->v[1];

    /* set output level */
    hpf->level = hpf->input - hpf->y[0] - hpf->y[1];
  }

  return 0;
}

