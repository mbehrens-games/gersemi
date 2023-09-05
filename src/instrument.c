/*******************************************************************************
** instrument.c (synth instrument)
*******************************************************************************/

#include <stdlib.h>
#include <math.h>

#include "bank.h"
#include "envelope.h"
#include "filter.h"
#include "instrument.h"
#include "lfo.h"
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

  /* type */
  ins->type = INSTRUMENT_TYPE_INACTIVE;

  /* voice index */
  ins->voice_index = -1;

  /* notes */
  for (k = 0; k < INSTRUMENT_NUM_PRESSED_NOTES; k++)
    ins->pressed_notes[k] = 0;

  for (k = 0; k < INSTRUMENT_NUM_RELEASED_NOTES; k++)
    ins->released_notes[k] = 0;

  /* volume & panning */
  ins->volume = 0;
  ins->panning = 0;

  /* portamento */
  ins->porta_switch = 0;
  ins->porta_speed = 0;

  /* mod wheel & aftertouch */
  ins->mod_wheel_amount = 0;
  ins->aftertouch_amount = 0;

  return 0;
}

/*******************************************************************************
** instrument_set_layout()
*******************************************************************************/
short int instrument_set_layout(int layout)
{
  int k;

  instrument* ins;

  if (layout == INSTRUMENT_LAYOUT_1_POLY_8_MONO)
  {
    /* 1 polyphonic instrument */
    ins = &G_instrument_bank[0];

    ins->type = INSTRUMENT_TYPE_POLY;
    ins->voice_index = BANK_INST_VOICES_START_INDEX;

    /* 8 monophonic instruments */
    for (k = 0; k < 8; k++)
    {
      ins = &G_instrument_bank[1 + k];

      ins->type = INSTRUMENT_TYPE_MONO;
      ins->voice_index = BANK_INST_VOICES_START_INDEX + 4 + k;
    }

    /* drums */
    ins = &G_instrument_bank[9];

    ins->type = INSTRUMENT_TYPE_DRUMS;
    ins->voice_index = BANK_DRUM_VOICES_START_INDEX;
  }
  else if (layout == INSTRUMENT_LAYOUT_2_POLY_4_MONO)
  {
    /* 2 polyphonic instruments */
    for (k = 0; k < 2; k++)
    {
      ins = &G_instrument_bank[0 + k];

      ins->type = INSTRUMENT_TYPE_POLY;
      ins->voice_index = BANK_INST_VOICES_START_INDEX + 4 * k;
    }

    /* 4 monophonic instruments */
    for (k = 0; k < 4; k++)
    {
      ins = &G_instrument_bank[2 + k];

      ins->type = INSTRUMENT_TYPE_MONO;
      ins->voice_index = BANK_INST_VOICES_START_INDEX + 8 + k;
    }

    /* 3 inactive instruments */
    for (k = 0; k < 3; k++)
    {
      ins = &G_instrument_bank[6 + k];

      ins->type = INSTRUMENT_TYPE_INACTIVE;
      ins->voice_index = -1;
    }

    /* drums */
    ins = &G_instrument_bank[9];

    ins->type = INSTRUMENT_TYPE_DRUMS;
    ins->voice_index = BANK_DRUM_VOICES_START_INDEX;
  }
  else
  {
    /* 1 polyphonic instrument */
    ins = &G_instrument_bank[0];

    ins->type = INSTRUMENT_TYPE_POLY;
    ins->voice_index = BANK_INST_VOICES_START_INDEX;

    /* 8 monophonic instruments */
    for (k = 0; k < 8; k++)
    {
      ins = &G_instrument_bank[1 + k];

      ins->type = INSTRUMENT_TYPE_MONO;
      ins->voice_index = BANK_INST_VOICES_START_INDEX + 4 + k;
    }

    /* drums */
    ins = &G_instrument_bank[9];

    ins->type = INSTRUMENT_TYPE_DRUMS;
    ins->voice_index = BANK_DRUM_VOICES_START_INDEX;
  }

  return 0;
}

/*******************************************************************************
** instrument_load_patch()
*******************************************************************************/
short int instrument_load_patch(int instrument_index, int patch_index)
{
  int k;

  instrument* ins;

  int voice_index;
  int bound;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain instrument pointer */
  ins = &G_instrument_bank[instrument_index];

  /* if instrument is inactive, return */
  if (ins->type == INSTRUMENT_TYPE_INACTIVE)
    return 0;

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

    envelope_load_patch(voice_index + k, 0, patch_index);
    envelope_load_patch(voice_index + k, 1, patch_index);
    envelope_load_patch(voice_index + k, 2, patch_index);
    envelope_load_patch(voice_index + k, 3, patch_index);

    lfo_load_patch(voice_index + k, patch_index);

    sweep_load_patch(voice_index + k, patch_index);

    filter_load_patch(voice_index + k, patch_index);
  }

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

  /* if instrument is inactive, return */
  if (ins->type == INSTRUMENT_TYPE_INACTIVE)
    return 0;

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
    /* if a note is being shifted out of the pressed notes, */
    /* add it on to the front of the released notes.        */
    if (ins->pressed_notes[INSTRUMENT_NUM_PRESSED_NOTES - 1] != 0)
    {
      for (m = INSTRUMENT_NUM_RELEASED_NOTES - 1; m > 0; m--)
        ins->released_notes[m] = ins->released_notes[m - 1];

      ins->released_notes[0] = ins->pressed_notes[INSTRUMENT_NUM_PRESSED_NOTES - 1];
    }

    /* add the new pressed note */
    for (m = INSTRUMENT_NUM_PRESSED_NOTES - 1; m > 0; m--)
      ins->pressed_notes[m] = ins->pressed_notes[m - 1];

    ins->pressed_notes[0] = note;
  }

  /* make sure that this note does not also appear in the released notes list */
  for (k = 0; k < INSTRUMENT_NUM_RELEASED_NOTES; k++)
  {
    if (ins->released_notes[k] == note)
    {
      for (m = k; m < INSTRUMENT_NUM_RELEASED_NOTES - 1; m++)
        ins->released_notes[m] = ins->released_notes[m + 1];

      ins->released_notes[INSTRUMENT_NUM_RELEASED_NOTES - 1] = 0;

      break;
    }
  }

  /* determine selected voice for this key on */
  base_voice_index = ins->voice_index;
  selected_voice_index = -1;

  if (ins->type == INSTRUMENT_TYPE_POLY)
  {
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

        if (v->base_note == 0)
        {
          selected_voice_index = base_voice_index + k;
          break;
        }
      }
    }

    /* if a voice is still playing a released */
    /* note, replace it with the new note.    */
    if (selected_voice_index == -1)
    {
      for (m = INSTRUMENT_NUM_RELEASED_NOTES - 1; m >= 0; m--)
      {
        if (selected_voice_index != -1)
          break;

        for (k = 0; k < 4; k++)
        {
          v = &G_voice_bank[base_voice_index + k];

          if (v->base_note == ins->released_notes[m])
          {
            selected_voice_index = base_voice_index + k;
            break;
          }
        }
      }
    }
  }
  else if (ins->type == INSTRUMENT_TYPE_MONO)
  {
    selected_voice_index = base_voice_index;
  }
  else
    return 0;

  /* if no voice index found, return */
  if (selected_voice_index == -1)
    return 0;

  printf("Key-On: Note %d on Voice %d...\n", note, selected_voice_index);

  /* send key on to the selected voice associated with this instrument */
  v = &G_voice_bank[selected_voice_index];

  voice_set_note(selected_voice_index, note);

  envelope_trigger(selected_voice_index, 0, v->osc_note[0]);
  envelope_trigger(selected_voice_index, 1, v->osc_note[1]);
  envelope_trigger(selected_voice_index, 2, v->osc_note[2]);
  envelope_trigger(selected_voice_index, 3, v->osc_note[3]);

  lfo_trigger(selected_voice_index);

  sweep_trigger(selected_voice_index, note);

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

  /* if instrument is inactive, return */
  if (ins->type == INSTRUMENT_TYPE_INACTIVE)
    return 0;

  /* make sure the note is valid */
  if ((note < TUNING_NOTE_A0) || (note > TUNING_NOTE_C8))
    return 0;

  /* check if this note was recently released. if it   */
  /* was, move it to the front of the released notes.  */
  for (k = 0; k < INSTRUMENT_NUM_RELEASED_NOTES; k++)
  {
    if (ins->released_notes[k] == note)
    {
      for (m = k; m > 0; m--)
        ins->released_notes[m] = ins->released_notes[m - 1];

      ins->released_notes[0] = note;

      break;
    }
  }

  /* otherwise, add this note to the front and  */
  /* shift the current notes back in the list.  */
  if (ins->released_notes[0] != note)
  {
    /* add the new released note */
    for (m = INSTRUMENT_NUM_RELEASED_NOTES - 1; m > 0; m--)
      ins->released_notes[m] = ins->released_notes[m - 1];

    ins->released_notes[0] = note;
  }

  /* make sure that this note does not also appear in the pressed notes list */
  for (k = 0; k < INSTRUMENT_NUM_PRESSED_NOTES; k++)
  {
    if (ins->pressed_notes[k] == note)
    {
      for (m = k; m < INSTRUMENT_NUM_PRESSED_NOTES - 1; m++)
        ins->pressed_notes[m] = ins->pressed_notes[m + 1];

      ins->pressed_notes[INSTRUMENT_NUM_PRESSED_NOTES - 1] = 0;

      break;
    }
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

  printf("Key-Off: Note %d on Voice %d...\n", note, selected_voice_index);

  /* send key off to the selected voice associated with this instrument */
  envelope_release(selected_voice_index, 0);
  envelope_release(selected_voice_index, 1);
  envelope_release(selected_voice_index, 2);
  envelope_release(selected_voice_index, 3);

  return 0;
}

/*******************************************************************************
** instrument_update_all()
*******************************************************************************/
short int instrument_update_all()
{

  return 0;
}

