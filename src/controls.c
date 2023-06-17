/*******************************************************************************
** controls.c (keyboard / mouse input)
*******************************************************************************/

#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdlib.h>

#include "controls.h"
#include "global.h"
#include "graphics.h"
#include "progloop.h"
#include "screen.h"
#include "synth.h"

#define CONTROLS_MOUSE_CURSOR_IS_OVER_BUTTON(offset_x, offset_y, width)             \
  ( (remapped_x >= (GRAPHICS_OVERSCAN_WIDTH - 16 * width) / 2 + (4 * offset_x)) &&  \
    (remapped_x <  (GRAPHICS_OVERSCAN_WIDTH + 16 * width) / 2 + (4 * offset_x)) &&  \
    (remapped_y >= (GRAPHICS_OVERSCAN_HEIGHT - 16 * 1) / 2 + (4 * offset_y))    &&  \
    (remapped_y <  (GRAPHICS_OVERSCAN_HEIGHT + 16 * 1) / 2 + (4 * offset_y)))

/*******************************************************************************
** controls_keyboard_key_pressed()
*******************************************************************************/
short int controls_keyboard_key_pressed(SDL_Scancode code)
{
  /* left square bracket */
  if (code == SDL_SCANCODE_LEFTBRACKET)
  {
    graphics_decrease_window_size();
  }

  /* right square bracket */
  if (code == SDL_SCANCODE_RIGHTBRACKET)
  {
    graphics_increase_window_size();
  }

  /* patches screen */
  if (G_game_screen == PROGRAM_SCREEN_PATCHES)
  {
    if (code == SDL_SCANCODE_Z)
      synth_key_on(0, 2, 0, 8, 8);
    else if (code == SDL_SCANCODE_X)
      synth_key_on(0, 2, 1, 8, 8);
    else if (code == SDL_SCANCODE_C)
      synth_key_on(0, 2, 2, 8, 8);
    else if (code == SDL_SCANCODE_V)
      synth_key_on(0, 2, 3, 8, 8);
    else if (code == SDL_SCANCODE_B)
      synth_key_on(0, 2, 4, 8, 8);
    else if (code == SDL_SCANCODE_N)
      synth_key_on(0, 2, 5, 8, 8);
    else if (code == SDL_SCANCODE_M)
      synth_key_on(0, 2, 6, 8, 8);
    else if (code == SDL_SCANCODE_COMMA)
      synth_key_on(0, 2, 7, 8, 8);
  }

  return 0;
}

/*******************************************************************************
** controls_mouse_button_pressed()
*******************************************************************************/
short int controls_mouse_button_pressed(Uint8 button, Sint32 x, Sint32 y)
{
  int remapped_x;
  int remapped_y;

  /* find the mouse button location in overscan coordinates */
  remapped_x = (x * GRAPHICS_OVERSCAN_WIDTH) / G_viewport_w;
  remapped_y = (y * GRAPHICS_OVERSCAN_HEIGHT) / G_viewport_h;

  /* patches screen */
  if (G_game_screen == PROGRAM_SCREEN_PATCHES)
  {
    if (button == SDL_BUTTON_LEFT)
    {
      if (CONTROLS_MOUSE_CURSOR_IS_OVER_BUTTON(-12, -22, 5))
        program_loop_change_screen(PROGRAM_SCREEN_PATTERNS);
    }
  }
  /* patterns screen */
  else if (G_game_screen == PROGRAM_SCREEN_PATTERNS)
  {
    if (button == SDL_BUTTON_LEFT)
    {
      if (CONTROLS_MOUSE_CURSOR_IS_OVER_BUTTON(-30, -22, 4))
        program_loop_change_screen(PROGRAM_SCREEN_PATCHES);
    }
  }

  return 0;
}

