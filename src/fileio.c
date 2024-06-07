/*******************************************************************************
** fileio.c (loading and saving from native file formats)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bank.h"
#include "fileio.h"
#include "patch.h"

/* 52 bytes */
enum
{
  /* osc frequency mode, detune, waveform (3 bytes) */
  FILEIO_PATCH_BYTE_OSC_1_FREQ_MODE_DETUNE_WAVEFORM = 0,  /* frequency mode (1 bit), detune (3 bits), waveform (4 bits) */
  FILEIO_PATCH_BYTE_OSC_2_FREQ_MODE_DETUNE_WAVEFORM,      /* frequency mode (1 bit), detune (3 bits), waveform (4 bits) */
  FILEIO_PATCH_BYTE_OSC_3_FREQ_MODE_DETUNE_WAVEFORM,      /* frequency mode (1 bit), detune (3 bits), waveform (4 bits) */
  /* osc multiple, divisor (3 bytes) */
  FILEIO_PATCH_BYTE_OSC_1_MULTIPLE_DIVISOR,               /* multiple or octave (4 bits), divisor or note (4 bits) */
  FILEIO_PATCH_BYTE_OSC_2_MULTIPLE_DIVISOR,               /* multiple or octave (4 bits), divisor or note (4 bits) */
  FILEIO_PATCH_BYTE_OSC_3_MULTIPLE_DIVISOR,               /* multiple or octave (4 bits), divisor or note (4 bits) */
  /* osc sync, phi (1 byte) */
  FILEIO_PATCH_BYTE_OSC_SYNC_PHI,                         /* osc sync (1 bit), phi (2 bits each, 6 bits total) */
  /* algorithm, filters, osc / env routing (3 bytes) */
  FILEIO_PATCH_BYTE_ALGORITHM_OSC_1_ENV_1_ROUTING,        /* algorithm (2 bits), osc routing (3 bits), env routing (3 bits) */
  FILEIO_PATCH_BYTE_HIGHPASS_CUTOFF_OSC_2_ENV_2_ROUTING,  /* highpass cutoff (2 bits), osc routing (3 bits), env routing (3 bits) */
  FILEIO_PATCH_BYTE_LOWPASS_CUTOFF_OSC_3_ENV_3_ROUTING,   /* lowpass cutoff (2 bits), osc routing (3 bits), env routing (3 bits) */
  /* envelope 1 (7 bytes) */
  FILEIO_PATCH_BYTE_ENV_1_ATTACK,                         /* attack time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_1_DECAY,                          /* decay time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_1_SUSTAIN,                        /* sustain time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_1_RELEASE,                        /* release time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_1_AMPLITUDE,                      /* max level (7 bits) */
  FILEIO_PATCH_BYTE_ENV_1_HOLD_LEVEL,                     /* hold level (7 bits) */
  FILEIO_PATCH_BYTE_ENV_1_HOLD_MODE_KEYSCALING,           /* hold mode (2 bits), rate keyscaling (3 bits), level keyscaling (3 bits) */
  /* envelope 2 (7 bytes) */
  FILEIO_PATCH_BYTE_ENV_2_ATTACK,                         /* attack time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_2_DECAY,                          /* decay time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_2_SUSTAIN,                        /* sustain time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_2_RELEASE,                        /* release time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_2_AMPLITUDE,                      /* max level (7 bits) */
  FILEIO_PATCH_BYTE_ENV_2_HOLD_LEVEL,                     /* hold level (7 bits) */
  FILEIO_PATCH_BYTE_ENV_2_HOLD_MODE_KEYSCALING,           /* hold mode (2 bits), rate keyscaling (3 bits), level keyscaling (3 bits) */
  /* envelope 3 (7 bytes) */
  FILEIO_PATCH_BYTE_ENV_3_ATTACK,                         /* attack time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_3_DECAY,                          /* decay time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_3_SUSTAIN,                        /* sustain time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_3_RELEASE,                        /* release time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_3_AMPLITUDE,                      /* max level (7 bits) */
  FILEIO_PATCH_BYTE_ENV_3_HOLD_LEVEL,                     /* hold level (7 bits) */
  FILEIO_PATCH_BYTE_ENV_3_HOLD_MODE_KEYSCALING,           /* hold mode (2 bits), rate keyscaling (3 bits), level keyscaling (3 bits) */
  /* vibrato lfo (3 bytes) */
  FILEIO_PATCH_BYTE_VIBRATO_POLARITY_WAVEFORM_SPEED,      /* polarity (1 bit), waveform (2 bits), speed (4 bits) */
  FILEIO_PATCH_BYTE_VIBRATO_SYNC_DELAY,                   /* sync (1 bit), delay (6 bits) */
  FILEIO_PATCH_BYTE_VIBRATO_DEPTH,                        /* depth (8 bits) */
  /* tremolo lfo (3 bytes) */
  FILEIO_PATCH_BYTE_TREMOLO_WAVEFORM_SPEED,               /* waveform (2 bits), speed (4 bits) */
  FILEIO_PATCH_BYTE_TREMOLO_SYNC_DELAY,                   /* sync (1 bit), delay (6 bits) */
  FILEIO_PATCH_BYTE_TREMOLO_DEPTH,                        /* depth (8 bits) */
  /* chorus (3 bytes) */
  FILEIO_PATCH_BYTE_CHORUS_WAVEFORM_SPEED,                /* waveform (2 bits), speed (4 bits) */
  FILEIO_PATCH_BYTE_CHORUS_SYNC_DELAY,                    /* sync (1 bit), delay (6 bits) */
  FILEIO_PATCH_BYTE_CHORUS_DEPTH,                         /* depth (8 bits) */
  /* sensitivities, midi controller routing (4 bytes) */
  FILEIO_PATCH_BYTE_BOOST_VELOCITY_SENS,                  /* sensitivities (4 bits each, 8 bits total) */
  FILEIO_PATCH_BYTE_VIBRATO_TREMOLO_SENS,                 /* sensitivities (4 bits each, 8 bits total) */
  FILEIO_PATCH_BYTE_CHORUS_SENS_MOD_WHEEL_ROUTING,        /* chorus sensitivity (4 bits), mod wheel routing (4 bits) */
  FILEIO_PATCH_BYTE_AFTERTOUCH_EXP_PEDAL_ROUTING,         /* aftertouch routing (4 bits), exp pedal routing (4 bits) */
  /* pitch envelope (5 bytes) */
  FILEIO_PATCH_BYTE_PITCH_ENV_ATTACK,                     /* attack time (7 bits) */
  FILEIO_PATCH_BYTE_PITCH_ENV_DECAY,                      /* decay time (7 bits) */
  FILEIO_PATCH_BYTE_PITCH_ENV_RELEASE,                    /* release time (7 bits) */
  FILEIO_PATCH_BYTE_PITCH_ENV_MAXIMUM,                    /* max level (7 bits) */
  FILEIO_PATCH_BYTE_PITCH_ENV_FINALE,                     /* finale level (7 bits) */
  /* pitch wheel, arpeggio, portamento (3 bytes) */
  FILEIO_PATCH_BYTE_PITCH_WHEEL,                          /* pitch wheel mode (1 bit), range (5 bits) */
  FILEIO_PATCH_BYTE_ARPEGGIO,                             /* arpeggio pattern (2 bits), octaves (2 bits), speed (4 bits) */
  FILEIO_PATCH_BYTE_PORTAMENTO,                           /* arpeggio mode (1 bit), portamento mode (1 bit), follow/legato (2 bits), speed (4 bits) */
  FILEIO_PATCH_NUM_DATA_BYTES
};

/* 10 bytes */
#define FILEIO_CART_NUM_NAME_BYTES  (PATCH_CART_NAME_SIZE - 1)

/* 10 bytes */
#define FILEIO_PATCH_NUM_NAME_BYTES (PATCH_PATCH_NAME_SIZE - 1)

/* should be 10 + 52 = 62 bytes */
#define FILEIO_PATCH_NUM_TOTAL_BYTES  ( FILEIO_PATCH_NUM_NAME_BYTES +          \
                                        FILEIO_PATCH_NUM_DATA_BYTES)

/* should be 10 + (16 * 62) = 1002 bytes */
#define FILEIO_CART_NUM_TOTAL_BYTES   ( FILEIO_CART_NUM_NAME_BYTES +           \
                                        (FILEIO_PATCH_NUM_TOTAL_BYTES * BANK_PATCHES_PER_CART))

#define FILEIO_CART_NAME_INDEX 0

#define FILEIO_COMPUTE_PATCH_NAME_INDEX(patch_index)                           \
  ( FILEIO_CART_NUM_NAME_BYTES +                                               \
    (patch_index * FILEIO_PATCH_NUM_TOTAL_BYTES))

#define FILEIO_COMPUTE_PATCH_BYTE_INDEX(patch_index, byte)                     \
  ( FILEIO_CART_NUM_NAME_BYTES +                                               \
    (patch_index * FILEIO_PATCH_NUM_TOTAL_BYTES) +                             \
    FILEIO_PATCH_NUM_NAME_BYTES +                                              \
    FILEIO_PATCH_BYTE_##byte)

/*******************************************************************************
** fileio_cart_load()
*******************************************************************************/
short int fileio_cart_load(int cart_index, char* filename)
{
  int m;

  FILE* fp;
  char  signature[4];
  char  type[4];

  char cart_data[FILEIO_CART_NUM_TOTAL_BYTES];

  char current_byte;

  cart* c;
  patch* p;

  /* make sure cart index is valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(cart_index))
    return 1;

  /* make sure filename is valid */
  if (filename == NULL)
    return 1;

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
  if (fread(cart_data, 1, FILEIO_CART_NUM_TOTAL_BYTES, fp) == 0)
  {
    fclose(fp);
    return 1;
  }

  /* close cart file */
  fclose(fp);

  /* reset cart */
  patch_reset_cart(cart_index);

  /* obtain cart pointer */
  c = &G_cart_bank[cart_index];

  /* load cart name */
  strncpy(&c->name[0], &cart_data[FILEIO_CART_NAME_INDEX], PATCH_CART_NAME_SIZE);

  /* load cart data */
  for (m = 0; m < BANK_PATCHES_PER_CART; m++)
  {
    /* obtain patch pointer */
    p = &(c->patches[m]);

    /* load patch name */
    strncpy(&p->name[0], &cart_data[FILEIO_COMPUTE_PATCH_NAME_INDEX(m)], PATCH_PATCH_NAME_SIZE);

    /* load patch data */

    /* osc frequency mode, detune, waveform */
    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, OSC_1_FREQ_MODE_DETUNE_WAVEFORM)];

    p->osc_freq_mode[0] = PATCH_OSC_FREQ_MODE_LOWER_BOUND + ((current_byte >> 7) & 0x01);
    p->osc_detune[0] = PATCH_OSC_DETUNE_LOWER_BOUND + ((current_byte >> 4) & 0x07);
    p->osc_waveform[0] = PATCH_OSC_WAVEFORM_LOWER_BOUND + (current_byte & 0x0F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, OSC_2_FREQ_MODE_DETUNE_WAVEFORM)];

    p->osc_freq_mode[1] = PATCH_OSC_FREQ_MODE_LOWER_BOUND + ((current_byte >> 7) & 0x01);
    p->osc_detune[1] = PATCH_OSC_DETUNE_LOWER_BOUND + ((current_byte >> 4) & 0x07);
    p->osc_waveform[1] = PATCH_OSC_WAVEFORM_LOWER_BOUND + (current_byte & 0x0F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, OSC_3_FREQ_MODE_DETUNE_WAVEFORM)];

    p->osc_freq_mode[2] = PATCH_OSC_FREQ_MODE_LOWER_BOUND + ((current_byte >> 7) & 0x01);
    p->osc_detune[2] = PATCH_OSC_DETUNE_LOWER_BOUND + ((current_byte >> 4) & 0x07);
    p->osc_waveform[2] = PATCH_OSC_WAVEFORM_LOWER_BOUND + (current_byte & 0x0F);

    /* osc multiple, divisor */
    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, OSC_1_MULTIPLE_DIVISOR)];

    if (p->osc_freq_mode[0] == PATCH_OSC_FREQ_MODE_FIXED)
    {
      p->osc_octave[0] = PATCH_OSC_OCTAVE_LOWER_BOUND + ((current_byte >> 4) & 0x0F);
      p->osc_note[0] = PATCH_OSC_NOTE_LOWER_BOUND + (current_byte & 0x0F);
    }
    else
    {
      p->osc_multiple[0] = PATCH_OSC_MULTIPLE_LOWER_BOUND + ((current_byte >> 4) & 0x0F);
      p->osc_divisor[0] = PATCH_OSC_DIVISOR_LOWER_BOUND + (current_byte & 0x0F);
    }

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, OSC_2_MULTIPLE_DIVISOR)];

    if (p->osc_freq_mode[1] == PATCH_OSC_FREQ_MODE_FIXED)
    {
      p->osc_octave[1] = PATCH_OSC_OCTAVE_LOWER_BOUND + ((current_byte >> 4) & 0x0F);
      p->osc_note[1] = PATCH_OSC_NOTE_LOWER_BOUND + (current_byte & 0x0F);
    }
    else
    {
      p->osc_multiple[1] = PATCH_OSC_MULTIPLE_LOWER_BOUND + ((current_byte >> 4) & 0x0F);
      p->osc_divisor[1] = PATCH_OSC_DIVISOR_LOWER_BOUND + (current_byte & 0x0F);
    }

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, OSC_3_MULTIPLE_DIVISOR)];

    if (p->osc_freq_mode[2] == PATCH_OSC_FREQ_MODE_FIXED)
    {
      p->osc_octave[2] = PATCH_OSC_OCTAVE_LOWER_BOUND + ((current_byte >> 4) & 0x0F);
      p->osc_note[2] = PATCH_OSC_NOTE_LOWER_BOUND + (current_byte & 0x0F);
    }
    else
    {
      p->osc_multiple[2] = PATCH_OSC_MULTIPLE_LOWER_BOUND + ((current_byte >> 4) & 0x0F);
      p->osc_divisor[2] = PATCH_OSC_DIVISOR_LOWER_BOUND + (current_byte & 0x0F);
    }

    /* osc sync, phi */
    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, OSC_SYNC_PHI)];

    p->osc_sync = PATCH_SYNC_LOWER_BOUND + ((current_byte >> 6) & 0x01);
    p->osc_phi[0] = PATCH_OSC_PHI_LOWER_BOUND + ((current_byte >> 4) & 0x03);
    p->osc_phi[1] = PATCH_OSC_PHI_LOWER_BOUND + ((current_byte >> 2) & 0x03);
    p->osc_phi[2] = PATCH_OSC_PHI_LOWER_BOUND + (current_byte & 0x03);

    /* algorithm, filters, osc / env routing */
    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ALGORITHM_OSC_1_ENV_1_ROUTING)];

    p->algorithm = PATCH_ALGORITHM_LOWER_BOUND + ((current_byte >> 6) & 0x03);
    p->osc_routing[0] = PATCH_OSC_ROUTING_MASK & ((current_byte >> 3) & 0x07);
    p->env_routing[0] = PATCH_ENV_ROUTING_MASK & (current_byte & 0x07);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, HIGHPASS_CUTOFF_OSC_2_ENV_2_ROUTING)];

    p->highpass_cutoff = PATCH_HIGHPASS_CUTOFF_LOWER_BOUND + ((current_byte >> 6) & 0x03);
    p->osc_routing[1] = PATCH_OSC_ROUTING_MASK & ((current_byte >> 3) & 0x07);
    p->env_routing[1] = PATCH_ENV_ROUTING_MASK & (current_byte & 0x07);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, LOWPASS_CUTOFF_OSC_3_ENV_3_ROUTING)];

    p->lowpass_cutoff = PATCH_LOWPASS_CUTOFF_LOWER_BOUND + ((current_byte >> 6) & 0x03);
    p->osc_routing[2] = PATCH_OSC_ROUTING_MASK & ((current_byte >> 3) & 0x07);
    p->env_routing[2] = PATCH_ENV_ROUTING_MASK & (current_byte & 0x07);

    /* envelope 1 */
    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_1_ATTACK)];

    p->env_attack[0] = PATCH_ENV_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_1_DECAY)];

    p->env_decay[0] = PATCH_ENV_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_1_SUSTAIN)];

    p->env_sustain[0] = PATCH_ENV_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_1_RELEASE)];

    p->env_release[0] = PATCH_ENV_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_1_AMPLITUDE)];

    p->env_amplitude[0] = PATCH_ENV_LEVEL_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_1_HOLD_LEVEL)];

    p->env_hold_level[0] = PATCH_ENV_LEVEL_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_1_HOLD_MODE_KEYSCALING)];

    p->env_hold_mode[0] = PATCH_ENV_HOLD_MODE_LOWER_BOUND + ((current_byte >> 6) & 0x03);
    p->env_rate_ks[0] = PATCH_ENV_KEYSCALING_LOWER_BOUND + ((current_byte >> 3) & 0x07);
    p->env_level_ks[0] = PATCH_ENV_KEYSCALING_LOWER_BOUND + (current_byte & 0x07);

    /* envelope 2 */
    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_2_ATTACK)];

    p->env_attack[1] = PATCH_ENV_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_2_DECAY)];

    p->env_decay[1] = PATCH_ENV_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_2_SUSTAIN)];

    p->env_sustain[1] = PATCH_ENV_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_2_RELEASE)];

    p->env_release[1] = PATCH_ENV_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_2_AMPLITUDE)];

    p->env_amplitude[1] = PATCH_ENV_LEVEL_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_2_HOLD_LEVEL)];

    p->env_hold_level[1] = PATCH_ENV_LEVEL_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_2_HOLD_MODE_KEYSCALING)];

    p->env_hold_mode[1] = PATCH_ENV_HOLD_MODE_LOWER_BOUND + ((current_byte >> 6) & 0x03);
    p->env_rate_ks[1] = PATCH_ENV_KEYSCALING_LOWER_BOUND + ((current_byte >> 3) & 0x07);
    p->env_level_ks[1] = PATCH_ENV_KEYSCALING_LOWER_BOUND + (current_byte & 0x07);

    /* envelope 3 */
    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_3_ATTACK)];

    p->env_attack[2] = PATCH_ENV_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_3_DECAY)];

    p->env_decay[2] = PATCH_ENV_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_3_SUSTAIN)];

    p->env_sustain[2] = PATCH_ENV_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_3_RELEASE)];

    p->env_release[2] = PATCH_ENV_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_3_AMPLITUDE)];

    p->env_amplitude[2] = PATCH_ENV_LEVEL_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_3_HOLD_LEVEL)];

    p->env_hold_level[2] = PATCH_ENV_LEVEL_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_3_HOLD_MODE_KEYSCALING)];

    p->env_hold_mode[2] = PATCH_ENV_HOLD_MODE_LOWER_BOUND + ((current_byte >> 6) & 0x03);
    p->env_rate_ks[2] = PATCH_ENV_KEYSCALING_LOWER_BOUND + ((current_byte >> 3) & 0x07);
    p->env_level_ks[2] = PATCH_ENV_KEYSCALING_LOWER_BOUND + (current_byte & 0x07);

    /* vibrato lfo */
    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, VIBRATO_POLARITY_WAVEFORM_SPEED)];

    p->lfo_polarity[0] = PATCH_LFO_POLARITY_LOWER_BOUND + ((current_byte >> 7) & 0x01);
    p->lfo_waveform[0] = PATCH_LFO_WAVEFORM_LOWER_BOUND + ((current_byte >> 4) & 0x03);
    p->lfo_speed[0] = PATCH_LFO_SPEED_LOWER_BOUND + (current_byte & 0x0F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, VIBRATO_SYNC_DELAY)];

    p->lfo_sync[0] = PATCH_SYNC_LOWER_BOUND + ((current_byte >> 7) & 0x01);
    p->lfo_delay[0] = PATCH_LFO_DELAY_LOWER_BOUND + (current_byte & 0x3F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, VIBRATO_DEPTH)];

    p->lfo_depth[0] = PATCH_LFO_DEPTH_LOWER_BOUND + (current_byte & 0x7F);

    /* tremolo lfo */
    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, TREMOLO_WAVEFORM_SPEED)];

    p->lfo_waveform[1] = PATCH_LFO_WAVEFORM_LOWER_BOUND + ((current_byte >> 4) & 0x03);
    p->lfo_speed[1] = PATCH_LFO_SPEED_LOWER_BOUND + (current_byte & 0x0F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, TREMOLO_SYNC_DELAY)];

    p->lfo_sync[1] = PATCH_SYNC_LOWER_BOUND + ((current_byte >> 7) & 0x01);
    p->lfo_delay[1] = PATCH_LFO_DELAY_LOWER_BOUND + (current_byte & 0x3F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, TREMOLO_DEPTH)];

    p->lfo_depth[1] = PATCH_LFO_DEPTH_LOWER_BOUND + (current_byte & 0x7F);

    /* chorus */
    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, CHORUS_WAVEFORM_SPEED)];

    p->lfo_waveform[2] = PATCH_LFO_WAVEFORM_LOWER_BOUND + ((current_byte >> 4) & 0x03);
    p->lfo_speed[2] = PATCH_LFO_SPEED_LOWER_BOUND + (current_byte & 0x0F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, CHORUS_SYNC_DELAY)];

    p->lfo_sync[2] = PATCH_SYNC_LOWER_BOUND + ((current_byte >> 7) & 0x01);
    p->lfo_delay[2] = PATCH_LFO_DELAY_LOWER_BOUND + (current_byte & 0x3F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, CHORUS_DEPTH)];

    p->lfo_depth[2] = PATCH_LFO_DEPTH_LOWER_BOUND + (current_byte & 0x7F);

    /* sensitivities, midi controller routing */
    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, BOOST_VELOCITY_SENS)];

    p->boost_sensitivity = PATCH_SENSITIVITY_LOWER_BOUND + ((current_byte >> 4) & 0x0F);
    p->velocity_sensitivity = PATCH_SENSITIVITY_LOWER_BOUND + (current_byte & 0x0F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, VIBRATO_TREMOLO_SENS)];

    p->lfo_sensitivity[0] = PATCH_SENSITIVITY_LOWER_BOUND + ((current_byte >> 4) & 0x0F);
    p->lfo_sensitivity[1] = PATCH_SENSITIVITY_LOWER_BOUND + (current_byte & 0x0F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, CHORUS_SENS_MOD_WHEEL_ROUTING)];

    p->lfo_sensitivity[2] = PATCH_SENSITIVITY_LOWER_BOUND + ((current_byte >> 4) & 0x0F);
    p->mod_wheel_routing = PATCH_MIDI_CONT_ROUTING_MASK & (current_byte & 0x0F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, AFTERTOUCH_EXP_PEDAL_ROUTING)];

    p->aftertouch_routing = PATCH_MIDI_CONT_ROUTING_MASK & ((current_byte >> 4) & 0x0F);
    p->exp_pedal_routing = PATCH_MIDI_CONT_ROUTING_MASK & (current_byte & 0x0F);

    /* pitch envelope */
    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, PITCH_ENV_ATTACK)];

    p->peg_attack = PATCH_PEG_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, PITCH_ENV_DECAY)];

    p->peg_decay = PATCH_PEG_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, PITCH_ENV_RELEASE)];

    p->peg_release = PATCH_PEG_TIME_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, PITCH_ENV_MAXIMUM)];

    p->peg_maximum = PATCH_PEG_LEVEL_LOWER_BOUND + (current_byte & 0x7F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, PITCH_ENV_FINALE)];

    p->peg_finale = PATCH_PEG_LEVEL_LOWER_BOUND + (current_byte & 0x7F);

    /* pitch wheel, arpeggio, portamento */
    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, PITCH_WHEEL)];

    p->pitch_wheel_mode = PATCH_PITCH_WHEEL_MODE_LOWER_BOUND + ((current_byte >> 6) & 0x01);
    p->pitch_wheel_range = PATCH_PITCH_WHEEL_RANGE_LOWER_BOUND + (current_byte & 0x1F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ARPEGGIO)];

    p->arpeggio_pattern = PATCH_ARPEGGIO_PATTERN_LOWER_BOUND + ((current_byte >> 6) & 0x03);
    p->arpeggio_octaves = PATCH_ARPEGGIO_OCTAVES_LOWER_BOUND + ((current_byte >> 4) & 0x03);
    p->arpeggio_speed = PATCH_ARPEGGIO_SPEED_LOWER_BOUND + (current_byte & 0x0F);

    current_byte = cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, PORTAMENTO)];

    p->arpeggio_mode = PATCH_ARPEGGIO_MODE_LOWER_BOUND + ((current_byte >> 7) & 0x01);
    p->portamento_mode = PATCH_PORTAMENTO_MODE_LOWER_BOUND + ((current_byte >> 6) & 0x01);
    p->portamento_legato = PATCH_PORTAMENTO_LEGATO_LOWER_BOUND + ((current_byte >> 4) & 0x03);
    p->portamento_speed = PATCH_PORTAMENTO_SPEED_LOWER_BOUND + (current_byte & 0x0F);
  }

  /* validate the cart */
  patch_validate_cart(cart_index);

  return 0;
}

/*******************************************************************************
** fileio_cart_save()
*******************************************************************************/
short int fileio_cart_save(int cart_index, char* filename)
{
  int m;

  FILE* fp;
  char  signature[4];
  char  type[4];

  char cart_data[FILEIO_CART_NUM_TOTAL_BYTES];

  char current_byte;

  cart* c;
  patch* p;

  /* make sure cart index is valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(cart_index))
    return 1;

  /* make sure filename is valid */
  if (filename == NULL)
    return 0;

  /* initialize cart data */
  for (m = 0; m < FILEIO_CART_NUM_TOTAL_BYTES; m++)
    cart_data[m] = 0;

  /* validate the cart */
  patch_validate_cart(cart_index);

  /* obtain cart pointer */
  c = &G_cart_bank[cart_index];

  /* save cart name */
  strncpy(&cart_data[FILEIO_CART_NAME_INDEX], &c->name[0], PATCH_CART_NAME_SIZE);

  /* save cart data */
  for (m = 0; m < BANK_PATCHES_PER_CART; m++)
  {
    /* obtain patch pointer */
    p = &(c->patches[m]);

    /* save patch name */
    strncpy(&cart_data[FILEIO_COMPUTE_PATCH_NAME_INDEX(m)], &p->name[0], PATCH_PATCH_NAME_SIZE);

    /* osc frequency mode, detune, waveform */
    current_byte = ((p->osc_freq_mode[0] - PATCH_OSC_FREQ_MODE_LOWER_BOUND) & 0x01) << 7;
    current_byte |= ((p->osc_detune[0] - PATCH_OSC_DETUNE_LOWER_BOUND) & 0x07) << 4;
    current_byte |= (p->osc_waveform[0] - PATCH_OSC_WAVEFORM_LOWER_BOUND) & 0x0F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, OSC_1_FREQ_MODE_DETUNE_WAVEFORM)] = current_byte;

    current_byte = ((p->osc_freq_mode[1] - PATCH_OSC_FREQ_MODE_LOWER_BOUND) & 0x01) << 7;
    current_byte |= ((p->osc_detune[1] - PATCH_OSC_DETUNE_LOWER_BOUND) & 0x07) << 4;
    current_byte |= (p->osc_waveform[1] - PATCH_OSC_WAVEFORM_LOWER_BOUND) & 0x0F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, OSC_2_FREQ_MODE_DETUNE_WAVEFORM)] = current_byte;

    current_byte = ((p->osc_freq_mode[2] - PATCH_OSC_FREQ_MODE_LOWER_BOUND) & 0x01) << 7;
    current_byte |= ((p->osc_detune[2] - PATCH_OSC_DETUNE_LOWER_BOUND) & 0x07) << 4;
    current_byte |= (p->osc_waveform[2] - PATCH_OSC_WAVEFORM_LOWER_BOUND) & 0x0F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, OSC_3_FREQ_MODE_DETUNE_WAVEFORM)] = current_byte;

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

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, OSC_1_MULTIPLE_DIVISOR)] = current_byte;

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

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, OSC_2_MULTIPLE_DIVISOR)] = current_byte;

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

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, OSC_3_MULTIPLE_DIVISOR)] = current_byte;

    /* osc sync, phi */
    current_byte = ((p->osc_sync - PATCH_SYNC_LOWER_BOUND) & 0x01) << 6;
    current_byte |= ((p->osc_phi[0] - PATCH_OSC_PHI_LOWER_BOUND) & 0x03) << 4;
    current_byte |= ((p->osc_phi[1] - PATCH_OSC_PHI_LOWER_BOUND) & 0x03) << 2;
    current_byte |= (p->osc_phi[2] - PATCH_OSC_PHI_LOWER_BOUND) & 0x03;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, OSC_SYNC_PHI)] = current_byte;

    /* algorithm, filters, osc / env routing */
    current_byte = ((p->algorithm - PATCH_ALGORITHM_LOWER_BOUND) & 0x03) << 6;
    current_byte |= ((p->osc_routing[0] & PATCH_OSC_ROUTING_MASK) & 0x07) << 3;
    current_byte |= (p->env_routing[0] & PATCH_ENV_ROUTING_MASK) & 0x07;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ALGORITHM_OSC_1_ENV_1_ROUTING)] = current_byte;

    current_byte = ((p->highpass_cutoff - PATCH_HIGHPASS_CUTOFF_LOWER_BOUND) & 0x03) << 6;
    current_byte |= ((p->osc_routing[1] & PATCH_OSC_ROUTING_MASK) & 0x07) << 3;
    current_byte |= (p->env_routing[1] & PATCH_ENV_ROUTING_MASK) & 0x07;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, HIGHPASS_CUTOFF_OSC_2_ENV_2_ROUTING)] = current_byte;

    current_byte = ((p->lowpass_cutoff - PATCH_LOWPASS_CUTOFF_LOWER_BOUND) & 0x03) << 6;
    current_byte |= ((p->osc_routing[2] & PATCH_OSC_ROUTING_MASK) & 0x07) << 3;
    current_byte |= (p->env_routing[2] & PATCH_ENV_ROUTING_MASK) & 0x07;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, LOWPASS_CUTOFF_OSC_3_ENV_3_ROUTING)] = current_byte;

    /* envelope 1 */
    current_byte = (p->env_attack[0] - PATCH_ENV_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_1_ATTACK)] = current_byte;

    current_byte = (p->env_decay[0] - PATCH_ENV_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_1_DECAY)] = current_byte;

    current_byte = (p->env_sustain[0] - PATCH_ENV_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_1_SUSTAIN)] = current_byte;

    current_byte = (p->env_release[0] - PATCH_ENV_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_1_RELEASE)] = current_byte;

    current_byte = (p->env_amplitude[0] - PATCH_ENV_LEVEL_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_1_AMPLITUDE)] = current_byte;

    current_byte = (p->env_hold_level[0] - PATCH_ENV_LEVEL_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_1_HOLD_LEVEL)] = current_byte;

    current_byte = ((p->env_hold_mode[0] - PATCH_ENV_HOLD_MODE_LOWER_BOUND) & 0x03) << 6;
    current_byte |= ((p->env_rate_ks[0] - PATCH_ENV_KEYSCALING_LOWER_BOUND) & 0x07) << 3;
    current_byte |= (p->env_level_ks[0] - PATCH_ENV_KEYSCALING_LOWER_BOUND) & 0x07;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_1_HOLD_MODE_KEYSCALING)] = current_byte;

    /* envelope 2 */
    current_byte = (p->env_attack[1] - PATCH_ENV_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_2_ATTACK)] = current_byte;

    current_byte = (p->env_decay[1] - PATCH_ENV_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_2_DECAY)] = current_byte;

    current_byte = (p->env_sustain[1] - PATCH_ENV_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_2_SUSTAIN)] = current_byte;

    current_byte = (p->env_release[1] - PATCH_ENV_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_2_RELEASE)] = current_byte;

    current_byte = (p->env_amplitude[1] - PATCH_ENV_LEVEL_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_2_AMPLITUDE)] = current_byte;

    current_byte = (p->env_hold_level[1] - PATCH_ENV_LEVEL_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_2_HOLD_LEVEL)] = current_byte;

    current_byte = ((p->env_hold_mode[1] - PATCH_ENV_HOLD_MODE_LOWER_BOUND) & 0x03) << 6;
    current_byte |= ((p->env_rate_ks[1] - PATCH_ENV_KEYSCALING_LOWER_BOUND) & 0x07) << 3;
    current_byte |= (p->env_level_ks[1] - PATCH_ENV_KEYSCALING_LOWER_BOUND) & 0x07;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_2_HOLD_MODE_KEYSCALING)] = current_byte;

    /* envelope 3 */
    current_byte = (p->env_attack[2] - PATCH_ENV_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_3_ATTACK)] = current_byte;

    current_byte = (p->env_decay[2] - PATCH_ENV_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_3_DECAY)] = current_byte;

    current_byte = (p->env_sustain[2] - PATCH_ENV_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_3_SUSTAIN)] = current_byte;

    current_byte = (p->env_release[2] - PATCH_ENV_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_3_RELEASE)] = current_byte;

    current_byte = (p->env_amplitude[2] - PATCH_ENV_LEVEL_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_3_AMPLITUDE)] = current_byte;

    current_byte = (p->env_hold_level[2] - PATCH_ENV_LEVEL_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_3_HOLD_LEVEL)] = current_byte;

    current_byte = ((p->env_hold_mode[2] - PATCH_ENV_HOLD_MODE_LOWER_BOUND) & 0x03) << 6;
    current_byte |= ((p->env_rate_ks[2] - PATCH_ENV_KEYSCALING_LOWER_BOUND) & 0x07) << 3;
    current_byte |= (p->env_level_ks[2] - PATCH_ENV_KEYSCALING_LOWER_BOUND) & 0x07;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ENV_3_HOLD_MODE_KEYSCALING)] = current_byte;

    /* vibrato lfo */
    current_byte = ((p->lfo_polarity[0] - PATCH_LFO_POLARITY_LOWER_BOUND) & 0x01) << 7;
    current_byte |= ((p->lfo_waveform[0] - PATCH_LFO_WAVEFORM_LOWER_BOUND) & 0x03) << 4;
    current_byte |= (p->lfo_speed[0] - PATCH_LFO_SPEED_LOWER_BOUND) & 0x0F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, VIBRATO_POLARITY_WAVEFORM_SPEED)] = current_byte;

    current_byte = ((p->lfo_sync[0] - PATCH_SYNC_LOWER_BOUND) & 0x01) << 7;
    current_byte |= (p->lfo_delay[0] - PATCH_LFO_DELAY_LOWER_BOUND) & 0x3F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, VIBRATO_SYNC_DELAY)] = current_byte;

    current_byte = (p->lfo_depth[0] - PATCH_LFO_DEPTH_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, VIBRATO_DEPTH)] = current_byte;

    /* tremolo lfo */
    current_byte = ((p->lfo_waveform[1] - PATCH_LFO_WAVEFORM_LOWER_BOUND) & 0x03) << 4;
    current_byte |= (p->lfo_speed[1] - PATCH_LFO_SPEED_LOWER_BOUND) & 0x0F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, TREMOLO_WAVEFORM_SPEED)] = current_byte;

    current_byte = ((p->lfo_sync[1] - PATCH_SYNC_LOWER_BOUND) & 0x01) << 7;
    current_byte |= (p->lfo_delay[1] - PATCH_LFO_DELAY_LOWER_BOUND) & 0x3F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, TREMOLO_SYNC_DELAY)] = current_byte;

    current_byte = (p->lfo_depth[1] - PATCH_LFO_DEPTH_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, TREMOLO_DEPTH)] = current_byte;

    /* chorus */
    current_byte = ((p->lfo_waveform[2] - PATCH_LFO_WAVEFORM_LOWER_BOUND) & 0x03) << 4;
    current_byte |= (p->lfo_speed[2] - PATCH_LFO_SPEED_LOWER_BOUND) & 0x0F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, CHORUS_WAVEFORM_SPEED)] = current_byte;

    current_byte = ((p->lfo_sync[2] - PATCH_SYNC_LOWER_BOUND) & 0x01) << 7;
    current_byte |= (p->lfo_delay[2] - PATCH_LFO_DELAY_LOWER_BOUND) & 0x3F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, CHORUS_SYNC_DELAY)] = current_byte;

    current_byte = (p->lfo_depth[2] - PATCH_LFO_DEPTH_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, CHORUS_DEPTH)] = current_byte;

    /* sensitivities */
    current_byte = ((p->boost_sensitivity - PATCH_SENSITIVITY_LOWER_BOUND) & 0x0F) << 4;
    current_byte |= (p->velocity_sensitivity - PATCH_SENSITIVITY_LOWER_BOUND) & 0x0F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, BOOST_VELOCITY_SENS)] = current_byte;

    current_byte = ((p->lfo_sensitivity[0] - PATCH_SENSITIVITY_LOWER_BOUND) & 0x0F) << 4;
    current_byte |= (p->lfo_sensitivity[1] - PATCH_SENSITIVITY_LOWER_BOUND) & 0x0F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, VIBRATO_TREMOLO_SENS)] = current_byte;

    current_byte = ((p->lfo_sensitivity[2] - PATCH_SENSITIVITY_LOWER_BOUND) & 0x0F) << 4;
    current_byte |= (p->mod_wheel_routing & PATCH_MIDI_CONT_ROUTING_MASK) & 0x0F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, CHORUS_SENS_MOD_WHEEL_ROUTING)] = current_byte;

    current_byte = ((p->aftertouch_routing & PATCH_MIDI_CONT_ROUTING_MASK) & 0x0F) << 4;
    current_byte |= (p->exp_pedal_routing & PATCH_MIDI_CONT_ROUTING_MASK) & 0x0F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, AFTERTOUCH_EXP_PEDAL_ROUTING)] = current_byte;

    /* pitch envelope */
    current_byte = (p->peg_attack - PATCH_PEG_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, PITCH_ENV_ATTACK)] = current_byte;

    current_byte = (p->peg_decay - PATCH_PEG_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, PITCH_ENV_DECAY)] = current_byte;

    current_byte = (p->peg_release - PATCH_PEG_TIME_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, PITCH_ENV_RELEASE)] = current_byte;

    current_byte = (p->peg_maximum - PATCH_PEG_LEVEL_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, PITCH_ENV_MAXIMUM)] = current_byte;

    current_byte = (p->peg_finale - PATCH_PEG_LEVEL_LOWER_BOUND) & 0x7F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, PITCH_ENV_FINALE)] = current_byte;

    /* pitch wheel, arpeggio, portamento */
    current_byte = ((p->pitch_wheel_mode - PATCH_PITCH_WHEEL_MODE_LOWER_BOUND) & 0x01) << 6;
    current_byte |= (p->pitch_wheel_range - PATCH_PITCH_WHEEL_RANGE_LOWER_BOUND) & 0x1F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, PITCH_WHEEL)] = current_byte;

    current_byte = ((p->arpeggio_pattern - PATCH_ARPEGGIO_PATTERN_LOWER_BOUND) & 0x03) << 6;
    current_byte |= ((p->arpeggio_octaves - PATCH_ARPEGGIO_OCTAVES_LOWER_BOUND) & 0x03) << 4;
    current_byte |= (p->arpeggio_speed - PATCH_ARPEGGIO_SPEED_LOWER_BOUND) & 0x0F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, ARPEGGIO)] = current_byte;

    current_byte = ((p->arpeggio_mode - PATCH_ARPEGGIO_MODE_LOWER_BOUND) & 0x01) << 7;
    current_byte |= ((p->portamento_mode - PATCH_PORTAMENTO_MODE_LOWER_BOUND) & 0x01) << 6;
    current_byte |= ((p->portamento_legato - PATCH_PORTAMENTO_LEGATO_LOWER_BOUND) & 0x03) << 4;
    current_byte |= (p->portamento_speed - PATCH_PORTAMENTO_SPEED_LOWER_BOUND) & 0x0F;

    cart_data[FILEIO_COMPUTE_PATCH_BYTE_INDEX(m, PORTAMENTO)] = current_byte;
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
  if (fwrite(cart_data, 1, FILEIO_CART_NUM_TOTAL_BYTES, fp) == 0)
  {
    fclose(fp);
    return 1;
  }

  /* close cart file */
  fclose(fp);

  return 0;
}

