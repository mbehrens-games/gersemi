/*******************************************************************************
** loop.c (main program loop)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "controls.h"
#include "global.h"
#include "layout.h"
#include "loop.h"
#include "program.h"
#include "render.h"
#include "vball.h"

/*******************************************************************************
** loop_advance_frame()
*******************************************************************************/
short int loop_advance_frame()
{
  /* process user input */
  controls_process_user_input_standard();

  /* check for quit flag */
  if (G_program_flags & PROGRAM_FLAG_QUIT)
    return 0;

  /* update vbos */
  if (G_program_screen == PROGRAM_SCREEN_CART)
  {
    vb_all_load_top_bar();
    vb_all_load_cart_screen();
  }
  else if (G_program_screen == PROGRAM_SCREEN_INSTRUMENTS)
  {
    vb_all_load_top_bar();
    vb_all_load_instruments_screen();
  }
  else if (G_program_screen == PROGRAM_SCREEN_SONG)
  {
    vb_all_load_top_bar();
    vb_all_load_song_screen();
  }
  else if (G_program_screen == PROGRAM_SCREEN_MIXER)
  {
    vb_all_load_top_bar();
    vb_all_load_mixer_screen();
  }
  else if (G_program_screen == PROGRAM_SCREEN_SOUND_FX)
  {
    vb_all_load_top_bar();
    vb_all_load_sound_fx_screen();
  }
  else if (G_program_screen == PROGRAM_SCREEN_DPCM)
  {
    vb_all_load_top_bar();
    vb_all_load_dpcm_screen();
  }
  else if (G_program_screen == PROGRAM_SCREEN_TEXT_ENTRY_CART_NAME)
  {
    vb_all_load_top_bar();
    vb_all_load_cart_screen();
  }
  else if (G_program_screen == PROGRAM_SCREEN_TEXT_ENTRY_PATCH_NAME)
  {
    vb_all_load_top_bar();
    vb_all_load_cart_screen();
  }

  /* rendering */
  render_all();

#if 0
  /* update timer count */
  G_timer_count += 1;

  if (G_timer_count >= 240)
    G_timer_count = 0;
#endif

  return 0;
}

