/*******************************************************************************
** global.c (global variables)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "key.h"
#include "screen.h"

int G_game_screen;
int G_last_screen;

int G_flag_window_minimized;
int G_flag_quit_program;

unsigned int G_timer_count;
unsigned int G_saved_timer_count;

int G_current_scroll_amount;
int G_max_scroll_amount;

int G_patch_edit_octave;
int G_patch_edit_signature;
int G_patch_edit_mode;

int G_patch_edit_degree;

int G_patch_edit_patch_index;
int G_patch_edit_voice_index;

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

  G_patch_edit_octave = KEY_MIDDLE_OCTAVE;
  G_patch_edit_signature = KEY_SIG_CLEAR;
  G_patch_edit_mode = KEY_MODE_MAJOR;

  G_patch_edit_degree = 0;

  G_patch_edit_patch_index = 0;
  G_patch_edit_voice_index = 0;

  /* generate non-synth (gui related) tables */
  key_generate_tables();

  return 0;
}

