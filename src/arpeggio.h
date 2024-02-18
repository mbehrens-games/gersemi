/*******************************************************************************
** arpeggio.h (arpeggiator)
*******************************************************************************/

#ifndef ARPEGGIO_H
#define ARPEGGIO_H

#include "bank.h"

typedef struct arpeggio
{
  /* arpeggio parameters */
  short int mode;
  short int pattern;
  short int speed;
  short int sync;

  /* phase, phase increment */
  unsigned int phase;
  unsigned int increment;

  /* tempo */
  short int tempo;
} arpeggio;

/* arpeggio bank */
extern arpeggio G_arpeggio_bank[BANK_NUM_ARPEGGIOS];

/* function declarations */
short int arpeggio_setup_all();
short int arpeggio_reset(int voice_index);

short int arpeggio_load_patch(int voice_index, int patch_index);

short int arpeggio_set_tempo(int voice_index, short int tempo);

short int arpeggio_sync_phase(int voice_index);
short int arpeggio_trigger(int voice_index);

short int arpeggio_update_all();

short int arpeggio_generate_tables();

#endif
