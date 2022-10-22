/*******************************************************************************
** filter.h (filter)
*******************************************************************************/

#ifndef FILTER_H
#define FILTER_H

enum
{
  FILTER_MODE_LOWPASS = 0,
  FILTER_MODE_HIGHPASS
};

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
  int mode;

  int fc_index;

  int s[FILTER_NUM_STAGES];
  int v[FILTER_NUM_STAGES];
  int y[FILTER_NUM_STAGES];

  int level;
} filter;

/* function declarations */
short int filter_setup(filter* fltr, int mode, int fc_index);

short int filter_set_index(filter* fltr, int fc_index);

short int filter_reset(filter* fltr);
short int filter_update(filter* fltr, int input);

#endif
