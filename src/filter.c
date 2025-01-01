/*******************************************************************************
** filter.c (highpass & lowpass filters)
*******************************************************************************/

#include <stdlib.h>
#include <math.h>

#include "bank.h"
#include "cart.h"
#include "clock.h"
#include "filter.h"
#include "tuning.h"

#define PI      3.14159265358979323846f
#define TWO_PI  6.28318530717958647693f

/* cutoff_index note tables */
static short int S_highpass_cutoff_note_table[4] = 
  { TUNING_NOTE_A0 + 0 * 12 + 0,  /* A0 */
    TUNING_NOTE_A0 + 1 * 12 + 0,  /* A1 */
    TUNING_NOTE_A0 + 2 * 12 + 0,  /* A2 */
    TUNING_NOTE_A0 + 3 * 12 + 0   /* A3 */
  };

static short int S_lowpass_cutoff_note_table[4] = 
  { TUNING_NOTE_C4 + 3 * 12 + 4,  /* E7 */
    TUNING_NOTE_C4 + 3 * 12 + 7,  /* G7 */
    TUNING_NOTE_C4 + 3 * 12 + 9,  /* A7 */
    TUNING_NOTE_C4 + 4 * 12 + 0   /* C8 */
  };

/* filter coefficient tables */
static int S_highpass_stage_multiplier_table[4];
static int S_lowpass_stage_multiplier_table[4];

/* filter bank */
filter G_filter_bank[BANK_NUM_VOICES];

/*******************************************************************************
** filter_reset_all()
*******************************************************************************/
short int filter_reset_all()
{
  int k;
  int m;

  filter* flt;

  /* reset all filters */
  for (k = 0; k < BANK_NUM_VOICES; k++)
  {
    /* obtain filter pointer */
    flt = &G_filter_bank[k];

    /* cart & patch indices */
    flt->cart_index = 0;
    flt->patch_index = 0;

    /* lines */
    flt->input = 0;
    flt->level = 0;

    /* highpass state */
    for (m = 0; m < 2; m++)
    {
      flt->hpf_s[m] = 0;
      flt->hpf_v[m] = 0;
      flt->hpf_y[m] = 0;
    }

    /* lowpass state */
    for (m = 0; m < 2; m++)
    {
      flt->lpf_s[m] = 0;
      flt->lpf_v[m] = 0;
      flt->lpf_y[m] = 0;
    }
  }

  return 0;
}

/*******************************************************************************
** filter_load_patch()
*******************************************************************************/
short int filter_load_patch(int voice_index, 
                            int cart_index, int patch_index)
{
  filter* flt;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* make sure that the cart & patch indices are valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(cart_index))
    return 1;

  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain filter pointer */
  flt = &G_filter_bank[voice_index];

  /* set cart and patch indices */
  flt->cart_index = cart_index;
  flt->patch_index = patch_index;

  return 0;
}

/*******************************************************************************
** filter_update_all()
*******************************************************************************/
short int filter_update_all()
{
  int m;

  filter* flt;

  cart* c;
  patch* p;

  int multiplier;

  for (m = 0; m < BANK_NUM_VOICES; m++)
  {
    /* obtain filter pointer */
    flt = &G_filter_bank[m];

    /* obtain cart & patch pointers */
    c = &G_cart_bank[flt->cart_index];
    p = &(c->patches[flt->patch_index]);

    /* see Vadim Zavalishin's "The Art of VA Filter Design" (p. 77) */

    /* lowpass filter */
    multiplier = S_lowpass_stage_multiplier_table[p->values[PATCH_PARAM_LOWPASS_CUTOFF]];

    /* integrator 1 */
    flt->lpf_v[0] = ((flt->input - flt->lpf_s[0]) * multiplier) >> 15;
    flt->lpf_y[0] = flt->lpf_v[0] + flt->lpf_s[0];
    flt->lpf_s[0] = flt->lpf_y[0] + flt->lpf_v[0];

    /* integrator 2 */
    flt->lpf_v[1] = ((flt->lpf_y[0] - flt->lpf_s[1]) * multiplier) >> 15;
    flt->lpf_y[1] = flt->lpf_v[1] + flt->lpf_s[1];
    flt->lpf_s[1] = flt->lpf_y[1] + flt->lpf_v[1];

    /* highpass filter */
    multiplier = S_highpass_stage_multiplier_table[p->values[PATCH_PARAM_HIGHPASS_CUTOFF]];

    /* integrator 1 */
    flt->hpf_v[0] = ((flt->lpf_y[1] - flt->hpf_s[0]) * multiplier) >> 15;
    flt->hpf_y[0] = flt->hpf_v[0] + flt->hpf_s[0];
    flt->hpf_s[0] = flt->hpf_y[0] + flt->hpf_v[0];

    /* integrator 2 */
    flt->hpf_v[1] = ((flt->lpf_y[1] - flt->hpf_y[0] - flt->hpf_s[1]) * multiplier) >> 15;
    flt->hpf_y[1] = flt->hpf_v[1] + flt->hpf_s[1];
    flt->hpf_s[1] = flt->hpf_y[1] + flt->hpf_v[1];

    /* set output level */
    flt->level = flt->lpf_y[1] - flt->hpf_y[0] - flt->hpf_y[1];
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
  for (m = 0; m < 4; m++)
  {
    val = 440.0f * exp(log(2) * ((S_highpass_cutoff_note_table[m] - TUNING_NOTE_A4) / 12.0f));

    omega_0_delta_t_over_2 = 
      tanf(0.5f * TWO_PI * val * CLOCK_DELTA_T_SECONDS);

    S_highpass_stage_multiplier_table[m] = 
      (int) (32768 * (omega_0_delta_t_over_2 / (1.0f + omega_0_delta_t_over_2)) + 0.5f);
  }

  for (m = 0; m < 4; m++)
  {
    val = 440.0f * exp(log(2) * ((S_lowpass_cutoff_note_table[m] - TUNING_NOTE_A4) / 12.0f));

    omega_0_delta_t_over_2 = 
      tanf(0.5f * TWO_PI * val * CLOCK_DELTA_T_SECONDS);

    S_lowpass_stage_multiplier_table[m] = 
      (int) (32768 * (omega_0_delta_t_over_2 / (1.0f + omega_0_delta_t_over_2)) + 0.5f);
  }

  return 0;
}

