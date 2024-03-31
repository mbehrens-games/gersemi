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
              {  1.0f,  /*   1 note  per beat                     */
                 2.0f,  /*   2 notes per beat (1 note per 8th)    */
                 3.0f,  /*   3 notes per beat                     */
                 4.0f,  /*   4 notes per beat (2 notes per 8th)   */
                 5.0f,  /*   5 notes per beat                     */
                 6.0f,  /*   6 notes per beat (3 notes per 8th)   */
                 8.0f,  /*   4 notes per  8th (2 notes per 16th)  */
                10.0f,  /*   5 notes per  8th                     */
                12.0f,  /*   6 notes per  8th (3 notes per 16th)  */
                16.0f,  /*   4 notes per 16th                     */
                20.0f,  /*   5 notes per 16th                     */
                24.0f   /*   6 notes per 16th                     */
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

    a->tempo = TEMPO_DEFAULT;
    a->phase = 0;

    ARPEGGIO_SET_PHASE_INCREMENT()

    for (m = 0; m < ARPEGGIO_NUM_NOTES; m++)
      a->sorted_notes[m] = TUNING_NOTE_BLANK;

    a->num_notes = 0;

    for (m = 0; m < ARPEGGIO_NUM_STEPS; m++)
      a->pattern_steps[m] = TUNING_NOTE_BLANK;

    a->last_note = TUNING_NOTE_BLANK;

    a->step_index = 0;

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
    arpeggio_set_mode(instrument_index, p->arpeggio_mode);
  }
  else
    arpeggio_set_mode(instrument_index, PATCH_ARPEGGIO_MODE_DEFAULT);

  /* pattern */
  if ((p->arpeggio_pattern >= PATCH_ARPEGGIO_PATTERN_LOWER_BOUND) && 
      (p->arpeggio_pattern <= PATCH_ARPEGGIO_PATTERN_UPPER_BOUND))
  {
    arpeggio_set_pattern(instrument_index, p->arpeggio_pattern);
  }
  else
    arpeggio_set_pattern(instrument_index, PATCH_ARPEGGIO_PATTERN_DEFAULT);

  /* octaves */
  if ((p->arpeggio_octaves >= PATCH_ARPEGGIO_OCTAVES_LOWER_BOUND) && 
      (p->arpeggio_octaves <= PATCH_ARPEGGIO_OCTAVES_UPPER_BOUND))
  {
    arpeggio_set_octaves(instrument_index, p->arpeggio_octaves);
  }
  else
    arpeggio_set_octaves(instrument_index, PATCH_ARPEGGIO_OCTAVES_DEFAULT);

  /* speed */
  if ((p->arpeggio_speed >= PATCH_ARPEGGIO_SPEED_LOWER_BOUND) && 
      (p->arpeggio_speed <= PATCH_ARPEGGIO_SPEED_UPPER_BOUND))
  {
    arpeggio_set_speed(instrument_index, p->arpeggio_speed);
  }
  else
    arpeggio_set_speed(instrument_index, PATCH_ARPEGGIO_SPEED_DEFAULT);

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

  /* generate pattern steps */
  if (a->on_switch == MIDI_CONT_SWITCH_STATE_ON)
    arpeggio_generate_pattern_steps(instrument_index);

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

  /* generate pattern steps */
  if (a->on_switch == MIDI_CONT_SWITCH_STATE_ON)
    arpeggio_generate_pattern_steps(instrument_index);

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
  int m;

  arpeggio* a;

  char new_note;

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
  {
    a->phase = 0;
    a->step_index = 0;

    arpeggio_generate_sorted_notes(instrument_index);
    arpeggio_generate_pattern_steps(instrument_index);

    new_note = a->pattern_steps[a->step_index];

    instrument_note_on(instrument_index, new_note);

    a->last_note = new_note;
  }
  /* send note-offs to all keys if the arpeggio is off */
  else if (a->on_switch == MIDI_CONT_SWITCH_STATE_OFF)
  {
    for (m = 0; m < a->num_notes; m++)
      instrument_note_off(instrument_index, a->sorted_notes[m]);
  }

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
** arpeggio_sync_to_tempo()
*******************************************************************************/
short int arpeggio_sync_to_tempo(int instrument_index)
{
  arpeggio* a;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* obtain arpeggio pointer */
  a = &G_arpeggio_bank[instrument_index];

  /* reset phase */
  a->phase = 0;

  return 0;
}

/*******************************************************************************
** arpeggio_generate_sorted_notes()
*******************************************************************************/
short int arpeggio_generate_sorted_notes(int instrument_index)
{
  int m;
  int n;

  arpeggio* a;
  instrument* ins;

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
  for (m = 1; m < a->num_notes; m++)
  {
    for (n = m; n > 0; n--)
    {
      if (a->sorted_notes[n - 1] > a->sorted_notes[n])
      {
        val = a->sorted_notes[n - 1];
        a->sorted_notes[n - 1] = a->sorted_notes[n];
        a->sorted_notes[n] = val;
      }
      else
        break;
    }
  }

  /* remove duplicates */
  m = 0;

  while (m < a->num_notes - 1)
  {
    if (a->sorted_notes[m] == TUNING_NOTE_BLANK)
      break;

    /* if a duplicate is found, shift the notes above this down one.  */
    /* note that we continue to avoid iterating the index m, as the   */
    /* next note should now be shifted down to the index m.           */
    if (a->sorted_notes[m] == a->sorted_notes[m + 1])
    {
      for (n = m + 1; n < a->num_notes - 1; n++)
        a->sorted_notes[n] = a->sorted_notes[n + 1];

      a->sorted_notes[a->num_notes - 1] = TUNING_NOTE_BLANK;
      a->num_notes -= 1;

      continue;
    }

    /* otherwise, go to the next note! */
    m += 1;
  }

  return 0;
}

/*******************************************************************************
** arpeggio_generate_pattern_steps()
*******************************************************************************/
short int arpeggio_generate_pattern_steps(int instrument_index)
{
  int m;

  arpeggio* a;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* obtain arpeggio pointer */
  a = &G_arpeggio_bank[instrument_index];

  /* reset pattern steps */
  for (m = 0; m < ARPEGGIO_NUM_STEPS; m++)
    a->pattern_steps[m] = TUNING_NOTE_BLANK;

  /* set the pattern steps based on the pattern! */
  if (a->pattern == PATCH_ARPEGGIO_PATTERN_UP)
  {
    for (m = 0; m < a->num_notes; m++)
      a->pattern_steps[m] = a->sorted_notes[m];

    a->num_steps = a->num_notes;
  }
  else if (a->pattern == PATCH_ARPEGGIO_PATTERN_DOWN)
  {
    for (m = 0; m < a->num_notes; m++)
      a->pattern_steps[m] = a->sorted_notes[a->num_notes - 1 - m];

    a->num_steps = a->num_notes;
  }
  else if (a->pattern == PATCH_ARPEGGIO_PATTERN_UP_AND_DOWN)
  {
    for (m = 0; m < a->num_notes - 1; m++)
      a->pattern_steps[m] = a->sorted_notes[m];

    for (m = 0; m < a->num_notes - 1; m++)
      a->pattern_steps[m + (a->num_notes - 1)] = a->sorted_notes[a->num_notes - 1 - m];

    a->num_steps = 2 * (a->num_notes - 1);
  }
  else if (a->pattern == PATCH_ARPEGGIO_PATTERN_UP_AND_DOWN_ALT)
  {
    for (m = 0; m < a->num_notes; m++)
      a->pattern_steps[m] = a->sorted_notes[m];

    for (m = 0; m < a->num_notes; m++)
      a->pattern_steps[m + a->num_notes] = a->sorted_notes[a->num_notes - 1 - m];

    a->num_steps = 2 * a->num_notes;
  }

  return 0;
}

/*******************************************************************************
** arpeggio_key_on_or_key_off()
*******************************************************************************/
short int arpeggio_key_on_or_key_off(int instrument_index)
{
  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  arpeggio_generate_sorted_notes(instrument_index);
  arpeggio_generate_pattern_steps(instrument_index);

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

      /* update step index and determine the new note */
      a->step_index += 1;

      if (a->step_index >= a->num_steps)
        a->step_index = 0;

      new_note = a->pattern_steps[a->step_index];

      /* trigger next note, if necessary */
      if (new_note != TUNING_NOTE_BLANK)
      {
        instrument_note_on(k, new_note);

        /* the note off for the previous note follows the note on   */
        /* so that the portamento slide will trigger, if necessary  */
        if ((a->mode == PATCH_ARPEGGIO_MODE_HARP) && (a->last_note != new_note))
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

