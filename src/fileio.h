/*******************************************************************************
** fileio.h (loading and saving from native file formats)
*******************************************************************************/

#ifndef FILEIO_H
#define FILEIO_H

#include "bank.h"

/* 17 bytes */
enum
{
  FILEIO_PATCH_BYTE_GENERAL_MOD_WHEEL_EFFECT_LFO_DELAY = 0,   /* effect (3 bits), lfo delay (5 bits) */
  FILEIO_PATCH_BYTE_GENERAL_AFTERTOUCH_EFFECT_LFO_FREQUENCY,  /* effect (3 bits), lfo frequency (5 bits) */
  FILEIO_PATCH_BYTE_GENERAL_EXP_PEDAL_EFFECT_LFO_QUANTIZE,    /* effect (3 bits), lfo quantize (5 bits) */
  FILEIO_PATCH_BYTE_GENERAL_LFO_DELAY_VIBRATO_BASE,           /* lfo waveform (3 bits), base (5 bits) */
  FILEIO_PATCH_BYTE_GENERAL_ALGORITHM_TREMOLO_BASE,           /* algorithm (3 bits), base (5 bits) */
  FILEIO_PATCH_BYTE_GENERAL_EFFECT_MODES_ARPEGGIO_STYLE,      /* modes (3 bits total), arpeggio mode (1 bit), pattern (2 bits), octaves (2 bits) */
  FILEIO_PATCH_BYTE_GENERAL_ARPEGGIO_SPEED_VIBRATO_DEPTH,     /* arpeggio speed (4 bits), depth (4 bits) */
  FILEIO_PATCH_BYTE_GENERAL_TREMOLO_BOOST_DEPTH,              /* depths (8 bits total) */
  FILEIO_PATCH_BYTE_GENERAL_OSC_SYNC_LOWPASS_PEG_ATTACK,      /* osc sync (1 bit), lowpass (2 bits), peg attack (5 bits) */
  FILEIO_PATCH_BYTE_GENERAL_LFO_SYNC_HIGHPASS_PEG_DECAY,      /* lfo sync (1 bit), highpass (2 bits), peg decay (5 bits) */
  FILEIO_PATCH_BYTE_GENERAL_PORTAMENTO_STYLE_PEG_RELEASE,     /* portamento mode (1 bit), follow (1 bit), legato (1 bit), peg release (5 bits) */
  FILEIO_PATCH_BYTE_GENERAL_PORTAMENTO_SPEED_SUSTAIN_PEDAL,   /* portamento speed (4 bits), pedal adjust (4 bits) */
  FILEIO_PATCH_BYTE_GENERAL_PEG_STYLE_NOISE_FREQUENCY,        /* peg transpose (1 bit), legato (1 bit), noise frequency (5 bits) */
  FILEIO_PATCH_BYTE_GENERAL_NOISE_MODE_PEG_AMPLITUDE,         /* noise mode (2 bits), peg amplitude (6 bits) */
  FILEIO_PATCH_BYTE_GENERAL_LEGACY_PEG_SUSTAIN,               /* legacy (2 bits total), peg sustain (6 bits) */
  FILEIO_PATCH_BYTE_GENERAL_VELOCITY_BENDER_MODES_PEG_FINALE, /* velocity mode (1 bit), pitch wheel mode (1 bit), peg finale (6 bits) */
  FILEIO_PATCH_BYTE_GENERAL_VELOCITY_SCALING_BENDER_RANGE,    /* scaling (4 bits), range (4 bits) */
  FILEIO_PATCH_NUM_GENERAL_BYTES 
};

/* 9 bytes */
enum
{
  FILEIO_PATCH_BYTE_OSC_ENV_ATTACK_WAVEFORM = 0,  /* waveform (3 bits), attack (5 bits) */
  FILEIO_PATCH_BYTE_OSC_ENV_DECAY_1_FEEDBACK,     /* feedback (3 bits), decay 1 (5 bits) */
  FILEIO_PATCH_BYTE_OSC_ENV_DECAY_2_RATE_KS,      /* rate keyscaling (3 bits), decay 2 (5 bits) */
  FILEIO_PATCH_BYTE_OSC_ENV_RELEASE_LEVEL_KS,     /* level keyscaling (3 bits), release (5 bits) */
  FILEIO_PATCH_BYTE_OSC_ENV_PHI_BREAK_POINT,      /* phi (4 bits), break point (4 bits) */
  FILEIO_PATCH_BYTE_OSC_ENV_MULTIPLE_DIVISOR,     /* multiple (4 bits), divisor (4 bits) */
  FILEIO_PATCH_BYTE_OSC_ENV_DETUNE,               /* detune (7 bits) */
  FILEIO_PATCH_BYTE_OSC_ENV_AMPLITUDE,            /* amplitude (7 bits) */
  FILEIO_PATCH_BYTE_OSC_ENV_FREQ_MODE_SUSTAIN,    /* frequency mode (1 bit), sustain (5 bits) */
  FILEIO_PATCH_NUM_OSC_ENV_BYTES
};

/* 10 bytes */
#define FILEIO_PATCH_NAME_BYTES 10
#define FILEIO_PATCH_NAME_SIZE  (FILEIO_PATCH_NAME_BYTES + 1)

/* 8 bytes */
#define FILEIO_CART_NAME_BYTES  8
#define FILEIO_CART_NAME_SIZE   (FILEIO_CART_NAME_BYTES + 1)

/* should be 10 + 17 + (4 * 9) = 63 bytes */
#define FILEIO_PATCH_NUM_BYTES  ( FILEIO_PATCH_NAME_SIZE +                     \
                                  FILEIO_PATCH_NUM_GENERAL_BYTES +             \
                                  (4 * FILEIO_PATCH_NUM_OSC_ENV_BYTES))

#define FILEIO_PATCH_GENERAL_START_INDEX  FILEIO_PATCH_NAME_SIZE
#define FILEIO_PATCH_GENERAL_END_INDEX    ( FILEIO_PATCH_GENERAL_START_INDEX + \
                                            FILEIO_PATCH_NUM_GENERAL_BYTES)

#define FILEIO_PATCH_OSC_ENV_START_INDEX  FILEIO_PATCH_GENERAL_END_INDEX
#define FILEIO_PATCH_OSC_ENV_END_INDEX    ( FILEIO_PATCH_OSC_ENV_START_INDEX + \
                                            4 * FILEIO_PATCH_NUM_OSC_ENV_BYTES)

/* should be 8 + (16 * 63) = 1016 bytes */
#define FILEIO_CART_NUM_BYTES   ( FILEIO_CART_NAME_SIZE +                      \
                                  (FILEIO_PATCH_NUM_BYTES * BANK_PATCHES_PER_CART))

/* function declarations */
short int fileio_cart_load(int cart_num, char* filename);
short int fileio_cart_save(int cart_num, char* filename);

#endif
