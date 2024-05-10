/*******************************************************************************
** fileio.h (loading and saving from native file formats)
*******************************************************************************/

#ifndef FILEIO_H
#define FILEIO_H

#include "bank.h"

/* 52 bytes */
enum
{
  /* osc waveform, detune (3 bytes) */
  FILEIO_PATCH_BYTE_OSC_1_WAVEFORM_DETUNE = 0,      /* waveform (4 bits), detune (3 bits) */
  FILEIO_PATCH_BYTE_OSC_2_WAVEFORM_DETUNE,          /* waveform (4 bits), detune (3 bits) */
  FILEIO_PATCH_BYTE_OSC_3_WAVEFORM_DETUNE,          /* waveform (4 bits), detune (3 bits) */
  /* osc frequency mode, multiple, divisor (3 bytes) */
  FILEIO_PATCH_BYTE_OSC_1_FREQ_MODE_MUL_DIV,        /* frequency mode (1 bit), multiple (4 bits), divisor (3 bits) */
  FILEIO_PATCH_BYTE_OSC_2_FREQ_MODE_MUL_DIV,        /* frequency mode (1 bit), multiple (4 bits), divisor (3 bits) */
  FILEIO_PATCH_BYTE_OSC_3_FREQ_MODE_MUL_DIV,        /* frequency mode (1 bit), multiple (4 bits), divisor (3 bits) */
  /* algorithm, osc phi, osc sync, filters (2 byte) */
  FILEIO_PATCH_BYTE_OSC_SYNC_PHI,                   /* osc sync (1 bit), phi (2 bits each, 6 bits total) */
  FILEIO_PATCH_BYTE_OSC_ALGORITHM_FILTER_CUTOFFS,   /* algorithm (2 bits), highpass cutoff (2 bits), lowpass cutoff (2 bits) */
  /* envelope 1 (7 bytes) */
  FILEIO_PATCH_BYTE_ENV_1_ATTACK,                   /* attack time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_1_DECAY,                    /* decay time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_1_RELEASE,                  /* release time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_1_LEVEL,                    /* max level (7 bits) */
  FILEIO_PATCH_BYTE_ENV_1_SUSTAIN,                  /* sustain level (7 bits) */
  FILEIO_PATCH_BYTE_ENV_1_HOLD_PEDAL,               /* hold shift (4 bits), pedal shift (4 bits) */
  FILEIO_PATCH_BYTE_ENV_1_KEYSCALING,               /* rate keyscaling (4 bits), level keyscaling (4 bits) */
  /* envelope 2 (7 bytes) */
  FILEIO_PATCH_BYTE_ENV_2_ATTACK,                   /* attack time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_2_DECAY,                    /* decay time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_2_RELEASE,                  /* release time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_2_LEVEL,                    /* max level (7 bits) */
  FILEIO_PATCH_BYTE_ENV_2_SUSTAIN,                  /* sustain level (7 bits) */
  FILEIO_PATCH_BYTE_ENV_2_HOLD_PEDAL,               /* hold shift (4 bits), pedal shift (4 bits) */
  FILEIO_PATCH_BYTE_ENV_2_KEYSCALING,               /* rate keyscaling (4 bits), level keyscaling (4 bits) */
  /* envelope 3 (7 bytes) */
  FILEIO_PATCH_BYTE_ENV_3_ATTACK,                   /* attack time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_3_DECAY,                    /* decay time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_3_RELEASE,                  /* release time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_3_LEVEL,                    /* max level (7 bits) */
  FILEIO_PATCH_BYTE_ENV_3_SUSTAIN,                  /* sustain level (7 bits) */
  FILEIO_PATCH_BYTE_ENV_3_HOLD_PEDAL,               /* hold shift (4 bits), pedal shift (4 bits) */
  FILEIO_PATCH_BYTE_ENV_3_KEYSCALING,               /* rate keyscaling (4 bits), level keyscaling (4 bits) */
  /* chorus (3 bytes) */
  FILEIO_PATCH_BYTE_CHORUS_WAVEFORM_SPEED,          /* waveform (2 bits), speed (4 bits) */
  FILEIO_PATCH_BYTE_CHORUS_SYNC_DELAY,              /* sync (1 bit), delay (6 bits) */
  FILEIO_PATCH_BYTE_CHORUS_DEPTH,                   /* depth (8 bits) */
  /* vibrato lfo (3 bytes) */
  FILEIO_PATCH_BYTE_VIBRATO_WAVEFORM_SPEED,         /* waveform (2 bits), speed (4 bits) */
  FILEIO_PATCH_BYTE_VIBRATO_SYNC_DELAY,             /* sync (1 bit),delay (6 bits) */
  FILEIO_PATCH_BYTE_VIBRATO_DEPTH,                  /* depth (8 bits) */
  /* tremolo lfo (3 bytes) */
  FILEIO_PATCH_BYTE_TREMOLO_WAVEFORM_SPEED,         /* waveform (2 bits), speed (4 bits) */
  FILEIO_PATCH_BYTE_TREMOLO_SYNC_DELAY,             /* sync (1 bit),delay (6 bits) */
  FILEIO_PATCH_BYTE_TREMOLO_DEPTH,                  /* depth (8 bits) */
  /* sensitivities (3 bytes) */
  FILEIO_PATCH_BYTE_VIBRATO_TREMOLO_SENS,           /* sensitivities (4 bits each, 8 bits total) */
  FILEIO_PATCH_BYTE_BOOST_VELOCITY_SENS,            /* sensitivities (4 bits each, 8 bits total) */
  FILEIO_PATCH_BYTE_CHORUS_SENS,                    /* chorus sensitivity (4 bits) */
  /* pitch envelope (5 bytes) */
  FILEIO_PATCH_BYTE_PITCH_ENV_ATTACK,               /* attack time (7 bits) */
  FILEIO_PATCH_BYTE_PITCH_ENV_DECAY,                /* decay time (7 bits) */
  FILEIO_PATCH_BYTE_PITCH_ENV_RELEASE,              /* release time (7 bits) */
  FILEIO_PATCH_BYTE_PITCH_ENV_MAXIMUM,              /* max level (7 bits) */
  FILEIO_PATCH_BYTE_PITCH_ENV_FINALE,               /* finale level (7 bits) */
  /* pitch wheel, arpeggio, portamento (3 bytes) */
  FILEIO_PATCH_BYTE_PITCH_WHEEL,                    /* pitch wheel mode (1 bit), range (4 bits) */
  FILEIO_PATCH_BYTE_ARPEGGIO,                       /* arpeggio pattern (2 bits), octaves (2 bits), speed (4 bits) */
  FILEIO_PATCH_BYTE_PORTAMENTO,                     /* arpeggio mode (1 bit), portamento mode (1 bit), follow/legato (2 bits), speed (4 bits) */
  /* midi controller routing (3 bytes) */
  FILEIO_PATCH_BYTE_ROUTING_TREMOLO_MOD_WHEEL,      /* tremolo routing (3 bits), mod wheel routing (4 bits) */
  FILEIO_PATCH_BYTE_ROUTING_BOOST_AFTERTOUCH,       /* boost routing (3 bits), aftertouch routing (4 bits) */
  FILEIO_PATCH_BYTE_ROUTING_VELOCITY_EXP_PEDAL,     /* velocity routing (3 bits), exp pedal routing (4 bits) */
  FILEIO_PATCH_NUM_DATA_BYTES
};

/* 10 bytes */
#define FILEIO_PATCH_NUM_NAME_BYTES 10
#define FILEIO_PATCH_NAME_SIZE      (FILEIO_PATCH_NUM_NAME_BYTES + 1)

/* 8 bytes */
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
