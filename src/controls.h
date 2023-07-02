/*******************************************************************************
** controls.h (keyboard / mouse input)
*******************************************************************************/

#ifndef CONTROLS_H
#define CONTROLS_H

#include <SDL2/SDL.h>

/* function declarations */
short int controls_setup();

short int controls_keyboard_key_pressed(SDL_Scancode code);
short int controls_keyboard_key_released(SDL_Scancode code);

short int controls_mouse_button_pressed(Uint8 button, Sint32 x, Sint32 y);
short int controls_mouse_button_released(Uint8 button);

short int controls_mouse_cursor_moved(Sint32 x, Sint32 y);
short int controls_mouse_wheel_moved(Sint32 movement);

short int controls_process_user_input_standard();

#endif
