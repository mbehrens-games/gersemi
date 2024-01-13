/*******************************************************************************
** fileio.h (loading and saving from native file formats)
*******************************************************************************/

#ifndef FILEIO_H
#define FILEIO_H

#include "cart.h"

enum
{
  FILEIO_PATCH_BYTE_GENERAL_ALGORITHM_VELOCITY = 0,           /* algorithm (3 bits), velocity mode (1 bit), scaling (4 bits) */
  FILEIO_PATCH_BYTE_GENERAL_NOISE,                            /* noise mode (2 bits), noise frequency (5 bits) */
  FILEIO_PATCH_BYTE_GENERAL_PORTAMENTO,                       /* portamento mode (1 bit), legato (1 bit), speed (4 bits) */
  FILEIO_PATCH_BYTE_GENERAL_FILTER_CUTOFFS_SUSTAIN_PEDAL,     /* lowpass (2 bits), highpass (2 bits), pedal adjust (4 bits) */
  FILEIO_PATCH_BYTE_GENERAL_SYNC_PITCH_WHEEL,                 /* osc sync (1 bit), lfo sync (1 bit), pitch wheel mode (1 bit), range (4 bits) */
  FILEIO_PATCH_BYTE_GENERAL_VIBRATO_TREMOLO_DEPTHS,           /* depths (8 bits, 4 bits each) */
  FILEIO_PATCH_BYTE_GENERAL_EFFECT_MODES_BOOST_DEPTH,         /* modes (3 bits, 1 bit each), depth (4 bits) */
  FILEIO_PATCH_BYTE_GENERAL_MOD_WHEEL_EFFECT_VIBRATO_BASE,    /* effect (2 bits), base (5 bits) */
  FILEIO_PATCH_BYTE_GENERAL_AFTERTOUCH_EFFECT_TREMOLO_BASE,   /* effect (2 bits), base (5 bits) */
  FILEIO_PATCH_BYTE_GENERAL_LFO_WAVEFORM_FREQUENCY,           /* lfo waveform (3 bits), lfo frequency (5 bits) */
  FILEIO_PATCH_BYTE_GENERAL_LFO_DELAY,                        /* lfo delay (5 bits) */
  FILEIO_PATCH_BYTE_GENERAL_LFO_QUANTIZE,                     /* lfo quantize (5 bits) */
  FILEIO_PATCH_NUM_GENERAL_BYTES 
};

enum
{
  FILEIO_PATCH_BYTE_OSC_ENV_WAVEFORM_FEEDBACK = 0,            /* waveform (3 bits), feedback (3 bits) */
  FILEIO_PATCH_BYTE_OSC_ENV_MULTIPLE_DIVISOR_OR_NOTE_OCTAVE,  /* multiple/note (4 bits), divisor/octave (4 bits) */
  FILEIO_PATCH_BYTE_OSC_ENV_DETUNE,                           /* detune (6 bits) */
  FILEIO_PATCH_BYTE_OSC_ENV_PHI_FREQ_MODE_BREAK_POINT,        /* phi (2 bits), freq mode (1 bit), break point (4 bits) */
  FILEIO_PATCH_BYTE_OSC_ENV_ATTACK_RATE_KS,                   /* attack (5 bits), rate keyscaling (3 bits) */
  FILEIO_PATCH_BYTE_OSC_ENV_DECAY_1_LEVEL_KS,                 /* decay 1 (5 bits), level keyscaling (3 bits) */
  FILEIO_PATCH_BYTE_OSC_ENV_DECAY_2,                          /* decay 2 (5 bits) */
  FILEIO_PATCH_BYTE_OSC_ENV_RELEASE,                          /* release (5 bits) */
  FILEIO_PATCH_BYTE_OSC_ENV_AMPLITUDE,                        /* amplitude (6 bits) */
  FILEIO_PATCH_BYTE_OSC_ENV_SUSTAIN,                          /* sustain (5 bits) */
  FILEIO_PATCH_NUM_OSC_ENV_BYTES
};

#define FILEIO_PATCH_NUM_BYTES  ( FILEIO_PATCH_NUM_GENERAL_BYTES +             \
                                  (4 * FILEIO_PATCH_NUM_OSC_ENV_BYTES))

#define FILEIO_PATCH_GENERAL_START_INDEX  0
#define FILEIO_PATCH_GENERAL_END_INDEX    ( FILEIO_PATCH_GENERAL_START_INDEX + \
                                            FILEIO_PATCH_NUM_GENERAL_BYTES)

#define FILEIO_PATCH_OSC_ENV_START_INDEX  FILEIO_PATCH_GENERAL_END_INDEX
#define FILEIO_PATCH_OSC_ENV_END_INDEX    ( FILEIO_PATCH_OSC_ENV_START_INDEX + \
                                            4 * FILEIO_PATCH_NUM_OSC_ENV_BYTES)

#define FILEIO_CART_NUM_BYTES   (FILEIO_PATCH_NUM_BYTES * CART_PATCHES_PER_CART)

/* function declarations */
short int fileio_cart_load(int cart_num, char* filename);
short int fileio_cart_save(int cart_num, char* filename);

#endif
