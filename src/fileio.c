/*******************************************************************************
** fileio.c (loading and saving from native file formats)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "bank.h"
#include "fileio.h"
#include "patch.h"

#define FILEIO_PATCH_COMPUTE_NAME_INDEX(patch_num)                             \
  ( FILEIO_CART_NAME_SIZE +                                                    \
    (patch_num * FILEIO_PATCH_NUM_BYTES))

#define FILEIO_PATCH_COMPUTE_BYTE_INDEX(patch_num, byte)                       \
  ( FILEIO_CART_NAME_SIZE +                                                    \
    (patch_num * FILEIO_PATCH_NUM_BYTES) +                                     \
    FILEIO_PATCH_BYTE_##byte)

/*******************************************************************************
** fileio_cart_load()
*******************************************************************************/
short int fileio_cart_load(int cart_num, char* filename)
{
  int k;

  FILE* fp;
  char  signature[4];
  char  type[4];

  unsigned char cart_data[FILEIO_CART_NUM_BYTES];

  unsigned char current_byte;

  patch* p;

  int patch_index;

  /* make sure filename is valid */
  if (filename == NULL)
    return 0;

  /* make sure cart number is valid */
  if (PATCH_CART_NO_IS_NOT_VALID(cart_num))
    return 0;

  /* open cart file */
  fp = fopen(filename, "rb");

  /* if file did not open, return */
  if (fp == NULL)
    return 0;

  /* read signature */
  if (fread(signature, 1, 4, fp) < 4)
  {
    fclose(fp);
    return 1;
  }

  if ((signature[0] != 'G') || 
      (signature[1] != 'E') || 
      (signature[2] != 'R') || 
      (signature[3] != 'S'))
  {
    fclose(fp);
    return 1;
  }

  /* read type */
  if (fread(type, 1, 4, fp) < 4)
  {
    fclose(fp);
    return 1;
  }

  if ((type[0] != 'C') || 
      (type[1] != 'A') || 
      (type[2] != 'R') || 
      (type[3] != 'T'))
  {
    fclose(fp);
    return 1;
  }

  /* read cart data */
  if (fread(cart_data, 1, FILEIO_CART_NUM_BYTES, fp) == 0)
  {
    fclose(fp);
    return 1;
  }

  /* close cart file */
  fclose(fp);

  /* load cart data */
  for (k = 0; k < BANK_PATCHES_PER_CART; k++)
  {
    PATCH_COMPUTE_PATCH_INDEX(cart_num, (PATCH_PATCH_NO_LOWER_BOUND + k))

    p = &G_patch_bank[patch_index];

    /* reset patch */
    patch_reset_patch(patch_index);

    /* osc frequency mode, detune, waveform */
    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, OSC_1_FREQ_MODE_DETUNE_WAVEFORM)];

    p->osc_freq_mode[0] = PATCH_OSC_FREQ_MODE_LOWER_BOUND + ((current_byte & 0x80) >> 7);
    p->osc_detune[0] = PATCH_OSC_DETUNE_LOWER_BOUND + ((current_byte & 0x70) >> 4);
    p->osc_waveform[0] = PATCH_OSC_WAVEFORM_LOWER_BOUND + (current_byte & 0x0F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, OSC_2_FREQ_MODE_DETUNE_WAVEFORM)];

    p->osc_freq_mode[1] = PATCH_OSC_FREQ_MODE_LOWER_BOUND + ((current_byte & 0x80) >> 7);
    p->osc_detune[1] = PATCH_OSC_DETUNE_LOWER_BOUND + ((current_byte & 0x70) >> 4);
    p->osc_waveform[1] = PATCH_OSC_WAVEFORM_LOWER_BOUND + (current_byte & 0x0F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, OSC_3_FREQ_MODE_DETUNE_WAVEFORM)];

    p->osc_freq_mode[2] = PATCH_OSC_FREQ_MODE_LOWER_BOUND + ((current_byte & 0x80) >> 7);
    p->osc_detune[2] = PATCH_OSC_DETUNE_LOWER_BOUND + ((current_byte & 0x70) >> 4);
    p->osc_waveform[2] = PATCH_OSC_WAVEFORM_LOWER_BOUND + (current_byte & 0x0F);

    /* osc multiple, divisor */
    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, OSC_1_MULTIPLE_DIVISOR)];

    if (p->osc_freq_mode[0] == PATCH_OSC_FREQ_MODE_FIXED)
    {
      p->osc_octave[0] = PATCH_OSC_OCTAVE_LOWER_BOUND + ((current_byte & 0xF0) >> 4);
      p->osc_note[0] = PATCH_OSC_NOTE_LOWER_BOUND + (current_byte & 0x0F);
    }
    else
    {
      p->osc_multiple[0] = PATCH_OSC_MULTIPLE_LOWER_BOUND + ((current_byte & 0xF0) >> 4);
      p->osc_divisor[0] = PATCH_OSC_DIVISOR_LOWER_BOUND + (current_byte & 0x0F);
    }

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, OSC_2_MULTIPLE_DIVISOR)];

    if (p->osc_freq_mode[1] == PATCH_OSC_FREQ_MODE_FIXED)
    {
      p->osc_octave[1] = PATCH_OSC_OCTAVE_LOWER_BOUND + ((current_byte & 0xF0) >> 4);
      p->osc_note[1] = PATCH_OSC_NOTE_LOWER_BOUND + (current_byte & 0x0F);
    }
    else
    {
      p->osc_multiple[1] = PATCH_OSC_MULTIPLE_LOWER_BOUND + ((current_byte & 0xF0) >> 4);
      p->osc_divisor[1] = PATCH_OSC_DIVISOR_LOWER_BOUND + (current_byte & 0x0F);
    }

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, OSC_3_MULTIPLE_DIVISOR)];

    if (p->osc_freq_mode[2] == PATCH_OSC_FREQ_MODE_FIXED)
    {
      p->osc_octave[2] = PATCH_OSC_OCTAVE_LOWER_BOUND + ((current_byte & 0xF0) >> 4);
      p->osc_note[2] = PATCH_OSC_NOTE_LOWER_BOUND + (current_byte & 0x0F);
    }
    else
    {
      p->osc_multiple[2] = PATCH_OSC_MULTIPLE_LOWER_BOUND + ((current_byte & 0xF0) >> 4);
      p->osc_divisor[2] = PATCH_OSC_DIVISOR_LOWER_BOUND + (current_byte & 0x0F);
    }

    /* osc sync, phi */
    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, OSC_SYNC_PHI)];

    p->osc_sync = PATCH_SYNC_LOWER_BOUND + ((current_byte & 0x40) >> 6);
    p->osc_phi[0] = PATCH_OSC_PHI_LOWER_BOUND + ((current_byte & 0x30) >> 4);
    p->osc_phi[1] = PATCH_OSC_PHI_LOWER_BOUND + ((current_byte & 0x0C) >> 2);
    p->osc_phi[2] = PATCH_OSC_PHI_LOWER_BOUND + (current_byte & 0x03);

    /* algorithm, filters, osc / env routing */
    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ALGORITHM_OSC_1_ENV_1_ROUTING)];

    p->algorithm = PATCH_ALGORITHM_LOWER_BOUND + ((current_byte & 0xC0) >> 6);
    p->osc_routing[0] = PATCH_OSC_ROUTING_MASK & ((current_byte & 0x38) >> 3);
    p->env_routing[0] = PATCH_ENV_ROUTING_MASK & (current_byte & 0x07);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, HIGHPASS_CUTOFF_OSC_2_ENV_2_ROUTING)];

    p->highpass_cutoff = PATCH_HIGHPASS_CUTOFF_LOWER_BOUND + ((current_byte & 0xC0) >> 6);
    p->osc_routing[1] = PATCH_OSC_ROUTING_MASK & ((current_byte & 0x38) >> 3);
    p->env_routing[1] = PATCH_ENV_ROUTING_MASK & (current_byte & 0x07);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, LOWPASS_CUTOFF_OSC_3_ENV_3_ROUTING)];

    p->lowpass_cutoff = PATCH_LOWPASS_CUTOFF_LOWER_BOUND + ((current_byte & 0xC0) >> 6);
    p->osc_routing[2] = PATCH_OSC_ROUTING_MASK & ((current_byte & 0x38) >> 3);
    p->env_routing[2] = PATCH_ENV_ROUTING_MASK & (current_byte & 0x07);

    /* envelope 1 */
    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_1_ATTACK)];

    p->env_attack[0] = PATCH_ENV_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_1_DECAY)];

    p->env_decay[0] = PATCH_ENV_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_1_SUSTAIN)];

    p->env_sustain[0] = PATCH_ENV_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_1_RELEASE)];

    p->env_release[0] = PATCH_ENV_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_1_LEVEL)];

    p->env_amplitude[0] = PATCH_ENV_LEVEL_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_1_TRANSITION)];

    p->env_transition[0] = PATCH_ENV_LEVEL_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_1_HOLD_MODE_KEYSCALING)];

    p->env_hold_mode[0] = PATCH_ENV_HOLD_MODE_LOWER_BOUND + ((current_byte & 0xC0) >> 6);
    p->env_rate_ks[0] = PATCH_ENV_KEYSCALING_LOWER_BOUND + ((current_byte & 0x38) >> 3);
    p->env_level_ks[0] = PATCH_ENV_KEYSCALING_LOWER_BOUND + (current_byte & 0x07);

    /* envelope 2 */
    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_2_ATTACK)];

    p->env_attack[1] = PATCH_ENV_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_2_DECAY)];

    p->env_decay[1] = PATCH_ENV_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_2_SUSTAIN)];

    p->env_sustain[1] = PATCH_ENV_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_2_RELEASE)];

    p->env_release[1] = PATCH_ENV_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_2_LEVEL)];

    p->env_amplitude[1] = PATCH_ENV_LEVEL_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_2_TRANSITION)];

    p->env_transition[1] = PATCH_ENV_LEVEL_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_2_HOLD_MODE_KEYSCALING)];

    p->env_hold_mode[1] = PATCH_ENV_HOLD_MODE_LOWER_BOUND + ((current_byte & 0xC0) >> 6);
    p->env_rate_ks[1] = PATCH_ENV_KEYSCALING_LOWER_BOUND + ((current_byte & 0x38) >> 3);
    p->env_level_ks[1] = PATCH_ENV_KEYSCALING_LOWER_BOUND + (current_byte & 0x07);

    /* envelope 3 */
    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_3_ATTACK)];

    p->env_attack[2] = PATCH_ENV_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_3_DECAY)];

    p->env_decay[2] = PATCH_ENV_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_3_SUSTAIN)];

    p->env_sustain[2] = PATCH_ENV_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_3_RELEASE)];

    p->env_release[2] = PATCH_ENV_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_3_LEVEL)];

    p->env_amplitude[2] = PATCH_ENV_LEVEL_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_3_TRANSITION)];

    p->env_transition[2] = PATCH_ENV_LEVEL_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_3_HOLD_MODE_KEYSCALING)];

    p->env_hold_mode[2] = PATCH_ENV_HOLD_MODE_LOWER_BOUND + ((current_byte & 0xC0) >> 6);
    p->env_rate_ks[2] = PATCH_ENV_KEYSCALING_LOWER_BOUND + ((current_byte & 0x38) >> 3);
    p->env_level_ks[2] = PATCH_ENV_KEYSCALING_LOWER_BOUND + (current_byte & 0x07);

    /* vibrato lfo */
    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, VIBRATO_POLARITY_WAVEFORM_SPEED)];

    p->lfo_polarity[0] = PATCH_LFO_POLARITY_LOWER_BOUND + ((current_byte & 0x80) >> 7);
    p->lfo_waveform[0] = PATCH_LFO_WAVEFORM_LOWER_BOUND + ((current_byte & 0x30) >> 4);
    p->lfo_speed[0] = PATCH_LFO_SPEED_LOWER_BOUND + (current_byte & 0x0F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, VIBRATO_SYNC_DELAY)];

    p->lfo_sync[0] = PATCH_SYNC_LOWER_BOUND + ((current_byte & 0x80) >> 7);
    p->lfo_delay[0] = PATCH_LFO_DELAY_LOWER_BOUND + (current_byte & 0x3F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, VIBRATO_DEPTH)];

    p->lfo_depth[0] = PATCH_LFO_DEPTH_LOWER_BOUND + (current_byte & 0x7F);

    /* tremolo lfo */
    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, TREMOLO_WAVEFORM_SPEED)];

    p->lfo_waveform[1] = PATCH_LFO_WAVEFORM_LOWER_BOUND + ((current_byte & 0x30) >> 4);
    p->lfo_speed[1] = PATCH_LFO_SPEED_LOWER_BOUND + (current_byte & 0x0F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, TREMOLO_SYNC_DELAY)];

    p->lfo_sync[1] = PATCH_SYNC_LOWER_BOUND + ((current_byte & 0x80) >> 7);
    p->lfo_delay[1] = PATCH_LFO_DELAY_LOWER_BOUND + (current_byte & 0x3F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, TREMOLO_DEPTH)];

    p->lfo_depth[1] = PATCH_LFO_DEPTH_LOWER_BOUND + (current_byte & 0x7F);

    /* chorus */
    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, CHORUS_WAVEFORM_SPEED)];

    p->lfo_waveform[2] = PATCH_LFO_WAVEFORM_LOWER_BOUND + ((current_byte & 0x30) >> 4);
    p->lfo_speed[2] = PATCH_LFO_SPEED_LOWER_BOUND + (current_byte & 0x0F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, CHORUS_SYNC_DELAY)];

    p->lfo_sync[2] = PATCH_SYNC_LOWER_BOUND + ((current_byte & 0x80) >> 7);
    p->lfo_delay[2] = PATCH_LFO_DELAY_LOWER_BOUND + (current_byte & 0x3F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, CHORUS_DEPTH)];

    p->lfo_depth[2] = PATCH_LFO_DEPTH_LOWER_BOUND + (current_byte & 0x7F);

    /* sensitivities, midi controller routing */
    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, BOOST_VELOCITY_SENS)];

    p->boost_sensitivity = PATCH_SENSITIVITY_LOWER_BOUND + ((current_byte & 0xF0) >> 4);
    p->velocity_sensitivity = PATCH_SENSITIVITY_LOWER_BOUND + (current_byte & 0x0F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, VIBRATO_TREMOLO_SENS)];

    p->lfo_sensitivity[0] = PATCH_SENSITIVITY_LOWER_BOUND + ((current_byte & 0xF0) >> 4);
    p->lfo_sensitivity[1] = PATCH_SENSITIVITY_LOWER_BOUND + (current_byte & 0x0F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, CHORUS_SENS_MOD_WHEEL_ROUTING)];

    p->lfo_sensitivity[2] = PATCH_SENSITIVITY_LOWER_BOUND + ((current_byte & 0xF0) >> 4);
    p->mod_wheel_routing = PATCH_MIDI_CONT_ROUTING_MASK & (current_byte & 0x0F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, AFTERTOUCH_EXP_PEDAL_ROUTING)];

    p->aftertouch_routing = PATCH_MIDI_CONT_ROUTING_MASK & ((current_byte & 0xF0) >> 4);
    p->exp_pedal_routing = PATCH_MIDI_CONT_ROUTING_MASK & (current_byte & 0x0F);

    /* pitch envelope */
    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, PITCH_ENV_ATTACK)];

    p->peg_attack = PATCH_PEG_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, PITCH_ENV_DECAY)];

    p->peg_decay = PATCH_PEG_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, PITCH_ENV_RELEASE)];

    p->peg_release = PATCH_PEG_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, PITCH_ENV_MAXIMUM)];

    p->peg_maximum = PATCH_PEG_LEVEL_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, PITCH_ENV_FINALE)];

    p->peg_finale = PATCH_PEG_LEVEL_LOWER_BOUND + (current_byte & 0x7F);

    /* pitch wheel, arpeggio, portamento */
    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, PITCH_WHEEL)];

    p->pitch_wheel_mode = PATCH_PITCH_WHEEL_MODE_LOWER_BOUND + ((current_byte & 0x40) >> 6);
    p->pitch_wheel_range = PATCH_PITCH_WHEEL_RANGE_LOWER_BOUND + (current_byte & 0x1F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ARPEGGIO)];

    p->arpeggio_pattern = PATCH_ARPEGGIO_PATTERN_LOWER_BOUND + ((current_byte & 0xC0) >> 6);
    p->arpeggio_octaves = PATCH_ARPEGGIO_OCTAVES_LOWER_BOUND + ((current_byte & 0x30) >> 4);
    p->arpeggio_speed = PATCH_ARPEGGIO_SPEED_LOWER_BOUND + (current_byte & 0x0F);

    current_byte = cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, PORTAMENTO)];

    p->arpeggio_mode = PATCH_ARPEGGIO_MODE_LOWER_BOUND + ((current_byte & 0x80) >> 7);
    p->portamento_mode = PATCH_PORTAMENTO_MODE_LOWER_BOUND + ((current_byte & 0x40) >> 6);
    p->portamento_legato = PATCH_PORTAMENTO_LEGATO_LOWER_BOUND + ((current_byte & 0x30) >> 4);
    p->portamento_speed = PATCH_PORTAMENTO_SPEED_LOWER_BOUND + (current_byte & 0x0F);

    /* validate the parameters */
    patch_validate_patch(patch_index);
  }

  return 0;
}

/*******************************************************************************
** fileio_cart_save()
*******************************************************************************/
short int fileio_cart_save(int cart_num, char* filename)
{
  int k;

  FILE* fp;
  char  signature[4];
  char  type[4];

  unsigned char cart_data[FILEIO_CART_NUM_BYTES];

  unsigned char current_byte;

  patch* p;

  int patch_index;

  /* make sure filename is valid */
  if (filename == NULL)
    return 0;

  /* make sure cart number is valid */
  if (PATCH_CART_NO_IS_NOT_VALID(cart_num))
    return 0;

  /* initialize cart data */
  for (k = 0; k < FILEIO_CART_NUM_BYTES; k++)
    cart_data[k] = 0;

  /* generate cart data */
  for (k = 0; k < BANK_PATCHES_PER_CART; k++)
  {
    PATCH_COMPUTE_PATCH_INDEX(cart_num, (PATCH_PATCH_NO_LOWER_BOUND + k))

    p = &G_patch_bank[patch_index];

    /* osc frequency mode, detune, waveform */
    current_byte = ((p->osc_freq_mode[0] - PATCH_OSC_FREQ_MODE_LOWER_BOUND) & 0x01) << 7;
    current_byte |= ((p->osc_detune[0] - PATCH_OSC_DETUNE_LOWER_BOUND) & 0x07) << 4;
    current_byte |= (p->osc_waveform[0] - PATCH_OSC_WAVEFORM_LOWER_BOUND) & 0x0F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, OSC_1_FREQ_MODE_DETUNE_WAVEFORM)] = current_byte;

    current_byte = ((p->osc_freq_mode[1] - PATCH_OSC_FREQ_MODE_LOWER_BOUND) & 0x01) << 7;
    current_byte |= ((p->osc_detune[1] - PATCH_OSC_DETUNE_LOWER_BOUND) & 0x07) << 4;
    current_byte |= (p->osc_waveform[1] - PATCH_OSC_WAVEFORM_LOWER_BOUND) & 0x0F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, OSC_2_FREQ_MODE_DETUNE_WAVEFORM)] = current_byte;

    current_byte = ((p->osc_freq_mode[2] - PATCH_OSC_FREQ_MODE_LOWER_BOUND) & 0x01) << 7;
    current_byte |= ((p->osc_detune[2] - PATCH_OSC_DETUNE_LOWER_BOUND) & 0x07) << 4;
    current_byte |= (p->osc_waveform[2] - PATCH_OSC_WAVEFORM_LOWER_BOUND) & 0x0F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, OSC_3_FREQ_MODE_DETUNE_WAVEFORM)] = current_byte;

    /* osc multiple, divisor */
    if (p->osc_freq_mode[0] == PATCH_OSC_FREQ_MODE_FIXED)
    {
      current_byte = ((p->osc_octave[0] - PATCH_OSC_OCTAVE_LOWER_BOUND) & 0x0F) << 4;
      current_byte |= (p->osc_note[0] - PATCH_OSC_NOTE_LOWER_BOUND) & 0x0F;
    }
    else
    {
      current_byte = ((p->osc_multiple[0] - PATCH_OSC_MULTIPLE_LOWER_BOUND) & 0x0F) << 4;
      current_byte |= (p->osc_divisor[0] - PATCH_OSC_DIVISOR_LOWER_BOUND) & 0x0F;
    }

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, OSC_1_MULTIPLE_DIVISOR)] = current_byte;

    if (p->osc_freq_mode[1] == PATCH_OSC_FREQ_MODE_FIXED)
    {
      current_byte = ((p->osc_octave[1] - PATCH_OSC_OCTAVE_LOWER_BOUND) & 0x0F) << 4;
      current_byte |= (p->osc_note[1] - PATCH_OSC_NOTE_LOWER_BOUND) & 0x0F;
    }
    else
    {
      current_byte = ((p->osc_multiple[1] - PATCH_OSC_MULTIPLE_LOWER_BOUND) & 0x0F) << 4;
      current_byte |= (p->osc_divisor[1] - PATCH_OSC_DIVISOR_LOWER_BOUND) & 0x0F;
    }

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, OSC_2_MULTIPLE_DIVISOR)] = current_byte;

    if (p->osc_freq_mode[2] == PATCH_OSC_FREQ_MODE_FIXED)
    {
      current_byte = ((p->osc_octave[2] - PATCH_OSC_OCTAVE_LOWER_BOUND) & 0x0F) << 4;
      current_byte |= (p->osc_note[2] - PATCH_OSC_NOTE_LOWER_BOUND) & 0x0F;
    }
    else
    {
      current_byte = ((p->osc_multiple[2] - PATCH_OSC_MULTIPLE_LOWER_BOUND) & 0x0F) << 4;
      current_byte |= (p->osc_divisor[2] - PATCH_OSC_DIVISOR_LOWER_BOUND) & 0x0F;
    }

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, OSC_3_MULTIPLE_DIVISOR)] = current_byte;

    /* osc sync, phi */
    current_byte = ((p->osc_sync - PATCH_SYNC_LOWER_BOUND) & 0x01) << 6;
    current_byte |= ((p->osc_phi[0] - PATCH_OSC_PHI_LOWER_BOUND) & 0x03) << 4;
    current_byte |= ((p->osc_phi[1] - PATCH_OSC_PHI_LOWER_BOUND) & 0x03) << 2;
    current_byte |= (p->osc_phi[2] - PATCH_OSC_PHI_LOWER_BOUND) & 0x03;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, OSC_SYNC_PHI)] = current_byte;

    /* algorithm, filters, osc / env routing */
    current_byte = ((p->algorithm - PATCH_ALGORITHM_LOWER_BOUND) & 0x03) << 6;
    current_byte |= ((p->osc_routing[0] & PATCH_OSC_ROUTING_MASK) & 0x07) << 3;
    current_byte |= (p->env_routing[0] & PATCH_ENV_ROUTING_MASK) & 0x07;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ALGORITHM_OSC_1_ENV_1_ROUTING)] = current_byte;

    current_byte = ((p->highpass_cutoff - PATCH_HIGHPASS_CUTOFF_LOWER_BOUND) & 0x03) << 6;
    current_byte |= ((p->osc_routing[1] & PATCH_OSC_ROUTING_MASK) & 0x07) << 3;
    current_byte |= (p->env_routing[1] & PATCH_ENV_ROUTING_MASK) & 0x07;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, HIGHPASS_CUTOFF_OSC_2_ENV_2_ROUTING)] = current_byte;

    current_byte = ((p->lowpass_cutoff - PATCH_LOWPASS_CUTOFF_LOWER_BOUND) & 0x03) << 6;
    current_byte |= ((p->osc_routing[2] & PATCH_OSC_ROUTING_MASK) & 0x07) << 3;
    current_byte |= (p->env_routing[2] & PATCH_ENV_ROUTING_MASK) & 0x07;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, LOWPASS_CUTOFF_OSC_3_ENV_3_ROUTING)] = current_byte;

    /* envelope 1 */
    current_byte = (p->env_attack[0] - PATCH_ENV_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_1_ATTACK)] = current_byte;

    current_byte = (p->env_decay[0] - PATCH_ENV_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_1_DECAY)] = current_byte;

    current_byte = (p->env_sustain[0] - PATCH_ENV_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_1_SUSTAIN)] = current_byte;

    current_byte = (p->env_release[0] - PATCH_ENV_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_1_RELEASE)] = current_byte;

    current_byte = (p->env_amplitude[0] - PATCH_ENV_LEVEL_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_1_LEVEL)] = current_byte;

    current_byte = (p->env_transition[0] - PATCH_ENV_LEVEL_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_1_TRANSITION)] = current_byte;

    current_byte = ((p->env_hold_mode[0] - PATCH_ENV_HOLD_MODE_LOWER_BOUND) & 0x03) << 6;
    current_byte |= ((p->env_rate_ks[0] - PATCH_ENV_KEYSCALING_LOWER_BOUND) & 0x07) << 3;
    current_byte |= (p->env_level_ks[0] - PATCH_ENV_KEYSCALING_LOWER_BOUND) & 0x07;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_1_HOLD_MODE_KEYSCALING)] = current_byte;

    /* envelope 2 */
    current_byte = (p->env_attack[1] - PATCH_ENV_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_2_ATTACK)] = current_byte;

    current_byte = (p->env_decay[1] - PATCH_ENV_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_2_DECAY)] = current_byte;

    current_byte = (p->env_sustain[1] - PATCH_ENV_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_2_SUSTAIN)] = current_byte;

    current_byte = (p->env_release[1] - PATCH_ENV_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_2_RELEASE)] = current_byte;

    current_byte = (p->env_amplitude[1] - PATCH_ENV_LEVEL_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_2_LEVEL)] = current_byte;

    current_byte = (p->env_transition[1] - PATCH_ENV_LEVEL_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_2_TRANSITION)] = current_byte;

    current_byte = ((p->env_hold_mode[1] - PATCH_ENV_HOLD_MODE_LOWER_BOUND) & 0x03) << 6;
    current_byte |= ((p->env_rate_ks[1] - PATCH_ENV_KEYSCALING_LOWER_BOUND) & 0x07) << 3;
    current_byte |= (p->env_level_ks[1] - PATCH_ENV_KEYSCALING_LOWER_BOUND) & 0x07;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_2_HOLD_MODE_KEYSCALING)] = current_byte;

    /* envelope 3 */
    current_byte = (p->env_attack[2] - PATCH_ENV_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_3_ATTACK)] = current_byte;

    current_byte = (p->env_decay[2] - PATCH_ENV_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_3_DECAY)] = current_byte;

    current_byte = (p->env_sustain[2] - PATCH_ENV_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_3_SUSTAIN)] = current_byte;

    current_byte = (p->env_release[2] - PATCH_ENV_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_3_RELEASE)] = current_byte;

    current_byte = (p->env_amplitude[2] - PATCH_ENV_LEVEL_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_3_LEVEL)] = current_byte;

    current_byte = (p->env_transition[2] - PATCH_ENV_LEVEL_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_3_TRANSITION)] = current_byte;

    current_byte = ((p->env_hold_mode[2] - PATCH_ENV_HOLD_MODE_LOWER_BOUND) & 0x03) << 6;
    current_byte |= ((p->env_rate_ks[2] - PATCH_ENV_KEYSCALING_LOWER_BOUND) & 0x07) << 3;
    current_byte |= (p->env_level_ks[2] - PATCH_ENV_KEYSCALING_LOWER_BOUND) & 0x07;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ENV_3_HOLD_MODE_KEYSCALING)] = current_byte;

    /* vibrato lfo */
    current_byte = ((p->lfo_polarity[0] - PATCH_LFO_POLARITY_LOWER_BOUND) & 0x01) << 7;
    current_byte |= ((p->lfo_waveform[0] - PATCH_LFO_WAVEFORM_LOWER_BOUND) & 0x03) << 4;
    current_byte |= (p->lfo_speed[0] - PATCH_LFO_SPEED_LOWER_BOUND) & 0x0F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, VIBRATO_POLARITY_WAVEFORM_SPEED)] = current_byte;

    current_byte = ((p->lfo_sync[0] - PATCH_SYNC_LOWER_BOUND) & 0x01) << 7;
    current_byte |= (p->lfo_delay[0] - PATCH_LFO_DELAY_LOWER_BOUND) & 0x3F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, VIBRATO_SYNC_DELAY)] = current_byte;

    current_byte = (p->lfo_depth[0] - PATCH_LFO_DEPTH_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, VIBRATO_DEPTH)] = current_byte;

    /* tremolo lfo */
    current_byte = ((p->lfo_waveform[1] - PATCH_LFO_WAVEFORM_LOWER_BOUND) & 0x03) << 4;
    current_byte |= (p->lfo_speed[1] - PATCH_LFO_SPEED_LOWER_BOUND) & 0x0F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, TREMOLO_WAVEFORM_SPEED)] = current_byte;

    current_byte = ((p->lfo_sync[1] - PATCH_SYNC_LOWER_BOUND) & 0x01) << 7;
    current_byte |= (p->lfo_delay[1] - PATCH_LFO_DELAY_LOWER_BOUND) & 0x3F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, TREMOLO_SYNC_DELAY)] = current_byte;

    current_byte = (p->lfo_depth[1] - PATCH_LFO_DEPTH_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, TREMOLO_DEPTH)] = current_byte;

    /* chorus */
    current_byte = ((p->lfo_waveform[2] - PATCH_LFO_WAVEFORM_LOWER_BOUND) & 0x03) << 4;
    current_byte |= (p->lfo_speed[2] - PATCH_LFO_SPEED_LOWER_BOUND) & 0x0F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, CHORUS_WAVEFORM_SPEED)] = current_byte;

    current_byte = ((p->lfo_sync[2] - PATCH_SYNC_LOWER_BOUND) & 0x01) << 7;
    current_byte |= (p->lfo_delay[2] - PATCH_LFO_DELAY_LOWER_BOUND) & 0x3F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, CHORUS_SYNC_DELAY)] = current_byte;

    current_byte = (p->lfo_depth[2] - PATCH_LFO_DEPTH_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, CHORUS_DEPTH)] = current_byte;

    /* sensitivities */
    current_byte = ((p->boost_sensitivity - PATCH_SENSITIVITY_LOWER_BOUND) & 0x0F) << 4;
    current_byte |= (p->velocity_sensitivity - PATCH_SENSITIVITY_LOWER_BOUND) & 0x0F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, BOOST_VELOCITY_SENS)] = current_byte;

    current_byte = ((p->lfo_sensitivity[0] - PATCH_SENSITIVITY_LOWER_BOUND) & 0x0F) << 4;
    current_byte |= (p->lfo_sensitivity[1] - PATCH_SENSITIVITY_LOWER_BOUND) & 0x0F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, VIBRATO_TREMOLO_SENS)] = current_byte;

    current_byte = ((p->lfo_sensitivity[2] - PATCH_SENSITIVITY_LOWER_BOUND) & 0x0F) << 4;
    current_byte |= (p->mod_wheel_routing & PATCH_MIDI_CONT_ROUTING_MASK) & 0x0F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, CHORUS_SENS_MOD_WHEEL_ROUTING)] = current_byte;

    current_byte = ((p->aftertouch_routing & PATCH_MIDI_CONT_ROUTING_MASK) & 0x0F) << 4;
    current_byte |= (p->exp_pedal_routing & PATCH_MIDI_CONT_ROUTING_MASK) & 0x0F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, AFTERTOUCH_EXP_PEDAL_ROUTING)] = current_byte;

    /* pitch envelope */
    current_byte = (p->peg_attack - PATCH_PEG_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, PITCH_ENV_ATTACK)] = current_byte;

    current_byte = (p->peg_decay - PATCH_PEG_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, PITCH_ENV_DECAY)] = current_byte;

    current_byte = (p->peg_release - PATCH_PEG_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, PITCH_ENV_RELEASE)] = current_byte;

    current_byte = (p->peg_maximum - PATCH_PEG_LEVEL_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, PITCH_ENV_MAXIMUM)] = current_byte;

    current_byte = (p->peg_finale - PATCH_PEG_LEVEL_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, PITCH_ENV_FINALE)] = current_byte;

    /* pitch wheel, arpeggio, portamento */
    current_byte = ((p->pitch_wheel_mode - PATCH_PITCH_WHEEL_MODE_LOWER_BOUND) & 0x01) << 6;
    current_byte |= (p->pitch_wheel_range - PATCH_PITCH_WHEEL_RANGE_LOWER_BOUND) & 0x1F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, PITCH_WHEEL)] = current_byte;

    current_byte = ((p->arpeggio_pattern - PATCH_ARPEGGIO_PATTERN_LOWER_BOUND) & 0x03) << 6;
    current_byte |= ((p->arpeggio_octaves - PATCH_ARPEGGIO_OCTAVES_LOWER_BOUND) & 0x03) << 4;
    current_byte |= (p->arpeggio_speed - PATCH_ARPEGGIO_SPEED_LOWER_BOUND) & 0x0F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, ARPEGGIO)] = current_byte;

    current_byte = ((p->arpeggio_mode - PATCH_ARPEGGIO_MODE_LOWER_BOUND) & 0x01) << 7;
    current_byte |= ((p->portamento_mode - PATCH_PORTAMENTO_MODE_LOWER_BOUND) & 0x01) << 6;
    current_byte |= ((p->portamento_legato - PATCH_PORTAMENTO_LEGATO_LOWER_BOUND) & 0x03) << 4;
    current_byte |= (p->portamento_speed - PATCH_PORTAMENTO_SPEED_LOWER_BOUND) & 0x0F;

    cart_data[FILEIO_PATCH_COMPUTE_BYTE_INDEX(k, PORTAMENTO)] = current_byte;
  }

  /* open cart file */
  fp = fopen(filename, "wb");

  /* if file did not open, return */
  if (fp == NULL)
    return 0;

  /* write signature */
  signature[0] = 'G';
  signature[1] = 'E';
  signature[2] = 'R';
  signature[3] = 'S';

  if (fwrite(signature, 1, 4, fp) < 4)
  {
    fclose(fp);
    return 1;
  }

  /* write type */
  type[0] = 'C';
  type[1] = 'A';
  type[2] = 'R';
  type[3] = 'T';

  if (fwrite(type, 1, 4, fp) < 4)
  {
    fclose(fp);
    return 1;
  }

  /* write cart data */
  if (fwrite(cart_data, 1, FILEIO_CART_NUM_BYTES, fp) == 0)
  {
    fclose(fp);
    return 1;
  }

  /* close cart file */
  fclose(fp);

  return 0;
}

