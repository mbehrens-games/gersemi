/*******************************************************************************
** controls.h (keyboard / mouse input)
*******************************************************************************/

#ifndef CONTROLS_H
#define CONTROLS_H

#include <SDL2/SDL.h>

/* function declarations */
short int controls_keyboard_key_pressed(SDL_Scancode code);
short int controls_mouse_button_pressed(Uint8 button, Sint32 x, Sint32 y);

#endif
