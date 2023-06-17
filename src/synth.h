/*******************************************************************************
** synth.h (individual synth)
*******************************************************************************/

#ifndef SYNTH_H
#define SYNTH_H

extern int G_synth_level_left;
extern int G_synth_level_right;

/* function declarations */
short int synth_generate_tables();
short int synth_reset_banks();

short int synth_load_patch(int voice_index, int patch_index);

short int synth_key_on( int voice_index, 
                        int octave, int degree, 
                        int volume, int brightness);
short int synth_key_off(int voice_index);

short int synth_update();

#endif
