/*******************************************************************************
** arpeggio.c (arpeggio)
*******************************************************************************/

#include <stdio.h>    /* testing */
#include <stdlib.h>
#include <math.h>

#include "arpeggio.h"
#include "bank.h"
#include "clock.h"
#include "instrument.h"
#include "midicont.h"
#include "patch.h"
#include "tempo.h"
#include "tuning.h"

#define ARPEGGIO_SET_PHASE_INCREMENT()                                         \
  a->increment = S_arpeggio_phase_increment_table[a->tempo - TEMPO_LOWER_BOUND][a->speed - PATCH_ARPEGGIO_SPEED_LOWER_BOUND];

/* arpeggio speed table (in notes per beat) */
static float  S_arpeggio_speed_table[PATCH_ARPEGGIO_SPEED_NUM_VALUES] = 
              {  1.0f,  /*   4 notes in 4 beats (1 note  per beat)      */
                 1.5f,  /*   6 notes in 4 beats                         */
                 2.0f,  /*   8 notes in 4 beats (1 note  per eighth)    */
                 2.5f,  /*  10 notes in 4 beats                         */
                 3.0f,  /*  12 notes in 4 beats (3 notes per beat)      */
                 3.5f,  /*  14 notes in 4 beats                         */
                 4.0f,  /*  16 notes in 4 beats (1 note  per sixteenth) */
                 5.0f,  /*  20 notes in 4 beats                         */
                 6.0f,  /*  24 notes in 4 beats (3 notes per eighth)    */
                 7.0f,  /*  28 notes in 4 beats                         */
                 8.0f,  /*  32 notes in 4 beats (2 notes per sixteenth) */
                10.0f,  /*  40 notes in 4 beats                         */
                12.0f,  /*  48 notes in 4 beats (3 notes per sixteenth) */
                14.0f,  /*  56 notes in 4 beats                         */
                16.0f,  /*  64 notes in 4 beats (4 notes per sixteenth) */
                24.0f   /*  96 notes in 4 beats (6 notes per sixteenth) */
              };

/* phase increment table */
static unsigned int S_arpeggio_phase_increment_table[TEMPO_NUM_VALUES][PATCH_ARPEGGIO_SPEED_NUM_VALUES];

/* arpeggio bank */
arpeggio G_arpeggio_bank[BANK_NUM_ARPEGGIOS];

/*******************************************************************************
** arpeggio_reset_all()
*******************************************************************************/
short int arpeggio_reset_all()
{
  int k;
  int m;

  arpeggio* a;

  /* reset all arpeggios */
  for (k = 0; k < BANK_NUM_ARPEGGIOS; k++)
  {
    /* obtain arpeggio pointer */
    a = &G_arpeggio_bank[k];

    /* initialize arpeggio variables */
    a->mode = PATCH_ARPEGGIO_MODE_DEFAULT;
    a->pattern = PATCH_ARPEGGIO_PATTERN_DEFAULT;
    a->octaves = PATCH_ARPEGGIO_OCTAVES_DEFAULT;
    a->speed = PATCH_ARPEGGIO_SPEED_DEFAULT;

    a->phase = 0;
    a->tempo = TEMPO_DEFAULT;

    ARPEGGIO_SET_PHASE_INCREMENT()

    a->step_index = 0;

    for (m = 0; m < ARPEGGIO_NUM_NOTES; m++)
      a->sorted_notes[m] = TUNING_NOTE_BLANK;

    a->num_notes = 0;

    a->last_note = TUNING_NOTE_BLANK;

    a->on_switch = MIDI_CONT_ARPEGGIO_SWITCH_DEFAULT;
  }

  return 0;
}

/*******************************************************************************
** arpeggio_load_patch()
*******************************************************************************/
short int arpeggio_load_patch(int instrument_index, int patch_index)
{
  arpeggio* a;
  patch* p;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain patch pointer */
  p = &G_patch_bank[patch_index];

  /* obtain arpeggio pointer */
  a = &G_arpeggio_bank[instrument_index];

  /* mode */
  if ((p->arpeggio_mode >= PATCH_ARPEGGIO_MODE_LOWER_BOUND) && 
      (p->arpeggio_mode <= PATCH_ARPEGGIO_MODE_UPPER_BOUND))
  {
    a->mode = p->arpeggio_mode;
  }
  else
    a->mode = PATCH_ARPEGGIO_MODE_DEFAULT;

  /* pattern */
  if ((p->arpeggio_pattern >= PATCH_ARPEGGIO_PATTERN_LOWER_BOUND) && 
      (p->arpeggio_pattern <= PATCH_ARPEGGIO_PATTERN_UPPER_BOUND))
  {
    a->pattern = p->arpeggio_pattern;
  }
  else
    a->pattern = PATCH_ARPEGGIO_PATTERN_DEFAULT;

  /* octaves */
  if ((p->arpeggio_octaves >= PATCH_ARPEGGIO_OCTAVES_LOWER_BOUND) && 
      (p->arpeggio_octaves <= PATCH_ARPEGGIO_OCTAVES_UPPER_BOUND))
  {
    a->octaves = p->arpeggio_octaves;
  }
  else
    a->octaves = PATCH_ARPEGGIO_OCTAVES_DEFAULT;

  /* speed */
  if ((p->arpeggio_speed >= PATCH_ARPEGGIO_SPEED_LOWER_BOUND) && 
      (p->arpeggio_speed <= PATCH_ARPEGGIO_SPEED_UPPER_BOUND))
  {
    a->speed = p->arpeggio_speed;
  }
  else
    a->speed = PATCH_ARPEGGIO_SPEED_DEFAULT;

  /* set phase increment */
  ARPEGGIO_SET_PHASE_INCREMENT()

  return 0;
}

/*******************************************************************************
** arpeggio_set_mode()
*******************************************************************************/
short int arpeggio_set_mode(int instrument_index, short int mode)
{
  arpeggio* a;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the mode is valid */
  if ((mode < PATCH_ARPEGGIO_MODE_LOWER_BOUND) || 
      (mode > PATCH_ARPEGGIO_MODE_UPPER_BOUND))
  {
    return 0;
  }

  /* obtain arpeggio pointer */
  a = &G_arpeggio_bank[instrument_index];

  /* set the mode */
  a->mode = mode;

  return 0;
}

/*******************************************************************************
** arpeggio_set_pattern()
*******************************************************************************/
short int arpeggio_set_pattern(int instrument_index, short int pattern)
{
  arpeggio* a;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the pattern is valid */
  if ((pattern < PATCH_ARPEGGIO_PATTERN_LOWER_BOUND) || 
      (pattern > PATCH_ARPEGGIO_PATTERN_UPPER_BOUND))
  {
    return 0;
  }

  /* obtain arpeggio pointer */
  a = &G_arpeggio_bank[instrument_index];

  /* set the pattern */
  a->pattern = pattern;

  return 0;
}

/*******************************************************************************
** arpeggio_set_octaves()
*******************************************************************************/
short int arpeggio_set_octaves(int instrument_index, short int octaves)
{
  arpeggio* a;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the octaves is valid */
  if ((octaves < PATCH_ARPEGGIO_OCTAVES_LOWER_BOUND) || 
      (octaves > PATCH_ARPEGGIO_OCTAVES_UPPER_BOUND))
  {
    return 0;
  }

  /* obtain arpeggio pointer */
  a = &G_arpeggio_bank[instrument_index];

  /* set the octaves */
  a->octaves = octaves;

  /* generate notes */
  arpeggio_generate_notes(instrument_index);

  return 0;
}

/*******************************************************************************
** arpeggio_set_speed()
*******************************************************************************/
short int arpeggio_set_speed(int instrument_index, short int speed)
{
  arpeggio* a;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the speed is valid */
  if ((speed < PATCH_ARPEGGIO_SPEED_LOWER_BOUND) || 
      (speed > PATCH_ARPEGGIO_SPEED_UPPER_BOUND))
  {
    return 0;
  }

  /* obtain arpeggio pointer */
  a = &G_arpeggio_bank[instrument_index];

  /* set the speed */
  a->speed = speed;

  /* set phase increment */
  ARPEGGIO_SET_PHASE_INCREMENT()

  return 0;
}

/*******************************************************************************
** arpeggio_set_switch()
*******************************************************************************/
short int arpeggio_set_switch(int instrument_index, short int state)
{
  arpeggio* a;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the switch state is valid */
  if ((state < MIDI_CONT_ARPEGGIO_SWITCH_LOWER_BOUND) || 
      (state > MIDI_CONT_ARPEGGIO_SWITCH_UPPER_BOUND))
  {
    return 0;
  }

  /* obtain arpeggio pointer */
  a = &G_arpeggio_bank[instrument_index];

  /* if the switch is already set to this state, return */
  if (a->on_switch == state)
    return 0;

  /* set the switch */
  a->on_switch = state;

  /* reset the phase if necessary */
  if (a->on_switch == MIDI_CONT_SWITCH_STATE_ON)
    a->phase = 0;

  return 0;
}

/*******************************************************************************
** arpeggio_set_tempo()
*******************************************************************************/
short int arpeggio_set_tempo(int instrument_index, short int tempo)
{
  arpeggio* a;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* obtain arpeggio pointer */
  a = &G_arpeggio_bank[instrument_index];

  /* set tempo */
  if (tempo < TEMPO_LOWER_BOUND)
    a->tempo = TEMPO_LOWER_BOUND;
  else if (tempo > TEMPO_UPPER_BOUND)
    a->tempo = TEMPO_UPPER_BOUND;
  else
    a->tempo = tempo;

  /* adjust phase increment based on tempo */
  ARPEGGIO_SET_PHASE_INCREMENT()

  return 0;
}

/*******************************************************************************
** arpeggio_generate_notes()
*******************************************************************************/
short int arpeggio_generate_notes(int instrument_index)
{
  int m;
  int n;

  arpeggio* a;
  instrument* ins;

  int index;
  int val;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* obtain arpeggio pointer */
  a = &G_arpeggio_bank[instrument_index];

  /* obtain instrument pointer */
  ins = &G_instrument_bank[instrument_index];

  /* reset sorted notes */
  for (m = 0; m < ARPEGGIO_NUM_NOTES; m++)
    a->sorted_notes[m] = TUNING_NOTE_BLANK;

  /* insert the pressed keys into the notes array */
  for (m = 0; m < ins->num_pressed; m++)
  {
    for (n = 0; n < a->octaves; n++)
    {
      a->sorted_notes[n * ins->num_pressed + m] = (12 * n) + ins->pressed_keys[m];
    }
  }

  /* determine the current number of notes */
  a->num_notes = ins->num_pressed * a->octaves;

  /* sort the notes! */
  for (m = 0; m < a->num_notes; m++)
  {
    for (n = m + 1; n < a->num_notes; n++)
    {
      
    }
  }

  /* remove duplicates */

  return 0;
}

/*******************************************************************************
** arpeggio_update_all()
*******************************************************************************/
short int arpeggio_update_all()
{
  int k;

  arpeggio* a;

  int new_note;

  /* update all arpeggios */
  for (k = 0; k < BANK_NUM_ARPEGGIOS; k++)
  {
    /* obtain arpeggio pointer */
    a = &G_arpeggio_bank[k];

    /* if this arpeggio is off, skip it! */
    if (a->on_switch == MIDI_CONT_SWITCH_STATE_OFF)
      continue;

    /* if no keys are pressed, skip it! */
    if (a->num_notes == 0)
      continue;

    /* update phase */
    a->phase += a->increment;

    /* wraparound phase register (28 bits) */
    if (a->phase > 0xFFFFFFF)
    {
      a->phase &= 0xFFFFFFF;

      /* update pattern index */
      a->step_index = (a->step_index + 1) % a->num_notes;

      /* determine the new note */
      new_note = a->sorted_notes[a->step_index];

      /* trigger next note, if necessary */
      if (new_note != TUNING_NOTE_BLANK)
      {
        instrument_note_on(k, new_note);

        if (a->mode == PATCH_ARPEGGIO_MODE_HARP)
          instrument_note_off(k, a->last_note);
      }

      /* store last played note */
      a->last_note = new_note;
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

