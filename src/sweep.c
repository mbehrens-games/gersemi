/*******************************************************************************
** sweep.c (portamento / glissando)
*******************************************************************************/

#include <stdio.h>    /* testing */
#include <stdlib.h>
#include <math.h>

#include "bank.h"
#include "cart.h"
#include "clock.h"
#include "instrument.h"
#include "midicont.h"
#include "sweep.h"
#include "tempo.h"
#include "tuning.h"

#define PI      3.14159265358979323846f
#define TWO_PI  6.28318530717958647693f

/* speed table (in semitones per beat) */
static int  S_sweep_speed_table[16] = 
            { 1,  2,  3,  4,  5,  6,  7,  8, 
              9, 10, 11, 12, 16, 19, 22, 24 
            };

/* phase increment table */
static unsigned int S_sweep_phase_increment_table[TEMPO_NUM_VALUES];

/* sweep bank */
sweep G_sweep_bank[BANK_NUM_SWEEPS];

/*******************************************************************************
** sweep_reset_all()
*******************************************************************************/
short int sweep_reset_all()
{
  int k;

  sweep* sw;

  /* setup all sweeps */
  for (k = 0; k < BANK_NUM_SWEEPS; k++)
  {
    /* obtain sweep pointer */
    sw = &G_sweep_bank[k];

    /* initialize sweep variables */
    sw->mode = 0;
    sw->legato = 0;
    sw->speed = 0;

    sw->tempo = TEMPO_DEFAULT;

    sw->phase = 0;

    sw->increment = S_sweep_phase_increment_table[sw->tempo];
    sw->increment *= S_sweep_speed_table[0];

    sw->note = TUNING_NOTE_BLANK;
    sw->target = 0;
    sw->offset = 0;

    sw->on_switch = MIDI_CONT_TOGGLE_SWITCH_OFF;

    sw->level = 0;
  }

  return 0;
}

/*******************************************************************************
** sweep_load_patch()
*******************************************************************************/
short int sweep_load_patch( int instrument_index, 
                            int cart_index, int patch_index)
{
  sweep* sw;

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

  /* obtain sweep pointer */
  sw = &G_sweep_bank[instrument_index];

  /* load patch parameters */
  if (PATCH_PARAM_IS_VALID_LOOKUP_BY_NAME(PORTAMENTO_MODE))
    sw->mode = p->values[PATCH_PARAM_PORTAMENTO_MODE];
  else
    sw->mode = 0;

  if (PATCH_PARAM_IS_VALID_LOOKUP_BY_NAME(PORTAMENTO_LEGATO))
    sw->legato = p->values[PATCH_PARAM_PORTAMENTO_LEGATO];
  else
    sw->legato = 0;

  if (PATCH_PARAM_IS_VALID_LOOKUP_BY_NAME(PORTAMENTO_SPEED))
    sw->speed = p->values[PATCH_PARAM_PORTAMENTO_SPEED];
  else
    sw->speed = 0;

  /* set phase increment */
  sw->increment = S_sweep_phase_increment_table[sw->tempo];
  sw->increment *= S_sweep_speed_table[sw->speed];

  return 0;
}

/*******************************************************************************
** sweep_set_speed()
*******************************************************************************/
short int sweep_set_speed(int instrument_index, short int speed)
{
  sweep* sw;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the speed is valid */
  if (PATCH_VALUE_IS_NOT_VALID_LOOKUP_BY_NAME(speed, PORTAMENTO_SPEED))
    return 0;

  /* obtain sweep pointer */
  sw = &G_sweep_bank[instrument_index];

  /* set the speed */
  sw->speed = speed;

  /* set phase increment */
  sw->increment = S_sweep_phase_increment_table[sw->tempo];
  sw->increment *= S_sweep_speed_table[sw->speed];

  return 0;
}

/*******************************************************************************
** sweep_set_switch()
*******************************************************************************/
short int sweep_set_switch(int instrument_index, short int state)
{
  int m;

  sweep* sw;
  instrument* ins;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the switch state is valid */
  if ((state != MIDI_CONT_TOGGLE_SWITCH_OFF) && 
      (state != MIDI_CONT_TOGGLE_SWITCH_ON))
  {
    return 0;
  }

  /* obtain sweep pointer */
  sw = &G_sweep_bank[instrument_index];

  /* obtain instrument pointer */
  ins = &G_instrument_bank[instrument_index];

  /* if the switch is already set to this state, return */
  if (sw->on_switch == state)
    return 0;

  /* set the switch */
  sw->on_switch = state;

  /* if the portamento is now on, send a note off to all notes and reset slide */
  if (sw->on_switch == MIDI_CONT_TOGGLE_SWITCH_ON)
  {
    for (m = 0; m < ins->num_pressed; m++)
      instrument_note_off(instrument_index, ins->pressed_keys[m]);

    sw->note = TUNING_NOTE_BLANK;
    sw->target = 0;
    sw->offset = 0;
  }
  /* if the portamento is now off, send a note-off to the slide note */
  else if (sw->on_switch == MIDI_CONT_TOGGLE_SWITCH_OFF)
  {
    instrument_note_off(instrument_index, sw->note);

    sw->note = TUNING_NOTE_BLANK;
  }

  return 0;
}

/*******************************************************************************
** sweep_set_tempo()
*******************************************************************************/
short int sweep_set_tempo(int instrument_index, short int tempo)
{
  sweep* sw;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* obtain sweep pointer */
  sw = &G_sweep_bank[instrument_index];

  /* set tempo */
  if (tempo < 0)
    sw->tempo = 0;
  else if (tempo > TEMPO_NUM_VALUES - 1)
    sw->tempo = TEMPO_NUM_VALUES - 1;
  else
    sw->tempo = tempo;

  /* set phase increment */
  sw->increment = S_sweep_phase_increment_table[sw->tempo];
  sw->increment *= S_sweep_speed_table[sw->speed];

  return 0;
}

/*******************************************************************************
** sweep_tempo_sync()
*******************************************************************************/
short int sweep_tempo_sync(int instrument_index)
{
  sweep* sw;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* obtain sweep pointer */
  sw = &G_sweep_bank[instrument_index];

  /* reset phase */
  sw->phase = 0;

  return 0;
}

/*******************************************************************************
** sweep_key_pressed()
*******************************************************************************/
short int sweep_key_pressed(int instrument_index, int note)
{
  sweep* sw;
  instrument* ins;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the note is valid */
  if (TUNING_NOTE_IS_NOT_PLAYABLE(note))
    return 0;

  /* obtain sweep pointer */
  sw = &G_sweep_bank[instrument_index];

  /* obtain instrument pointer */
  ins = &G_instrument_bank[instrument_index];

  /* check if the portamento is on */
  if (sw->on_switch == MIDI_CONT_TOGGLE_SWITCH_ON)
  {
    /* if this is the only key pressed, just play this note without any pitch slide */
    if (ins->num_pressed == 1)
    {
      sw->note = note;
      sw->target = 0;
      sw->offset = 0;

      instrument_note_on(instrument_index, note);
    }
    /* otherwise, start or continue the pitch slide */
    else if (ins->num_pressed > 1)
    {
      /* if the legato is off, store the end note of the slide */
      if (sw->legato == 0)
      {
        if (sw->note == TUNING_NOTE_BLANK)
          sw->offset = (ins->pressed_keys[1] - ins->pressed_keys[0]) * TUNING_NUM_SEMITONE_STEPS;
        else
          sw->offset += (sw->note - ins->pressed_keys[0]) * TUNING_NUM_SEMITONE_STEPS;

        sw->note = ins->pressed_keys[0];
        sw->target = 0;

        instrument_note_on(instrument_index, sw->note);
      }
      /* if the legato is on, store the start note of the slide */
      else
      {
        if (sw->note == TUNING_NOTE_BLANK)
        {
          sw->offset = 0;
          sw->note = ins->pressed_keys[1];

          instrument_note_on(instrument_index, sw->note);
        }

        sw->target = (ins->pressed_keys[0] - sw->note) * TUNING_NUM_SEMITONE_STEPS;
      }
    }
  }
  else
    instrument_note_on(instrument_index, note);

  return 0;
}

/*******************************************************************************
** sweep_key_released()
*******************************************************************************/
short int sweep_key_released(int instrument_index, int note)
{
  sweep* sw;
  instrument* ins;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the note is valid */
  if (TUNING_NOTE_IS_NOT_PLAYABLE(note))
    return 0;

  /* obtain sweep pointer */
  sw = &G_sweep_bank[instrument_index];

  /* obtain instrument pointer */
  ins = &G_instrument_bank[instrument_index];

  /* check if the portamento is on */
  if (sw->on_switch == MIDI_CONT_TOGGLE_SWITCH_ON)
  {
    /* if the sweep is inactive, just send the note-off */
    if (sw->note == TUNING_NOTE_BLANK)
    {
      instrument_note_off(instrument_index, note);
    }
    /* if the legato is off, send the note-off when */
    /* the end note of the pitch slide is released  */
    else if (sw->legato == 0)
    {
      /* the stored note is the end note of the slide */
      if (note == sw->note)
      {
        instrument_note_off(instrument_index, sw->note);
        sw->note = TUNING_NOTE_BLANK;
      }
    }
    /* if the legato is set to follow, send the note-off  */
    /* when the end note of the pitch slide is released   */
    else if (sw->legato == 1)
    {
      /* the stored note is the start note of the slide */
      if (note == (sw->note + (sw->target / TUNING_NUM_SEMITONE_STEPS)))
      {
        instrument_note_off(instrument_index, sw->note);
        sw->note = TUNING_NOTE_BLANK;
      }
    }
    /* if the legato is set to hammer-on, send the note-off */
    /* when the start note of the pitch slide is released   */
    else if (sw->legato == 2)
    {
      /* the stored note is the start note of the slide */
      if (note == sw->note)
      {
        instrument_note_off(instrument_index, sw->note);
        sw->note = TUNING_NOTE_BLANK;
      }
      /* otherwise, re-direct slide to the last pressed key */
      else if (ins->num_pressed > 0)
        sw->target = (ins->pressed_keys[0] - sw->note) * TUNING_NUM_SEMITONE_STEPS;
    }
  }
  else
    instrument_note_off(instrument_index, note);

  return 0;
}

/*******************************************************************************
** sweep_update_all()
*******************************************************************************/
short int sweep_update_all()
{
  int k;

  sweep* sw;

  /* update all sweeps */
  for (k = 0; k < BANK_NUM_SWEEPS; k++)
  {
    /* obtain sweep pointer */
    sw = &G_sweep_bank[k];

    /* update phase */
    sw->phase += sw->increment;

    /* check if a period was completed */

    /* wraparound phase register (28 bits) */
    if (sw->phase > 0xFFFFFFF)
    {
      if (sw->offset < sw->target)
        sw->offset += 1;
      else if (sw->offset > sw->target)
        sw->offset -= 1;

      sw->phase &= 0xFFFFFFF;
    }

    /* set level based on mode */
    if (sw->mode == 0)
      sw->level = sw->offset;
    else if (sw->mode == 1)
    {
      if (sw->offset >= 0)
      {
        sw->level = sw->offset;
        sw->level /= TUNING_NUM_SEMITONE_STEPS;
        sw->level *= TUNING_NUM_SEMITONE_STEPS;
      }
      else
      {
        sw->level = -sw->offset;
        sw->level /= TUNING_NUM_SEMITONE_STEPS;
        sw->level *= -TUNING_NUM_SEMITONE_STEPS;
      }
    }
    else
      sw->level = 0;
  }

  return 0;
}

/*******************************************************************************
** sweep_generate_tables()
*******************************************************************************/
short int sweep_generate_tables()
{
  int m;

  /* phase increment table */
  for (m = 0; m < TEMPO_NUM_VALUES; m++)
  {
    S_sweep_phase_increment_table[m] = 
      (int) ((1 * TUNING_NUM_SEMITONE_STEPS * TEMPO_COMPUTE_BEATS_PER_SECOND(m) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
  }

  return 0;
}

