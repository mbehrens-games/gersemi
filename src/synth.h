/*******************************************************************************
** synth.h (the synth!)
*******************************************************************************/

#ifndef SYNTH_H
#define SYNTH_H

extern int G_synth_level_left;
extern int G_synth_level_right;

/* function declarations */
short int synth_generate_tables();
short int synth_reset_banks();

short int synth_update();

#endif
