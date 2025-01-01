/*******************************************************************************
** cart.h (carts & patches)
*******************************************************************************/

#ifndef CART_H
#define CART_H

#include "bank.h"

enum
{
  PATCH_PARAM_FEEDBACK = 0, 
  PATCH_PARAM_ALGORITHM, 
  PATCH_PARAM_OSC_SYNC, 
  /* osc 1 */
  PATCH_PARAM_OSC_1_WAVEFORM, 
  PATCH_PARAM_OSC_1_FREQ_MODE, 
  PATCH_PARAM_OSC_1_MULTIPLE, 
  PATCH_PARAM_OSC_1_DIVISOR, 
  PATCH_PARAM_OSC_1_DETUNE, 
  /* osc 2 */
  PATCH_PARAM_OSC_2_WAVEFORM, 
  PATCH_PARAM_OSC_2_FREQ_MODE, 
  PATCH_PARAM_OSC_2_MULTIPLE, 
  PATCH_PARAM_OSC_2_DIVISOR, 
  PATCH_PARAM_OSC_2_DETUNE, 
  /* osc 3 */
  PATCH_PARAM_OSC_3_WAVEFORM, 
  PATCH_PARAM_OSC_3_FREQ_MODE, 
  PATCH_PARAM_OSC_3_MULTIPLE, 
  PATCH_PARAM_OSC_3_DIVISOR, 
  PATCH_PARAM_OSC_3_DETUNE, 
  /* osc 4 */
  PATCH_PARAM_OSC_4_WAVEFORM, 
  PATCH_PARAM_OSC_4_FREQ_MODE, 
  PATCH_PARAM_OSC_4_MULTIPLE, 
  PATCH_PARAM_OSC_4_DIVISOR, 
  PATCH_PARAM_OSC_4_DETUNE, 
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
  /* env 4 */
  PATCH_PARAM_ENV_4_ATTACK, 
  PATCH_PARAM_ENV_4_DECAY, 
  PATCH_PARAM_ENV_4_SUSTAIN, 
  PATCH_PARAM_ENV_4_RELEASE, 
  PATCH_PARAM_ENV_4_MAX_LEVEL, 
  PATCH_PARAM_ENV_4_HOLD_LEVEL, 
  PATCH_PARAM_ENV_4_RATE_KS, 
  PATCH_PARAM_ENV_4_LEVEL_KS, 
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
  /* velocity */
  PATCH_PARAM_VELOCITY_SENSITIVITY, 
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
