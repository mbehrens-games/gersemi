/*******************************************************************************
** voice.h (synth voice)
*******************************************************************************/

#ifndef VOICE_H
#define VOICE_H

#include "bank.h"

#define VOICE_NUM_OSCS_AND_ENVS 4

enum
{
  VOICE_ALGORITHM_1_CAR_CHAIN = 0,
  VOICE_ALGORITHM_1_CAR_Y,
  VOICE_ALGORITHM_1_CAR_CRAB_CLAW,
  VOICE_ALGORITHM_2_CAR_TWIN,
  VOICE_ALGORITHM_2_CAR_STACKED,
  VOICE_ALGORITHM_3_CAR_ONE_TO_THREE,
  VOICE_ALGORITHM_3_CAR_ONE_TO_ONE,
  VOICE_ALGORITHM_4_CAR_PIPES,
  VOICE_NUM_ALGORITHMS
};

typedef struct voice
{
  /* algorithm */
  int algorithm;

  /* base note */
  int base_note;

  /* current notes */
  int osc_note[VOICE_NUM_OSCS_AND_ENVS];

  /* base pitch table indices */
  int osc_base_pitch_index[VOICE_NUM_OSCS_AND_ENVS];

  /* phases */
  unsigned int osc_phase[VOICE_NUM_OSCS_AND_ENVS];

  /* pitch offsets */
  short int osc_offset_coarse[VOICE_NUM_OSCS_AND_ENVS];
  short int osc_offset_fine[VOICE_NUM_OSCS_AND_ENVS];

  /* feedback */
  int osc_feedback[VOICE_NUM_OSCS_AND_ENVS];

  int feed_in[2 * VOICE_NUM_OSCS_AND_ENVS];

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

short int voice_generate_tables();

#endif
