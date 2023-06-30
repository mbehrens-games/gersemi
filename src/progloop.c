/*******************************************************************************
** progloop.c (main prog loop functions)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "controls.h"
#include "progloop.h"
#include "global.h"
#include "render.h"
#include "screen.h"
#include "vball.h"

/*******************************************************************************
** program_loop_change_screen()
*******************************************************************************/
short int program_loop_change_screen(int screen)
{
  /* make sure screen is valid */
  if ((screen < 0) || (screen >= PROGRAM_NUM_SCREENS))
    return 0;

  /* update screen if necessary */
  if (screen != G_game_screen)
  {
    G_last_screen = G_game_screen;
    G_game_screen = screen;
  }

  /* patches screen */
  if (screen == PROGRAM_SCREEN_PATCHES)
  {
    G_timer_count = 0;

    render_reset_vbos();

    vb_all_load_background();
    vb_all_load_common_panels_and_buttons();
    vb_all_load_patches_overlay();
  }
  /* patterns screen */
  else if (screen == PROGRAM_SCREEN_PATTERNS)
  {
    G_timer_count = 0;

    render_reset_vbos();

    vb_all_load_background();
    vb_all_load_common_panels_and_buttons();
    vb_all_load_patterns_overlay();
  }
  /* quit command issued */
  else if (screen == PROGRAM_SCREEN_QUIT)
  {
    G_flag_quit_program = 1;
  }

  return 0;
}

/*******************************************************************************
** program_loop_advance_frame()
*******************************************************************************/
short int program_loop_advance_frame()
{
  /* process user input */
  controls_process_user_input_standard();

  /* check for quit flag */
  if (G_flag_quit_program == 1)
    return 0;

  /* update vbos */
  if (G_game_screen == PROGRAM_SCREEN_PATCHES)
  {
    vb_all_load_common_overlay();
    vb_all_load_patches_overlay();
  }
  else if (G_game_screen == PROGRAM_SCREEN_PATTERNS)
  {
    vb_all_load_common_overlay();
    vb_all_load_patterns_overlay();
  }

  /* rendering */
  render_all();

  /* update timer count */
  G_timer_count += 1;

  if (G_timer_count >= 240)
    G_timer_count = 0;

  return 0;
}

