/*******************************************************************************
** voice.h (synth voice)
*******************************************************************************/

#ifndef VOICE_H
#define VOICE_H

#include "bank.h"
#include "envelope.h"

#define VOICE_NUM_OSCS_AND_ENVS 4

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
  /* program */
  int program;

  /* current note */
  int osc_note[VOICE_NUM_OSCS_AND_ENVS];

  /* base pitch table indices */
  int osc_base_pitch_index[VOICE_NUM_OSCS_AND_ENVS];

  /* phases */
  unsigned int osc_phase[VOICE_NUM_OSCS_AND_ENVS];

  /* pitch offsets */
  short int osc_offset_coarse[VOICE_NUM_OSCS_AND_ENVS];
  short int osc_offset_fine[VOICE_NUM_OSCS_AND_ENVS];

  /* feedback */
  short int feedback;

  int feed_in[2];

  /* noise generator */
  short int noise_period;
  short int noise_mix;

  unsigned int noise_lfsr;

  /* envelopes */
  short int env_input[VOICE_NUM_OSCS_AND_ENVS];

  /* output level */
  int level;
} voice;

/* voice bank */
extern voice G_voice_bank[BANK_NUM_VOICES];

/* function declarations */
short int voice_setup_all();
short int voice_reset(int voice_index);

short int voice_load_patch(int voice_index, int patch_index);
short int voice_set_note(int voice_index, int note);

short int voice_update_all();

#endif
