/*******************************************************************************
** voice.h (synth voice)
*******************************************************************************/

#ifndef VOICE_H
#define VOICE_H

#include "bank.h"

#define VOICE_NUM_OSCS 4

typedef struct voice
{
  /* cart & patch indices */
  short int cart_index;
  short int patch_index;

  /* looked at by the instrument. can probably move it up there! */
  int base_note;

  /* lfo */
  short int tempo;

  int lfo_delay_cycles;

  unsigned int lfo_phase;
  unsigned int lfo_increment;

  /* oscillators */
  short int osc_pitch_index[VOICE_NUM_OSCS];

  unsigned int osc_phase[VOICE_NUM_OSCS];
  unsigned int noise_lfsr;

  /* envelopes */
  int env_stage[VOICE_NUM_OSCS];

  unsigned int env_increment[VOICE_NUM_OSCS];
  unsigned int env_phase[VOICE_NUM_OSCS];

  short int env_attenuation[VOICE_NUM_OSCS];

  short int env_ks_rate_adjustment[VOICE_NUM_OSCS];
  short int env_ks_level_adjustment[VOICE_NUM_OSCS];

  short int env_volume_adjustment[VOICE_NUM_OSCS];
  short int env_amplitude_adjustment[VOICE_NUM_OSCS];
  short int env_velocity_adjustment[VOICE_NUM_OSCS];

  /* lfo input */
  short int vibrato_base;
  short int vibrato_extra;

  short int tremolo_base;
  short int tremolo_extra;

  short int boost_extra;

  /* midi controller positions */
  short int vibrato_wheel_pos;
  short int tremolo_wheel_pos;
  short int boost_wheel_pos;

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
short int voice_note_off(int voice_index);

short int voice_update_all();

short int voice_generate_tables();

#endif
