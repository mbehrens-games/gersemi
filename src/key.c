/*******************************************************************************
** key.c (musical keys)
*******************************************************************************/

#include <stdio.h>  /* testing */
#include <stdlib.h>
#include <math.h>

#include "bank.h"
#include "key.h"
#include "tuning.h"

/* key table */

/* keys: 15 key signatures (0-7 sharps, 0-7 flats)  */
/* modes: 7 (major, minor, dorian, lydian, etc)     */
/* notes: 36 (7 per octave, 5 octaves, + 1 extra)   */
static int S_key_to_note_table[15][7][36];

/* key signatures */
key G_key_signature_bank[BANK_NUM_KEY_SETTINGS];
key G_key_modulation_bank[BANK_NUM_KEY_SETTINGS];

/*******************************************************************************
** key_setup_all()
*******************************************************************************/
short int key_setup_all()
{
  int k;

  /* setup all lfos */
  for (k = 0; k < BANK_NUM_VOICES; k++)
    key_reset(k);

  return 0;
}

/*******************************************************************************
** key_reset()
*******************************************************************************/
short int key_reset(int voice_index)
{
  key* ks;
  key* km;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain key pointers */
  ks = &G_key_signature_bank[voice_index];
  km = &G_key_modulation_bank[voice_index];

  /* reset values */
  ks->sig = 7;
  ks->mode = 0;

  km->sig = 0;
  km->mode = 0;

  return 0;
}

#if 0
/*******************************************************************************
** key_set_key_signature()
*******************************************************************************/
short int key_set_key_signature(int voice_index, int signature)
{
  int bank_index;

  /* determine bank index */
  if (BANK_INDEX_IS_IN_SFX_VOICES(voice_index))
  {
    bank_index = voice_index - BANK_SFX_VOICES_START_INDEX;
  }
  else if ( BANK_INDEX_IS_IN_DRUM_VOICES(voice_index) || 
            BANK_INDEX_IS_IN_INST_VOICES(voice_index))
  {
    bank_index = BANK_SFX_VOICES_END_INDEX - BANK_SFX_VOICES_START_INDEX;
  }
  else
    return 0;

  /* set table indices */
  if (signature == KEY_SIGNATURE_C_FLAT_MAJOR)
  {
    G_key_index_bank[bank_index] = 0;
    G_key_mode_bank[bank_index] = 0;
  }
  else if (signature == KEY_SIGNATURE_G_FLAT_MAJOR)
  {
    G_key_index_bank[bank_index] = 1;
    G_key_mode_bank[bank_index] = 0;
  }
  else if (signature == KEY_SIGNATURE_D_FLAT_MAJOR)
  {
    G_key_index_bank[bank_index] = 2;
    G_key_mode_bank[bank_index] = 0;
  }
  else if (signature == KEY_SIGNATURE_A_FLAT_MAJOR)
  {
    G_key_index_bank[bank_index] = 3;
    G_key_mode_bank[bank_index] = 0;
  }
  else if (signature == KEY_SIGNATURE_E_FLAT_MAJOR)
  {
    G_key_index_bank[bank_index] = 4;
    G_key_mode_bank[bank_index] = 0;
  }
  else if (signature == KEY_SIGNATURE_B_FLAT_MAJOR)
  {
    G_key_index_bank[bank_index] = 5;
    G_key_mode_bank[bank_index] = 0;
  }
  else if (signature == KEY_SIGNATURE_F_MAJOR)
  {
    G_key_index_bank[bank_index] = 6;
    G_key_mode_bank[bank_index] = 0;
  }
  else if (signature == KEY_SIGNATURE_C_MAJOR)
  {
    G_key_index_bank[bank_index] = 7;
    G_key_mode_bank[bank_index] = 0;
  }
  else if (signature == KEY_SIGNATURE_G_MAJOR)
  {
    G_key_index_bank[bank_index] = 8;
    G_key_mode_bank[bank_index] = 0;
  }
  else if (signature == KEY_SIGNATURE_D_MAJOR)
  {
    G_key_index_bank[bank_index] = 9;
    G_key_mode_bank[bank_index] = 0;
  }
  else if (signature == KEY_SIGNATURE_A_MAJOR)
  {
    G_key_index_bank[bank_index] = 10;
    G_key_mode_bank[bank_index] = 0;
  }
  else if (signature == KEY_SIGNATURE_E_MAJOR)
  {
    G_key_index_bank[bank_index] = 11;
    G_key_mode_bank[bank_index] = 0;
  }
  else if (signature == KEY_SIGNATURE_B_MAJOR)
  {
    G_key_index_bank[bank_index] = 12;
    G_key_mode_bank[bank_index] = 0;
  }
  else if (signature == KEY_SIGNATURE_F_SHARP_MAJOR)
  {
    G_key_index_bank[bank_index] = 13;
    G_key_mode_bank[bank_index] = 0;
  }
  else if (signature == KEY_SIGNATURE_C_SHARP_MAJOR)
  {
    G_key_index_bank[bank_index] = 14;
    G_key_mode_bank[bank_index] = 0;
  }
  else if (signature == KEY_SIGNATURE_A_FLAT_MINOR)
  {
    G_key_index_bank[bank_index] = 0;
    G_key_mode_bank[bank_index] = 5;
  }
  else if (signature == KEY_SIGNATURE_E_FLAT_MINOR)
  {
    G_key_index_bank[bank_index] = 1;
    G_key_mode_bank[bank_index] = 5;
  }
  else if (signature == KEY_SIGNATURE_B_FLAT_MINOR)
  {
    G_key_index_bank[bank_index] = 2;
    G_key_mode_bank[bank_index] = 5;
  }
  else if (signature == KEY_SIGNATURE_F_MINOR)
  {
    G_key_index_bank[bank_index] = 3;
    G_key_mode_bank[bank_index] = 5;
  }
  else if (signature == KEY_SIGNATURE_C_MINOR)
  {
    G_key_index_bank[bank_index] = 4;
    G_key_mode_bank[bank_index] = 5;
  }
  else if (signature == KEY_SIGNATURE_G_MINOR)
  {
    G_key_index_bank[bank_index] = 5;
    G_key_mode_bank[bank_index] = 5;
  }
  else if (signature == KEY_SIGNATURE_D_MINOR)
  {
    G_key_index_bank[bank_index] = 6;
    G_key_mode_bank[bank_index] = 5;
  }
  else if (signature == KEY_SIGNATURE_A_MINOR)
  {
    G_key_index_bank[bank_index] = 7;
    G_key_mode_bank[bank_index] = 5;
  }
  else if (signature == KEY_SIGNATURE_E_MINOR)
  {
    G_key_index_bank[bank_index] = 8;
    G_key_mode_bank[bank_index] = 5;
  }
  else if (signature == KEY_SIGNATURE_B_MINOR)
  {
    G_key_index_bank[bank_index] = 9;
    G_key_mode_bank[bank_index] = 5;
  }
  else if (signature == KEY_SIGNATURE_F_SHARP_MINOR)
  {
    G_key_index_bank[bank_index] = 10;
    G_key_mode_bank[bank_index] = 5;
  }
  else if (signature == KEY_SIGNATURE_C_SHARP_MINOR)
  {
    G_key_index_bank[bank_index] = 11;
    G_key_mode_bank[bank_index] = 5;
  }
  else if (signature == KEY_SIGNATURE_G_SHARP_MINOR)
  {
    G_key_index_bank[bank_index] = 12;
    G_key_mode_bank[bank_index] = 5;
  }
  else if (signature == KEY_SIGNATURE_D_SHARP_MINOR)
  {
    G_key_index_bank[bank_index] = 13;
    G_key_mode_bank[bank_index] = 5;
  }
  else if (signature == KEY_SIGNATURE_A_SHARP_MINOR)
  {
    G_key_index_bank[bank_index] = 14;
    G_key_mode_bank[bank_index] = 5;
  }
  else
  {
    G_key_index_bank[bank_index] = 7;
    G_key_mode_bank[bank_index] = 0;
  }

  return 0;
}
#endif

/*******************************************************************************
** key_note_lookup()
*******************************************************************************/
int key_note_lookup(int voice_index, int octave, int degree)
{
  int note;

  int sig_index;
  int mode_index;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 0;

  /* make sure that the octave and degree are valid */
  if ((octave < 2) || (octave > 6))
    return 0;

  if ((degree < 0) || (degree > 7))
    return 0;

  /* determine adjusted indices */
  sig_index = G_key_signature_bank[voice_index].sig;
  mode_index = G_key_signature_bank[voice_index].mode;

  /* lookup note */
  note = S_key_to_note_table[sig_index][mode_index][(octave - 2) * 7 + degree];

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
  S_key_to_note_table[7][0][2 * 7 + 0] = 12 * TUNING_MIDDLE_OCTAVE + 0;
  S_key_to_note_table[7][0][2 * 7 + 1] = 12 * TUNING_MIDDLE_OCTAVE + 2;
  S_key_to_note_table[7][0][2 * 7 + 2] = 12 * TUNING_MIDDLE_OCTAVE + 4;
  S_key_to_note_table[7][0][2 * 7 + 3] = 12 * TUNING_MIDDLE_OCTAVE + 5;
  S_key_to_note_table[7][0][2 * 7 + 4] = 12 * TUNING_MIDDLE_OCTAVE + 7;
  S_key_to_note_table[7][0][2 * 7 + 5] = 12 * TUNING_MIDDLE_OCTAVE + 9;
  S_key_to_note_table[7][0][2 * 7 + 6] = 12 * TUNING_MIDDLE_OCTAVE + 11;

  /* compute notes in other keys */

  /* note that if the tonic is G, A, or B, it should be below middle C */
  /*           if the tonic is D, E, or F, it should be above middle C */

  /* adding sharps: sharp the fourth and make the fifth the new tonic */
  for (m = 8; m < 15; m++)
  {
    /* apply transformation */
    S_key_to_note_table[m][0][2 * 7 + 0] = S_key_to_note_table[m - 1][0][2 * 7 + 4];
    S_key_to_note_table[m][0][2 * 7 + 1] = S_key_to_note_table[m - 1][0][2 * 7 + 5];
    S_key_to_note_table[m][0][2 * 7 + 2] = S_key_to_note_table[m - 1][0][2 * 7 + 6];
    S_key_to_note_table[m][0][2 * 7 + 3] = S_key_to_note_table[m - 1][0][2 * 7 + 0];
    S_key_to_note_table[m][0][2 * 7 + 4] = S_key_to_note_table[m - 1][0][2 * 7 + 1];
    S_key_to_note_table[m][0][2 * 7 + 5] = S_key_to_note_table[m - 1][0][2 * 7 + 2];
    S_key_to_note_table[m][0][2 * 7 + 6] = S_key_to_note_table[m - 1][0][2 * 7 + 3] + 1;

    /* adjust notes so that they are in ascending order */
    S_key_to_note_table[m][0][2 * 7 + 0] -= 12;
    S_key_to_note_table[m][0][2 * 7 + 1] -= 12;
    S_key_to_note_table[m][0][2 * 7 + 2] -= 12;

    /* apply octave adjustment if necessary */

    /* if the tonic is less than Gb3, shift up one octave */
    while (S_key_to_note_table[m][0][2 * 7 + 0] < 3 * 12 + 6)
    {
      for (k = 0; k < 7; k++)
        S_key_to_note_table[m][0][2 * 7 + k] += 12;
    }

    /* if the tonic is more than F#4, shift down one octave */
    while (S_key_to_note_table[m][0][2 * 7 + 0] > 4 * 12 + 6)
    {
      for (k = 0; k < 7; k++)
        S_key_to_note_table[m][0][2 * 7 + k] -= 12;
    }
  }

  /* adding flats: flat the seventh and make the fourth the new tonic */
  for (m = 6; m >= 0; m--)
  {
    /* apply transformation */
    S_key_to_note_table[m][0][2 * 7 + 0] = S_key_to_note_table[m + 1][0][2 * 7 + 3];
    S_key_to_note_table[m][0][2 * 7 + 1] = S_key_to_note_table[m + 1][0][2 * 7 + 4];
    S_key_to_note_table[m][0][2 * 7 + 2] = S_key_to_note_table[m + 1][0][2 * 7 + 5];
    S_key_to_note_table[m][0][2 * 7 + 3] = S_key_to_note_table[m + 1][0][2 * 7 + 6] - 1;
    S_key_to_note_table[m][0][2 * 7 + 4] = S_key_to_note_table[m + 1][0][2 * 7 + 0];
    S_key_to_note_table[m][0][2 * 7 + 5] = S_key_to_note_table[m + 1][0][2 * 7 + 1];
    S_key_to_note_table[m][0][2 * 7 + 6] = S_key_to_note_table[m + 1][0][2 * 7 + 2];

    /* adjust notes so that they are in ascending order */
    S_key_to_note_table[m][0][2 * 7 + 4] += 12;
    S_key_to_note_table[m][0][2 * 7 + 5] += 12;
    S_key_to_note_table[m][0][2 * 7 + 6] += 12;

    /* apply octave adjustment if necessary */

    /* if the tonic is less than Gb3, shift up one octave */
    while (S_key_to_note_table[m][0][2 * 7 + 0] < 3 * 12 + 6)
    {
      for (k = 0; k < 7; k++)
        S_key_to_note_table[m][0][2 * 7 + k] += 12;
    }

    /* if the tonic is more than F#4, shift down one octave */
    while (S_key_to_note_table[m][0][2 * 7 + 0] > 4 * 12 + 6)
    {
      for (k = 0; k < 7; k++)
        S_key_to_note_table[m][0][2 * 7 + k] -= 12;
    }
  }

  /* compute modes of each key */
  for (m = 0; m < 15; m++)
  {
    for (n = 1; n < 7; n++)
    {
      /* apply transformation */
      S_key_to_note_table[m][n][2 * 7 + 0] = S_key_to_note_table[m][n - 1][2 * 7 + 1];
      S_key_to_note_table[m][n][2 * 7 + 1] = S_key_to_note_table[m][n - 1][2 * 7 + 2];
      S_key_to_note_table[m][n][2 * 7 + 2] = S_key_to_note_table[m][n - 1][2 * 7 + 3];
      S_key_to_note_table[m][n][2 * 7 + 3] = S_key_to_note_table[m][n - 1][2 * 7 + 4];
      S_key_to_note_table[m][n][2 * 7 + 4] = S_key_to_note_table[m][n - 1][2 * 7 + 5];
      S_key_to_note_table[m][n][2 * 7 + 5] = S_key_to_note_table[m][n - 1][2 * 7 + 6];
      S_key_to_note_table[m][n][2 * 7 + 6] = S_key_to_note_table[m][n - 1][2 * 7 + 0];

      /* adjust notes so that they are in ascending order */
      S_key_to_note_table[m][n][2 * 7 + 6] += 12;

      /* apply octave adjustment if necessary */

      /* if the tonic is less than Gb3, shift up one octave */
      while (S_key_to_note_table[m][n][2 * 7 + 0] < 3 * 12 + 6)
      {
        for (k = 0; k < 7; k++)
          S_key_to_note_table[m][n][2 * 7 + k] += 12;
      }

      /* if the tonic is more than F#4, shift down one octave */
      while (S_key_to_note_table[m][n][2 * 7 + 0] > 4 * 12 + 6)
      {
        for (k = 0; k < 7; k++)
          S_key_to_note_table[m][n][2 * 7 + k] -= 12;
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
        S_key_to_note_table[m][n][0 * 7 + k] = S_key_to_note_table[m][n][2 * 7 + k] - 24;
        S_key_to_note_table[m][n][1 * 7 + k] = S_key_to_note_table[m][n][2 * 7 + k] - 12;
        S_key_to_note_table[m][n][3 * 7 + k] = S_key_to_note_table[m][n][2 * 7 + k] + 12;
        S_key_to_note_table[m][n][4 * 7 + k] = S_key_to_note_table[m][n][2 * 7 + k] + 24;
      }

      S_key_to_note_table[m][n][5 * 7 + 0] = S_key_to_note_table[m][n][2 * 7 + 0] + 36;
    }
  }

#if 0
  /* testing: print out the middle octaves for all keys! */
  for (m = 0; m < 15; m++)
  {
    printf("Major Key %d: ", m);

    for (k = 0; k < 8; k++)
      printf("%d ", S_key_to_note_table[m][0][2 * 7 + k]);

    printf("\n");
  }

  for (m = 0; m < 15; m++)
  {
    printf("Minor Key %d: ", m);

    for (k = 0; k < 8; k++)
      printf("%d ", S_key_to_note_table[m][5][2 * 7 + k]);

    printf("\n");
  }
#endif

  return 0;
}
