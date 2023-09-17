/*******************************************************************************
** voice.h (synth voice)
*******************************************************************************/

#ifndef VOICE_H
#define VOICE_H

#include "bank.h"

typedef struct voice
{
  /* algorithm */
  short int algorithm;

  /* currently playing notes, pitch indices */
  int base_note;
  int osc_note[BANK_OSCS_AND_ENVS_PER_VOICE];
  int osc_pitch_index[BANK_OSCS_AND_ENVS_PER_VOICE];

  /* phases */
  unsigned int osc_phase[BANK_OSCS_AND_ENVS_PER_VOICE];

  /* feedback levels */
  int feed_in[2 * BANK_OSCS_AND_ENVS_PER_VOICE];

  /* voice parameters */
  short int osc_waveform[BANK_OSCS_AND_ENVS_PER_VOICE];
  short int osc_feedback_multiplier[BANK_OSCS_AND_ENVS_PER_VOICE];
  short int osc_sync[BANK_OSCS_AND_ENVS_PER_VOICE];

  short int osc_freq_mode[BANK_OSCS_AND_ENVS_PER_VOICE];
  short int osc_offset[BANK_OSCS_AND_ENVS_PER_VOICE];
  short int osc_detune[BANK_OSCS_AND_ENVS_PER_VOICE];

  short int vibrato_enable[BANK_OSCS_AND_ENVS_PER_VOICE];
  short int tremolo_enable[BANK_OSCS_AND_ENVS_PER_VOICE];
  short int boost_enable[BANK_OSCS_AND_ENVS_PER_VOICE];

  /* envelope levels */
  short int env_input[BANK_OSCS_AND_ENVS_PER_VOICE];

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
