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

#define TUNING_NUM_INDICES  (10 * 12 * TUNING_NUM_SEMITONE_STEPS)
#define TUNING_TABLE_SIZE   ( 1 * 12 * TUNING_NUM_SEMITONE_STEPS)

#define TUNING_MIDDLE_OCTAVE  4

#define TUNING_NOTE_C0  (12 * (TUNING_MIDDLE_OCTAVE - 4))
#define TUNING_NOTE_A0  (12 * (TUNING_MIDDLE_OCTAVE - 4) + 9)
#define TUNING_NOTE_C4  (12 * (TUNING_MIDDLE_OCTAVE + 0))
#define TUNING_NOTE_C8  (12 * (TUNING_MIDDLE_OCTAVE + 4))

extern unsigned int G_tuning_phase_increment_table[];

/* function declarations */
short int tuning_reset();

short int tuning_set_system(int system);
short int tuning_set_fork(int fork);

short int tuning_calculate_tables();

#endif
