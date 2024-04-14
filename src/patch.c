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

#define PATCH_RESET_FLAGS(param, name)                                         \
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

  /* wave */
  PATCH_RESET_PARAMETER(wave_set,             WAVE_SET)
  PATCH_RESET_PARAMETER(wave_mix,             WAVE_MIX)

  /* extra */
  PATCH_RESET_PARAMETER(extra_mode,           EXTRA_MODE)

  PATCH_RESET_PARAMETER(subosc_waveform,      SUBOSC_WAVEFORM)
  PATCH_RESET_PARAMETER(subosc_mix,           SUBOSC_MIX)

  /* lowpass filter */
  PATCH_RESET_PARAMETER(lowpass_multiple,     LOWPASS_MULTIPLE)
  PATCH_RESET_PARAMETER(lowpass_resonance,    LOWPASS_RESONANCE)
  PATCH_RESET_PARAMETER(lowpass_keytracking,  LOWPASS_KEYTRACKING)

  /* highpass filter */
  PATCH_RESET_PARAMETER(highpass_cutoff,      HIGHPASS_CUTOFF)

  /* amplitude envelope */
  PATCH_RESET_PARAMETER(env_attack,           ENV_TIME)
  PATCH_RESET_PARAMETER(env_decay_1,          ENV_TIME)
  PATCH_RESET_PARAMETER(env_decay_2,          ENV_TIME)
  PATCH_RESET_PARAMETER(env_release,          ENV_TIME)
  PATCH_RESET_PARAMETER(env_sustain,          ENV_LEVEL)
  PATCH_RESET_PARAMETER(env_rate_ks,          ENV_KEYSCALING)
  PATCH_RESET_PARAMETER(env_level_ks,         ENV_KEYSCALING)
  PATCH_RESET_PARAMETER(env_break_point,      ENV_BREAK_POINT)

  /* filter / pitch / extra envelopes */
  for (m = 0; m < BANK_PEGS_PER_VOICE; m++)
  {
    PATCH_RESET_PARAMETER(peg_attack[m],      PEG_TIME)
    PATCH_RESET_PARAMETER(peg_decay[m],       PEG_TIME)
    PATCH_RESET_PARAMETER(peg_release[m],     PEG_TIME)
    PATCH_RESET_PARAMETER(peg_level[m],       PEG_LEVEL)
    PATCH_RESET_PARAMETER(peg_hold[m],        PEG_LEVEL)
    PATCH_RESET_PARAMETER(peg_finale[m],      PEG_LEVEL)
    PATCH_RESET_PARAMETER(peg_rate_ks[m],     PEG_KEYSCALING)
  }

  /* lfo */
  for (m = 0; m < BANK_LFOS_PER_VOICE; m++)
  {
    PATCH_RESET_PARAMETER(lfo_waveform[m],    LFO_WAVEFORM)
    PATCH_RESET_PARAMETER(lfo_delay[m],       LFO_DELAY)
    PATCH_RESET_PARAMETER(lfo_polarity[m],    LFO_POLARITY)
    PATCH_RESET_PARAMETER(lfo_frequency[m],   LFO_FREQUENCY)
    PATCH_RESET_PARAMETER(lfo_base[m],        LFO_BASE)
    PATCH_RESET_PARAMETER(lfo_depth[m],       LFO_DEPTH)
  }

  /* chorus */
  PATCH_RESET_PARAMETER(chorus_waveform,      CHORUS_WAVEFORM)
  PATCH_RESET_PARAMETER(chorus_frequency,     CHORUS_FREQUENCY)
  PATCH_RESET_PARAMETER(chorus_dry_wet,       CHORUS_DRY_WET)
  PATCH_RESET_PARAMETER(chorus_base,          CHORUS_BASE)
  PATCH_RESET_PARAMETER(chorus_depth,         CHORUS_DEPTH)

  /* arpeggio */
  PATCH_RESET_PARAMETER(arpeggio_mode,        ARPEGGIO_MODE)
  PATCH_RESET_PARAMETER(arpeggio_pattern,     ARPEGGIO_PATTERN)
  PATCH_RESET_PARAMETER(arpeggio_octaves,     ARPEGGIO_OCTAVES)
  PATCH_RESET_PARAMETER(arpeggio_speed,       ARPEGGIO_SPEED)

  /* portamento */
  PATCH_RESET_PARAMETER(portamento_mode,      PORTAMENTO_MODE)
  PATCH_RESET_PARAMETER(portamento_follow,    PORTAMENTO_FOLLOW)
  PATCH_RESET_PARAMETER(portamento_legato,    PORTAMENTO_LEGATO)
  PATCH_RESET_PARAMETER(portamento_speed,     PORTAMENTO_SPEED)

  /* sync */
  PATCH_RESET_PARAMETER(sync_osc,             SYNC)
  PATCH_RESET_PARAMETER(sync_lfo,             SYNC)

  /* bitcrush */
  PATCH_RESET_PARAMETER(bitcrush_env,         BITCRUSH_ENV)
  PATCH_RESET_PARAMETER(bitcrush_osc,         BITCRUSH_OSC)

  /* envelope bias */
  PATCH_RESET_PARAMETER(boost_depth,          BOOST_DEPTH)
  PATCH_RESET_PARAMETER(velocity_depth,       VELOCITY_DEPTH)

  /* pitch wheel */
  PATCH_RESET_PARAMETER(pitch_wheel_mode,     PITCH_WHEEL_MODE)
  PATCH_RESET_PARAMETER(pitch_wheel_range,    PITCH_WHEEL_RANGE)

  /* sustain pedal */
  PATCH_RESET_PARAMETER(pedal_adjust,         PEDAL_ADJUST)

  /* lfo routing */
  PATCH_RESET_FLAGS(lfo_routing,              LFO_ROUTING)

  /* envelope bias routing */
  PATCH_RESET_FLAGS(boost_routing,            ENV_BIAS_ROUTING)
  PATCH_RESET_FLAGS(velocity_routing,         ENV_BIAS_ROUTING)

  /* midi controller routing */
  PATCH_RESET_FLAGS(mod_wheel_routing,        MIDI_CONT_ROUTING)
  PATCH_RESET_FLAGS(aftertouch_routing,       MIDI_CONT_ROUTING)
  PATCH_RESET_FLAGS(exp_pedal_routing,        MIDI_CONT_ROUTING)

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

  /* wave */
  PATCH_BOUND_PARAMETER(wave_set,             WAVE_SET)
  PATCH_BOUND_PARAMETER(wave_mix,             WAVE_MIX)

  /* extra */
  PATCH_BOUND_PARAMETER(extra_mode,           EXTRA_MODE)

  PATCH_BOUND_PARAMETER(subosc_waveform,      SUBOSC_WAVEFORM)
  PATCH_BOUND_PARAMETER(subosc_mix,           SUBOSC_MIX)

  /* lowpass filter */
  PATCH_BOUND_PARAMETER(lowpass_multiple,     LOWPASS_MULTIPLE)
  PATCH_BOUND_PARAMETER(lowpass_resonance,    LOWPASS_RESONANCE)
  PATCH_BOUND_PARAMETER(lowpass_keytracking,  LOWPASS_KEYTRACKING)

  /* highpass filter */
  PATCH_BOUND_PARAMETER(highpass_cutoff,      HIGHPASS_CUTOFF)

  /* amplitude envelope */
  PATCH_BOUND_PARAMETER(env_attack,           ENV_TIME)
  PATCH_BOUND_PARAMETER(env_decay_1,          ENV_TIME)
  PATCH_BOUND_PARAMETER(env_decay_2,          ENV_TIME)
  PATCH_BOUND_PARAMETER(env_release,          ENV_TIME)
  PATCH_BOUND_PARAMETER(env_sustain,          ENV_LEVEL)
  PATCH_BOUND_PARAMETER(env_rate_ks,          ENV_KEYSCALING)
  PATCH_BOUND_PARAMETER(env_level_ks,         ENV_KEYSCALING)
  PATCH_BOUND_PARAMETER(env_break_point,      ENV_BREAK_POINT)

  /* filter / pitch / extra envelopes */
  for (m = 0; m < BANK_PEGS_PER_VOICE; m++)
  {
    PATCH_BOUND_PARAMETER(peg_attack[m],      PEG_TIME)
    PATCH_BOUND_PARAMETER(peg_decay[m],       PEG_TIME)
    PATCH_BOUND_PARAMETER(peg_release[m],     PEG_TIME)
    PATCH_BOUND_PARAMETER(peg_level[m],       PEG_LEVEL)
    PATCH_BOUND_PARAMETER(peg_hold[m],        PEG_LEVEL)
    PATCH_BOUND_PARAMETER(peg_finale[m],      PEG_LEVEL)
    PATCH_BOUND_PARAMETER(peg_rate_ks[m],     PEG_KEYSCALING)
  }

  /* lfo */
  for (m = 0; m < BANK_LFOS_PER_VOICE; m++)
  {
    PATCH_BOUND_PARAMETER(lfo_waveform[m],    LFO_WAVEFORM)
    PATCH_BOUND_PARAMETER(lfo_delay[m],       LFO_DELAY)
    PATCH_BOUND_PARAMETER(lfo_polarity[m],    LFO_POLARITY)
    PATCH_BOUND_PARAMETER(lfo_frequency[m],   LFO_FREQUENCY)
    PATCH_BOUND_PARAMETER(lfo_base[m],        LFO_BASE)
    PATCH_BOUND_PARAMETER(lfo_depth[m],       LFO_DEPTH)
  }

  /* chorus */
  PATCH_BOUND_PARAMETER(chorus_waveform,      CHORUS_WAVEFORM)
  PATCH_BOUND_PARAMETER(chorus_frequency,     CHORUS_FREQUENCY)
  PATCH_BOUND_PARAMETER(chorus_dry_wet,       CHORUS_DRY_WET)
  PATCH_BOUND_PARAMETER(chorus_base,          CHORUS_BASE)
  PATCH_BOUND_PARAMETER(chorus_depth,         CHORUS_DEPTH)

  /* arpeggio */
  PATCH_BOUND_PARAMETER(arpeggio_mode,        ARPEGGIO_MODE)
  PATCH_BOUND_PARAMETER(arpeggio_pattern,     ARPEGGIO_PATTERN)
  PATCH_BOUND_PARAMETER(arpeggio_octaves,     ARPEGGIO_OCTAVES)
  PATCH_BOUND_PARAMETER(arpeggio_speed,       ARPEGGIO_SPEED)

  /* portamento */
  PATCH_BOUND_PARAMETER(portamento_mode,      PORTAMENTO_MODE)
  PATCH_BOUND_PARAMETER(portamento_follow,    PORTAMENTO_FOLLOW)
  PATCH_BOUND_PARAMETER(portamento_legato,    PORTAMENTO_LEGATO)
  PATCH_BOUND_PARAMETER(portamento_speed,     PORTAMENTO_SPEED)

  /* sync */
  PATCH_BOUND_PARAMETER(sync_osc,             SYNC)
  PATCH_BOUND_PARAMETER(sync_lfo,             SYNC)

  /* bitcrush */
  PATCH_BOUND_PARAMETER(bitcrush_env,         BITCRUSH_ENV)
  PATCH_BOUND_PARAMETER(bitcrush_osc,         BITCRUSH_OSC)

  /* envelope bias */
  PATCH_BOUND_PARAMETER(boost_depth,          BOOST_DEPTH)
  PATCH_BOUND_PARAMETER(velocity_depth,       VELOCITY_DEPTH)

  /* pitch wheel */
  PATCH_BOUND_PARAMETER(pitch_wheel_mode,     PITCH_WHEEL_MODE)
  PATCH_BOUND_PARAMETER(pitch_wheel_range,    PITCH_WHEEL_RANGE)

  /* sustain pedal */
  PATCH_BOUND_PARAMETER(pedal_adjust,         PEDAL_ADJUST)

  /* lfo routing */
  PATCH_MASK_FLAGS(lfo_routing,               LFO_ROUTING)

  /* envelope bias routing */
  PATCH_MASK_FLAGS(boost_routing,             ENV_BIAS_ROUTING)
  PATCH_MASK_FLAGS(velocity_routing,          ENV_BIAS_ROUTING)

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

  /* wave */
  PATCH_COPY_PARAMETER(wave_set)
  PATCH_COPY_PARAMETER(wave_mix)

  /* extra */
  PATCH_COPY_PARAMETER(extra_mode)

  PATCH_COPY_PARAMETER(subosc_waveform)
  PATCH_COPY_PARAMETER(subosc_mix)

  /* lowpass filter */
  PATCH_COPY_PARAMETER(lowpass_multiple)
  PATCH_COPY_PARAMETER(lowpass_resonance)
  PATCH_COPY_PARAMETER(lowpass_keytracking)

  /* highpass filter */
  PATCH_COPY_PARAMETER(highpass_cutoff)

  /* amplitude envelope */
  PATCH_COPY_PARAMETER(env_attack)
  PATCH_COPY_PARAMETER(env_decay_1)
  PATCH_COPY_PARAMETER(env_decay_2)
  PATCH_COPY_PARAMETER(env_release)
  PATCH_COPY_PARAMETER(env_sustain)
  PATCH_COPY_PARAMETER(env_rate_ks)
  PATCH_COPY_PARAMETER(env_level_ks)
  PATCH_COPY_PARAMETER(env_break_point)

  /* filter / pitch / extra envelopes */
  for (m = 0; m < BANK_PEGS_PER_VOICE; m++)
  {
    PATCH_COPY_PARAMETER(peg_attack[m])
    PATCH_COPY_PARAMETER(peg_decay[m])
    PATCH_COPY_PARAMETER(peg_release[m])
    PATCH_COPY_PARAMETER(peg_level[m])
    PATCH_COPY_PARAMETER(peg_hold[m])
    PATCH_COPY_PARAMETER(peg_finale[m])
    PATCH_COPY_PARAMETER(peg_rate_ks[m])
  }

  /* lfo */
  for (m = 0; m < BANK_LFOS_PER_VOICE; m++)
  {
    PATCH_COPY_PARAMETER(lfo_waveform[m])
    PATCH_COPY_PARAMETER(lfo_delay[m])
    PATCH_COPY_PARAMETER(lfo_polarity[m])
    PATCH_COPY_PARAMETER(lfo_frequency[m])
    PATCH_COPY_PARAMETER(lfo_base[m])
    PATCH_COPY_PARAMETER(lfo_depth[m])
  }

  /* chorus */
  PATCH_COPY_PARAMETER(chorus_waveform)
  PATCH_COPY_PARAMETER(chorus_frequency)
  PATCH_COPY_PARAMETER(chorus_dry_wet)
  PATCH_COPY_PARAMETER(chorus_base)
  PATCH_COPY_PARAMETER(chorus_depth)

  /* arpeggio */
  PATCH_COPY_PARAMETER(arpeggio_mode)
  PATCH_COPY_PARAMETER(arpeggio_pattern)
  PATCH_COPY_PARAMETER(arpeggio_octaves)
  PATCH_COPY_PARAMETER(arpeggio_speed)

  /* portamento */
  PATCH_COPY_PARAMETER(portamento_mode)
  PATCH_COPY_PARAMETER(portamento_follow)
  PATCH_COPY_PARAMETER(portamento_legato)
  PATCH_COPY_PARAMETER(portamento_speed)

  /* sync */
  PATCH_COPY_PARAMETER(sync_osc)
  PATCH_COPY_PARAMETER(sync_lfo)

  /* bitcrush */
  PATCH_COPY_PARAMETER(bitcrush_env)
  PATCH_COPY_PARAMETER(bitcrush_osc)

  /* envelope bias */
  PATCH_COPY_PARAMETER(boost_depth)
  PATCH_COPY_PARAMETER(velocity_depth)

  /* pitch wheel */
  PATCH_COPY_PARAMETER(pitch_wheel_mode)
  PATCH_COPY_PARAMETER(pitch_wheel_range)

  /* sustain pedal */
  PATCH_COPY_PARAMETER(pedal_adjust)

  /* lfo routing */
  PATCH_COPY_FLAGS(lfo_routing)

  /* envelope bias routing */
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

