/*******************************************************************************
** arpeggio.c (arpeggiator)
*******************************************************************************/

#include <stdio.h>    /* testing */
#include <stdlib.h>
#include <math.h>

#include "arpeggio.h"
#include "bank.h"
#include "cart.h"
#include "clock.h"
#include "instrument.h"
#include "midicont.h"
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
  if (a->pattern == 0)                                                         \
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
  else if (a->pattern == 1)                                                    \
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
  else if ( (a->pattern == 2) ||                                               \
            (a->pattern == 3))                                                 \
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
static int  S_arpeggio_speed_table[12] = 
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
    a->mode = 0;
    a->pattern = 0;
    a->octaves = 0;
    a->speed = 0;

    a->tempo = TEMPO_DEFAULT;

    a->phase = 0;

    a->increment = S_arpeggio_phase_increment_table[a->tempo];
    a->increment *= S_arpeggio_speed_table[0];

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
  if (a->pattern == 0)
  {
    a->num_steps = a->num_notes;
  }
  else if (a->pattern == 1)
  {
    a->num_steps = a->num_notes;

    for (m = 0; m < a->num_notes / 2; m++)
    {
      val = a->pattern_steps[a->num_notes - 1 - m];
      a->pattern_steps[a->num_notes - 1 - m] = a->pattern_steps[m];
      a->pattern_steps[m] = val;
    }
  }
  else if (a->pattern == 2)
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
  else if (a->pattern == 3)
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

  /* load patch parameters */
  if (PATCH_PARAM_IS_VALID_LOOKUP_BY_NAME(ARPEGGIO_MODE))
    a->mode = p->values[PATCH_PARAM_ARPEGGIO_MODE];
  else
    a->mode = 0;

  if (PATCH_PARAM_IS_VALID_LOOKUP_BY_NAME(ARPEGGIO_PATTERN))
    a->pattern = p->values[PATCH_PARAM_ARPEGGIO_PATTERN];
  else
    a->pattern = 0;

  if (PATCH_PARAM_IS_VALID_LOOKUP_BY_NAME(ARPEGGIO_OCTAVE))
    a->octaves = p->values[PATCH_PARAM_ARPEGGIO_OCTAVE];
  else
    a->octaves = 0;

  if (PATCH_PARAM_IS_VALID_LOOKUP_BY_NAME(ARPEGGIO_SPEED))
    a->speed = p->values[PATCH_PARAM_ARPEGGIO_SPEED];
  else
    a->speed = 0;

  /* set phase increment */
  a->increment = S_arpeggio_phase_increment_table[a->tempo];
  a->increment *= S_arpeggio_speed_table[a->speed];

  /* restart arpeggio if necessary */
  ARPEGGIO_RESTART()

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
  if (PATCH_VALUE_IS_NOT_VALID_LOOKUP_BY_NAME(mode, ARPEGGIO_MODE))
    return 0;

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
  if (PATCH_VALUE_IS_NOT_VALID_LOOKUP_BY_NAME(pattern, ARPEGGIO_PATTERN))
    return 0;

  /* obtain arpeggio pointer */
  a = &G_arpeggio_bank[instrument_index];

  /* set the pattern */
  a->pattern = pattern;

  /* restart arpeggio if necessary */
  ARPEGGIO_RESTART()

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
  if (PATCH_VALUE_IS_NOT_VALID_LOOKUP_BY_NAME(octaves, ARPEGGIO_OCTAVE))
    return 0;

  /* obtain arpeggio pointer */
  a = &G_arpeggio_bank[instrument_index];

  /* set the octaves */
  a->octaves = octaves;

  /* restart arpeggio if necessary */
  ARPEGGIO_RESTART()

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
  if (PATCH_VALUE_IS_NOT_VALID_LOOKUP_BY_NAME(speed, ARPEGGIO_SPEED))
    return 0;

  /* obtain arpeggio pointer */
  a = &G_arpeggio_bank[instrument_index];

  /* set the speed */
  a->speed = speed;

  /* set phase increment */
  a->increment = S_arpeggio_phase_increment_table[a->tempo];
  a->increment *= S_arpeggio_speed_table[a->speed];

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
  ARPEGGIO_RESTART()

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
  if (tempo < 0)
    a->tempo = 0;
  else if (tempo > TEMPO_NUM_VALUES - 1)
    a->tempo = TEMPO_NUM_VALUES - 1;
  else
    a->tempo = tempo;

  /* adjust phase increment based on tempo */
  a->increment = S_arpeggio_phase_increment_table[a->tempo];
  a->increment *= S_arpeggio_speed_table[a->speed];

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
    if ((a->pattern == 2) || 
        (a->pattern == 3))
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
    if ((a->pattern == 2) || 
        (a->pattern == 3))
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
      if (a->mode == 0)
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
      (int) ((1 * TEMPO_COMPUTE_BEATS_PER_SECOND(m) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
  }

  return 0;
}

