/*******************************************************************************
** highpass.c (highpass filter)
*******************************************************************************/

#include <stdlib.h>
#include <math.h>

#include "bank.h"
#include "clock.h"
#include "highpass.h"
#include "patch.h"
#include "tuning.h"

#define PI      3.14159265358979323846f
#define TWO_PI  6.28318530717958647693f

/* cutoff note table */
static short int S_highpass_cutoff_note_table[PATCH_HIGHPASS_CUTOFF_NUM_VALUES] = 
  { TUNING_NOTE_A0 + 0 * 12,  /* A0 */
    TUNING_NOTE_A0 + 1 * 12,  /* A1 */
    TUNING_NOTE_A0 + 2 * 12,  /* A2 */
    TUNING_NOTE_A0 + 3 * 12   /* A3 */
  };

/* filter coefficient table */
static int S_highpass_stage_multiplier_table[PATCH_HIGHPASS_CUTOFF_NUM_VALUES];

/* highpass bank */
highpass G_highpass_bank[BANK_NUM_HIGHPASSES];

/*******************************************************************************
** highpass_reset_all()
*******************************************************************************/
short int highpass_reset_all()
{
  int k;
  int m;

  highpass* f;

  /* reset all highpass filters */
  for (k = 0; k < BANK_NUM_HIGHPASSES; k++)
  {
    /* obtain highpass pointer */
    f = &G_highpass_bank[k];

    /* set cutoff */
    f->cutoff = PATCH_HIGHPASS_CUTOFF_DEFAULT;

    /* reset state */
    f->input = 0;

    for (m = 0; m < HIGHPASS_NUM_STAGES; m++)
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
** highpass_load_patch()
*******************************************************************************/
short int highpass_load_patch(int voice_index, int patch_index)
{
  highpass* f;
  patch* p;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain patch pointer */
  p = &G_patch_bank[patch_index];

  /* obtain highpass pointer */
  f = &G_highpass_bank[voice_index];

  /* set cutoff */
  if ((p->highpass_cutoff >= PATCH_HIGHPASS_CUTOFF_LOWER_BOUND) && 
      (p->highpass_cutoff <= PATCH_HIGHPASS_CUTOFF_UPPER_BOUND))
  {
    f->cutoff = p->highpass_cutoff;
  }
  else
    f->cutoff = PATCH_HIGHPASS_CUTOFF_DEFAULT;

  return 0;
}

/*******************************************************************************
** highpass_update_all()
*******************************************************************************/
short int highpass_update_all()
{
  int m;

  highpass* f;

  int multiplier;

  for (m = 0; m < BANK_NUM_HIGHPASSES; m++)
  {
    /* obtain highpass pointer */
    f = &G_highpass_bank[m];

    /* see Vadim Zavalishin's "The Art of VA Filter Design" (p. 77) */
    multiplier = S_highpass_stage_multiplier_table[f->cutoff];

    /* integrator 1 */
    f->v[0] = ((f->input - f->s[0]) * multiplier) / 32768;
    f->y[0] = f->v[0] + f->s[0];
    f->s[0] = f->y[0] + f->v[0];

    /* integrator 2 */
    f->v[1] = ((f->input - f->y[0] - f->s[1]) * multiplier) / 32768;
    f->y[1] = f->v[1] + f->s[1];
    f->s[1] = f->y[1] + f->v[1];

    /* set output level */
    f->level = f->input - f->y[0] - f->y[1];
  }

  return 0;
}

/*******************************************************************************
** highpass_generate_tables()
*******************************************************************************/
short int highpass_generate_tables()
{
  int m;

  double val;
  float omega_0_delta_t_over_2;

  /* compute highpass coefficients */

  /* see Vadim Zavalishin's "The Art of VA Filter Design" for equations */

  /* pre-warping (section 3.8, p. 62)                               */
  /* (1/2) * new_omega_0 * delta_T = tan((1/2) * omega_0 * delta_T) */

  /* 1st order stage multiplier calculation (section 3.10, p. 76-77)              */
  /* multiplier = ((1/2) * omega_0 * delta_T) / [1 + ((1/2) * omega_0 * delta_T)] */

  /* note that we compute the lowpass filter coefficients at each note */
  for (m = 0; m < PATCH_HIGHPASS_CUTOFF_NUM_VALUES; m++)
  {
    val = 440.0f * exp(log(2) * ((S_highpass_cutoff_note_table[m] - TUNING_NOTE_A4) / 12.0f));

    omega_0_delta_t_over_2 = 
      tanf(0.5f * TWO_PI * val * CLOCK_DELTA_T_SECONDS);

    S_highpass_stage_multiplier_table[m] = 
      (int) (32768 * (omega_0_delta_t_over_2 / (1.0f + omega_0_delta_t_over_2)) + 0.5f);
  }

  return 0;
}

