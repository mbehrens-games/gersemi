/*******************************************************************************
** arpeggio.c (arpeggio)
*******************************************************************************/

#include <stdio.h>    /* testing */
#include <stdlib.h>
#include <math.h>

#include "arpeggio.h"
#include "bank.h"
#include "clock.h"
#include "midicont.h"
#include "patch.h"
#include "tempo.h"

/* arpeggio speed table (in notes per beat) */
static float  S_arpeggio_speed_table[PATCH_ARPEGGIO_SPEED_NUM_VALUES] = 
              { 0.75f, 
                 1.0f,  /* 1 note  per beat       */
                 1.5f, 
                 2.0f,  /* 1 note  per eighth     */
                 2.5f, 
                 3.0f,  /* 3 notes per beat       */
                 3.5f, 
                 4.0f,  /* 1 note  per sixteenth  */
                 5.0f, 
                 6.0f,  /* 3 notes per eighth     */
                 7.0f, 
                 8.0f,  /* 2 notes per sixteenth  */
                10.0f, 
                12.0f,  /* 3 notes per sixteenth  */
                14.0f, 
                16.0f   /* 4 notes per sixteenth  */
              };

/* phase increment table */
static unsigned int S_arpeggio_phase_increment_table[TEMPO_NUM_VALUES][PATCH_ARPEGGIO_SPEED_NUM_VALUES];

/* arpeggio bank */
arpeggio G_arpeggio_bank[BANK_NUM_BENDERS];

/*******************************************************************************
** arpeggio_setup_all()
*******************************************************************************/
short int arpeggio_setup_all()
{
  int k;

  /* setup all arpeggios */
  for (k = 0; k < BANK_NUM_VOICES; k++)
    arpeggio_reset(k);

  return 0;
}

/*******************************************************************************
** arpeggio_reset()
*******************************************************************************/
short int arpeggio_reset(int voice_index)
{
  int m;

  arpeggio* a;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  for (m = 0; m < BANK_ARPEGGIOS_PER_VOICE; m++)
  {
    /* obtain arpeggio pointer */
    a = &G_arpeggio_bank[BANK_ARPEGGIOS_PER_VOICE * voice_index + m];

    /* initialize arpeggio variables */
    a->mode = PATCH_ARPEGGIO_MODE_DEFAULT;
    a->pattern = PATCH_ARPEGGIO_PATTERN_DEFAULT;
    a->speed = PATCH_ARPEGGIO_SPEED_DEFAULT;
    a->sync = PATCH_SYNC_DEFAULT;

    a->phase = 0;
    a->increment = 
      S_arpeggio_phase_increment_table[TEMPO_DEFAULT - TEMPO_LOWER_BOUND][PATCH_ARPEGGIO_SPEED_DEFAULT - PATCH_ARPEGGIO_SPEED_LOWER_BOUND];

    a->tempo = TEMPO_DEFAULT;
  }

  return 0;
}

/*******************************************************************************
** arpeggio_load_patch()
*******************************************************************************/
short int arpeggio_load_patch(int voice_index, int patch_index)
{
  arpeggio* a;
  patch* p;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain patch pointer */
  p = &G_patch_bank[patch_index];

  /* obtain arpeggio pointer */
  a = &G_arpeggio_bank[voice_index];

  /* mode */
  if ((p->arpeggio_mode >= PATCH_ARPEGGIO_MODE_LOWER_BOUND) && 
      (p->arpeggio_mode <= PATCH_ARPEGGIO_MODE_UPPER_BOUND))
  {
    a->mode = p->arpeggio_mode;
  }
  else
    a->mode = PATCH_ARPEGGIO_MODE_LOWER_BOUND;

  /* pattern */
  if ((p->arpeggio_pattern >= PATCH_ARPEGGIO_PATTERN_LOWER_BOUND) && 
      (p->arpeggio_pattern <= PATCH_ARPEGGIO_PATTERN_UPPER_BOUND))
  {
    a->pattern = p->arpeggio_pattern;
  }
  else
    a->pattern = PATCH_ARPEGGIO_PATTERN_LOWER_BOUND;

  /* speed */
  if ((p->arpeggio_speed >= PATCH_ARPEGGIO_SPEED_LOWER_BOUND) && 
      (p->arpeggio_speed <= PATCH_ARPEGGIO_SPEED_UPPER_BOUND))
  {
    a->speed = p->arpeggio_speed;
  }
  else
    a->speed = PATCH_ARPEGGIO_SPEED_LOWER_BOUND;

  /* sync */
  if ((p->sync_arp >= PATCH_SYNC_LOWER_BOUND) && 
      (p->sync_arp <= PATCH_SYNC_UPPER_BOUND))
  {
    a->speed = p->sync_arp;
  }
  else
    a->speed = PATCH_SYNC_LOWER_BOUND;

  return 0;
}

/*******************************************************************************
** arpeggio_set_tempo()
*******************************************************************************/
short int arpeggio_set_tempo(int voice_index, short int tempo)
{
  arpeggio* a;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain arpeggio pointer */
  a = &G_arpeggio_bank[voice_index];

  /* set tempo */
  if (tempo < TEMPO_LOWER_BOUND)
    a->tempo = TEMPO_LOWER_BOUND;
  else if (tempo > TEMPO_UPPER_BOUND)
    a->tempo = TEMPO_UPPER_BOUND;
  else
    a->tempo = tempo;

  /* adjust phase increment based on tempo */
  a->increment = S_arpeggio_phase_increment_table[a->tempo - TEMPO_LOWER_BOUND][a->speed - PATCH_ARPEGGIO_SPEED_LOWER_BOUND];

  return 0;
}

/*******************************************************************************
** arpeggio_sync_phase()
*******************************************************************************/
short int arpeggio_sync_phase(int voice_index)
{
  arpeggio* a;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain arpeggio pointer */
  a = &G_arpeggio_bank[voice_index];

  /* reset phase if necessary */
  if (a->sync == PATCH_SYNC_ON)
    a->phase = 0;

  return 0;
}

/*******************************************************************************
** arpeggio_trigger()
*******************************************************************************/
short int arpeggio_trigger(int voice_index)
{
  arpeggio* a;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain arpeggio pointer */
  a = &G_arpeggio_bank[voice_index];


  return 0;
}

/*******************************************************************************
** arpeggio_update_all()
*******************************************************************************/
short int arpeggio_update_all()
{
  int k;

  arpeggio* a;

  /* update all arpeggios */
  for (k = 0; k < BANK_NUM_VOICES; k++)
  {
    /* obtain arpeggio pointer */
    a = &G_arpeggio_bank[k];

    /* update phase */
    a->phase += a->increment;

    /* wraparound phase register (28 bits) */
    if (a->phase > 0xFFFFFFF)
    {
      a->phase &= 0xFFFFFFF;

      /* trigger next note... */
    }
  }

  return 0;
}

/*******************************************************************************
** arpeggio_generate_tables()
*******************************************************************************/
short int arpeggio_generate_tables()
{
  int k;
  int m;

  /* phase increment table */
  for (k = 0; k < TEMPO_NUM_VALUES; k++)
  {
    for (m = 0; m < PATCH_ARPEGGIO_SPEED_NUM_VALUES; m++)
    {
      S_arpeggio_phase_increment_table[k][m] = 
        (int) ((TEMPO_COMPUTE_BEATS_PER_SECOND(k + TEMPO_LOWER_BOUND) * S_arpeggio_speed_table[m] * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
    }
  }

#if 0
  /* print out phase increments for various tempos */
  printf("Arpeggio Speeds for Tempo 120 BPM:\n");

  for (m = 0; m < PATCH_ARPEGGIO_SPEED_NUM_VALUES; m++)
  {
    printf( "  Speed %d: %d \n", 
            m, S_arpeggio_phase_increment_table[120 - TEMPO_LOWER_BOUND][m]);
  }

  printf("Arpeggio Speeds for Tempo 180 BPM:\n");

  for (m = 0; m < PATCH_ARPEGGIO_SPEED_NUM_VALUES; m++)
  {
    printf( "  Speed %d: %d \n", 
            m, S_arpeggio_phase_increment_table[180 - TEMPO_LOWER_BOUND][m]);
  }
#endif

  return 0;
}

