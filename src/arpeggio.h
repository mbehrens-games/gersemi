/*******************************************************************************
** arpeggio.h (arpeggiator)
*******************************************************************************/

#ifndef ARPEGGIO_H
#define ARPEGGIO_H

#include "bank.h"
#include "patch.h"

#define ARPEGGIO_NUM_KEYS   12
#define ARPEGGIO_NUM_NOTES  (ARPEGGIO_NUM_KEYS * PATCH_ARPEGGIO_OCTAVES_NUM_VALUES)
#define ARPEGGIO_NUM_STEPS  (2 * ARPEGGIO_NUM_NOTES)

typedef struct arpeggio
{
  /* arpeggio parameters */
  short int mode;
  short int pattern;
  short int octaves;
  short int speed;

  /* tempo */
  short int tempo;

  /* phase, phase increment */
  unsigned int phase;
  unsigned int increment;

  /* pattern steps */
  short int pattern_steps[ARPEGGIO_NUM_STEPS];
  short int num_steps;
  short int num_notes;

  /* step index */
  short int step_index;

  /* switch */
  short int on_switch;
} arpeggio;

/* arpeggio bank */
extern arpeggio G_arpeggio_bank[BANK_NUM_ARPEGGIOS];

/* function declarations */
short int arpeggio_reset_all();

short int arpeggio_load_patch(int instrument_index, 
                              int cart_index, int patch_index);

short int arpeggio_set_mode(int instrument_index, short int mode);
short int arpeggio_set_pattern(int instrument_index, short int pattern);
short int arpeggio_set_octaves(int instrument_index, short int octaves);
short int arpeggio_set_speed(int instrument_index, short int speed);

short int arpeggio_set_switch(int instrument_index, short int state);
short int arpeggio_set_tempo(int instrument_index, short int tempo);

short int arpeggio_tempo_sync(int instrument_index);

short int arpeggio_key_pressed(int instrument_index, int note);
short int arpeggio_key_released(int instrument_index, int note);

short int arpeggio_update_all();

short int arpeggio_generate_tables();

#endif
