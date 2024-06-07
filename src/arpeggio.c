/*******************************************************************************
** arpeggio.c (arpeggiator)
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

#define ARPEGGIO_RESTART()                                                     \
  /* send a note-off to all current notes */                                   \
  for (m = 0; m < a->num_notes; m++)                                           \
    instrument_note_off(instrument_index, a->pattern_steps[m]);                \
                                                                               \
  /* compute pattern steps */                                                  \
  arpeggio_compute_pattern_steps(instrument_index);                            \
                                                                               \
  /* restart the arpeggio */                                                   \
  if (a->on_switch == MIDI_CONT_TOGGLE_SWITCH_ON)                              \
  {                                                                            \
    a->phase = 0;                                                              \
    a->step_index = 0;                                                         \
                                                                               \
    instrument_note_on( instrument_index,                                      \
                        a->pattern_steps[a->step_index]);                      \
  }

#define ARPEGGIO_ADJUST_STEP_INDEX()                                           \
  if (a->pattern == PATCH_ARPEGGIO_PATTERN_UP)                                 \
  {                                                                            \
    a->step_index = 0;                                                         \
                                                                               \
    for (m = 0; m < a->num_steps; m++)                                         \
    {                                                                          \
      if (a->pattern_steps[m] <= last_note)                                    \
        a->step_index = m;                                                     \
      else                                                                     \
        break;                                                                 \
    }                                                                          \
  }                                                                            \
  else if (a->pattern == PATCH_ARPEGGIO_PATTERN_DOWN)                          \
  {                                                                            \
    a->step_index = 0;                                                         \
                                                                               \
    for (m = 0; m < a->num_steps; m++)                                         \
    {                                                                          \
      if (a->pattern_steps[m] >= last_note)                                    \
        a->step_index = m;                                                     \
      else                                                                     \
        break;                                                                 \
    }                                                                          \
  }                                                                            \
  else if ( (a->pattern == PATCH_ARPEGGIO_PATTERN_UP_AND_DOWN) ||              \
            (a->pattern == PATCH_ARPEGGIO_PATTERN_UP_AND_DOWN_ALT))            \
  {                                                                            \
    if (last_half == 0)                                                        \
    {                                                                          \
      a->step_index = 0;                                                       \
                                                                               \
      for (m = 0; m < a->num_steps / 2; m++)                                   \
      {                                                                        \
        if (a->pattern_steps[m] <= last_note)                                  \
          a->step_index = m;                                                   \
        else                                                                   \
          break;                                                               \
      }                                                                        \
    }                                                                          \
    else                                                                       \
    {                                                                          \
      a->step_index = a->num_steps / 2;                                        \
                                                                               \
      for (m = a->num_steps / 2; m < a->num_steps; m++)                        \
      {                                                                        \
        if (a->pattern_steps[m] >= last_note)                                  \
          a->step_index = m;                                                   \
        else                                                                   \
          break;                                                               \
      }                                                                        \
    }                                                                          \
  }

/* speed table (in notes per beat) */
static int  S_arpeggio_speed_table[PATCH_ARPEGGIO_SPEED_NUM_VALUES] = 
            {  1,  2,  3,  4,  5,  6, 
               8, 10, 12, 16, 20, 24 
            };

/* phase increment table */
static unsigned int S_arpeggio_phase_increment_table[TEMPO_NUM_VALUES];

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

    a->increment = 
      S_arpeggio_phase_increment_table[TEMPO_DEFAULT - TEMPO_LOWER_BOUND];
    a->increment *= 
      S_arpeggio_speed_table[PATCH_ARPEGGIO_SPEED_DEFAULT - PATCH_ARPEGGIO_SPEED_LOWER_BOUND];

    for (m = 0; m < ARPEGGIO_NUM_STEPS; m++)
      a->pattern_steps[m] = TUNING_NOTE_BLANK;

    a->num_steps = 0;
    a->num_notes = 0;

    a->step_index = 0;

    a->on_switch = MIDI_CONT_TOGGLE_SWITCH_OFF;
  }

  return 0;
}

/*******************************************************************************
** arpeggio_compute_pattern_steps()
*******************************************************************************/
short int arpeggio_compute_pattern_steps(int instrument_index)
{
  int m;
  int n;

  arpeggio* a;
  instrument* ins;

  int val;
  int index;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* obtain arpeggio pointer */
  a = &G_arpeggio_bank[instrument_index];

  /* obtain instrument pointer */
  ins = &G_instrument_bank[instrument_index];

  /* reset pattern steps */
  for (m = 0; m < ARPEGGIO_NUM_STEPS; m++)
    a->pattern_steps[m] = TUNING_NOTE_BLANK;

  a->num_steps = 0;
  a->num_notes = 0;

  /* if there are no pressed keys, reset the step index and return */
  if (ins->num_pressed == 0)
  {
    a->step_index = 0;

    return 0;
  }

  /* insert the pressed keys into the steps array */
  if (ins->num_pressed > ARPEGGIO_NUM_KEYS)
  {
    a->num_notes = ARPEGGIO_NUM_KEYS * a->octaves;

    for (n = 0; n < a->octaves; n++)
    {
      for (m = 0; m < ARPEGGIO_NUM_KEYS; m++)
        a->pattern_steps[(n * ARPEGGIO_NUM_KEYS) + m] = (12 * n) + ins->pressed_keys[m];
    }
  }
  else
  {
    a->num_notes = ins->num_pressed * a->octaves;

    for (n = 0; n < a->octaves; n++)
    {
      for (m = 0; m < ins->num_pressed; m++)
        a->pattern_steps[(n * ins->num_pressed) + m] = (12 * n) + ins->pressed_keys[m];
    }
  }

  /* sort the notes! */
  for (m = 0; m < a->num_notes; m++)
  {
    index = m;

    for (n = m + 1; n < a->num_notes; n++)
    {
      if (a->pattern_steps[n] < a->pattern_steps[index])
        index = n;
    }

    if (index != m)
    {
      val = a->pattern_steps[index];
      a->pattern_steps[index] = a->pattern_steps[m];
      a->pattern_steps[m] = val;
    }
  }

  /* remove duplicates */
  for (m = a->num_notes - 2; m >= 0; m--)
  {
    if (a->pattern_steps[m] == TUNING_NOTE_BLANK)
      continue;

    /* if a duplicate is found, shift the notes above this one down. */
    if (a->pattern_steps[m] == a->pattern_steps[m + 1])
    {
      for (n = m + 1; n < a->num_notes - 1; n++)
        a->pattern_steps[n] = a->pattern_steps[n + 1];

      a->pattern_steps[a->num_notes - 1] = TUNING_NOTE_BLANK;
      a->num_notes -= 1;
    }
  }

  /* clamp notes */
  for (m = 0; m < a->num_notes; m++)
  {
    if (a->pattern_steps[m] < TUNING_NOTE_A0)
      a->pattern_steps[m] = TUNING_NOTE_A0;
    else if (a->pattern_steps[m] > TUNING_NOTE_C8)
      a->pattern_steps[m] = TUNING_NOTE_C8;
  }

  /* set the pattern steps based on the pattern */
  if (a->pattern == PATCH_ARPEGGIO_PATTERN_UP)
  {
    a->num_steps = a->num_notes;
  }
  else if (a->pattern == PATCH_ARPEGGIO_PATTERN_DOWN)
  {
    a->num_steps = a->num_notes;

    for (m = 0; m < a->num_notes / 2; m++)
    {
      val = a->pattern_steps[a->num_notes - 1 - m];
      a->pattern_steps[a->num_notes - 1 - m] = a->pattern_steps[m];
      a->pattern_steps[m] = val;
    }
  }
  else if (a->pattern == PATCH_ARPEGGIO_PATTERN_UP_AND_DOWN)
  {
    if (a->num_notes >= 3)
    {
      a->num_steps = 2 * (a->num_notes - 1);

      for (m = 1; m < a->num_notes - 1; m++)
        a->pattern_steps[a->num_steps - m] = a->pattern_steps[m];
    }
    else
      a->num_steps = a->num_notes;
  }
  else if (a->pattern == PATCH_ARPEGGIO_PATTERN_UP_AND_DOWN_ALT)
  {
    a->num_steps = 2 * a->num_notes;

    for (m = 0; m < a->num_notes; m++)
      a->pattern_steps[a->num_steps - 1 - m] = a->pattern_steps[m];
  }
  else
    a->num_steps = a->num_notes;

  return 0;
}

/*******************************************************************************
** arpeggio_load_patch()
*******************************************************************************/
short int arpeggio_load_patch(int instrument_index, 
                              int cart_index, int patch_index)
{
  int m;

  arpeggio* a;

  cart* c;
  patch* p;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure that the cart & patch indices are valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(cart_index))
    return 1;

  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain cart & patch pointers */
  c = &G_cart_bank[cart_index];
  p = &(c->patches[patch_index]);

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
  a->increment = 
    S_arpeggio_phase_increment_table[a->tempo - TEMPO_LOWER_BOUND];
  a->increment *= 
    S_arpeggio_speed_table[a->speed - PATCH_ARPEGGIO_SPEED_LOWER_BOUND];

  /* restart arpeggio if necessary */
  if (a->on_switch == MIDI_CONT_TOGGLE_SWITCH_ON)
  {
    ARPEGGIO_RESTART()
  }

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

  int m;

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

  /* restart arpeggio if necessary */
  if (a->on_switch == MIDI_CONT_TOGGLE_SWITCH_ON)
  {
    ARPEGGIO_RESTART()
  }

  return 0;
}

/*******************************************************************************
** arpeggio_set_octaves()
*******************************************************************************/
short int arpeggio_set_octaves(int instrument_index, short int octaves)
{
  arpeggio* a;

  int m;

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

  /* restart arpeggio if necessary */
  if (a->on_switch == MIDI_CONT_TOGGLE_SWITCH_ON)
  {
    ARPEGGIO_RESTART()
  }

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
  a->increment = 
    S_arpeggio_phase_increment_table[a->tempo - TEMPO_LOWER_BOUND];
  a->increment *= 
    S_arpeggio_speed_table[a->speed - PATCH_ARPEGGIO_SPEED_LOWER_BOUND];

  return 0;
}

/*******************************************************************************
** arpeggio_set_switch()
*******************************************************************************/
short int arpeggio_set_switch(int instrument_index, short int state)
{
  int m;

  arpeggio* a;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the switch state is valid */
  if ((state != MIDI_CONT_TOGGLE_SWITCH_OFF) && 
      (state != MIDI_CONT_TOGGLE_SWITCH_ON))
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

  /* restart arpeggio if necessary */
  if (a->on_switch == MIDI_CONT_TOGGLE_SWITCH_ON)
  {
    ARPEGGIO_RESTART()
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
  a->increment = 
    S_arpeggio_phase_increment_table[a->tempo - TEMPO_LOWER_BOUND];
  a->increment *= 
    S_arpeggio_speed_table[a->speed - PATCH_ARPEGGIO_SPEED_LOWER_BOUND];

  return 0;
}

/*******************************************************************************
** arpeggio_tempo_sync()
*******************************************************************************/
short int arpeggio_tempo_sync(int instrument_index)
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
** arpeggio_key_pressed()
*******************************************************************************/
short int arpeggio_key_pressed(int instrument_index, int note)
{
  int m;

  arpeggio* a;

  int last_note;
  int last_half;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the note is valid */
  if (TUNING_NOTE_IS_NOT_PLAYABLE(note))
    return 0;

  /* obtain arpeggio pointer */
  a = &G_arpeggio_bank[instrument_index];

  /* check if the arpeggio is on */
  if (a->on_switch == MIDI_CONT_TOGGLE_SWITCH_ON)
  {
    /* determine last note */
    if (a->num_notes == 0)
      last_note = TUNING_NOTE_BLANK;
    else
      last_note = a->pattern_steps[a->step_index];

    /* determine last half */
    if ((a->pattern == PATCH_ARPEGGIO_PATTERN_UP_AND_DOWN) || 
        (a->pattern == PATCH_ARPEGGIO_PATTERN_UP_AND_DOWN_ALT))
    {
      if (a->step_index >= a->num_steps / 2)
        last_half = 1;
    }
    else
      last_half = 0;

    /* compute pattern steps */
    arpeggio_compute_pattern_steps(instrument_index);

    /* if this is the first key pressed, trigger the arpeggio */
    if ((a->num_notes == 1) && (last_note == TUNING_NOTE_BLANK))
    {
      a->phase = 0;
      a->step_index = 0;

      instrument_note_on(instrument_index, a->pattern_steps[a->step_index]);
    }
    /* adjust the step index, if necessary */
    else if (a->pattern_steps[a->step_index] != last_note)
    {
      ARPEGGIO_ADJUST_STEP_INDEX()
    }
  }

  return 0;
}

/*******************************************************************************
** arpeggio_key_released()
*******************************************************************************/
short int arpeggio_key_released(int instrument_index, int note)
{
  int m;

  arpeggio* a;

  int last_note;
  int last_half;

  int shifted_note;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the note is valid */
  if (TUNING_NOTE_IS_NOT_PLAYABLE(note))
    return 0;

  /* obtain arpeggio pointer */
  a = &G_arpeggio_bank[instrument_index];

  /* check if the arpeggio is on */
  if (a->on_switch == MIDI_CONT_TOGGLE_SWITCH_ON)
  {
    /* send a note off to this note in all octaves */
    for (m = 0; m < a->octaves; m++)
    {
      shifted_note = (12 * m) + note;

      if (shifted_note > TUNING_NOTE_C8)
        shifted_note = TUNING_NOTE_C8;

      instrument_note_off(instrument_index, shifted_note);
    }

    /* determine last note */
    if (a->num_notes == 0)
      last_note = TUNING_NOTE_BLANK;
    else
      last_note = a->pattern_steps[a->step_index];

    /* determine last half */
    if ((a->pattern == PATCH_ARPEGGIO_PATTERN_UP_AND_DOWN) || 
        (a->pattern == PATCH_ARPEGGIO_PATTERN_UP_AND_DOWN_ALT))
    {
      if (a->step_index >= a->num_steps / 2)
        last_half = 1;
    }
    else
      last_half = 0;

    /* compute pattern steps */
    arpeggio_compute_pattern_steps(instrument_index);

    /* adjust the step index, if necessary */
    if (a->pattern_steps[a->step_index] != last_note)
    {
      ARPEGGIO_ADJUST_STEP_INDEX()
    }
  }

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
  for (k = 0; k < BANK_NUM_ARPEGGIOS; k++)
  {
    /* obtain arpeggio pointer */
    a = &G_arpeggio_bank[k];

    /* update phase */
    a->phase += a->increment;

    /* wraparound phase register (28 bits) */
    if (a->phase > 0xFFFFFFF)
    {
      a->phase &= 0xFFFFFFF;

      /* if this arpeggio is off, skip it! */
      if (a->on_switch == MIDI_CONT_TOGGLE_SWITCH_OFF)
        continue;

      /* if no keys are pressed, skip it! */
      if (a->num_steps == 0)
        continue;

      /* send note off for the previous note, if necessary */
      if (a->mode == PATCH_ARPEGGIO_MODE_HARP)
        instrument_note_off(k, a->pattern_steps[a->step_index]);

      /* update step index */
      a->step_index += 1;

      if (a->step_index >= a->num_steps)
        a->step_index = 0;

      /* trigger next note */
      instrument_note_on(k, a->pattern_steps[a->step_index]);
    }
  }

  return 0;
}

/*******************************************************************************
** arpeggio_generate_tables()
*******************************************************************************/
short int arpeggio_generate_tables()
{
  int m;

  /* phase increment table */
  for (m = 0; m < TEMPO_NUM_VALUES; m++)
  {
    S_arpeggio_phase_increment_table[m] = 
      (int) ((1 * TEMPO_COMPUTE_BEATS_PER_SECOND(m + TEMPO_LOWER_BOUND) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
  }

  return 0;
}

