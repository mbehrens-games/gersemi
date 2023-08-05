/*******************************************************************************
** voice.h (synth voice)
*******************************************************************************/

#ifndef VOICE_H
#define VOICE_H

#include "bank.h"

#define VOICE_NUM_OSCS_AND_ENVS 4

typedef struct voice
{
  /* algorithm */
  short int algorithm;

  /* currently playing notes, pitch indices */
  int base_note;
  int osc_note[VOICE_NUM_OSCS_AND_ENVS];
  int osc_pitch_index[VOICE_NUM_OSCS_AND_ENVS];

  /* phases */
  unsigned int osc_phase[VOICE_NUM_OSCS_AND_ENVS];

  /* feedback levels */
  int feed_in[2 * VOICE_NUM_OSCS_AND_ENVS];

  /* voice parameters */
  short int osc_waveform[VOICE_NUM_OSCS_AND_ENVS];
  short int osc_feedback_multiplier[VOICE_NUM_OSCS_AND_ENVS];
  short int osc_sync[VOICE_NUM_OSCS_AND_ENVS];

  short int osc_freq_mode[VOICE_NUM_OSCS_AND_ENVS];
  short int osc_offset[VOICE_NUM_OSCS_AND_ENVS];
  short int osc_detune[VOICE_NUM_OSCS_AND_ENVS];

  short int vibrato_enable[VOICE_NUM_OSCS_AND_ENVS];
  short int tremolo_enable[VOICE_NUM_OSCS_AND_ENVS];
  short int boost_enable[VOICE_NUM_OSCS_AND_ENVS];

  /* envelope levels */
  short int env_input[VOICE_NUM_OSCS_AND_ENVS];

  /* lfo levels */
  short int vibrato_input;
  short int tremolo_input;

  /* sweep level */
  short int sweep_input;

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
