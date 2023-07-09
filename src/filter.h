/*******************************************************************************
** filter.h (filter)
*******************************************************************************/

#ifndef FILTER_H
#define FILTER_H

#include "bank.h"

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

short int filter_load_patch(int voice_index, int patch_index);

short int filter_update_all();

#endif
