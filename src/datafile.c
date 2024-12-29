/*******************************************************************************
** datafile.c (native file format loading & saving)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bank.h"
#include "cart.h"
#include "datafile.h"

/* 50 bytes */
enum
{
  /* osc waveform, feedback, detune (4 bytes) */
  PATCH_DATA_BYTE_OSC_1_WAVEFORM_FEEDBACK_DETUNE = 0, /* waveform (2 bits), feedback (3 bits), detune (3 bits) */
  PATCH_DATA_BYTE_OSC_2_WAVEFORM_FEEDBACK_DETUNE,     /* waveform (2 bits), feedback (3 bits), detune (3 bits) */
  PATCH_DATA_BYTE_OSC_3_WAVEFORM_FEEDBACK_DETUNE,     /* waveform (2 bits), feedback (3 bits), detune (3 bits) */
  PATCH_DATA_BYTE_OSC_4_WAVEFORM_FEEDBACK_DETUNE,     /* waveform (2 bits), feedback (3 bits), detune (3 bits) */
  /* osc frequency mode, multiple, divisor (4 bytes) */
  PATCH_DATA_BYTE_OSC_1_FREQ_MODE_MULTIPLE_DIVISOR,   /* frequency mode (1 bit), multiple/divisor or note/octave (7 bits) */
  PATCH_DATA_BYTE_OSC_2_FREQ_MODE_MULTIPLE_DIVISOR,   /* frequency mode (1 bit), multiple/divisor or note/octave (7 bits) */
  PATCH_DATA_BYTE_OSC_3_FREQ_MODE_MULTIPLE_DIVISOR,   /* frequency mode (1 bit), multiple/divisor or note/octave (7 bits) */
  PATCH_DATA_BYTE_OSC_4_FREQ_MODE_MULTIPLE_DIVISOR,   /* frequency mode (1 bit), multiple/divisor or note/octave (7 bits) */
  /* osc phase shift (1 byte) */
  PATCH_DATA_BYTE_OSC_PHI,                            /* phi (8 bits total, 2 bits per envelope) */
  /* legacy keyscaling, noise enable, algorithm (1 byte) */
  PATCH_DATA_BYTE_OSC_SYNC_LEGACY_NOISE_ALGORITHM,    /* osc sync (1 bit), legacy keyscaling (1 bit), osc 4 noise (1 bit), algorithm (3 bits) */
  /* envelope 1 (5 bytes) */
  PATCH_DATA_BYTE_ENV_1_ATTACK,                       /* attack time (5 bits) */
  PATCH_DATA_BYTE_ENV_1_DECAY,                        /* decay time (5 bits) */
  PATCH_DATA_BYTE_ENV_1_HOLD_MODE_SUSTAIN,            /* sustain time (5 bits), hold mode (2 bits) */
  PATCH_DATA_BYTE_ENV_1_HOLD_LEVEL_RELEASE,           /* release time (4 bits), hold level (4 bits) */
  PATCH_DATA_BYTE_ENV_1_MAX_LEVEL,                    /* max level (7 bits) */
  /* envelope 2 (5 bytes) */
  PATCH_DATA_BYTE_ENV_2_ATTACK,                       /* attack time (5 bits) */
  PATCH_DATA_BYTE_ENV_2_DECAY,                        /* decay time (5 bits) */
  PATCH_DATA_BYTE_ENV_2_HOLD_MODE_SUSTAIN,            /* sustain time (5 bits), hold mode (2 bits) */
  PATCH_DATA_BYTE_ENV_2_HOLD_LEVEL_RELEASE,           /* release time (4 bits), hold level (4 bits) */
  PATCH_DATA_BYTE_ENV_2_MAX_LEVEL,                    /* max level (7 bits) */
  /* envelope 3 (5 bytes) */
  PATCH_DATA_BYTE_ENV_3_ATTACK,                       /* attack time (5 bits) */
  PATCH_DATA_BYTE_ENV_3_DECAY,                        /* decay time (5 bits) */
  PATCH_DATA_BYTE_ENV_3_HOLD_MODE_SUSTAIN,            /* sustain time (5 bits), hold mode (2 bits) */
  PATCH_DATA_BYTE_ENV_3_HOLD_LEVEL_RELEASE,           /* release time (4 bits), hold level (4 bits) */
  PATCH_DATA_BYTE_ENV_3_MAX_LEVEL,                    /* max level (7 bits) */
  /* envelope 4 (5 bytes) */
  PATCH_DATA_BYTE_ENV_4_ATTACK,                       /* attack time (5 bits) */
  PATCH_DATA_BYTE_ENV_4_DECAY,                        /* decay time (5 bits) */
  PATCH_DATA_BYTE_ENV_4_HOLD_MODE_SUSTAIN,            /* sustain time (5 bits), hold mode (2 bits) */
  PATCH_DATA_BYTE_ENV_4_HOLD_LEVEL_RELEASE,           /* release time (4 bits), hold level (4 bits) */
  PATCH_DATA_BYTE_ENV_4_MAX_LEVEL,                    /* max level (7 bits) */
  /* keyscaling (2 bytes) */
  PATCH_DATA_BYTE_RATE_KEYSCALING,                    /* rate keyscaling (8 bits total, 2 bits per envelope) */
  PATCH_DATA_BYTE_LEVEL_KEYSCALING,                   /* level keyscaling (8 bits total, 2 bits per envelope) */
  /* vibrato, tremolo (5 bytes) */
  PATCH_DATA_BYTE_VIBRATO_TREMOLO_SPEED,              /* vibrato speed (4 bits), tremolo speed (4 bits) */
  PATCH_DATA_BYTE_VIBRATO_WAVEFORM_DELAY,             /* waveform (2 bits), delay (6 bits) */
  PATCH_DATA_BYTE_VIBRATO_SYNC_DEPTH,                 /* sync (1 bit), depth (7 bits) */
  PATCH_DATA_BYTE_TREMOLO_WAVEFORM_DELAY,             /* waveform (2 bits), delay (6 bits) */
  PATCH_DATA_BYTE_TREMOLO_SYNC_DEPTH,                 /* sync (1 bit), depth (7 bits) */
  /* sensitivities, filters, midi controller routing (3 bytes) */
  PATCH_DATA_BYTE_TREMOLO_VIBRATO_SENS_MOD_WHEEL,     /* tremolo sens (2 bits), vibrato sens (3 bits), mod wheel routing (3 bits) */
  PATCH_DATA_BYTE_HIGHPASS_BOOST_SENS_AFTERTOUCH,     /* highpass cutoff (2 bits), boost sens (3 bits), aftertouch routing (3 bits) */
  PATCH_DATA_BYTE_LOWPASS_VELOCITY_SENS_EXP_PEDAL,    /* lowpass cutoff (2 bits), velocity sens (3 bits), exp pedal routing (3 bits) */
  /* lfo, boost, velocity routing (2 bytes) */
  PATCH_DATA_BYTE_VIBRATO_TREMOLO_ROUTING,            /* vibrato routing (4 bits), velocity routing (4 bits) */
  PATCH_DATA_BYTE_BOOST_VELOCITY_ROUTING,             /* tremolo routing (4 bits), boost routing (4 bits) */
  /* pitch envelope (5 bytes) */
  PATCH_DATA_BYTE_PITCH_ENV_ATTACK,                   /* attack time (5 bits) */
  PATCH_DATA_BYTE_PITCH_ENV_DECAY,                    /* decay time (5 bits) */
  PATCH_DATA_BYTE_PITCH_ENV_RELEASE,                  /* release time (4 bits) */
  PATCH_DATA_BYTE_PITCH_ENV_MAX,                      /* max level (7 bits) */
  PATCH_DATA_BYTE_PITCH_ENV_FINALE,                   /* finale level (7 bits) */
  /* pitch wheel, arpeggio, portamento (3 bytes) */
  PATCH_DATA_BYTE_PITCH_WHEEL,                        /* pitch wheel mode (1 bit), range (4 bits) */
  PATCH_DATA_BYTE_ARPEGGIO,                           /* arpeggio pattern (2 bits), octaves (2 bits), speed (4 bits) */
  PATCH_DATA_BYTE_PORTAMENTO,                         /* arpeggio mode (1 bit), portamento mode (1 bit), follow/legato (2 bits), speed (4 bits) */
  PATCH_DATA_NUM_BYTES
};

/* 12 bytes */
#define CART_NAME_NUM_BYTES   (CART_NAME_SIZE - 1)

/* 12 bytes */
#define PATCH_NAME_NUM_BYTES  (PATCH_NAME_SIZE - 1)

/* should be 12 + 50 = 62 bytes */
#define DATAFILE_BYTES_PER_PATCH  ( PATCH_NAME_NUM_BYTES +                     \
                                    PATCH_DATA_NUM_BYTES)

/* should be 12 + (16 * 62) = 1004 bytes */
#define DATAFILE_BYTES_PER_CART   ( CART_NAME_NUM_BYTES +                      \
                                    (DATAFILE_BYTES_PER_PATCH * BANK_PATCHES_PER_CART))

#define DATAFILE_CART_NAME_INDEX 0

#define DATAFILE_COMPUTE_PATCH_NAME_INDEX(patch_index)                         \
  ( CART_NAME_NUM_BYTES +                                                      \
    (patch_index * DATAFILE_BYTES_PER_PATCH))

#define DATAFILE_COMPUTE_PATCH_DATA_INDEX(patch_index, byte)                   \
  ( DATAFILE_COMPUTE_PATCH_NAME_INDEX(patch_index) +                           \
    PATCH_NAME_NUM_BYTES +                                                     \
    PATCH_DATA_BYTE_##byte)

/*******************************************************************************
** datafile_cart_load()
*******************************************************************************/
short int datafile_cart_load(int cart_index, char* filename)
{
  int m;

  FILE* fp;

  char signature[17];
  char cart_data[DATAFILE_BYTES_PER_CART];

  char current_byte;

  cart* c;
  patch* p;

  /* make sure cart index is valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(cart_index))
    return 1;

  /* make sure filename is valid */
  if (filename == NULL)
    return 1;

  /* reset signature */
  for (m = 0; m < 17; m++)
    signature[m] = '\0';

  /* open cart file */
  fp = fopen(filename, "rb");

  /* if file did not open, return */
  if (fp == NULL)
    return 0;

  /* read signature */
  if (fread(signature, 1, 16, fp) < 16)
  {
    fclose(fp);
    return 1;
  }

  if ((strncmp(&signature[0],  "NSKM", 4)) || 
      (strncmp(&signature[4],  "GERS", 4)) || 
      (strncmp(&signature[8],  "CART", 4)) || 
      (strncmp(&signature[12], "v1.0", 4)))
  {
    fclose(fp);
    return 1;
  }

  /* read cart data */
  if (fread(cart_data, 1, DATAFILE_BYTES_PER_CART, fp) == 0)
  {
    fclose(fp);
    return 1;
  }

  /* close cart file */
  fclose(fp);

  /* reset cart */
  cart_reset_cart(cart_index);

  /* obtain cart pointer */
  c = &G_cart_bank[cart_index];

  /* load cart name */
  memcpy(&c->name[0], &cart_data[DATAFILE_CART_NAME_INDEX], CART_NAME_NUM_BYTES);

  /* load cart data */
  for (m = 0; m < BANK_PATCHES_PER_CART; m++)
  {
    /* obtain patch pointer */
    p = &(c->patches[m]);

    /* load patch name */
    memcpy(&p->name[0], &cart_data[DATAFILE_COMPUTE_PATCH_NAME_INDEX(m)], PATCH_NAME_NUM_BYTES);

    /* load patch data */

    /* osc waveform, feedback, detune */
    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, OSC_1_WAVEFORM_FEEDBACK_DETUNE)];

    p->values[PATCH_PARAM_OSC_1_WAVEFORM] = (current_byte >> 6) & 0x03;
    p->values[PATCH_PARAM_OSC_1_FEEDBACK] = (current_byte >> 3) & 0x07;
    p->values[PATCH_PARAM_OSC_1_DETUNE] = current_byte & 0x07;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, OSC_2_WAVEFORM_FEEDBACK_DETUNE)];

    p->values[PATCH_PARAM_OSC_2_WAVEFORM] = (current_byte >> 6) & 0x03;
    p->values[PATCH_PARAM_OSC_2_FEEDBACK] = (current_byte >> 3) & 0x07;
    p->values[PATCH_PARAM_OSC_2_DETUNE] = current_byte & 0x07;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, OSC_3_WAVEFORM_FEEDBACK_DETUNE)];

    p->values[PATCH_PARAM_OSC_3_WAVEFORM] = (current_byte >> 6) & 0x03;
    p->values[PATCH_PARAM_OSC_3_FEEDBACK] = (current_byte >> 3) & 0x07;
    p->values[PATCH_PARAM_OSC_3_DETUNE] = current_byte & 0x07;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, OSC_4_WAVEFORM_FEEDBACK_DETUNE)];

    p->values[PATCH_PARAM_OSC_4_WAVEFORM] = (current_byte >> 6) & 0x03;
    p->values[PATCH_PARAM_OSC_4_FEEDBACK] = (current_byte >> 3) & 0x07;
    p->values[PATCH_PARAM_OSC_4_DETUNE] = current_byte & 0x07;

    /* osc frequency mode, multiple, divisor */
    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, OSC_1_FREQ_MODE_MULTIPLE_DIVISOR)];

    p->values[PATCH_PARAM_OSC_1_FREQ_MODE] = (current_byte >> 7) & 0x01;
    p->values[PATCH_PARAM_OSC_1_MULTIPLE] = (current_byte >> 3) & 0x0F;
    p->values[PATCH_PARAM_OSC_1_DIVISOR] = current_byte & 0x07;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, OSC_2_FREQ_MODE_MULTIPLE_DIVISOR)];

    p->values[PATCH_PARAM_OSC_2_FREQ_MODE] = (current_byte >> 7) & 0x01;
    p->values[PATCH_PARAM_OSC_2_MULTIPLE] = (current_byte >> 3) & 0x0F;
    p->values[PATCH_PARAM_OSC_2_DIVISOR] = current_byte & 0x07;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, OSC_3_FREQ_MODE_MULTIPLE_DIVISOR)];

    p->values[PATCH_PARAM_OSC_3_FREQ_MODE] = (current_byte >> 7) & 0x01;
    p->values[PATCH_PARAM_OSC_3_MULTIPLE] = (current_byte >> 3) & 0x0F;
    p->values[PATCH_PARAM_OSC_3_DIVISOR] = current_byte & 0x07;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, OSC_4_FREQ_MODE_MULTIPLE_DIVISOR)];

    p->values[PATCH_PARAM_OSC_4_FREQ_MODE] = (current_byte >> 7) & 0x01;
    p->values[PATCH_PARAM_OSC_4_MULTIPLE] = (current_byte >> 3) & 0x0F;
    p->values[PATCH_PARAM_OSC_4_DIVISOR] = current_byte & 0x07;

    /* osc phase shift */
    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, OSC_PHI)];

    p->values[PATCH_PARAM_OSC_1_PHI] = (current_byte >> 6) & 0x03;
    p->values[PATCH_PARAM_OSC_2_PHI] = (current_byte >> 4) & 0x03;
    p->values[PATCH_PARAM_OSC_3_PHI] = (current_byte >> 2) & 0x03;
    p->values[PATCH_PARAM_OSC_4_PHI] = current_byte & 0x03;

    /* legacy keyscaling, noise enable, algorithm */
    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, OSC_SYNC_LEGACY_NOISE_ALGORITHM)];

    p->values[PATCH_PARAM_OSC_SYNC] = (current_byte >> 5) & 0x01;
    p->values[PATCH_PARAM_LEGACY_KEYSCALE] = (current_byte >> 4) & 0x01;
    p->values[PATCH_PARAM_NOISE_ENABLE] = (current_byte >> 3) & 0x01;
    p->values[PATCH_PARAM_ALGORITHM] = current_byte & 0x07;

    /* envelope 1 */
    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_1_ATTACK)];

    p->values[PATCH_PARAM_ENV_1_ATTACK] = current_byte & 0x1F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_1_DECAY)];

    p->values[PATCH_PARAM_ENV_1_DECAY] = current_byte & 0x1F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_1_HOLD_MODE_SUSTAIN)];

    p->values[PATCH_PARAM_ENV_1_HOLD_MODE] = (current_byte >> 5) & 0x03;
    p->values[PATCH_PARAM_ENV_1_SUSTAIN] = current_byte & 0x1F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_1_HOLD_LEVEL_RELEASE)];

    p->values[PATCH_PARAM_ENV_1_HOLD_LEVEL] = (current_byte >> 4) & 0x0F;
    p->values[PATCH_PARAM_ENV_1_RELEASE] = current_byte & 0x0F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_1_MAX_LEVEL)];

    p->values[PATCH_PARAM_ENV_1_MAX_LEVEL] = current_byte & 0x7F;

    /* envelope 2 */
    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_2_ATTACK)];

    p->values[PATCH_PARAM_ENV_2_ATTACK] = current_byte & 0x1F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_2_DECAY)];

    p->values[PATCH_PARAM_ENV_2_DECAY] = current_byte & 0x1F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_2_HOLD_MODE_SUSTAIN)];

    p->values[PATCH_PARAM_ENV_2_HOLD_MODE] = (current_byte >> 5) & 0x03;
    p->values[PATCH_PARAM_ENV_2_SUSTAIN] = current_byte & 0x1F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_2_HOLD_LEVEL_RELEASE)];

    p->values[PATCH_PARAM_ENV_2_HOLD_LEVEL] = (current_byte >> 4) & 0x0F;
    p->values[PATCH_PARAM_ENV_2_RELEASE] = current_byte & 0x0F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_2_MAX_LEVEL)];

    p->values[PATCH_PARAM_ENV_2_MAX_LEVEL] = current_byte & 0x7F;

    /* envelope 3 */
    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_3_ATTACK)];

    p->values[PATCH_PARAM_ENV_3_ATTACK] = current_byte & 0x1F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_3_DECAY)];

    p->values[PATCH_PARAM_ENV_3_DECAY] = current_byte & 0x1F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_3_HOLD_MODE_SUSTAIN)];

    p->values[PATCH_PARAM_ENV_3_HOLD_MODE] = (current_byte >> 5) & 0x03;
    p->values[PATCH_PARAM_ENV_3_SUSTAIN] = current_byte & 0x1F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_3_HOLD_LEVEL_RELEASE)];

    p->values[PATCH_PARAM_ENV_3_HOLD_LEVEL] = (current_byte >> 4) & 0x0F;
    p->values[PATCH_PARAM_ENV_3_RELEASE] = current_byte & 0x0F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_3_MAX_LEVEL)];

    p->values[PATCH_PARAM_ENV_3_MAX_LEVEL] = current_byte & 0x7F;

    /* envelope 4 */
    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_4_ATTACK)];

    p->values[PATCH_PARAM_ENV_4_ATTACK] = current_byte & 0x1F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_4_DECAY)];

    p->values[PATCH_PARAM_ENV_4_DECAY] = current_byte & 0x1F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_4_HOLD_MODE_SUSTAIN)];

    p->values[PATCH_PARAM_ENV_4_HOLD_MODE] = (current_byte >> 5) & 0x03;
    p->values[PATCH_PARAM_ENV_4_SUSTAIN] = current_byte & 0x1F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_4_HOLD_LEVEL_RELEASE)];

    p->values[PATCH_PARAM_ENV_4_HOLD_LEVEL] = (current_byte >> 4) & 0x0F;
    p->values[PATCH_PARAM_ENV_4_RELEASE] = current_byte & 0x0F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_4_MAX_LEVEL)];

    p->values[PATCH_PARAM_ENV_4_MAX_LEVEL] = current_byte & 0x7F;

    /* keyscaling */
    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, RATE_KEYSCALING)];

    p->values[PATCH_PARAM_ENV_1_RATE_KEYSCALING] = (current_byte >> 6) & 0x03;
    p->values[PATCH_PARAM_ENV_2_RATE_KEYSCALING] = (current_byte >> 4) & 0x03;
    p->values[PATCH_PARAM_ENV_3_RATE_KEYSCALING] = (current_byte >> 2) & 0x03;
    p->values[PATCH_PARAM_ENV_4_RATE_KEYSCALING] = current_byte & 0x03;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, LEVEL_KEYSCALING)];

    p->values[PATCH_PARAM_ENV_1_LEVEL_KEYSCALING] = (current_byte >> 6) & 0x03;
    p->values[PATCH_PARAM_ENV_2_LEVEL_KEYSCALING] = (current_byte >> 4) & 0x03;
    p->values[PATCH_PARAM_ENV_3_LEVEL_KEYSCALING] = (current_byte >> 2) & 0x03;
    p->values[PATCH_PARAM_ENV_4_LEVEL_KEYSCALING] = current_byte & 0x03;

    /* vibrato, tremolo */
    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, VIBRATO_TREMOLO_SPEED)];

    p->values[PATCH_PARAM_VIBRATO_SPEED] = (current_byte >> 4) & 0x0F;
    p->values[PATCH_PARAM_TREMOLO_SPEED] = current_byte & 0x0F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, VIBRATO_WAVEFORM_DELAY)];

    p->values[PATCH_PARAM_VIBRATO_WAVEFORM] = (current_byte >> 6) & 0x03;
    p->values[PATCH_PARAM_VIBRATO_DELAY] = current_byte & 0x3F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, VIBRATO_SYNC_DEPTH)];

    p->values[PATCH_PARAM_VIBRATO_SYNC] = (current_byte >> 7) & 0x01;
    p->values[PATCH_PARAM_VIBRATO_DEPTH] = current_byte & 0x7F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, TREMOLO_WAVEFORM_DELAY)];

    p->values[PATCH_PARAM_TREMOLO_WAVEFORM] = (current_byte >> 6) & 0x03;
    p->values[PATCH_PARAM_TREMOLO_DELAY] = current_byte & 0x3F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, TREMOLO_SYNC_DEPTH)];

    p->values[PATCH_PARAM_TREMOLO_SYNC] = (current_byte >> 7) & 0x01;
    p->values[PATCH_PARAM_TREMOLO_DEPTH] = current_byte & 0x7F;

    /* sensitivities, filters, midi controller routing */
    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, TREMOLO_VIBRATO_SENS_MOD_WHEEL)];

    p->values[PATCH_PARAM_TREMOLO_SENSITIVITY] = (current_byte >> 6) & 0x03;
    p->values[PATCH_PARAM_VIBRATO_SENSITIVITY] = (current_byte >> 3) & 0x07;
    p->values[PATCH_PARAM_MOD_WHEEL_ROUTING_VIBRATO] = (current_byte >> 2) & 0x01;
    p->values[PATCH_PARAM_MOD_WHEEL_ROUTING_TREMOLO] = (current_byte >> 1) & 0x01;
    p->values[PATCH_PARAM_MOD_WHEEL_ROUTING_BOOST] = current_byte & 0x01;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, HIGHPASS_BOOST_SENS_AFTERTOUCH)];

    p->values[PATCH_PARAM_HIGHPASS_CUTOFF] = (current_byte >> 6) & 0x03;
    p->values[PATCH_PARAM_BOOST_SENSITIVITY] = (current_byte >> 3) & 0x07;
    p->values[PATCH_PARAM_AFTERTOUCH_ROUTING_VIBRATO] = (current_byte >> 2) & 0x01;
    p->values[PATCH_PARAM_AFTERTOUCH_ROUTING_TREMOLO] = (current_byte >> 1) & 0x01;
    p->values[PATCH_PARAM_AFTERTOUCH_ROUTING_BOOST] = current_byte & 0x01;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, LOWPASS_VELOCITY_SENS_EXP_PEDAL)];

    p->values[PATCH_PARAM_LOWPASS_CUTOFF] = (current_byte >> 6) & 0x03;
    p->values[PATCH_PARAM_VELOCITY_SENSITIVITY] = (current_byte >> 3) & 0x07;
    p->values[PATCH_PARAM_EXP_PEDAL_ROUTING_VIBRATO] = (current_byte >> 2) & 0x01;
    p->values[PATCH_PARAM_EXP_PEDAL_ROUTING_TREMOLO] = (current_byte >> 1) & 0x01;
    p->values[PATCH_PARAM_EXP_PEDAL_ROUTING_BOOST] = current_byte & 0x01;

    /* lfo, boost, velocity routing */
    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, VIBRATO_TREMOLO_ROUTING)];

    p->values[PATCH_PARAM_VIBRATO_ROUTING_OSC_1] = (current_byte >> 7) & 0x01;
    p->values[PATCH_PARAM_VIBRATO_ROUTING_OSC_2] = (current_byte >> 6) & 0x01;
    p->values[PATCH_PARAM_VIBRATO_ROUTING_OSC_3] = (current_byte >> 5) & 0x01;
    p->values[PATCH_PARAM_VIBRATO_ROUTING_OSC_4] = (current_byte >> 4) & 0x01;
    p->values[PATCH_PARAM_TREMOLO_ROUTING_ENV_1] = (current_byte >> 3) & 0x01;
    p->values[PATCH_PARAM_TREMOLO_ROUTING_ENV_2] = (current_byte >> 2) & 0x01;
    p->values[PATCH_PARAM_TREMOLO_ROUTING_ENV_3] = (current_byte >> 1) & 0x01;
    p->values[PATCH_PARAM_TREMOLO_ROUTING_ENV_4] = current_byte & 0x01;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, BOOST_VELOCITY_ROUTING)];

    p->values[PATCH_PARAM_BOOST_ROUTING_ENV_1] = (current_byte >> 7) & 0x01;
    p->values[PATCH_PARAM_BOOST_ROUTING_ENV_2] = (current_byte >> 6) & 0x01;
    p->values[PATCH_PARAM_BOOST_ROUTING_ENV_3] = (current_byte >> 5) & 0x01;
    p->values[PATCH_PARAM_BOOST_ROUTING_ENV_4] = (current_byte >> 4) & 0x01;
    p->values[PATCH_PARAM_VELOCITY_ROUTING_ENV_1] = (current_byte >> 3) & 0x01;
    p->values[PATCH_PARAM_VELOCITY_ROUTING_ENV_2] = (current_byte >> 2) & 0x01;
    p->values[PATCH_PARAM_VELOCITY_ROUTING_ENV_3] = (current_byte >> 1) & 0x01;
    p->values[PATCH_PARAM_VELOCITY_ROUTING_ENV_4] = current_byte & 0x01;

    /* pitch envelope */
    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, PITCH_ENV_ATTACK)];

    p->values[PATCH_PARAM_PITCH_ENV_ATTACK] = current_byte & 0x1F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, PITCH_ENV_DECAY)];

    p->values[PATCH_PARAM_PITCH_ENV_DECAY] = current_byte & 0x1F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, PITCH_ENV_RELEASE)];

    p->values[PATCH_PARAM_PITCH_ENV_RELEASE] = current_byte & 0x0F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, PITCH_ENV_MAX)];

    p->values[PATCH_PARAM_PITCH_ENV_MAX] = current_byte & 0x7F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, PITCH_ENV_FINALE)];

    p->values[PATCH_PARAM_PITCH_ENV_FINALE] = current_byte & 0x7F;

    /* pitch wheel, arpeggio, portamento */
    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, PITCH_WHEEL)];

    p->values[PATCH_PARAM_PITCH_WHEEL_MODE] = (current_byte >> 4) & 0x01;
    p->values[PATCH_PARAM_PITCH_WHEEL_RANGE] = current_byte & 0x0F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ARPEGGIO)];

    p->values[PATCH_PARAM_ARPEGGIO_PATTERN] = (current_byte >> 6) & 0x03;
    p->values[PATCH_PARAM_ARPEGGIO_OCTAVE] = (current_byte >> 4) & 0x03;
    p->values[PATCH_PARAM_ARPEGGIO_SPEED] = current_byte & 0x0F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, PORTAMENTO)];

    p->values[PATCH_PARAM_ARPEGGIO_MODE] = (current_byte >> 7) & 0x01;
    p->values[PATCH_PARAM_PORTAMENTO_MODE] = (current_byte >> 6) & 0x01;
    p->values[PATCH_PARAM_PORTAMENTO_LEGATO] = (current_byte >> 4) & 0x03;
    p->values[PATCH_PARAM_PORTAMENTO_SPEED] = current_byte & 0x0F;
  }

  /* validate the cart */
  cart_validate_cart(cart_index);

  return 0;
}

/*******************************************************************************
** datafile_cart_save()
*******************************************************************************/
short int datafile_cart_save(int cart_index, char* filename)
{
  int m;

  FILE* fp;

  char signature[17];
  char cart_data[DATAFILE_BYTES_PER_CART];

  char current_byte;

  cart* c;
  patch* p;

  /* make sure cart index is valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(cart_index))
    return 1;

  /* make sure filename is valid */
  if (filename == NULL)
    return 0;

  /* reset signature */
  for (m = 0; m < 17; m++)
    signature[m] = '\0';

  /* initialize cart data */
  for (m = 0; m < DATAFILE_BYTES_PER_CART; m++)
    cart_data[m] = 0;

  /* validate the cart */
  cart_validate_cart(cart_index);

  /* obtain cart pointer */
  c = &G_cart_bank[cart_index];

  /* save cart name */
  memcpy(&cart_data[DATAFILE_CART_NAME_INDEX], &c->name[0], CART_NAME_NUM_BYTES);

  /* save cart data */
  for (m = 0; m < BANK_PATCHES_PER_CART; m++)
  {
    /* obtain patch pointer */
    p = &(c->patches[m]);

    /* save patch name */
    memcpy(&cart_data[DATAFILE_COMPUTE_PATCH_NAME_INDEX(m)], &p->name[0], PATCH_NAME_NUM_BYTES);

    /* osc waveform, feedback, detune */
    current_byte =  (p->values[PATCH_PARAM_OSC_1_WAVEFORM] & 0x03) << 6;
    current_byte |= (p->values[PATCH_PARAM_OSC_1_FEEDBACK] & 0x07) << 3;
    current_byte |= p->values[PATCH_PARAM_OSC_1_DETUNE] & 0x07;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, OSC_1_WAVEFORM_FEEDBACK_DETUNE)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_OSC_2_WAVEFORM] & 0x03) << 6;
    current_byte |= (p->values[PATCH_PARAM_OSC_2_FEEDBACK] & 0x07) << 3;
    current_byte |= p->values[PATCH_PARAM_OSC_2_DETUNE] & 0x07;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, OSC_2_WAVEFORM_FEEDBACK_DETUNE)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_OSC_3_WAVEFORM] & 0x03) << 6;
    current_byte |= (p->values[PATCH_PARAM_OSC_3_FEEDBACK] & 0x07) << 3;
    current_byte |= p->values[PATCH_PARAM_OSC_3_DETUNE] & 0x07;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, OSC_3_WAVEFORM_FEEDBACK_DETUNE)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_OSC_4_WAVEFORM] & 0x03) << 6;
    current_byte |= (p->values[PATCH_PARAM_OSC_4_FEEDBACK] & 0x07) << 3;
    current_byte |= p->values[PATCH_PARAM_OSC_4_DETUNE] & 0x07;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, OSC_4_WAVEFORM_FEEDBACK_DETUNE)] = current_byte;

    /* osc frequency mode, multiple, divisor */
    current_byte =  (p->values[PATCH_PARAM_OSC_1_FREQ_MODE] & 0x01) << 7;
    current_byte |= (p->values[PATCH_PARAM_OSC_1_MULTIPLE] & 0x0F) << 3;
    current_byte |= p->values[PATCH_PARAM_OSC_1_DIVISOR] & 0x07;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, OSC_1_FREQ_MODE_MULTIPLE_DIVISOR)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_OSC_2_FREQ_MODE] & 0x01) << 7;
    current_byte |= (p->values[PATCH_PARAM_OSC_2_MULTIPLE] & 0x0F) << 3;
    current_byte |= p->values[PATCH_PARAM_OSC_2_DIVISOR] & 0x07;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, OSC_2_FREQ_MODE_MULTIPLE_DIVISOR)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_OSC_3_FREQ_MODE] & 0x01) << 7;
    current_byte |= (p->values[PATCH_PARAM_OSC_3_MULTIPLE] & 0x0F) << 3;
    current_byte |= p->values[PATCH_PARAM_OSC_3_DIVISOR] & 0x07;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, OSC_3_FREQ_MODE_MULTIPLE_DIVISOR)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_OSC_4_FREQ_MODE] & 0x01) << 7;
    current_byte |= (p->values[PATCH_PARAM_OSC_4_MULTIPLE] & 0x0F) << 3;
    current_byte |= p->values[PATCH_PARAM_OSC_4_DIVISOR] & 0x07;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, OSC_4_FREQ_MODE_MULTIPLE_DIVISOR)] = current_byte;

    /* osc phase shift */
    current_byte =  (p->values[PATCH_PARAM_OSC_1_PHI] & 0x03) << 6;
    current_byte |= (p->values[PATCH_PARAM_OSC_2_PHI] & 0x03) << 4;
    current_byte |= (p->values[PATCH_PARAM_OSC_3_PHI] & 0x03) << 2;
    current_byte |= p->values[PATCH_PARAM_OSC_4_PHI] & 0x03;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, OSC_PHI)] = current_byte;

    /* legacy keyscaling, noise enable, algorithm */
    current_byte =  (p->values[PATCH_PARAM_OSC_SYNC] & 0x01) << 5;
    current_byte |= (p->values[PATCH_PARAM_LEGACY_KEYSCALE] & 0x01) << 4;
    current_byte |= (p->values[PATCH_PARAM_NOISE_ENABLE] & 0x01) << 3;
    current_byte |= p->values[PATCH_PARAM_ALGORITHM] & 0x07;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, OSC_SYNC_LEGACY_NOISE_ALGORITHM)] = current_byte;

    /* envelope 1 */
    current_byte = p->values[PATCH_PARAM_ENV_1_ATTACK] & 0x1F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_1_ATTACK)] = current_byte;

    current_byte = p->values[PATCH_PARAM_ENV_1_DECAY] & 0x1F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_1_DECAY)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_ENV_1_HOLD_MODE] & 0x03) << 5;
    current_byte |= p->values[PATCH_PARAM_ENV_1_SUSTAIN] & 0x1F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_1_HOLD_MODE_SUSTAIN)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_ENV_1_HOLD_LEVEL] & 0x0F) << 4;
    current_byte |= p->values[PATCH_PARAM_ENV_1_RELEASE] & 0x0F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_1_HOLD_LEVEL_RELEASE)] = current_byte;

    current_byte = p->values[PATCH_PARAM_ENV_1_MAX_LEVEL] & 0x7F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_1_MAX_LEVEL)] = current_byte;

    /* envelope 2 */
    current_byte = p->values[PATCH_PARAM_ENV_2_ATTACK] & 0x1F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_2_ATTACK)] = current_byte;

    current_byte = p->values[PATCH_PARAM_ENV_2_DECAY] & 0x1F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_2_DECAY)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_ENV_2_HOLD_MODE] & 0x03) << 5;
    current_byte |= p->values[PATCH_PARAM_ENV_2_SUSTAIN] & 0x1F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_2_HOLD_MODE_SUSTAIN)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_ENV_2_HOLD_LEVEL] & 0x0F) << 4;
    current_byte |= p->values[PATCH_PARAM_ENV_2_RELEASE] & 0x0F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_2_HOLD_LEVEL_RELEASE)] = current_byte;

    current_byte = p->values[PATCH_PARAM_ENV_2_MAX_LEVEL] & 0x7F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_2_MAX_LEVEL)] = current_byte;

    /* envelope 3 */
    current_byte = p->values[PATCH_PARAM_ENV_3_ATTACK] & 0x1F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_3_ATTACK)] = current_byte;

    current_byte = p->values[PATCH_PARAM_ENV_3_DECAY] & 0x1F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_3_DECAY)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_ENV_3_HOLD_MODE] & 0x03) << 5;
    current_byte |= p->values[PATCH_PARAM_ENV_3_SUSTAIN] & 0x1F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_3_HOLD_MODE_SUSTAIN)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_ENV_3_HOLD_LEVEL] & 0x0F) << 4;
    current_byte |= p->values[PATCH_PARAM_ENV_3_RELEASE] & 0x0F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_3_HOLD_LEVEL_RELEASE)] = current_byte;

    current_byte = p->values[PATCH_PARAM_ENV_3_MAX_LEVEL] & 0x7F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_3_MAX_LEVEL)] = current_byte;

    /* envelope 4 */
    current_byte = p->values[PATCH_PARAM_ENV_4_ATTACK] & 0x1F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_4_ATTACK)] = current_byte;

    current_byte = p->values[PATCH_PARAM_ENV_4_DECAY] & 0x1F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_4_DECAY)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_ENV_4_HOLD_MODE] & 0x03) << 5;
    current_byte |= p->values[PATCH_PARAM_ENV_4_SUSTAIN] & 0x1F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_4_HOLD_MODE_SUSTAIN)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_ENV_4_HOLD_LEVEL] & 0x0F) << 4;
    current_byte |= p->values[PATCH_PARAM_ENV_4_RELEASE] & 0x0F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_4_HOLD_LEVEL_RELEASE)] = current_byte;

    current_byte = p->values[PATCH_PARAM_ENV_4_MAX_LEVEL] & 0x7F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_4_MAX_LEVEL)] = current_byte;

    /* keyscaling */
    current_byte =  (p->values[PATCH_PARAM_ENV_1_RATE_KEYSCALING] & 0x03) << 6;
    current_byte |= (p->values[PATCH_PARAM_ENV_2_RATE_KEYSCALING] & 0x03) << 4;
    current_byte |= (p->values[PATCH_PARAM_ENV_3_RATE_KEYSCALING] & 0x03) << 2;
    current_byte |= p->values[PATCH_PARAM_ENV_4_RATE_KEYSCALING] & 0x03;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, RATE_KEYSCALING)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_ENV_1_LEVEL_KEYSCALING] & 0x03) << 6;
    current_byte |= (p->values[PATCH_PARAM_ENV_2_LEVEL_KEYSCALING] & 0x03) << 4;
    current_byte |= (p->values[PATCH_PARAM_ENV_3_LEVEL_KEYSCALING] & 0x03) << 2;
    current_byte |= p->values[PATCH_PARAM_ENV_4_LEVEL_KEYSCALING] & 0x03;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, LEVEL_KEYSCALING)] = current_byte;

    /* vibrato, tremolo */
    current_byte =  (p->values[PATCH_PARAM_VIBRATO_SPEED] & 0x0F) << 4;
    current_byte |= p->values[PATCH_PARAM_TREMOLO_SPEED] & 0x0F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, VIBRATO_TREMOLO_SPEED)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_VIBRATO_WAVEFORM] & 0x03) << 6;
    current_byte |= p->values[PATCH_PARAM_VIBRATO_DELAY] & 0x3F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, VIBRATO_WAVEFORM_DELAY)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_VIBRATO_SYNC] & 0x01) << 7;
    current_byte |= p->values[PATCH_PARAM_VIBRATO_DEPTH] & 0x7F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, VIBRATO_SYNC_DEPTH)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_TREMOLO_WAVEFORM] & 0x03) << 6;
    current_byte |= p->values[PATCH_PARAM_TREMOLO_DELAY] & 0x3F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, TREMOLO_WAVEFORM_DELAY)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_TREMOLO_SYNC] & 0x01) << 7;
    current_byte |= p->values[PATCH_PARAM_TREMOLO_DEPTH] & 0x7F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, TREMOLO_SYNC_DEPTH)] = current_byte;

    /* sensitivities, filters, midi controller routing */
    current_byte =  (p->values[PATCH_PARAM_TREMOLO_SENSITIVITY] & 0x03) << 6;
    current_byte |= (p->values[PATCH_PARAM_VIBRATO_SENSITIVITY] & 0x07) << 3;
    current_byte |= (p->values[PATCH_PARAM_MOD_WHEEL_ROUTING_VIBRATO] & 0x01) << 2;
    current_byte |= (p->values[PATCH_PARAM_MOD_WHEEL_ROUTING_TREMOLO] & 0x01) << 1;
    current_byte |= p->values[PATCH_PARAM_MOD_WHEEL_ROUTING_BOOST] & 0x01;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, TREMOLO_VIBRATO_SENS_MOD_WHEEL)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_HIGHPASS_CUTOFF] & 0x03) << 6;
    current_byte |= (p->values[PATCH_PARAM_BOOST_SENSITIVITY] & 0x07) << 3;
    current_byte |= (p->values[PATCH_PARAM_AFTERTOUCH_ROUTING_VIBRATO] & 0x01) << 2;
    current_byte |= (p->values[PATCH_PARAM_AFTERTOUCH_ROUTING_TREMOLO] & 0x01) << 1;
    current_byte |= p->values[PATCH_PARAM_AFTERTOUCH_ROUTING_BOOST] & 0x01;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, HIGHPASS_BOOST_SENS_AFTERTOUCH)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_LOWPASS_CUTOFF] & 0x03) << 6;
    current_byte |= (p->values[PATCH_PARAM_VELOCITY_SENSITIVITY] & 0x07) << 3;
    current_byte |= (p->values[PATCH_PARAM_EXP_PEDAL_ROUTING_VIBRATO] & 0x01) << 2;
    current_byte |= (p->values[PATCH_PARAM_EXP_PEDAL_ROUTING_TREMOLO] & 0x01) << 1;
    current_byte |= p->values[PATCH_PARAM_EXP_PEDAL_ROUTING_BOOST] & 0x01;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, LOWPASS_VELOCITY_SENS_EXP_PEDAL)] = current_byte;

    /* lfo, boost, velocity routing */
    current_byte =  (p->values[PATCH_PARAM_VIBRATO_ROUTING_OSC_1] & 0x01) << 7;
    current_byte |= (p->values[PATCH_PARAM_VIBRATO_ROUTING_OSC_2] & 0x01) << 6;
    current_byte |= (p->values[PATCH_PARAM_VIBRATO_ROUTING_OSC_3] & 0x01) << 5;
    current_byte |= (p->values[PATCH_PARAM_VIBRATO_ROUTING_OSC_4] & 0x01) << 4;
    current_byte |= (p->values[PATCH_PARAM_TREMOLO_ROUTING_ENV_1] & 0x01) << 3;
    current_byte |= (p->values[PATCH_PARAM_TREMOLO_ROUTING_ENV_2] & 0x01) << 2;
    current_byte |= (p->values[PATCH_PARAM_TREMOLO_ROUTING_ENV_3] & 0x01) << 1;
    current_byte |= p->values[PATCH_PARAM_TREMOLO_ROUTING_ENV_4] & 0x01;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, VIBRATO_TREMOLO_ROUTING)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_BOOST_ROUTING_ENV_1] & 0x01) << 7;
    current_byte |= (p->values[PATCH_PARAM_BOOST_ROUTING_ENV_2] & 0x01) << 6;
    current_byte |= (p->values[PATCH_PARAM_BOOST_ROUTING_ENV_3] & 0x01) << 5;
    current_byte |= (p->values[PATCH_PARAM_BOOST_ROUTING_ENV_4] & 0x01) << 4;
    current_byte |= (p->values[PATCH_PARAM_VELOCITY_ROUTING_ENV_1] & 0x01) << 3;
    current_byte |= (p->values[PATCH_PARAM_VELOCITY_ROUTING_ENV_2] & 0x01) << 2;
    current_byte |= (p->values[PATCH_PARAM_VELOCITY_ROUTING_ENV_3] & 0x01) << 1;
    current_byte |= p->values[PATCH_PARAM_VELOCITY_ROUTING_ENV_4] & 0x01;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, BOOST_VELOCITY_ROUTING)] = current_byte;

    /* pitch envelope */
    current_byte = p->values[PATCH_PARAM_PITCH_ENV_ATTACK] & 0x1F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, PITCH_ENV_ATTACK)] = current_byte;

    current_byte = p->values[PATCH_PARAM_PITCH_ENV_DECAY] & 0x1F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, PITCH_ENV_DECAY)] = current_byte;

    current_byte = p->values[PATCH_PARAM_PITCH_ENV_RELEASE] & 0x0F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, PITCH_ENV_RELEASE)] = current_byte;

    current_byte = p->values[PATCH_PARAM_PITCH_ENV_MAX] & 0x7F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, PITCH_ENV_MAX)] = current_byte;

    current_byte = p->values[PATCH_PARAM_PITCH_ENV_FINALE] & 0x7F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, PITCH_ENV_FINALE)] = current_byte;

    /* pitch wheel, arpeggio, portamento */
    current_byte =  (p->values[PATCH_PARAM_PITCH_WHEEL_MODE] & 0x01) << 4;
    current_byte |= p->values[PATCH_PARAM_PITCH_WHEEL_RANGE] & 0x0F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, PITCH_WHEEL)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_ARPEGGIO_PATTERN] & 0x03) << 6;
    current_byte |= (p->values[PATCH_PARAM_ARPEGGIO_OCTAVE] & 0x03) << 4;
    current_byte |= p->values[PATCH_PARAM_ARPEGGIO_SPEED] & 0x0F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ARPEGGIO)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_ARPEGGIO_MODE] & 0x01) << 7;
    current_byte |= (p->values[PATCH_PARAM_PORTAMENTO_MODE] & 0x01) << 6;
    current_byte |= (p->values[PATCH_PARAM_PORTAMENTO_LEGATO] & 0x03) << 4;
    current_byte |= p->values[PATCH_PARAM_PORTAMENTO_SPEED] & 0x0F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, PORTAMENTO)] = current_byte;
  }

  /* open cart file */
  fp = fopen(filename, "wb");

  /* if file did not open, return */
  if (fp == NULL)
    return 0;

  /* write signature */
  strcpy(signature, "NSKM");
  strcat(signature, "GERS");
  strcat(signature, "CART");
  strcat(signature, "v1.0");

  fwrite(signature, 1, 16, fp);

  /* write cart data */
  if (fwrite(cart_data, 1, DATAFILE_BYTES_PER_CART, fp) == 0)
  {
    fclose(fp);
    return 1;
  }

  /* close cart file */
  fclose(fp);

  return 0;
}

