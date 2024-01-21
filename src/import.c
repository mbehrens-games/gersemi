/*******************************************************************************
** import.c (loading non-native file formats)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bank.h"
#include "import.h"
#include "patch.h"
#include "text.h"

/* opm string parsing macros */
#define IMPORT_OPM_PARSE_SKIP_SPACES()                                         \
  while ( (line_pos < IMPORT_OPM_TEXT_LINE_MAX_LENGTH) &&                      \
          TEXT_CHARACTER_IS_SPACE_OR_TAB(text_line[line_pos]))                 \
  {                                                                            \
    line_pos += 1;                                                             \
  }

#define IMPORT_OPM_PARSE_SCAN_INTEGER()                                        \
  token_size = 0;                                                              \
                                                                               \
  while (line_pos + token_size < IMPORT_OPM_TEXT_LINE_MAX_LENGTH)              \
  {                                                                            \
    if (TEXT_CHARACTER_IS_DIGIT(text_line[line_pos + token_size]))             \
      token_size += 1;                                                         \
    else                                                                       \
      break;                                                                   \
  }

#define IMPORT_OPM_ADVANCE_OVER_TOKEN()                                        \
  line_pos += token_size;

#define IMPORT_OPM_PARSE_SCAN_ALPHANUMERIC_STRING()                            \
  token_size = 0;                                                              \
                                                                               \
  while (line_pos + token_size < IMPORT_OPM_TEXT_LINE_MAX_LENGTH)              \
  {                                                                            \
    if (TEXT_CHARACTER_IS_DIGIT(text_line[line_pos + token_size])   ||         \
        TEXT_CHARACTER_IS_LETTER(text_line[line_pos + token_size])  ||         \
        (text_line[line_pos + token_size] >= ' '))                             \
    {                                                                          \
      token_size += 1;                                                         \
    }                                                                          \
    else                                                                       \
      break;                                                                   \
  }

#define IMPORT_OPM_READ_GENERAL_VALUE(name)                                    \
  IMPORT_OPM_PARSE_SKIP_SPACES()                                               \
  IMPORT_OPM_PARSE_SCAN_INTEGER()                                              \
                                                                               \
  patch_data[num_patches][IMPORT_OPM_COMPUTE_VALUE_GENERAL_INDEX(name)] =      \
    strtol(&text_line[line_pos], NULL, 10);                                    \
                                                                               \
  IMPORT_OPM_ADVANCE_OVER_TOKEN()

#define IMPORT_OPM_READ_OPERATOR_VALUE(op_num, name)                                \
  IMPORT_OPM_PARSE_SKIP_SPACES()                                                    \
  IMPORT_OPM_PARSE_SCAN_INTEGER()                                                   \
                                                                                    \
  patch_data[num_patches][IMPORT_OPM_COMPUTE_VALUE_OPERATOR_INDEX(op_num, name)] =  \
    strtol(&text_line[line_pos], NULL, 10);                                         \
                                                                                    \
  IMPORT_OPM_ADVANCE_OVER_TOKEN()

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

/* detune tables */
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

static short int  S_import_ym2151_detune_table[IMPORT_YM2151_DETUNE_1_NUM_VALUES] = 
                  { PATCH_OSC_DETUNE_DEFAULT + 0, 
                    PATCH_OSC_DETUNE_DEFAULT + 1, 
                    PATCH_OSC_DETUNE_DEFAULT + 2, 
                    PATCH_OSC_DETUNE_DEFAULT + 3, 
                    PATCH_OSC_DETUNE_DEFAULT + 0, 
                    PATCH_OSC_DETUNE_DEFAULT - 1, 
                    PATCH_OSC_DETUNE_DEFAULT - 2, 
                    PATCH_OSC_DETUNE_DEFAULT - 3 
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

/* tremolo & vibrato depth tables */
static short int  S_import_ym2151_tremolo_depth_table[IMPORT_YM2151_TREMOLO_DEPTH_NUM_VALUES] = 
                  { PATCH_EFFECT_DEPTH_LOWER_BOUND + 0, 
                    PATCH_EFFECT_DEPTH_LOWER_BOUND + 3, 
                    PATCH_EFFECT_DEPTH_LOWER_BOUND + 7, 
                    PATCH_EFFECT_DEPTH_LOWER_BOUND + 15 
                  };

static short int  S_import_ym2151_vibrato_depth_table[IMPORT_YM2151_VIBRATO_DEPTH_NUM_VALUES] = 
                  { PATCH_EFFECT_DEPTH_LOWER_BOUND + 0, 
                    PATCH_EFFECT_DEPTH_LOWER_BOUND + 0, 
                    PATCH_EFFECT_DEPTH_LOWER_BOUND + 2, 
                    PATCH_EFFECT_DEPTH_LOWER_BOUND + 4, 
                    PATCH_EFFECT_DEPTH_LOWER_BOUND + 7, 
                    PATCH_EFFECT_DEPTH_LOWER_BOUND + 12, 
                    PATCH_EFFECT_DEPTH_LOWER_BOUND + 14, 
                    PATCH_EFFECT_DEPTH_LOWER_BOUND + 15 
                  };

static short int  S_import_ym2151_effect_base_table[IMPORT_YM2151_EFFECT_BASE_NUM_VALUES / IMPORT_YM2151_EFFECT_BASE_DIVISOR] = 
                  { PATCH_EFFECT_BASE_LOWER_BOUND +  0, 
                    PATCH_EFFECT_BASE_LOWER_BOUND +  1, 
                    PATCH_EFFECT_BASE_LOWER_BOUND +  1, 
                    PATCH_EFFECT_BASE_LOWER_BOUND +  2, 
                    PATCH_EFFECT_BASE_LOWER_BOUND +  2, 
                    PATCH_EFFECT_BASE_LOWER_BOUND +  3, 
                    PATCH_EFFECT_BASE_LOWER_BOUND +  3, 
                    PATCH_EFFECT_BASE_LOWER_BOUND +  4, 
                    PATCH_EFFECT_BASE_LOWER_BOUND +  4, 
                    PATCH_EFFECT_BASE_LOWER_BOUND +  5, 
                    PATCH_EFFECT_BASE_LOWER_BOUND +  5, 
                    PATCH_EFFECT_BASE_LOWER_BOUND +  6, 
                    PATCH_EFFECT_BASE_LOWER_BOUND +  6, 
                    PATCH_EFFECT_BASE_LOWER_BOUND +  7, 
                    PATCH_EFFECT_BASE_LOWER_BOUND +  7, 
                    PATCH_EFFECT_BASE_LOWER_BOUND +  8, 
                    PATCH_EFFECT_BASE_LOWER_BOUND +  8, 
                    PATCH_EFFECT_BASE_LOWER_BOUND +  9, 
                    PATCH_EFFECT_BASE_LOWER_BOUND +  9, 
                    PATCH_EFFECT_BASE_LOWER_BOUND + 10, 
                    PATCH_EFFECT_BASE_LOWER_BOUND + 10, 
                    PATCH_EFFECT_BASE_LOWER_BOUND + 11, 
                    PATCH_EFFECT_BASE_LOWER_BOUND + 11, 
                    PATCH_EFFECT_BASE_LOWER_BOUND + 12, 
                    PATCH_EFFECT_BASE_LOWER_BOUND + 12, 
                    PATCH_EFFECT_BASE_LOWER_BOUND + 13, 
                    PATCH_EFFECT_BASE_LOWER_BOUND + 13, 
                    PATCH_EFFECT_BASE_LOWER_BOUND + 14, 
                    PATCH_EFFECT_BASE_LOWER_BOUND + 14, 
                    PATCH_EFFECT_BASE_LOWER_BOUND + 15, 
                    PATCH_EFFECT_BASE_LOWER_BOUND + 15, 
                    PATCH_EFFECT_BASE_LOWER_BOUND + 16 
                  };

/* lfo waveform table */
static short int  S_import_ym2151_lfo_waveform_table[IMPORT_YM2151_LFO_WAVEFORM_NUM_VALUES] = 
                  { PATCH_LFO_WAVEFORM_SAW_UP, 
                    PATCH_LFO_WAVEFORM_SQUARE, 
                    PATCH_LFO_WAVEFORM_TRIANGLE, 
                    PATCH_LFO_WAVEFORM_NOISE_SQUARE 
                  };

/*******************************************************************************
** import_sbi_load()
*******************************************************************************/
short int import_sbi_load(int cart_num, int patch_num, 
                          char* filename, int inst_id, int batching)
{
  int k;
  int m;

  FILE* fp;

  char      signature[4];
  short int type;

  unsigned char patch_data[IMPORT_SBI_MAX_PATCHES][IMPORT_SBI_4OP_TOTAL_NUM_BYTES];
  char          name_data[IMPORT_SBI_MAX_PATCHES][IMPORT_SBI_PATCH_NAME_NUM_BYTES];

  unsigned char first_byte;
  unsigned char second_byte;

  int num_patches;

  patch* p;

  int patch_index;

  short int batch_count;

  short int op_order[4];
  short int current_op;
  short int fb_op;

  short int start_indices[4];
  short int num_ops;

  /* make sure the cart number is valid */
  if (PATCH_CART_NO_IS_NOT_VALID(cart_num))
    return 0;

  /* make sure the patch number is valid */
  if (PATCH_PATCH_NO_IS_NOT_VALID(patch_num))
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

  /* determine batch count */
  if (batching == IMPORT_BATCHING_1)
    batch_count = 1;
  else if (batching == IMPORT_BATCHING_8)
    batch_count = 8;
  else if (batching == IMPORT_BATCHING_16)
    batch_count = 16;
  else
    batch_count = 1;

  /* load batch */
  for (k = 0; k < batch_count; k++)
  {
    /* make sure the instrument id is valid */
    if (IMPORT_INSTRUMENT_ID_IS_NOT_VALID(inst_id + k))
      break;

    /* make sure the patch number is valid */
    if (PATCH_PATCH_NO_IS_NOT_VALID(patch_num + k))
      break;

    /* load patch data to cart */
    PATCH_COMPUTE_PATCH_INDEX(cart_num, patch_num + k)

    p = &G_patch_bank[patch_index];

    /* reset patch */
    patch_reset_patch(patch_index);

    /* patch name */
    strncpy(&G_patch_names[patch_index][0], &name_data[inst_id + k][0], PATCH_PATCH_NAME_SIZE);

    /* algorithm & feedback */
    if (type == IMPORT_SBI_TYPE_4OP)
    {
      first_byte = patch_data[inst_id + k][IMPORT_SBI_COMPUTE_BYTE_SHARED_INDEX(FEEDBACK_ALGORITHM)];
      second_byte = patch_data[inst_id + k][IMPORT_SBI_COMPUTE_BYTE_4OP_EXTRA_INDEX(ALGORITHM)];
    }
    else
    {
      first_byte = patch_data[inst_id + k][IMPORT_SBI_COMPUTE_BYTE_SHARED_INDEX(FEEDBACK_ALGORITHM)];
      second_byte = 0;
    }

    /* determine op order */
    if (type == IMPORT_SBI_TYPE_4OP)
    {
      /* chain */
      if (((second_byte & 0x01) == 0) && ((first_byte & 0x01) == 0))
      {
        op_order[0] = 0;
        op_order[1] = 1;
        op_order[2] = 2;
        op_order[3] = 3;

        fb_op = 0;
      }
      /* stacked */
      else if (((second_byte & 0x01) == 0) && ((first_byte & 0x01) == 1))
      {
        op_order[0] = 3;
        op_order[1] = 0;
        op_order[2] = 1;
        op_order[3] = 2;

        fb_op = 3;
      }
      /* twin */
      else if (((second_byte & 0x01) == 1) && ((first_byte & 0x01) == 0))
      {
        op_order[0] = 0;
        op_order[1] = 2;
        op_order[2] = 1;
        op_order[3] = 3;

        fb_op = 0;
      }
      /* 1 to 1 */
      else
      {
        op_order[0] = 2;
        op_order[1] = 0;
        op_order[2] = 1;
        op_order[3] = 3;

        fb_op = 2;
      }

      start_indices[0] = IMPORT_SBI_M1_START_INDEX;
      start_indices[1] = IMPORT_SBI_C1_START_INDEX;
      start_indices[2] = IMPORT_SBI_M2_START_INDEX;
      start_indices[3] = IMPORT_SBI_C2_START_INDEX;

      num_ops = 4;

      p->algorithm = S_import_opl_algorithm_table[(first_byte & 0x01) | ((second_byte & 0x01) << 1)];
    }
    else
    {
      if ((first_byte & 0x01) == 0)
      {
        op_order[0] = 2;
        op_order[1] = 3;
        op_order[2] = 0;
        op_order[3] = 1;

        fb_op = 2;
      }
      else
      {
        op_order[0] = 2;
        op_order[1] = 3;
        op_order[2] = 0;
        op_order[3] = 1;

        fb_op = 2;
      }

      start_indices[0] = IMPORT_SBI_M1_START_INDEX;
      start_indices[1] = IMPORT_SBI_C1_START_INDEX;
      start_indices[2] = IMPORT_SBI_M2_START_INDEX;
      start_indices[3] = IMPORT_SBI_C2_START_INDEX;

      num_ops = 2;

      p->algorithm = S_import_opl_algorithm_table[first_byte & 0x01];
    }

    p->osc_feedback[fb_op] = S_import_feedback_table[(first_byte & 0x0E) >> 1];

    for (m = 0; m < num_ops; m++)
    {
      current_op = op_order[m];

      /* level keyscaling, total level */
      first_byte = patch_data[inst_id + k][start_indices[m] + IMPORT_SBI_OPERATOR_OFFSET_LEVEL_KEYSCALE_TOTAL_LEVEL];

      p->env_level_ks[current_op] = S_import_level_ks_table[(first_byte & 0xC0) >> 6];
      p->env_amplitude[current_op] = S_import_env_amplitude_table[first_byte & 0x3F];

      /* attack rate, decay 1 rate */
      first_byte = patch_data[inst_id + k][start_indices[m] + IMPORT_SBI_OPERATOR_OFFSET_ATTACK_RATE_DECAY_RATE];

      p->env_attack[current_op] = S_import_opl_env_rate_table[(first_byte & 0xF0) >> 4];
      p->env_decay_1[current_op] = S_import_opl_env_rate_table[first_byte & 0x0F];
      p->env_decay_2[current_op] = p->env_decay_1[current_op];

      /* sustain level, release rate */
      first_byte = patch_data[inst_id + k][start_indices[m] + IMPORT_SBI_OPERATOR_OFFSET_SUSTAIN_LEVEL_RELEASE_RATE];

      p->env_sustain[current_op] = S_import_env_sustain_table[(first_byte & 0xF0) >> 4];
      p->env_release[current_op] = S_import_opl_env_rate_table[first_byte & 0x0F];

      /* waveform */
      first_byte = patch_data[inst_id + k][start_indices[m] + IMPORT_SBI_OPERATOR_OFFSET_WAVEFORM];

      p->osc_waveform[current_op] = S_import_waveform_table[first_byte & 0x07];

      /* multiple, flags */
      first_byte = patch_data[inst_id + k][start_indices[m] + IMPORT_SBI_OPERATOR_OFFSET_MULTIPLE_FLAGS];

      p->osc_freq_mode[current_op] = PATCH_OSC_FREQ_MODE_RATIO;

      p->osc_multiple[current_op] = S_import_opl_multiple_table[first_byte & 0x0F];
      p->osc_divisor[current_op] = S_import_divisor_table[first_byte & 0x0F];

      if (((first_byte & 0x80) >> 7) != 0)
      {
        p->tremolo_depth = PATCH_EFFECT_DEPTH_LOWER_BOUND + 7;
        p->tremolo_base = PATCH_EFFECT_BASE_UPPER_BOUND;
      }

      if (((first_byte & 0x40) >> 6) != 0)
      {
        p->vibrato_depth = PATCH_EFFECT_DEPTH_LOWER_BOUND + 3;
        p->vibrato_base = PATCH_EFFECT_BASE_UPPER_BOUND;
      }

      if (((first_byte & 0x20) >> 5) != 0)
        p->env_decay_2[current_op] = S_import_opl_env_rate_table[0];

      p->env_rate_ks[current_op] = S_import_opl_rate_ks_table[(first_byte & 0x10) >> 4];
    }

    /* validate the parameters */
    patch_validate_patch(patch_index);
  }

  return 0;
}

/*******************************************************************************
** import_tfi_load()
*******************************************************************************/
short int import_tfi_load(int cart_num, int patch_num, 
                          char* filename, int inst_id, int batching)
{
  int k;
  int m;

  FILE* fp;

  unsigned char patch_data[IMPORT_TFI_MAX_PATCHES][IMPORT_TFI_NUM_BYTES];
  unsigned char first_byte;
  unsigned char second_byte;

  int num_patches;

  patch* p;

  int patch_index;

  short int batch_count;

  short int op_order[4];
  short int current_op;
  short int fb_op;

  /* make sure the cart number is valid */
  if (PATCH_CART_NO_IS_NOT_VALID(cart_num))
    return 0;

  /* make sure the patch number is valid */
  if (PATCH_PATCH_NO_IS_NOT_VALID(patch_num))
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

  /* close .tfi file */
  fclose(fp);

  /* determine batch count */

  /* note that .tfi files should always just have 1 patch.  */
  /* however, we support alternate versions of .tfi files   */
  /* that contain multiple .tfi's concatenated together.    */
  if (batching == IMPORT_BATCHING_1)
    batch_count = 1;
  else if (batching == IMPORT_BATCHING_8)
    batch_count = 8;
  else if (batching == IMPORT_BATCHING_16)
    batch_count = 16;
  else
    batch_count = 1;

  /* load batch */
  for (k = 0; k < batch_count; k++)
  {
    /* make sure the instrument id is valid */
    if (IMPORT_INSTRUMENT_ID_IS_NOT_VALID(inst_id + k))
      break;

    /* make sure the patch number is valid */
    if (PATCH_PATCH_NO_IS_NOT_VALID(patch_num + k))
      break;

    /* load patch data to cart */
    PATCH_COMPUTE_PATCH_INDEX(cart_num, patch_num + k)

    p = &G_patch_bank[patch_index];

    /* reset patch */
    patch_reset_patch(patch_index);

    /* algorithm & feedback */
    first_byte = patch_data[inst_id + k][IMPORT_TFI_COMPUTE_BYTE_GENERAL_INDEX(ALGORITHM)];
    second_byte = patch_data[inst_id + k][IMPORT_TFI_COMPUTE_BYTE_GENERAL_INDEX(FEEDBACK)];

    /* determine operator ordering */
    /* (the order in the file should be S1, S3, S2, S4) */
#if 1
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
#else
    if ((first_byte & 0x07) == 2)
    {
      op_order[0] = 2;
      op_order[1] = 0;
      op_order[2] = 1;
      op_order[3] = 3;

      fb_op = 2;
    }
    else if ((first_byte & 0x07) == 4)
    {
      op_order[0] = 0;
      op_order[1] = 2;
      op_order[2] = 1;
      op_order[3] = 3;

      fb_op = 0;
    }
    else
    {
      op_order[0] = 0;
      op_order[1] = 1;
      op_order[2] = 2;
      op_order[3] = 3;

      fb_op = 0;
    }
#endif

    /* set the algorithm & feedback */
    p->algorithm = S_import_ym2612_algorithm_table[first_byte & 0x07];
    p->osc_feedback[fb_op] = S_import_feedback_table[second_byte & 0x07];

    for (m = 0; m < 4; m++)
    {
      current_op = op_order[m];

      /* multiple */
      first_byte = patch_data[inst_id + k][IMPORT_TFI_COMPUTE_BYTE_OPERATOR_INDEX(m, MULTIPLE)];

      p->osc_freq_mode[current_op] = PATCH_OSC_FREQ_MODE_RATIO;

      p->osc_multiple[current_op] = S_import_ym2612_multiple_table[first_byte & 0x0F];
      p->osc_divisor[current_op] = S_import_divisor_table[first_byte & 0x0F];

      /* detune */
      first_byte = patch_data[inst_id + k][IMPORT_TFI_COMPUTE_BYTE_OPERATOR_INDEX(m, DETUNE)];

      p->osc_detune[current_op] = S_import_ym2612_detune_table[first_byte & 0x07];

      /* total level */
      /* note that if the tl value is >= 64, it is clamped to 63 */
      first_byte = patch_data[inst_id + k][IMPORT_TFI_COMPUTE_BYTE_OPERATOR_INDEX(m, TOTAL_LEVEL)];

      if ((first_byte & 0x7F) > 63)
        p->env_amplitude[current_op] = S_import_env_amplitude_table[63];
      else
        p->env_amplitude[current_op] = S_import_env_amplitude_table[first_byte & 0x3F];

      /* rate keyscaling */
      first_byte = patch_data[inst_id + k][IMPORT_TFI_COMPUTE_BYTE_OPERATOR_INDEX(m, RATE_KEYSCALE)];

      p->env_rate_ks[current_op] = S_import_ym2612_rate_ks_table[first_byte & 0x03];

      /* attack */
      first_byte = patch_data[inst_id + k][IMPORT_TFI_COMPUTE_BYTE_OPERATOR_INDEX(m, ATTACK_RATE)];

      p->env_attack[current_op] = S_import_ym2612_env_rate_table[first_byte & 0x1F];

      /* decay 1 */
      first_byte = patch_data[inst_id + k][IMPORT_TFI_COMPUTE_BYTE_OPERATOR_INDEX(m, DECAY_1_RATE)];

      p->env_decay_1[current_op] = S_import_ym2612_env_rate_table[first_byte & 0x1F];

      /* decay 2 */
      first_byte = patch_data[inst_id + k][IMPORT_TFI_COMPUTE_BYTE_OPERATOR_INDEX(m, DECAY_2_RATE)];

      p->env_decay_2[current_op] = S_import_ym2612_env_rate_table[first_byte & 0x1F];

      /* release */
      first_byte = patch_data[inst_id + k][IMPORT_TFI_COMPUTE_BYTE_OPERATOR_INDEX(m, RELEASE_RATE)];

      p->env_release[current_op] = S_import_opl_env_rate_table[first_byte & 0x0F];

      /* sustain level */
      first_byte = patch_data[inst_id + k][IMPORT_TFI_COMPUTE_BYTE_OPERATOR_INDEX(m, SUSTAIN_LEVEL)];

      p->env_sustain[current_op] = S_import_env_sustain_table[first_byte & 0x0F];
    }

    /* validate the parameters */
    patch_validate_patch(patch_index);
  }

  return 0;
}

/*******************************************************************************
** import_opm_load()
*******************************************************************************/
short int import_opm_load(int cart_num, int patch_num, 
                          char* filename, int inst_id, int batching)
{
  int k;
  int m;

  FILE* fp;

  short int patch_data[IMPORT_OPM_MAX_PATCHES][IMPORT_OPM_NUM_VALUES];
  char      name_data[IMPORT_OPM_MAX_PATCHES][PATCH_PATCH_NAME_SIZE];

  char      text_line[IMPORT_OPM_TEXT_LINE_MAX_LENGTH];

  short int line_pos;
  short int token_size;

  short int num_lines;

  int num_patches;

  patch* p;

  int patch_index;

  short int batch_count;

  short int op_order[4];
  short int current_op;
  short int fb_op;

  short int val;

  /* make sure the cart number is valid */
  if (PATCH_CART_NO_IS_NOT_VALID(cart_num))
    return 0;

  /* make sure the patch number is valid */
  if (PATCH_PATCH_NO_IS_NOT_VALID(patch_num))
    return 0;

  /* make sure filename is valid */
  if (filename == NULL)
    return 0;

  /* open .opm file */
  fp = fopen(filename, "rt");

  /* if file did not open, return */
  if (fp == NULL)
    return 0;

  /* read patch data */
  num_patches = 0;
  num_lines = 0;

  while (fgets(&text_line[0], IMPORT_OPM_TEXT_LINE_MAX_LENGTH, fp) != NULL)
  {
    line_pos = 0;

    IMPORT_OPM_PARSE_SKIP_SPACES()

    /* skip comments */
    if (!strncmp(&text_line[line_pos], "//", 2))
      continue;

    /* check if we have read a whole patch (7 lines) */
    if (num_lines >= 7)
    {
      num_lines = 0;
      num_patches += 1;
    }

    /* patch id and name */
    if (!strncmp(&text_line[line_pos], "@:", 2))
    {
      line_pos += 2;

      /* read patch id token */
      IMPORT_OPM_PARSE_SCAN_INTEGER()

      /* ignore the patch id for now */
      IMPORT_OPM_ADVANCE_OVER_TOKEN()

      /* read patch name */
      IMPORT_OPM_PARSE_SKIP_SPACES()
      IMPORT_OPM_PARSE_SCAN_ALPHANUMERIC_STRING()

      /* initialize patch name */
      for (m = 0; m < PATCH_PATCH_NAME_SIZE; m++)
        name_data[num_patches][m] = '\0';

      /* copy patch name */
      if (token_size < PATCH_PATCH_NAME_SIZE)
        strncpy(&name_data[num_patches][0], &text_line[line_pos], token_size);
      else
        strncpy(&name_data[num_patches][0], &text_line[line_pos], PATCH_PATCH_NAME_SIZE);

      num_lines += 1;
    }
    /* lfo line */
    else if (!strncmp(&text_line[line_pos], "LFO:", 4))
    {
      line_pos += 4;

      /* read values */
      IMPORT_OPM_READ_GENERAL_VALUE(LFO_FREQUENCY)
      IMPORT_OPM_READ_GENERAL_VALUE(TREMOLO_BASE)
      IMPORT_OPM_READ_GENERAL_VALUE(VIBRATO_BASE)
      IMPORT_OPM_READ_GENERAL_VALUE(LFO_WAVEFORM)
      IMPORT_OPM_READ_GENERAL_VALUE(NOISE_FREQUENCY)

      num_lines += 1;
    }
    /* ch line */
    else if (!strncmp(&text_line[line_pos], "CH:", 3))
    {
      line_pos += 3;

      /* read values */
      IMPORT_OPM_READ_GENERAL_VALUE(PANNING)
      IMPORT_OPM_READ_GENERAL_VALUE(FEEDBACK)
      IMPORT_OPM_READ_GENERAL_VALUE(ALGORITHM)
      IMPORT_OPM_READ_GENERAL_VALUE(TREMOLO_DEPTH)
      IMPORT_OPM_READ_GENERAL_VALUE(VIBRATO_DEPTH)
      IMPORT_OPM_READ_GENERAL_VALUE(SLOT_MASK)
      IMPORT_OPM_READ_GENERAL_VALUE(NOISE_ENABLE)

      num_lines += 1;
    }
    /* op line */
    else if ( (!strncmp(&text_line[line_pos], "M1:", 3)) || 
              (!strncmp(&text_line[line_pos], "C1:", 3)) || 
              (!strncmp(&text_line[line_pos], "M2:", 3)) || 
              (!strncmp(&text_line[line_pos], "C2:", 3)))
    {
      if (!strncmp(&text_line[line_pos], "M1:", 3))
        current_op = 0;
      else if (!strncmp(&text_line[line_pos], "C1:", 3))
        current_op = 1;
      else if (!strncmp(&text_line[line_pos], "M2:", 3))
        current_op = 2;
      else
        current_op = 3;

      line_pos += 3;

      /* read values */
      IMPORT_OPM_READ_OPERATOR_VALUE(current_op, ATTACK_RATE)
      IMPORT_OPM_READ_OPERATOR_VALUE(current_op, DECAY_1_RATE)
      IMPORT_OPM_READ_OPERATOR_VALUE(current_op, DECAY_2_RATE)
      IMPORT_OPM_READ_OPERATOR_VALUE(current_op, RELEASE_RATE)
      IMPORT_OPM_READ_OPERATOR_VALUE(current_op, SUSTAIN_LEVEL)
      IMPORT_OPM_READ_OPERATOR_VALUE(current_op, TOTAL_LEVEL)
      IMPORT_OPM_READ_OPERATOR_VALUE(current_op, RATE_KEYSCALE)
      IMPORT_OPM_READ_OPERATOR_VALUE(current_op, MULTIPLE)
      IMPORT_OPM_READ_OPERATOR_VALUE(current_op, DETUNE_1)
      IMPORT_OPM_READ_OPERATOR_VALUE(current_op, DETUNE_2)
      IMPORT_OPM_READ_OPERATOR_VALUE(current_op, TREMOLO_ENABLE)

      num_lines += 1;
    }
  }

  /* close .opm file */
  fclose(fp);

  /* determine batch count */
  if (batching == IMPORT_BATCHING_1)
    batch_count = 1;
  else if (batching == IMPORT_BATCHING_8)
    batch_count = 8;
  else if (batching == IMPORT_BATCHING_16)
    batch_count = 16;
  else
    batch_count = 1;

  /* load batch */
  for (k = 0; k < batch_count; k++)
  {
    /* make sure the instrument id is valid */
    if (IMPORT_INSTRUMENT_ID_IS_NOT_VALID(inst_id + k))
      break;

    /* make sure the patch number is valid */
    if (PATCH_PATCH_NO_IS_NOT_VALID(patch_num + k))
      break;

    /* load patch data to cart */
    PATCH_COMPUTE_PATCH_INDEX(cart_num, patch_num + k)

    p = &G_patch_bank[patch_index];

    /* reset patch */
    patch_reset_patch(patch_index);

    /* patch name */
    strncpy(&G_patch_names[patch_index][0], &name_data[inst_id + k][0], PATCH_PATCH_NAME_SIZE);

    /* determine operator ordering */
    if (patch_data[inst_id + k][IMPORT_OPM_COMPUTE_VALUE_GENERAL_INDEX(ALGORITHM)] == 2)
    {
      op_order[0] = 2;
      op_order[1] = 0;
      op_order[2] = 1;
      op_order[3] = 3;

      fb_op = 2;
    }
    else if (patch_data[inst_id + k][IMPORT_OPM_COMPUTE_VALUE_GENERAL_INDEX(ALGORITHM)] == 4)
    {
      op_order[0] = 0;
      op_order[1] = 2;
      op_order[2] = 1;
      op_order[3] = 3;

      fb_op = 0;
    }
    else
    {
      op_order[0] = 0;
      op_order[1] = 1;
      op_order[2] = 2;
      op_order[3] = 3;

      fb_op = 0;
    }

    /* algorithm */
    val = patch_data[inst_id + k][IMPORT_OPM_COMPUTE_VALUE_GENERAL_INDEX(ALGORITHM)];

    if ((val >= 0) && (val < IMPORT_YM2151_ALGORITHM_NUM_VALUES))
      p->algorithm = S_import_ym2612_algorithm_table[val];

    /* feedback */
    val = patch_data[inst_id + k][IMPORT_OPM_COMPUTE_VALUE_GENERAL_INDEX(FEEDBACK)];

    if ((val >= 0) && (val < IMPORT_YM2151_FEEDBACK_NUM_VALUES))
      p->osc_feedback[fb_op] = S_import_feedback_table[val];

    /* lfo waveform */
    val = patch_data[inst_id + k][IMPORT_OPM_COMPUTE_VALUE_GENERAL_INDEX(LFO_WAVEFORM)];

    if ((val >= 0) && (val < IMPORT_YM2151_LFO_WAVEFORM_NUM_VALUES))
      p->lfo_waveform = S_import_ym2151_lfo_waveform_table[val];

    /* tremolo depth */
    val = patch_data[inst_id + k][IMPORT_OPM_COMPUTE_VALUE_GENERAL_INDEX(TREMOLO_DEPTH)];

    if ((val >= 0) && (val < IMPORT_YM2151_TREMOLO_DEPTH_NUM_VALUES))
      p->tremolo_depth = S_import_ym2151_tremolo_depth_table[val];

    /* vibrato depth */
    val = patch_data[inst_id + k][IMPORT_OPM_COMPUTE_VALUE_GENERAL_INDEX(VIBRATO_DEPTH)];

    if ((val >= 0) && (val < IMPORT_YM2151_VIBRATO_DEPTH_NUM_VALUES))
      p->vibrato_depth = S_import_ym2151_vibrato_depth_table[val];

    /* tremolo base */
    val = patch_data[inst_id + k][IMPORT_OPM_COMPUTE_VALUE_GENERAL_INDEX(TREMOLO_BASE)];

    if ((val >= 0) && (val < IMPORT_YM2151_EFFECT_BASE_NUM_VALUES))
      p->tremolo_base = S_import_ym2151_effect_base_table[val / IMPORT_YM2151_EFFECT_BASE_DIVISOR];

    /* vibrato base */
    val = patch_data[inst_id + k][IMPORT_OPM_COMPUTE_VALUE_GENERAL_INDEX(VIBRATO_BASE)];

    if ((val >= 0) && (val < IMPORT_YM2151_EFFECT_BASE_NUM_VALUES))
      p->vibrato_base = S_import_ym2151_effect_base_table[val / IMPORT_YM2151_EFFECT_BASE_DIVISOR];

    /* noise enable */
    val = patch_data[inst_id + k][IMPORT_OPM_COMPUTE_VALUE_GENERAL_INDEX(NOISE_ENABLE)];

    if (val > 0)
      p->noise_mode = PATCH_NOISE_MODE_SQUARE;

    for (m = 0; m < 4; m++)
    {
      current_op = op_order[m];

      /* multiple */
      val = patch_data[inst_id + k][IMPORT_OPM_COMPUTE_VALUE_OPERATOR_INDEX(m, MULTIPLE)];

      if ((val >= 0) && (val < IMPORT_YM2151_MULTIPLE_NUM_VALUES))
      {
        p->osc_multiple[current_op] = S_import_ym2612_multiple_table[val];
        p->osc_divisor[current_op] = S_import_divisor_table[val];
      }

      /* detune 1 */
      val = patch_data[inst_id + k][IMPORT_OPM_COMPUTE_VALUE_OPERATOR_INDEX(m, DETUNE_1)];

      if ((val >= 0) && (val < IMPORT_YM2151_DETUNE_1_NUM_VALUES))
        p->env_rate_ks[current_op] = S_import_ym2151_detune_table[val];

      /* attack rate */
      val = patch_data[inst_id + k][IMPORT_OPM_COMPUTE_VALUE_OPERATOR_INDEX(m, ATTACK_RATE)];

      if ((val >= 0) && (val < IMPORT_YM2151_ENV_RATE_NUM_VALUES))
        p->env_attack[current_op] = S_import_ym2612_env_rate_table[val];

      /* decay 1 rate */
      val = patch_data[inst_id + k][IMPORT_OPM_COMPUTE_VALUE_OPERATOR_INDEX(m, DECAY_1_RATE)];

      if ((val >= 0) && (val < IMPORT_YM2151_ENV_RATE_NUM_VALUES))
        p->env_decay_1[current_op] = S_import_ym2612_env_rate_table[val];

      /* decay 2 rate */
      val = patch_data[inst_id + k][IMPORT_OPM_COMPUTE_VALUE_OPERATOR_INDEX(m, DECAY_2_RATE)];

      if ((val >= 0) && (val < IMPORT_YM2151_ENV_RATE_NUM_VALUES))
        p->env_decay_2[current_op] = S_import_ym2612_env_rate_table[val];

      /* release rate */
      val = patch_data[inst_id + k][IMPORT_OPM_COMPUTE_VALUE_OPERATOR_INDEX(m, RELEASE_RATE)];

      if ((val >= 0) && (val < IMPORT_OPL_ENV_RATE_NUM_VALUES))
        p->env_release[current_op] = S_import_opl_env_rate_table[val];

      /* total level */
      val = patch_data[inst_id + k][IMPORT_OPM_COMPUTE_VALUE_OPERATOR_INDEX(m, TOTAL_LEVEL)];

      if ((val >= 0) && (val < IMPORT_YM2151_TOTAL_LEVEL_NUM_VALUES))
      {
        if (val > 63)
          p->env_amplitude[current_op] = S_import_env_amplitude_table[63];
        else
          p->env_amplitude[current_op] = S_import_env_amplitude_table[val];
      }

      /* sustain level */
      val = patch_data[inst_id + k][IMPORT_OPM_COMPUTE_VALUE_OPERATOR_INDEX(m, SUSTAIN_LEVEL)];

      if ((val >= 0) && (val < IMPORT_YM2612_SUSTAIN_LEVEL_NUM_VALUES))
        p->env_sustain[current_op] = S_import_env_sustain_table[val];

      /* rate keyscaling */
      val = patch_data[inst_id + k][IMPORT_OPM_COMPUTE_VALUE_OPERATOR_INDEX(m, RATE_KEYSCALE)];

      if ((val >= 0) && (val < IMPORT_YM2612_RATE_KEYSCALE_NUM_VALUES))
        p->env_rate_ks[current_op] = S_import_ym2612_rate_ks_table[val];
    }

    /* validate the parameters */
    patch_validate_patch(patch_index);
  }

  return 0;
}

