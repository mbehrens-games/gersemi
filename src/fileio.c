/*******************************************************************************
** fileio.c (loading and saving from native file formats)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "bank.h"
#include "fileio.h"
#include "patch.h"

#define FILEIO_PATCH_COMPUTE_GENERAL_INDEX(patch_num, name)                    \
  ( (patch_num * FILEIO_PATCH_NUM_BYTES) + FILEIO_PATCH_GENERAL_START_INDEX +  \
    FILEIO_PATCH_BYTE_GENERAL_##name)

#define FILEIO_PATCH_COMPUTE_OSC_ENV_INDEX(patch_num, osc_env_num, name)       \
  ( (patch_num * FILEIO_PATCH_NUM_BYTES) + FILEIO_PATCH_OSC_ENV_START_INDEX +  \
    (osc_env_num * FILEIO_PATCH_NUM_OSC_ENV_BYTES) + FILEIO_PATCH_BYTE_OSC_ENV_##name)

/*******************************************************************************
** fileio_cart_load()
*******************************************************************************/
short int fileio_cart_load(int cart_num, char* filename)
{
  int k;
  int m;

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

    /* algorithm, velocity */
    current_byte = cart_data[FILEIO_PATCH_COMPUTE_GENERAL_INDEX(k, ALGORITHM_VELOCITY)];

    p->algorithm = PATCH_ALGORITHM_LOWER_BOUND + ((current_byte & 0xE0) >> 5);
    p->velocity_mode = PATCH_VELOCITY_MODE_LOWER_BOUND + ((current_byte & 0x10) >> 4);
    p->velocity_scaling = PATCH_VELOCITY_SCALING_LOWER_BOUND + (current_byte & 0x0F);

    /* noise */
    current_byte = cart_data[FILEIO_PATCH_COMPUTE_GENERAL_INDEX(k, NOISE)];

    p->noise_mode = PATCH_NOISE_MODE_LOWER_BOUND + ((current_byte & 0xC0) >> 6);
    p->noise_frequency = PATCH_NOISE_FREQUENCY_LOWER_BOUND + (current_byte & 0x1F);

    /* portamento */
    current_byte = cart_data[FILEIO_PATCH_COMPUTE_GENERAL_INDEX(k, PORTAMENTO)];

    p->portamento_mode = PATCH_PORTAMENTO_MODE_LOWER_BOUND + ((current_byte & 0x40) >> 6);
    p->portamento_legato = PATCH_PORTAMENTO_LEGATO_LOWER_BOUND + ((current_byte & 0x10) >> 4);
    p->portamento_speed = PATCH_PORTAMENTO_SPEED_LOWER_BOUND + (current_byte & 0x0F);

    /* filter cutoffs, sustain pedal */
    current_byte = cart_data[FILEIO_PATCH_COMPUTE_GENERAL_INDEX(k, FILTER_CUTOFFS_SUSTAIN_PEDAL)];

    p->highpass_cutoff = PATCH_HIGHPASS_CUTOFF_LOWER_BOUND + ((current_byte & 0xC0) >> 6);
    p->lowpass_cutoff = PATCH_LOWPASS_CUTOFF_LOWER_BOUND + ((current_byte & 0x30) >> 4);
    p->pedal_adjust = PATCH_PEDAL_ADJUST_LOWER_BOUND + (current_byte & 0x0F);

    /* sync, pitch wheel */
    current_byte = cart_data[FILEIO_PATCH_COMPUTE_GENERAL_INDEX(k, SYNC_PITCH_WHEEL)];

    p->sync_osc = PATCH_SYNC_LOWER_BOUND + ((current_byte & 0x80) >> 7);
    p->sync_lfo = PATCH_SYNC_LOWER_BOUND + ((current_byte & 0x40) >> 6);
    p->pitch_wheel_mode = PATCH_PITCH_WHEEL_MODE_LOWER_BOUND + ((current_byte & 0x10) >> 4);
    p->pitch_wheel_range = PATCH_PITCH_WHEEL_RANGE_LOWER_BOUND + (current_byte & 0x0F);

    /* vibrato & tremolo depths */
    current_byte = cart_data[FILEIO_PATCH_COMPUTE_GENERAL_INDEX(k, VIBRATO_TREMOLO_DEPTHS)];

    p->vibrato_depth = PATCH_EFFECT_DEPTH_LOWER_BOUND + ((current_byte & 0xF0) >> 4);
    p->tremolo_depth = PATCH_EFFECT_DEPTH_LOWER_BOUND + (current_byte & 0x0F);

    /* effect modes, boost depth */
    current_byte = cart_data[FILEIO_PATCH_COMPUTE_GENERAL_INDEX(k, EFFECT_MODES_BOOST_DEPTH)];

    p->vibrato_mode = PATCH_VIBRATO_MODE_LOWER_BOUND + ((current_byte & 0x40) >> 6);
    p->tremolo_mode = PATCH_TREMOLO_MODE_LOWER_BOUND + ((current_byte & 0x20) >> 5);
    p->boost_mode = PATCH_BOOST_MODE_LOWER_BOUND + ((current_byte & 0x10) >> 4);
    p->boost_depth = PATCH_EFFECT_DEPTH_LOWER_BOUND + (current_byte & 0x0F);

    /* mod wheel effect, vibrato base */
    current_byte = cart_data[FILEIO_PATCH_COMPUTE_GENERAL_INDEX(k, MOD_WHEEL_EFFECT_VIBRATO_BASE)];

    p->mod_wheel_effect = PATCH_CONTROLLER_EFFECT_LOWER_BOUND + ((current_byte & 0xC0) >> 6);
    p->vibrato_base = PATCH_EFFECT_BASE_LOWER_BOUND + (current_byte & 0x1F);

    /* aftertouch effect, tremolo base */
    current_byte = cart_data[FILEIO_PATCH_COMPUTE_GENERAL_INDEX(k, AFTERTOUCH_EFFECT_TREMOLO_BASE)];

    p->aftertouch_effect = PATCH_CONTROLLER_EFFECT_LOWER_BOUND + ((current_byte & 0xC0) >> 6);
    p->tremolo_base = PATCH_EFFECT_BASE_LOWER_BOUND + (current_byte & 0x1F);

    /* lfo waveform, delay */
    current_byte = cart_data[FILEIO_PATCH_COMPUTE_GENERAL_INDEX(k, LFO_WAVEFORM_DELAY)];

    p->lfo_waveform = PATCH_LFO_WAVEFORM_LOWER_BOUND + ((current_byte & 0xE0) >> 5);
    p->lfo_delay = PATCH_LFO_DELAY_LOWER_BOUND + (current_byte & 0x1F);

    /* lfo frequency */
    current_byte = cart_data[FILEIO_PATCH_COMPUTE_GENERAL_INDEX(k, LFO_FREQUENCY)];

    p->lfo_frequency = PATCH_LFO_FREQUENCY_LOWER_BOUND + (current_byte & 0x1F);

    /* lfo quantize */
    current_byte = cart_data[FILEIO_PATCH_COMPUTE_GENERAL_INDEX(k, LFO_QUANTIZE)];

    p->lfo_quantize = PATCH_LFO_QUANTIZE_LOWER_BOUND + (current_byte & 0x1F);

    /* oscillators, envelopes */
    for (m = 0; m < 4; m++)
    {
      /* waveform, feedback */
      current_byte = cart_data[FILEIO_PATCH_COMPUTE_OSC_ENV_INDEX(k, m, WAVEFORM_FEEDBACK)];

      p->osc_waveform[m] = PATCH_OSC_WAVEFORM_LOWER_BOUND + ((current_byte & 0xF0) >> 4);
      p->osc_feedback[m] = PATCH_OSC_FEEDBACK_LOWER_BOUND + (current_byte & 0x0F);

      /* multiple, divisor or note, octave */
      current_byte = cart_data[FILEIO_PATCH_COMPUTE_OSC_ENV_INDEX(k, m, MULTIPLE_DIVISOR_OR_NOTE_OCTAVE)];

      if (p->osc_freq_mode[m] == PATCH_OSC_FREQ_MODE_LOWER_BOUND)
      {
        p->osc_multiple[m] = PATCH_OSC_MULTIPLE_LOWER_BOUND + ((current_byte & 0xF0) >> 4);
        p->osc_divisor[m] = PATCH_OSC_DIVISOR_LOWER_BOUND + (current_byte & 0x0F);
      }
      else
      {
        p->osc_note[m] = PATCH_OSC_NOTE_LOWER_BOUND + ((current_byte & 0xF0) >> 4);
        p->osc_octave[m] = PATCH_OSC_OCTAVE_LOWER_BOUND + (current_byte & 0x0F);
      }

      /* detune */
      current_byte = cart_data[FILEIO_PATCH_COMPUTE_OSC_ENV_INDEX(k, m, DETUNE)];

      p->osc_detune[m] = PATCH_OSC_DETUNE_LOWER_BOUND + (current_byte & 0x3F);

      /* phi, frequency mode, break point */
      current_byte = cart_data[FILEIO_PATCH_COMPUTE_OSC_ENV_INDEX(k, m, PHI_FREQ_MODE_BREAK_POINT)];

      p->osc_phi[m] = PATCH_OSC_PHI_LOWER_BOUND + ((current_byte & 0xC0) >> 6);
      p->osc_freq_mode[m] = PATCH_OSC_FREQ_MODE_LOWER_BOUND + ((current_byte & 0x10) >> 4);
      p->env_break_point[m] = PATCH_ENV_BREAK_POINT_LOWER_BOUND + (current_byte & 0x0F);

      /* attack, rate keyscaling */
      current_byte = cart_data[FILEIO_PATCH_COMPUTE_OSC_ENV_INDEX(k, m, ATTACK_RATE_KS)];

      p->env_rate_ks[m] = PATCH_ENV_KEYSCALING_LOWER_BOUND + ((current_byte & 0xE0) >> 5);
      p->env_attack[m] = PATCH_ENV_RATE_LOWER_BOUND + (current_byte & 0x1F);

      /* decay 1, level keyscaling */
      current_byte = cart_data[FILEIO_PATCH_COMPUTE_OSC_ENV_INDEX(k, m, DECAY_1_LEVEL_KS)];

      p->env_level_ks[m] = PATCH_ENV_KEYSCALING_LOWER_BOUND + ((current_byte & 0xE0) >> 5);
      p->env_decay_1[m] = PATCH_ENV_RATE_LOWER_BOUND + (current_byte & 0x1F);

      /* decay 2 */
      current_byte = cart_data[FILEIO_PATCH_COMPUTE_OSC_ENV_INDEX(k, m, DECAY_2)];

      p->env_decay_2[m] = PATCH_ENV_RATE_LOWER_BOUND + (current_byte & 0x1F);

      /* release */
      current_byte = cart_data[FILEIO_PATCH_COMPUTE_OSC_ENV_INDEX(k, m, RELEASE)];

      p->env_release[m] = PATCH_ENV_RATE_LOWER_BOUND + (current_byte & 0x1F);

      /* amplitude */
      current_byte = cart_data[FILEIO_PATCH_COMPUTE_OSC_ENV_INDEX(k, m, AMPLITUDE)];

      p->env_amplitude[m] = PATCH_ENV_AMPLITUDE_LOWER_BOUND + (current_byte & 0x3F);

      /* sustain */
      current_byte = cart_data[FILEIO_PATCH_COMPUTE_OSC_ENV_INDEX(k, m, SUSTAIN)];

      p->env_sustain[m] = PATCH_ENV_SUSTAIN_LOWER_BOUND + (current_byte & 0x1F);
    }

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
  int m;

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
  for (m = 0; m < FILEIO_CART_NUM_BYTES; m++)
    cart_data[m] = 0;

  /* generate cart data */
  for (k = 0; k < BANK_PATCHES_PER_CART; k++)
  {
    PATCH_COMPUTE_PATCH_INDEX(cart_num, (PATCH_PATCH_NO_LOWER_BOUND + k))

    p = &G_patch_bank[patch_index];

    /* algorithm, velocity */
    current_byte = ((p->algorithm - PATCH_ALGORITHM_LOWER_BOUND) & 0x07) << 5;
    current_byte |= ((p->velocity_mode - PATCH_VELOCITY_MODE_LOWER_BOUND) & 0x01) << 4;
    current_byte |= (p->velocity_scaling - PATCH_VELOCITY_SCALING_LOWER_BOUND) & 0x0F;

    cart_data[FILEIO_PATCH_COMPUTE_GENERAL_INDEX(k, ALGORITHM_VELOCITY)] = current_byte;

    /* noise */
    current_byte = ((p->noise_mode - PATCH_NOISE_MODE_LOWER_BOUND) & 0x03) << 6;
    current_byte |= (p->noise_frequency - PATCH_NOISE_FREQUENCY_LOWER_BOUND) & 0x1F;

    cart_data[FILEIO_PATCH_COMPUTE_GENERAL_INDEX(k, NOISE)] = current_byte;

    /* portamento */
    current_byte = ((p->portamento_mode - PATCH_PORTAMENTO_MODE_LOWER_BOUND) & 0x01) << 6;
    current_byte |= ((p->portamento_legato - PATCH_PORTAMENTO_LEGATO_LOWER_BOUND) & 0x01) << 4;
    current_byte |= (p->portamento_speed - PATCH_PORTAMENTO_SPEED_LOWER_BOUND) & 0x0F;

    cart_data[FILEIO_PATCH_COMPUTE_GENERAL_INDEX(k, PORTAMENTO)] = current_byte;

    /* filter cutoffs, sustain pedal */
    current_byte = ((p->highpass_cutoff - PATCH_HIGHPASS_CUTOFF_LOWER_BOUND) & 0x03) << 6;
    current_byte |= ((p->lowpass_cutoff - PATCH_LOWPASS_CUTOFF_LOWER_BOUND) & 0x03) << 4;
    current_byte |= (p->pedal_adjust - PATCH_PEDAL_ADJUST_LOWER_BOUND) & 0x0F;

    cart_data[FILEIO_PATCH_COMPUTE_GENERAL_INDEX(k, FILTER_CUTOFFS_SUSTAIN_PEDAL)] = current_byte;

    /* sync, pitch wheel */
    current_byte = ((p->sync_osc - PATCH_SYNC_LOWER_BOUND) & 0x01) << 7;
    current_byte |= ((p->sync_lfo - PATCH_SYNC_LOWER_BOUND) & 0x01) << 6;
    current_byte |= ((p->pitch_wheel_mode - PATCH_PITCH_WHEEL_MODE_LOWER_BOUND) & 0x01) << 4;
    current_byte |= (p->pitch_wheel_range - PATCH_PITCH_WHEEL_RANGE_LOWER_BOUND) & 0x0F;

    cart_data[FILEIO_PATCH_COMPUTE_GENERAL_INDEX(k, SYNC_PITCH_WHEEL)] = current_byte;

    /* vibrato & tremolo depths */
    current_byte = ((p->vibrato_depth - PATCH_EFFECT_DEPTH_LOWER_BOUND) & 0x0F) << 4;
    current_byte |= (p->tremolo_depth - PATCH_EFFECT_DEPTH_LOWER_BOUND) & 0x0F;

    cart_data[FILEIO_PATCH_COMPUTE_GENERAL_INDEX(k, VIBRATO_TREMOLO_DEPTHS)] = current_byte;

    /* effect modes, boost depth */
    current_byte = ((p->vibrato_mode - PATCH_VIBRATO_MODE_LOWER_BOUND) & 0x01) << 6;
    current_byte |= ((p->tremolo_mode - PATCH_TREMOLO_MODE_LOWER_BOUND) & 0x01) << 5;
    current_byte |= ((p->boost_mode - PATCH_BOOST_MODE_LOWER_BOUND) & 0x01) << 4;
    current_byte |= (p->boost_depth - PATCH_EFFECT_DEPTH_LOWER_BOUND) & 0x0F;

    cart_data[FILEIO_PATCH_COMPUTE_GENERAL_INDEX(k, EFFECT_MODES_BOOST_DEPTH)] = current_byte;

    /* mod wheel effect, vibrato base */
    current_byte = ((p->mod_wheel_effect - PATCH_CONTROLLER_EFFECT_LOWER_BOUND) & 0x03) << 6;
    current_byte |= (p->vibrato_base - PATCH_EFFECT_BASE_LOWER_BOUND) & 0x1F;

    cart_data[FILEIO_PATCH_COMPUTE_GENERAL_INDEX(k, MOD_WHEEL_EFFECT_VIBRATO_BASE)] = current_byte;

    /* aftertouch effect, tremolo base */
    current_byte = ((p->aftertouch_effect - PATCH_CONTROLLER_EFFECT_LOWER_BOUND) & 0x03) << 6;
    current_byte |= (p->tremolo_base - PATCH_EFFECT_BASE_LOWER_BOUND) & 0x1F;

    cart_data[FILEIO_PATCH_COMPUTE_GENERAL_INDEX(k, AFTERTOUCH_EFFECT_TREMOLO_BASE)] = current_byte;

    /* lfo waveform, delay */
    current_byte = ((p->lfo_waveform - PATCH_LFO_WAVEFORM_LOWER_BOUND) & 0x07) << 5;
    current_byte |= (p->lfo_delay - PATCH_LFO_DELAY_LOWER_BOUND) & 0x1F;

    cart_data[FILEIO_PATCH_COMPUTE_GENERAL_INDEX(k, LFO_WAVEFORM_DELAY)] = current_byte;

    /* lfo frequency */
    current_byte = (p->lfo_frequency - PATCH_LFO_FREQUENCY_LOWER_BOUND) & 0x1F;

    cart_data[FILEIO_PATCH_COMPUTE_GENERAL_INDEX(k, LFO_FREQUENCY)] = current_byte;

    /* lfo quantize */
    current_byte = (p->lfo_quantize - PATCH_LFO_QUANTIZE_LOWER_BOUND) & 0x1F;

    cart_data[FILEIO_PATCH_COMPUTE_GENERAL_INDEX(k, LFO_QUANTIZE)] = current_byte;

    /* oscillators, envelopes */
    for (m = 0; m < 4; m++)
    {
      /* waveform, feedback */
      current_byte = ((p->osc_waveform[m] - PATCH_OSC_WAVEFORM_LOWER_BOUND) & 0x0F) << 4;
      current_byte |= (p->osc_feedback[m] - PATCH_OSC_FEEDBACK_LOWER_BOUND) & 0x0F;

      cart_data[FILEIO_PATCH_COMPUTE_OSC_ENV_INDEX(k, m, WAVEFORM_FEEDBACK)] = current_byte;

      /* multiple, divisor or note, octave */
      if (p->osc_freq_mode[m] == PATCH_OSC_FREQ_MODE_LOWER_BOUND)
      {
        current_byte = ((p->osc_multiple[m] - PATCH_OSC_MULTIPLE_LOWER_BOUND) & 0x0F) << 4;
        current_byte |= (p->osc_divisor[m] - PATCH_OSC_DIVISOR_LOWER_BOUND) & 0x0F;
      }
      else
      {
        current_byte = ((p->osc_note[m] - PATCH_OSC_NOTE_LOWER_BOUND) & 0x0F) << 4;
        current_byte |= (p->osc_octave[m] - PATCH_OSC_OCTAVE_LOWER_BOUND) & 0x0F;
      }

      cart_data[FILEIO_PATCH_COMPUTE_OSC_ENV_INDEX(k, m, MULTIPLE_DIVISOR_OR_NOTE_OCTAVE)] = current_byte;

      /* detune */
      current_byte = (p->osc_detune[m] - PATCH_OSC_DETUNE_LOWER_BOUND) & 0x3F;

      cart_data[FILEIO_PATCH_COMPUTE_OSC_ENV_INDEX(k, m, DETUNE)] = current_byte;

      /* phi, frequency mode, break point */
      current_byte = ((p->osc_phi[m] - PATCH_OSC_PHI_LOWER_BOUND) & 0x03) << 6;
      current_byte |= ((p->osc_freq_mode[m] - PATCH_OSC_FREQ_MODE_LOWER_BOUND) & 0x01) << 4;
      current_byte |= (p->env_break_point[m] - PATCH_ENV_BREAK_POINT_LOWER_BOUND) & 0x0F;

      cart_data[FILEIO_PATCH_COMPUTE_OSC_ENV_INDEX(k, m, PHI_FREQ_MODE_BREAK_POINT)] = current_byte;

      /* attack, rate keyscaling  */
      current_byte = ((p->env_rate_ks[m] - PATCH_ENV_KEYSCALING_LOWER_BOUND) & 0x07) << 5;
      current_byte |= (p->env_attack[m] - PATCH_ENV_RATE_LOWER_BOUND) & 0x1F;

      cart_data[FILEIO_PATCH_COMPUTE_OSC_ENV_INDEX(k, m, ATTACK_RATE_KS)] = current_byte;

      /* decay 1, level keyscaling */
      current_byte = ((p->env_level_ks[m] - PATCH_ENV_KEYSCALING_LOWER_BOUND) & 0x07) << 5;
      current_byte |= (p->env_decay_1[m] - PATCH_ENV_RATE_LOWER_BOUND) & 0x1F;

      cart_data[FILEIO_PATCH_COMPUTE_OSC_ENV_INDEX(k, m, DECAY_1_LEVEL_KS)] = current_byte;

      /* decay 2 */
      current_byte = (p->env_decay_2[m] - PATCH_ENV_RATE_LOWER_BOUND) & 0x1F;

      cart_data[FILEIO_PATCH_COMPUTE_OSC_ENV_INDEX(k, m, DECAY_2)] = current_byte;

      /* release */
      current_byte = (p->env_release[m] - PATCH_ENV_RATE_LOWER_BOUND) & 0x1F;

      cart_data[FILEIO_PATCH_COMPUTE_OSC_ENV_INDEX(k, m, RELEASE)] = current_byte;

      /* amplitude */
      current_byte = (p->env_amplitude[m] - PATCH_ENV_AMPLITUDE_LOWER_BOUND) & 0x3F;

      cart_data[FILEIO_PATCH_COMPUTE_OSC_ENV_INDEX(k, m, AMPLITUDE)] = current_byte;

      /* sustain */
      current_byte = (p->env_sustain[m] - PATCH_ENV_SUSTAIN_LOWER_BOUND) & 0x1F;

      cart_data[FILEIO_PATCH_COMPUTE_OSC_ENV_INDEX(k, m, SUSTAIN)] = current_byte;
    }
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

