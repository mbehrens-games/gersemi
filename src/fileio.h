/*******************************************************************************
** fileio.h (loading and saving from native file formats)
*******************************************************************************/

#ifndef FILEIO_H
#define FILEIO_H

#include "bank.h"

/* 5 bytes */
enum
{
  FILEIO_PATCH_BYTE_OSC_SYNC_DETUNE_1 = 0,  /* osc sync (1 bit), osc 1 detune (7 bits) */
  FILEIO_PATCH_BYTE_OSC_SPECIAL_1,          /* depends on this voice's program (1-8 bits) */
  FILEIO_PATCH_BYTE_OSC_SPECIAL_2,          /* depends on this voice's program (1-8 bits) */
  FILEIO_PATCH_BYTE_OSC_SPECIAL_3,          /* depends on this voice's program (1-8 bits) */
  FILEIO_PATCH_BYTE_OSC_SPECIAL_4,          /* depends on this voice's program (1-8 bits) */
  FILEIO_PATCH_NUM_OSC_BYTES
};

/* 8 bytes */
enum
{
  FILEIO_PATCH_BYTE_AMP_ENV_ATTACK = 0,           /* attack (7 bits) */
  FILEIO_PATCH_BYTE_AMP_ENV_DECAY_1,              /* decay 1 (7 bits) */
  FILEIO_PATCH_BYTE_AMP_ENV_DECAY_2,              /* decay 2 (7 bits) */
  FILEIO_PATCH_BYTE_AMP_ENV_RELEASE,              /* release (7 bits) */
  FILEIO_PATCH_BYTE_AMP_ENV_AMPLITUDE,            /* amplitude (7 bits) */
  FILEIO_PATCH_BYTE_AMP_ENV_SUSTAIN,              /* sustain (7 bits) */
  FILEIO_PATCH_BYTE_AMP_ENV_LEVEL_KS,             /* level keyscaling left (4 bits), right (4 bits) */
  FILEIO_PATCH_BYTE_AMP_ENV_RATE_KS_BREAK_POINT,  /* rate keyscaling (4 bits), break point (4 bits) */
  FILEIO_PATCH_NUM_AMP_ENV_BYTES
};

/* 8 bytes */
enum
{
  FILEIO_PATCH_BYTE_PITCH_ENV_ATTACK = 0, /* attack (7 bits) */
  FILEIO_PATCH_BYTE_PITCH_ENV_DECAY_1,    /* decay 1 (7 bits) */
  FILEIO_PATCH_BYTE_PITCH_ENV_DECAY_2,    /* decay 2 (7 bits) */
  FILEIO_PATCH_BYTE_PITCH_ENV_RELEASE,    /* release (7 bits) */
  FILEIO_PATCH_BYTE_PITCH_ENV_AMPLITUDE,  /* amplitude (7 bits) */
  FILEIO_PATCH_BYTE_PITCH_ENV_SUSTAIN,    /* sustain (7 bits) */
  FILEIO_PATCH_BYTE_PITCH_ENV_HOLD,       /* hold (7 bits) */
  FILEIO_PATCH_BYTE_PITCH_ENV_FINALE,     /* finale (7 bits) */
  FILEIO_PATCH_NUM_PITCH_ENV_BYTES
};

/* 23 bytes */
enum
{
  FILEIO_PATCH_BYTE_GENERAL_PROGRAM_SUSTAIN_PEDAL = 0,    /* program (3 bits), pedal adjust (4 bits) */
  FILEIO_PATCH_BYTE_GENERAL_HIGHPASS_LOWPASS_KEYTRACKING, /* highpass cutoff (3 bits), lowpass keytracking (4 bits) */
  FILEIO_PATCH_BYTE_GENERAL_LOWPASS_CUTOFF,               /* lowpass cutoff (7 bits) */
  FILEIO_PATCH_BYTE_GENERAL_ARPEGGIO,                     /* arpeggio pattern (2 bits), octaves (2 bits), speed (4 bits) */
  FILEIO_PATCH_BYTE_GENERAL_PORTAMENTO,                   /* portamento mode (1 bit), follow (1 bit), legato (1 bit), speed (4 bits) */
  FILEIO_PATCH_BYTE_GENERAL_LFO_WAVEFORM_DELAY,           /* lfo waveform (3 bits), lfo delay (5 bits) */
  FILEIO_PATCH_BYTE_GENERAL_LFO_SYNC_POLARITY_FREQUENCY,  /* lfo sync (1 bit), polarity (1 bit), frequency (5 bits) */
  FILEIO_PATCH_BYTE_GENERAL_LFO_QUANTIZE,                 /* lfo quantize (5 bits) */
  FILEIO_PATCH_BYTE_GENERAL_VIBRATO,                      /* vibrato mode (1 bit), base (7 bits) */
  FILEIO_PATCH_BYTE_GENERAL_TREMOLO,                      /* tremolo mode (1 bit), base (7 bits) */
  FILEIO_PATCH_BYTE_GENERAL_MOD_WHEEL,                    /* mod wheel routing (8 bits) */
  FILEIO_PATCH_BYTE_GENERAL_AFTERTOUCH,                   /* aftertouch routing (8 bits) */
  FILEIO_PATCH_BYTE_GENERAL_EXPRESSION_PEDAL,             /* exp pedal routing (8 bits) */

  FILEIO_PATCH_BYTE_GENERAL_VIBRATO_TREMOLO_DEPTH,        /* depths (4 bits each, 8 bits total) */
  FILEIO_PATCH_BYTE_GENERAL_BOOST_DEPTH,                  /* depth (4 bits each) */

  FILEIO_PATCH_BYTE_GENERAL_BITCRUSH,                     /*  */
  FILEIO_PATCH_BYTE_GENERAL_LEGATO,                       /*  */

  FILEIO_PATCH_BYTE_GENERAL_NOISE_MODE_FREQUENCY,         /* noise mode (2 bits), frequency (5 bits) */
  FILEIO_PATCH_BYTE_GENERAL_NOISE_GATE,                   /* noise gate type (1 bit), delay (5 bits) */
  FILEIO_PATCH_BYTE_GENERAL_VELOCITY,                     /* velocity mode (1 bit), scaling (4 bits) */
  FILEIO_PATCH_BYTE_GENERAL_PITCH_WHEEL,                  /* pitch wheel mode (1 bit), range (4 bits) */
  FILEIO_PATCH_BYTE_GENERAL_CHORUS_WAVEFORM_FREQUENCY,    /* chorus waveform (2 bits), frequency (5 bits) */
  FILEIO_PATCH_BYTE_GENERAL_CHORUS_BASE,                  /* chorus base (7 bits) */
  FILEIO_PATCH_NUM_GENERAL_BYTES
};

/* controller routings: vibrato on osc1&2, vibrato on pitch env (or what it affects), */
/*                      tremolo on amp env, tremolo on filt env,                      */
/*                      boost on amp env, boost on filt env, boost on pitch env,      */
/*                      chorus effect                                                 */

/* 10 bytes */
#define FILEIO_PATCH_NAME_BYTES 10
#define FILEIO_PATCH_NAME_SIZE  (FILEIO_PATCH_NAME_BYTES + 1)

/* 10 bytes */
#define FILEIO_CART_NAME_BYTES  10
#define FILEIO_CART_NAME_SIZE   (FILEIO_CART_NAME_BYTES + 1)

/* should be 10 + 5 + (3 * 8) + 23 = 62 bytes */
#define FILEIO_PATCH_NUM_BYTES  ( FILEIO_PATCH_NAME_SIZE +                     \
                                  (1 * FILEIO_PATCH_NUM_OSC_BYTES) +           \
                                  (2 * FILEIO_PATCH_NUM_AMP_ENV_BYTES) +       \
                                  (1 * FILEIO_PATCH_NUM_PITCH_ENV_BYTES) +     \
                                  FILEIO_PATCH_NUM_GENERAL_BYTES)

#define FILEIO_PATCH_GENERAL_START_INDEX  FILEIO_PATCH_NAME_SIZE
#define FILEIO_PATCH_GENERAL_END_INDEX    ( FILEIO_PATCH_GENERAL_START_INDEX + \
                                            FILEIO_PATCH_NUM_GENERAL_BYTES)

#define FILEIO_PATCH_OSC_START_INDEX  FILEIO_PATCH_GENERAL_END_INDEX
#define FILEIO_PATCH_OSC_END_INDEX    ( FILEIO_PATCH_OSC_START_INDEX + \
                                        1 * FILEIO_PATCH_NUM_OSC_BYTES)

/* should be 10 + (16 * 62) = 1002 bytes */
#define FILEIO_CART_NUM_BYTES   ( FILEIO_CART_NAME_SIZE +                      \
                                  (FILEIO_PATCH_NUM_BYTES * BANK_PATCHES_PER_CART))

/* function declarations */
short int fileio_cart_load(int cart_num, char* filename);
short int fileio_cart_save(int cart_num, char* filename);

#endif
