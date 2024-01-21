/*******************************************************************************
** global.c (global variables)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "bank.h"
#include "global.h"
#include "midicont.h"
#include "patch.h"
#include "screen.h"
#include "tuning.h"

int G_game_screen;
int G_last_screen;

int G_flag_window_minimized;
int G_flag_quit_program;

unsigned int G_timer_count;
unsigned int G_saved_timer_count;

int G_current_scroll_amount;
int G_max_scroll_amount;

int G_patch_edit_cart_number;
int G_patch_edit_patch_number;

int G_patch_edit_instrument_index;

int G_patch_edit_octave;

int G_patch_edit_note_velocity;
int G_patch_edit_mod_wheel_pos;
int G_patch_edit_aftertouch_pos;
int G_patch_edit_pitch_wheel_pos;

/*******************************************************************************
** globals_init_variables()
*******************************************************************************/
short int globals_init_variables()
{
  /* initialize variables */
  G_game_screen = PROGRAM_SCREEN_CART;
  G_last_screen = PROGRAM_SCREEN_CART;

  G_flag_window_minimized = 0;
  G_flag_quit_program = 0;

  G_timer_count = 0;
  G_saved_timer_count = 0;

  G_current_scroll_amount = 0;
  G_max_scroll_amount = 0;

  G_patch_edit_cart_number = PATCH_CART_NO_DEFAULT;
  G_patch_edit_patch_number = PATCH_PATCH_NO_DEFAULT;

  G_patch_edit_instrument_index = 0;

  G_patch_edit_octave = TUNING_MIDDLE_OCTAVE;

  G_patch_edit_note_velocity = MIDI_CONT_NOTE_VELOCITY_DEFAULT;
  G_patch_edit_mod_wheel_pos = MIDI_CONT_MOD_WHEEL_DEFAULT;
  G_patch_edit_aftertouch_pos = MIDI_CONT_AFTERTOUCH_DEFAULT;
  G_patch_edit_pitch_wheel_pos = MIDI_CONT_PITCH_WHEEL_DEFAULT;

  /* generate non-synth (gui related) tables */
  /*key_generate_tables();*/

  return 0;
}

