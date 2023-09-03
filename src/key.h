/*******************************************************************************
** key.h (musical keys)
*******************************************************************************/

#ifndef KEY_H
#define KEY_H

#if 0
enum
{
  /* major keys */
  KEY_SIGNATURE_C_FLAT_MAJOR = 0, /* 7 flats  */
  KEY_SIGNATURE_G_FLAT_MAJOR,
  KEY_SIGNATURE_D_FLAT_MAJOR,
  KEY_SIGNATURE_A_FLAT_MAJOR,
  KEY_SIGNATURE_E_FLAT_MAJOR,
  KEY_SIGNATURE_B_FLAT_MAJOR,
  KEY_SIGNATURE_F_MAJOR,
  KEY_SIGNATURE_C_MAJOR,          /* clear    */
  KEY_SIGNATURE_G_MAJOR,
  KEY_SIGNATURE_D_MAJOR,
  KEY_SIGNATURE_A_MAJOR,
  KEY_SIGNATURE_E_MAJOR,
  KEY_SIGNATURE_B_MAJOR,
  KEY_SIGNATURE_F_SHARP_MAJOR,
  KEY_SIGNATURE_C_SHARP_MAJOR,    /* 7 sharps */
  /* minor keys */
  KEY_SIGNATURE_A_FLAT_MINOR,     /* 7 flats  */
  KEY_SIGNATURE_E_FLAT_MINOR,
  KEY_SIGNATURE_B_FLAT_MINOR,
  KEY_SIGNATURE_F_MINOR,
  KEY_SIGNATURE_C_MINOR,
  KEY_SIGNATURE_G_MINOR,
  KEY_SIGNATURE_D_MINOR,
  KEY_SIGNATURE_A_MINOR,          /* clear    */
  KEY_SIGNATURE_E_MINOR,
  KEY_SIGNATURE_B_MINOR,
  KEY_SIGNATURE_F_SHARP_MINOR,
  KEY_SIGNATURE_C_SHARP_MINOR,
  KEY_SIGNATURE_G_SHARP_MINOR,
  KEY_SIGNATURE_D_SHARP_MINOR,
  KEY_SIGNATURE_A_SHARP_MINOR,    /* 7 sharps */
  KEY_NUM_SIGNATURES
};
#endif

/* octaves and notes */
#define KEY_LOWEST_OCTAVE   0   /* 1 */
#define KEY_MIDDLE_OCTAVE   3   /* 4 */
#define KEY_HIGHEST_OCTAVE  6   /* 7 */

#define KEY_NUM_OCTAVES  (KEY_HIGHEST_OCTAVE - KEY_LOWEST_OCTAVE + 1)

#define KEY_NOTE_C0  (12 * (KEY_MIDDLE_OCTAVE - 4))
#define KEY_NOTE_A0  (12 * (KEY_MIDDLE_OCTAVE - 4) + 9)
#define KEY_NOTE_C4  (12 * (KEY_MIDDLE_OCTAVE + 0))
#define KEY_NOTE_C8  (12 * (KEY_MIDDLE_OCTAVE + 4))

#define KEY_NUM_NOTES (7 * KEY_NUM_OCTAVES + 1)

/* degrees */
#define KEY_NUM_DEGREES 8

/* signatures and modes */
#define KEY_NUM_SIGS  15
#define KEY_NUM_MODES 7

#define KEY_SIG_CLEAR   7

#define KEY_SIG_1_FLAT  (KEY_SIG_CLEAR - 1)
#define KEY_SIG_2_FLAT  (KEY_SIG_CLEAR - 2)
#define KEY_SIG_3_FLAT  (KEY_SIG_CLEAR - 3)
#define KEY_SIG_4_FLAT  (KEY_SIG_CLEAR - 4)
#define KEY_SIG_5_FLAT  (KEY_SIG_CLEAR - 5)
#define KEY_SIG_6_FLAT  (KEY_SIG_CLEAR - 6)
#define KEY_SIG_7_FLAT  (KEY_SIG_CLEAR - 7)

#define KEY_SIG_1_SHARP (KEY_SIG_CLEAR + 1)
#define KEY_SIG_2_SHARP (KEY_SIG_CLEAR + 2)
#define KEY_SIG_3_SHARP (KEY_SIG_CLEAR + 3)
#define KEY_SIG_4_SHARP (KEY_SIG_CLEAR + 4)
#define KEY_SIG_5_SHARP (KEY_SIG_CLEAR + 5)
#define KEY_SIG_6_SHARP (KEY_SIG_CLEAR + 6)
#define KEY_SIG_7_SHARP (KEY_SIG_CLEAR + 7)

#define KEY_MODE_MAJOR  0
#define KEY_MODE_MINOR  5

/* function declarations */
int       key_note_lookup(int signature, int mode, int octave, int degree);

short int key_generate_tables();

#endif
