/*******************************************************************************
** key.c (musical keys)
*******************************************************************************/

#include <stdio.h>  /* testing */
#include <stdlib.h>

#include "key.h"

/* highlight table */
char G_key_highlight_table[KEY_NUM_NOTES];

/* notes table */
static char S_key_notes_table[KEY_NUM_SIGNATURES][KEY_NUM_MODES][KEY_NUM_DEGREES];

/*******************************************************************************
** key_set_current_key()
*******************************************************************************/
short int key_set_current_key(int signature, int mode)
{
  int m;
  int n;

  /* make sure signature and mode are valid */
  if ((signature >= 0) && (signature < KEY_NUM_SIGNATURES))
    return 1;

  if ((mode >= 0) && (mode < KEY_NUM_MODES))
    return 1;

  /* initialize note table */
  for (m = 0; m < KEY_NUM_NOTES; m++)
  {
    G_key_highlight_table[m] = KEY_HIGHLIGHT_OFF;

    for (n = 0; n < KEY_NUM_DEGREES; n++)
    {
      if (m == S_key_notes_table[signature][mode][n])
      {
        if (n == KEY_DEGREE_TONIC)
          G_key_highlight_table[m] = KEY_HIGHLIGHT_TONIC;
        else
          G_key_highlight_table[m] = KEY_HIGHLIGHT_ON;

        break;
      }
    }
  }

  return 0;
}

/*******************************************************************************
** key_generate_tables()
*******************************************************************************/
short int key_generate_tables()
{
  int m;
  int n;

  /* key table */

  /* set notes for c major in the octave */
  S_key_notes_table[KEY_SIGNATURE_CLEAR][KEY_MODE_IONIAN][KEY_DEGREE_TONIC]   = KEY_NOTE_C;
  S_key_notes_table[KEY_SIGNATURE_CLEAR][KEY_MODE_IONIAN][KEY_DEGREE_SECOND]  = KEY_NOTE_D;
  S_key_notes_table[KEY_SIGNATURE_CLEAR][KEY_MODE_IONIAN][KEY_DEGREE_THIRD]   = KEY_NOTE_E;
  S_key_notes_table[KEY_SIGNATURE_CLEAR][KEY_MODE_IONIAN][KEY_DEGREE_FOURTH]  = KEY_NOTE_F;
  S_key_notes_table[KEY_SIGNATURE_CLEAR][KEY_MODE_IONIAN][KEY_DEGREE_FIFTH]   = KEY_NOTE_G;
  S_key_notes_table[KEY_SIGNATURE_CLEAR][KEY_MODE_IONIAN][KEY_DEGREE_SIXTH]   = KEY_NOTE_A;
  S_key_notes_table[KEY_SIGNATURE_CLEAR][KEY_MODE_IONIAN][KEY_DEGREE_SEVENTH] = KEY_NOTE_B;

  /* compute notes in other keys */

  /* adding sharps: sharp the fourth and make the fifth the new key */
  for (m = KEY_SIGNATURE_1_SHARP; m <= KEY_SIGNATURE_7_SHARP; m++)
  {
    S_key_notes_table[m][KEY_MODE_IONIAN][KEY_DEGREE_TONIC]   = S_key_notes_table[m - 1][KEY_MODE_IONIAN][KEY_DEGREE_FIFTH];
    S_key_notes_table[m][KEY_MODE_IONIAN][KEY_DEGREE_SECOND]  = S_key_notes_table[m - 1][KEY_MODE_IONIAN][KEY_DEGREE_SIXTH];
    S_key_notes_table[m][KEY_MODE_IONIAN][KEY_DEGREE_THIRD]   = S_key_notes_table[m - 1][KEY_MODE_IONIAN][KEY_DEGREE_SEVENTH];
    S_key_notes_table[m][KEY_MODE_IONIAN][KEY_DEGREE_FOURTH]  = S_key_notes_table[m - 1][KEY_MODE_IONIAN][KEY_DEGREE_TONIC];
    S_key_notes_table[m][KEY_MODE_IONIAN][KEY_DEGREE_FIFTH]   = S_key_notes_table[m - 1][KEY_MODE_IONIAN][KEY_DEGREE_SECOND];
    S_key_notes_table[m][KEY_MODE_IONIAN][KEY_DEGREE_SIXTH]   = S_key_notes_table[m - 1][KEY_MODE_IONIAN][KEY_DEGREE_THIRD];
    S_key_notes_table[m][KEY_MODE_IONIAN][KEY_DEGREE_SEVENTH] = S_key_notes_table[m - 1][KEY_MODE_IONIAN][KEY_DEGREE_FOURTH] + 1;

    /* make sure the notes are all within a single octave */
    if (S_key_notes_table[m][KEY_MODE_IONIAN][KEY_DEGREE_SEVENTH] >= 12)
      S_key_notes_table[m][KEY_MODE_IONIAN][KEY_DEGREE_SEVENTH] -= 12;
  }

  /* adding flats: flat the seventh and make the fourth the new key */
  for (m = KEY_SIGNATURE_1_FLAT; m >= KEY_SIGNATURE_7_FLAT; m--)
  {
    S_key_notes_table[m][KEY_MODE_IONIAN][KEY_DEGREE_TONIC]   = S_key_notes_table[m + 1][KEY_MODE_IONIAN][KEY_DEGREE_FOURTH];
    S_key_notes_table[m][KEY_MODE_IONIAN][KEY_DEGREE_SECOND]  = S_key_notes_table[m + 1][KEY_MODE_IONIAN][KEY_DEGREE_FIFTH];
    S_key_notes_table[m][KEY_MODE_IONIAN][KEY_DEGREE_THIRD]   = S_key_notes_table[m + 1][KEY_MODE_IONIAN][KEY_DEGREE_SIXTH];
    S_key_notes_table[m][KEY_MODE_IONIAN][KEY_DEGREE_FOURTH]  = S_key_notes_table[m + 1][KEY_MODE_IONIAN][KEY_DEGREE_SEVENTH] - 1;
    S_key_notes_table[m][KEY_MODE_IONIAN][KEY_DEGREE_FIFTH]   = S_key_notes_table[m + 1][KEY_MODE_IONIAN][KEY_DEGREE_TONIC];
    S_key_notes_table[m][KEY_MODE_IONIAN][KEY_DEGREE_SIXTH]   = S_key_notes_table[m + 1][KEY_MODE_IONIAN][KEY_DEGREE_SECOND];
    S_key_notes_table[m][KEY_MODE_IONIAN][KEY_DEGREE_SEVENTH] = S_key_notes_table[m + 1][KEY_MODE_IONIAN][KEY_DEGREE_THIRD];

    /* make sure the notes are all within a single octave */
    if (S_key_notes_table[m][KEY_MODE_IONIAN][KEY_DEGREE_FOURTH] < 0)
      S_key_notes_table[m][KEY_MODE_IONIAN][KEY_DEGREE_FOURTH] += 12;
  }

  /* compute modes of each key */
  for (m = 0; m < KEY_NUM_SIGNATURES; m++)
  {
    for (n = KEY_MODE_DORIAN; n <= KEY_MODE_LOCRIAN; n++)
    {
      S_key_notes_table[m][n][KEY_DEGREE_TONIC]   = S_key_notes_table[m][n - 1][KEY_DEGREE_SECOND];
      S_key_notes_table[m][n][KEY_DEGREE_SECOND]  = S_key_notes_table[m][n - 1][KEY_DEGREE_THIRD];
      S_key_notes_table[m][n][KEY_DEGREE_THIRD]   = S_key_notes_table[m][n - 1][KEY_DEGREE_FOURTH];
      S_key_notes_table[m][n][KEY_DEGREE_FOURTH]  = S_key_notes_table[m][n - 1][KEY_DEGREE_FIFTH];
      S_key_notes_table[m][n][KEY_DEGREE_FIFTH]   = S_key_notes_table[m][n - 1][KEY_DEGREE_SIXTH];
      S_key_notes_table[m][n][KEY_DEGREE_SIXTH]   = S_key_notes_table[m][n - 1][KEY_DEGREE_SEVENTH];
      S_key_notes_table[m][n][KEY_DEGREE_SEVENTH] = S_key_notes_table[m][n - 1][KEY_DEGREE_TONIC];
    }
  }

#if 0
  /* testing: print out the octave for all keys! */
  for (m = 0; m < KEY_NUM_SIGNATURES; m++)
  {
    printf("Major Key %d: ", m);

    for (n = 0; n < KEY_NUM_DEGREES; n++)
      printf("%d ", S_key_notes_table[m][KEY_MODE_IONIAN][n]);

    printf("\n");
  }

  for (m = 0; m < KEY_NUM_SIGNATURES; m++)
  {
    printf("Minor Key %d: ", m);

    for (n = 0; n < KEY_NUM_DEGREES; n++)
      printf("%d ", S_key_notes_table[m][KEY_MODE_AEOLIAN][n]);

    printf("\n");
  }
#endif

  return 0;
}
