/*******************************************************************************
** fileio.h (loading and saving from native file formats)
*******************************************************************************/

#ifndef FILEIO_H
#define FILEIO_H

#include "bank.h"

/* 52 bytes */
enum
{
  /* osc frequency mode, detune, waveform (3 bytes) */
  FILEIO_PATCH_BYTE_OSC_1_FREQ_MODE_DETUNE_WAVEFORM = 0,  /* frequency mode (1 bit), detune (3 bits), waveform (4 bits) */
  FILEIO_PATCH_BYTE_OSC_2_FREQ_MODE_DETUNE_WAVEFORM,      /* frequency mode (1 bit), detune (3 bits), waveform (4 bits) */
  FILEIO_PATCH_BYTE_OSC_3_FREQ_MODE_DETUNE_WAVEFORM,      /* frequency mode (1 bit), detune (3 bits), waveform (4 bits) */
  /* osc multiple, divisor (3 bytes) */
  FILEIO_PATCH_BYTE_OSC_1_MULTIPLE_DIVISOR,               /* multiple or octave (4 bits), divisor or note (4 bits) */
  FILEIO_PATCH_BYTE_OSC_2_MULTIPLE_DIVISOR,               /* multiple or octave (4 bits), divisor or note (4 bits) */
  FILEIO_PATCH_BYTE_OSC_3_MULTIPLE_DIVISOR,               /* multiple or octave (4 bits), divisor or note (4 bits) */
  /* osc sync, phi (1 byte) */
  FILEIO_PATCH_BYTE_OSC_SYNC_PHI,                         /* osc sync (1 bit), phi (2 bits each, 6 bits total) */
  /* algorithm, filters, osc / env routing (3 bytes) */
  FILEIO_PATCH_BYTE_ALGORITHM_OSC_1_ENV_1_ROUTING,        /* algorithm (2 bits), osc routing (3 bits), env routing (3 bits) */
  FILEIO_PATCH_BYTE_HIGHPASS_CUTOFF_OSC_2_ENV_2_ROUTING,  /* highpass cutoff (2 bits), osc routing (3 bits), env routing (3 bits) */
  FILEIO_PATCH_BYTE_LOWPASS_CUTOFF_OSC_3_ENV_3_ROUTING,   /* lowpass cutoff (2 bits), osc routing (3 bits), env routing (3 bits) */
  /* envelope 1 (7 bytes) */
  FILEIO_PATCH_BYTE_ENV_1_ATTACK,                         /* attack time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_1_DECAY,                          /* decay time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_1_SUSTAIN,                        /* sustain time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_1_RELEASE,                        /* release time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_1_LEVEL,                          /* max level (7 bits) */
  FILEIO_PATCH_BYTE_ENV_1_TRANSITION,                     /* transition level (7 bits) */
  FILEIO_PATCH_BYTE_ENV_1_HOLD_MODE_KEYSCALING,           /* hold mode (2 bits), rate keyscaling (3 bits), level keyscaling (3 bits) */
  /* envelope 2 (7 bytes) */
  FILEIO_PATCH_BYTE_ENV_2_ATTACK,                         /* attack time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_2_DECAY,                          /* decay time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_2_SUSTAIN,                        /* sustain time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_2_RELEASE,                        /* release time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_2_LEVEL,                          /* max level (7 bits) */
  FILEIO_PATCH_BYTE_ENV_2_TRANSITION,                     /* transition level (7 bits) */
  FILEIO_PATCH_BYTE_ENV_2_HOLD_MODE_KEYSCALING,           /* hold mode (2 bits), rate keyscaling (3 bits), level keyscaling (3 bits) */
  /* envelope 3 (7 bytes) */
  FILEIO_PATCH_BYTE_ENV_3_ATTACK,                         /* attack time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_3_DECAY,                          /* decay time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_3_SUSTAIN,                        /* sustain time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_3_RELEASE,                        /* release time (7 bits) */
  FILEIO_PATCH_BYTE_ENV_3_LEVEL,                          /* max level (7 bits) */
  FILEIO_PATCH_BYTE_ENV_3_TRANSITION,                     /* transition level (7 bits) */
  FILEIO_PATCH_BYTE_ENV_3_HOLD_MODE_KEYSCALING,           /* hold mode (2 bits), rate keyscaling (3 bits), level keyscaling (3 bits) */
  /* vibrato lfo (3 bytes) */
  FILEIO_PATCH_BYTE_VIBRATO_POLARITY_WAVEFORM_SPEED,      /* polarity (1 bit), waveform (2 bits), speed (4 bits) */
  FILEIO_PATCH_BYTE_VIBRATO_SYNC_DELAY,                   /* sync (1 bit), delay (6 bits) */
  FILEIO_PATCH_BYTE_VIBRATO_DEPTH,                        /* depth (8 bits) */
  /* tremolo lfo (3 bytes) */
  FILEIO_PATCH_BYTE_TREMOLO_WAVEFORM_SPEED,               /* waveform (2 bits), speed (4 bits) */
  FILEIO_PATCH_BYTE_TREMOLO_SYNC_DELAY,                   /* sync (1 bit), delay (6 bits) */
  FILEIO_PATCH_BYTE_TREMOLO_DEPTH,                        /* depth (8 bits) */
  /* chorus (3 bytes) */
  FILEIO_PATCH_BYTE_CHORUS_WAVEFORM_SPEED,                /* waveform (2 bits), speed (4 bits) */
  FILEIO_PATCH_BYTE_CHORUS_SYNC_DELAY,                    /* sync (1 bit), delay (6 bits) */
  FILEIO_PATCH_BYTE_CHORUS_DEPTH,                         /* depth (8 bits) */
  /* sensitivities, midi controller routing (4 bytes) */
  FILEIO_PATCH_BYTE_BOOST_VELOCITY_SENS,                  /* sensitivities (4 bits each, 8 bits total) */
  FILEIO_PATCH_BYTE_VIBRATO_TREMOLO_SENS,                 /* sensitivities (4 bits each, 8 bits total) */
  FILEIO_PATCH_BYTE_CHORUS_SENS_MOD_WHEEL_ROUTING,        /* chorus sensitivity (4 bits), mod wheel routing (4 bits) */
  FILEIO_PATCH_BYTE_AFTERTOUCH_EXP_PEDAL_ROUTING,         /* aftertouch routing (4 bits), exp pedal routing (4 bits) */
  /* pitch envelope (5 bytes) */
  FILEIO_PATCH_BYTE_PITCH_ENV_ATTACK,                     /* attack time (7 bits) */
  FILEIO_PATCH_BYTE_PITCH_ENV_DECAY,                      /* decay time (7 bits) */
  FILEIO_PATCH_BYTE_PITCH_ENV_RELEASE,                    /* release time (7 bits) */
  FILEIO_PATCH_BYTE_PITCH_ENV_MAXIMUM,                    /* max level (7 bits) */
  FILEIO_PATCH_BYTE_PITCH_ENV_FINALE,                     /* finale level (7 bits) */
  /* pitch wheel, arpeggio, portamento (3 bytes) */
  FILEIO_PATCH_BYTE_PITCH_WHEEL,                          /* pitch wheel mode (1 bit), range (5 bits) */
  FILEIO_PATCH_BYTE_ARPEGGIO,                             /* arpeggio pattern (2 bits), octaves (2 bits), speed (4 bits) */
  FILEIO_PATCH_BYTE_PORTAMENTO,                           /* arpeggio mode (1 bit), portamento mode (1 bit), follow/legato (2 bits), speed (4 bits) */
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
