/*******************************************************************************
** global.c (global variables)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "screen.h"
#include "tuning.h"
#include "wheel.h"

int G_game_screen;
int G_last_screen;

int G_flag_window_minimized;
int G_flag_quit_program;

unsigned int G_timer_count;
unsigned int G_saved_timer_count;

int G_current_scroll_amount;
int G_max_scroll_amount;

int G_patch_edit_patch_index;
int G_patch_edit_instrument_index;

int G_patch_edit_octave;
int G_patch_edit_mod_wheel_amount;
int G_patch_edit_aftertouch_amount;
int G_patch_edit_pitch_wheel_amount;

/*******************************************************************************
** globals_init_variables()
*******************************************************************************/
short int globals_init_variables()
{
  /* initialize variables */
  G_game_screen = PROGRAM_SCREEN_PATCHES;
  G_last_screen = PROGRAM_SCREEN_PATCHES;

  G_flag_window_minimized = 0;
  G_flag_quit_program = 0;

  G_timer_count = 0;
  G_saved_timer_count = 0;

  G_current_scroll_amount = 0;
  G_max_scroll_amount = 0;

  G_patch_edit_patch_index = 0;
  G_patch_edit_instrument_index = 0;

  G_patch_edit_octave = TUNING_MIDDLE_OCTAVE;

  G_patch_edit_mod_wheel_amount = WHEEL_MOD_WHEEL_LOWER_BOUND;
  G_patch_edit_aftertouch_amount = WHEEL_AFTERTOUCH_LOWER_BOUND;
  G_patch_edit_pitch_wheel_amount = WHEEL_PITCH_WHEEL_CENTER_VALUE;

  /* generate non-synth (gui related) tables */
  /*key_generate_tables();*/

  return 0;
}

