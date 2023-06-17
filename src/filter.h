/*******************************************************************************
** filter.h (filter)
*******************************************************************************/

#ifndef FILTER_H
#define FILTER_H

#include "bank.h"

enum
{
  FILTER_LOWPASS_CUTOFF_E7 = 0,
  FILTER_LOWPASS_CUTOFF_G7,
  FILTER_LOWPASS_CUTOFF_A7,
  FILTER_LOWPASS_CUTOFF_C8,
  FILTER_NUM_LOWPASS_CUTOFFS
};

enum
{
  FILTER_HIGHPASS_CUTOFF_A0 = 0,
  FILTER_HIGHPASS_CUTOFF_A1,
  FILTER_HIGHPASS_CUTOFF_A2,
  FILTER_HIGHPASS_CUTOFF_A3,
  FILTER_NUM_HIGHPASS_CUTOFFS
};

#define FILTER_NUM_STAGES 2

typedef struct filter
{
  int cutoff;

  int input;

  int s[FILTER_NUM_STAGES];
  int v[FILTER_NUM_STAGES];
  int y[FILTER_NUM_STAGES];

  int level;
} filter;

/* filter bank */
extern filter G_filter_bank[BANK_NUM_FILTERS];

/* function declarations */
short int filter_setup_all();
short int filter_reset(int voice_index);

short int filter_set_lowpass_cutoff(int voice_index, int cutoff);
short int filter_set_highpass_cutoff(int voice_index, int cutoff);

short int filter_update_all();

#endif
