/*******************************************************************************
** fileio.c (loading and saving from native file formats)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "fileio.h"
#include "patch.h"

#define FILEIO_COMPUTE_PATCH_GENERAL_INDEX(patch_num, name)                    \
  ( (patch_num * FILEIO_PATCH_NUM_BYTES) + FILEIO_PATCH_GENERAL_START_INDEX +  \
    FILEIO_PATCH_BYTE_GENERAL_##name)

#define FILEIO_COMPUTE_PATCH_OSC_ENV_INDEX(patch_num, osc_env_num, name)       \
  ( (patch_num * FILEIO_PATCH_NUM_BYTES) + FILEIO_PATCH_OSC_ENV_START_INDEX +  \
    (osc_env_num * FILEIO_PATCH_NUM_OSC_ENV_BYTES) + FILEIO_PATCH_BYTE_OSC_ENV_##name)

/*******************************************************************************
** fileio_patch_set_load()
*******************************************************************************/
short int fileio_patch_set_load(char* filename, int set_num)
{
  int k;
  int m;

  FILE* fp;
  char  signature[4];
  char  type[4];

  unsigned char patch_set_data[FILEIO_SET_NUM_BYTES];

  unsigned char current_byte;

  patch* p;

  /* make sure filename is valid */
  if (filename == NULL)
    return 0;

  /* make sure patch set number is valid */
  if ((set_num < 0) || (set_num >= FILEIO_NUM_SETS))
    return 0;

  /* open patch set file */
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

  if ((type[0] != 'P') || 
      (type[1] != 'A') || 
      (type[2] != 'T') || 
      (type[3] != 'C'))
  {
    fclose(fp);
    return 1;
  }

  /* read patch set data */
  if (fread(patch_set_data, 1, FILEIO_SET_NUM_BYTES, fp) == 0)
  {
    fclose(fp);
    return 1;
  }

  /* close patch set file */
  fclose(fp);

  /* load patch set data */
  for (k = 0; k < FILEIO_PATCHES_PER_SET; k++)
  {
    p = &G_patch_bank[set_num * FILEIO_PATCHES_PER_SET + k];

    /* reset patch */
    patch_reset(set_num * FILEIO_PATCHES_PER_SET + k);

    /* algorithm, filter cutoffs */
    current_byte = patch_set_data[FILEIO_COMPUTE_PATCH_GENERAL_INDEX(k, ALGORITHM_FILTER_CUTOFFS)];

    p->algorithm = PATCH_ALGORITHM_LOWER_BOUND + ((current_byte & 0x07) >> 4);
    p->highpass_cutoff = PATCH_HIGHPASS_CUTOFF_LOWER_BOUND + ((current_byte & 0x0C) >> 2);
    p->lowpass_cutoff = PATCH_LOWPASS_CUTOFF_LOWER_BOUND + (current_byte & 0x03);

    /* vibrato depth, effect modes */
    current_byte = patch_set_data[FILEIO_COMPUTE_PATCH_GENERAL_INDEX(k, VIBRATO_DEPTH_EFFECT_MODES)];

    p->vibrato_mode = PATCH_VIBRATO_MODE_LOWER_BOUND + ((current_byte & 0x40) >> 6);
    p->tremolo_mode = PATCH_TREMOLO_MODE_LOWER_BOUND + ((current_byte & 0x20) >> 5);
    p->boost_mode = PATCH_BOOST_MODE_LOWER_BOUND + ((current_byte & 0x10) >> 4);
    p->vibrato_depth = PATCH_EFFECT_DEPTH_LOWER_BOUND + (current_byte & 0x0F);

    /* tremolo & boost depths */
    current_byte = patch_set_data[FILEIO_COMPUTE_PATCH_GENERAL_INDEX(k, TREMOLO_BOOST_DEPTH)];

    p->tremolo_depth = PATCH_EFFECT_DEPTH_LOWER_BOUND + ((current_byte & 0xF0) >> 4);
    p->boost_depth = PATCH_EFFECT_DEPTH_LOWER_BOUND + (current_byte & 0x0F);

    /* mod wheel effect, vibrato base */
    current_byte = patch_set_data[FILEIO_COMPUTE_PATCH_GENERAL_INDEX(k, MOD_WHEEL_EFFECT_VIBRATO_BASE)];

    p->mod_wheel_effect = PATCH_CONTROLLER_EFFECT_LOWER_BOUND + ((current_byte & 0xC0) >> 6);
    p->vibrato_base = PATCH_EFFECT_BASE_LOWER_BOUND + (current_byte & 0x1F);

    /* aftertouch effect, tremolo base */
    current_byte = patch_set_data[FILEIO_COMPUTE_PATCH_GENERAL_INDEX(k, AFTERTOUCH_EFFECT_TREMOLO_BASE)];

    p->aftertouch_effect = PATCH_CONTROLLER_EFFECT_LOWER_BOUND + ((current_byte & 0xC0) >> 6);
    p->tremolo_base = PATCH_EFFECT_BASE_LOWER_BOUND + (current_byte & 0x1F);

    /* portamento mode, legato, and speed */
    current_byte = patch_set_data[FILEIO_COMPUTE_PATCH_GENERAL_INDEX(k, PORTAMENTO_MODE_LEGATO_SPEED)];

    p->portamento_mode = PATCH_PORTAMENTO_MODE_LOWER_BOUND + ((current_byte & 0x80) >> 7);
    p->portamento_legato = PATCH_PORTAMENTO_LEGATO_LOWER_BOUND + ((current_byte & 0x40) >> 6);
    p->portamento_speed = PATCH_PORTAMENTO_SPEED_LOWER_BOUND + (current_byte & 0x0F);

    /* noise mode, frequency */
    current_byte = patch_set_data[FILEIO_COMPUTE_PATCH_GENERAL_INDEX(k, NOISE_MODE_FREQUENCY)];

    p->noise_mode = PATCH_NOISE_MODE_LOWER_BOUND + ((current_byte & 0xC0) >> 6);
    p->noise_frequency = PATCH_NOISE_FREQUENCY_LOWER_BOUND + (current_byte & 0x1F);

    /* oscillator sync, lfo sync, pedal adjust */
    current_byte = patch_set_data[FILEIO_COMPUTE_PATCH_GENERAL_INDEX(k, SYNC_PEDAL_ADJUST)];

    p->sync_osc = PATCH_SYNC_OSC_LOWER_BOUND + ((current_byte & 0x80) >> 7);
    p->sync_lfo = PATCH_SYNC_LFO_LOWER_BOUND + ((current_byte & 0x40) >> 6);
    p->pedal_adjust = PATCH_PEDAL_ADJUST_LOWER_BOUND + (current_byte & 0x0F);

    /* key follow modes, pitch wheel mode, range */
    current_byte = patch_set_data[FILEIO_COMPUTE_PATCH_GENERAL_INDEX(k, KEY_FOLLOW_PITCH_WHEEL_MODE_RANGE)];

    p->key_follow_rate = PATCH_KEY_FOLLOW_MODE_LOWER_BOUND + ((current_byte & 0x80) >> 7);
    p->key_follow_level = PATCH_KEY_FOLLOW_MODE_LOWER_BOUND + ((current_byte & 0x40) >> 6);
    p->pitch_wheel_mode = PATCH_PITCH_WHEEL_MODE_LOWER_BOUND + ((current_byte & 0x20) >> 5);
    p->pitch_wheel_range = PATCH_PITCH_WHEEL_RANGE_LOWER_BOUND + (current_byte & 0x0F);

    /* lfo waveform, frequency */
    current_byte = patch_set_data[FILEIO_COMPUTE_PATCH_GENERAL_INDEX(k, LFO_WAVEFORM_FREQUENCY)];

    p->lfo_waveform = PATCH_LFO_WAVEFORM_LOWER_BOUND + ((current_byte & 0xE0) >> 5);
    p->lfo_frequency = PATCH_LFO_FREQUENCY_LOWER_BOUND + (current_byte & 0x1F);

    /* lfo delay */
    current_byte = patch_set_data[FILEIO_COMPUTE_PATCH_GENERAL_INDEX(k, LFO_DELAY)];

    p->lfo_delay = PATCH_LFO_DELAY_LOWER_BOUND + (current_byte & 0x1F);

    /* lfo sample & hold */
    current_byte = patch_set_data[FILEIO_COMPUTE_PATCH_GENERAL_INDEX(k, LFO_QUANTIZE)];

    p->lfo_quantize = PATCH_LFO_QUANTIZE_LOWER_BOUND + (current_byte & 0x1F);

    /* oscillators, envelopes */
    for (m = 0; m < 4; m++)
    {
      /* waveform, feedback */
      current_byte = patch_set_data[FILEIO_COMPUTE_PATCH_OSC_ENV_INDEX(k, m, WAVEFORM_FEEDBACK)];

      p->osc_waveform[m] = PATCH_OSC_WAVEFORM_LOWER_BOUND + ((current_byte & 0xF0) >> 4);
      p->osc_feedback[m] = PATCH_OSC_FEEDBACK_LOWER_BOUND + (current_byte & 0x0F);

      /* multiple, divisor or note, octave */
      current_byte = patch_set_data[FILEIO_COMPUTE_PATCH_OSC_ENV_INDEX(k, m, MULTIPLE_DIVISOR_OR_NOTE_OCTAVE)];

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
      current_byte = patch_set_data[FILEIO_COMPUTE_PATCH_OSC_ENV_INDEX(k, m, DETUNE)];

      p->osc_detune[m] = PATCH_OSC_DETUNE_LOWER_BOUND + (current_byte & 0x3F);

      /* phi, frequency mode, break point */
      current_byte = patch_set_data[FILEIO_COMPUTE_PATCH_OSC_ENV_INDEX(k, m, PHI_FREQ_MODE_BREAK_POINT)];

      p->osc_phi[m] = PATCH_OSC_PHI_LOWER_BOUND + ((current_byte & 0xC0) >> 6);
      p->osc_freq_mode[m] = PATCH_OSC_FREQ_MODE_LOWER_BOUND + ((current_byte & 0x10) >> 4);
      p->env_break_point[m] = PATCH_ENV_BREAK_POINT_LOWER_BOUND + (current_byte & 0x0F);

      /* attack, rate keyscaling */
      current_byte = patch_set_data[FILEIO_COMPUTE_PATCH_OSC_ENV_INDEX(k, m, ATTACK_RATE_KS)];

      p->env_rate_ks[m] = PATCH_ENV_KEYSCALING_LOWER_BOUND + ((current_byte & 0xE0) >> 5);
      p->env_attack[m] = PATCH_ENV_RATE_LOWER_BOUND + (current_byte & 0x1F);

      /* decay 1, level keyscaling */
      current_byte = patch_set_data[FILEIO_COMPUTE_PATCH_OSC_ENV_INDEX(k, m, DECAY_1_LEVEL_KS)];

      p->env_level_ks[m] = PATCH_ENV_KEYSCALING_LOWER_BOUND + ((current_byte & 0xE0) >> 5);
      p->env_decay_1[m] = PATCH_ENV_RATE_LOWER_BOUND + (current_byte & 0x1F);

      /* decay 2 */
      current_byte = patch_set_data[FILEIO_COMPUTE_PATCH_OSC_ENV_INDEX(k, m, DECAY_2)];

      p->env_decay_2[m] = PATCH_ENV_RATE_LOWER_BOUND + (current_byte & 0x1F);

      /* release */
      current_byte = patch_set_data[FILEIO_COMPUTE_PATCH_OSC_ENV_INDEX(k, m, RELEASE)];

      p->env_release[m] = PATCH_ENV_RATE_LOWER_BOUND + (current_byte & 0x1F);

      /* amplitude */
      current_byte = patch_set_data[FILEIO_COMPUTE_PATCH_OSC_ENV_INDEX(k, m, AMPLITUDE)];

      p->env_amplitude[m] = PATCH_ENV_AMPLITUDE_LOWER_BOUND + (current_byte & 0x3F);

      /* sustain */
      current_byte = patch_set_data[FILEIO_COMPUTE_PATCH_OSC_ENV_INDEX(k, m, SUSTAIN)];

      p->env_sustain[m] = PATCH_ENV_SUSTAIN_LOWER_BOUND + (current_byte & 0x1F);
    }

    /* validate the parameters */
    patch_validate(set_num * FILEIO_PATCHES_PER_SET + k);
  }

  return 0;
}

/*******************************************************************************
** fileio_patch_set_save()
*******************************************************************************/
short int fileio_patch_set_save(char* filename, int set_num)
{
  int k;
  int m;

  FILE* fp;
  char  signature[4];
  char  type[4];

  unsigned char patch_set_data[FILEIO_SET_NUM_BYTES];

  unsigned char current_byte;

  patch* p;

  /* make sure filename is valid */
  if (filename == NULL)
    return 0;

  /* make sure patch set number is valid */
  if ((set_num < 0) || (set_num >= FILEIO_NUM_SETS))
    return 0;

  /* initialize patch set data */
  for (m = 0; m < FILEIO_SET_NUM_BYTES; m++)
    patch_set_data[m] = 0;

  /* generate patch set data */
  for (k = 0; k < FILEIO_PATCHES_PER_SET; k++)
  {
    p = &G_patch_bank[set_num * FILEIO_PATCHES_PER_SET + k];

    /* algorithm, filter cutoffs */
    current_byte = ((p->algorithm - PATCH_ALGORITHM_LOWER_BOUND) & 0x07) << 4;
    current_byte |= ((p->highpass_cutoff - PATCH_HIGHPASS_CUTOFF_LOWER_BOUND) & 0x03) << 2;
    current_byte |= (p->lowpass_cutoff - PATCH_LOWPASS_CUTOFF_LOWER_BOUND) & 0x03;

    patch_set_data[FILEIO_COMPUTE_PATCH_GENERAL_INDEX(k, ALGORITHM_FILTER_CUTOFFS)] = current_byte;

    /* vibrato depth, effect modes */
    current_byte = ((p->vibrato_mode - PATCH_VIBRATO_MODE_LOWER_BOUND) & 0x01) << 6;
    current_byte |= ((p->tremolo_mode - PATCH_TREMOLO_MODE_LOWER_BOUND) & 0x01) << 5;
    current_byte |= ((p->boost_mode - PATCH_BOOST_MODE_LOWER_BOUND) & 0x01) << 4;
    current_byte |= (p->vibrato_depth - PATCH_EFFECT_DEPTH_LOWER_BOUND) & 0x0F;

    patch_set_data[FILEIO_COMPUTE_PATCH_GENERAL_INDEX(k, VIBRATO_DEPTH_EFFECT_MODES)] = current_byte;

    /* tremolo & boost depths */
    current_byte = ((p->tremolo_depth - PATCH_EFFECT_DEPTH_LOWER_BOUND) & 0x0F) << 4;
    current_byte |= (p->boost_depth - PATCH_EFFECT_DEPTH_LOWER_BOUND) & 0x0F;

    patch_set_data[FILEIO_COMPUTE_PATCH_GENERAL_INDEX(k, TREMOLO_BOOST_DEPTH)] = current_byte;

    /* mod wheel effect, vibrato base */
    current_byte = ((p->mod_wheel_effect - PATCH_CONTROLLER_EFFECT_LOWER_BOUND) & 0x03) << 6;
    current_byte |= (p->vibrato_base - PATCH_EFFECT_BASE_LOWER_BOUND) & 0x1F;

    patch_set_data[FILEIO_COMPUTE_PATCH_GENERAL_INDEX(k, MOD_WHEEL_EFFECT_VIBRATO_BASE)] = current_byte;

    /* aftertouch effect, tremolo base */
    current_byte = ((p->aftertouch_effect - PATCH_CONTROLLER_EFFECT_LOWER_BOUND) & 0x03) << 6;
    current_byte |= (p->tremolo_base - PATCH_EFFECT_BASE_LOWER_BOUND) & 0x1F;

    patch_set_data[FILEIO_COMPUTE_PATCH_GENERAL_INDEX(k, AFTERTOUCH_EFFECT_TREMOLO_BASE)] = current_byte;

    /* portamento mode, legato, and speed */
    current_byte = ((p->portamento_mode - PATCH_PORTAMENTO_MODE_LOWER_BOUND) & 0x01) << 7;
    current_byte |= ((p->portamento_legato - PATCH_PORTAMENTO_LEGATO_LOWER_BOUND) & 0x01) << 6;
    current_byte |= (p->portamento_speed - PATCH_PORTAMENTO_SPEED_LOWER_BOUND) & 0x0F;

    patch_set_data[FILEIO_COMPUTE_PATCH_GENERAL_INDEX(k, PORTAMENTO_MODE_LEGATO_SPEED)] = current_byte;

    /* noise mode, frequency */
    current_byte = ((p->noise_mode - PATCH_NOISE_MODE_LOWER_BOUND) & 0x03) << 6;
    current_byte |= (p->noise_frequency - PATCH_NOISE_FREQUENCY_LOWER_BOUND) & 0x1F;

    patch_set_data[FILEIO_COMPUTE_PATCH_GENERAL_INDEX(k, NOISE_MODE_FREQUENCY)] = current_byte;

    /* oscillator sync, lfo sync, pedal adjust */
    current_byte = ((p->sync_osc - PATCH_SYNC_OSC_LOWER_BOUND) & 0x01) << 7;
    current_byte |= ((p->sync_lfo - PATCH_SYNC_LFO_LOWER_BOUND) & 0x01) << 6;
    current_byte |= (p->pedal_adjust - PATCH_PEDAL_ADJUST_LOWER_BOUND) & 0x0F;

    patch_set_data[FILEIO_COMPUTE_PATCH_GENERAL_INDEX(k, SYNC_PEDAL_ADJUST)] = current_byte;

    /* key follow modes, pitch wheel mode, range */
    current_byte = ((p->key_follow_rate - PATCH_KEY_FOLLOW_MODE_LOWER_BOUND) & 0x01) << 7;
    current_byte |= ((p->key_follow_level - PATCH_KEY_FOLLOW_MODE_LOWER_BOUND) & 0x01) << 6;
    current_byte |= ((p->pitch_wheel_mode - PATCH_PITCH_WHEEL_MODE_LOWER_BOUND) & 0x01) << 5;
    current_byte |= (p->pitch_wheel_range - PATCH_PITCH_WHEEL_RANGE_LOWER_BOUND) & 0x0F;

    patch_set_data[FILEIO_COMPUTE_PATCH_GENERAL_INDEX(k, KEY_FOLLOW_PITCH_WHEEL_MODE_RANGE)] = current_byte;

    /* lfo waveform, frequency */
    current_byte = ((p->lfo_waveform - PATCH_LFO_WAVEFORM_LOWER_BOUND) & 0x07) << 5;
    current_byte |= (p->lfo_frequency - PATCH_LFO_FREQUENCY_LOWER_BOUND) & 0x1F;

    patch_set_data[FILEIO_COMPUTE_PATCH_GENERAL_INDEX(k, LFO_WAVEFORM_FREQUENCY)] = current_byte;

    /* lfo delay */
    current_byte = (p->lfo_delay - PATCH_LFO_DELAY_LOWER_BOUND) & 0x1F;

    patch_set_data[FILEIO_COMPUTE_PATCH_GENERAL_INDEX(k, LFO_DELAY)] = current_byte;

    /* lfo sample and hold */
    current_byte = (p->lfo_quantize - PATCH_LFO_QUANTIZE_LOWER_BOUND) & 0x1F;

    patch_set_data[FILEIO_COMPUTE_PATCH_GENERAL_INDEX(k, LFO_QUANTIZE)] = current_byte;

    /* oscillators, envelopes */
    for (m = 0; m < 4; m++)
    {
      /* waveform, feedback */
      current_byte = ((p->osc_waveform[m] - PATCH_OSC_WAVEFORM_LOWER_BOUND) & 0x0F) << 4;
      current_byte |= (p->osc_feedback[m] - PATCH_OSC_FEEDBACK_LOWER_BOUND) & 0x0F;

      patch_set_data[FILEIO_COMPUTE_PATCH_OSC_ENV_INDEX(k, m, WAVEFORM_FEEDBACK)] = current_byte;

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

      patch_set_data[FILEIO_COMPUTE_PATCH_OSC_ENV_INDEX(k, m, MULTIPLE_DIVISOR_OR_NOTE_OCTAVE)] = current_byte;

      /* detune */
      current_byte = (p->osc_detune[m] - PATCH_OSC_DETUNE_LOWER_BOUND) & 0x3F;

      patch_set_data[FILEIO_COMPUTE_PATCH_OSC_ENV_INDEX(k, m, DETUNE)] = current_byte;

      /* phi, frequency mode, break point */
      current_byte = ((p->osc_phi[m] - PATCH_OSC_PHI_LOWER_BOUND) & 0x03) << 6;
      current_byte |= ((p->osc_freq_mode[m] - PATCH_OSC_FREQ_MODE_LOWER_BOUND) & 0x01) << 4;
      current_byte |= (p->env_break_point[m] - PATCH_ENV_BREAK_POINT_LOWER_BOUND) & 0x0F;

      patch_set_data[FILEIO_COMPUTE_PATCH_OSC_ENV_INDEX(k, m, PHI_FREQ_MODE_BREAK_POINT)] = current_byte;

      /* attack, rate keyscaling  */
      current_byte = ((p->env_rate_ks[m] - PATCH_ENV_KEYSCALING_LOWER_BOUND) & 0x07) << 5;
      current_byte |= (p->env_attack[m] - PATCH_ENV_RATE_LOWER_BOUND) & 0x1F;

      patch_set_data[FILEIO_COMPUTE_PATCH_OSC_ENV_INDEX(k, m, ATTACK_RATE_KS)] = current_byte;

      /* decay 1, level keyscaling */
      current_byte = ((p->env_level_ks[m] - PATCH_ENV_KEYSCALING_LOWER_BOUND) & 0x07) << 5;
      current_byte |= (p->env_decay_1[m] - PATCH_ENV_RATE_LOWER_BOUND) & 0x1F;

      patch_set_data[FILEIO_COMPUTE_PATCH_OSC_ENV_INDEX(k, m, DECAY_1_LEVEL_KS)] = current_byte;

      /* decay 2 */
      current_byte = (p->env_decay_2[m] - PATCH_ENV_RATE_LOWER_BOUND) & 0x1F;

      patch_set_data[FILEIO_COMPUTE_PATCH_OSC_ENV_INDEX(k, m, DECAY_2)] = current_byte;

      /* release */
      current_byte = (p->env_release[m] - PATCH_ENV_RATE_LOWER_BOUND) & 0x1F;

      patch_set_data[FILEIO_COMPUTE_PATCH_OSC_ENV_INDEX(k, m, RELEASE)] = current_byte;

      /* amplitude */
      current_byte = (p->env_amplitude[m] - PATCH_ENV_AMPLITUDE_LOWER_BOUND) & 0x3F;

      patch_set_data[FILEIO_COMPUTE_PATCH_OSC_ENV_INDEX(k, m, AMPLITUDE)] = current_byte;

      /* sustain */
      current_byte = (p->env_sustain[m] - PATCH_ENV_SUSTAIN_LOWER_BOUND) & 0x1F;

      patch_set_data[FILEIO_COMPUTE_PATCH_OSC_ENV_INDEX(k, m, SUSTAIN)] = current_byte;
    }
  }

  /* open patch set file */
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
  type[0] = 'P';
  type[1] = 'A';
  type[2] = 'T';
  type[3] = 'C';

  if (fwrite(type, 1, 4, fp) < 4)
  {
    fclose(fp);
    return 1;
  }

  /* write patch set data */
  if (fwrite(patch_set_data, 1, FILEIO_SET_NUM_BYTES, fp) == 0)
  {
    fclose(fp);
    return 1;
  }

  /* close patch set file */
  fclose(fp);

  return 0;
}

