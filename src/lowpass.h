/*******************************************************************************
** lowpass.h (lowpass filter)
*******************************************************************************/

#ifndef LOWPASS_H
#define LOWPASS_H

#include "bank.h"

#define LOWPASS_NUM_STAGES 2

typedef struct lowpass
{
  short int base_note;
  short int offset;

  short int cutoff_note;

  int input;

  int s[LOWPASS_NUM_STAGES];
  int v[LOWPASS_NUM_STAGES];
  int y[LOWPASS_NUM_STAGES];

  int level;
} lowpass;

/* lowpass bank */
extern lowpass G_lowpass_bank[BANK_NUM_LOWPASSES];

/* function declarations */
short int lowpass_reset_all();

short int lowpass_load_patch(int voice_index, int patch_index);

short int lowpass_set_note(int voice_index, int note);

short int lowpass_update_all();

short int lowpass_generate_tables();

#endif
