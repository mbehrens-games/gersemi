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

  /* oscillators */
  short int osc_waveform[BANK_OSCILLATORS_PER_VOICE];
  short int osc_phi[BANK_OSCILLATORS_PER_VOICE];
  short int osc_freq_mode[BANK_OSCILLATORS_PER_VOICE];

  short int osc_note_offset[BANK_OSCILLATORS_PER_VOICE];
  short int osc_detune_offset[BANK_OSCILLATORS_PER_VOICE];

  unsigned char osc_routing[BANK_OSCILLATORS_PER_VOICE];

  /* currently playing notes, pitch indices */
  short int base_note;

  short int osc_pitch_index[BANK_OSCILLATORS_PER_VOICE];

  /* phases */
  unsigned int osc_phase[BANK_OSCILLATORS_PER_VOICE];

  /* noise lfsr */
  unsigned int noise_lfsr;

  /* envelope input levels */
  short int env_input[BANK_ENVELOPES_PER_VOICE];

  /* vibrato */
  short int vibrato_base;
  short int vibrato_extra;

  /* pitch wheel */
  short int pitch_wheel_mode;
  short int pitch_wheel_max;

  /* pitch envelope, sweep */
  short int peg_input;
  short int sweep_input;

  /* midi controller routing */
  unsigned char mod_wheel_routing;
  unsigned char aftertouch_routing;
  unsigned char exp_pedal_routing;

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

short int voice_load_patch( int voice_index, 
                            int cart_index, int patch_index);

short int voice_note_on(int voice_index, int note);

short int voice_update_all();

short int voice_generate_tables();

#endif
