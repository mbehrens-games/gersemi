/*******************************************************************************
** instrument.c (synth instrument)
*******************************************************************************/

#include <stdlib.h>
#include <math.h>

#include "arpeggio.h"
#include "bank.h"
#include "envelope.h"
#include "filter.h"
#include "instrument.h"
#include "lfo.h"
#include "midicont.h"
#include "patch.h"
#include "peg.h"
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

  int next_ins_index;
  int next_voice_index;

  /* reset all instruments */
  for (m = 0; m < BANK_NUM_INSTRUMENTS; m++)
  {
    /* obtain instrument pointer */
    ins = &G_instrument_bank[m];

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
    ins->mod_wheel_pos = MIDI_CONT_UNI_WHEEL_DEFAULT;
    ins->aftertouch_pos = MIDI_CONT_UNI_WHEEL_DEFAULT;
    ins->exp_pedal_pos = MIDI_CONT_UNI_WHEEL_DEFAULT;
    ins->pitch_wheel_pos = MIDI_CONT_BI_WHEEL_DEFAULT;

    /* portamento switch, arpeggio switch, sustain pedal */
    ins->port_arp_switch = MIDI_CONT_PORT_ARP_SWITCH_OFF;
    ins->sustain_pedal = MIDI_CONT_SUSTAIN_PEDAL_UP;
  }

  /* setup instrument mappings */
  next_ins_index = 0;
  next_voice_index = 0;

  /* poly instruments */
  for (m = 0; m < BANK_NUM_POLY_INSTRUMENTS; m++)
  {
    ins = &G_instrument_bank[next_ins_index];

    ins->voice_index = next_voice_index;
    ins->last_offset = 0;
    ins->polyphony = BANK_VOICES_PER_POLY_INSTRUMENT;

    next_ins_index += 1;
    next_voice_index += BANK_VOICES_PER_POLY_INSTRUMENT;
  }

  /* mono instruments */
  for (m = 0; m < BANK_NUM_MONO_INSTRUMENTS; m++)
  {
    ins = &G_instrument_bank[next_ins_index];

    ins->voice_index = next_voice_index;
    ins->last_offset = 0;
    ins->polyphony = BANK_VOICES_PER_MONO_INSTRUMENT;

    next_ins_index += 1;
    next_voice_index += BANK_VOICES_PER_MONO_INSTRUMENT;
  }

  /* sound fx instruments */
  for (m = 0; m < BANK_NUM_SOUND_FX_INSTRUMENTS; m++)
  {
    ins = &G_instrument_bank[next_ins_index];

    ins->voice_index = next_voice_index;
    ins->last_offset = 0;
    ins->polyphony = BANK_VOICES_PER_SOUND_FX_INSTRUMENT;

    next_ins_index += 1;
    next_voice_index += BANK_VOICES_PER_SOUND_FX_INSTRUMENT;
  }

  return 0;
}

/*******************************************************************************
** instrument_load_patch()
*******************************************************************************/
short int instrument_load_patch(int instrument_index, 
                                int cart_index, int patch_index)
{
  int k;
  int m;

  instrument* ins;

  int voice_index;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure that the cart & patch indices are valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(cart_index))
    return 1;

  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* load patch to this instrument's midi controllers */
  sweep_load_patch(instrument_index, cart_index, patch_index);
  arpeggio_load_patch(instrument_index, cart_index, patch_index);

   /* load patch to all voices associated with this instrument */
  for (k = 0; k < BANK_NUM_INSTRUMENTS; k++)
  {
    ins = &G_instrument_bank[k];

    for (m = 0; m < ins->polyphony; m++)
    {
      voice_index = ins->voice_index + m;

      voice_load_patch(voice_index, cart_index, patch_index);
      envelope_load_patch(voice_index, cart_index, patch_index);
      lfo_load_patch(voice_index, cart_index, patch_index);
      filter_load_patch(voice_index, cart_index, patch_index);
      peg_load_patch(voice_index, cart_index, patch_index);
    }
  }

  return 0;
}

/*******************************************************************************
** instrument_note_on()
*******************************************************************************/
short int instrument_note_on(int instrument_index, int note)
{
  int m;

  instrument* ins;
  voice* v;

  short int next_offset;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the note is valid */
  if (TUNING_NOTE_IS_NOT_PLAYABLE(note))
    return 0;

  /* obtain instrument pointer */
  ins = &G_instrument_bank[instrument_index];

  /* initialize offset */
  next_offset = 0;

  /* if this is a monophonic instrument,    */
  /* just select the only voice available!  */
  if (ins->polyphony == 1)
  {
    next_offset = 0;
  }
  /* if the portamento is on, use the voice of the  */
  /* most recently pressed note for this note on    */
  else if (ins->port_arp_switch == MIDI_CONT_PORT_ARP_SWITCH_PORTAMENTO)
  {
    next_offset = ins->last_offset;
  }
  /* otherwise, select an available voice! */
  else
  {
    /* send a note-off to all voices playing this note */
    for (m = 0; m < ins->polyphony; m++)
    {
      v = &G_voice_bank[ins->voice_index + m];

      if (note == v->base_note)
      {
        envelope_note_off(ins->voice_index + m);
        peg_note_off(ins->voice_index + m);
      }
    }

    /* find the next available note (round robin) */
    next_offset = (ins->last_offset + 1) % ins->polyphony;
    ins->last_offset = next_offset;
  }

  /* send note-on to the selected voice associated with this instrument */
  voice_note_on(ins->voice_index + next_offset, note);

  envelope_note_on( ins->voice_index + next_offset, 
                    note, ins->note_velocity, ins->sustain_pedal);

  peg_note_on(ins->voice_index + next_offset);
  lfo_note_on(ins->voice_index + next_offset);

  return 0;
}

/*******************************************************************************
** instrument_note_off()
*******************************************************************************/
short int instrument_note_off(int instrument_index, int note)
{
  int m;

  instrument* ins;
  voice* v;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the note is valid */
  if (TUNING_NOTE_IS_NOT_PLAYABLE(note))
    return 0;

  /* obtain instrument pointer */
  ins = &G_instrument_bank[instrument_index];

  /* send a note-off to all voices playing this note */
  for (m = 0; m < ins->polyphony; m++)
  {
    v = &G_voice_bank[ins->voice_index + m];

    if (note == v->base_note)
    {
      envelope_note_off(ins->voice_index + m);
      peg_note_off(ins->voice_index + m);
    }
  }

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
  if (ins->port_arp_switch == MIDI_CONT_PORT_ARP_SWITCH_OFF)
    instrument_note_on(instrument_index, note);
  else if (ins->port_arp_switch == MIDI_CONT_PORT_ARP_SWITCH_PORTAMENTO)
    sweep_key_pressed(instrument_index, note);
  else if (ins->port_arp_switch == MIDI_CONT_PORT_ARP_SWITCH_ARPEGGIO)
    arpeggio_key_pressed(instrument_index, note);

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
  if (ins->sustain_pedal == MIDI_CONT_SUSTAIN_PEDAL_UP)
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

    if (ins->port_arp_switch == MIDI_CONT_PORT_ARP_SWITCH_OFF)
      instrument_note_off(instrument_index, note);
    else if (ins->port_arp_switch == MIDI_CONT_PORT_ARP_SWITCH_PORTAMENTO)
      sweep_key_released(instrument_index, note);
    else if (ins->port_arp_switch == MIDI_CONT_PORT_ARP_SWITCH_ARPEGGIO)
      arpeggio_key_released(instrument_index, note);
  }
  /* if the sustain pedal is down,  */
  /* add this key to the held keys. */
  else if (ins->sustain_pedal == MIDI_CONT_SUSTAIN_PEDAL_DOWN)
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
  int m;
  int n;

  instrument* ins;
  voice* v;
  envelope* e;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the mod wheel position is valid */
  if ((pos < MIDI_CONT_UNI_WHEEL_LOWER_BOUND) || 
      (pos > MIDI_CONT_UNI_WHEEL_UPPER_BOUND))
  {
    return 0;
  }

  /* obtain pointers */
  ins = &G_instrument_bank[instrument_index];

  /* if this new position is the same as the current position, return */
  if (pos == ins->mod_wheel_pos)
    return 0;

  /* set the new mod wheel position */
  ins->mod_wheel_pos = pos;

  /* set the mod wheel input for other units associated with this instrument */
  for (m = 0; m < ins->polyphony; m++)
  {
    v = &G_voice_bank[ins->voice_index + m];

    v->mod_wheel_pos = pos;

    for (n = 0; n < BANK_ENVELOPES_PER_VOICE; n++)
    {
      e = &G_envelope_bank[(ins->voice_index + m) * BANK_ENVELOPES_PER_VOICE + n];

      e->mod_wheel_pos = pos;
    }
  }

  return 0;
}

/*******************************************************************************
** instrument_set_aftertouch_position()
*******************************************************************************/
short int instrument_set_aftertouch_position(int instrument_index, short int pos)
{
  int m;
  int n;

  instrument* ins;
  voice* v;
  envelope* e;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the aftertouch position is valid */
  if ((pos < MIDI_CONT_UNI_WHEEL_LOWER_BOUND) || 
      (pos > MIDI_CONT_UNI_WHEEL_UPPER_BOUND))
  {
    return 0;
  }

  /* obtain pointers */
  ins = &G_instrument_bank[instrument_index];

  /* if this new position is the same as the current position, return */
  if (pos == ins->aftertouch_pos)
    return 0;

  /* set the new aftertouch position */
  ins->aftertouch_pos = pos;

  /* set the aftertouch input for other units associated with this instrument */
  for (m = 0; m < ins->polyphony; m++)
  {
    v = &G_voice_bank[ins->voice_index + m];

    v->aftertouch_pos = pos;

    for (n = 0; n < BANK_ENVELOPES_PER_VOICE; n++)
    {
      e = &G_envelope_bank[(ins->voice_index + m) * BANK_ENVELOPES_PER_VOICE + n];

      e->aftertouch_pos = pos;
    }
  }

  return 0;
}

/*******************************************************************************
** instrument_set_exp_pedal_position()
*******************************************************************************/
short int instrument_set_exp_pedal_position(int instrument_index, short int pos)
{
  int m;
  int n;

  instrument* ins;
  voice* v;
  envelope* e;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the exp pedal position is valid */
  if ((pos < MIDI_CONT_UNI_WHEEL_LOWER_BOUND) || 
      (pos > MIDI_CONT_UNI_WHEEL_UPPER_BOUND))
  {
    return 0;
  }

  /* obtain pointers */
  ins = &G_instrument_bank[instrument_index];

  /* if this new position is the same as the current position, return */
  if (pos == ins->exp_pedal_pos)
    return 0;

  /* set the new exp pedal position */
  ins->exp_pedal_pos = pos;

  /* set the exp pedal input for other units associated with this instrument */
  for (m = 0; m < ins->polyphony; m++)
  {
    v = &G_voice_bank[ins->voice_index + m];

    v->exp_pedal_pos = pos;

    for (n = 0; n < BANK_ENVELOPES_PER_VOICE; n++)
    {
      e = &G_envelope_bank[(ins->voice_index + m) * BANK_ENVELOPES_PER_VOICE + n];

      e->exp_pedal_pos = pos;
    }
  }

  return 0;
}

/*******************************************************************************
** instrument_set_pitch_wheel_position()
*******************************************************************************/
short int instrument_set_pitch_wheel_position(int instrument_index, short int pos)
{
  int m;

  instrument* ins;
  voice* v;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the pitch wheel position is valid */
  if ((pos < MIDI_CONT_BI_WHEEL_LOWER_BOUND) || 
      (pos > MIDI_CONT_BI_WHEEL_UPPER_BOUND))
  {
    return 0;
  }

  /* obtain pointers */
  ins = &G_instrument_bank[instrument_index];

  /* if this new position is the same as the current position, return */
  if (pos == ins->pitch_wheel_pos)
    return 0;

  /* set the new pitch wheel position */
  ins->pitch_wheel_pos = pos;

  /* set the pitch wheel input for other units associated with this instrument */
  for (m = 0; m < ins->polyphony; m++)
  {
    v = &G_voice_bank[ins->voice_index + m];

    v->pitch_wheel_pos = pos;
  }

  return 0;
}

/*******************************************************************************
** instrument_set_port_arp_switch()
*******************************************************************************/
short int instrument_set_port_arp_switch(int instrument_index, int state)
{
  instrument* ins;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure the switch state is valid */
  if ((state != MIDI_CONT_PORT_ARP_SWITCH_OFF)        && 
      (state != MIDI_CONT_PORT_ARP_SWITCH_PORTAMENTO) && 
      (state != MIDI_CONT_PORT_ARP_SWITCH_ARPEGGIO))
  {
    return 0;
  }

  /* obtain pointers */
  ins = &G_instrument_bank[instrument_index];

  /* if the switch is already set to this state, return */
  if (ins->port_arp_switch == state)
    return 0;

  /* set the portamento/arpeggio switch in the instrument */
  ins->port_arp_switch = state;

  /* set the switch input for this instrument's sweep & arpeggio units */
  if (ins->port_arp_switch == MIDI_CONT_PORT_ARP_SWITCH_PORTAMENTO)
  {
    arpeggio_set_switch(instrument_index, MIDI_CONT_TOGGLE_SWITCH_OFF);
    sweep_set_switch(instrument_index, MIDI_CONT_TOGGLE_SWITCH_ON);
  }
  else if (ins->port_arp_switch == MIDI_CONT_PORT_ARP_SWITCH_ARPEGGIO)
  {
    sweep_set_switch(instrument_index, MIDI_CONT_TOGGLE_SWITCH_OFF);
    arpeggio_set_switch(instrument_index, MIDI_CONT_TOGGLE_SWITCH_ON);
  }
  else
  {
    sweep_set_switch(instrument_index, MIDI_CONT_TOGGLE_SWITCH_OFF);
    arpeggio_set_switch(instrument_index, MIDI_CONT_TOGGLE_SWITCH_OFF);
  }

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
  if ((state != MIDI_CONT_SUSTAIN_PEDAL_UP) && 
      (state != MIDI_CONT_SUSTAIN_PEDAL_DOWN))
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

  /* if the sustain pedal is now up, release all held keys */
  if (ins->sustain_pedal == MIDI_CONT_SUSTAIN_PEDAL_UP)
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
      if (ins->port_arp_switch == MIDI_CONT_PORT_ARP_SWITCH_OFF)
        instrument_note_off(instrument_index, ins->held_keys[k]);
      else if (ins->port_arp_switch == MIDI_CONT_PORT_ARP_SWITCH_PORTAMENTO)
        sweep_key_released(instrument_index, ins->held_keys[k]);
      else if (ins->port_arp_switch == MIDI_CONT_PORT_ARP_SWITCH_ARPEGGIO)
        arpeggio_key_released(instrument_index, ins->held_keys[k]);

      ins->held_keys[k] = TUNING_NOTE_BLANK;
    }

    ins->num_held = 0;
  }

  return 0;
}

