/*******************************************************************************
** voice.h (synth voice)
*******************************************************************************/

#ifndef VOICE_H
#define VOICE_H

#include "envelope.h"
#include "filter.h"
#include "lfo.h"
#include "linear.h"
#include "patch.h"
#include "sweep.h"

enum
{
  VOICE_PROGRAM_SYNC_SQUARE = 0,
  VOICE_PROGRAM_SYNC_TRIANGLE,
  VOICE_PROGRAM_SYNC_SAW,
  VOICE_PROGRAM_SYNC_PHAT_SAW,
  VOICE_PROGRAM_RING_SQUARE,
  VOICE_PROGRAM_RING_TRIANGLE,
  VOICE_PROGRAM_RING_SAW,
  VOICE_PROGRAM_RING_PHAT_SAW,
  VOICE_PROGRAM_PULSE_WAVES,
  VOICE_PROGRAM_FM_1_CARRIER_CHAIN,
  VOICE_PROGRAM_FM_1_CARRIER_Y,
  VOICE_PROGRAM_FM_1_CARRIER_LEFT_CRAB_CLAW,
  VOICE_PROGRAM_FM_1_CARRIER_RIGHT_CRAB_CLAW,
  VOICE_PROGRAM_FM_1_CARRIER_DIAMOND,
  VOICE_PROGRAM_FM_1_CARRIER_THREE_TO_ONE,
  VOICE_PROGRAM_FM_2_CARRIERS_TWIN,
  VOICE_PROGRAM_FM_2_CARRIERS_STACK,
  VOICE_PROGRAM_FM_2_CARRIERS_STACK_ALT,
  VOICE_PROGRAM_FM_2_CARRIERS_SHARED,
  VOICE_PROGRAM_FM_3_CARRIERS_ONE_TO_THREE,
  VOICE_PROGRAM_FM_3_CARRIERS_ONE_TO_TWO,
  VOICE_PROGRAM_FM_3_CARRIERS_ONE_TO_ONE,
  VOICE_PROGRAM_FM_3_CARRIERS_ONE_TO_ONE_ALT,
  VOICE_PROGRAM_FM_4_CARRIERS_PIPES,
  VOICE_NUM_PROGRAMS
};

typedef struct voice
{
  /* current note */
  int note;

  /* base pitch table indices */
  int osc_1_base_pitch_index;
  int osc_2_base_pitch_index;
  int osc_3_base_pitch_index;
  int osc_4_base_pitch_index;
  int osc_5_base_pitch_index;

  /* phases */
  unsigned int osc_1_phase;
  unsigned int osc_2_phase;
  unsigned int osc_3_phase;
  unsigned int osc_4_phase;
  unsigned int osc_5_phase;

  /* program */
  int program;

  /* pitch offsets */
  short int osc_1_offset_coarse;
  short int osc_1_offset_fine;

  short int osc_2_offset_coarse;
  short int osc_2_offset_fine;

  short int osc_3_offset_coarse;
  short int osc_3_offset_fine;

  short int osc_4_offset_coarse;
  short int osc_4_offset_fine;

  /* feedback */
  short int feedback;

  /* noise generator */
  short int noise_period;
  short int noise_alternate;

  unsigned int noise_lfsr;

  /* envelopes */
  envelope osc_1_env;
  envelope osc_2_env;
  envelope osc_3_env;
  envelope osc_4_env;
  envelope osc_5_env;

  /* linear envelope */
  linear extra_env;

  /* lfos */
  lfo vibrato;
  lfo tremolo;
  lfo wobble;

  /* sweep */
  sweep pitch_sweep;

  /* filters */
  filter lowpass;
  filter highpass;

  /* feedback values */
  int feed_in[2];

  /* output level */
  int level;
} voice;

/* function declarations */
short int voice_setup(voice* v);

short int voice_load_patch(voice* v, patch* p);

short int voice_set_vibrato(voice* v, int depth, int tempo, int speed);
short int voice_set_tremolo(voice* v, int depth, int tempo, int speed);
short int voice_set_wobble(voice* v, int depth, int tempo, int speed);

short int voice_set_pitch_sweep(voice* v, int mode, 
                                          int tempo, 
                                          int speed);

short int voice_key_on(voice* v, int note, int volume, int brightness);
short int voice_key_off(voice* v);

short int voice_update(voice* v);

#endif
