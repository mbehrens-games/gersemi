/*******************************************************************************
** fileio.h (loading and saving from native file formats)
*******************************************************************************/

#ifndef FILEIO_H
#define FILEIO_H

#include "bank.h"

/* 52 bytes */
enum
{
  /* oscillators (4 bytes) */
  FILEIO_PATCH_BYTE_OSC_MODE_WAVEFORMS = 0,         /* mode (3 bits), wave osc waveforms (2 bits), blend/detune switch (1 bit) */
  FILEIO_PATCH_BYTE_OSC_BLEND_OR_DETUNE,            /* wave osc blend or detune (7 bits) */
  FILEIO_PATCH_BYTE_OSC_SPECIAL_1,                  /* special (1-8 bits) */
  FILEIO_PATCH_BYTE_OSC_SPECIAL_2,                  /* special (1-8 bits) */
  /* filters (2 bytes) */
  FILEIO_PATCH_BYTE_LP_MULTIPLE_RESONANCE,          /* lowpass multiple (4 bits), resonance (4 bits) */
  FILEIO_PATCH_BYTE_HP_CUTOFF_LP_KEYTRACKING,       /* highpass cutoff (3 bits), lowpass keytracking (4 bits) */
  /* amplitude envelope (5 bytes) */
  FILEIO_PATCH_BYTE_AMP_ENV_ATTACK,                 /* attack (7 bits) */
  FILEIO_PATCH_BYTE_AMP_ENV_DECAY_1,                /* decay 1 (7 bits) */
  FILEIO_PATCH_BYTE_AMP_ENV_DECAY_2,                /* decay 2 (7 bits) */
  FILEIO_PATCH_BYTE_AMP_ENV_RELEASE,                /* release (7 bits) */
  FILEIO_PATCH_BYTE_AMP_ENV_SUSTAIN,                /* sustain (7 bits) */
  /* filter envelope (6 bytes) */
  FILEIO_PATCH_BYTE_FILTER_ENV_ATTACK,              /* attack (7 bits) */
  FILEIO_PATCH_BYTE_FILTER_ENV_DECAY,               /* decay (7 bits) */
  FILEIO_PATCH_BYTE_FILTER_ENV_RELEASE,             /* release (7 bits) */
  FILEIO_PATCH_BYTE_FILTER_ENV_LEVEL,               /* level (7 bits) */
  FILEIO_PATCH_BYTE_FILTER_ENV_HOLD,                /* hold (7 bits) */
  FILEIO_PATCH_BYTE_FILTER_ENV_FINALE,              /* finale (7 bits) */
  /* pitch envelope (6 bytes) */
  FILEIO_PATCH_BYTE_PITCH_ENV_ATTACK,               /* attack (7 bits) */
  FILEIO_PATCH_BYTE_PITCH_ENV_DECAY,                /* decay 1 (7 bits) */
  FILEIO_PATCH_BYTE_PITCH_ENV_RELEASE,              /* release (7 bits) */
  FILEIO_PATCH_BYTE_PITCH_ENV_LEVEL,                /* level (7 bits) */
  FILEIO_PATCH_BYTE_PITCH_ENV_HOLD,                 /* hold (7 bits) */
  FILEIO_PATCH_BYTE_PITCH_ENV_FINALE,               /* finale (7 bits) */
  /* extra envelope (6 bytes) */
  FILEIO_PATCH_BYTE_EXTRA_ENV_ATTACK,               /* attack (7 bits) */
  FILEIO_PATCH_BYTE_EXTRA_ENV_DECAY,                /* decay 1 (7 bits) */
  FILEIO_PATCH_BYTE_EXTRA_ENV_RELEASE,              /* release (7 bits) */
  FILEIO_PATCH_BYTE_EXTRA_ENV_LEVEL,                /* level (7 bits) */
  FILEIO_PATCH_BYTE_EXTRA_ENV_HOLD,                 /* hold (7 bits) */
  FILEIO_PATCH_BYTE_EXTRA_ENV_FINALE,               /* finale (7 bits) */
  /* keyscaling (3 bytes) */
  FILEIO_PATCH_BYTE_AMP_ENV_LEVEL_KS_BREAK_POINT,   /* level keyscaling for amplitude env (4 bits), break point (4 bits) */
  FILEIO_PATCH_BYTE_AMP_FILTER_ENV_RATE_KS,         /* rate keyscaling for amplitude env (4 bits), filter env (4 bits) */
  FILEIO_PATCH_BYTE_PITCH_EXTRA_ENV_RATE_KS,        /* rate keyscaling for pitch env (4 bits), extra env (4 bits) */
  /* lfo 1 (3 bytes) */
  FILEIO_PATCH_BYTE_LFO_1_WAVEFORM_DELAY,           /* lfo 1 waveform (3 bits), delay (5 bits) */
  FILEIO_PATCH_BYTE_LFO_1_POLARITY_FREQUENCY,       /* lfo 1 polarity (1 bit), frequency (5 bits) */
  FILEIO_PATCH_BYTE_LFO_1_BASE,                     /* lfo 1 base (7 bits) */
  /* lfo 2 (3 bytes) */
  FILEIO_PATCH_BYTE_LFO_2_WAVEFORM_DELAY,           /* lfo 2 waveform (3 bits), delay (5 bits) */
  FILEIO_PATCH_BYTE_LFO_2_POLARITY_FREQUENCY,       /* lfo 2 polarity (1 bit), frequency (5 bits) */
  FILEIO_PATCH_BYTE_LFO_2_BASE,                     /* lfo 2 base (7 bits) */
  /* chorus (3 bytes) */
  FILEIO_PATCH_BYTE_CHORUS_WAVEFORM_FREQUENCY,      /* chorus waveform (2 bits), frequency (5 bits) */
  FILEIO_PATCH_BYTE_CHORUS_DRY_WET,                 /* chorus dry/wet mix (7 bits) */
  FILEIO_PATCH_BYTE_CHORUS_BASE,                    /* chorus base (7 bits) */
  /* depths (3 bytes) */
  FILEIO_PATCH_BYTE_LFO_1_LFO_2_DEPTH,              /* depths (4 bits each, 8 bits total) */
  FILEIO_PATCH_BYTE_BOOST_VELOCITY_DEPTH,           /* depths (4 bits each, 8 bits total) */
  FILEIO_PATCH_BYTE_CHORUS_DEPTH,                   /* chorus depth (4 bits) */
  /* various things (5 bytes) */
  FILEIO_PATCH_BYTE_BITCRUSH,                       /* envelope levels (4 bits), oscillator levels (4 bits) */
  FILEIO_PATCH_BYTE_SYNC_SUSTAIN_PEDAL,             /* osc sync (1 bit), lfo sync (1 bit), pedal adjust (4 bits) */
  FILEIO_PATCH_BYTE_PITCH_WHEEL,                    /* arpeggio mode (1 bit), pitch wheel mode (1 bit), range (4 bits) */
  FILEIO_PATCH_BYTE_ARPEGGIO,                       /* arpeggio pattern (2 bits), octaves (2 bits), speed (4 bits) */
  FILEIO_PATCH_BYTE_PORTAMENTO,                     /* portamento mode (1 bit), follow (1 bit), legato (1 bit), speed (4 bits) */
  /* lfo & midi controller routing (3 bytes) */
  FILEIO_PATCH_BYTE_ROUTING_LFO_EXPRESSION_PEDAL,   /* lfo routing (4 bits), exp pedal routing (4 bits) */
  FILEIO_PATCH_BYTE_ROUTING_MOD_WHEEL_AFTERTOUCH,   /* mod wheel routing (4 bits), aftertouch routing (4 bits) */
  FILEIO_PATCH_BYTE_ROUTING_ENV_PEG_ADJUST,         /* amp / filter / extra env adjustment (2 bits each, 6 bits total) */
  FILEIO_PATCH_NUM_DATA_BYTES
};

/* 10 bytes */
#define FILEIO_PATCH_NUM_NAME_BYTES 10
#define FILEIO_PATCH_NAME_SIZE      (FILEIO_PATCH_NUM_NAME_BYTES + 1)

/* 10 bytes */
#define FILEIO_CART_NUM_NAME_BYTES  10
#define FILEIO_CART_NAME_SIZE       (FILEIO_CART_NUM_NAME_BYTES + 1)

/* should be 10 + 52 = 62 bytes */
#define FILEIO_PATCH_NUM_BYTES  ( FILEIO_PATCH_NAME_SIZE +                     \
                                  FILEIO_PATCH_NUM_DATA_BYTES)

/* should be 10 + (16 * 62) = 1002 bytes */
#define FILEIO_CART_NUM_BYTES   ( FILEIO_CART_NAME_SIZE +                      \
                                  (FILEIO_PATCH_NUM_BYTES * BANK_PATCHES_PER_CART))

/* function declarations */
short int fileio_cart_load(int cart_num, char* filename);
short int fileio_cart_save(int cart_num, char* filename);

#endif
