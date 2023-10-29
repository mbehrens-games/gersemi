/*******************************************************************************
** fileio.h (loading and saving from native file formats)
*******************************************************************************/

#ifndef FILEIO_H
#define FILEIO_H

enum
{
  FILEIO_PATCH_BYTE_GENERAL_ALGORITHM_CUTOFFS = 0, /* algorithm (3 bits), lowpass (2 bits), highpass (2 bits) */
  FILEIO_PATCH_BYTE_GENERAL_VIBRATO_DEPTH_EFFECT_MODES, /* depth (4 bits), modes (3 bits, 1 bit each) */
  FILEIO_PATCH_BYTE_GENERAL_TREMOLO_BOOST_DEPTH, /* depths (8 bits, 4 bits each) */
  FILEIO_PATCH_BYTE_GENERAL_MOD_WHEEL_EFFECT_VIBRATO_BASE, /* base (5 bits), effect (2 bits) */
  FILEIO_PATCH_BYTE_GENERAL_AFTERTOUCH_EFFECT_TREMOLO_BASE, /* base (5 bits), effect (2 bits) */
  FILEIO_PATCH_BYTE_GENERAL_PORT_ARP_MODE_SPEED, /* mode (3 bits), speed (4 bits) */
  FILEIO_PATCH_BYTE_GENERAL_PORT_ARP_DIRECTION_PEDAL_SHIFT, /* direction (2 bits), pedal shift (4 bits) */
  FILEIO_PATCH_BYTE_GENERAL_PITCH_WHEEL_MODE_RANGE, /* mode (1 bit), range (4 bits) */
  FILEIO_PATCH_NUM_GENERAL_BYTES 
};

enum
{
  FILEIO_PATCH_BYTE_OSC_WAVEFORM_FEEDBACK = 0, /* waveform (3 bits), feedback (3 bits) */
  FILEIO_PATCH_BYTE_OSC_SYNC_FREQ_MODE, /* sync (1 bit), phi (2 bits), freq mode (1 bit) */
  FILEIO_PATCH_BYTE_OSC_MULTIPLE_DIVISOR_OR_NOTE_OCTAVE, /* multiple/note (4 bits), divisor/octave (4 bits) */
  FILEIO_PATCH_BYTE_OSC_DETUNE, /* detune (6 bits) */
  FILEIO_PATCH_NUM_OSC_BYTES
};

enum
{
  FILEIO_PATCH_BYTE_ENV_ATTACK = 0, /* attack (5 bits) */
  FILEIO_PATCH_BYTE_ENV_DECAY_1,    /* decay 1 (5 bits) */
  FILEIO_PATCH_BYTE_ENV_DECAY_2,    /* decay 2 (5 bits) */
  FILEIO_PATCH_BYTE_ENV_RELEASE,    /* release (5 bits) */
  FILEIO_PATCH_BYTE_ENV_AMPLITUDE_RATE_KS,  /* amplitude (6 bits), rate keyscaling (2 bits) */
  FILEIO_PATCH_BYTE_ENV_SUSTAIN_LEVEL_KS,   /* sustain (5 bits), level keyscaling (2 bits) */
  FILEIO_PATCH_NUM_ENV_BYTES
};

enum
{
  FILEIO_PATCH_BYTE_LFO_WAVEFORM_SYNC_QUANTIZE = 0, /* waveform (3 bits), sync (1 bit), quantize (4 bits) */
  FILEIO_PATCH_BYTE_LFO_FREQUENCY, /* frequency (5 bits) */
  FILEIO_PATCH_BYTE_LFO_DELAY, /* delay (5 bits) */
  FILEIO_PATCH_BYTE_LFO_SAH_NOISE, /* noise frequency (6 bits) */
  FILEIO_PATCH_NUM_LFO_BYTES
};

#define FILEIO_PATCH_NUM_BYTES  ( FILEIO_PATCH_NUM_GENERAL_BYTES +             \
                                  (4 * FILEIO_PATCH_NUM_OSC_BYTES) +           \
                                  (4 * FILEIO_PATCH_NUM_ENV_BYTES) +           \
                                  (1 * FILEIO_PATCH_NUM_LFO_BYTES))

#define FILEIO_PATCH_GENERAL_START_INDEX  0
#define FILEIO_PATCH_GENERAL_END_INDEX    ( FILEIO_PATCH_GENERAL_START_INDEX + \
                                            FILEIO_PATCH_NUM_GENERAL_BYTES)

#define FILEIO_PATCH_OSC_START_INDEX  FILEIO_PATCH_GENERAL_END_INDEX
#define FILEIO_PATCH_OSC_END_INDEX    ( FILEIO_PATCH_OSC_START_INDEX +         \
                                        4 * FILEIO_PATCH_NUM_OSC_BYTES)

#define FILEIO_PATCH_ENV_START_INDEX  FILEIO_PATCH_OSC_END_INDEX
#define FILEIO_PATCH_ENV_END_INDEX    ( FILEIO_PATCH_ENV_START_INDEX +         \
                                        4 * FILEIO_PATCH_NUM_ENV_BYTES)

#define FILEIO_PATCH_LFO_START_INDEX  FILEIO_PATCH_ENV_END_INDEX
#define FILEIO_PATCH_LFO_END_INDEX    ( FILEIO_PATCH_LFO_START_INDEX +         \
                                        1 * FILEIO_PATCH_NUM_LFO_BYTES)

#define FILEIO_NUM_SETS         8
#define FILEIO_PATCHES_PER_SET  32

#define FILEIO_SET_NUM_BYTES    (FILEIO_PATCH_NUM_BYTES * FILEIO_PATCHES_PER_SET)

/* function declarations */
short int fileio_patch_set_load(char* filename, int set_num);
short int fileio_patch_set_save(char* filename, int set_num);

#endif
