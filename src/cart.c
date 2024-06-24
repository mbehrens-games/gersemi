/*******************************************************************************
** cart.c (carts & patches)
*******************************************************************************/

#include <stdio.h>
#include <string.h>

#include "bank.h"
#include "cart.h"

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

/* cart bank */
cart G_cart_bank[BANK_NUM_CARTS];

/*******************************************************************************
** cart_reset_all()
*******************************************************************************/
short int cart_reset_all()
{
  int m;

  /* reset carts */
  for (m = 0; m < BANK_NUM_CARTS; m++)
    cart_reset_cart(m);

  return 0;
}

/*******************************************************************************
** cart_reset_patch()
*******************************************************************************/
short int cart_reset_patch(int cart_index, int patch_index)
{
  int m;

  cart* c;
  patch* p;

  /* make sure that the cart & patch indices are valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(cart_index))
    return 1;

  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain cart & patch pointers */
  c = &G_cart_bank[cart_index];
  p = &(c->patches[patch_index]);

  /* reset patch name */
  for (m = 0; m < PATCH_NAME_SIZE; m++)
    p->name[m] = '\0';

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
    PATCH_RESET_PARAMETER(osc_octave[m],      OSC_OCTAVE)
    PATCH_RESET_PARAMETER(osc_note[m],        OSC_NOTE)
    PATCH_RESET_PARAMETER(osc_detune[m],      OSC_DETUNE)

    PATCH_CLEAR_FLAGS(osc_routing[m],         OSC_ROUTING)
  }

  /* amplitude envelopes */
  for (m = 0; m < BANK_ENVELOPES_PER_VOICE; m++)
  {
    PATCH_RESET_PARAMETER(env_attack[m],      ENV_TIME)
    PATCH_RESET_PARAMETER(env_decay[m],       ENV_TIME)
    PATCH_RESET_PARAMETER(env_sustain[m],     ENV_TIME)
    PATCH_RESET_PARAMETER(env_release[m],     ENV_TIME)
    PATCH_RESET_PARAMETER(env_amplitude[m],   ENV_LEVEL)
    PATCH_RESET_PARAMETER(env_hold_level[m],  ENV_LEVEL)
    PATCH_RESET_PARAMETER(env_hold_mode[m],   ENV_HOLD_MODE)
    PATCH_RESET_PARAMETER(env_rate_ks[m],     ENV_KEYSCALING)
    PATCH_RESET_PARAMETER(env_level_ks[m],    ENV_KEYSCALING)

    PATCH_CLEAR_FLAGS(env_routing[m],         ENV_ROUTING)
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
    PATCH_RESET_PARAMETER(lfo_polarity[m],    LFO_POLARITY)
  }

  /* boost */
  PATCH_RESET_PARAMETER(boost_sensitivity,    SENSITIVITY)

  /* velocity */
  PATCH_RESET_PARAMETER(velocity_sensitivity, SENSITIVITY)

  /* filters */
  PATCH_RESET_PARAMETER(highpass_cutoff,      HIGHPASS_CUTOFF)
  PATCH_RESET_PARAMETER(lowpass_cutoff,       LOWPASS_CUTOFF)

  /* pitch envelope */
  PATCH_RESET_PARAMETER(peg_attack,           PEG_TIME)
  PATCH_RESET_PARAMETER(peg_decay,            PEG_TIME)
  PATCH_RESET_PARAMETER(peg_release,          PEG_TIME)
  PATCH_RESET_PARAMETER(peg_maximum,          PEG_LEVEL)
  PATCH_RESET_PARAMETER(peg_finale,           PEG_LEVEL)

  /* pitch wheel */
  PATCH_RESET_PARAMETER(pitch_wheel_mode,     PITCH_WHEEL_MODE)
  PATCH_RESET_PARAMETER(pitch_wheel_range,    PITCH_WHEEL_RANGE)

  /* arpeggio */
  PATCH_RESET_PARAMETER(arpeggio_mode,        ARPEGGIO_MODE)
  PATCH_RESET_PARAMETER(arpeggio_pattern,     ARPEGGIO_PATTERN)
  PATCH_RESET_PARAMETER(arpeggio_octaves,     ARPEGGIO_OCTAVES)
  PATCH_RESET_PARAMETER(arpeggio_speed,       ARPEGGIO_SPEED)

  /* portamento */
  PATCH_RESET_PARAMETER(portamento_mode,      PORTAMENTO_MODE)
  PATCH_RESET_PARAMETER(portamento_legato,    PORTAMENTO_LEGATO)
  PATCH_RESET_PARAMETER(portamento_speed,     PORTAMENTO_SPEED)

  /* midi controller routing */
  PATCH_CLEAR_FLAGS(mod_wheel_routing,        MIDI_CONT_ROUTING)
  PATCH_CLEAR_FLAGS(aftertouch_routing,       MIDI_CONT_ROUTING)
  PATCH_CLEAR_FLAGS(exp_pedal_routing,        MIDI_CONT_ROUTING)

  return 0;
}

/*******************************************************************************
** cart_validate_patch()
*******************************************************************************/
short int cart_validate_patch(int cart_index, int patch_index)
{
  int m;

  cart* c;
  patch* p;

  /* make sure that the cart & patch indices are valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(cart_index))
    return 1;

  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain cart & patch pointers */
  c = &G_cart_bank[cart_index];
  p = &(c->patches[patch_index]);

  /* validate patch name */
  p->name[PATCH_NAME_SIZE - 1] = '\0';

  for (m = PATCH_NAME_SIZE - 2; m >= 0; m--)
  {
    if ((p->name[m] == ' ') || (p->name[m] == '\0'))
      p->name[m] = '\0';
    else
      break;
  }

  for (m = 0; m < PATCH_NAME_SIZE; m++)
  {
    if (!(CART_CHARACTER_IS_VALID_IN_CART_OR_PATCH_NAME(p->name[m])))
      p->name[m] = ' ';
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
    PATCH_BOUND_PARAMETER(osc_octave[m],      OSC_OCTAVE)
    PATCH_BOUND_PARAMETER(osc_note[m],        OSC_NOTE)
    PATCH_BOUND_PARAMETER(osc_detune[m],      OSC_DETUNE)

    PATCH_MASK_FLAGS(osc_routing[m],          OSC_ROUTING)
  }

  /* amplitude envelopes */
  for (m = 0; m < BANK_ENVELOPES_PER_VOICE; m++)
  {
    PATCH_BOUND_PARAMETER(env_attack[m],      ENV_TIME)
    PATCH_BOUND_PARAMETER(env_decay[m],       ENV_TIME)
    PATCH_BOUND_PARAMETER(env_sustain[m],     ENV_TIME)
    PATCH_BOUND_PARAMETER(env_release[m],     ENV_TIME)
    PATCH_BOUND_PARAMETER(env_amplitude[m],   ENV_LEVEL)
    PATCH_BOUND_PARAMETER(env_hold_level[m],  ENV_LEVEL)
    PATCH_BOUND_PARAMETER(env_hold_mode[m],   ENV_HOLD_MODE)
    PATCH_BOUND_PARAMETER(env_rate_ks[m],     ENV_KEYSCALING)
    PATCH_BOUND_PARAMETER(env_level_ks[m],    ENV_KEYSCALING)

    PATCH_MASK_FLAGS(env_routing[m],          ENV_ROUTING)
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
    PATCH_BOUND_PARAMETER(lfo_polarity[m],    LFO_POLARITY)
  }

  /* boost */
  PATCH_BOUND_PARAMETER(boost_sensitivity,    SENSITIVITY)

  /* velocity */
  PATCH_BOUND_PARAMETER(velocity_sensitivity, SENSITIVITY)

  /* filters */
  PATCH_BOUND_PARAMETER(highpass_cutoff,      HIGHPASS_CUTOFF)
  PATCH_BOUND_PARAMETER(lowpass_cutoff,       LOWPASS_CUTOFF)

  /* pitch envelope */
  PATCH_BOUND_PARAMETER(peg_attack,           PEG_TIME)
  PATCH_BOUND_PARAMETER(peg_decay,            PEG_TIME)
  PATCH_BOUND_PARAMETER(peg_release,          PEG_TIME)
  PATCH_BOUND_PARAMETER(peg_maximum,          PEG_LEVEL)
  PATCH_BOUND_PARAMETER(peg_finale,           PEG_LEVEL)

  /* pitch wheel */
  PATCH_BOUND_PARAMETER(pitch_wheel_mode,     PITCH_WHEEL_MODE)
  PATCH_BOUND_PARAMETER(pitch_wheel_range,    PITCH_WHEEL_RANGE)

  /* arpeggio */
  PATCH_BOUND_PARAMETER(arpeggio_mode,        ARPEGGIO_MODE)
  PATCH_BOUND_PARAMETER(arpeggio_pattern,     ARPEGGIO_PATTERN)
  PATCH_BOUND_PARAMETER(arpeggio_octaves,     ARPEGGIO_OCTAVES)
  PATCH_BOUND_PARAMETER(arpeggio_speed,       ARPEGGIO_SPEED)

  /* portamento */
  PATCH_BOUND_PARAMETER(portamento_mode,      PORTAMENTO_MODE)
  PATCH_BOUND_PARAMETER(portamento_legato,    PORTAMENTO_LEGATO)
  PATCH_BOUND_PARAMETER(portamento_speed,     PORTAMENTO_SPEED)

  /* midi controller routing */
  PATCH_MASK_FLAGS(mod_wheel_routing,         MIDI_CONT_ROUTING)
  PATCH_MASK_FLAGS(aftertouch_routing,        MIDI_CONT_ROUTING)
  PATCH_MASK_FLAGS(exp_pedal_routing,         MIDI_CONT_ROUTING)

  return 0;
}

/*******************************************************************************
** cart_copy_patch()
*******************************************************************************/
short int cart_copy_patch( int dest_cart_index,  int dest_patch_index, 
                            int src_cart_index,   int src_patch_index)
{
  int m;

  cart* dest_c;
  patch* dest_p;

  cart* src_c;
  patch* src_p;

  /* make sure the destination and source indices are different */
  if ((dest_cart_index == src_cart_index) && 
      (dest_patch_index == src_patch_index))
  {
    return 1;
  }

  /* make sure that the cart & patch indices are valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(dest_cart_index))
    return 1;

  if (BANK_PATCH_INDEX_IS_NOT_VALID(dest_patch_index))
    return 1;

  if (BANK_CART_INDEX_IS_NOT_VALID(src_cart_index))
    return 1;

  if (BANK_PATCH_INDEX_IS_NOT_VALID(src_patch_index))
    return 1;

  /* obtain destination pointers */
  dest_c = &G_cart_bank[dest_cart_index];
  dest_p = &(dest_c->patches[dest_patch_index]);

  /* obtain source pointers */
  src_c = &G_cart_bank[src_cart_index];
  src_p = &(src_c->patches[src_patch_index]);

  /* copy patch name */
  strncpy(dest_p->name, src_p->name, PATCH_NAME_SIZE);

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
    PATCH_COPY_PARAMETER(osc_octave[m])
    PATCH_COPY_PARAMETER(osc_note[m])
    PATCH_COPY_PARAMETER(osc_detune[m])

    PATCH_COPY_FLAGS(osc_routing[m])
  }

  /* amplitude envelopes */
  for (m = 0; m < BANK_ENVELOPES_PER_VOICE; m++)
  {
    PATCH_COPY_PARAMETER(env_attack[m])
    PATCH_COPY_PARAMETER(env_decay[m])
    PATCH_COPY_PARAMETER(env_sustain[m])
    PATCH_COPY_PARAMETER(env_release[m])
    PATCH_COPY_PARAMETER(env_amplitude[m])
    PATCH_COPY_PARAMETER(env_hold_level[m])
    PATCH_COPY_PARAMETER(env_hold_mode[m])
    PATCH_COPY_PARAMETER(env_rate_ks[m])
    PATCH_COPY_PARAMETER(env_level_ks[m])

    PATCH_COPY_FLAGS(env_routing[m])
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
    PATCH_COPY_PARAMETER(lfo_polarity[m])
  }

  /* boost */
  PATCH_COPY_PARAMETER(boost_sensitivity)

  /* velocity */
  PATCH_COPY_PARAMETER(velocity_sensitivity)

  /* filters */
  PATCH_COPY_PARAMETER(highpass_cutoff)
  PATCH_COPY_PARAMETER(lowpass_cutoff)

  /* pitch envelope */
  PATCH_COPY_PARAMETER(peg_attack)
  PATCH_COPY_PARAMETER(peg_decay)
  PATCH_COPY_PARAMETER(peg_release)
  PATCH_COPY_PARAMETER(peg_maximum)
  PATCH_COPY_PARAMETER(peg_finale)

  /* pitch wheel */
  PATCH_COPY_PARAMETER(pitch_wheel_mode)
  PATCH_COPY_PARAMETER(pitch_wheel_range)

  /* arpeggio */
  PATCH_COPY_PARAMETER(arpeggio_mode)
  PATCH_COPY_PARAMETER(arpeggio_pattern)
  PATCH_COPY_PARAMETER(arpeggio_octaves)
  PATCH_COPY_PARAMETER(arpeggio_speed)

  /* portamento */
  PATCH_COPY_PARAMETER(portamento_mode)
  PATCH_COPY_PARAMETER(portamento_legato)
  PATCH_COPY_PARAMETER(portamento_speed)

  /* midi controller routing */
  PATCH_COPY_FLAGS(mod_wheel_routing)
  PATCH_COPY_FLAGS(aftertouch_routing)
  PATCH_COPY_FLAGS(exp_pedal_routing)

  return 0;
}

/*******************************************************************************
** cart_reset_cart()
*******************************************************************************/
short int cart_reset_cart(int cart_index)
{
  int m;

  cart* c;

  /* make sure that the cart index is valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(cart_index))
    return 1;

  /* obtain cart pointer */
  c = &G_cart_bank[cart_index];

  /* reset cart name */
  for (m = 0; m < CART_NAME_SIZE; m++)
    c->name[m] = '\0';

  /* reset all patches in cart */
  for (m = 0; m < BANK_PATCHES_PER_CART; m++)
    cart_reset_patch(cart_index, m);

  return 0;
}

/*******************************************************************************
** cart_validate_cart()
*******************************************************************************/
short int cart_validate_cart(int cart_index)
{
  int m;

  cart* c;

  /* make sure that the cart index is valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(cart_index))
    return 1;

  /* obtain cart pointer */
  c = &G_cart_bank[cart_index];

  /* validate cart name */
  c->name[CART_NAME_SIZE - 1] = '\0';

  for (m = CART_NAME_SIZE - 2; m >= 0; m--)
  {
    if ((c->name[m] == ' ') || (c->name[m] == '\0'))
      c->name[m] = '\0';
    else
      break;
  }

  for (m = 0; m < CART_NAME_SIZE; m++)
  {
    if (!(CART_CHARACTER_IS_VALID_IN_CART_OR_PATCH_NAME(c->name[m])))
      c->name[m] = ' ';
  }

  /* validate all patches in cart */
  for (m = 0; m < BANK_PATCHES_PER_CART; m++)
    cart_validate_patch(cart_index, m);

  return 0;
}

/*******************************************************************************
** cart_copy_cart()
*******************************************************************************/
short int cart_copy_cart(int dest_cart_index, int src_cart_index)
{
  int m;

  cart* dest_c;
  cart* src_c;

  /* make sure the destination and source indices are different */
  if (dest_cart_index == src_cart_index)
    return 1;

  /* make sure that the cart indices are valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(dest_cart_index))
    return 1;

  if (BANK_CART_INDEX_IS_NOT_VALID(src_cart_index))
    return 1;

  /* obtain source & destination pointers */
  dest_c = &G_cart_bank[dest_cart_index];
  src_c = &G_cart_bank[src_cart_index];

  /* copy cart name */
  strncpy(dest_c->name, src_c->name, CART_NAME_SIZE);

  /* copy all patches in cart */
  for (m = 0; m < BANK_PATCHES_PER_CART; m++)
    cart_copy_patch(dest_cart_index, m, src_cart_index, m);

  return 0;
}

