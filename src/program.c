/*******************************************************************************
** program.c (program state functions)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "layout.h"
#include "program.h"
#include "render.h"
#include "vball.h"

int G_program_screen;

int G_program_flags;

/*******************************************************************************
** program_reset()
*******************************************************************************/
short int program_reset()
{
  G_program_screen = PROGRAM_SCREEN_CART;

  G_program_flags = PROGRAM_FLAGS_CLEAR;

  return 0;
}

/*******************************************************************************
** program_set_screen()
*******************************************************************************/
short int program_set_screen(int screen)
{
  /* make sure screen is valid */
  if ((screen < 0) || (screen >= PROGRAM_NUM_SCREENS))
    return 0;

#if 0
  /* if we are already on this screen, return */
  if (screen == G_program_screen)
    return 0;
#endif

  /* set the screen */
  G_program_screen = screen;

  /* cart screen */
  if (screen == PROGRAM_SCREEN_CART)
  {
    G_current_scroll_amount = 0;
    G_max_scroll_amount = LAYOUT_CART_MAX_SCROLL_AMOUNT;

    layout_reset_cart_states();

    render_reset_vbos();

    vb_all_load_background();
    vb_all_load_top_bar();
    vb_all_load_cart_screen();
  }
  /* instruments screen */
  else if (screen == PROGRAM_SCREEN_INSTRUMENTS)
  {
    G_current_scroll_amount = 0;
    G_max_scroll_amount = LAYOUT_INSTRUMENTS_MAX_SCROLL_AMOUNT;

    render_reset_vbos();

    vb_all_load_background();
    vb_all_load_top_bar();
    vb_all_load_instruments_screen();
  }
  /* song screen */
  else if (screen == PROGRAM_SCREEN_SONG)
  {
    G_current_scroll_amount = 0;
    G_max_scroll_amount = LAYOUT_SONG_MAX_SCROLL_AMOUNT;

    render_reset_vbos();

    vb_all_load_background();
    vb_all_load_top_bar();
    vb_all_load_song_screen();
  }
  /* mixer screen */
  else if (screen == PROGRAM_SCREEN_MIXER)
  {
    G_current_scroll_amount = 0;
    G_max_scroll_amount = LAYOUT_MIXER_MAX_SCROLL_AMOUNT;

    render_reset_vbos();

    vb_all_load_background();
    vb_all_load_top_bar();
    vb_all_load_mixer_screen();
  }
  /* sound fx screen */
  else if (screen == PROGRAM_SCREEN_SOUND_FX)
  {
    G_current_scroll_amount = 0;
    G_max_scroll_amount = LAYOUT_SOUND_FX_MAX_SCROLL_AMOUNT;

    render_reset_vbos();

    vb_all_load_background();
    vb_all_load_top_bar();
    vb_all_load_sound_fx_screen();
  }
  /* dpcm screen */
  else if (screen == PROGRAM_SCREEN_DPCM)
  {
    G_current_scroll_amount = 0;
    G_max_scroll_amount = LAYOUT_DPCM_MAX_SCROLL_AMOUNT;

    render_reset_vbos();

    vb_all_load_background();
    vb_all_load_top_bar();
    vb_all_load_dpcm_screen();
  }
  /* text entry: cart name (on cart screen) */
  else if (screen == PROGRAM_SCREEN_TEXT_ENTRY_CART_NAME)
  {
    render_reset_vbos();

    vb_all_load_background();
    vb_all_load_top_bar();
    vb_all_load_cart_screen();
  }
  /* text entry: patch name (on cart screen) */
  else if (screen == PROGRAM_SCREEN_TEXT_ENTRY_PATCH_NAME)
  {
    render_reset_vbos();

    vb_all_load_background();
    vb_all_load_top_bar();
    vb_all_load_cart_screen();
  }

  return 0;
}

