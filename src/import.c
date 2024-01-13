/*******************************************************************************
** import.c (loading non-native file formats)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bank.h"
#include "cart.h"
#include "import.h"
#include "patch.h"

#define IMPORT_SBI_COMPUTE_BYTE_SHARED_INDEX(name)                             \
  ( IMPORT_SBI_SHARED_START_INDEX +                                            \
    IMPORT_SBI_SHARED_BYTE_##name)

#define IMPORT_SBI_COMPUTE_BYTE_2OP_EXTRA_INDEX(name)                          \
  ( IMPORT_SBI_2OP_EXTRA_START_INDEX +                                         \
    IMPORT_SBI_2OP_EXTRA_BYTE_##name)

#define IMPORT_SBI_COMPUTE_BYTE_4OP_EXTRA_INDEX(name)                          \
  ( IMPORT_SBI_4OP_EXTRA_START_INDEX +                                         \
    IMPORT_SBI_4OP_EXTRA_BYTE_##name)

#define IMPORT_TFI_COMPUTE_BYTE_GENERAL_INDEX(name)                            \
  ( IMPORT_TFI_GENERAL_START_INDEX +                                           \
    IMPORT_TFI_BYTE_GENERAL_##name)

#define IMPORT_TFI_COMPUTE_BYTE_OPERATOR_INDEX(op_num, name)                   \
  ( IMPORT_TFI_OPERATOR_START_INDEX +                                          \
    (op_num * IMPORT_TFI_NUM_OPERATOR_BYTES) +                                 \
    IMPORT_TFI_BYTE_OPERATOR_##name)

#define IMPORT_OPM_COMPUTE_VALUE_GENERAL_INDEX(name)                           \
  ( IMPORT_OPM_GENERAL_START_INDEX +                                           \
    IMPORT_OPM_VALUE_GENERAL_##name)

#define IMPORT_OPM_COMPUTE_VALUE_OPERATOR_INDEX(op_num, name)                  \
  ( IMPORT_OPM_OPERATOR_START_INDEX +                                          \
    (op_num * IMPORT_OPM_NUM_OPERATOR_VALUES) +                                \
    IMPORT_OPM_VALUE_OPERATOR_##name)

/* algorithm tables */
static short int  S_import_opl_algorithm_table[IMPORT_OPL_ALGORITHM_NUM_VALUES] = 
                  { PATCH_ALGORITHM_1C_CHAIN, 
                    PATCH_ALGORITHM_2C_STACKED, 
                    PATCH_ALGORITHM_2C_TWIN, 
                    PATCH_ALGORITHM_3C_1_TO_1 
                  };

static short int  S_import_ym2612_algorithm_table[IMPORT_YM2612_ALGORITHM_NUM_VALUES] = 
                  { PATCH_ALGORITHM_1C_CHAIN, 
                    PATCH_ALGORITHM_1C_THE_Y, 
                    PATCH_ALGORITHM_1C_CRAB, 
                    PATCH_ALGORITHM_1C_CRAB, 
                    PATCH_ALGORITHM_2C_TWIN, 
                    PATCH_ALGORITHM_3C_1_TO_3, 
                    PATCH_ALGORITHM_3C_1_TO_1, 
                    PATCH_ALGORITHM_4C_PIPES 
                  };

/* feedback table */
static short int  S_import_feedback_table[IMPORT_OPL_FEEDBACK_NUM_VALUES] = 
                  { PATCH_OSC_FEEDBACK_LOWER_BOUND + 0, 
                    PATCH_OSC_FEEDBACK_LOWER_BOUND + 1, 
                    PATCH_OSC_FEEDBACK_LOWER_BOUND + 2, 
                    PATCH_OSC_FEEDBACK_LOWER_BOUND + 3, 
                    PATCH_OSC_FEEDBACK_LOWER_BOUND + 4, 
                    PATCH_OSC_FEEDBACK_LOWER_BOUND + 5, 
                    PATCH_OSC_FEEDBACK_LOWER_BOUND + 6, 
                    PATCH_OSC_FEEDBACK_LOWER_BOUND + 7 
                  };

/* multiple & divisor tables */
static short int  S_import_opl_multiple_table[IMPORT_OPL_MULTIPLE_NUM_VALUES] = 
                  { PATCH_OSC_MULTIPLE_LOWER_BOUND + 0, 
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 0, /* 1 */
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 1, 
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 2, 
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 3, 
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 4, 
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 5, 
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 6, 
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 7, 
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 8, 
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 9, 
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 9, 
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 11, 
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 11, 
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 14, 
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 14 
                  };

static short int  S_import_ym2612_multiple_table[IMPORT_YM2612_MULTIPLE_NUM_VALUES] = 
                  { PATCH_OSC_MULTIPLE_LOWER_BOUND + 0, 
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 0, /* 1 */
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 1, 
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 2, 
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 3, 
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 4, 
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 5, 
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 6, 
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 7, 
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 8, 
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 9, 
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 10, 
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 11, 
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 12, 
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 13, 
                    PATCH_OSC_MULTIPLE_LOWER_BOUND + 14 
                  };

static short int  S_import_divisor_table[IMPORT_OPL_MULTIPLE_NUM_VALUES] = 
                  { PATCH_OSC_DIVISOR_LOWER_BOUND + 1, 
                    PATCH_OSC_DIVISOR_LOWER_BOUND + 0, /* 1 */
                    PATCH_OSC_DIVISOR_LOWER_BOUND + 0, 
                    PATCH_OSC_DIVISOR_LOWER_BOUND + 0, 
                    PATCH_OSC_DIVISOR_LOWER_BOUND + 0, 
                    PATCH_OSC_DIVISOR_LOWER_BOUND + 0, 
                    PATCH_OSC_DIVISOR_LOWER_BOUND + 0, 
                    PATCH_OSC_DIVISOR_LOWER_BOUND + 0, 
                    PATCH_OSC_DIVISOR_LOWER_BOUND + 0, 
                    PATCH_OSC_DIVISOR_LOWER_BOUND + 0, 
                    PATCH_OSC_DIVISOR_LOWER_BOUND + 0, 
                    PATCH_OSC_DIVISOR_LOWER_BOUND + 0, 
                    PATCH_OSC_DIVISOR_LOWER_BOUND + 0, 
                    PATCH_OSC_DIVISOR_LOWER_BOUND + 0, 
                    PATCH_OSC_DIVISOR_LOWER_BOUND + 0, 
                    PATCH_OSC_DIVISOR_LOWER_BOUND + 0 
                  };

/* waveform table */
static short int  S_import_waveform_table[IMPORT_OPL_WAVEFORM_NUM_VALUES] = 
                  { PATCH_OSC_WAVEFORM_SINE, 
                    PATCH_OSC_WAVEFORM_HALF, 
                    PATCH_OSC_WAVEFORM_FULL, 
                    PATCH_OSC_WAVEFORM_QUARTER, 
                    PATCH_OSC_WAVEFORM_ALTERNATING, 
                    PATCH_OSC_WAVEFORM_CAMEL, 
                    PATCH_OSC_WAVEFORM_SQUARE, 
                    PATCH_OSC_WAVEFORM_LOG_SAW 
                  };

/* detune table */
static short int  S_import_ym2612_detune_table[IMPORT_YM2612_DETUNE_NUM_VALUES] = 
                  { PATCH_OSC_DETUNE_DEFAULT - 3, 
                    PATCH_OSC_DETUNE_DEFAULT - 2, 
                    PATCH_OSC_DETUNE_DEFAULT - 1, 
                    PATCH_OSC_DETUNE_DEFAULT + 0, 
                    PATCH_OSC_DETUNE_DEFAULT + 1, 
                    PATCH_OSC_DETUNE_DEFAULT + 2, 
                    PATCH_OSC_DETUNE_DEFAULT + 3, 
                    PATCH_OSC_DETUNE_DEFAULT + 0 
                  };

/* envelope rate tables */
static short int  S_import_opl_env_rate_table[IMPORT_OPL_ENV_RATE_NUM_VALUES] = 
                  { PATCH_ENV_RATE_LOWER_BOUND +  1, 
                    PATCH_ENV_RATE_LOWER_BOUND +  3, 
                    PATCH_ENV_RATE_LOWER_BOUND +  5, 
                    PATCH_ENV_RATE_LOWER_BOUND +  7, 
                    PATCH_ENV_RATE_LOWER_BOUND +  9, 
                    PATCH_ENV_RATE_LOWER_BOUND + 11, 
                    PATCH_ENV_RATE_LOWER_BOUND + 13, 
                    PATCH_ENV_RATE_LOWER_BOUND + 15, 
                    PATCH_ENV_RATE_LOWER_BOUND + 17, 
                    PATCH_ENV_RATE_LOWER_BOUND + 19, 
                    PATCH_ENV_RATE_LOWER_BOUND + 21, 
                    PATCH_ENV_RATE_LOWER_BOUND + 23, 
                    PATCH_ENV_RATE_LOWER_BOUND + 25, 
                    PATCH_ENV_RATE_LOWER_BOUND + 27, 
                    PATCH_ENV_RATE_LOWER_BOUND + 29, 
                    PATCH_ENV_RATE_LOWER_BOUND + 31 
                  };

static short int  S_import_ym2612_env_rate_table[IMPORT_YM2612_ENV_RATE_NUM_VALUES] = 
                  { PATCH_ENV_RATE_LOWER_BOUND +  0, 
                    PATCH_ENV_RATE_LOWER_BOUND +  1, 
                    PATCH_ENV_RATE_LOWER_BOUND +  2, 
                    PATCH_ENV_RATE_LOWER_BOUND +  3, 
                    PATCH_ENV_RATE_LOWER_BOUND +  4, 
                    PATCH_ENV_RATE_LOWER_BOUND +  5, 
                    PATCH_ENV_RATE_LOWER_BOUND +  6, 
                    PATCH_ENV_RATE_LOWER_BOUND +  7, 
                    PATCH_ENV_RATE_LOWER_BOUND +  8, 
                    PATCH_ENV_RATE_LOWER_BOUND +  9, 
                    PATCH_ENV_RATE_LOWER_BOUND + 10, 
                    PATCH_ENV_RATE_LOWER_BOUND + 11, 
                    PATCH_ENV_RATE_LOWER_BOUND + 12, 
                    PATCH_ENV_RATE_LOWER_BOUND + 13, 
                    PATCH_ENV_RATE_LOWER_BOUND + 14, 
                    PATCH_ENV_RATE_LOWER_BOUND + 15, 
                    PATCH_ENV_RATE_LOWER_BOUND + 16, 
                    PATCH_ENV_RATE_LOWER_BOUND + 17, 
                    PATCH_ENV_RATE_LOWER_BOUND + 18, 
                    PATCH_ENV_RATE_LOWER_BOUND + 19, 
                    PATCH_ENV_RATE_LOWER_BOUND + 20, 
                    PATCH_ENV_RATE_LOWER_BOUND + 21, 
                    PATCH_ENV_RATE_LOWER_BOUND + 22, 
                    PATCH_ENV_RATE_LOWER_BOUND + 23, 
                    PATCH_ENV_RATE_LOWER_BOUND + 24, 
                    PATCH_ENV_RATE_LOWER_BOUND + 25, 
                    PATCH_ENV_RATE_LOWER_BOUND + 26, 
                    PATCH_ENV_RATE_LOWER_BOUND + 27, 
                    PATCH_ENV_RATE_LOWER_BOUND + 28, 
                    PATCH_ENV_RATE_LOWER_BOUND + 29, 
                    PATCH_ENV_RATE_LOWER_BOUND + 30, 
                    PATCH_ENV_RATE_LOWER_BOUND + 31 
                  };

/* envelope amplitude & sustain level tables */
static short int  S_import_env_amplitude_table[IMPORT_OPL_TOTAL_LEVEL_NUM_VALUES] = 
                  { PATCH_ENV_AMPLITUDE_LOWER_BOUND + 32, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 31, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 31, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 30, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 29, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 29, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 28, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 27, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 27, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 26, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 25, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 25, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 24, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 23, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 23, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 22, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 21, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 21, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 20, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 19, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 19, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 18, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 17, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 17, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 16, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 15, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 15, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 14, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 13, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 13, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 12, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 11, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 11, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND + 10, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  9, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  9, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  8, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  7, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  7, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  6, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  5, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  5, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  4, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  3, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  3, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  2, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  1, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  1, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  0, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  0, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  0, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  0, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  0, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  0, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  0, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  0, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  0, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  0, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  0, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  0, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  0, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  0, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  0, 
                    PATCH_ENV_AMPLITUDE_LOWER_BOUND +  0 
                  };

static short int  S_import_env_sustain_table[IMPORT_OPL_SUSTAIN_LEVEL_NUM_VALUES] = 
                  { PATCH_ENV_SUSTAIN_LOWER_BOUND + 16, 
                    PATCH_ENV_SUSTAIN_LOWER_BOUND + 15, 
                    PATCH_ENV_SUSTAIN_LOWER_BOUND + 14, 
                    PATCH_ENV_SUSTAIN_LOWER_BOUND + 13, 
                    PATCH_ENV_SUSTAIN_LOWER_BOUND + 12, 
                    PATCH_ENV_SUSTAIN_LOWER_BOUND + 11, 
                    PATCH_ENV_SUSTAIN_LOWER_BOUND + 10, 
                    PATCH_ENV_SUSTAIN_LOWER_BOUND +  9, 
                    PATCH_ENV_SUSTAIN_LOWER_BOUND +  8, 
                    PATCH_ENV_SUSTAIN_LOWER_BOUND +  7, 
                    PATCH_ENV_SUSTAIN_LOWER_BOUND +  6, 
                    PATCH_ENV_SUSTAIN_LOWER_BOUND +  5, 
                    PATCH_ENV_SUSTAIN_LOWER_BOUND +  4, 
                    PATCH_ENV_SUSTAIN_LOWER_BOUND +  3, 
                    PATCH_ENV_SUSTAIN_LOWER_BOUND +  2, 
                    PATCH_ENV_SUSTAIN_LOWER_BOUND +  1 
                  };

/* keyscaling tables */
static short int  S_import_opl_rate_ks_table[IMPORT_OPL_RATE_KEYSCALE_NUM_VALUES] = 
                  { PATCH_ENV_KEYSCALING_LOWER_BOUND + 1, 
                    PATCH_ENV_KEYSCALING_LOWER_BOUND + 5 
                  };

static short int  S_import_ym2612_rate_ks_table[IMPORT_YM2612_RATE_KEYSCALE_NUM_VALUES] = 
                  { PATCH_ENV_KEYSCALING_LOWER_BOUND + 1, 
                    PATCH_ENV_KEYSCALING_LOWER_BOUND + 3, 
                    PATCH_ENV_KEYSCALING_LOWER_BOUND + 5, 
                    PATCH_ENV_KEYSCALING_LOWER_BOUND + 7 
                  };

static short int  S_import_level_ks_table[IMPORT_OPL_LEVEL_KEYSCALE_NUM_VALUES] = 
                  { PATCH_ENV_KEYSCALING_LOWER_BOUND + 1, 
                    PATCH_ENV_KEYSCALING_LOWER_BOUND + 3, 
                    PATCH_ENV_KEYSCALING_LOWER_BOUND + 5, 
                    PATCH_ENV_KEYSCALING_LOWER_BOUND + 7 
                  };

/*******************************************************************************
** import_sbi_load()
*******************************************************************************/
short int import_sbi_load(int cart_num, int patch_num, 
                          char* filename, int inst_id)
{
  FILE* fp;

  char      signature[4];
  short int type;

  unsigned char patch_data[IMPORT_SBI_MAX_PATCHES][IMPORT_SBI_4OP_TOTAL_NUM_BYTES];
  unsigned char name_data[IMPORT_SBI_MAX_PATCHES][IMPORT_SBI_PATCH_NAME_NUM_BYTES];

  unsigned char first_byte;
  unsigned char second_byte;

  int num_patches;

  patch* p;

  int patch_index;

  short int car_op[2];
  short int mod_op[2];

  short int current_op;
  short int fb_op;

  /* make sure the cart number is valid */
  if (CART_TOTAL_CART_NO_IS_NOT_VALID(cart_num))
    return 0;

  /* make sure the patch number is valid */
  if (CART_PATCH_NO_IS_NOT_VALID(patch_num))
    return 0;

  /* make sure filename is valid */
  if (filename == NULL)
    return 0;

  /* open .sbi file */
  fp = fopen(filename, "rb");

  /* if file did not open, return */
  if (fp == NULL)
    return 0;

  /* read patch data */
  num_patches = 0;

  while (fread(&signature, 1, 4, fp) == 4)
  {
    /* determine type based on signature */
    if (!strncmp(signature, "SBI\x1A", 4))
      type = IMPORT_SBI_TYPE_SBI;
    else if (!strncmp(signature, "2OP\x1A", 4))
      type = IMPORT_SBI_TYPE_2OP;
    else if (!strncmp(signature, "4OP\x1A", 4))
      type = IMPORT_SBI_TYPE_4OP;
    else
      break;

    /* read patch name and patch data */
    if (fread(&name_data[num_patches][0], 1, IMPORT_SBI_PATCH_NAME_NUM_BYTES, fp) < IMPORT_SBI_PATCH_NAME_NUM_BYTES)
      break;

    if ((type == IMPORT_SBI_TYPE_SBI) || 
        (type == IMPORT_SBI_TYPE_2OP))
    {
      if (fread(&patch_data[num_patches][0], 1, IMPORT_SBI_2OP_TOTAL_NUM_BYTES, fp) < IMPORT_SBI_2OP_TOTAL_NUM_BYTES)
        break;
    }
    else if (type == IMPORT_SBI_TYPE_4OP)
    {
      if (fread(&patch_data[num_patches][0], 1, IMPORT_SBI_4OP_TOTAL_NUM_BYTES, fp) < IMPORT_SBI_4OP_TOTAL_NUM_BYTES)
        break;
    }
    else
      break;

    /* increment patch count */
    num_patches += 1;

    if (num_patches == IMPORT_SBI_MAX_PATCHES)
      break;
  }

  /* close .sbi file */
  fclose(fp);

  /* make sure instrument id is valid */
  if ((inst_id < 0) || (inst_id >= num_patches))
    return 0;

  /* load patch data to cart */
  CART_COMPUTE_PATCH_INDEX(cart_num, patch_num)

  p = &G_patch_bank[patch_index];

  /* reset patch */
  patch_reset(patch_index);

  /* algorithm & feedback */
  if (type == IMPORT_SBI_TYPE_4OP)
  {
    first_byte = patch_data[inst_id][IMPORT_SBI_COMPUTE_BYTE_SHARED_INDEX(FEEDBACK_ALGORITHM)];
    second_byte = patch_data[inst_id][IMPORT_SBI_COMPUTE_BYTE_4OP_EXTRA_INDEX(ALGORITHM)];
  }
  else
  {
    first_byte = patch_data[inst_id][IMPORT_SBI_COMPUTE_BYTE_SHARED_INDEX(FEEDBACK_ALGORITHM)];
    second_byte = 0;
  }

  if (type == IMPORT_SBI_TYPE_4OP)
  {
    /* chain */
    if (((second_byte & 0x01) == 0) && ((first_byte & 0x01) == 0))
    {
      mod_op[0] = 0;
      car_op[0] = 1;
      mod_op[1] = 2;
      car_op[1] = 3;

      fb_op = 0;
    }
    /* stacked */
    else if (((second_byte & 0x01) == 0) && ((first_byte & 0x01) == 1))
    {
      mod_op[0] = 3;
      car_op[0] = 0;
      mod_op[1] = 1;
      car_op[1] = 2;

      fb_op = 3;
    }
    /* twin */
    else if (((second_byte & 0x01) == 1) && ((first_byte & 0x01) == 0))
    {
      mod_op[0] = 0;
      car_op[0] = 2;
      mod_op[1] = 1;
      car_op[1] = 3;

      fb_op = 0;
    }
    /* 1 to 1 */
    else
    {
      mod_op[0] = 2;
      car_op[0] = 0;
      mod_op[1] = 1;
      car_op[1] = 3;

      fb_op = 2;
    }

    p->algorithm = S_import_opl_algorithm_table[(first_byte & 0x01) | ((second_byte & 0x01) << 1)];
  }
  else
  {
    if ((first_byte & 0x01) == 0)
    {
      mod_op[0] = 2;
      car_op[0] = 3;
      mod_op[1] = 0;
      car_op[1] = 1;

      fb_op = 2;
    }
    else
    {
      mod_op[0] = 2;
      car_op[0] = 3;
      mod_op[1] = 0;
      car_op[1] = 1;

      fb_op = 2;
    }

    p->algorithm = S_import_opl_algorithm_table[first_byte & 0x01];
  }

  p->osc_feedback[fb_op] = S_import_feedback_table[(first_byte & 0x0E) >> 1];

  /* modulator 1 */
  current_op = mod_op[0];

  /* modulator level keyscaling, total level */
  first_byte = patch_data[inst_id][IMPORT_SBI_COMPUTE_BYTE_SHARED_INDEX(MODULATOR_LEVEL_KEYSCALE_TOTAL_LEVEL)];

  p->env_level_ks[current_op] = S_import_level_ks_table[(first_byte & 0xC0) >> 6];
  p->env_amplitude[current_op] = S_import_env_amplitude_table[first_byte & 0x3F];

  /* modulator attack rate, decay 1 rate */
  first_byte = patch_data[inst_id][IMPORT_SBI_COMPUTE_BYTE_SHARED_INDEX(MODULATOR_ATTACK_RATE_DECAY_RATE)];

  p->env_attack[current_op] = S_import_opl_env_rate_table[(first_byte & 0xF0) >> 4];
  p->env_decay_1[current_op] = S_import_opl_env_rate_table[first_byte & 0x0F];
  p->env_decay_2[current_op] = p->env_decay_1[current_op];

  /* modulator sustain level, release rate */
  first_byte = patch_data[inst_id][IMPORT_SBI_COMPUTE_BYTE_SHARED_INDEX(MODULATOR_SUSTAIN_LEVEL_RELEASE_RATE)];

  p->env_sustain[current_op] = S_import_env_sustain_table[(first_byte & 0xF0) >> 4];
  p->env_release[current_op] = S_import_opl_env_rate_table[first_byte & 0x0F];

  /* modulator waveform */
  first_byte = patch_data[inst_id][IMPORT_SBI_COMPUTE_BYTE_SHARED_INDEX(MODULATOR_WAVEFORM)];

  p->osc_waveform[current_op] = S_import_waveform_table[first_byte & 0x07];

  /* modulator multiple, flags */
  first_byte = patch_data[inst_id][IMPORT_SBI_COMPUTE_BYTE_SHARED_INDEX(MODULATOR_MULTIPLE_FLAGS)];

  p->osc_freq_mode[current_op] = PATCH_OSC_FREQ_MODE_RATIO;

  p->osc_multiple[current_op] = S_import_opl_multiple_table[first_byte & 0x0F];
  p->osc_divisor[current_op] = S_import_divisor_table[first_byte & 0x0F];

  if (((first_byte & 0x20) >> 5) != 0)
    p->env_decay_2[current_op] = S_import_opl_env_rate_table[0];

  p->env_rate_ks[current_op] = S_import_opl_rate_ks_table[(first_byte & 0x10) >> 4];

  /* carrier 1 */
  current_op = car_op[0];

  /* carrier level keyscaling, total level */
  first_byte = patch_data[inst_id][IMPORT_SBI_COMPUTE_BYTE_SHARED_INDEX(CARRIER_LEVEL_KEYSCALE_TOTAL_LEVEL)];

  p->env_level_ks[current_op] = S_import_level_ks_table[(first_byte & 0xC0) >> 6];
  p->env_amplitude[current_op] = S_import_env_amplitude_table[first_byte & 0x3F];

  /* carrier attack rate, decay 1 rate */
  first_byte = patch_data[inst_id][IMPORT_SBI_COMPUTE_BYTE_SHARED_INDEX(CARRIER_ATTACK_RATE_DECAY_RATE)];

  p->env_attack[current_op] = S_import_opl_env_rate_table[(first_byte & 0xF0) >> 4];
  p->env_decay_1[current_op] = S_import_opl_env_rate_table[first_byte & 0x0F];
  p->env_decay_2[current_op] = p->env_decay_1[current_op];

  /* carrier sustain level, release rate */
  first_byte = patch_data[inst_id][IMPORT_SBI_COMPUTE_BYTE_SHARED_INDEX(CARRIER_SUSTAIN_LEVEL_RELEASE_RATE)];

  p->env_sustain[current_op] = S_import_env_sustain_table[(first_byte & 0xF0) >> 4];
  p->env_release[current_op] = S_import_opl_env_rate_table[first_byte & 0x0F];

  /* carrier waveform */
  first_byte = patch_data[inst_id][IMPORT_SBI_COMPUTE_BYTE_SHARED_INDEX(CARRIER_WAVEFORM)];

  p->osc_waveform[current_op] = S_import_waveform_table[first_byte & 0x07];

  /* carrier multiple, flags */
  first_byte = patch_data[inst_id][IMPORT_SBI_COMPUTE_BYTE_SHARED_INDEX(CARRIER_MULTIPLE_FLAGS)];

  p->osc_freq_mode[current_op] = PATCH_OSC_FREQ_MODE_RATIO;

  p->osc_multiple[current_op] = S_import_opl_multiple_table[first_byte & 0x0F];
  p->osc_divisor[current_op] = S_import_divisor_table[first_byte & 0x0F];

  if (((first_byte & 0x20) >> 5) != 0)
    p->env_decay_2[current_op] = S_import_opl_env_rate_table[0];

  p->env_rate_ks[current_op] = S_import_opl_rate_ks_table[(first_byte & 0x10) >> 4];

  if (type == IMPORT_SBI_TYPE_4OP)
  {
    /* modulator 2 */
    current_op = mod_op[1];

    /* modulator level keyscaling, total level */
    first_byte = patch_data[inst_id][IMPORT_SBI_COMPUTE_BYTE_4OP_EXTRA_INDEX(MODULATOR_LEVEL_KEYSCALE_TOTAL_LEVEL)];

    p->env_level_ks[current_op] = S_import_level_ks_table[(first_byte & 0xC0) >> 6];
    p->env_amplitude[current_op] = S_import_env_amplitude_table[first_byte & 0x3F];

    /* modulator attack rate, decay 1 rate */
    first_byte = patch_data[inst_id][IMPORT_SBI_COMPUTE_BYTE_4OP_EXTRA_INDEX(MODULATOR_ATTACK_RATE_DECAY_RATE)];

    p->env_attack[current_op] = S_import_opl_env_rate_table[(first_byte & 0xF0) >> 4];
    p->env_decay_1[current_op] = S_import_opl_env_rate_table[first_byte & 0x0F];
    p->env_decay_2[current_op] = p->env_decay_1[current_op];

    /* modulator sustain level, release rate */
    first_byte = patch_data[inst_id][IMPORT_SBI_COMPUTE_BYTE_4OP_EXTRA_INDEX(MODULATOR_SUSTAIN_LEVEL_RELEASE_RATE)];

    p->env_sustain[current_op] = S_import_env_sustain_table[(first_byte & 0xF0) >> 4];
    p->env_release[current_op] = S_import_opl_env_rate_table[first_byte & 0x0F];

    /* modulator waveform */
    first_byte = patch_data[inst_id][IMPORT_SBI_COMPUTE_BYTE_4OP_EXTRA_INDEX(MODULATOR_WAVEFORM)];

    p->osc_waveform[current_op] = S_import_waveform_table[first_byte & 0x07];

    /* modulator multiple, flags */
    first_byte = patch_data[inst_id][IMPORT_SBI_COMPUTE_BYTE_4OP_EXTRA_INDEX(MODULATOR_MULTIPLE_FLAGS)];

    p->osc_freq_mode[current_op] = PATCH_OSC_FREQ_MODE_RATIO;

    p->osc_multiple[current_op] = S_import_opl_multiple_table[first_byte & 0x0F];
    p->osc_divisor[current_op] = S_import_divisor_table[first_byte & 0x0F];

    if (((first_byte & 0x20) >> 5) != 0)
      p->env_decay_2[current_op] = S_import_opl_env_rate_table[0];

    p->env_rate_ks[current_op] = S_import_opl_rate_ks_table[(first_byte & 0x10) >> 4];

    /* carrier 2 */
    current_op = car_op[1];

    /* carrier level keyscaling, total level */
    first_byte = patch_data[inst_id][IMPORT_SBI_COMPUTE_BYTE_4OP_EXTRA_INDEX(CARRIER_LEVEL_KEYSCALE_TOTAL_LEVEL)];

    p->env_level_ks[current_op] = S_import_level_ks_table[(first_byte & 0xC0) >> 6];
    p->env_amplitude[current_op] = S_import_env_amplitude_table[first_byte & 0x3F];

    /* carrier attack rate, decay 1 rate */
    first_byte = patch_data[inst_id][IMPORT_SBI_COMPUTE_BYTE_4OP_EXTRA_INDEX(CARRIER_ATTACK_RATE_DECAY_RATE)];

    p->env_attack[current_op] = S_import_opl_env_rate_table[(first_byte & 0xF0) >> 4];
    p->env_decay_1[current_op] = S_import_opl_env_rate_table[first_byte & 0x0F];
    p->env_decay_2[current_op] = p->env_decay_1[current_op];

    /* carrier sustain level, release rate */
    first_byte = patch_data[inst_id][IMPORT_SBI_COMPUTE_BYTE_4OP_EXTRA_INDEX(CARRIER_SUSTAIN_LEVEL_RELEASE_RATE)];

    p->env_sustain[current_op] = S_import_env_sustain_table[(first_byte & 0xF0) >> 4];
    p->env_release[current_op] = S_import_opl_env_rate_table[first_byte & 0x0F];

    /* carrier waveform */
    first_byte = patch_data[inst_id][IMPORT_SBI_COMPUTE_BYTE_4OP_EXTRA_INDEX(CARRIER_WAVEFORM)];

    p->osc_waveform[current_op] = S_import_waveform_table[first_byte & 0x07];

    /* carrier multiple, flags */
    first_byte = patch_data[inst_id][IMPORT_SBI_COMPUTE_BYTE_4OP_EXTRA_INDEX(CARRIER_MULTIPLE_FLAGS)];

    p->osc_freq_mode[current_op] = PATCH_OSC_FREQ_MODE_RATIO;

    p->osc_multiple[current_op] = S_import_opl_multiple_table[first_byte & 0x0F];
    p->osc_divisor[current_op] = S_import_divisor_table[first_byte & 0x0F];

    if (((first_byte & 0x20) >> 5) != 0)
      p->env_decay_2[current_op] = S_import_opl_env_rate_table[0];

    p->env_rate_ks[current_op] = S_import_opl_rate_ks_table[(first_byte & 0x10) >> 4];
  }

  /* validate the parameters */
  patch_validate(patch_index);

  return 0;
}

/*******************************************************************************
** import_tfi_load()
*******************************************************************************/
short int import_tfi_load(int cart_num, int patch_num, 
                          char* filename, int inst_id)
{
  int m;

  FILE* fp;

  unsigned char patch_data[IMPORT_TFI_MAX_PATCHES][IMPORT_TFI_NUM_BYTES];
  unsigned char first_byte;
  unsigned char second_byte;

  int num_patches;

  patch* p;

  int patch_index;

  short int op_order[4];
  short int current_op;
  short int fb_op;

  /* make sure the cart number is valid */
  if (CART_TOTAL_CART_NO_IS_NOT_VALID(cart_num))
    return 0;

  /* make sure the patch number is valid */
  if (CART_PATCH_NO_IS_NOT_VALID(patch_num))
    return 0;

  /* make sure filename is valid */
  if (filename == NULL)
    return 0;

  /* open .tfi file */
  fp = fopen(filename, "rb");

  /* if file did not open, return */
  if (fp == NULL)
    return 0;

  /* read patch data */
  num_patches = 0;

  while (fread(&patch_data[num_patches][0], 1, IMPORT_TFI_NUM_BYTES, fp) == IMPORT_TFI_NUM_BYTES)
  {
    num_patches += 1;
  }

  /* close .tmb file */
  fclose(fp);

  /* make sure instrument id is valid */
  if ((inst_id < 0) || (inst_id >= num_patches))
    return 0;

  /* load patch data to cart */
  CART_COMPUTE_PATCH_INDEX(cart_num, patch_num)

  p = &G_patch_bank[patch_index];

  /* reset patch */
  patch_reset(patch_index);

  /* algorithm & feedback */
  first_byte = patch_data[inst_id][IMPORT_TFI_COMPUTE_BYTE_GENERAL_INDEX(ALGORITHM)];
  second_byte = patch_data[inst_id][IMPORT_TFI_COMPUTE_BYTE_GENERAL_INDEX(FEEDBACK)];

  /* determine operator ordering */
  /* (the order in the file should be S1, S3, S2, S4) */
  if ((first_byte & 0x07) == 2)
  {
    op_order[0] = 2;
    op_order[1] = 1;
    op_order[2] = 0;
    op_order[3] = 3;

    fb_op = 2;
  }
  else if ((first_byte & 0x07) == 4)
  {
    op_order[0] = 0;
    op_order[1] = 1;
    op_order[2] = 2;
    op_order[3] = 3;

    fb_op = 0;
  }
  else
  {
    op_order[0] = 0;
    op_order[1] = 2;
    op_order[2] = 1;
    op_order[3] = 3;

    fb_op = 0;
  }

  /* set the algorithm & feedback */
  p->algorithm = S_import_ym2612_algorithm_table[first_byte & 0x07];
  p->osc_feedback[fb_op] = S_import_feedback_table[second_byte & 0x07];

  for (m = 0; m < 4; m++)
  {
    current_op = op_order[m];

    /* multiple */
    first_byte = patch_data[inst_id][IMPORT_TFI_COMPUTE_BYTE_OPERATOR_INDEX(current_op, MULTIPLE)];

    p->osc_freq_mode[current_op] = PATCH_OSC_FREQ_MODE_RATIO;

    p->osc_multiple[current_op] = S_import_ym2612_multiple_table[first_byte & 0x0F];
    p->osc_divisor[current_op] = S_import_divisor_table[first_byte & 0x0F];

    /* detune */
    first_byte = patch_data[inst_id][IMPORT_TFI_COMPUTE_BYTE_OPERATOR_INDEX(current_op, DETUNE)];

    p->osc_detune[current_op] = S_import_ym2612_detune_table[first_byte & 0x07];

    /* total level */
    /* note that if the tl value is >= 64, it is clamped to 63 */
    first_byte = patch_data[inst_id][IMPORT_TFI_COMPUTE_BYTE_OPERATOR_INDEX(current_op, TOTAL_LEVEL)];

    if ((first_byte & 0x40) != 0)
      p->env_amplitude[current_op] = S_import_env_amplitude_table[63];
    else
      p->env_amplitude[current_op] = S_import_env_amplitude_table[first_byte & 0x3F];

    /* rate keyscaling */
    first_byte = patch_data[inst_id][IMPORT_TFI_COMPUTE_BYTE_OPERATOR_INDEX(current_op, RATE_KEYSCALE)];

    p->env_rate_ks[current_op] = S_import_ym2612_rate_ks_table[first_byte & 0x03];

    /* attack */
    first_byte = patch_data[inst_id][IMPORT_TFI_COMPUTE_BYTE_OPERATOR_INDEX(current_op, ATTACK_RATE)];

    p->env_attack[current_op] = S_import_ym2612_env_rate_table[first_byte & 0x1F];

    /* decay 1 */
    first_byte = patch_data[inst_id][IMPORT_TFI_COMPUTE_BYTE_OPERATOR_INDEX(current_op, DECAY_1_RATE)];

    p->env_decay_1[current_op] = S_import_ym2612_env_rate_table[first_byte & 0x1F];

    /* decay 2 */
    first_byte = patch_data[inst_id][IMPORT_TFI_COMPUTE_BYTE_OPERATOR_INDEX(current_op, DECAY_2_RATE)];

    p->env_decay_2[current_op] = S_import_ym2612_env_rate_table[first_byte & 0x1F];

    /* release */
    first_byte = patch_data[inst_id][IMPORT_TFI_COMPUTE_BYTE_OPERATOR_INDEX(current_op, RELEASE_RATE)];

    p->env_release[current_op] = S_import_opl_env_rate_table[first_byte & 0x0F];

    /* sustain level */
    first_byte = patch_data[inst_id][IMPORT_TFI_COMPUTE_BYTE_OPERATOR_INDEX(current_op, SUSTAIN_LEVEL)];

    p->env_sustain[current_op] = S_import_env_sustain_table[first_byte & 0x0F];
  }

  /* validate the parameters */
  patch_validate(patch_index);

  return 0;
}

