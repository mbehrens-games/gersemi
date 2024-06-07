/*******************************************************************************
** filter.c (highpass & lowpass filters)
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

/* cutoff_index note tables */
static short int S_highpass_cutoff_note_table[PATCH_HIGHPASS_CUTOFF_NUM_VALUES] = 
  { TUNING_NOTE_A0 + 0 * 12 + 0,  /* A0 */
    TUNING_NOTE_A0 + 1 * 12 + 0,  /* A1 */
    TUNING_NOTE_A0 + 2 * 12 + 0,  /* A2 */
    TUNING_NOTE_A0 + 3 * 12 + 0   /* A3 */
  };

static short int S_lowpass_cutoff_note_table[PATCH_LOWPASS_CUTOFF_NUM_VALUES] = 
  { TUNING_NOTE_C4 + 3 * 12 + 4,  /* E7 */
    TUNING_NOTE_C4 + 3 * 12 + 7,  /* G7 */
    TUNING_NOTE_C4 + 3 * 12 + 9,  /* A7 */
    TUNING_NOTE_C4 + 4 * 12 + 0   /* C8 */
  };

/* filter coefficient tables */
static int S_highpass_stage_multiplier_table[PATCH_HIGHPASS_CUTOFF_NUM_VALUES];
static int S_lowpass_stage_multiplier_table[PATCH_LOWPASS_CUTOFF_NUM_VALUES];

/* filter bank */
filter G_highpass_filter_bank[BANK_NUM_FILTER_SETS];
filter G_lowpass_filter_bank[BANK_NUM_FILTER_SETS];

/*******************************************************************************
** filter_reset_all()
*******************************************************************************/
short int filter_reset_all()
{
  int k;
  int m;

  filter* hpf;
  filter* lpf;

  /* reset all filters */
  for (k = 0; k < BANK_NUM_FILTER_SETS; k++)
  {
    /* obtain filter pointers */
    hpf = &G_highpass_filter_bank[k];
    lpf = &G_lowpass_filter_bank[k];

    /* reset highpass filter */

    /* set cutoff */
    hpf->cutoff_index = PATCH_HIGHPASS_CUTOFF_DEFAULT;

    /* reset state */
    hpf->input = 0;

    for (m = 0; m < FILTER_NUM_STAGES; m++)
    {
      hpf->s[m] = 0;
      hpf->v[m] = 0;
      hpf->y[m] = 0;
    }

    hpf->level = 0;

    /* reset lowpass filter */

    /* set cutoff */
    lpf->cutoff_index = PATCH_HIGHPASS_CUTOFF_DEFAULT;

    /* reset state */
    lpf->input = 0;

    for (m = 0; m < FILTER_NUM_STAGES; m++)
    {
      lpf->s[m] = 0;
      lpf->v[m] = 0;
      lpf->y[m] = 0;
    }

    lpf->level = 0;
  }

  return 0;
}

/*******************************************************************************
** filter_load_patch()
*******************************************************************************/
short int filter_load_patch(int voice_index, 
                            int cart_index, int patch_index)
{
  filter* hpf;
  filter* lpf;

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

  /* obtain filter pointers */
  hpf = &G_highpass_filter_bank[voice_index];
  lpf = &G_lowpass_filter_bank[voice_index];

  /* highpass cutoff */
  if ((p->highpass_cutoff >= PATCH_HIGHPASS_CUTOFF_LOWER_BOUND) && 
      (p->highpass_cutoff <= PATCH_HIGHPASS_CUTOFF_UPPER_BOUND))
  {
    hpf->cutoff_index = p->highpass_cutoff - PATCH_HIGHPASS_CUTOFF_LOWER_BOUND;
  }
  else
    hpf->cutoff_index = PATCH_HIGHPASS_CUTOFF_DEFAULT - PATCH_HIGHPASS_CUTOFF_LOWER_BOUND;

  /* lowpass cutoff */
  if ((p->lowpass_cutoff >= PATCH_LOWPASS_CUTOFF_LOWER_BOUND) && 
      (p->lowpass_cutoff <= PATCH_LOWPASS_CUTOFF_UPPER_BOUND))
  {
    lpf->cutoff_index = p->lowpass_cutoff - PATCH_LOWPASS_CUTOFF_LOWER_BOUND;
  }
  else
    lpf->cutoff_index = PATCH_LOWPASS_CUTOFF_DEFAULT - PATCH_LOWPASS_CUTOFF_LOWER_BOUND;

  return 0;
}

/*******************************************************************************
** filter_update_all()
*******************************************************************************/
short int filter_update_all()
{
  int m;

  filter* hpf;
  filter* lpf;

  int multiplier;

  for (m = 0; m < BANK_NUM_FILTER_SETS; m++)
  {
    /* obtain filter pointers */
    hpf = &G_highpass_filter_bank[m];
    lpf = &G_lowpass_filter_bank[m];

    /* see Vadim Zavalishin's "The Art of VA Filter Design" (p. 77) */

    /* apply highpass filter */
    multiplier = S_highpass_stage_multiplier_table[hpf->cutoff_index];

    /* integrator 1 */
    hpf->v[0] = ((hpf->input - hpf->s[0]) * multiplier) / 32768;
    hpf->y[0] = hpf->v[0] + hpf->s[0];
    hpf->s[0] = hpf->y[0] + hpf->v[0];

    /* integrator 2 */
    hpf->v[1] = ((hpf->input - hpf->y[0] - hpf->s[1]) * multiplier) / 32768;
    hpf->y[1] = hpf->v[1] + hpf->s[1];
    hpf->s[1] = hpf->y[1] + hpf->v[1];

    /* set highpass output level */
    hpf->level = hpf->input - hpf->y[0] - hpf->y[1];

    /* apply lowpass filter */
    lpf->input = hpf->level;

    multiplier = S_lowpass_stage_multiplier_table[lpf->cutoff_index];

    /* integrator 1 */
    lpf->v[0] = ((lpf->input - lpf->s[0]) * multiplier) / 32768;
    lpf->y[0] = lpf->v[0] + lpf->s[0];
    lpf->s[0] = lpf->y[0] + lpf->v[0];

    /* integrator 2 */
    lpf->v[1] = ((lpf->y[0] - lpf->s[1]) * multiplier) / 32768;
    lpf->y[1] = lpf->v[1] + lpf->s[1];
    lpf->s[1] = lpf->y[1] + lpf->v[1];

    /* set lowpass output level */
    lpf->level = lpf->y[1];
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

  /* note that we compute the lowpass filter coefficients at each note */
  for (m = 0; m < PATCH_HIGHPASS_CUTOFF_NUM_VALUES; m++)
  {
    val = 440.0f * exp(log(2) * ((S_highpass_cutoff_note_table[m] - TUNING_NOTE_A4) / 12.0f));

    omega_0_delta_t_over_2 = 
      tanf(0.5f * TWO_PI * val * CLOCK_DELTA_T_SECONDS);

    S_highpass_stage_multiplier_table[m] = 
      (int) (32768 * (omega_0_delta_t_over_2 / (1.0f + omega_0_delta_t_over_2)) + 0.5f);
  }

  for (m = 0; m < PATCH_LOWPASS_CUTOFF_NUM_VALUES; m++)
  {
    val = 440.0f * exp(log(2) * ((S_lowpass_cutoff_note_table[m] - TUNING_NOTE_A4) / 12.0f));

    omega_0_delta_t_over_2 = 
      tanf(0.5f * TWO_PI * val * CLOCK_DELTA_T_SECONDS);

    S_lowpass_stage_multiplier_table[m] = 
      (int) (32768 * (omega_0_delta_t_over_2 / (1.0f + omega_0_delta_t_over_2)) + 0.5f);
  }

  return 0;
}

