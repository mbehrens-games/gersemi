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

/* octaves range from -4 to 9 */
#define TUNING_LOWEST_OCTAVE  0   /* -4 */
#define TUNING_MIDDLE_OCTAVE  8   /*  4 */
#define TUNING_HIGHEST_OCTAVE 13  /*  9 */

#define TUNING_NUM_OCTAVES  (TUNING_HIGHEST_OCTAVE - TUNING_LOWEST_OCTAVE + 1)

#define TUNING_AUDITION_OCTAVE_DEFAULT      TUNING_MIDDLE_OCTAVE
#define TUNING_AUDITION_OCTAVE_LOWER_BOUND  (TUNING_MIDDLE_OCTAVE - 3)
#define TUNING_AUDITION_OCTAVE_UPPER_BOUND  (TUNING_MIDDLE_OCTAVE + 3)

#define TUNING_NOTE_C0  (12 * (TUNING_MIDDLE_OCTAVE - 4))
#define TUNING_NOTE_A0  (12 * (TUNING_MIDDLE_OCTAVE - 4) + 9)
#define TUNING_NOTE_C4  (12 * (TUNING_MIDDLE_OCTAVE + 0))
#define TUNING_NOTE_C8  (12 * (TUNING_MIDDLE_OCTAVE + 4))

#define TUNING_NOTE_BLANK (12 * TUNING_LOWEST_OCTAVE - 1)

#define TUNING_NUM_NOTES    (TUNING_NUM_OCTAVES * 12)
#define TUNING_NUM_INDICES  (TUNING_NUM_NOTES * TUNING_NUM_SEMITONE_STEPS)

#define TUNING_TABLE_SIZE   (12 * TUNING_NUM_SEMITONE_STEPS)

extern unsigned int G_tuning_phase_increment_table[];

/* function declarations */
short int tuning_reset();

short int tuning_set_system(int system);
short int tuning_set_fork(int fork);

short int tuning_calculate_tables();

#endif
