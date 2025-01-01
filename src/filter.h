/*******************************************************************************
** filter.h (highpass & lowpass filters)
*******************************************************************************/

#ifndef FILTER_H
#define FILTER_H

#include "bank.h"

typedef struct filter
{
  /* cart & patch indices */
  short int cart_index;
  short int patch_index;

  /* filter */
  int input;

  int hpf_s[2];
  int hpf_v[2];
  int hpf_y[2];

  int lpf_s[2];
  int lpf_v[2];
  int lpf_y[2];

  int level;
} filter;

/* filter bank */
extern filter G_filter_bank[BANK_NUM_VOICES];

/* function declarations */
short int filter_reset_all();

short int filter_load_patch(int voice_index, 
                            int cart_index, int patch_index);

short int filter_update_all();

short int filter_generate_tables();

#endif
