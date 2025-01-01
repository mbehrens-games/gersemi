/*******************************************************************************
** datafile.c (native file format loading & saving)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bank.h"
#include "cart.h"
#include "datafile.h"

/* 36 bytes */
enum
{
  /* top line parameters (1 byte) */
  PATCH_DATA_BYTE_OSC_SYNC_ALGORITHM_FEEDBACK = 0,    /* osc sync (1 bit), algorithm (3 bits), feedback (3 bits) */
  /* osc waveform (1 byte) */
  PATCH_DATA_BYTE_OSC_WAVEFORM,                       /* waveform (8 bits, 2 bits each) */
  /* filter cutoffs, osc detune (2 bytes) */
  PATCH_DATA_BYTE_HIGHPASS_OSC_1_2_DETUNE,            /* highpass (2 bits), osc 1 detune (3 bits), osc 2 detune (3 bits) */
  PATCH_DATA_BYTE_LOWPASS_OSC_3_4_DETUNE,             /* lowpass (2 bits), osc 3 detune (3 bits), osc 4 detune (3 bits) */
  /* osc frequency mode, multiple, divisor (4 bytes) */
  PATCH_DATA_BYTE_OSC_1_FREQ_MODE_MULTIPLE_DIVISOR,   /* frequency mode (1 bit), multiple/divisor or note/octave (7 bits) */
  PATCH_DATA_BYTE_OSC_2_FREQ_MODE_MULTIPLE_DIVISOR,   /* frequency mode (1 bit), multiple/divisor or note/octave (7 bits) */
  PATCH_DATA_BYTE_OSC_3_FREQ_MODE_MULTIPLE_DIVISOR,   /* frequency mode (1 bit), multiple/divisor or note/octave (7 bits) */
  PATCH_DATA_BYTE_OSC_4_FREQ_MODE_MULTIPLE_DIVISOR,   /* frequency mode (1 bit), multiple/divisor or note/octave (7 bits) */
  /* envelope 1 (5 bytes) */
  PATCH_DATA_BYTE_ENV_1_ATTACK,                       /* attack time (5 bits) */
  PATCH_DATA_BYTE_ENV_1_DECAY,                        /* decay time (5 bits) */
  PATCH_DATA_BYTE_ENV_1_SUSTAIN,                      /* sustain time (5 bits) */
  PATCH_DATA_BYTE_ENV_1_HOLD_LEVEL_RELEASE,           /* release time (4 bits), hold level (4 bits) */
  PATCH_DATA_BYTE_ENV_1_MAX_LEVEL,                    /* max level (7 bits) */
  /* envelope 2 (5 bytes) */
  PATCH_DATA_BYTE_ENV_2_ATTACK,                       /* attack time (5 bits) */
  PATCH_DATA_BYTE_ENV_2_DECAY,                        /* decay time (5 bits) */
  PATCH_DATA_BYTE_ENV_2_SUSTAIN,                      /* sustain time (5 bits) */
  PATCH_DATA_BYTE_ENV_2_HOLD_LEVEL_RELEASE,           /* release time (4 bits), hold level (4 bits) */
  PATCH_DATA_BYTE_ENV_2_MAX_LEVEL,                    /* max level (7 bits) */
  /* envelope 3 (5 bytes) */
  PATCH_DATA_BYTE_ENV_3_ATTACK,                       /* attack time (5 bits) */
  PATCH_DATA_BYTE_ENV_3_DECAY,                        /* decay time (5 bits) */
  PATCH_DATA_BYTE_ENV_3_SUSTAIN,                      /* sustain time (5 bits) */
  PATCH_DATA_BYTE_ENV_3_HOLD_LEVEL_RELEASE,           /* release time (4 bits), hold level (4 bits) */
  PATCH_DATA_BYTE_ENV_3_MAX_LEVEL,                    /* max level (7 bits) */
  /* envelope 4 (5 bytes) */
  PATCH_DATA_BYTE_ENV_4_ATTACK,                       /* attack time (5 bits) */
  PATCH_DATA_BYTE_ENV_4_DECAY,                        /* decay time (5 bits) */
  PATCH_DATA_BYTE_ENV_4_SUSTAIN,                      /* sustain time (5 bits) */
  PATCH_DATA_BYTE_ENV_4_HOLD_LEVEL_RELEASE,           /* release time (4 bits), hold level (4 bits) */
  PATCH_DATA_BYTE_ENV_4_MAX_LEVEL,                    /* max level (7 bits) */
  /* keyscaling (2 bytes) */
  PATCH_DATA_BYTE_RATE_KEYSCALING,                    /* rate keyscaling (8 bits total, 2 bits per envelope) */
  PATCH_DATA_BYTE_LEVEL_KEYSCALING,                   /* level keyscaling (8 bits total, 2 bits per envelope) */
  /* lfo (2 bytes) */
  PATCH_DATA_BYTE_LFO_WAVEFORM_DELAY,                 /* lfo waveform (2 bits), delay (6 bits) */
  PATCH_DATA_BYTE_LFO_SYNC_SPEED,                     /* lfo sync (1 bit), speed (4 bits) */
  /* sensitivities and depths (4 bytes) */
  PATCH_DATA_BYTE_VIBRATO_VELOCITY_SENSITIVITY,       /* polarity (1 bit), sensitivity (3 bits each) */
  PATCH_DATA_BYTE_TREMOLO_BOOST_SENSITIVITY,          /* mode (1 bit each), sensitivity (3 bits each) */
  PATCH_DATA_BYTE_VIBRATO_DEPTH,                      /* depth (7 bits) */
  PATCH_DATA_BYTE_TREMOLO_DEPTH,                      /* depth (7 bits) */
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
    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, OSC_SYNC_ALGORITHM_FEEDBACK)];

    p->values[PATCH_PARAM_OSC_SYNC] = (current_byte >> 7) & 0x01;
    p->values[PATCH_PARAM_ALGORITHM] = (current_byte >> 3) & 0x07;
    p->values[PATCH_PARAM_FEEDBACK] = current_byte & 0x07;

    /* osc waveform */
    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, OSC_WAVEFORM)];

    p->values[PATCH_PARAM_OSC_1_WAVEFORM] = (current_byte >> 6) & 0x03;
    p->values[PATCH_PARAM_OSC_2_WAVEFORM] = (current_byte >> 4) & 0x03;
    p->values[PATCH_PARAM_OSC_3_WAVEFORM] = (current_byte >> 2) & 0x03;
    p->values[PATCH_PARAM_OSC_4_WAVEFORM] = current_byte & 0x03;

    /* filter cutoffs, osc detune */
    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, HIGHPASS_OSC_1_2_DETUNE)];

    p->values[PATCH_PARAM_HIGHPASS_CUTOFF] = (current_byte >> 6) & 0x03;
    p->values[PATCH_PARAM_OSC_1_DETUNE] = (current_byte >> 3) & 0x07;
    p->values[PATCH_PARAM_OSC_2_DETUNE] = current_byte & 0x07;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, LOWPASS_OSC_3_4_DETUNE)];

    p->values[PATCH_PARAM_LOWPASS_CUTOFF] = (current_byte >> 6) & 0x03;
    p->values[PATCH_PARAM_OSC_3_DETUNE] = (current_byte >> 3) & 0x07;
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

    /* envelope 1 */
    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_1_ATTACK)];

    p->values[PATCH_PARAM_ENV_1_ATTACK] = current_byte & 0x1F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_1_DECAY)];

    p->values[PATCH_PARAM_ENV_1_DECAY] = current_byte & 0x1F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_1_SUSTAIN)];

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

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_2_SUSTAIN)];

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

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_3_SUSTAIN)];

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

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_4_SUSTAIN)];

    p->values[PATCH_PARAM_ENV_4_SUSTAIN] = current_byte & 0x1F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_4_HOLD_LEVEL_RELEASE)];

    p->values[PATCH_PARAM_ENV_4_HOLD_LEVEL] = (current_byte >> 4) & 0x0F;
    p->values[PATCH_PARAM_ENV_4_RELEASE] = current_byte & 0x0F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_4_MAX_LEVEL)];

    p->values[PATCH_PARAM_ENV_4_MAX_LEVEL] = current_byte & 0x7F;

    /* keyscaling */
    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, RATE_KEYSCALING)];

    p->values[PATCH_PARAM_ENV_1_RATE_KS] = (current_byte >> 6) & 0x03;
    p->values[PATCH_PARAM_ENV_2_RATE_KS] = (current_byte >> 4) & 0x03;
    p->values[PATCH_PARAM_ENV_3_RATE_KS] = (current_byte >> 2) & 0x03;
    p->values[PATCH_PARAM_ENV_4_RATE_KS] = current_byte & 0x03;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, LEVEL_KEYSCALING)];

    p->values[PATCH_PARAM_ENV_1_LEVEL_KS] = (current_byte >> 6) & 0x03;
    p->values[PATCH_PARAM_ENV_2_LEVEL_KS] = (current_byte >> 4) & 0x03;
    p->values[PATCH_PARAM_ENV_3_LEVEL_KS] = (current_byte >> 2) & 0x03;
    p->values[PATCH_PARAM_ENV_4_LEVEL_KS] = current_byte & 0x03;

    /* lfo */
    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, LFO_WAVEFORM_DELAY)];

    p->values[PATCH_PARAM_LFO_WAVEFORM] = (current_byte >> 6) & 0x03;
    p->values[PATCH_PARAM_LFO_DELAY] = current_byte & 0x3F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, LFO_SYNC_SPEED)];

    p->values[PATCH_PARAM_LFO_SYNC] = (current_byte >> 4) & 0x01;
    p->values[PATCH_PARAM_LFO_SPEED] = current_byte & 0x0F;

    /* sensitivities and depths */
    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, VIBRATO_VELOCITY_SENSITIVITY)];

    p->values[PATCH_PARAM_VIBRATO_POLARITY] = (current_byte >> 6) & 0x01;
    p->values[PATCH_PARAM_VIBRATO_SENSITIVITY] = (current_byte >> 3) & 0x07;
    p->values[PATCH_PARAM_VELOCITY_SENSITIVITY] = current_byte & 0x07;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, TREMOLO_BOOST_SENSITIVITY)];

    p->values[PATCH_PARAM_TREMOLO_MODE] = (current_byte >> 7) & 0x01;
    p->values[PATCH_PARAM_BOOST_MODE] = (current_byte >> 6) & 0x01;
    p->values[PATCH_PARAM_TREMOLO_SENSITIVITY] = (current_byte >> 3) & 0x07;
    p->values[PATCH_PARAM_BOOST_SENSITIVITY] = current_byte & 0x07;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, VIBRATO_DEPTH)];

    p->values[PATCH_PARAM_VIBRATO_DEPTH] = current_byte & 0x7F;

    current_byte = cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, TREMOLO_DEPTH)];

    p->values[PATCH_PARAM_TREMOLO_DEPTH] = current_byte & 0x7F;
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

    /* save patch data */
    current_byte =  (p->values[PATCH_PARAM_OSC_SYNC] & 0x01) << 7;
    current_byte |= (p->values[PATCH_PARAM_ALGORITHM] & 0x07) << 3;
    current_byte |= p->values[PATCH_PARAM_FEEDBACK] & 0x07;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, OSC_SYNC_ALGORITHM_FEEDBACK)] = current_byte;

    /* osc waveform */
    current_byte =  (p->values[PATCH_PARAM_OSC_1_WAVEFORM] & 0x03) << 6;
    current_byte |= (p->values[PATCH_PARAM_OSC_2_WAVEFORM] & 0x03) << 4;
    current_byte |= (p->values[PATCH_PARAM_OSC_3_WAVEFORM] & 0x03) << 2;
    current_byte |= p->values[PATCH_PARAM_OSC_4_WAVEFORM] & 0x03;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, OSC_WAVEFORM)] = current_byte;

    /* filter cutoffs, osc detune */
    current_byte =  (p->values[PATCH_PARAM_HIGHPASS_CUTOFF] & 0x03) << 6;
    current_byte |= (p->values[PATCH_PARAM_OSC_1_DETUNE] & 0x07) << 3;
    current_byte |= p->values[PATCH_PARAM_OSC_2_DETUNE] & 0x07;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, HIGHPASS_OSC_1_2_DETUNE)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_LOWPASS_CUTOFF] & 0x03) << 6;
    current_byte |= (p->values[PATCH_PARAM_OSC_3_DETUNE] & 0x07) << 3;
    current_byte |= p->values[PATCH_PARAM_OSC_4_DETUNE] & 0x07;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, LOWPASS_OSC_3_4_DETUNE)] = current_byte;

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

    /* envelope 1 */
    current_byte = p->values[PATCH_PARAM_ENV_1_ATTACK] & 0x1F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_1_ATTACK)] = current_byte;

    current_byte = p->values[PATCH_PARAM_ENV_1_DECAY] & 0x1F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_1_DECAY)] = current_byte;

    current_byte = p->values[PATCH_PARAM_ENV_1_SUSTAIN] & 0x1F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_1_SUSTAIN)] = current_byte;

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

    current_byte = p->values[PATCH_PARAM_ENV_2_SUSTAIN] & 0x1F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_2_SUSTAIN)] = current_byte;

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

    current_byte = p->values[PATCH_PARAM_ENV_3_SUSTAIN] & 0x1F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_3_SUSTAIN)] = current_byte;

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

    current_byte = p->values[PATCH_PARAM_ENV_4_SUSTAIN] & 0x1F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_4_SUSTAIN)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_ENV_4_HOLD_LEVEL] & 0x0F) << 4;
    current_byte |= p->values[PATCH_PARAM_ENV_4_RELEASE] & 0x0F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_4_HOLD_LEVEL_RELEASE)] = current_byte;

    current_byte = p->values[PATCH_PARAM_ENV_4_MAX_LEVEL] & 0x7F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, ENV_4_MAX_LEVEL)] = current_byte;

    /* keyscaling */
    current_byte =  (p->values[PATCH_PARAM_ENV_1_RATE_KS] & 0x03) << 6;
    current_byte |= (p->values[PATCH_PARAM_ENV_2_RATE_KS] & 0x03) << 4;
    current_byte |= (p->values[PATCH_PARAM_ENV_3_RATE_KS] & 0x03) << 2;
    current_byte |= p->values[PATCH_PARAM_ENV_4_RATE_KS] & 0x03;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, RATE_KEYSCALING)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_ENV_1_LEVEL_KS] & 0x03) << 6;
    current_byte |= (p->values[PATCH_PARAM_ENV_2_LEVEL_KS] & 0x03) << 4;
    current_byte |= (p->values[PATCH_PARAM_ENV_3_LEVEL_KS] & 0x03) << 2;
    current_byte |= p->values[PATCH_PARAM_ENV_4_LEVEL_KS] & 0x03;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, LEVEL_KEYSCALING)] = current_byte;

    /* lfo */
    current_byte =  (p->values[PATCH_PARAM_LFO_WAVEFORM] & 0x03) << 6;
    current_byte |= p->values[PATCH_PARAM_LFO_DELAY] & 0x3F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, LFO_WAVEFORM_DELAY)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_LFO_SYNC] & 0x01) << 4;
    current_byte |= p->values[PATCH_PARAM_LFO_SPEED] & 0x0F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, LFO_SYNC_SPEED)] = current_byte;

    /* sensitivities and depths */
    current_byte =  (p->values[PATCH_PARAM_VIBRATO_POLARITY] & 0x01) << 6;
    current_byte |= (p->values[PATCH_PARAM_VIBRATO_SENSITIVITY] & 0x07) << 3;
    current_byte |= p->values[PATCH_PARAM_VELOCITY_SENSITIVITY] & 0x07;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, VIBRATO_VELOCITY_SENSITIVITY)] = current_byte;

    current_byte =  (p->values[PATCH_PARAM_TREMOLO_MODE] & 0x01) << 7;
    current_byte |= (p->values[PATCH_PARAM_BOOST_MODE] & 0x01) << 6;
    current_byte |= (p->values[PATCH_PARAM_TREMOLO_SENSITIVITY] & 0x07) << 3;
    current_byte |= p->values[PATCH_PARAM_BOOST_SENSITIVITY] & 0x07;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, TREMOLO_BOOST_SENSITIVITY)] = current_byte;

    current_byte = p->values[PATCH_PARAM_VIBRATO_DEPTH] & 0x7F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, VIBRATO_DEPTH)] = current_byte;

    current_byte = p->values[PATCH_PARAM_TREMOLO_DEPTH] & 0x7F;

    cart_data[DATAFILE_COMPUTE_PATCH_DATA_INDEX(m, TREMOLO_DEPTH)] = current_byte;
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

