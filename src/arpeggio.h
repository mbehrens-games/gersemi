/*******************************************************************************
** arpeggio.h (arpeggiator)
*******************************************************************************/

#ifndef ARPEGGIO_H
#define ARPEGGIO_H

#include "bank.h"
#include "patch.h"

#define ARPEGGIO_NUM_KEYS   16
#define ARPEGGIO_NUM_NOTES  (ARPEGGIO_NUM_KEYS * PATCH_ARPEGGIO_OCTAVES_NUM_VALUES)

typedef struct arpeggio
{
  /* arpeggio parameters */
  short int mode;
  short int pattern;
  short int octaves;
  short int speed;

  /* phase, phase increment */
  unsigned int phase;
  unsigned int increment;

  /* tempo */
  short int tempo;

  /* step index */
  short int step_index;

  /* notes */
  short int sorted_notes[ARPEGGIO_NUM_NOTES];
  short int num_notes;

  short int last_note;

  /* switch */
  short int on_switch;
} arpeggio;

/* arpeggio bank */
extern arpeggio G_arpeggio_bank[BANK_NUM_ARPEGGIOS];

/* function declarations */
short int arpeggio_reset_all();

short int arpeggio_load_patch(int instrument_index, int patch_index);

short int arpeggio_set_mode(int instrument_index, short int mode);
short int arpeggio_set_pattern(int instrument_index, short int pattern);
short int arpeggio_set_octaves(int instrument_index, short int octaves);
short int arpeggio_set_speed(int instrument_index, short int speed);

short int arpeggio_set_switch(int instrument_index, short int state);

short int arpeggio_set_tempo(int instrument_index, short int tempo);

short int arpeggio_generate_notes(int instrument_index);

short int arpeggio_update_all();

short int arpeggio_generate_tables();

#endif
