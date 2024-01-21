/*******************************************************************************
** bank.h (bank sizes)
*******************************************************************************/

#ifndef BANK_H
#define BANK_H

/* carts */
#define BANK_NUM_EDITOR_CARTS   8
#define BANK_NUM_PLAYBACK_CARTS 1
#define BANK_NUM_SOUND_FX_CARTS 1

#define BANK_NUM_CARTS  ( BANK_NUM_EDITOR_CARTS +                              \
                          BANK_NUM_PLAYBACK_CARTS +                            \
                          BANK_NUM_SOUND_FX_CARTS)

#define BANK_CART_INDEX_IS_VALID(index)                                        \
  ((index >= 0) && (index < BANK_NUM_CARTS))

#define BANK_CART_INDEX_IS_NOT_VALID(index)                                    \
  (!(BANK_CART_INDEX_IS_VALID(index)))

/* patches */
#define BANK_PATCHES_PER_CART   16

#define BANK_NUM_PATCHES (BANK_NUM_CARTS * BANK_PATCHES_PER_CART)

#define BANK_PATCH_INDEX_IS_VALID(index)                                       \
  ((index >= 0) && (index < BANK_NUM_PATCHES))

#define BANK_PATCH_INDEX_IS_NOT_VALID(index)                                   \
  (!(BANK_PATCH_INDEX_IS_VALID(index)))

/* voices */
#define BANK_NUM_INST_VOICES  12
#define BANK_NUM_DRUM_VOICES  6
#define BANK_NUM_SFX_VOICES   6

#define BANK_NUM_VOICES       ( BANK_NUM_INST_VOICES +                         \
                                BANK_NUM_DRUM_VOICES +                         \
                                BANK_NUM_SFX_VOICES)

#define BANK_INST_VOICES_START_INDEX  0
#define BANK_INST_VOICES_END_INDEX    ( BANK_INST_VOICES_START_INDEX +         \
                                        BANK_NUM_INST_VOICES)

#define BANK_DRUM_VOICES_START_INDEX  BANK_INST_VOICES_END_INDEX
#define BANK_DRUM_VOICES_END_INDEX    ( BANK_DRUM_VOICES_START_INDEX +         \
                                        BANK_NUM_DRUM_VOICES)

#define BANK_SFX_VOICES_START_INDEX   BANK_DRUM_VOICES_END_INDEX
#define BANK_SFX_VOICES_END_INDEX     ( BANK_SFX_VOICES_START_INDEX +          \
                                        BANK_NUM_SFX_VOICES)

#define BANK_INDEX_IS_IN_INST_VOICES(index)                                    \
  ( (index >= BANK_INST_VOICES_START_INDEX) &&                                 \
    (index <  BANK_INST_VOICES_END_INDEX))

#define BANK_INDEX_IS_IN_DRUM_VOICES(index)                                    \
  ( (index >= BANK_DRUM_VOICES_START_INDEX) &&                                 \
    (index <  BANK_DRUM_VOICES_END_INDEX))

#define BANK_INDEX_IS_IN_SFX_VOICES(index)                                     \
  ( (index >= BANK_SFX_VOICES_START_INDEX) &&                                  \
    (index <  BANK_SFX_VOICES_END_INDEX))

#define BANK_VOICE_INDEX_IS_VALID(index)                                       \
  ((index >= 0) && (index < BANK_NUM_VOICES))

#define BANK_VOICE_INDEX_IS_NOT_VALID(index)                                   \
  (!(BANK_VOICE_INDEX_IS_VALID(index)))

/* envelopes, lfos, filters, etc */
#define BANK_OSCS_AND_ENVS_PER_VOICE    4
#define BANK_LFOS_PER_VOICE             1
#define BANK_BOOSTS_PER_VOICE           1
#define BANK_SWEEPS_PER_VOICE           1
#define BANK_BENDERS_PER_VOICE          1
#define BANK_LOWPASS_FILTERS_PER_VOICE  1
#define BANK_HIGHPASS_FILTERS_PER_VOICE 1

#define BANK_NUM_ENVELOPES        (BANK_NUM_VOICES * BANK_OSCS_AND_ENVS_PER_VOICE)
#define BANK_NUM_LFOS             (BANK_NUM_VOICES * BANK_LFOS_PER_VOICE)
#define BANK_NUM_BOOSTS           (BANK_NUM_VOICES * BANK_BOOSTS_PER_VOICE)
#define BANK_NUM_SWEEPS           (BANK_NUM_VOICES * BANK_SWEEPS_PER_VOICE)
#define BANK_NUM_BENDERS          (BANK_NUM_VOICES * BANK_BENDERS_PER_VOICE)
#define BANK_NUM_LOWPASS_FILTERS  (BANK_NUM_VOICES * BANK_LOWPASS_FILTERS_PER_VOICE)
#define BANK_NUM_HIGHPASS_FILTERS (BANK_NUM_VOICES * BANK_HIGHPASS_FILTERS_PER_VOICE)

/* instruments */
#define BANK_NUM_INSTRUMENTS  10

#define BANK_INSTRUMENT_INDEX_IS_VALID(index)                                  \
  ((index >= 0) && (index < BANK_NUM_INSTRUMENTS))

#define BANK_INSTRUMENT_INDEX_IS_NOT_VALID(index)                              \
  (!(BANK_INSTRUMENT_INDEX_IS_VALID(index)))

#endif
