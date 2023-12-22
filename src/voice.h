/*******************************************************************************
** voice.h (synth voice)
*******************************************************************************/

#ifndef VOICE_H
#define VOICE_H

#include "bank.h"

typedef struct voice
{
  /* algorithm, sync */
  short int algorithm;
  short int sync;

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
  short int osc_phi[BANK_OSCS_AND_ENVS_PER_VOICE];

  short int osc_freq_mode[BANK_OSCS_AND_ENVS_PER_VOICE];
  short int osc_offset[BANK_OSCS_AND_ENVS_PER_VOICE];
  short int osc_detune[BANK_OSCS_AND_ENVS_PER_VOICE];

  /* noise */
  short int     noise_mode;
  unsigned int  noise_lfsr;

  unsigned int  noise_phase;
  unsigned int  noise_increment;

  /* effect modes */
  short int tremolo_mode;
  short int boost_mode;

  /* envelope levels */
  short int env_input[BANK_OSCS_AND_ENVS_PER_VOICE];

  /* lfo levels */
  short int lfo_input_vibrato;
  short int lfo_input_tremolo;

  /* boost level */
  short int boost_input;

  /* sweep level */
  short int sweep_input;

  /* bender level */
  short int bender_input;

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
short int voice_sync_phases(int voice_index);

short int voice_update_all();

short int voice_generate_tables();

#endif
