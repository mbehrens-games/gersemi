/*******************************************************************************
** patch.c (patch settings)
*******************************************************************************/

#include <stdio.h>
#include <string.h>

#include "bank.h"
#include "patch.h"
#include "text.h"

#define PATCH_RESET_PARAMETER(param, name)                                     \
  p->param = PATCH_##name##_DEFAULT;

#define PATCH_BOUND_PARAMETER(param, name)                                     \
  if (p->param < PATCH_##name##_LOWER_BOUND)                                   \
    p->param = PATCH_##name##_LOWER_BOUND;                                     \
  else if (p->param > PATCH_##name##_UPPER_BOUND)                              \
    p->param = PATCH_##name##_UPPER_BOUND;

#define PATCH_COPY_PARAMETER(param)                                            \
  dest_p->param = src_p->param;

#define PATCH_CLEAR_FLAGS(param, name)                                         \
  p->param = PATCH_##name##_CLEAR;

#define PATCH_MASK_FLAGS(param, name)                                          \
  p->param &= PATCH_##name##_MASK;

#define PATCH_COPY_FLAGS(param)                                                \
  dest_p->param = src_p->param;

/* name & patch banks */
char  G_cart_names[BANK_NUM_CARTS][PATCH_CART_NAME_SIZE];
char  G_patch_names[BANK_NUM_PATCHES][PATCH_PATCH_NAME_SIZE];

patch G_patch_bank[BANK_NUM_PATCHES];

/*******************************************************************************
** patch_reset_all()
*******************************************************************************/
short int patch_reset_all()
{
  int k;
  int m;

  /* reset cart names */
  for (k = 0; k < BANK_NUM_CARTS; k++)
  {
    for (m = 0; m < PATCH_CART_NAME_SIZE; m++)
    {
      G_cart_names[k][m] = '\0';
    }
  }

  /* reset patch names */
  for (k = 0; k < BANK_NUM_PATCHES; k++)
  {
    for (m = 0; m < PATCH_PATCH_NAME_SIZE; m++)
    {
      G_patch_names[k][m] = '\0';
    }
  }

  /* reset patches */
  for (k = 0; k < BANK_NUM_PATCHES; k++)
  {
    patch_reset_patch(k);
  }

  return 0;
}

/*******************************************************************************
** patch_reset_patch()
*******************************************************************************/
short int patch_reset_patch(int patch_index)
{
  int m;

  patch* p;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain patch pointer */
  p = &G_patch_bank[patch_index];

  /* reset patch name */
  for (m = 0; m < PATCH_PATCH_NAME_SIZE; m++)
    G_patch_names[patch_index][m] = '\0';

  /* algorithm */
  PATCH_RESET_PARAMETER(algorithm,            ALGORITHM)

  /* oscillator sync */
  PATCH_RESET_PARAMETER(osc_sync,             SYNC)

  /* oscillators */
  for (m = 0; m < BANK_OSCILLATORS_PER_VOICE; m++)
  {
    PATCH_RESET_PARAMETER(osc_waveform[m],    OSC_WAVEFORM)
    PATCH_RESET_PARAMETER(osc_phi[m],         OSC_PHI)

    PATCH_RESET_PARAMETER(osc_freq_mode[m],   OSC_FREQ_MODE)
    PATCH_RESET_PARAMETER(osc_multiple[m],    OSC_MULTIPLE)
    PATCH_RESET_PARAMETER(osc_divisor[m],     OSC_DIVISOR)
    PATCH_RESET_PARAMETER(osc_detune[m],      OSC_DETUNE)
  }

  /* amplitude envelopes */
  for (m = 0; m < BANK_ENVELOPES_PER_VOICE; m++)
  {
    PATCH_RESET_PARAMETER(env_attack[m],      ENV_TIME)
    PATCH_RESET_PARAMETER(env_decay[m],       ENV_TIME)
    PATCH_RESET_PARAMETER(env_release[m],     ENV_TIME)
    PATCH_RESET_PARAMETER(env_amplitude[m],   ENV_LEVEL)
    PATCH_RESET_PARAMETER(env_sustain[m],     ENV_LEVEL)
    PATCH_RESET_PARAMETER(env_hold[m],        ENV_HOLD)
    PATCH_RESET_PARAMETER(env_pedal[m],       ENV_HOLD)
    PATCH_RESET_PARAMETER(env_rate_ks[m],     ENV_KEYSCALING)
    PATCH_RESET_PARAMETER(env_level_ks[m],    ENV_KEYSCALING)
  }

  /* lfos */
  for (m = 0; m < BANK_LFOS_PER_VOICE; m++)
  {
    PATCH_RESET_PARAMETER(lfo_waveform[m],    LFO_WAVEFORM)
    PATCH_RESET_PARAMETER(lfo_delay[m],       LFO_DELAY)
    PATCH_RESET_PARAMETER(lfo_speed[m],       LFO_SPEED)
    PATCH_RESET_PARAMETER(lfo_depth[m],       LFO_DEPTH)
    PATCH_RESET_PARAMETER(lfo_sensitivity[m], SENSITIVITY)
    PATCH_RESET_PARAMETER(lfo_sync[m],        SYNC)
  }

  /* arpeggio */
  PATCH_RESET_PARAMETER(arpeggio_mode,        ARPEGGIO_MODE)
  PATCH_RESET_PARAMETER(arpeggio_pattern,     ARPEGGIO_PATTERN)
  PATCH_RESET_PARAMETER(arpeggio_octaves,     ARPEGGIO_OCTAVES)
  PATCH_RESET_PARAMETER(arpeggio_speed,       ARPEGGIO_SPEED)

  /* boost */
  PATCH_RESET_PARAMETER(boost_sensitivity,    SENSITIVITY)

  /* velocity */
  PATCH_RESET_PARAMETER(velocity_sensitivity, SENSITIVITY)

  /* pitch envelope */
  PATCH_RESET_PARAMETER(peg_attack,           PEG_TIME)
  PATCH_RESET_PARAMETER(peg_decay,            PEG_TIME)
  PATCH_RESET_PARAMETER(peg_release,          PEG_TIME)
  PATCH_RESET_PARAMETER(peg_maximum,          PEG_LEVEL)
  PATCH_RESET_PARAMETER(peg_finale,           PEG_LEVEL)

  /* portamento */
  PATCH_RESET_PARAMETER(portamento_mode,      PORTAMENTO_MODE)
  PATCH_RESET_PARAMETER(portamento_legato,    PORTAMENTO_LEGATO)
  PATCH_RESET_PARAMETER(portamento_speed,     PORTAMENTO_SPEED)

  /* pitch wheel */
  PATCH_RESET_PARAMETER(pitch_wheel_mode,     PITCH_WHEEL_MODE)
  PATCH_RESET_PARAMETER(pitch_wheel_range,    PITCH_WHEEL_RANGE)

  /* filters */
  PATCH_RESET_PARAMETER(highpass_cutoff,      HIGHPASS_CUTOFF)
  PATCH_RESET_PARAMETER(lowpass_cutoff,       LOWPASS_CUTOFF)

  /* envelope adjustment routing */
  PATCH_CLEAR_FLAGS(tremolo_routing,          ENV_ADJUST_ROUTING)
  PATCH_CLEAR_FLAGS(boost_routing,            ENV_ADJUST_ROUTING)
  PATCH_CLEAR_FLAGS(velocity_routing,         ENV_ADJUST_ROUTING)

  /* midi controller routing */
  PATCH_CLEAR_FLAGS(mod_wheel_routing,        MIDI_CONT_ROUTING)
  PATCH_CLEAR_FLAGS(aftertouch_routing,       MIDI_CONT_ROUTING)
  PATCH_CLEAR_FLAGS(exp_pedal_routing,        MIDI_CONT_ROUTING)

  return 0;
}

/*******************************************************************************
** patch_validate_patch()
*******************************************************************************/
short int patch_validate_patch(int patch_index)
{
  int m;

  patch* p;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain patch pointer */
  p = &G_patch_bank[patch_index];

  /* validate patch name */
  G_patch_names[patch_index][PATCH_PATCH_NAME_SIZE - 1] = '\0';

  for (m = PATCH_PATCH_NAME_SIZE - 2; m >= 0; m--)
  {
    if ((G_patch_names[patch_index][m] == ' ') || (G_patch_names[patch_index][m] == '\0'))
      G_patch_names[patch_index][m] = '\0';
    else
      break;
  }

  for (m = 0; m < PATCH_PATCH_NAME_SIZE; m++)
  {
    if (!(TEXT_CHARACTER_IS_VALID_IN_PATCH_NAME(G_patch_names[patch_index][m])))
      G_patch_names[patch_index][m] = ' ';
  }

  /* algorithm */
  PATCH_BOUND_PARAMETER(algorithm,            ALGORITHM)

  /* oscillator sync */
  PATCH_BOUND_PARAMETER(osc_sync,             SYNC)

  /* oscillators */
  for (m = 0; m < BANK_OSCILLATORS_PER_VOICE; m++)
  {
    PATCH_BOUND_PARAMETER(osc_waveform[m],    OSC_WAVEFORM)
    PATCH_BOUND_PARAMETER(osc_phi[m],         OSC_PHI)

    PATCH_BOUND_PARAMETER(osc_freq_mode[m],   OSC_FREQ_MODE)
    PATCH_BOUND_PARAMETER(osc_multiple[m],    OSC_MULTIPLE)
    PATCH_BOUND_PARAMETER(osc_divisor[m],     OSC_DIVISOR)
    PATCH_BOUND_PARAMETER(osc_detune[m],      OSC_DETUNE)
  }

  /* amplitude envelopes */
  for (m = 0; m < BANK_ENVELOPES_PER_VOICE; m++)
  {
    PATCH_BOUND_PARAMETER(env_attack[m],      ENV_TIME)
    PATCH_BOUND_PARAMETER(env_decay[m],       ENV_TIME)
    PATCH_BOUND_PARAMETER(env_release[m],     ENV_TIME)
    PATCH_BOUND_PARAMETER(env_amplitude[m],   ENV_LEVEL)
    PATCH_BOUND_PARAMETER(env_sustain[m],     ENV_LEVEL)
    PATCH_BOUND_PARAMETER(env_hold[m],        ENV_HOLD)
    PATCH_BOUND_PARAMETER(env_pedal[m],       ENV_HOLD)
    PATCH_BOUND_PARAMETER(env_rate_ks[m],     ENV_KEYSCALING)
    PATCH_BOUND_PARAMETER(env_level_ks[m],    ENV_KEYSCALING)
  }

  /* lfos */
  for (m = 0; m < BANK_LFOS_PER_VOICE; m++)
  {
    PATCH_BOUND_PARAMETER(lfo_waveform[m],    LFO_WAVEFORM)
    PATCH_BOUND_PARAMETER(lfo_delay[m],       LFO_DELAY)
    PATCH_BOUND_PARAMETER(lfo_speed[m],       LFO_SPEED)
    PATCH_BOUND_PARAMETER(lfo_depth[m],       LFO_DEPTH)
    PATCH_BOUND_PARAMETER(lfo_sensitivity[m], SENSITIVITY)
    PATCH_BOUND_PARAMETER(lfo_sync[m],        SYNC)
  }

  /* arpeggio */
  PATCH_BOUND_PARAMETER(arpeggio_mode,        ARPEGGIO_MODE)
  PATCH_BOUND_PARAMETER(arpeggio_pattern,     ARPEGGIO_PATTERN)
  PATCH_BOUND_PARAMETER(arpeggio_octaves,     ARPEGGIO_OCTAVES)
  PATCH_BOUND_PARAMETER(arpeggio_speed,       ARPEGGIO_SPEED)

  /* boost */
  PATCH_BOUND_PARAMETER(boost_sensitivity,    SENSITIVITY)

  /* velocity */
  PATCH_BOUND_PARAMETER(velocity_sensitivity, SENSITIVITY)

  /* pitch envelope */
  PATCH_BOUND_PARAMETER(peg_attack,           PEG_TIME)
  PATCH_BOUND_PARAMETER(peg_decay,            PEG_TIME)
  PATCH_BOUND_PARAMETER(peg_release,          PEG_TIME)
  PATCH_BOUND_PARAMETER(peg_maximum,          PEG_LEVEL)
  PATCH_BOUND_PARAMETER(peg_finale,           PEG_LEVEL)

  /* portamento */
  PATCH_BOUND_PARAMETER(portamento_mode,      PORTAMENTO_MODE)
  PATCH_BOUND_PARAMETER(portamento_legato,    PORTAMENTO_LEGATO)
  PATCH_BOUND_PARAMETER(portamento_speed,     PORTAMENTO_SPEED)

  /* pitch wheel */
  PATCH_BOUND_PARAMETER(pitch_wheel_mode,     PITCH_WHEEL_MODE)
  PATCH_BOUND_PARAMETER(pitch_wheel_range,    PITCH_WHEEL_RANGE)

  /* filters */
  PATCH_BOUND_PARAMETER(highpass_cutoff,      HIGHPASS_CUTOFF)
  PATCH_BOUND_PARAMETER(lowpass_cutoff,       LOWPASS_CUTOFF)

  /* envelope adjustment routing */
  PATCH_MASK_FLAGS(tremolo_routing,           ENV_ADJUST_ROUTING)
  PATCH_MASK_FLAGS(boost_routing,             ENV_ADJUST_ROUTING)
  PATCH_MASK_FLAGS(velocity_routing,          ENV_ADJUST_ROUTING)

  /* midi controller routing */
  PATCH_MASK_FLAGS(mod_wheel_routing,         MIDI_CONT_ROUTING)
  PATCH_MASK_FLAGS(aftertouch_routing,        MIDI_CONT_ROUTING)
  PATCH_MASK_FLAGS(exp_pedal_routing,         MIDI_CONT_ROUTING)

  return 0;
}

/*******************************************************************************
** patch_copy_patch()
*******************************************************************************/
short int patch_copy_patch( int dest_patch_index, int src_patch_index)
{
  int m;

  patch* dest_p;
  patch* src_p;

  char* dest_name;
  char* src_name;

  /* make sure the destination and source indices are different */
  if (dest_patch_index == src_patch_index)
    return 1;

  /* make sure that the patch indices is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(dest_patch_index))
    return 1;

  if (BANK_PATCH_INDEX_IS_NOT_VALID(src_patch_index))
    return 1;

  /* obtain destination pointers */
  dest_p = &G_patch_bank[dest_patch_index];
  dest_name = &G_patch_names[dest_patch_index][0];

  /* obtain source pointers */
  src_p = &G_patch_bank[src_patch_index];
  src_name = &G_patch_names[src_patch_index][0];

  /* copy patch name */
  strncpy(dest_name, src_name, PATCH_PATCH_NAME_SIZE);

  /* algorithm */
  PATCH_COPY_PARAMETER(algorithm)

  /* oscillator sync */
  PATCH_COPY_PARAMETER(osc_sync)

  /* oscillators */
  for (m = 0; m < BANK_OSCILLATORS_PER_VOICE; m++)
  {
    PATCH_COPY_PARAMETER(osc_waveform[m])
    PATCH_COPY_PARAMETER(osc_phi[m])

    PATCH_COPY_PARAMETER(osc_freq_mode[m])
    PATCH_COPY_PARAMETER(osc_multiple[m])
    PATCH_COPY_PARAMETER(osc_divisor[m])
    PATCH_COPY_PARAMETER(osc_detune[m])
  }

  /* amplitude envelopes */
  for (m = 0; m < BANK_ENVELOPES_PER_VOICE; m++)
  {
    PATCH_COPY_PARAMETER(env_attack[m])
    PATCH_COPY_PARAMETER(env_decay[m])
    PATCH_COPY_PARAMETER(env_release[m])
    PATCH_COPY_PARAMETER(env_amplitude[m])
    PATCH_COPY_PARAMETER(env_sustain[m])
    PATCH_COPY_PARAMETER(env_hold[m])
    PATCH_COPY_PARAMETER(env_pedal[m])
    PATCH_COPY_PARAMETER(env_rate_ks[m])
    PATCH_COPY_PARAMETER(env_level_ks[m])
  }

  /* lfos */
  for (m = 0; m < BANK_LFOS_PER_VOICE; m++)
  {
    PATCH_COPY_PARAMETER(lfo_waveform[m])
    PATCH_COPY_PARAMETER(lfo_delay[m])
    PATCH_COPY_PARAMETER(lfo_speed[m])
    PATCH_COPY_PARAMETER(lfo_depth[m])
    PATCH_COPY_PARAMETER(lfo_sensitivity[m])
    PATCH_COPY_PARAMETER(lfo_sync[m])
  }

  /* arpeggio */
  PATCH_COPY_PARAMETER(arpeggio_mode)
  PATCH_COPY_PARAMETER(arpeggio_pattern)
  PATCH_COPY_PARAMETER(arpeggio_octaves)
  PATCH_COPY_PARAMETER(arpeggio_speed)

  /* boost */
  PATCH_COPY_PARAMETER(boost_sensitivity)

  /* velocity */
  PATCH_COPY_PARAMETER(velocity_sensitivity)

  /* pitch envelope */
  PATCH_COPY_PARAMETER(peg_attack)
  PATCH_COPY_PARAMETER(peg_decay)
  PATCH_COPY_PARAMETER(peg_release)
  PATCH_COPY_PARAMETER(peg_maximum)
  PATCH_COPY_PARAMETER(peg_finale)

  /* portamento */
  PATCH_COPY_PARAMETER(portamento_mode)
  PATCH_COPY_PARAMETER(portamento_legato)
  PATCH_COPY_PARAMETER(portamento_speed)

  /* pitch wheel */
  PATCH_COPY_PARAMETER(pitch_wheel_mode)
  PATCH_COPY_PARAMETER(pitch_wheel_range)

  /* filters */
  PATCH_COPY_PARAMETER(highpass_cutoff)
  PATCH_COPY_PARAMETER(lowpass_cutoff)

  /* envelope adjustment routing */
  PATCH_COPY_FLAGS(tremolo_routing)
  PATCH_COPY_FLAGS(boost_routing)
  PATCH_COPY_FLAGS(velocity_routing)

  /* midi controller routing */
  PATCH_COPY_FLAGS(mod_wheel_routing)
  PATCH_COPY_FLAGS(aftertouch_routing)
  PATCH_COPY_FLAGS(exp_pedal_routing)

  return 0;
}

/*******************************************************************************
** patch_reset_cart()
*******************************************************************************/
short int patch_reset_cart(int cart_index)
{
  int m;

  int patch_index;

  /* make sure that the cart index is valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(cart_index))
    return 1;

  /* reset cart name */
  for (m = 0; m < PATCH_CART_NAME_SIZE; m++)
    G_cart_names[cart_index][m] = '\0';

  /* reset all patches in cart */
  for (m = 0; m < BANK_PATCHES_PER_CART; m++)
  {
    patch_index = (cart_index * BANK_PATCHES_PER_CART) + m;

    patch_reset_patch(patch_index);
  }

  return 0;
}

/*******************************************************************************
** patch_validate_cart()
*******************************************************************************/
short int patch_validate_cart(int cart_index)
{
  int m;

  int patch_index;

  /* make sure that the cart index is valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(cart_index))
    return 1;

  /* validate cart name */
  G_cart_names[cart_index][PATCH_CART_NAME_SIZE - 1] = '\0';

  for (m = PATCH_CART_NAME_SIZE - 2; m >= 0; m--)
  {
    if ((G_cart_names[cart_index][m] == ' ') || (G_cart_names[cart_index][m] == '\0'))
      G_cart_names[cart_index][m] = '\0';
    else
      break;
  }

  for (m = 0; m < PATCH_CART_NAME_SIZE; m++)
  {
    if (!(TEXT_CHARACTER_IS_VALID_IN_CART_NAME(G_cart_names[cart_index][m])))
      G_cart_names[cart_index][m] = ' ';
  }

  /* validate all patches in cart */
  for (m = 0; m < BANK_PATCHES_PER_CART; m++)
  {
    patch_index = (cart_index * BANK_PATCHES_PER_CART) + m;

    patch_validate_patch(patch_index);
  }

  return 0;
}

/*******************************************************************************
** patch_copy_cart()
*******************************************************************************/
short int patch_copy_cart(int dest_cart_index, int src_cart_index)
{
  int m;

  char* dest_name;
  char* src_name;

  int dest_patch_index;
  int src_patch_index;

  /* make sure the destination and source indices are different */
  if (dest_cart_index == src_cart_index)
    return 1;

  /* make sure the cart indices are valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(dest_cart_index))
    return 1;

  if (BANK_CART_INDEX_IS_NOT_VALID(src_cart_index))
    return 1;

  /* determine cart name pointers */
  dest_name = &G_cart_names[dest_cart_index][0];
  src_name = &G_cart_names[src_cart_index][0];

  /* copy cart name */
  strncpy(dest_name, src_name, PATCH_CART_NAME_SIZE);

  /* copy all patches in cart */
  for (m = 0; m < BANK_PATCHES_PER_CART; m++)
  {
    dest_patch_index = (dest_cart_index * BANK_PATCHES_PER_CART) + m;
    src_patch_index = (src_cart_index * BANK_PATCHES_PER_CART) + m;

    patch_copy_patch(dest_patch_index, src_patch_index);
  }

  return 0;
}

