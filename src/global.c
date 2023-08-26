/*******************************************************************************
** global.c (global variables)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "screen.h"

int G_game_screen;
int G_last_screen;

int G_flag_window_minimized;
int G_flag_quit_program;

unsigned int G_timer_count;
unsigned int G_saved_timer_count;

int G_current_scroll_amount;
int G_max_scroll_amount;

int G_common_edit_octave;
int G_common_edit_degree;

int G_patch_edit_patch_index;
int G_patch_edit_voice_index;

int G_pattern_edit_highlight_column;
int G_pattern_edit_highlight_step;

/*******************************************************************************
** globals_init_variables()
*******************************************************************************/
short int globals_init_variables()
{
  G_game_screen = PROGRAM_SCREEN_PATCHES;
  G_last_screen = PROGRAM_SCREEN_PATCHES;

  G_flag_window_minimized = 0;
  G_flag_quit_program = 0;

  G_timer_count = 0;
  G_saved_timer_count = 0;

  G_current_scroll_amount = 0;
  G_max_scroll_amount = 0;

  G_common_edit_octave = 4;
  G_common_edit_degree = 0;

  G_patch_edit_patch_index = 0;
  G_patch_edit_voice_index = 0;

  G_pattern_edit_highlight_column = 0;
  G_pattern_edit_highlight_step = 0;

  return 0;
}

