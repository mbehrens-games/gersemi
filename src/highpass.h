/*******************************************************************************
** highpass.h (highpass filter)
*******************************************************************************/

#ifndef HIGHPASS_H
#define HIGHPASS_H

#include "bank.h"

#define HIGHPASS_NUM_STAGES 2

typedef struct highpass
{
  short int cutoff;

  int input;

  int s[HIGHPASS_NUM_STAGES];
  int v[HIGHPASS_NUM_STAGES];
  int y[HIGHPASS_NUM_STAGES];

  int level;
} highpass;

/* highpass bank */
extern highpass G_highpass_bank[BANK_NUM_HIGHPASSES];

/* function declarations */
short int highpass_reset_all();

short int highpass_load_patch(int voice_index, int patch_index);

short int highpass_update_all();

short int highpass_generate_tables();

#endif
