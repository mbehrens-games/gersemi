/*******************************************************************************
** voice.h (synth voice)
*******************************************************************************/

#ifndef VOICE_H
#define VOICE_H

#include "bank.h"

typedef struct voice
{
  /* program, sync */
  short int program;
  short int sync;

  /* currently playing notes, pitch indices */
  int base_note;

  int osc_1_pitch_index;
  int osc_2_pitch_index;

  /* phases */
  unsigned int osc_1_phase;
  unsigned int osc_2_phase;

  /* voice parameters */
  short int osc_1_waveform;
  short int osc_2_waveform;

  short int osc_1_offset;
  short int osc_2_offset;

  /* amplitude effect modes */
  short int tremolo_mode;
  short int boost_mode;
  short int velocity_mode;

  /* velocity scaling, adjustment */
  short int velocity_scaling_amount;
  short int velocity_adjustment;

  /* envelope level */
  short int env_input;

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
short int voice_reset_all();

short int voice_load_patch(int voice_index, int patch_index);

short int voice_set_note(int voice_index, int note, int vel);
short int voice_sync_to_key(int voice_index);

short int voice_update_all();

short int voice_generate_tables();

#endif
