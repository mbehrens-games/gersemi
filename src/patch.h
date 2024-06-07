/*******************************************************************************
** patch.h (patch settings)
*******************************************************************************/

#ifndef PATCH_H
#define PATCH_H

#include "bank.h"

enum
{
  PATCH_SYNC_OFF = 0, 
  PATCH_SYNC_ON 
};

enum
{
  PATCH_ALGORITHM_1_FM_2_FM_3 = 1, 
  PATCH_ALGORITHM_1_ADD_2_FM_3, 
  PATCH_ALGORITHM_2_FM_3_ADD_1, 
  PATCH_ALGORITHM_1_ADD_2_ADD_3 
};

enum
{
  PATCH_OSC_WAVEFORM_SINE = 1, 
  PATCH_OSC_WAVEFORM_HALF_RECT, 
  PATCH_OSC_WAVEFORM_FULL_RECT, 
  PATCH_OSC_WAVEFORM_QUARTER_RECT, 
  PATCH_OSC_WAVEFORM_EPO_SINE, 
  PATCH_OSC_WAVEFORM_EPO_FULL_RECT, 
  PATCH_OSC_WAVEFORM_SQUARE, 
  PATCH_OSC_WAVEFORM_SAW_DOWN, 
  PATCH_OSC_WAVEFORM_SAW_UP, 
  PATCH_OSC_WAVEFORM_TRIANGLE, 
  PATCH_OSC_WAVEFORM_EPO_SQUARE, 
  PATCH_OSC_WAVEFORM_EPO_SAW_DOWN, 
  PATCH_OSC_WAVEFORM_EPO_SAW_UP, 
  PATCH_OSC_WAVEFORM_EPO_TRIANGLE,
  PATCH_OSC_WAVEFORM_NOISE_SQUARE, 
  PATCH_OSC_WAVEFORM_NOISE_SAW 
};

enum
{
  PATCH_OSC_PHI_0 = 1, 
  PATCH_OSC_PHI_90, 
  PATCH_OSC_PHI_180, 
  PATCH_OSC_PHI_270 
};

enum
{
  PATCH_OSC_FREQ_MODE_RATIO = 1, 
  PATCH_OSC_FREQ_MODE_FIXED 
};

enum
{
  PATCH_ENV_HOLD_MODE_OFF = 1, 
  PATCH_ENV_HOLD_MODE_WITH_PEDAL, 
  PATCH_ENV_HOLD_MODE_WITHOUT_PEDAL, 
  PATCH_ENV_HOLD_MODE_ALWAYS 
};

enum
{
  PATCH_LFO_WAVEFORM_TRIANGLE = 1, 
  PATCH_LFO_WAVEFORM_SQUARE, 
  PATCH_LFO_WAVEFORM_SAW_UP, 
  PATCH_LFO_WAVEFORM_SAW_DOWN 
};

enum
{
  PATCH_LFO_POLARITY_BI = 1, 
  PATCH_LFO_POLARITY_UNI 
};

enum
{
  PATCH_PITCH_WHEEL_MODE_BEND = 1, 
  PATCH_PITCH_WHEEL_MODE_SEMITONES 
};

enum
{
  PATCH_ARPEGGIO_MODE_HARP = 1, 
  PATCH_ARPEGGIO_MODE_ROLLED 
};

enum
{
  PATCH_ARPEGGIO_PATTERN_UP = 0, 
  PATCH_ARPEGGIO_PATTERN_DOWN, 
  PATCH_ARPEGGIO_PATTERN_UP_AND_DOWN, 
  PATCH_ARPEGGIO_PATTERN_UP_AND_DOWN_ALT 
};

enum
{
  PATCH_PORTAMENTO_MODE_BEND = 1, 
  PATCH_PORTAMENTO_MODE_SEMITONES 
};

enum
{
  PATCH_PORTAMENTO_LEGATO_OFF = 0, 
  PATCH_PORTAMENTO_LEGATO_FOLLOW, 
  PATCH_PORTAMENTO_LEGATO_HAMMER 
};

/* cart parameters */
#define PATCH_CART_NUMBER_DEFAULT       1
#define PATCH_CART_NUMBER_LOWER_BOUND   1
#define PATCH_CART_NUMBER_UPPER_BOUND   BANK_NUM_CARTS
#define PATCH_CART_NUMBER_NUM_VALUES    (PATCH_CART_NUMBER_UPPER_BOUND - PATCH_CART_NUMBER_LOWER_BOUND + 1)

#define PATCH_PATCH_NUMBER_DEFAULT      1
#define PATCH_PATCH_NUMBER_LOWER_BOUND  1
#define PATCH_PATCH_NUMBER_UPPER_BOUND  BANK_PATCHES_PER_CART
#define PATCH_PATCH_NUMBER_NUM_VALUES   (PATCH_PATCH_NUMBER_UPPER_BOUND - PATCH_PATCH_NUMBER_LOWER_BOUND + 1)

/* name strings: x characters + 1 null terminator */
#define PATCH_CART_NAME_SIZE  (10 + 1)
#define PATCH_PATCH_NAME_SIZE (10 + 1)

/* patch parameters */

/* algorithm */
#define PATCH_ALGORITHM_DEFAULT     PATCH_ALGORITHM_1_FM_2_FM_3
#define PATCH_ALGORITHM_LOWER_BOUND PATCH_ALGORITHM_1_FM_2_FM_3
#define PATCH_ALGORITHM_UPPER_BOUND PATCH_ALGORITHM_1_ADD_2_ADD_3
#define PATCH_ALGORITHM_NUM_VALUES  (PATCH_ALGORITHM_UPPER_BOUND - PATCH_ALGORITHM_LOWER_BOUND + 1)

/* sync */
#define PATCH_SYNC_DEFAULT          PATCH_SYNC_ON
#define PATCH_SYNC_LOWER_BOUND      PATCH_SYNC_OFF
#define PATCH_SYNC_UPPER_BOUND      PATCH_SYNC_ON
#define PATCH_SYNC_NUM_VALUES       (PATCH_SYNC_UPPER_BOUND - PATCH_SYNC_LOWER_BOUND + 1)

/* oscillators */
#define PATCH_OSC_WAVEFORM_DEFAULT      PATCH_OSC_WAVEFORM_SINE
#define PATCH_OSC_WAVEFORM_LOWER_BOUND  PATCH_OSC_WAVEFORM_SINE
#define PATCH_OSC_WAVEFORM_UPPER_BOUND  PATCH_OSC_WAVEFORM_NOISE_SAW
#define PATCH_OSC_WAVEFORM_NUM_VALUES   (PATCH_OSC_WAVEFORM_UPPER_BOUND - PATCH_OSC_WAVEFORM_LOWER_BOUND + 1)

#define PATCH_OSC_PHI_DEFAULT           PATCH_OSC_PHI_0
#define PATCH_OSC_PHI_LOWER_BOUND       PATCH_OSC_PHI_0
#define PATCH_OSC_PHI_UPPER_BOUND       PATCH_OSC_PHI_270
#define PATCH_OSC_PHI_NUM_VALUES        (PATCH_OSC_PHI_UPPER_BOUND - PATCH_OSC_PHI_LOWER_BOUND + 1)

#define PATCH_OSC_FREQ_MODE_DEFAULT     PATCH_OSC_FREQ_MODE_RATIO
#define PATCH_OSC_FREQ_MODE_LOWER_BOUND PATCH_OSC_FREQ_MODE_RATIO
#define PATCH_OSC_FREQ_MODE_UPPER_BOUND PATCH_OSC_FREQ_MODE_FIXED
#define PATCH_OSC_FREQ_MODE_NUM_VALUES  (PATCH_OSC_FREQ_MODE_UPPER_BOUND - PATCH_OSC_FREQ_MODE_LOWER_BOUND + 1)

#define PATCH_OSC_MULTIPLE_DEFAULT      1
#define PATCH_OSC_MULTIPLE_LOWER_BOUND  1
#define PATCH_OSC_MULTIPLE_UPPER_BOUND  16
#define PATCH_OSC_MULTIPLE_NUM_VALUES   (PATCH_OSC_MULTIPLE_UPPER_BOUND - PATCH_OSC_MULTIPLE_LOWER_BOUND + 1)

#define PATCH_OSC_DIVISOR_DEFAULT       1
#define PATCH_OSC_DIVISOR_LOWER_BOUND   1
#define PATCH_OSC_DIVISOR_UPPER_BOUND   8
#define PATCH_OSC_DIVISOR_NUM_VALUES    (PATCH_OSC_DIVISOR_UPPER_BOUND - PATCH_OSC_DIVISOR_LOWER_BOUND + 1)

#define PATCH_OSC_OCTAVE_DEFAULT        4
#define PATCH_OSC_OCTAVE_LOWER_BOUND    0
#define PATCH_OSC_OCTAVE_UPPER_BOUND    9
#define PATCH_OSC_OCTAVE_NUM_VALUES     (PATCH_OSC_OCTAVE_UPPER_BOUND - PATCH_OSC_OCTAVE_LOWER_BOUND + 1)

#define PATCH_OSC_NOTE_DEFAULT          1
#define PATCH_OSC_NOTE_LOWER_BOUND      1
#define PATCH_OSC_NOTE_UPPER_BOUND      12
#define PATCH_OSC_NOTE_NUM_VALUES       (PATCH_OSC_NOTE_UPPER_BOUND - PATCH_OSC_NOTE_LOWER_BOUND + 1)

#define PATCH_OSC_DETUNE_DEFAULT        0
#define PATCH_OSC_DETUNE_LOWER_BOUND   -3
#define PATCH_OSC_DETUNE_UPPER_BOUND    3
#define PATCH_OSC_DETUNE_NUM_VALUES     (PATCH_OSC_DETUNE_UPPER_BOUND - PATCH_OSC_DETUNE_LOWER_BOUND + 1)

/* amplitude envelope */
#define PATCH_ENV_TIME_DEFAULT      1
#define PATCH_ENV_TIME_LOWER_BOUND  1
#define PATCH_ENV_TIME_UPPER_BOUND  100
#define PATCH_ENV_TIME_NUM_VALUES   (PATCH_ENV_TIME_UPPER_BOUND - PATCH_ENV_TIME_LOWER_BOUND + 1)

#define PATCH_ENV_LEVEL_DEFAULT     0
#define PATCH_ENV_LEVEL_LOWER_BOUND 0
#define PATCH_ENV_LEVEL_UPPER_BOUND 100
#define PATCH_ENV_LEVEL_NUM_VALUES  (PATCH_ENV_LEVEL_UPPER_BOUND - PATCH_ENV_LEVEL_LOWER_BOUND + 1)

#define PATCH_ENV_HOLD_MODE_DEFAULT       PATCH_ENV_HOLD_MODE_OFF
#define PATCH_ENV_HOLD_MODE_LOWER_BOUND   PATCH_ENV_HOLD_MODE_OFF
#define PATCH_ENV_HOLD_MODE_UPPER_BOUND   PATCH_ENV_HOLD_MODE_ALWAYS
#define PATCH_ENV_HOLD_MODE_NUM_VALUES    (PATCH_ENV_HOLD_MODE_UPPER_BOUND - PATCH_ENV_HOLD_MODE_LOWER_BOUND + 1)

#define PATCH_ENV_KEYSCALING_DEFAULT      2
#define PATCH_ENV_KEYSCALING_LOWER_BOUND  1
#define PATCH_ENV_KEYSCALING_UPPER_BOUND  8
#define PATCH_ENV_KEYSCALING_NUM_VALUES   (PATCH_ENV_KEYSCALING_UPPER_BOUND - PATCH_ENV_KEYSCALING_LOWER_BOUND + 1)

/* lfo */
#define PATCH_LFO_WAVEFORM_DEFAULT      PATCH_LFO_WAVEFORM_TRIANGLE
#define PATCH_LFO_WAVEFORM_LOWER_BOUND  PATCH_LFO_WAVEFORM_TRIANGLE
#define PATCH_LFO_WAVEFORM_UPPER_BOUND  PATCH_LFO_WAVEFORM_SAW_DOWN
#define PATCH_LFO_WAVEFORM_NUM_VALUES   (PATCH_LFO_WAVEFORM_UPPER_BOUND - PATCH_LFO_WAVEFORM_LOWER_BOUND + 1)

#define PATCH_LFO_DELAY_DEFAULT         0
#define PATCH_LFO_DELAY_LOWER_BOUND     0
#define PATCH_LFO_DELAY_UPPER_BOUND     48
#define PATCH_LFO_DELAY_NUM_VALUES      (PATCH_LFO_DELAY_UPPER_BOUND - PATCH_LFO_DELAY_LOWER_BOUND + 1)

#define PATCH_LFO_SPEED_DEFAULT         8
#define PATCH_LFO_SPEED_LOWER_BOUND     1
#define PATCH_LFO_SPEED_UPPER_BOUND     16
#define PATCH_LFO_SPEED_NUM_VALUES      (PATCH_LFO_SPEED_UPPER_BOUND - PATCH_LFO_SPEED_LOWER_BOUND + 1)

#define PATCH_LFO_DEPTH_DEFAULT         0
#define PATCH_LFO_DEPTH_LOWER_BOUND     0
#define PATCH_LFO_DEPTH_UPPER_BOUND     100
#define PATCH_LFO_DEPTH_NUM_VALUES      (PATCH_LFO_DEPTH_UPPER_BOUND - PATCH_LFO_DEPTH_LOWER_BOUND + 1)

#define PATCH_LFO_POLARITY_DEFAULT      PATCH_LFO_POLARITY_BI
#define PATCH_LFO_POLARITY_LOWER_BOUND  PATCH_LFO_POLARITY_BI
#define PATCH_LFO_POLARITY_UPPER_BOUND  PATCH_LFO_POLARITY_UNI
#define PATCH_LFO_POLARITY_NUM_VALUES   (PATCH_LFO_POLARITY_UPPER_BOUND - PATCH_LFO_POLARITY_LOWER_BOUND + 1)

/* sensitivity */
#define PATCH_SENSITIVITY_DEFAULT       8
#define PATCH_SENSITIVITY_LOWER_BOUND   1
#define PATCH_SENSITIVITY_UPPER_BOUND   16
#define PATCH_SENSITIVITY_NUM_VALUES    (PATCH_SENSITIVITY_UPPER_BOUND - PATCH_SENSITIVITY_LOWER_BOUND + 1)

/* filters */
#define PATCH_HIGHPASS_CUTOFF_DEFAULT     1
#define PATCH_HIGHPASS_CUTOFF_LOWER_BOUND 1
#define PATCH_HIGHPASS_CUTOFF_UPPER_BOUND 4
#define PATCH_HIGHPASS_CUTOFF_NUM_VALUES  (PATCH_HIGHPASS_CUTOFF_UPPER_BOUND - PATCH_HIGHPASS_CUTOFF_LOWER_BOUND + 1)

#define PATCH_LOWPASS_CUTOFF_DEFAULT      4
#define PATCH_LOWPASS_CUTOFF_LOWER_BOUND  1
#define PATCH_LOWPASS_CUTOFF_UPPER_BOUND  4
#define PATCH_LOWPASS_CUTOFF_NUM_VALUES   (PATCH_LOWPASS_CUTOFF_UPPER_BOUND - PATCH_LOWPASS_CUTOFF_LOWER_BOUND + 1)

/* pitch envelope */
#define PATCH_PEG_TIME_DEFAULT        1
#define PATCH_PEG_TIME_LOWER_BOUND    1
#define PATCH_PEG_TIME_UPPER_BOUND  100
#define PATCH_PEG_TIME_NUM_VALUES    (PATCH_PEG_TIME_UPPER_BOUND - PATCH_PEG_TIME_LOWER_BOUND + 1)

#define PATCH_PEG_LEVEL_DEFAULT       0
#define PATCH_PEG_LEVEL_LOWER_BOUND -50
#define PATCH_PEG_LEVEL_UPPER_BOUND  50
#define PATCH_PEG_LEVEL_NUM_VALUES   (PATCH_PEG_LEVEL_UPPER_BOUND - PATCH_PEG_LEVEL_LOWER_BOUND + 1)

/* pitch wheel */
#define PATCH_PITCH_WHEEL_MODE_DEFAULT      PATCH_PITCH_WHEEL_MODE_BEND
#define PATCH_PITCH_WHEEL_MODE_LOWER_BOUND  PATCH_PITCH_WHEEL_MODE_BEND
#define PATCH_PITCH_WHEEL_MODE_UPPER_BOUND  PATCH_PITCH_WHEEL_MODE_SEMITONES
#define PATCH_PITCH_WHEEL_MODE_NUM_VALUES   (PATCH_PITCH_WHEEL_MODE_UPPER_BOUND - PATCH_PITCH_WHEEL_MODE_LOWER_BOUND + 1)

#define PATCH_PITCH_WHEEL_RANGE_DEFAULT     1
#define PATCH_PITCH_WHEEL_RANGE_LOWER_BOUND 1
#define PATCH_PITCH_WHEEL_RANGE_UPPER_BOUND 24
#define PATCH_PITCH_WHEEL_RANGE_NUM_VALUES  (PATCH_PITCH_WHEEL_RANGE_UPPER_BOUND - PATCH_PITCH_WHEEL_RANGE_LOWER_BOUND + 1)

/* arpeggio */
#define PATCH_ARPEGGIO_MODE_DEFAULT         PATCH_ARPEGGIO_MODE_HARP
#define PATCH_ARPEGGIO_MODE_LOWER_BOUND     PATCH_ARPEGGIO_MODE_HARP
#define PATCH_ARPEGGIO_MODE_UPPER_BOUND     PATCH_ARPEGGIO_MODE_ROLLED
#define PATCH_ARPEGGIO_MODE_NUM_VALUES      (PATCH_ARPEGGIO_MODE_UPPER_BOUND - PATCH_ARPEGGIO_MODE_LOWER_BOUND + 1)

#define PATCH_ARPEGGIO_PATTERN_DEFAULT      PATCH_ARPEGGIO_PATTERN_UP
#define PATCH_ARPEGGIO_PATTERN_LOWER_BOUND  PATCH_ARPEGGIO_PATTERN_UP
#define PATCH_ARPEGGIO_PATTERN_UPPER_BOUND  PATCH_ARPEGGIO_PATTERN_UP_AND_DOWN_ALT
#define PATCH_ARPEGGIO_PATTERN_NUM_VALUES   (PATCH_ARPEGGIO_PATTERN_UPPER_BOUND - PATCH_ARPEGGIO_PATTERN_LOWER_BOUND + 1)

#define PATCH_ARPEGGIO_OCTAVES_DEFAULT      1
#define PATCH_ARPEGGIO_OCTAVES_LOWER_BOUND  1
#define PATCH_ARPEGGIO_OCTAVES_UPPER_BOUND  4
#define PATCH_ARPEGGIO_OCTAVES_NUM_VALUES   (PATCH_ARPEGGIO_OCTAVES_UPPER_BOUND - PATCH_ARPEGGIO_OCTAVES_LOWER_BOUND + 1)

#define PATCH_ARPEGGIO_SPEED_DEFAULT        1
#define PATCH_ARPEGGIO_SPEED_LOWER_BOUND    1
#define PATCH_ARPEGGIO_SPEED_UPPER_BOUND    12
#define PATCH_ARPEGGIO_SPEED_NUM_VALUES     (PATCH_ARPEGGIO_SPEED_UPPER_BOUND - PATCH_ARPEGGIO_SPEED_LOWER_BOUND + 1)

/* portamento */
#define PATCH_PORTAMENTO_MODE_DEFAULT       PATCH_PORTAMENTO_MODE_BEND
#define PATCH_PORTAMENTO_MODE_LOWER_BOUND   PATCH_PORTAMENTO_MODE_BEND
#define PATCH_PORTAMENTO_MODE_UPPER_BOUND   PATCH_PORTAMENTO_MODE_SEMITONES
#define PATCH_PORTAMENTO_MODE_NUM_VALUES    (PATCH_PORTAMENTO_MODE_UPPER_BOUND - PATCH_PORTAMENTO_MODE_LOWER_BOUND + 1)

#define PATCH_PORTAMENTO_LEGATO_DEFAULT     PATCH_PORTAMENTO_LEGATO_OFF
#define PATCH_PORTAMENTO_LEGATO_LOWER_BOUND PATCH_PORTAMENTO_LEGATO_OFF
#define PATCH_PORTAMENTO_LEGATO_UPPER_BOUND PATCH_PORTAMENTO_LEGATO_HAMMER
#define PATCH_PORTAMENTO_LEGATO_NUM_VALUES  (PATCH_PORTAMENTO_LEGATO_UPPER_BOUND - PATCH_PORTAMENTO_LEGATO_LOWER_BOUND + 1)

#define PATCH_PORTAMENTO_SPEED_DEFAULT      8
#define PATCH_PORTAMENTO_SPEED_LOWER_BOUND  1
#define PATCH_PORTAMENTO_SPEED_UPPER_BOUND  16
#define PATCH_PORTAMENTO_SPEED_NUM_VALUES   (PATCH_PORTAMENTO_SPEED_UPPER_BOUND - PATCH_PORTAMENTO_SPEED_LOWER_BOUND + 1)

/* routing parameter flags */
#define PATCH_OSC_ROUTING_CLEAR 0x00
#define PATCH_OSC_ROUTING_MASK  0x07

#define PATCH_OSC_ROUTING_FLAG_VIBRATO      0x01
#define PATCH_OSC_ROUTING_FLAG_PITCH_ENV    0x02
#define PATCH_OSC_ROUTING_FLAG_PITCH_WHEEL  0x04

#define PATCH_ENV_ROUTING_CLEAR 0x00
#define PATCH_ENV_ROUTING_MASK  0x07

#define PATCH_ENV_ROUTING_FLAG_TREMOLO  0x01
#define PATCH_ENV_ROUTING_FLAG_BOOST    0x02
#define PATCH_ENV_ROUTING_FLAG_VELOCITY 0x04

#define PATCH_MIDI_CONT_ROUTING_CLEAR 0x00
#define PATCH_MIDI_CONT_ROUTING_MASK  0x0F

#define PATCH_MIDI_CONT_ROUTING_FLAG_VIBRATO  0x01
#define PATCH_MIDI_CONT_ROUTING_FLAG_TREMOLO  0x02
#define PATCH_MIDI_CONT_ROUTING_FLAG_BOOST    0x04
#define PATCH_MIDI_CONT_ROUTING_FLAG_CHORUS   0x08

/* patch */
typedef struct patch
{
  /* name */
  char name[PATCH_PATCH_NAME_SIZE];

  /* algorithm */
  short int algorithm;

  /* oscillator sync */
  short int osc_sync;

  /* oscillators */
  short int osc_waveform[BANK_OSCILLATORS_PER_VOICE];
  short int osc_phi[BANK_OSCILLATORS_PER_VOICE];

  short int osc_freq_mode[BANK_OSCILLATORS_PER_VOICE];
  short int osc_multiple[BANK_OSCILLATORS_PER_VOICE];
  short int osc_divisor[BANK_OSCILLATORS_PER_VOICE];
  short int osc_octave[BANK_OSCILLATORS_PER_VOICE];
  short int osc_note[BANK_OSCILLATORS_PER_VOICE];
  short int osc_detune[BANK_OSCILLATORS_PER_VOICE];
  short int osc_routing[BANK_OSCILLATORS_PER_VOICE];

  /* amplitude envelopes */
  short int env_attack[BANK_ENVELOPES_PER_VOICE];
  short int env_decay[BANK_ENVELOPES_PER_VOICE];
  short int env_sustain[BANK_ENVELOPES_PER_VOICE];
  short int env_release[BANK_ENVELOPES_PER_VOICE];
  short int env_amplitude[BANK_ENVELOPES_PER_VOICE];
  short int env_hold_level[BANK_ENVELOPES_PER_VOICE];
  short int env_hold_mode[BANK_ENVELOPES_PER_VOICE];
  short int env_rate_ks[BANK_ENVELOPES_PER_VOICE];
  short int env_level_ks[BANK_ENVELOPES_PER_VOICE];
  short int env_routing[BANK_ENVELOPES_PER_VOICE];

  /* lfos */
  short int lfo_waveform[BANK_LFOS_PER_VOICE];
  short int lfo_delay[BANK_LFOS_PER_VOICE];
  short int lfo_speed[BANK_LFOS_PER_VOICE];
  short int lfo_depth[BANK_LFOS_PER_VOICE];
  short int lfo_sensitivity[BANK_LFOS_PER_VOICE];
  short int lfo_sync[BANK_LFOS_PER_VOICE];
  short int lfo_polarity[BANK_LFOS_PER_VOICE];

  /* boost */
  short int boost_sensitivity;

  /* velocity */
  short int velocity_sensitivity;

  /* filters */
  short int highpass_cutoff;
  short int lowpass_cutoff;

  /* pitch envelope */
  short int peg_attack;
  short int peg_decay;
  short int peg_release;
  short int peg_maximum;
  short int peg_finale;

  /* pitch wheel */
  short int pitch_wheel_mode;
  short int pitch_wheel_range;

  /* arpeggio */
  short int arpeggio_mode;
  short int arpeggio_pattern;
  short int arpeggio_octaves;
  short int arpeggio_speed;

  /* portamento */
  short int portamento_mode;
  short int portamento_legato;
  short int portamento_speed;

  /* midi controller routing */
  short int mod_wheel_routing;
  short int aftertouch_routing;
  short int exp_pedal_routing;
} patch;

/* cart */
typedef struct cart
{
  char name[PATCH_CART_NAME_SIZE];

  patch patches[BANK_PATCHES_PER_CART];
} cart;

/* cart bank */
extern cart G_cart_bank[BANK_NUM_CARTS];

/* function declarations */
short int patch_reset_all();

short int patch_reset_patch(int cart_index, int patch_index);
short int patch_validate_patch(int cart_index, int patch_index);
short int patch_copy_patch( int dest_cart_index,  int dest_patch_index, 
                            int src_cart_index,   int src_patch_index);

short int patch_reset_cart(int cart_index);
short int patch_validate_cart(int cart_index);
short int patch_copy_cart(int dest_cart_index, int src_cart_index);

#endif
