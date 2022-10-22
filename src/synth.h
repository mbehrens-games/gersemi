/*******************************************************************************
** synth.h (individual synth)
*******************************************************************************/

#ifndef SYNTH_H
#define SYNTH_H

#include "filter.h"
#include "patch.h"
#include "voice.h"

#define SYNTH_MAX_PATCHES     64

#define SYNTH_MAX_DRUM_VOICES 8
#define SYNTH_MAX_SFX_VOICES  8
#define SYNTH_MAX_INST_VOICES 8

extern patch  G_synth_patch_bank[SYNTH_MAX_PATCHES];

extern voice  G_synth_drum_voices[SYNTH_MAX_DRUM_VOICES];
extern voice  G_synth_sfx_voices[SYNTH_MAX_SFX_VOICES];
extern voice  G_synth_inst_voices[SYNTH_MAX_INST_VOICES];

extern int    G_synth_level;

/* function declarations */
short int synth_setup();

short int synth_load_patch(int voice_index, int patch_index);

short int synth_set_vibrato(int voice_index, int depth, int tempo, int speed);
short int synth_set_tremolo(int voice_index, int depth, int tempo, int speed);
short int synth_set_wobble(int voice_index, int depth, int tempo, int speed);

short int synth_set_pitch_sweep(int voice_index,  int mode, 
                                                  int tempo, 
                                                  int speed);

short int synth_key_on( int voice_index,
                        int note, int volume, int brightness);
short int synth_key_off(int voice_index);

short int synth_update();

#endif
