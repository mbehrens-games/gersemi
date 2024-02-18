/*******************************************************************************
** fileio.h (loading and saving from native file formats)
*******************************************************************************/

#ifndef FILEIO_H
#define FILEIO_H

#include "bank.h"

/* 14 bytes */
enum
{
  FILEIO_PATCH_BYTE_GENERAL_ALGORITHM_VIBRATO_DEPTH = 0,      /* algorithm (3 bits), depth (4 bits) */
  FILEIO_PATCH_BYTE_GENERAL_SYNC_TREMOLO_DEPTH,               /* osc sync (1 bit), lfo sync (1 bit), arpeggio sync (1 bit), depth (4 bits) */
  FILEIO_PATCH_BYTE_GENERAL_EFFECT_MODES_BOOST_DEPTH,         /* modes (3 bits, 1 bit each), depth (4 bits) */
  FILEIO_PATCH_BYTE_GENERAL_MOD_WHEEL_EFFECT_VIBRATO_BASE,    /* effect (2 bits), base (5 bits) */
  FILEIO_PATCH_BYTE_GENERAL_AFTERTOUCH_EFFECT_TREMOLO_BASE,   /* effect (2 bits), base (5 bits) */
  FILEIO_PATCH_BYTE_GENERAL_VELOCITY,                         /* velocity mode (1 bit), scaling (4 bits) */
  FILEIO_PATCH_BYTE_GENERAL_NOISE,                            /* noise mode (2 bits), noise frequency (5 bits) */
  FILEIO_PATCH_BYTE_GENERAL_PORTAMENTO,                       /* portamento mode (1 bit), legato (1 bit), speed (4 bits) */
  FILEIO_PATCH_BYTE_GENERAL_PITCH_WHEEL,                      /* pitch wheel mode (1 bit), range (4 bits) */
  FILEIO_PATCH_BYTE_GENERAL_ARPEGGIO,                         /* arpeggio mode (1 bit), pattern (3 bits), speed (4 bits) */
  FILEIO_PATCH_BYTE_GENERAL_FILTER_CUTOFFS_SUSTAIN_PEDAL,     /* lowpass (2 bits), highpass (2 bits), pedal adjust (4 bits) */
  FILEIO_PATCH_BYTE_GENERAL_LFO_WAVEFORM_DELAY,               /* lfo waveform (3 bits), lfo delay (5 bits) */
  FILEIO_PATCH_BYTE_GENERAL_LFO_FREQUENCY,                    /* lfo frequency (5 bits) */
  FILEIO_PATCH_BYTE_GENERAL_LFO_QUANTIZE,                     /* lfo quantize (5 bits) */
  FILEIO_PATCH_NUM_GENERAL_BYTES 
};

/* 9 bytes */
enum
{
  FILEIO_PATCH_BYTE_OSC_ENV_ATTACK_WAVEFORM = 0,              /* attack (5 bits), waveform (3 bits) */
  FILEIO_PATCH_BYTE_OSC_ENV_DECAY_1_FEEDBACK,                 /* decay 1 (5 bits), feedback (3 bits) */
  FILEIO_PATCH_BYTE_OSC_ENV_DECAY_2_RATE_KS,                  /* decay 2 (5 bits), rate keyscaling (3 bits) */
  FILEIO_PATCH_BYTE_OSC_ENV_RELEASE_LEVEL_KS,                 /* release (5 bits), level keyscaling (3 bits) */
  FILEIO_PATCH_BYTE_OSC_ENV_PHI_BREAK_POINT,                  /* phi (4 bits), break point (4 bits) */
  FILEIO_PATCH_BYTE_OSC_ENV_AMPLITUDE,                        /* amplitude (7 bits) */
  FILEIO_PATCH_BYTE_OSC_ENV_SUSTAIN,                          /* sustain (5 bits) */
  FILEIO_PATCH_BYTE_OSC_ENV_MULTIPLE_DIVISOR_OR_NOTE_OCTAVE,  /* multiple/note (4 bits), divisor/octave (4 bits) */
  FILEIO_PATCH_BYTE_OSC_ENV_FREQ_MODE_DETUNE,                 /* freq mode (1 bit), detune (7 bits) */
  FILEIO_PATCH_NUM_OSC_ENV_BYTES
};

/* 10 bytes */
#define FILEIO_PATCH_NAME_SIZE  12

/* 10 bytes */
#define FILEIO_CART_NAME_SIZE   12

/* should be 12 + 14 + (4 * 9) = 62 bytes */
#define FILEIO_PATCH_NUM_BYTES  ( FILEIO_PATCH_NAME_SIZE +                     \
                                  FILEIO_PATCH_NUM_GENERAL_BYTES +             \
                                  (4 * FILEIO_PATCH_NUM_OSC_ENV_BYTES))

#define FILEIO_PATCH_GENERAL_START_INDEX  FILEIO_PATCH_NAME_SIZE
#define FILEIO_PATCH_GENERAL_END_INDEX    ( FILEIO_PATCH_GENERAL_START_INDEX + \
                                            FILEIO_PATCH_NUM_GENERAL_BYTES)

#define FILEIO_PATCH_OSC_ENV_START_INDEX  FILEIO_PATCH_GENERAL_END_INDEX
#define FILEIO_PATCH_OSC_ENV_END_INDEX    ( FILEIO_PATCH_OSC_ENV_START_INDEX + \
                                            4 * FILEIO_PATCH_NUM_OSC_ENV_BYTES)

/* should be 12 + (16 * 62) = 1004 bytes */
#define FILEIO_CART_NUM_BYTES   ( FILEIO_CART_NAME_SIZE +                      \
                                  (FILEIO_PATCH_NUM_BYTES * BANK_PATCHES_PER_CART))

/* function declarations */
short int fileio_cart_load(int cart_num, char* filename);
short int fileio_cart_save(int cart_num, char* filename);

#endif
