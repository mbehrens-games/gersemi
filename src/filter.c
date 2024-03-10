/*******************************************************************************
** filter.c (filters)
*******************************************************************************/

#include <stdlib.h>
#include <math.h>

#include "bank.h"
#include "clock.h"
#include "filter.h"
#include "patch.h"

#define PI      3.14159265358979323846f
#define TWO_PI  6.28318530717958647693f

/* filter coefficient tables */
static int S_lowpass_filter_stage_multiplier_table[4];
static int S_highpass_filter_stage_multiplier_table[4];

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

  /* reset all lowpass filters */
  for (k = 0; k < BANK_NUM_LOWPASS_FILTERS; k++)
  {
    /* obtain lowpass filter pointer */
    f = &G_lowpass_filter_bank[k];

    /* set cutoff */
    f->multiplier = 
      S_lowpass_filter_stage_multiplier_table[PATCH_LOWPASS_CUTOFF_DEFAULT - PATCH_LOWPASS_CUTOFF_LOWER_BOUND];

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
    f->multiplier = 
      S_highpass_filter_stage_multiplier_table[PATCH_HIGHPASS_CUTOFF_DEFAULT - PATCH_HIGHPASS_CUTOFF_LOWER_BOUND];

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

  /* set lowpass cutoff */
  if ((p->lowpass_cutoff >= PATCH_LOWPASS_CUTOFF_LOWER_BOUND) && 
      (p->lowpass_cutoff <= PATCH_LOWPASS_CUTOFF_UPPER_BOUND))
  {
    f->multiplier = 
      S_lowpass_filter_stage_multiplier_table[p->lowpass_cutoff - PATCH_LOWPASS_CUTOFF_LOWER_BOUND];
  }
  else
  {
    f->multiplier = 
      S_lowpass_filter_stage_multiplier_table[PATCH_LOWPASS_CUTOFF_DEFAULT - PATCH_LOWPASS_CUTOFF_LOWER_BOUND];
  }

  /* obtain highpass filter pointer */
  f = &G_highpass_filter_bank[voice_index];

  /* set highpass cutoff */
  if ((p->highpass_cutoff >= PATCH_HIGHPASS_CUTOFF_LOWER_BOUND) && 
      (p->highpass_cutoff <= PATCH_HIGHPASS_CUTOFF_UPPER_BOUND))
  {
    f->multiplier = 
      S_highpass_filter_stage_multiplier_table[p->highpass_cutoff - PATCH_HIGHPASS_CUTOFF_LOWER_BOUND];
  }
  else
  {
    f->multiplier = 
      S_highpass_filter_stage_multiplier_table[PATCH_HIGHPASS_CUTOFF_DEFAULT - PATCH_HIGHPASS_CUTOFF_LOWER_BOUND];
  }

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
  float omega_0_delta_t_over_2;

  /* compute filter coefficients */

  /* see Vadim Zavalishin's "The Art of VA Filter Design" for equations */

  /* pre-warping (section 3.8, p. 62)                               */
  /* (1/2) * new_omega_0 * delta_T = tan((1/2) * omega_0 * delta_T) */

  /* 1st order stage multiplier calculation (section 3.10, p. 76-77)              */
  /* multiplier = ((1/2) * omega_0 * delta_T) / [1 + ((1/2) * omega_0 * delta_T)] */

  /* lowpass filter cutoffs */

  /* E7 (2.6 khz) */
  omega_0_delta_t_over_2 = 
    tanf(0.5f * TWO_PI * 2600.0f * CLOCK_DELTA_T_SECONDS);

  S_lowpass_filter_stage_multiplier_table[0] = 
    (int) (32768 * (omega_0_delta_t_over_2 / (1.0f + omega_0_delta_t_over_2)) + 0.5f);

  /* G7 (3.2 khz) */
  omega_0_delta_t_over_2 = 
    tanf(0.5f * TWO_PI * 3200.0f * CLOCK_DELTA_T_SECONDS);

  S_lowpass_filter_stage_multiplier_table[1] = 
    (int) (32768 * (omega_0_delta_t_over_2 / (1.0f + omega_0_delta_t_over_2)) + 0.5f);

  /* A7 (3.6 khz) */
  omega_0_delta_t_over_2 = 
    tanf(0.5f * TWO_PI * 3600.0f * CLOCK_DELTA_T_SECONDS);

  S_lowpass_filter_stage_multiplier_table[2] = 
    (int) (32768 * (omega_0_delta_t_over_2 / (1.0f + omega_0_delta_t_over_2)) + 0.5f);

  /* C8 (4.2 khz) */
  omega_0_delta_t_over_2 = 
    tanf(0.5f * TWO_PI * 4200.0f * CLOCK_DELTA_T_SECONDS);

  S_lowpass_filter_stage_multiplier_table[3] = 
    (int) (32768 * (omega_0_delta_t_over_2 / (1.0f + omega_0_delta_t_over_2)) + 0.5f);

  /* highpass filter cutoffs */

  /* A0 (27.5 hz) */
  omega_0_delta_t_over_2 = 
    tanf(0.5f * TWO_PI * 27.5f * CLOCK_DELTA_T_SECONDS);

  S_highpass_filter_stage_multiplier_table[0] = 
    (int) (32768 * (omega_0_delta_t_over_2 / (1.0f + omega_0_delta_t_over_2)) + 0.5f);

  /* A1 (55 hz) */
  omega_0_delta_t_over_2 = 
    tanf(0.5f * TWO_PI * 55.0f * CLOCK_DELTA_T_SECONDS);

  S_highpass_filter_stage_multiplier_table[1] = 
    (int) (32768 * (omega_0_delta_t_over_2 / (1.0f + omega_0_delta_t_over_2)) + 0.5f);

  /* A2 (110 hz) */
  omega_0_delta_t_over_2 = 
    tanf(0.5f * TWO_PI * 110.0f * CLOCK_DELTA_T_SECONDS);

  S_highpass_filter_stage_multiplier_table[2] = 
    (int) (32768 * (omega_0_delta_t_over_2 / (1.0f + omega_0_delta_t_over_2)) + 0.5f);

  /* A3 (220 hz) */
  omega_0_delta_t_over_2 = 
    tanf(0.5f * TWO_PI * 220.0f * CLOCK_DELTA_T_SECONDS);

  S_highpass_filter_stage_multiplier_table[3] = 
    (int) (32768 * (omega_0_delta_t_over_2 / (1.0f + omega_0_delta_t_over_2)) + 0.5f);

  return 0;
}

