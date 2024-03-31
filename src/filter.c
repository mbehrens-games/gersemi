/*******************************************************************************
** filter.c (filters)
*******************************************************************************/

#include <stdlib.h>
#include <math.h>

#include "bank.h"
#include "clock.h"
#include "filter.h"
#include "patch.h"
#include "tuning.h"

#define PI      3.14159265358979323846f
#define TWO_PI  6.28318530717958647693f

/* filter coefficient table */
static int S_filter_stage_multiplier_table[TUNING_NUM_INDICES];

/* highpass cutoff table */
static short int S_filter_highpass_cutoff_table[PATCH_HIGHPASS_CUTOFF_NUM_VALUES] = 
  { TUNING_INDEX_A0, 
    TUNING_INDEX_A0 + (1 * 12 + 0) * TUNING_NUM_SEMITONE_STEPS, /* A1 */
    TUNING_INDEX_A0 + (2 * 12 + 0) * TUNING_NUM_SEMITONE_STEPS, /* A2 */
    TUNING_INDEX_A0 + (3 * 12 + 0) * TUNING_NUM_SEMITONE_STEPS  /* A3 */
  };

/* filter bank */
filter G_lowpass_filter_bank[BANK_NUM_LOWPASS_FILTERS];
filter G_highpass_filter_bank[BANK_NUM_HIGHPASS_FILTERS];

/*******************************************************************************
** filter_reset_all()
*******************************************************************************/
short int filter_reset_all()
{
  int k;
  int m;

  filter* f;

  int cutoff_index;

  /* reset all lowpass filters */
  for (k = 0; k < BANK_NUM_LOWPASS_FILTERS; k++)
  {
    /* obtain lowpass filter pointer */
    f = &G_lowpass_filter_bank[k];

    /* set cutoff */
    f->multiplier = 
      S_filter_stage_multiplier_table[TUNING_INDEX_C8];

    /* reset state */
    f->input = 0;

    for (m = 0; m < FILTER_NUM_STAGES; m++)
    {
      f->s[m] = 0;
      f->v[m] = 0;
      f->y[m] = 0;
    }

    f->level = 0;
  }

  /* reset all highpass filters */
  for (k = 0; k < BANK_NUM_HIGHPASS_FILTERS; k++)
  {
    /* obtain highpass filter pointer */
    f = &G_highpass_filter_bank[k];

    /* set cutoff */
    cutoff_index = 
      S_filter_highpass_cutoff_table[PATCH_HIGHPASS_CUTOFF_DEFAULT - PATCH_HIGHPASS_CUTOFF_LOWER_BOUND];

    f->multiplier = 
      S_filter_stage_multiplier_table[cutoff_index];

    /* reset state */
    f->input = 0;

    for (m = 0; m < FILTER_NUM_STAGES; m++)
    {
      f->s[m] = 0;
      f->v[m] = 0;
      f->y[m] = 0;
    }

    f->level = 0;
  }

  return 0;
}

/*******************************************************************************
** filter_load_patch()
*******************************************************************************/
short int filter_load_patch(int voice_index, int patch_index)
{
  filter* f;
  patch* p;

  int cutoff_index;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain patch pointer */
  p = &G_patch_bank[patch_index];

  /* obtain lowpass filter pointer */
  f = &G_lowpass_filter_bank[voice_index];

  /* set lowpass cutoff (update later!) */
  f->multiplier = 
    S_filter_stage_multiplier_table[TUNING_INDEX_C8];

  /* obtain highpass filter pointer */
  f = &G_highpass_filter_bank[voice_index];

  /* set highpass cutoff */
  if ((p->highpass_cutoff >= PATCH_HIGHPASS_CUTOFF_LOWER_BOUND) && 
      (p->highpass_cutoff <= PATCH_HIGHPASS_CUTOFF_UPPER_BOUND))
  {
    cutoff_index = 
      S_filter_highpass_cutoff_table[p->highpass_cutoff - PATCH_HIGHPASS_CUTOFF_LOWER_BOUND];
  }
  else
  {
    cutoff_index = 
      S_filter_highpass_cutoff_table[PATCH_HIGHPASS_CUTOFF_DEFAULT - PATCH_HIGHPASS_CUTOFF_LOWER_BOUND];
  }

  f->multiplier = 
    S_filter_stage_multiplier_table[cutoff_index];

  return 0;
}

/*******************************************************************************
** filter_update_lowpass()
*******************************************************************************/
short int filter_update_lowpass()
{
  int m;

  filter* f;

  for (m = 0; m < BANK_NUM_LOWPASS_FILTERS; m++)
  {
    /* obtain lowpass filter pointer */
    f = &G_lowpass_filter_bank[m];

    /* see Vadim Zavalishin's "The Art of VA Filter Design" (p. 77) */

    /* integrator 1 */
    f->v[0] = ((f->input - f->s[0]) * f->multiplier) / 32768;
    f->y[0] = f->v[0] + f->s[0];
    f->s[0] = f->y[0] + f->v[0];

    /* integrator 2 */
    f->v[1] = ((f->y[0] - f->s[1]) * f->multiplier) / 32768;
    f->y[1] = f->v[1] + f->s[1];
    f->s[1] = f->y[1] + f->v[1];

    /* set output level */
    f->level = f->y[1];
  }

  return 0;
}

/*******************************************************************************
** filter_update_highpass()
*******************************************************************************/
short int filter_update_highpass()
{
  int m;

  filter* f;

  for (m = 0; m < BANK_NUM_HIGHPASS_FILTERS; m++)
  {
    /* obtain highpass filter pointer */
    f = &G_highpass_filter_bank[m];

    /* see Vadim Zavalishin's "The Art of VA Filter Design" (p. 77) */

    /* integrator 1 */
    f->v[0] = ((f->input - f->s[0]) * f->multiplier) / 32768;
    f->y[0] = f->v[0] + f->s[0];
    f->s[0] = f->y[0] + f->v[0];

    /* integrator 2 */
    f->v[1] = ((f->input - f->y[0] - f->s[1]) * f->multiplier) / 32768;
    f->y[1] = f->v[1] + f->s[1];
    f->s[1] = f->y[1] + f->v[1];

    /* set output level */
    f->level = f->input - f->y[0] - f->y[1];
  }

  return 0;
}

/*******************************************************************************
** filter_generate_tables()
*******************************************************************************/
short int filter_generate_tables()
{
  int m;

  double val;
  float omega_0_delta_t_over_2;

  /* compute filter coefficients */

  /* see Vadim Zavalishin's "The Art of VA Filter Design" for equations */

  /* pre-warping (section 3.8, p. 62)                               */
  /* (1/2) * new_omega_0 * delta_T = tan((1/2) * omega_0 * delta_T) */

  /* 1st order stage multiplier calculation (section 3.10, p. 76-77)              */
  /* multiplier = ((1/2) * omega_0 * delta_T) / [1 + ((1/2) * omega_0 * delta_T)] */

  /* note that we compute the lowpass filter coefficients at each index */
  for (m = 0; m < TUNING_NUM_INDICES; m++)
  {
    val = 440.0f * exp(log(2) * ((m - TUNING_INDEX_A4) / 1200.0f));

    omega_0_delta_t_over_2 = 
      tanf(0.5f * TWO_PI * val * CLOCK_DELTA_T_SECONDS);

    S_filter_stage_multiplier_table[m] = 
      (int) (32768 * (omega_0_delta_t_over_2 / (1.0f + omega_0_delta_t_over_2)) + 0.5f);
  }

  return 0;
}

