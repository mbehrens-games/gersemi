/*******************************************************************************
** global.c (global variables)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "screen.h"

int           G_game_screen;
int           G_last_screen;

int           G_flag_window_minimized;
int           G_flag_quit_program;

unsigned int  G_timer_count;
unsigned int  G_saved_timer_count;

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

  return 0;
}

