/*******************************************************************************
** lowpass.c (lowpasss)
*******************************************************************************/

#include <stdlib.h>
#include <math.h>

#include "bank.h"
#include "clock.h"
#include "lowpass.h"
#include "patch.h"
#include "tuning.h"

#define PI      3.14159265358979323846f
#define TWO_PI  6.28318530717958647693f

#define LOWPASS_BOUND_CUTOFF_NOTE(note)                                        \
  if (note < TUNING_NOTE_C0)                                                   \
    note = TUNING_NOTE_C0;                                                     \
  else if (note > TUNING_NOTE_B9)                                              \
    note = TUNING_NOTE_B9;

/* multiple table */
/* the values form the harmonic series!   */
static short int  S_lowpass_multiple_table[PATCH_LOWPASS_MULTIPLE_NUM_VALUES] = 
                  { 0 * 12 + 0,   /*  1x  */
                    1 * 12 + 0,   /*  2x  */
                    1 * 12 + 7,   /*  3x  */
                    2 * 12 + 0,   /*  4x  */
                    2 * 12 + 4,   /*  5x  */
                    2 * 12 + 7,   /*  6x  */
                    2 * 12 + 10,  /*  7x  */
                    3 * 12 + 0,   /*  8x  */
                    3 * 12 + 2,   /*  9x  */
                    3 * 12 + 4,   /* 10x  */
                    3 * 12 + 6,   /* 11x  */
                    3 * 12 + 7,   /* 12x  */
                    3 * 12 + 8,   /* 13x  */
                    3 * 12 + 10,  /* 14x  */
                    3 * 12 + 11,  /* 15x  */
                    4 * 12 + 0    /* 16x  */
                  };

/* lowpass coefficient table */
static int S_lowpass_stage_multiplier_table[TUNING_NUM_VALID_NOTES];

/* lowpass bank */
lowpass G_lowpass_bank[BANK_NUM_LOWPASSES];

/*******************************************************************************
** lowpass_reset_all()
*******************************************************************************/
short int lowpass_reset_all()
{
  int k;
  int m;

  lowpass* f;

  /* reset all lowpass filters */
  for (k = 0; k < BANK_NUM_LOWPASSES; k++)
  {
    /* obtain lowpass pointer */
    f = &G_lowpass_bank[k];

    /* set base note */
    f->base_note = TUNING_NOTE_C4;

    /* set offset */
    f->offset = 
      S_lowpass_multiple_table[PATCH_LOWPASS_MULTIPLE_DEFAULT - PATCH_LOWPASS_MULTIPLE_LOWER_BOUND];

    /* set cutoff note */
    f->cutoff_note = f->base_note + f->offset;

    LOWPASS_BOUND_CUTOFF_NOTE(f->cutoff_note)

    /* reset state */
    f->input = 0;

    for (m = 0; m < LOWPASS_NUM_STAGES; m++)
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
** lowpass_load_patch()
*******************************************************************************/
short int lowpass_load_patch(int voice_index, int patch_index)
{
  lowpass* f;
  patch* p;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain patch pointer */
  p = &G_patch_bank[patch_index];

  /* obtain lowpass pointer */
  f = &G_lowpass_bank[voice_index];

  /* set lowpass offset */
  if ((p->lowpass_multiple >= PATCH_LOWPASS_MULTIPLE_LOWER_BOUND) && 
      (p->lowpass_multiple <= PATCH_LOWPASS_MULTIPLE_UPPER_BOUND))
  {
    f->offset = 
      S_lowpass_multiple_table[p->lowpass_multiple - PATCH_LOWPASS_MULTIPLE_LOWER_BOUND];
  }
  else
  {
    f->offset = 
      S_lowpass_multiple_table[PATCH_LOWPASS_MULTIPLE_DEFAULT - PATCH_LOWPASS_MULTIPLE_LOWER_BOUND];
  }

  /* set lowpass cutoff note */
  f->cutoff_note = f->base_note + f->offset;

  LOWPASS_BOUND_CUTOFF_NOTE(f->cutoff_note)

  return 0;
}

/*******************************************************************************
** lowpass_set_note()
*******************************************************************************/
short int lowpass_set_note(int voice_index, int note)
{
  lowpass* f;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain lowpass pointer */
  f = &G_lowpass_bank[voice_index];

  /* if note is out of range, ignore */
  if (TUNING_NOTE_IS_NOT_VALID(note))
    return 0;

  /* set base note */
  f->base_note = note;

  /* set lowpass cutoff note */
  f->cutoff_note = f->base_note + f->offset;

  LOWPASS_BOUND_CUTOFF_NOTE(f->cutoff_note)

  return 0;
}

/*******************************************************************************
** lowpass_update_all()
*******************************************************************************/
short int lowpass_update_all()
{
  int m;

  lowpass* f;

  int multiplier;

  for (m = 0; m < BANK_NUM_LOWPASSES; m++)
  {
    /* obtain lowpass pointer */
    f = &G_lowpass_bank[m];

    /* see Vadim Zavalishin's "The Art of VA Filter Design" (p. 77) */
    multiplier = S_lowpass_stage_multiplier_table[f->cutoff_note];

    /* integrator 1 */
    f->v[0] = ((f->input - f->s[0]) * multiplier) / 32768;
    f->y[0] = f->v[0] + f->s[0];
    f->s[0] = f->y[0] + f->v[0];

    /* integrator 2 */
    f->v[1] = ((f->y[0] - f->s[1]) * multiplier) / 32768;
    f->y[1] = f->v[1] + f->s[1];
    f->s[1] = f->y[1] + f->v[1];

    /* set output level */
    f->level = f->y[1];
  }

  return 0;
}

/*******************************************************************************
** lowpass_generate_tables()
*******************************************************************************/
short int lowpass_generate_tables()
{
  int m;

  double val;
  float omega_0_delta_t_over_2;

  /* compute lowpass coefficients */

  /* see Vadim Zavalishin's "The Art of VA Filter Design" for equations */

  /* pre-warping (section 3.8, p. 62)                               */
  /* (1/2) * new_omega_0 * delta_T = tan((1/2) * omega_0 * delta_T) */

  /* 1st order stage multiplier calculation (section 3.10, p. 76-77)              */
  /* multiplier = ((1/2) * omega_0 * delta_T) / [1 + ((1/2) * omega_0 * delta_T)] */

  /* note that we compute the lowpass lowpass coefficients at each note */
  for (m = 0; m < TUNING_NUM_VALID_NOTES; m++)
  {
    val = 440.0f * exp(log(2) * ((m + TUNING_NOTE_C0 - TUNING_NOTE_A4) / 12.0f));

    omega_0_delta_t_over_2 = 
      tanf(0.5f * TWO_PI * val * CLOCK_DELTA_T_SECONDS);

    S_lowpass_stage_multiplier_table[m] = 
      (int) (32768 * (omega_0_delta_t_over_2 / (1.0f + omega_0_delta_t_over_2)) + 0.5f);
  }

  return 0;
}

