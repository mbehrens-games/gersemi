/*******************************************************************************
** global.c (global variables)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "midicont.h"
#include "patch.h"

int G_current_scroll_amount;
int G_max_scroll_amount;

short int G_patch_edit_cart_number;
short int G_patch_edit_patch_number;

short int G_patch_edit_instrument_index;

short int G_patch_edit_octave;
short int G_patch_edit_note_velocity;
short int G_patch_edit_pitch_wheel_pos;

short int G_patch_edit_mod_wheel_pos;
short int G_patch_edit_aftertouch_pos;
short int G_patch_edit_exp_pedal_pos;

/*******************************************************************************
** globals_init_variables()
*******************************************************************************/
short int globals_init_variables()
{
  /* initialize variables */
  G_current_scroll_amount = 0;
  G_max_scroll_amount = 0;

  G_patch_edit_cart_number = PATCH_CART_NUMBER_DEFAULT;
  G_patch_edit_patch_number = PATCH_PATCH_NUMBER_DEFAULT;

  G_patch_edit_instrument_index = 0;

  G_patch_edit_octave = MIDI_CONT_OCTAVE_DEFAULT;
  G_patch_edit_note_velocity = MIDI_CONT_NOTE_VELOCITY_DEFAULT;
  G_patch_edit_pitch_wheel_pos = MIDI_CONT_BI_WHEEL_DEFAULT;

  G_patch_edit_mod_wheel_pos = MIDI_CONT_UNI_WHEEL_DEFAULT;
  G_patch_edit_aftertouch_pos = MIDI_CONT_UNI_WHEEL_DEFAULT;
  G_patch_edit_exp_pedal_pos = MIDI_CONT_UNI_WHEEL_DEFAULT;

  return 0;
}

