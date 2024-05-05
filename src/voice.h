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

  /* oscillator 1 */
  short int osc_1_waveform;
  short int osc_1_phi;
  short int osc_1_pitch_offset;

  /* oscillator 2 */
  short int osc_2_waveform;
  short int osc_2_phi;
  short int osc_2_pitch_offset;

  /* oscillator 3 */
  short int osc_3_waveform;
  short int osc_3_phi;
  short int osc_3_pitch_offset;

  /* currently playing notes, pitch indices */
  short int base_note;

  short int osc_1_pitch_index;
  short int osc_2_pitch_index;
  short int osc_3_pitch_index;

  /* phases */
  unsigned int osc_1_phase;
  unsigned int osc_2_phase;
  unsigned int osc_3_phase;

  unsigned int chorus_1_phase;
  unsigned int chorus_2_phase;
  unsigned int chorus_3_phase;

  /* noise lfsrs */
  unsigned int osc_1_lfsr;
  unsigned int osc_2_lfsr;
  unsigned int osc_3_lfsr;

  /* envelope input levels */
  short int env_1_input;
  short int env_2_input;
  short int env_3_input;

  /* vibrato, sweep */
  short int vibrato_input;
  short int chorus_input;
  short int sweep_input;

  short int vibrato_adjustment;

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
