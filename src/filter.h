/*******************************************************************************
** filter.h (highpass & lowpass filters)
*******************************************************************************/

#ifndef FILTER_H
#define FILTER_H

#include "bank.h"

#define FILTER_NUM_STAGES 2

typedef struct filter
{
  short int cutoff_index;

  int input;

  int s[FILTER_NUM_STAGES];
  int v[FILTER_NUM_STAGES];
  int y[FILTER_NUM_STAGES];

  int level;
} filter;

/* filter bank */
extern filter G_highpass_filter_bank[BANK_NUM_FILTER_SETS];
extern filter G_lowpass_filter_bank[BANK_NUM_FILTER_SETS];

/* function declarations */
short int filter_reset_all();

short int filter_load_patch(int voice_index, 
                            int cart_index, int patch_index);

short int filter_update_all();

short int filter_generate_tables();

#endif
