/*******************************************************************************
** instrument.c (synth instrument)
*******************************************************************************/

#include <stdlib.h>
#include <math.h>

#include "bank.h"
#include "bender.h"
#include "boost.h"
#include "envelope.h"
#include "filter.h"
#include "instrument.h"
#include "lfo.h"
#include "midicont.h"
#include "patch.h"
#include "sweep.h"
#include "tuning.h"
#include "voice.h"

/* instrument bank */
instrument G_instrument_bank[BANK_NUM_INSTRUMENTS];

/*******************************************************************************
** instrument_setup_all()
*******************************************************************************/
short int instrument_setup_all()
{
  int k;

  /* setup all instruments */
  for (k = 0; k < BANK_NUM_INSTRUMENTS; k++)
    instrument_reset(k);

  return 0;
}

/*******************************************************************************
** instrument_reset()
*******************************************************************************/
short int instrument_reset(int instrument_index)
{
  int k;

  instrument* ins;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* obtain instrument pointer */
  ins = &G_instrument_bank[instrument_index];

  /* type & voice index */
  if (instrument_index == INSTRUMENT_POLY_INDEX)
  {
    ins->type = INSTRUMENT_TYPE_POLY;
    ins->voice_index = BANK_INST_VOICES_START_INDEX;
  }
  else if ( (instrument_index >= INSTRUMENT_MONO_START_INDEX) && 
            (instrument_index <= INSTRUMENT_MONO_END_INDEX))
  {
    ins->type = INSTRUMENT_TYPE_MONO;
    ins->voice_index = BANK_INST_VOICES_START_INDEX + 4 + (instrument_index - INSTRUMENT_MONO_START_INDEX);
  }
  else if (instrument_index == INSTRUMENT_DRUMS_INDEX)
  {
    ins->type = INSTRUMENT_TYPE_DRUMS;
    ins->voice_index = BANK_DRUM_VOICES_START_INDEX;
  }
  else
    return 0;

  /* notes */
  for (k = 0; k < INSTRUMENT_NUM_PRESSED_NOTES; k++)
    ins->pressed_notes[k] = TUNING_NOTE_BLANK;

  for (k = 0; k < INSTRUMENT_NUM_SWEEP_NOTES; k++)
    ins->sweep_notes[k] = TUNING_NOTE_BLANK;

  /* volume & panning */
  ins->volume = 0;
  ins->panning = 0;

  /* note velocity */
  ins->note_velocity = MIDI_CONT_NOTE_VELOCITY_DEFAULT;

   /* mod wheel, aftertouch, pitch wheel */
  ins->mod_wheel_pos = MIDI_CONT_MOD_WHEEL_DEFAULT;
  ins->aftertouch_pos = MIDI_CONT_AFTERTOUCH_DEFAULT;
  ins->pitch_wheel_pos = MIDI_CONT_PITCH_WHEEL_DEFAULT;

  /* portamento switch, arpeggio switch, sustain pedal */
  ins->portamento_switch = MIDI_CONT_PORTAMENTO_SWITCH_DEFAULT;
  ins->arpeggio_switch = MIDI_CONT_ARPEGGIO_SWITCH_DEFAULT;
  ins->sustain_pedal = MIDI_CONT_SUSTAIN_PEDAL_DEFAULT;

  /* legato */
  ins->legato = PATCH_PORTAMENTO_LEGATO_DEFAULT;

  return 0;
}

/*******************************************************************************
** instrument_load_patch()
*******************************************************************************/
short int instrument_load_patch(int instrument_index, 
                                int cart_number, int patch_number)
{
  int k;

  instrument* ins;
  patch* p;

  int voice_index;
  int patch_index;

  int bound;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure that the cart and patch numbers are valid */
  if (PATCH_CART_NO_IS_NOT_VALID(cart_number))
    return 1;

  if (PATCH_PATCH_NO_IS_NOT_VALID(patch_number))
    return 1;

  /* determine patch index */
  PATCH_COMPUTE_PATCH_INDEX(cart_number, patch_number)

  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain instrument and patch pointers */
  ins = &G_instrument_bank[instrument_index];
  p = &G_patch_bank[patch_index];

  /* set voice index and bound */
  if (ins->type == INSTRUMENT_TYPE_POLY)
    bound = 4;
  else if (ins->type == INSTRUMENT_TYPE_MONO)
    bound = 1;
  else
    bound = 1;

  voice_index = ins->voice_index;

  /* load patch to all voices associated with this instrument */
  for (k = 0; k < bound; k++)
  {
    voice_load_patch(voice_index + k, patch_index);
    envelope_load_patch(voice_index + k, patch_index);
    lfo_load_patch(voice_index + k, patch_index);
    boost_load_patch(voice_index + k, patch_index);
    sweep_load_patch(voice_index + k, patch_index);
    bender_load_patch(voice_index + k, patch_index);
    filter_load_patch(voice_index + k, patch_index);
  }

  /* legato */
  if ((p->portamento_legato >= PATCH_PORTAMENTO_LEGATO_LOWER_BOUND) && 
      (p->portamento_legato <= PATCH_PORTAMENTO_LEGATO_UPPER_BOUND))
  {
    ins->legato = p->portamento_legato;
  }
  else
    ins->legato = PATCH_PORTAMENTO_LEGATO_LOWER_BOUND;

  return 0;
}

/*******************************************************************************
** instrument_key_on()
*******************************************************************************/
short int instrument_key_on(int instrument_index, int note)
{
  int k;
  int m;

  instrument* ins;
  voice* v;

  int base_voice_index;
  int selected_voice_index;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* obtain instrument pointer */
  ins = &G_instrument_bank[instrument_index];

  /* make sure the note is valid */
  if ((note < TUNING_NOTE_A0) || (note > TUNING_NOTE_C8))
    return 0;

  /* check if this note was recently pressed. if it   */
  /* was, move it to the front of the pressed notes.  */
  for (k = 0; k < INSTRUMENT_NUM_PRESSED_NOTES; k++)
  {
    if (ins->pressed_notes[k] == note)
    {
      for (m = k; m > 0; m--)
        ins->pressed_notes[m] = ins->pressed_notes[m - 1];

      ins->pressed_notes[0] = note;

      break;
    }
  }

  /* otherwise, add this note to the front and  */
  /* shift the current notes back in the list.  */
  if (ins->pressed_notes[0] != note)
  {
    /* if a note is being shifted out of the pressed  */
    /* notes array, send a key-off to that note.      */
    if (ins->pressed_notes[INSTRUMENT_NUM_PRESSED_NOTES - 1] != TUNING_NOTE_BLANK)
    {
      instrument_key_off( instrument_index, 
                          ins->pressed_notes[INSTRUMENT_NUM_PRESSED_NOTES - 1]);
    }

    /* add the new pressed note */
    for (m = INSTRUMENT_NUM_PRESSED_NOTES - 1; m > 0; m--)
      ins->pressed_notes[m] = ins->pressed_notes[m - 1];

    ins->pressed_notes[0] = note;
  }

  /* cycle sweep notes */
  ins->sweep_notes[1] = ins->sweep_notes[0];
  ins->sweep_notes[0] = note;

  if (ins->sweep_notes[0] == ins->sweep_notes[1])
    ins->sweep_notes[1] = TUNING_NOTE_BLANK;

  /* determine selected voice for this key on */
  base_voice_index = ins->voice_index;
  selected_voice_index = -1;

  if (ins->type == INSTRUMENT_TYPE_POLY)
  {
    /* if the portamento is on, use the voice of      */
    /* the most recently pressed note for this key on */
    if (ins->portamento_switch == MIDI_CONT_SWITCH_STATE_ON)
    {
      for (k = 0; k < 4; k++)
      {
        v = &G_voice_bank[base_voice_index + k];

        if ((v->base_note == ins->pressed_notes[1]) && 
            (ins->pressed_notes[1] != TUNING_NOTE_BLANK))
        {
          selected_voice_index = base_voice_index + k;
          break;
        }
      }
    }

    /* if a voice is currently playing this note, */
    /* retrigger the note on that voice.          */
    if (selected_voice_index == -1)
    {
      for (k = 0; k < 4; k++)
      {
        v = &G_voice_bank[base_voice_index + k];

        if (v->base_note == note)
        {
          selected_voice_index = base_voice_index + k;
          break;
        }
      }
    }

    /* if a voice is inactive, use that */
    /* voice for playing the new note.  */
    if (selected_voice_index == -1)
    {
      for (k = 0; k < 4; k++)
      {
        v = &G_voice_bank[base_voice_index + k];

        if (v->base_note == TUNING_NOTE_BLANK)
        {
          selected_voice_index = base_voice_index + k;
          break;
        }
      }
    }

    /* if a voice is still playing a note that is not   */
    /* pressed right now, replace it with the new note. */
    if (selected_voice_index == -1)
    {
      for (k = 0; k < 4; k++)
      {
        v = &G_voice_bank[base_voice_index + k];

        selected_voice_index = base_voice_index + k;

        for (m = 1; m < INSTRUMENT_NUM_PRESSED_NOTES; m++)
        {
          if (v->base_note == ins->pressed_notes[m])
          {
            selected_voice_index = -1;
            break;
          }
        }

        if (selected_voice_index != -1)
          break;
      }
    }
  }
  else if (ins->type == INSTRUMENT_TYPE_MONO)
  {
    /* just use the only voice available! */
    selected_voice_index = base_voice_index;
  }
  else
    return 0;

  /* if no voice index found, return */
  if (selected_voice_index == -1)
    return 0;

  /* send key on to the selected voice associated with this instrument */
  voice_set_note(selected_voice_index, note);
  envelope_set_note(selected_voice_index, note);

  if ((ins->portamento_switch == MIDI_CONT_SWITCH_STATE_ON) && 
      (ins->sweep_notes[1] != TUNING_NOTE_BLANK))
  {
    if (ins->legato == PATCH_PORTAMENTO_LEGATO_OFF)
    {
      voice_sync_phases(selected_voice_index);
      lfo_sync_phase(selected_voice_index);
      envelope_trigger(selected_voice_index);
    }

    sweep_trigger(selected_voice_index, ins->sweep_notes[0]);
  }
  else
  {
    voice_sync_phases(selected_voice_index);
    lfo_sync_phase(selected_voice_index);
    envelope_trigger(selected_voice_index);

    sweep_set_note(selected_voice_index, ins->sweep_notes[0]);
  }

  lfo_trigger(selected_voice_index);

  return 0;
}

/*******************************************************************************
** instrument_key_off()
*******************************************************************************/
short int instrument_key_off(int instrument_index, int note)
{
  int k;
  int m;

  instrument* ins;
  voice* v;

  int base_voice_index;
  int selected_voice_index;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* obtain instrument pointer */
  ins = &G_instrument_bank[instrument_index];

  /* make sure the note is valid */
  if ((note < TUNING_NOTE_A0) || (note > TUNING_NOTE_C8))
    return 0;

  /* remove this note from the pressed notes list */
  for (k = 0; k < INSTRUMENT_NUM_PRESSED_NOTES; k++)
  {
    if (ins->pressed_notes[k] == note)
    {
      for (m = k; m < INSTRUMENT_NUM_PRESSED_NOTES - 1; m++)
        ins->pressed_notes[m] = ins->pressed_notes[m + 1];

      ins->pressed_notes[INSTRUMENT_NUM_PRESSED_NOTES - 1] = TUNING_NOTE_BLANK;

      break;
    }
  }

  /* remove this note from the sweep notes list */
  if (ins->sweep_notes[1] == note)
    ins->sweep_notes[1] = TUNING_NOTE_BLANK;

  if (ins->sweep_notes[0] == note)
  {
    ins->sweep_notes[0] = TUNING_NOTE_BLANK;
    ins->sweep_notes[1] = TUNING_NOTE_BLANK;
  }

  /* determine selected voice for this key off */
  base_voice_index = ins->voice_index;
  selected_voice_index = -1;

  if (ins->type == INSTRUMENT_TYPE_POLY)
  {
    /* if a voice is currently playing this note, */
    /* send the key off to that voice.            */
    for (k = 0; k < 4; k++)
    {
      v = &G_voice_bank[base_voice_index + k];

      if (v->base_note == note)
      {
        selected_voice_index = base_voice_index + k;
        break;
      }
    }
  }
  else if (ins->type == INSTRUMENT_TYPE_MONO)
  {
    v = &G_voice_bank[base_voice_index];

    if (v->base_note == note)
      selected_voice_index = base_voice_index;
  }
  else
    return 0;

  /* if no voice index found, return */
  if (selected_voice_index == -1)
    return 0;

#if 0
  /* double check that the selected voice is playing this note */
  v = &G_voice_bank[selected_voice_index];

  if (v->base_note != note)
    return 0;
#endif

  /* send key off to the selected voice associated with this instrument */
  envelope_release(selected_voice_index);

  return 0;
}

/*******************************************************************************
** instrument_set_note_velocity()
*******************************************************************************/
short int instrument_set_note_velocity(int instrument_index, short int vel)
{
  int k;

  instrument* ins;
  voice* v;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the note velocity is valid */
  if ((vel < MIDI_CONT_NOTE_VELOCITY_LOWER_BOUND) || 
      (vel > MIDI_CONT_NOTE_VELOCITY_UPPER_BOUND))
  {
    return 0;
  }

  /* obtain instrument pointer */
  ins = &G_instrument_bank[instrument_index];

  /* if this new velocity is the same as the current velocity, return */
  if (vel == ins->note_velocity)
    return 0;

  /* set the new note velocity */
  ins->note_velocity = vel;

  /* set the velocity input for each voice associated with this instrument */
  if (ins->type == INSTRUMENT_TYPE_POLY)
  {
    for (k = 0; k < 4; k++)
    {
      v = &G_voice_bank[ins->voice_index + k];
      v->note_velocity = vel;
    }
  }
  else if (ins->type == INSTRUMENT_TYPE_MONO)
  {
    v = &G_voice_bank[ins->voice_index];
    v->note_velocity = vel;
  }

  return 0;
}

/*******************************************************************************
** instrument_set_mod_wheel_position()
*******************************************************************************/
short int instrument_set_mod_wheel_position(int instrument_index, short int pos)
{
  int k;

  instrument* ins;
  lfo* l;
  boost* b;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the mod wheel position is valid */
  if ((pos < MIDI_CONT_MOD_WHEEL_LOWER_BOUND) || 
      (pos > MIDI_CONT_MOD_WHEEL_UPPER_BOUND))
  {
    return 0;
  }

  /* obtain instrument pointer */
  ins = &G_instrument_bank[instrument_index];

  /* if this new position is the same as the current position, return */
  if (pos == ins->mod_wheel_pos)
    return 0;

  /* set the new mod wheel position */
  ins->mod_wheel_pos = pos;

  /* set the mod wheel input for each voice associated with this instrument */
  if (ins->type == INSTRUMENT_TYPE_POLY)
  {
    for (k = 0; k < 4; k++)
    {
      l = &G_lfo_bank[ins->voice_index + k];
      l->mod_wheel_input = pos;

      b = &G_boost_bank[ins->voice_index + k];
      b->mod_wheel_input = pos;
    }
  }
  else if (ins->type == INSTRUMENT_TYPE_MONO)
  {
    l = &G_lfo_bank[ins->voice_index];
    l->mod_wheel_input = pos;

    b = &G_boost_bank[ins->voice_index];
    b->mod_wheel_input = pos;
  }

  return 0;
}

/*******************************************************************************
** instrument_set_aftertouch_position()
*******************************************************************************/
short int instrument_set_aftertouch_position(int instrument_index, short int pos)
{
  int k;

  instrument* ins;
  lfo* l;
  boost* b;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the aftertouch position is valid */
  if ((pos < MIDI_CONT_AFTERTOUCH_LOWER_BOUND) || 
      (pos > MIDI_CONT_AFTERTOUCH_UPPER_BOUND))
  {
    return 0;
  }

  /* obtain instrument pointer */
  ins = &G_instrument_bank[instrument_index];

  /* if this new position is the same as the current position, return */
  if (pos == ins->aftertouch_pos)
    return 0;

  /* set the new aftertouch position */
  ins->aftertouch_pos = pos;

  /* set the aftertouch input for each voice associated with this instrument */
  if (ins->type == INSTRUMENT_TYPE_POLY)
  {
    for (k = 0; k < 4; k++)
    {
      l = &G_lfo_bank[ins->voice_index + k];
      l->aftertouch_input = pos;

      b = &G_boost_bank[ins->voice_index + k];
      b->aftertouch_input = pos;

    }
  }
  else if (ins->type == INSTRUMENT_TYPE_MONO)
  {
    l = &G_lfo_bank[ins->voice_index];
    l->aftertouch_input = pos;

    b = &G_boost_bank[ins->voice_index];
    b->aftertouch_input = pos;
  }

  return 0;
}

/*******************************************************************************
** instrument_set_pitch_wheel_position()
*******************************************************************************/
short int instrument_set_pitch_wheel_position(int instrument_index, short int pos)
{
  int k;

  instrument* ins;
  bender* b;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the pitch wheel position is valid */
  if ((pos < MIDI_CONT_PITCH_WHEEL_LOWER_BOUND) || 
      (pos > MIDI_CONT_PITCH_WHEEL_UPPER_BOUND))
  {
    return 0;
  }

  /* obtain instrument pointer */
  ins = &G_instrument_bank[instrument_index];

  /* if this new position is the same as the current position, return */
  if (pos == ins->pitch_wheel_pos)
    return 0;

  /* set the new pitch wheel position */
  ins->pitch_wheel_pos = pos;

  /* set the pitch wheel input for each voice associated with this instrument */
  if (ins->type == INSTRUMENT_TYPE_POLY)
  {
    for (k = 0; k < 4; k++)
    {
      b = &G_bender_bank[ins->voice_index + k];
      b->pitch_wheel_input = pos;
    }
  }
  else if (ins->type == INSTRUMENT_TYPE_MONO)
  {
    b = &G_bender_bank[ins->voice_index];
    b->pitch_wheel_input = pos;
  }

  return 0;
}

/*******************************************************************************
** instrument_set_portamento_switch()
*******************************************************************************/
short int instrument_set_portamento_switch(int instrument_index, int state)
{
  instrument* ins;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the switch state is valid */
  if ((state < MIDI_CONT_PORTAMENTO_SWITCH_LOWER_BOUND) || 
      (state > MIDI_CONT_PORTAMENTO_SWITCH_UPPER_BOUND))
  {
    return 0;
  }

  /* obtain instrument pointer */
  ins = &G_instrument_bank[instrument_index];

  /* set the portamento switch in the instrument */
  ins->portamento_switch = state;

  return 0;
}

/*******************************************************************************
** instrument_set_arpeggio_switch()
*******************************************************************************/
short int instrument_set_arpeggio_switch(int instrument_index, int state)
{
  instrument* ins;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the switch state is valid */
  if ((state < MIDI_CONT_ARPEGGIO_SWITCH_LOWER_BOUND) || 
      (state > MIDI_CONT_ARPEGGIO_SWITCH_UPPER_BOUND))
  {
    return 0;
  }

  /* obtain instrument pointer */
  ins = &G_instrument_bank[instrument_index];

  /* set the arpeggio switch in the instrument */
  ins->arpeggio_switch = state;

  return 0;
}

/*******************************************************************************
** instrument_set_sustain_pedal()
*******************************************************************************/
short int instrument_set_sustain_pedal(int instrument_index, int state)
{
  int k;

  instrument* ins;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the pedal state is valid */
  if ((state < MIDI_CONT_SUSTAIN_PEDAL_LOWER_BOUND) || 
      (state > MIDI_CONT_SUSTAIN_PEDAL_UPPER_BOUND))
  {
    return 0;
  }

  /* obtain instrument pointer */
  ins = &G_instrument_bank[instrument_index];

  /* set the sustain pedal in the instrument */
  ins->sustain_pedal = state;

  /* set the sustain pedal for each voice associated with this instrument */
  if (ins->type == INSTRUMENT_TYPE_POLY)
  {
    for (k = 0; k < 4; k++)
    {
      envelope_set_sustain_pedal(ins->voice_index + k, ins->sustain_pedal);
    }
  }
  else if (ins->type == INSTRUMENT_TYPE_MONO)
  {
    envelope_set_sustain_pedal(ins->voice_index, ins->sustain_pedal);
  }

  return 0;
}

