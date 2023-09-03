/*******************************************************************************
** key.c (musical keys)
*******************************************************************************/

#include <stdio.h>  /* testing */
#include <stdlib.h>
#include <math.h>

#include "key.h"
#include "tuning.h"

/* key table */

/* keys: 15 key signatures (0-7 sharps, clear, 0-7 flats) */
/* modes: 7 (major, minor, dorian, lydian, etc)           */
/* notes: 50 (7 per octave, 7 octaves, + 1 extra)         */
static int S_key_to_note_table[KEY_NUM_SIGS][KEY_NUM_MODES][KEY_NUM_NOTES];

/*******************************************************************************
** key_note_lookup()
*******************************************************************************/
int key_note_lookup(int signature, int mode, int octave, int degree)
{
  int index;
  int note;

  /* make sure the key signature and mode are valid */
  if ((signature < 0) || (signature >= KEY_NUM_SIGS))
    return 0;

  if ((mode < 0) || (mode >= KEY_NUM_MODES))
    return 0;

  /* make sure that the octave and degree are valid */
  if ((octave < 0) || (octave >= KEY_NUM_OCTAVES))
    return 0;

  if ((degree < 0) || (degree >= KEY_NUM_DEGREES))
    return 0;

  /* determine the note index based on the octave and degree */
  index = 7 * octave + degree;

  if (index < 0)
    index = 0;
  else if (index > KEY_NUM_NOTES)
    index = KEY_NUM_NOTES;

  /* lookup note */
  note = S_key_to_note_table[signature][mode][index];

  return note;
}

/*******************************************************************************
** key_generate_tables()
*******************************************************************************/
short int key_generate_tables()
{
  int m;
  int n;
  int k;

  /* key table */

  /* set notes for c major in the middle octave */
  S_key_to_note_table[7][0][7 * KEY_MIDDLE_OCTAVE + 0] = 12 * TUNING_MIDDLE_OCTAVE + 0;
  S_key_to_note_table[7][0][7 * KEY_MIDDLE_OCTAVE + 1] = 12 * TUNING_MIDDLE_OCTAVE + 2;
  S_key_to_note_table[7][0][7 * KEY_MIDDLE_OCTAVE + 2] = 12 * TUNING_MIDDLE_OCTAVE + 4;
  S_key_to_note_table[7][0][7 * KEY_MIDDLE_OCTAVE + 3] = 12 * TUNING_MIDDLE_OCTAVE + 5;
  S_key_to_note_table[7][0][7 * KEY_MIDDLE_OCTAVE + 4] = 12 * TUNING_MIDDLE_OCTAVE + 7;
  S_key_to_note_table[7][0][7 * KEY_MIDDLE_OCTAVE + 5] = 12 * TUNING_MIDDLE_OCTAVE + 9;
  S_key_to_note_table[7][0][7 * KEY_MIDDLE_OCTAVE + 6] = 12 * TUNING_MIDDLE_OCTAVE + 11;

  /* compute notes in other keys */

  /* note that if the tonic is G, A, or B, it should be below middle C */
  /*           if the tonic is D, E, or F, it should be above middle C */

  /* adding sharps: sharp the fourth and make the fifth the new tonic */
  for (m = 8; m < 15; m++)
  {
    /* apply transformation */
    S_key_to_note_table[m][0][7 * KEY_MIDDLE_OCTAVE + 0] = S_key_to_note_table[m - 1][0][7 * KEY_MIDDLE_OCTAVE + 4];
    S_key_to_note_table[m][0][7 * KEY_MIDDLE_OCTAVE + 1] = S_key_to_note_table[m - 1][0][7 * KEY_MIDDLE_OCTAVE + 5];
    S_key_to_note_table[m][0][7 * KEY_MIDDLE_OCTAVE + 2] = S_key_to_note_table[m - 1][0][7 * KEY_MIDDLE_OCTAVE + 6];
    S_key_to_note_table[m][0][7 * KEY_MIDDLE_OCTAVE + 3] = S_key_to_note_table[m - 1][0][7 * KEY_MIDDLE_OCTAVE + 0];
    S_key_to_note_table[m][0][7 * KEY_MIDDLE_OCTAVE + 4] = S_key_to_note_table[m - 1][0][7 * KEY_MIDDLE_OCTAVE + 1];
    S_key_to_note_table[m][0][7 * KEY_MIDDLE_OCTAVE + 5] = S_key_to_note_table[m - 1][0][7 * KEY_MIDDLE_OCTAVE + 2];
    S_key_to_note_table[m][0][7 * KEY_MIDDLE_OCTAVE + 6] = S_key_to_note_table[m - 1][0][7 * KEY_MIDDLE_OCTAVE + 3] + 1;

    /* adjust notes so that they are in ascending order */
    S_key_to_note_table[m][0][7 * KEY_MIDDLE_OCTAVE + 0] -= 12;
    S_key_to_note_table[m][0][7 * KEY_MIDDLE_OCTAVE + 1] -= 12;
    S_key_to_note_table[m][0][7 * KEY_MIDDLE_OCTAVE + 2] -= 12;

    /* apply octave adjustment if necessary */

    /* if the tonic is less than Gb3, shift up one octave */
    while (S_key_to_note_table[m][0][7 * KEY_MIDDLE_OCTAVE + 0] < 12 * (TUNING_MIDDLE_OCTAVE - 1) + 6)
    {
      for (k = 0; k < 7; k++)
        S_key_to_note_table[m][0][7 * KEY_MIDDLE_OCTAVE + k] += 12;
    }

    /* if the tonic is more than F#4, shift down one octave */
    while (S_key_to_note_table[m][0][7 * KEY_MIDDLE_OCTAVE + 0] > 12 * TUNING_MIDDLE_OCTAVE + 6)
    {
      for (k = 0; k < 7; k++)
        S_key_to_note_table[m][0][7 * KEY_MIDDLE_OCTAVE + k] -= 12;
    }
  }

  /* adding flats: flat the seventh and make the fourth the new tonic */
  for (m = 6; m >= 0; m--)
  {
    /* apply transformation */
    S_key_to_note_table[m][0][7 * KEY_MIDDLE_OCTAVE + 0] = S_key_to_note_table[m + 1][0][7 * KEY_MIDDLE_OCTAVE + 3];
    S_key_to_note_table[m][0][7 * KEY_MIDDLE_OCTAVE + 1] = S_key_to_note_table[m + 1][0][7 * KEY_MIDDLE_OCTAVE + 4];
    S_key_to_note_table[m][0][7 * KEY_MIDDLE_OCTAVE + 2] = S_key_to_note_table[m + 1][0][7 * KEY_MIDDLE_OCTAVE + 5];
    S_key_to_note_table[m][0][7 * KEY_MIDDLE_OCTAVE + 3] = S_key_to_note_table[m + 1][0][7 * KEY_MIDDLE_OCTAVE + 6] - 1;
    S_key_to_note_table[m][0][7 * KEY_MIDDLE_OCTAVE + 4] = S_key_to_note_table[m + 1][0][7 * KEY_MIDDLE_OCTAVE + 0];
    S_key_to_note_table[m][0][7 * KEY_MIDDLE_OCTAVE + 5] = S_key_to_note_table[m + 1][0][7 * KEY_MIDDLE_OCTAVE + 1];
    S_key_to_note_table[m][0][7 * KEY_MIDDLE_OCTAVE + 6] = S_key_to_note_table[m + 1][0][7 * KEY_MIDDLE_OCTAVE + 2];

    /* adjust notes so that they are in ascending order */
    S_key_to_note_table[m][0][7 * KEY_MIDDLE_OCTAVE + 4] += 12;
    S_key_to_note_table[m][0][7 * KEY_MIDDLE_OCTAVE + 5] += 12;
    S_key_to_note_table[m][0][7 * KEY_MIDDLE_OCTAVE + 6] += 12;

    /* apply octave adjustment if necessary */

    /* if the tonic is less than Gb3, shift up one octave */
    while (S_key_to_note_table[m][0][7 * KEY_MIDDLE_OCTAVE + 0] < 12 * (TUNING_MIDDLE_OCTAVE - 1) + 6)
    {
      for (k = 0; k < 7; k++)
        S_key_to_note_table[m][0][7 * KEY_MIDDLE_OCTAVE + k] += 12;
    }

    /* if the tonic is more than F#4, shift down one octave */
    while (S_key_to_note_table[m][0][7 * KEY_MIDDLE_OCTAVE + 0] > 12 * TUNING_MIDDLE_OCTAVE + 6)
    {
      for (k = 0; k < 7; k++)
        S_key_to_note_table[m][0][7 * KEY_MIDDLE_OCTAVE + k] -= 12;
    }
  }

  /* compute modes of each key */
  for (m = 0; m < 15; m++)
  {
    for (n = 1; n < 7; n++)
    {
      /* apply transformation */
      S_key_to_note_table[m][n][7 * KEY_MIDDLE_OCTAVE + 0] = S_key_to_note_table[m][n - 1][7 * KEY_MIDDLE_OCTAVE + 1];
      S_key_to_note_table[m][n][7 * KEY_MIDDLE_OCTAVE + 1] = S_key_to_note_table[m][n - 1][7 * KEY_MIDDLE_OCTAVE + 2];
      S_key_to_note_table[m][n][7 * KEY_MIDDLE_OCTAVE + 2] = S_key_to_note_table[m][n - 1][7 * KEY_MIDDLE_OCTAVE + 3];
      S_key_to_note_table[m][n][7 * KEY_MIDDLE_OCTAVE + 3] = S_key_to_note_table[m][n - 1][7 * KEY_MIDDLE_OCTAVE + 4];
      S_key_to_note_table[m][n][7 * KEY_MIDDLE_OCTAVE + 4] = S_key_to_note_table[m][n - 1][7 * KEY_MIDDLE_OCTAVE + 5];
      S_key_to_note_table[m][n][7 * KEY_MIDDLE_OCTAVE + 5] = S_key_to_note_table[m][n - 1][7 * KEY_MIDDLE_OCTAVE + 6];
      S_key_to_note_table[m][n][7 * KEY_MIDDLE_OCTAVE + 6] = S_key_to_note_table[m][n - 1][7 * KEY_MIDDLE_OCTAVE + 0];

      /* adjust notes so that they are in ascending order */
      S_key_to_note_table[m][n][7 * KEY_MIDDLE_OCTAVE + 6] += 12;

      /* apply octave adjustment if necessary */

      /* if the tonic is less than Gb3, shift up one octave */
      while (S_key_to_note_table[m][n][7 * KEY_MIDDLE_OCTAVE + 0] < 12 * (TUNING_MIDDLE_OCTAVE - 1) + 6)
      {
        for (k = 0; k < 7; k++)
          S_key_to_note_table[m][n][7 * KEY_MIDDLE_OCTAVE + k] += 12;
      }

      /* if the tonic is more than F#4, shift down one octave */
      while (S_key_to_note_table[m][n][7 * KEY_MIDDLE_OCTAVE + 0] > 12 * TUNING_MIDDLE_OCTAVE + 6)
      {
        for (k = 0; k < 7; k++)
          S_key_to_note_table[m][n][7 * KEY_MIDDLE_OCTAVE + k] -= 12;
      }
    }
  }

  /* compute notes for all keys & modes in the other octaves */
  for (m = 0; m < 15; m++)
  {
    for (n = 0; n < 7; n++)
    {
      for (k = 0; k < 7; k++)
      {
        S_key_to_note_table[m][n][7 * (KEY_MIDDLE_OCTAVE - 3) + k] = S_key_to_note_table[m][n][7 * KEY_MIDDLE_OCTAVE + k] - 36;
        S_key_to_note_table[m][n][7 * (KEY_MIDDLE_OCTAVE - 2) + k] = S_key_to_note_table[m][n][7 * KEY_MIDDLE_OCTAVE + k] - 24;
        S_key_to_note_table[m][n][7 * (KEY_MIDDLE_OCTAVE - 1) + k] = S_key_to_note_table[m][n][7 * KEY_MIDDLE_OCTAVE + k] - 12;
        S_key_to_note_table[m][n][7 * (KEY_MIDDLE_OCTAVE + 1) + k] = S_key_to_note_table[m][n][7 * KEY_MIDDLE_OCTAVE + k] + 12;
        S_key_to_note_table[m][n][7 * (KEY_MIDDLE_OCTAVE + 2) + k] = S_key_to_note_table[m][n][7 * KEY_MIDDLE_OCTAVE + k] + 24;
        S_key_to_note_table[m][n][7 * (KEY_MIDDLE_OCTAVE + 3) + k] = S_key_to_note_table[m][n][7 * KEY_MIDDLE_OCTAVE + k] + 36;
      }

      S_key_to_note_table[m][n][7 * (KEY_MIDDLE_OCTAVE + 4) + 0] = S_key_to_note_table[m][n][7 * KEY_MIDDLE_OCTAVE + 0] + 48;
    }
  }

#if 0
  /* testing: print out the middle octaves for all keys! */
  for (m = 0; m < 15; m++)
  {
    printf("Major Key %d: ", m);

    for (k = 0; k < 8; k++)
      printf("%d ", S_key_to_note_table[m][KEY_MODE_MAJOR][7 * KEY_MIDDLE_OCTAVE + k]);

    printf("\n");
  }

  for (m = 0; m < 15; m++)
  {
    printf("Minor Key %d: ", m);

    for (k = 0; k < 8; k++)
      printf("%d ", S_key_to_note_table[m][KEY_MODE_MINOR][7 * KEY_MIDDLE_OCTAVE + k]);

    printf("\n");
  }
#endif

  return 0;
}
