/*******************************************************************************
** tuning.c (tuning systems)
*******************************************************************************/

#include <math.h>

#include "tuning.h"

#define PI      3.14159265358979323846f
#define TWO_PI  6.28318530717958647693f

/* multipliers from c in 12 tone equal temperament */
static float S_tuning_mult_12_equal_temperament[12] = 
        { 1.0,                  /* C  */
          1.059463094359293,    /* C# */
          1.122462048309375,    /* D  */
          1.189207115002721,    /* D# */
          1.259921049894870,    /* E  */
          1.334839854170037,    /* F  */
          1.414213562373095,    /* F# */
          1.498307076876678,    /* G  */
          1.587401051968203,    /* G# */
          1.681792830507429,    /* A  */
          1.781797436280675,    /* A# */
          1.887748625363391};   /* B  */

/* multipliers from c in pythagorean tuning */
static float S_tuning_mult_pythagorean[12] = 
        { 1.0,                  /* 1:1        C  */
          1.06787109375,        /* 2187:2048  C# */
          1.125,                /* 9:8        D  */
          1.18518518518519,     /* 32:27      Eb */
          1.265625,             /* 81:64      E  */
          1.33333333333333,     /* 4:3        F  */
          1.423828125,          /* 729:512    F# */
          1.5,                  /* 3:2        G  */
          1.601806640625,       /* 6561:4096  G# */
          1.6875,               /* 27:16      A  */
          1.77777777777778,     /* 16:9       Bb */
          1.8984375};           /* 243:128    B  */

/* multipliers from c in quarter comma meantone */
static float S_tuning_mult_quarter_comma_meantone[12] = 
        { 1.0,                  /* 1:1        C  */
          1.04490672652566,     /* 5^(7/4)/16 C# */
          1.1180339887499,      /* 5^(1/2):2  D  */
          1.19627902497696,     /* 4:5^(3/4)  Eb */
          1.25,                 /* 5:4        E  */
          1.33748060995284,     /* 2:5^(1/4)  F  */
          1.39754248593737,     /* 5^(3/2):8  F# */
          1.495348781221221,    /* 5^(1/4):1  G  */
          1.5625,               /* 25:16      G# */
          1.67185076244106,     /* 5^(3/4):2  A  */
          1.78885438199984,     /* 4:5^(1/2)  Bb */
          1.86918597652653};    /* 5^(5/4):4  B  */

/* multipliers from c in werckmeister iii */
static float S_tuning_mult_werckmeister_iii[12] = 
        { 1.0,                  /* 1:1              C  */
          1.05349794238683,     /* 256:243          C# */
          1.1174033085417,      /* 64*2^(1/2):81    D  */
          1.18518518518519,     /* 32:27            D# */
          1.25282724872715,     /* 256*2^(1/4):243  E  */
          1.33333333333333,     /* 4:3              F  */
          1.40466392318244,     /* 1024:729         F# */
          1.49492696045105,     /* 8^(5/4):9        G  */
          1.58024691358025,     /* 128:81           G# */
          1.6704363316362,      /* 1024*2^(1/4):729 A  */
          1.77777777777778,     /* 16:9             Bb */
          1.87924087309072};    /* 128*2^(1/4):81   B  */

/* tuning system & fork */
static int S_tuning_system;
static int S_tuning_fork;

/* offset table */
short int G_tuning_offset_table[12];

/*******************************************************************************
** tuning_generate_offsets()
*******************************************************************************/
short int tuning_generate_offsets()
{
  int     m;

  float*  mult_table;

  float   freq_fork;

  short int offset;

  /* reset tuning offsets */
  for (m = 0; m < 12; m++)
    G_tuning_offset_table[m] = 0;

  /* apply offset from tuning fork (same for all notes) */
  if (S_tuning_fork == TUNING_FORK_A430)
    freq_fork = 430.0f;
  else if (S_tuning_fork == TUNING_FORK_A432)
    freq_fork = 432.0f;
  else if (S_tuning_fork == TUNING_FORK_A434)
    freq_fork = 434.0f;
  else if (S_tuning_fork == TUNING_FORK_A436)
    freq_fork = 436.0f;
  else if (S_tuning_fork == TUNING_FORK_A438)
    freq_fork = 438.0f;
  else if (S_tuning_fork == TUNING_FORK_A440)
    freq_fork = 440.0f;
  else if (S_tuning_fork == TUNING_FORK_A442)
    freq_fork = 442.0f;
  else if (S_tuning_fork == TUNING_FORK_A444)
    freq_fork = 444.0f;
  else
    freq_fork = 440.0f;

  offset = (short int) (12 * TUNING_NUM_SEMITONE_STEPS * ((log(freq_fork) - log(440)) / log(2)) + 0.5f);

  for (m = 0; m < 12; m++)
    G_tuning_offset_table[m] += offset;

  /* apply offset from tuning system (different for each note) */
  if (S_tuning_system == TUNING_SYSTEM_12_EQUAL_TEMPERAMENT)
    mult_table = S_tuning_mult_12_equal_temperament;
  else if (S_tuning_system == TUNING_SYSTEM_PYTHAGOREAN)
    mult_table = S_tuning_mult_pythagorean;
  else if (S_tuning_system == TUNING_SYSTEM_QUARTER_COMMA_MEANTONE)
    mult_table = S_tuning_mult_quarter_comma_meantone;
  else if (S_tuning_system == TUNING_SYSTEM_WERCKMEISTER_III)
    mult_table = S_tuning_mult_werckmeister_iii;
  else
    mult_table = S_tuning_mult_12_equal_temperament;

  for (m = 0; m < 12; m++)
  {
    offset = (short int) (12 * TUNING_NUM_SEMITONE_STEPS * ((log(mult_table[m]) - log(S_tuning_mult_12_equal_temperament[m])) / log(2)) + 0.5f);

    G_tuning_offset_table[m] += offset;
  }

  return 0;
}

/*******************************************************************************
** tuning_reset()
*******************************************************************************/
short int tuning_reset()
{
  S_tuning_system = TUNING_SYSTEM_12_EQUAL_TEMPERAMENT;
  S_tuning_fork = TUNING_FORK_A440;

  tuning_generate_offsets();

  return 0;
}

/*******************************************************************************
** tuning_set_system()
*******************************************************************************/
short int tuning_set_system(int system)
{
  /* make sure system is valid */
  if ((system < 0) || (system >= TUNING_NUM_SYSTEMS))
    return 1;

  /* set tuning system and recalculate tables */
  S_tuning_system = system;

  tuning_generate_offsets();

  return 0;
}

/*******************************************************************************
** tuning_set_fork()
*******************************************************************************/
short int tuning_set_fork(int fork)
{
  /* make sure fork is valid */
  if ((fork < 0) || (fork >= TUNING_NUM_FORKS))
    return 1;

  /* set tuning fork and recalculate tables */
  S_tuning_fork = fork;

  tuning_generate_offsets();

  return 0;
}

