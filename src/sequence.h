/*******************************************************************************
** sequence.h (sequencer)
*******************************************************************************/

#ifndef SEQUENCE_H
#define SEQUENCE_H

/* function declarations */
short int sequencer_setup();

short int sequencer_reset();

short int sequencer_activate_step();
short int sequencer_update();

short int sequencer_generate_tables();

#endif
