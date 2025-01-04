/*******************************************************************************
** cart.h (carts & patches)
*******************************************************************************/

#ifndef CART_H
#define CART_H

#include "bank.h"

enum
{
  PATCH_SYNC_VAL_OFF = 0, 
  PATCH_SYNC_VAL_ON, 
  PATCH_NUM_SYNC_VALS 
};

enum
{
  PATCH_LEGACY_KEYSCALE_VAL_PITCH = 0, 
  PATCH_LEGACY_KEYSCALE_VAL_KEY, 
  PATCH_NUM_LEGACY_KEYSCALE_VALS 
};

enum
{
  PATCH_ALGORITHM_VAL_1C_CHAIN = 0, 
  PATCH_ALGORITHM_VAL_1C_THE_V, 
  PATCH_ALGORITHM_VAL_2C_1_TO_1, 
  PATCH_ALGORITHM_VAL_2C_1_TO_2, 
  PATCH_ALGORITHM_VAL_3C_PIPES, 
  PATCH_NUM_ALGORITHM_VALS 
};

enum
{
  PATCH_OSC_WAVEFORM_VAL_SINE = 0, 
  PATCH_OSC_WAVEFORM_VAL_HALF, 
  PATCH_OSC_WAVEFORM_VAL_FULL, 
  PATCH_OSC_WAVEFORM_VAL_QUARTER, 
  PATCH_NUM_OSC_WAVEFORM_VALS 
};

enum
{
  PATCH_OSC_FREQ_MODE_VAL_RATIO = 0, 
  PATCH_OSC_FREQ_MODE_VAL_FIXED, 
  PATCH_NUM_OSC_FREQ_MODE_VALS 
};

enum
{
  PATCH_LFO_WAVEFORM_VAL_TRIANGLE = 0, 
  PATCH_LFO_WAVEFORM_VAL_SQUARE, 
  PATCH_LFO_WAVEFORM_VAL_SAW_UP, 
  PATCH_LFO_WAVEFORM_VAL_SAW_DOWN, 
  PATCH_NUM_LFO_WAVEFORM_VALS 
};

enum
{
  PATCH_POLARITY_VAL_BI = 0, 
  PATCH_POLARITY_VAL_UNI, 
  PATCH_NUM_POLARITY_VALS 
};

enum
{
  PATCH_AM_MODE_VAL_CARRIERS = 0, 
  PATCH_AM_MODE_VAL_MODULATORS, 
  PATCH_NUM_AM_MODE_VALS 
};

#define PATCH_NUM_FEEDBACK_VALS       8
#define PATCH_NUM_OSC_MULTIPLE_VALS  16
#define PATCH_NUM_OSC_DIVISOR_VALS    8
#define PATCH_NUM_OSC_OCTAVE_VALS    10
#define PATCH_NUM_OSC_NOTE_VALS      12
#define PATCH_NUM_OSC_DETUNE_VALS     7
#define PATCH_NUM_ENV_TIME_VALS     100
#define PATCH_NUM_ENV_LEVEL_VALS    101
#define PATCH_NUM_KEYSCALING_VALS     4
#define PATCH_NUM_LFO_SPEED_VALS     16
#define PATCH_NUM_LFO_DELAY_VALS     49
#define PATCH_NUM_SENSITIVITY_VALS   16
#define PATCH_NUM_DEPTH_VALS        101
#define PATCH_NUM_CUTOFF_VALS         4

enum
{
  PATCH_PARAM_FEEDBACK = 0, 
  PATCH_PARAM_ALGORITHM, 
  PATCH_PARAM_OSC_SYNC, 
  PATCH_PARAM_LEGACY_KEYSCALE, 
  /* osc 1 */
  PATCH_PARAM_OSC_1_WAVEFORM, 
  PATCH_PARAM_OSC_1_FREQ_MODE, 
  PATCH_PARAM_OSC_1_MULTIPLE, 
  PATCH_PARAM_OSC_1_DIVISOR, 
  PATCH_PARAM_OSC_1_OCTAVE, 
  PATCH_PARAM_OSC_1_NOTE, 
  PATCH_PARAM_OSC_1_DETUNE, 
  /* osc 2 */
  PATCH_PARAM_OSC_2_WAVEFORM, 
  PATCH_PARAM_OSC_2_FREQ_MODE, 
  PATCH_PARAM_OSC_2_MULTIPLE, 
  PATCH_PARAM_OSC_2_DIVISOR, 
  PATCH_PARAM_OSC_2_OCTAVE, 
  PATCH_PARAM_OSC_2_NOTE, 
  PATCH_PARAM_OSC_2_DETUNE, 
  /* osc 3 */
  PATCH_PARAM_OSC_3_WAVEFORM, 
  PATCH_PARAM_OSC_3_FREQ_MODE, 
  PATCH_PARAM_OSC_3_MULTIPLE, 
  PATCH_PARAM_OSC_3_DIVISOR, 
  PATCH_PARAM_OSC_3_OCTAVE, 
  PATCH_PARAM_OSC_3_NOTE, 
  PATCH_PARAM_OSC_3_DETUNE, 
  /* env 1 */
  PATCH_PARAM_ENV_1_ATTACK, 
  PATCH_PARAM_ENV_1_DECAY, 
  PATCH_PARAM_ENV_1_SUSTAIN, 
  PATCH_PARAM_ENV_1_RELEASE, 
  PATCH_PARAM_ENV_1_MAX_LEVEL, 
  PATCH_PARAM_ENV_1_HOLD_LEVEL, 
  PATCH_PARAM_ENV_1_RATE_KS, 
  PATCH_PARAM_ENV_1_LEVEL_KS, 
  /* env 2 */
  PATCH_PARAM_ENV_2_ATTACK, 
  PATCH_PARAM_ENV_2_DECAY, 
  PATCH_PARAM_ENV_2_SUSTAIN, 
  PATCH_PARAM_ENV_2_RELEASE, 
  PATCH_PARAM_ENV_2_MAX_LEVEL, 
  PATCH_PARAM_ENV_2_HOLD_LEVEL, 
  PATCH_PARAM_ENV_2_RATE_KS, 
  PATCH_PARAM_ENV_2_LEVEL_KS, 
  /* env 3 */
  PATCH_PARAM_ENV_3_ATTACK, 
  PATCH_PARAM_ENV_3_DECAY, 
  PATCH_PARAM_ENV_3_SUSTAIN, 
  PATCH_PARAM_ENV_3_RELEASE, 
  PATCH_PARAM_ENV_3_MAX_LEVEL, 
  PATCH_PARAM_ENV_3_HOLD_LEVEL, 
  PATCH_PARAM_ENV_3_RATE_KS, 
  PATCH_PARAM_ENV_3_LEVEL_KS, 
  /* lfo */
  PATCH_PARAM_LFO_WAVEFORM, 
  PATCH_PARAM_LFO_SYNC, 
  PATCH_PARAM_LFO_SPEED, 
  PATCH_PARAM_LFO_DELAY, 
  /* vibrato */
  PATCH_PARAM_VIBRATO_POLARITY, 
  PATCH_PARAM_VIBRATO_SENSITIVITY, 
  PATCH_PARAM_VIBRATO_DEPTH, 
  /* tremolo */
  PATCH_PARAM_TREMOLO_MODE, 
  PATCH_PARAM_TREMOLO_SENSITIVITY, 
  PATCH_PARAM_TREMOLO_DEPTH, 
  /* boost */
  PATCH_PARAM_BOOST_MODE, 
  PATCH_PARAM_BOOST_SENSITIVITY, 
  /* filters */
  PATCH_PARAM_HIGHPASS_CUTOFF, 
  PATCH_PARAM_LOWPASS_CUTOFF, 
  PATCH_NUM_PARAMS
};

/* name strings: 12 characters + 1 null terminator */
#define CART_NAME_SIZE  (12 + 1)
#define PATCH_NAME_SIZE (12 + 1)

#define CART_CHARACTER_IS_VALID_IN_CART_OR_PATCH_NAME(c)                       \
  ( (c == ' ')                  ||                                             \
    (c == '\0')                 ||                                             \
    ((c >= '0') && (c <= '9'))  ||                                             \
    ((c >= 'A') && (c <= 'Z'))  ||                                             \
    ((c >= 'a') && (c <= 'z')))

/* shifts for oscillator and envelope params */
#define PATCH_PARAM_OSC_SHIFT (PATCH_PARAM_OSC_2_WAVEFORM - PATCH_PARAM_OSC_1_WAVEFORM)
#define PATCH_PARAM_ENV_SHIFT (PATCH_PARAM_ENV_2_ATTACK - PATCH_PARAM_ENV_1_ATTACK)

/* check if a parameter is within bounds */
#define PATCH_PARAM_IS_VALID_LOOKUP_BY_NAME(name)                              \
  ( (p->values[PATCH_PARAM_##name] >= 0) &&                                    \
    (p->values[PATCH_PARAM_##name] <= G_patch_param_bounds[PATCH_PARAM_##name]))

#define PATCH_VALUE_IS_VALID_LOOKUP_BY_NAME(val, name)                         \
  ((val >= 0) && (val <= G_patch_param_bounds[PATCH_PARAM_##name]))

/* cart parameters */
#define PATCH_CART_NUMBER_DEFAULT       1
#define PATCH_CART_NUMBER_LOWER_BOUND   1
#define PATCH_CART_NUMBER_UPPER_BOUND   BANK_NUM_CARTS
#define PATCH_CART_NUMBER_NUM_VALUES    (PATCH_CART_NUMBER_UPPER_BOUND - PATCH_CART_NUMBER_LOWER_BOUND + 1)

#define PATCH_PATCH_NUMBER_DEFAULT      1
#define PATCH_PATCH_NUMBER_LOWER_BOUND  1
#define PATCH_PATCH_NUMBER_UPPER_BOUND  BANK_PATCHES_PER_CART
#define PATCH_PATCH_NUMBER_NUM_VALUES   (PATCH_PATCH_NUMBER_UPPER_BOUND - PATCH_PATCH_NUMBER_LOWER_BOUND + 1)

typedef struct patch
{
  char name[PATCH_NAME_SIZE];

  unsigned char values[PATCH_NUM_PARAMS];
} patch;

typedef struct cart
{
  char name[CART_NAME_SIZE];

  patch patches[BANK_PATCHES_PER_CART];
} cart;

/* cart bank */
extern cart G_cart_bank[BANK_NUM_CARTS];

/* patch parameter bounds array */
extern unsigned char G_patch_param_bounds[PATCH_NUM_PARAMS];

/* function declarations */
short int cart_reset_all();

short int cart_reset_patch(int cart_index, int patch_index);
short int cart_validate_patch(int cart_index, int patch_index);
short int cart_copy_patch(int dest_cart_index,  int dest_patch_index, 
                          int src_cart_index,   int src_patch_index);

short int cart_reset_cart(int cart_index);
short int cart_validate_cart(int cart_index);
short int cart_copy_cart(int dest_cart_index, int src_cart_index);

#endif
