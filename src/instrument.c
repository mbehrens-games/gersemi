/*******************************************************************************
** instrument.c (synth instrument)
*******************************************************************************/

#include <stdlib.h>
#include <math.h>

#include "arpeggio.h"
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
** instrument_reset_all()
*******************************************************************************/
short int instrument_reset_all()
{
  int m;
  int n;

  instrument* ins;

  /* reset all instruments */
  for (m = 0; m < BANK_NUM_INSTRUMENTS; m++)
  {
    /* obtain instrument pointer */
    ins = &G_instrument_bank[m];

    /* type & voice index */
    if ((m >= BANK_POLY_INSTRUMENTS_START_INDEX) && 
        (m <  BANK_POLY_INSTRUMENTS_END_INDEX))
    {
      ins->type = INSTRUMENT_TYPE_POLY;

      ins->voice_index = BANK_POLY_VOICES_START_INDEX;
      ins->voice_index += BANK_VOICES_PER_POLY_INSTRUMENT * (m - BANK_POLY_INSTRUMENTS_START_INDEX);
    }
    else if ( (m >= BANK_MONO_INSTRUMENTS_START_INDEX) && 
              (m <  BANK_MONO_INSTRUMENTS_END_INDEX))
    {
      ins->type = INSTRUMENT_TYPE_MONO;

      ins->voice_index = BANK_MONO_VOICES_START_INDEX;
      ins->voice_index += BANK_VOICES_PER_MONO_INSTRUMENT * (m - BANK_MONO_INSTRUMENTS_START_INDEX);
    }
    else if ( (m >= BANK_SOUND_FX_INSTRUMENTS_START_INDEX) && 
              (m <  BANK_SOUND_FX_INSTRUMENTS_END_INDEX))
    {
      ins->type = INSTRUMENT_TYPE_MONO;

      ins->voice_index = BANK_SOUND_FX_VOICES_START_INDEX;
      ins->voice_index += BANK_VOICES_PER_SOUND_FX_INSTRUMENT * (m - BANK_SOUND_FX_INSTRUMENTS_START_INDEX);
    }
    else
    {
      ins->type = INSTRUMENT_TYPE_MONO;

      ins->voice_index = 0;
    }

    /* key arrays */
    for (n = 0; n < TUNING_NUM_PLAYABLE_NOTES; n++)
    {
      ins->pressed_keys[n] = TUNING_NOTE_BLANK;
      ins->held_keys[n] = TUNING_NOTE_BLANK;
    }

    /* key counts */
    ins->num_pressed = 0;
    ins->num_held = 0;

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
  }

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

  int patch_index;

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

  /* obtain instrument pointer */
  ins = &G_instrument_bank[instrument_index];

  /* load patch to this instrument's midi controllers */
  boost_load_patch(instrument_index, patch_index);
  bender_load_patch(instrument_index, patch_index);
  sweep_load_patch(instrument_index, patch_index);
  arpeggio_load_patch(instrument_index, patch_index);

   /* load patch to all voices associated with this instrument */
  if (ins->type == INSTRUMENT_TYPE_POLY)
  {
    for (k = 0; k < BANK_VOICES_PER_POLY_INSTRUMENT; k++)
    {
      voice_load_patch(ins->voice_index + k, patch_index);
      envelope_load_patch(ins->voice_index + k, patch_index);
      lfo_load_patch(ins->voice_index + k, patch_index);
      filter_load_patch(ins->voice_index + k, patch_index);
    }
  }
  else if (ins->type == INSTRUMENT_TYPE_MONO)
  {
    voice_load_patch(ins->voice_index, patch_index);
    envelope_load_patch(ins->voice_index, patch_index);
    lfo_load_patch(ins->voice_index, patch_index);
    filter_load_patch(ins->voice_index, patch_index);
  }

  return 0;
}

/*******************************************************************************
** instrument_note_on()
*******************************************************************************/
short int instrument_note_on(int instrument_index, int note)
{
  int k;
  int m;

  instrument* ins;
  voice* v;

  int same_voice_index;
  int newest_voice_index;
  int oldest_voice_index;
  int released_voice_index;

  int selected_voice_index;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* obtain instrument pointer */
  ins = &G_instrument_bank[instrument_index];

  /* make sure the note is valid */
  if (TUNING_NOTE_IS_NOT_PLAYABLE(note))
    return 0;

  /* determine selected voice for this note on */
  selected_voice_index = -1;

  if (ins->type == INSTRUMENT_TYPE_POLY)
  {
    /* determine voice indices */
    same_voice_index = -1;
    newest_voice_index = -1;
    oldest_voice_index = -1;
    released_voice_index = -1;

    for (k = 0; k < BANK_VOICES_PER_POLY_INSTRUMENT; k++)
    {
      v = &G_voice_bank[ins->voice_index + k];

      /* this voice is playing the new note already */
      if (same_voice_index == -1)
      {
        if (note == v->base_note)
          same_voice_index = ins->voice_index + k;
      }

      /* this voice is playing the most recently pressed key */
      if (newest_voice_index == -1)
      {
        if (ins->pressed_keys[0] == v->base_note)
          newest_voice_index = ins->voice_index + k;
      }

      /* this voice is playing the oldest pressed key */
      if (oldest_voice_index == -1)
      {
        if (ins->num_pressed < BANK_VOICES_PER_POLY_INSTRUMENT)
        {
          if (ins->pressed_keys[ins->num_pressed - 1] == v->base_note)
            oldest_voice_index = ins->voice_index + k;
        }
        else
        {
          if (ins->pressed_keys[BANK_VOICES_PER_POLY_INSTRUMENT - 1] == v->base_note)
            oldest_voice_index = ins->voice_index + k;
        }
      }

      /* this voice is playing a released key */
      if (released_voice_index == -1)
      {
        released_voice_index = ins->voice_index + k;

        for (m = 0; m < ins->num_pressed; m++)
        {
          if (ins->pressed_keys[m] == v->base_note)
          {
            released_voice_index = -1;
            break;
          }
        }
      }

      /* this voice is playing a blank note */
      if (released_voice_index == -1)
      {
        if (v->base_note == TUNING_NOTE_BLANK)
          released_voice_index = ins->voice_index + k;
      }
    }

    /* if the portamento is on, use the voice of the  */
    /* most recently pressed note for this note on    */
    if (ins->portamento_switch == MIDI_CONT_SWITCH_STATE_ON)
    {
      if (newest_voice_index != -1)
        selected_voice_index = newest_voice_index;
      else
        selected_voice_index = ins->voice_index;
    }
    /* if a voice is currently playing this note, */
    /* retrigger the note on that voice.          */
    else if (same_voice_index != -1)
    {
      selected_voice_index = same_voice_index;
    }
    /* if a voice is still playing a note that is not   */
    /* pressed right now, replace it with the new note. */
    else if (released_voice_index != -1)
    {
      selected_voice_index = released_voice_index;
    }
    /* otherwise, replace the oldest note with this note */
    else if (oldest_voice_index != -1)
    {
      selected_voice_index = oldest_voice_index;
    }
    /* if all else fails, just use the first voice available! */
    else
      selected_voice_index = ins->voice_index;
  }
  else if (ins->type == INSTRUMENT_TYPE_MONO)
  {
    /* just use the only voice available! */
    selected_voice_index = ins->voice_index;
  }
  else
    return 0;

  /* if no voice index found, return */
  if (selected_voice_index == -1)
    return 0;

  /* send note on to the selected voice associated with this instrument */
  voice_set_note(selected_voice_index, note, ins->note_velocity);
  envelope_set_note(selected_voice_index, note);

#if 0
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
#endif

  voice_sync_to_key(selected_voice_index);
  envelope_trigger(selected_voice_index);

  lfo_sync_to_key(selected_voice_index);
  lfo_trigger(selected_voice_index);

  return 0;
}

/*******************************************************************************
** instrument_note_off()
*******************************************************************************/
short int instrument_note_off(int instrument_index, int note)
{
  int k;

  instrument* ins;
  voice* v;

  int selected_voice_index;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* obtain instrument pointer */
  ins = &G_instrument_bank[instrument_index];

  /* make sure the note is valid */
  if (TUNING_NOTE_IS_NOT_PLAYABLE(note))
    return 0;

  /* determine selected voice for this note off */
  selected_voice_index = -1;

  if (ins->type == INSTRUMENT_TYPE_POLY)
  {
    for (k = 0; k < BANK_VOICES_PER_POLY_INSTRUMENT; k++)
    {
      v = &G_voice_bank[ins->voice_index + k];

      if (note == v->base_note)
        selected_voice_index = ins->voice_index + k;
    }
  }
  else if (ins->type == INSTRUMENT_TYPE_MONO)
  {
    v = &G_voice_bank[ins->voice_index];

    if (note == v->base_note)
      selected_voice_index = ins->voice_index;
  }
  else
    return 0;

  /* if no voice index found, return */
  if (selected_voice_index == -1)
    return 0;

  /* send note-off to the selected voice associated with this instrument */
  envelope_release(selected_voice_index);

  return 0;
}

/*******************************************************************************
** instrument_key_pressed()
*******************************************************************************/
short int instrument_key_pressed(int instrument_index, int note)
{
  int m;
  int n;

  instrument* ins;

  short int present_flag;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* obtain instrument pointer */
  ins = &G_instrument_bank[instrument_index];

  /* make sure the note is valid */
  if (TUNING_NOTE_IS_NOT_PLAYABLE(note))
    return 0;

  /* remove this key from the held keys, if necessary */
  for (m = 0; m < ins->num_held; m++)
  {
    if (ins->held_keys[m] == note)
    {
      for (n = m; n < ins->num_held - 1; n++)
        ins->held_keys[n] = ins->held_keys[n + 1];

      ins->held_keys[ins->num_held - 1] = TUNING_NOTE_BLANK;
      ins->num_held -= 1;

      break;
    }
  }

  /* if this key is already in the pressed keys, move it to the front */
  present_flag = 0;

  for (m = 0; m < ins->num_pressed; m++)
  {
    if (ins->pressed_keys[m] == note)
    {
      for (n = m; n > 0; n--)
        ins->pressed_keys[n] = ins->pressed_keys[n - 1];

      ins->pressed_keys[0] = note;

      present_flag = 1;

      break;
    }
  }

  /* if this key is not already in the pressed keys, add it */
  if (present_flag == 0)
  {
    for (n = ins->num_pressed; n > 0; n--)
      ins->pressed_keys[n] = ins->pressed_keys[n - 1];

    ins->pressed_keys[0] = note;

    if (ins->num_pressed < TUNING_NUM_PLAYABLE_NOTES)
      ins->num_pressed += 1;
  }

  /* send a note-on for this key */
  if (ins->arpeggio_switch == MIDI_CONT_SWITCH_STATE_OFF)
    instrument_note_on(instrument_index, note);

  return 0;
}

/*******************************************************************************
** instrument_key_released()
*******************************************************************************/
short int instrument_key_released(int instrument_index, int note)
{
  int m;
  int n;

  instrument* ins;

  short int present_flag;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* obtain instrument pointer */
  ins = &G_instrument_bank[instrument_index];

  /* make sure the note is valid */
  if (TUNING_NOTE_IS_NOT_PLAYABLE(note))
    return 0;

  /* if the sustain pedal is up, remove   */
  /* this key from the pressed keys, and  */
  /* then send a note-off for this key.   */
  if (ins->sustain_pedal == MIDI_CONT_PEDAL_STATE_UP)
  {
    /* remove this key from the pressed keys */
    for (m = 0; m < ins->num_pressed; m++)
    {
      if (ins->pressed_keys[m] == note)
      {
        for (n = m; n < ins->num_pressed - 1; n++)
          ins->pressed_keys[n] = ins->pressed_keys[n + 1];

        ins->pressed_keys[ins->num_pressed - 1] = TUNING_NOTE_BLANK;
        ins->num_pressed -= 1;

        break;
      }
    }

    if (ins->arpeggio_switch == MIDI_CONT_SWITCH_STATE_OFF)
      instrument_note_off(instrument_index, note);
  }
  /* if the sustain pedal is down,  */
  /* add this key to the held keys. */
  else if (ins->sustain_pedal == MIDI_CONT_PEDAL_STATE_DOWN)
  {
    /* if this key is already in the held keys, move it to the front */
    present_flag = 0;

    for (m = 0; m < ins->num_held; m++)
    {
      if (ins->held_keys[m] == note)
      {
        for (n = m; n > 0; n--)
          ins->held_keys[n] = ins->held_keys[n - 1];

        ins->held_keys[0] = note;

        present_flag = 1;

        break;
      }
    }

    /* if this key is not already in the held keys, add it */
    if (present_flag == 0)
    {
      for (n = ins->num_held; n > 0; n--)
        ins->held_keys[n] = ins->held_keys[n - 1];

      ins->held_keys[0] = note;

      if (ins->num_held < TUNING_NUM_PLAYABLE_NOTES)
        ins->num_held += 1;
    }
  }

  return 0;
}

/*******************************************************************************
** instrument_set_note_velocity()
*******************************************************************************/
short int instrument_set_note_velocity(int instrument_index, short int vel)
{
  instrument* ins;

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

  /* set the new note velocity (affects all subsequent notes) */
  ins->note_velocity = vel;

  return 0;
}

/*******************************************************************************
** instrument_set_mod_wheel_position()
*******************************************************************************/
short int instrument_set_mod_wheel_position(int instrument_index, short int pos)
{
  int k;

  instrument* ins;
  boost* b;
  lfo* l;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the mod wheel position is valid */
  if ((pos < MIDI_CONT_MOD_WHEEL_LOWER_BOUND) || 
      (pos > MIDI_CONT_MOD_WHEEL_UPPER_BOUND))
  {
    return 0;
  }

  /* obtain pointers */
  ins = &G_instrument_bank[instrument_index];
  b = &G_boost_bank[instrument_index];

  /* if this new position is the same as the current position, return */
  if (pos == ins->mod_wheel_pos)
    return 0;

  /* set the new mod wheel position */
  ins->mod_wheel_pos = pos;

  /* set the mod wheel input for this instrument's boost */
  b->mod_wheel_input = pos;

  /* set the mod wheel input for the lfos associated with this instrument */
  if (ins->type == INSTRUMENT_TYPE_POLY)
  {
    for (k = 0; k < BANK_VOICES_PER_POLY_INSTRUMENT; k++)
    {
      l = &G_lfo_bank[ins->voice_index + k];

      l->mod_wheel_input = pos;
    }
  }
  else if (ins->type == INSTRUMENT_TYPE_MONO)
  {
    l = &G_lfo_bank[ins->voice_index];

    l->mod_wheel_input = pos;
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
  boost* b;
  lfo* l;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the aftertouch position is valid */
  if ((pos < MIDI_CONT_AFTERTOUCH_LOWER_BOUND) || 
      (pos > MIDI_CONT_AFTERTOUCH_UPPER_BOUND))
  {
    return 0;
  }

  /* obtain pointers */
  ins = &G_instrument_bank[instrument_index];
  b = &G_boost_bank[instrument_index];

  /* if this new position is the same as the current position, return */
  if (pos == ins->aftertouch_pos)
    return 0;

  /* set the new aftertouch position */
  ins->aftertouch_pos = pos;

  /* set the aftertouch input for this instrument's boost */
  b->aftertouch_input = pos;

  /* set the aftertouch input for the lfos associated with this instrument */
  if (ins->type == INSTRUMENT_TYPE_POLY)
  {
    for (k = 0; k < BANK_VOICES_PER_POLY_INSTRUMENT; k++)
    {
      l = &G_lfo_bank[ins->voice_index + k];

      l->aftertouch_input = pos;
    }
  }
  else if (ins->type == INSTRUMENT_TYPE_MONO)
  {
    l = &G_lfo_bank[ins->voice_index];

    l->aftertouch_input = pos;
  }

  return 0;
}

/*******************************************************************************
** instrument_set_pitch_wheel_position()
*******************************************************************************/
short int instrument_set_pitch_wheel_position(int instrument_index, short int pos)
{
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

  /* obtain pointers */
  ins = &G_instrument_bank[instrument_index];
  b = &G_bender_bank[instrument_index];

  /* if this new position is the same as the current position, return */
  if (pos == ins->pitch_wheel_pos)
    return 0;

  /* set the new pitch wheel position */
  ins->pitch_wheel_pos = pos;

  /* set the pitch wheel input for this instrument's bender */
  b->pitch_wheel_input = ins->pitch_wheel_pos;

  return 0;
}

/*******************************************************************************
** instrument_set_portamento_switch()
*******************************************************************************/
short int instrument_set_portamento_switch(int instrument_index, int state)
{
  instrument* ins;
  sweep* sw;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the switch state is valid */
  if ((state < MIDI_CONT_PORTAMENTO_SWITCH_LOWER_BOUND) || 
      (state > MIDI_CONT_PORTAMENTO_SWITCH_UPPER_BOUND))
  {
    return 0;
  }

  /* obtain pointers */
  ins = &G_instrument_bank[instrument_index];
  sw = &G_sweep_bank[instrument_index];

  /* set the portamento switch in the instrument */
  ins->portamento_switch = state;

#if 0
  /* set the switch input for this instrument's sweep */
  sw->on_switch = ins->portamento_switch;
#endif

  return 0;
}

/*******************************************************************************
** instrument_set_arpeggio_switch()
*******************************************************************************/
short int instrument_set_arpeggio_switch(int instrument_index, int state)
{
  instrument* ins;
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

  /* obtain pointers */
  ins = &G_instrument_bank[instrument_index];
  a = &G_arpeggio_bank[instrument_index];

  /* set the arpeggio switch in the instrument */
  ins->arpeggio_switch = state;

  /* set the switch input for this instrument's arpeggio */
  a->on_switch = ins->arpeggio_switch;

  return 0;
}

/*******************************************************************************
** instrument_set_sustain_pedal()
*******************************************************************************/
short int instrument_set_sustain_pedal(int instrument_index, int state)
{
  int k;
  int m;
  int n;

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

  /* if this state is already set, return */
  if (ins->sustain_pedal == state)
    return 0;

  /* set the sustain pedal in the instrument */
  ins->sustain_pedal = state;

  /* set the sustain pedal for each voice associated with this instrument */
  if (ins->type == INSTRUMENT_TYPE_POLY)
  {
    for (k = 0; k < BANK_VOICES_PER_POLY_INSTRUMENT; k++)
      envelope_set_sustain_pedal(ins->voice_index + k, ins->sustain_pedal);
  }
  else if (ins->type == INSTRUMENT_TYPE_MONO)
  {
    envelope_set_sustain_pedal(ins->voice_index, ins->sustain_pedal);
  }

  /* if the sustain pedal is now up, release all held keys */
  if (ins->sustain_pedal == MIDI_CONT_PEDAL_STATE_UP)
  {
    for (k = 0; k < ins->num_held; k++)
    {
      /* remove this key from the pressed keys */
      for (m = 0; m < ins->num_pressed; m++)
      {
        if (ins->pressed_keys[m] == ins->held_keys[k])
        {
          for (n = m; n < ins->num_pressed - 1; n++)
            ins->pressed_keys[n] = ins->pressed_keys[n + 1];

          ins->pressed_keys[ins->num_pressed - 1] = TUNING_NOTE_BLANK;
          ins->num_pressed -= 1;

          break;
        }
      }

      /* send a note-off for this key */
      if (ins->arpeggio_switch == MIDI_CONT_SWITCH_STATE_OFF)
        instrument_note_off(instrument_index, ins->held_keys[k]);

      ins->held_keys[k] = TUNING_NOTE_BLANK;
    }

    ins->num_held = 0;
  }

  return 0;
}

