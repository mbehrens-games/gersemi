/*******************************************************************************
** voice.h (synth voice)
*******************************************************************************/

#ifndef VOICE_H
#define VOICE_H

#include "bank.h"

typedef struct voice
{
  /* mode, sync */
  short int mode;
  short int sync;

  /* currently playing notes, pitch indices */
  short int base_note;

  int wave_osc_1_pitch_index;
  int wave_osc_2_pitch_index;
  int extra_osc_pitch_index;

  /* phases */
  unsigned int wave_osc_1_phase;
  unsigned int wave_osc_2_phase;
  unsigned int extra_osc_phase;

  /* voice parameters */
  short int wave_osc_1_waveform;
  short int wave_osc_2_waveform;

  short int wave_osc_1_mix_adjustment;
  short int wave_osc_2_mix_adjustment;

  /* velocity scaling, adjustment */
  short int velocity_scaling_amount;
  short int velocity_adjustment;

  /* envelope level */
  short int env_input;

  /* lfo levels */

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
short int voice_reset_all();

short int voice_load_patch(int voice_index, int patch_index);

short int voice_set_note(int voice_index, int note, int vel);
short int voice_sync_to_key(int voice_index);

short int voice_update_all();

short int voice_generate_tables();

#endif
