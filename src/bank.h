/*******************************************************************************
** bank.h (bank sizes)
*******************************************************************************/

#ifndef BANK_H
#define BANK_H

/* instruments */
#define BANK_VOICES_PER_POLY_INSTRUMENT     6
#define BANK_VOICES_PER_MONO_INSTRUMENT     1
#define BANK_VOICES_PER_SOUND_FX_INSTRUMENT 1

#define BANK_NUM_POLY_INSTRUMENTS     2
#define BANK_NUM_MONO_INSTRUMENTS     12
#define BANK_NUM_SOUND_FX_INSTRUMENTS 6

#define BANK_NUM_INSTRUMENTS  ( BANK_NUM_POLY_INSTRUMENTS +                    \
                                BANK_NUM_MONO_INSTRUMENTS +                    \
                                BANK_NUM_SOUND_FX_INSTRUMENTS)

#define BANK_INSTRUMENT_INDEX_IS_VALID(index)                                  \
  ((index >= 0) && (index < BANK_NUM_INSTRUMENTS))

#define BANK_INSTRUMENT_INDEX_IS_NOT_VALID(index)                              \
  (!(BANK_INSTRUMENT_INDEX_IS_VALID(index)))

#define BANK_POLY_INSTRUMENTS_START_INDEX       0
#define BANK_POLY_INSTRUMENTS_END_INDEX         ( BANK_POLY_INSTRUMENTS_START_INDEX + \
                                                  BANK_NUM_POLY_INSTRUMENTS)

#define BANK_MONO_INSTRUMENTS_START_INDEX       BANK_POLY_INSTRUMENTS_END_INDEX
#define BANK_MONO_INSTRUMENTS_END_INDEX         ( BANK_MONO_INSTRUMENTS_START_INDEX + \
                                                  BANK_NUM_MONO_INSTRUMENTS)

#define BANK_SOUND_FX_INSTRUMENTS_START_INDEX   BANK_MONO_INSTRUMENTS_END_INDEX
#define BANK_SOUND_FX_INSTRUMENTS_END_INDEX     ( BANK_SOUND_FX_INSTRUMENTS_START_INDEX + \
                                                  BANK_NUM_SOUND_FX_INSTRUMENTS)

/* drum machine */
#define BANK_VOICES_PER_DRUM_MACHINE  6

#define BANK_NUM_DRUM_MACHINES        1

/* voices */
#define BANK_NUM_POLY_VOICES      (BANK_VOICES_PER_POLY_INSTRUMENT * BANK_NUM_POLY_INSTRUMENTS)
#define BANK_NUM_MONO_VOICES      (BANK_VOICES_PER_MONO_INSTRUMENT * BANK_NUM_MONO_INSTRUMENTS)
#define BANK_NUM_SOUND_FX_VOICES  (BANK_VOICES_PER_SOUND_FX_INSTRUMENT * BANK_NUM_SOUND_FX_INSTRUMENTS)
#define BANK_NUM_DRUM_VOICES      (BANK_VOICES_PER_DRUM_MACHINE * BANK_NUM_DRUM_MACHINES)

#define BANK_NUM_VOICES ( BANK_NUM_POLY_VOICES +                               \
                          BANK_NUM_MONO_VOICES +                               \
                          BANK_NUM_SOUND_FX_VOICES +                           \
                          BANK_NUM_DRUM_VOICES)

#define BANK_VOICE_INDEX_IS_VALID(index)                                       \
  ((index >= 0) && (index < BANK_NUM_VOICES))

#define BANK_VOICE_INDEX_IS_NOT_VALID(index)                                   \
  (!(BANK_VOICE_INDEX_IS_VALID(index)))

#define BANK_POLY_VOICES_START_INDEX      0
#define BANK_POLY_VOICES_END_INDEX        ( BANK_POLY_VOICES_START_INDEX +     \
                                            BANK_NUM_POLY_VOICES)

#define BANK_MONO_VOICES_START_INDEX      BANK_POLY_VOICES_END_INDEX
#define BANK_MONO_VOICES_END_INDEX        ( BANK_MONO_VOICES_START_INDEX +     \
                                            BANK_NUM_MONO_VOICES)

#define BANK_SOUND_FX_VOICES_START_INDEX  BANK_MONO_VOICES_END_INDEX
#define BANK_SOUND_FX_VOICES_END_INDEX    ( BANK_SOUND_FX_VOICES_START_INDEX + \
                                            BANK_NUM_SOUND_FX_VOICES)

#define BANK_DRUM_VOICES_START_INDEX      BANK_SOUND_FX_VOICES_END_INDEX
#define BANK_DRUM_VOICES_END_INDEX        ( BANK_DRUM_VOICES_START_INDEX +     \
                                            BANK_NUM_DRUM_VOICES)

/* envelopes, lfos, filters, etc */
#define BANK_OSCILLATORS_PER_VOICE  3
#define BANK_ENVELOPES_PER_VOICE    3
#define BANK_PEGS_PER_VOICE         1
#define BANK_LFOS_PER_VOICE         3
#define BANK_FILTER_SETS_PER_VOICE  1

#define BANK_NUM_OSCILLATORS  (BANK_NUM_VOICES * BANK_OSCILLATORS_PER_VOICE)
#define BANK_NUM_ENVELOPES    (BANK_NUM_VOICES * BANK_ENVELOPES_PER_VOICE)
#define BANK_NUM_PEGS         (BANK_NUM_VOICES * BANK_PEGS_PER_VOICE)
#define BANK_NUM_LFOS         (BANK_NUM_VOICES * BANK_LFOS_PER_VOICE)
#define BANK_NUM_FILTER_SETS  (BANK_NUM_VOICES * BANK_FILTER_SETS_PER_VOICE)

#define BANK_SWEEPS_PER_INSTRUMENT    1
#define BANK_ARPEGGIOS_PER_INSTRUMENT 1

#define BANK_NUM_SWEEPS     (BANK_NUM_INSTRUMENTS * BANK_SWEEPS_PER_INSTRUMENT)
#define BANK_NUM_ARPEGGIOS  (BANK_NUM_INSTRUMENTS * BANK_ARPEGGIOS_PER_INSTRUMENT)

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
#define BANK_PATCHES_PER_CART 16

#define BANK_NUM_PATCHES (BANK_NUM_CARTS * BANK_PATCHES_PER_CART)

#define BANK_PATCH_INDEX_IS_VALID(index)                                       \
  ((index >= 0) && (index < BANK_NUM_PATCHES))

#define BANK_PATCH_INDEX_IS_NOT_VALID(index)                                   \
  (!(BANK_PATCH_INDEX_IS_VALID(index)))

#endif
