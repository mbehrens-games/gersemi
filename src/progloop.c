/*******************************************************************************
** progloop.c (main prog loop functions)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "controls.h"
#include "global.h"
#include "layout.h"
#include "progloop.h"
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

  /* cart screen */
  if (screen == PROGRAM_SCREEN_CART)
  {
    G_timer_count = 0;

    G_current_scroll_amount = 0;
    G_max_scroll_amount = LAYOUT_CART_MAX_SCROLL_AMOUNT;

    render_reset_vbos();

    vb_all_load_background();
    vb_all_load_top_panel();
    vb_all_load_cart_screen();
  }
  /* instruments screen */
  else if (screen == PROGRAM_SCREEN_INSTRUMENTS)
  {
    G_timer_count = 0;

    G_current_scroll_amount = 0;
    G_max_scroll_amount = LAYOUT_INSTRUMENTS_MAX_SCROLL_AMOUNT;

    render_reset_vbos();

    vb_all_load_background();
    vb_all_load_top_panel();
    vb_all_load_instruments_screen();
  }
  /* song screen */
  else if (screen == PROGRAM_SCREEN_SONG)
  {
    G_timer_count = 0;

    G_current_scroll_amount = 0;
    G_max_scroll_amount = LAYOUT_SONG_MAX_SCROLL_AMOUNT;

    render_reset_vbos();

    vb_all_load_background();
    vb_all_load_top_panel();
    vb_all_load_song_screen();
  }
  /* mixer screen */
  else if (screen == PROGRAM_SCREEN_MIXER)
  {
    G_timer_count = 0;

    G_current_scroll_amount = 0;
    G_max_scroll_amount = LAYOUT_MIXER_MAX_SCROLL_AMOUNT;

    render_reset_vbos();

    vb_all_load_background();
    vb_all_load_top_panel();
    vb_all_load_mixer_screen();
  }
  /* sound fx screen */
  else if (screen == PROGRAM_SCREEN_SOUND_FX)
  {
    G_timer_count = 0;

    G_current_scroll_amount = 0;
    G_max_scroll_amount = LAYOUT_SOUND_FX_MAX_SCROLL_AMOUNT;

    render_reset_vbos();

    vb_all_load_background();
    vb_all_load_top_panel();
    vb_all_load_sound_fx_screen();
  }
  /* reverb screen */
  else if (screen == PROGRAM_SCREEN_REVERB)
  {
    G_timer_count = 0;

    G_current_scroll_amount = 0;
    G_max_scroll_amount = LAYOUT_REVERB_MAX_SCROLL_AMOUNT;

    render_reset_vbos();

    vb_all_load_background();
    vb_all_load_top_panel();
    vb_all_load_reverb_screen();
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
  if (G_game_screen == PROGRAM_SCREEN_CART)
  {
    vb_all_load_top_panel();
    vb_all_load_cart_screen();
  }
  else if (G_game_screen == PROGRAM_SCREEN_INSTRUMENTS)
  {
    vb_all_load_top_panel();
    vb_all_load_instruments_screen();
  }
  else if (G_game_screen == PROGRAM_SCREEN_SONG)
  {
    vb_all_load_top_panel();
    vb_all_load_song_screen();
  }
  else if (G_game_screen == PROGRAM_SCREEN_MIXER)
  {
    vb_all_load_top_panel();
    vb_all_load_mixer_screen();
  }
  else if (G_game_screen == PROGRAM_SCREEN_SOUND_FX)
  {
    vb_all_load_top_panel();
    vb_all_load_sound_fx_screen();
  }
  else if (G_game_screen == PROGRAM_SCREEN_REVERB)
  {
    vb_all_load_top_panel();
    vb_all_load_reverb_screen();
  }

  /* rendering */
  render_all();

  /* update timer count */
  G_timer_count += 1;

  if (G_timer_count >= 240)
    G_timer_count = 0;

  return 0;
}

