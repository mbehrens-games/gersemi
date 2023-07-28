/*******************************************************************************
** tuning.c (tuning systems)
*******************************************************************************/

#include <math.h>

#include "clock.h"
#include "tuning.h"

#define PI      3.14159265358979323846f
#define TWO_PI  6.28318530717958647693f

/* phase increment table */
unsigned int G_tuning_phase_increment_table[TUNING_TABLE_SIZE];

/* tuning system & fork */
static int S_tuning_system;
static int S_tuning_fork;

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

/* frequencies in the middle octave (populated during generation) */
static float S_tuning_freq_table[TUNING_TABLE_SIZE];

/*******************************************************************************
** tuning_reset()
*******************************************************************************/
short int tuning_reset()
{
  S_tuning_system = TUNING_SYSTEM_12_EQUAL_TEMPERAMENT;
  S_tuning_fork = TUNING_FORK_A440;

  tuning_calculate_tables();

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

  tuning_calculate_tables();

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

  tuning_calculate_tables();

  return 0;
}

/*******************************************************************************
** tuning_calculate_tables()
*******************************************************************************/
short int tuning_calculate_tables()
{
  int     m;
  int     n;

  float*  mult_table;

  float   cents;

  /* determine multiplier table */
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

  /* compute frequency at the tuning fork */
  if (S_tuning_fork == TUNING_FORK_C256)
    S_tuning_freq_table[0 * TUNING_NUM_SEMITONE_STEPS] = 256;
  else if (S_tuning_fork == TUNING_FORK_A432)
    S_tuning_freq_table[9 * TUNING_NUM_SEMITONE_STEPS] = 432;
  else if (S_tuning_fork == TUNING_FORK_A434)
    S_tuning_freq_table[9 * TUNING_NUM_SEMITONE_STEPS] = 434;
  else if (S_tuning_fork == TUNING_FORK_A436)
    S_tuning_freq_table[9 * TUNING_NUM_SEMITONE_STEPS] = 436;
  else if (S_tuning_fork == TUNING_FORK_A438)
    S_tuning_freq_table[9 * TUNING_NUM_SEMITONE_STEPS] = 438;
  else if (S_tuning_fork == TUNING_FORK_A440)
    S_tuning_freq_table[9 * TUNING_NUM_SEMITONE_STEPS] = 440;
  else if (S_tuning_fork == TUNING_FORK_A442)
    S_tuning_freq_table[9 * TUNING_NUM_SEMITONE_STEPS] = 442;
  else if (S_tuning_fork == TUNING_FORK_A444)
    S_tuning_freq_table[9 * TUNING_NUM_SEMITONE_STEPS] = 444;
  else
    S_tuning_freq_table[9 * TUNING_NUM_SEMITONE_STEPS] = 440;

  /* compute frequencies for the other notes in the middle octave */
  if (S_tuning_fork == TUNING_FORK_C256)
  {
    for (m = 1; m < 12; m++)
    {
      S_tuning_freq_table[m * TUNING_NUM_SEMITONE_STEPS] = 
        S_tuning_freq_table[0 * TUNING_NUM_SEMITONE_STEPS] * mult_table[m];
    }
  }
  else
  {
    for (m = 0; m < 12; m++)
    {
      if (m == 9)
        continue;

      S_tuning_freq_table[m * TUNING_NUM_SEMITONE_STEPS] = 
        S_tuning_freq_table[9 * TUNING_NUM_SEMITONE_STEPS] * (mult_table[m] / mult_table[9]);
    }
  }

  /* compute frequencies between the notes in the middle octave */
  for (m = 0; m < 12; m++)
  {
    if (m < 11)
      cents = (1200.0f * (log(mult_table[m + 1]) - log(mult_table[m])) / log(2)) / TUNING_NUM_SEMITONE_STEPS;
    else
      cents = (1200.0f * (log(2) - log(mult_table[m])) / log(2)) / TUNING_NUM_SEMITONE_STEPS;

    for (n = 1; n < TUNING_NUM_SEMITONE_STEPS; n++)
    {
      S_tuning_freq_table[m * TUNING_NUM_SEMITONE_STEPS + n] = 
        S_tuning_freq_table[m * TUNING_NUM_SEMITONE_STEPS] * exp(log(2) * ((cents * n) / 1200.0f));
    }
  }

  /* compute phase increments in the highest octave (octave 9)            */
  /* (the increments in other octaves can be obtained by right shifting)  */
  for (n = 0; n < TUNING_TABLE_SIZE; n++)
  {
    G_tuning_phase_increment_table[n] = 
      (int) (((S_tuning_freq_table[n] * 32) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
  }

#if 0
  printf("Frequency Table (Octave 4):\n");

  for (m = 0; m < TUNING_TABLE_SIZE / 4; m++)
  {
    printf("%f %f %f %f\n", S_tuning_freq_table[4 * m + 0], 
                            S_tuning_freq_table[4 * m + 1], 
                            S_tuning_freq_table[4 * m + 2], 
                            S_tuning_freq_table[4 * m + 3]);
  }
#endif

#if 0
  printf("Phase Increment Table (Octave 9):\n");

  for (m = 0; m < TUNING_TABLE_SIZE / 4; m++)
  {
    printf("%d %d %d %d\n", G_tuning_phase_increment_table[4 * m + 0], 
                            G_tuning_phase_increment_table[4 * m + 1], 
                            G_tuning_phase_increment_table[4 * m + 2], 
                            G_tuning_phase_increment_table[4 * m + 3]);
  }
#endif

  return 0;
}

