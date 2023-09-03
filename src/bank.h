/*******************************************************************************
** bank.h (bank sizes)
*******************************************************************************/

#ifndef BANK_H
#define BANK_H

/* patches */
#define BANK_NUM_SFX_PATCHES  64
#define BANK_NUM_INST_PATCHES 64

#define BANK_NUM_PATCHES      ( BANK_NUM_SFX_PATCHES +                         \
                                BANK_NUM_INST_PATCHES)

#define BANK_PATCH_INDEX_IS_VALID(index)                                       \
  ((index >= 0) && (index < BANK_NUM_PATCHES))

#define BANK_PATCH_INDEX_IS_NOT_VALID(index)                                   \
  (!(BANK_PATCH_INDEX_IS_VALID(index)))

/* voices */
#define BANK_NUM_SFX_VOICES   6
#define BANK_NUM_DRUM_VOICES  6
#define BANK_NUM_INST_VOICES  12

#define BANK_NUM_VOICES       ( BANK_NUM_SFX_VOICES +                          \
                                BANK_NUM_DRUM_VOICES +                         \
                                BANK_NUM_INST_VOICES)

#define BANK_SFX_VOICES_START_INDEX   0
#define BANK_SFX_VOICES_END_INDEX     ( BANK_SFX_VOICES_START_INDEX +          \
                                        BANK_NUM_SFX_VOICES)

#define BANK_DRUM_VOICES_START_INDEX BANK_SFX_VOICES_END_INDEX
#define BANK_DRUM_VOICES_END_INDEX    ( BANK_DRUM_VOICES_START_INDEX +         \
                                        BANK_NUM_DRUM_VOICES)

#define BANK_INST_VOICES_START_INDEX BANK_DRUM_VOICES_END_INDEX
#define BANK_INST_VOICES_END_INDEX   (  BANK_INST_VOICES_START_INDEX +         \
                                        BANK_NUM_INST_VOICES)

#define BANK_INDEX_IS_IN_SFX_VOICES(index)                                     \
  ( (index >= BANK_SFX_VOICES_START_INDEX) &&                                  \
    (index <  BANK_SFX_VOICES_END_INDEX))

#define BANK_INDEX_IS_IN_DRUM_VOICES(index)                                    \
  ( (index >= BANK_DRUM_VOICES_START_INDEX) &&                                 \
    (index <  BANK_DRUM_VOICES_END_INDEX))

#define BANK_INDEX_IS_IN_INST_VOICES(index)                                    \
  ( (index >= BANK_INST_VOICES_START_INDEX) &&                                 \
    (index <  BANK_INST_VOICES_END_INDEX))

#define BANK_VOICE_INDEX_IS_VALID(index)                                       \
  ((index >= 0) && (index < BANK_NUM_VOICES))

#define BANK_VOICE_INDEX_IS_NOT_VALID(index)                                   \
  (!(BANK_VOICE_INDEX_IS_VALID(index)))

/* envelopes, lfos, filters, etc */
#define BANK_NUM_ENVELOPES  (BANK_NUM_VOICES * 4)
#define BANK_NUM_LFOS       (BANK_NUM_VOICES * 1)
#define BANK_NUM_SWEEPS     (BANK_NUM_VOICES * 1)
#define BANK_NUM_FILTERS    (BANK_NUM_VOICES * 2)

#endif
