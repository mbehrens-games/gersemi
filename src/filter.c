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
  lpf->cutoff = PATCH_LOWPASS_CUTOFF_UPPER_BOUND;
  hpf->cutoff = PATCH_HIGHPASS_CUTOFF_LOWER_BOUND;

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
** filter_load_patch()
*******************************************************************************/
short int filter_load_patch(int voice_index, int patch_index)
{
  filter* lpf;
  filter* hpf;

  patch* p;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain filter and patch pointers */
  lpf = &G_filter_bank[2 * voice_index + 0];
  hpf = &G_filter_bank[2 * voice_index + 1];

  p = &G_patch_bank[patch_index];

  /* set lowpass cutoff */
  if ((p->lowpass_cutoff >= PATCH_LOWPASS_CUTOFF_LOWER_BOUND) && 
      (p->lowpass_cutoff <= PATCH_LOWPASS_CUTOFF_UPPER_BOUND))
  {
    lpf->cutoff = p->lowpass_cutoff;
  }
  else
    lpf->cutoff = PATCH_LOWPASS_CUTOFF_UPPER_BOUND;

  /* set highpass cutoff */
  if ((p->highpass_cutoff >= PATCH_HIGHPASS_CUTOFF_LOWER_BOUND) && 
      (p->highpass_cutoff <= PATCH_HIGHPASS_CUTOFF_UPPER_BOUND))
  {
    hpf->cutoff = p->highpass_cutoff;
  }
  else
    hpf->cutoff = PATCH_HIGHPASS_CUTOFF_LOWER_BOUND;

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
      S_lowpass_filter_stage_multiplier_table[lpf->cutoff];

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
      S_highpass_filter_stage_multiplier_table[hpf->cutoff];

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

