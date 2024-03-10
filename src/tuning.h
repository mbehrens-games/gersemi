/*******************************************************************************
** tuning.h (tuning systems)
*******************************************************************************/

#ifndef TUNING_H
#define TUNING_H

enum
{
  TUNING_SYSTEM_12_EQUAL_TEMPERAMENT = 0,
  TUNING_SYSTEM_PYTHAGOREAN,
  TUNING_SYSTEM_QUARTER_COMMA_MEANTONE,
  TUNING_SYSTEM_WERCKMEISTER_III,
  TUNING_NUM_SYSTEMS
};

enum
{
  TUNING_FORK_C256 = 0,
  TUNING_FORK_A432,
  TUNING_FORK_A434,
  TUNING_FORK_A436,
  TUNING_FORK_A438,
  TUNING_FORK_A440,
  TUNING_FORK_A442,
  TUNING_FORK_A444,
  TUNING_NUM_FORKS
};

#define TUNING_NUM_SEMITONE_STEPS 128

#define TUNING_TABLE_SIZE (12 * TUNING_NUM_SEMITONE_STEPS)

#define TUNING_NOTE_C0  (12 * 0 +  0)
#define TUNING_NOTE_A0  (12 * 0 +  9)
#define TUNING_NOTE_C4  (12 * 4 +  0)
#define TUNING_NOTE_C8  (12 * 8 +  0)
#define TUNING_NOTE_B9  (12 * 9 + 11)

#define TUNING_NOTE_BLANK (TUNING_NOTE_C0 - 1)

#define TUNING_NUM_VALID_NOTES    (TUNING_NOTE_B9 - TUNING_NOTE_C0 + 1)
#define TUNING_NUM_PLAYABLE_NOTES (TUNING_NOTE_C8 - TUNING_NOTE_A0 + 1)

#define TUNING_NUM_INDICES    (TUNING_NUM_VALID_NOTES * TUNING_NUM_SEMITONE_STEPS)

#define TUNING_LOWEST_OCTAVE  (TUNING_NOTE_C0 / 12)
#define TUNING_MIDDLE_OCTAVE  (TUNING_NOTE_C4 / 12)
#define TUNING_HIGHEST_OCTAVE (TUNING_NOTE_B9 / 12)

#if 0
#define TUNING_HIGHEST_OCTAVE ((TUNING_NUM_VALID_NOTES / 12) - 1)
#endif

#define TUNING_NOTE_IS_VALID(note)                                             \
  ((note >= TUNING_NOTE_C0) && (note <= TUNING_NOTE_B9))

#define TUNING_NOTE_IS_NOT_VALID(note)                                         \
  (!(TUNING_NOTE_IS_VALID(note)))

#define TUNING_NOTE_IS_PLAYABLE(note)                                          \
  ((note >= TUNING_NOTE_A0) && (note <= TUNING_NOTE_C8))

#define TUNING_NOTE_IS_NOT_PLAYABLE(note)                                      \
  (!(TUNING_NOTE_IS_PLAYABLE(note)))

#define TUNING_INDEX_IS_VALID(index)                                           \
  ((index >= 0) && (index <= TUNING_NUM_INDICES))

#define TUNING_INDEX_IS_NOT_VALID(index)                                       \
  (!(TUNING_INDEX_IS_VALID(index)))

extern unsigned int G_tuning_phase_increment_table[];

/* function declarations */
short int tuning_reset();

short int tuning_set_system(int system);
short int tuning_set_fork(int fork);

short int tuning_calculate_tables();

#endif
