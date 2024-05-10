/*******************************************************************************
** voice.h (synth voice)
*******************************************************************************/

#ifndef VOICE_H
#define VOICE_H

#include "bank.h"

#define VOICE_NUM_OSCILLATOR_SETS 2

typedef struct voice
{
  /* algorithm, sync */
  short int algorithm;
  short int sync;

  /* oscillators */
  short int osc_waveform[BANK_OSCILLATORS_PER_VOICE];
  short int osc_phi[BANK_OSCILLATORS_PER_VOICE];
  short int osc_pitch_offset[BANK_OSCILLATORS_PER_VOICE];

  /* currently playing notes, pitch indices */
  short int base_note;

  short int osc_pitch_index[BANK_OSCILLATORS_PER_VOICE];

  /* phases */
  unsigned int osc_phase[BANK_OSCILLATORS_PER_VOICE][VOICE_NUM_OSCILLATOR_SETS];

  /* noise lfsrs */
  unsigned int osc_lfsr[BANK_OSCILLATORS_PER_VOICE];

  /* envelope input levels */
  short int env_input[BANK_ENVELOPES_PER_VOICE];

  /* vibrato, chorus */
  short int vibrato_base;
  short int vibrato_extra;

  short int chorus_base;
  short int chorus_extra;

  short int vibrato_adjustment;
  short int chorus_adjustment;

  /* pitch wheel, sweep */
  short int pitch_wheel_max;

  short int sweep_input;

  /* routing */
  unsigned char routing;

  /* midi controller positions */
  short int mod_wheel_pos;
  short int aftertouch_pos;
  short int exp_pedal_pos;
  short int pitch_wheel_pos;

  /* output level */
  int level;
} voice;

/* voice bank */
extern voice G_voice_bank[BANK_NUM_VOICES];

/* function declarations */
short int voice_reset_all();

short int voice_load_patch(int voice_index, int patch_index);

short int voice_set_note(int voice_index, int note);
short int voice_sync_to_key(int voice_index);

short int voice_update_all();

short int voice_generate_tables();

#endif
