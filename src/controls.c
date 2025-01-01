/*******************************************************************************
** controls.c (keyboard / mouse input)
*******************************************************************************/

#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdlib.h>

#include "bank.h"
#include "cart.h"
#include "controls.h"
#include "global.h"
#include "graphics.h"
#include "instrument.h"
#include "layout.h"
#include "midicont.h"
#include "program.h"
#include "synth.h"
#include "tuning.h"

enum
{
  /* common keys */
  CONTROLS_KEY_INDEX_COMMON_ESCAPE = 0, 
  CONTROLS_KEY_INDEX_COMMON_DECREASE_WINDOW_SIZE, 
  CONTROLS_KEY_INDEX_COMMON_INCREASE_WINDOW_SIZE, 
  CONTROLS_KEY_INDEX_COMMON_SCROLL_UP, 
  CONTROLS_KEY_INDEX_COMMON_SCROLL_DOWN, 
  /* musical keyboard keys */
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_ZERO_C, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_ZERO_C_SHARP, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_ZERO_D, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_ZERO_D_SHARP, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_ZERO_E, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_ZERO_F, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_ZERO_F_SHARP, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_ZERO_G, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_ZERO_G_SHARP, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_ZERO_A, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_ZERO_A_SHARP, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_ZERO_B, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_ONE_C, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_ONE_C_SHARP, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_ONE_D, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_ONE_D_SHARP, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_ONE_E, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_ONE_F, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_ONE_F_SHARP, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_ONE_G, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_ONE_G_SHARP, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_ONE_A, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_ONE_A_SHARP, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_ONE_B, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_TWO_C, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_TWO_C_SHARP, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_TWO_D, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_TWO_D_SHARP, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_TWO_E, 
  /* text entry keys */
  CONTROLS_KEY_INDEX_TEXT_ENTRY_A, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_B, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_C, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_D, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_E, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_F, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_G, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_H, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_I, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_J, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_K, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_L, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_M, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_N, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_O, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_P, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_Q, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_R, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_S, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_T, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_U, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_V, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_W, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_X, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_Y, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_Z, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_0, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_1, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_2, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_3, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_4, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_5, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_6, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_7, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_8, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_9, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_SPACE, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_PERIOD, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_HYPHEN, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_ENTER, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_BACKSPACE, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_LEFT_SHIFT, 
  CONTROLS_KEY_INDEX_TEXT_ENTRY_RIGHT_SHIFT, 
  CONTROLS_NUM_KEY_INDICES 
};

enum
{
  CONTROLS_KEY_STATE_OFF = 0,
  CONTROLS_KEY_STATE_PRESSED,
  CONTROLS_KEY_STATE_ON,
  CONTROLS_KEY_STATE_RELEASED
};

enum
{
  CONTROLS_MOUSE_BUTTON_INDEX_LEFT = 0, 
  CONTROLS_MOUSE_BUTTON_INDEX_RIGHT, 
  CONTROLS_NUM_MOUSE_BUTTON_INDICES
};

enum
{
  CONTROLS_MOUSE_BUTTON_STATE_OFF = 0,
  CONTROLS_MOUSE_BUTTON_STATE_PRESSED,
  CONTROLS_MOUSE_BUTTON_STATE_ON,
  CONTROLS_MOUSE_BUTTON_STATE_RELEASED
};

/* keyboard key macros */
#define CONTROLS_KEY_IS_PRESSED(index)                                         \
  (S_key_states[index] == CONTROLS_KEY_STATE_PRESSED)

#define CONTROLS_KEY_IS_RELEASED(index)                                        \
  (S_key_states[index] == CONTROLS_KEY_STATE_RELEASED)

#define CONTROLS_KEY_IS_ON_OR_PRESSED(index)                                   \
  ( (S_key_states[index] == CONTROLS_KEY_STATE_ON) ||                          \
    (S_key_states[index] == CONTROLS_KEY_STATE_PRESSED))

#define CONTROLS_KEY_IS_OFF_OR_RELEASED(index)                                 \
  ( (S_key_states[index] == CONTROLS_KEY_STATE_OFF) ||                         \
    (S_key_states[index] == CONTROLS_KEY_STATE_RELEASED))

#define CONTROLS_KEY_IS_PRESSED_BY_NAME(name)                                  \
  CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_##name)

#define CONTROLS_KEY_IS_RELEASED_BY_NAME(name)                                 \
  CONTROLS_KEY_IS_RELEASED(CONTROLS_KEY_INDEX_##name)

#define CONTROLS_KEY_IS_ON_OR_PRESSED_BY_NAME(name)                            \
  CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_##name)

#define CONTROLS_KEY_IS_OFF_OR_RELEASED_BY_NAME(name)                          \
  CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_##name)

/* mouse button macros */
#define CONTROLS_MOUSE_BUTTON_IS_PRESSED(index)                                \
  (S_mouse_button_states[index] == CONTROLS_MOUSE_BUTTON_STATE_PRESSED)

#define CONTROLS_MOUSE_BUTTON_IS_RELEASED(index)                               \
  (S_mouse_button_states[index] == CONTROLS_MOUSE_BUTTON_STATE_RELEASED)

#define CONTROLS_MOUSE_BUTTON_IS_ON_OR_PRESSED(index)                          \
  ( (S_mouse_button_states[index] == CONTROLS_MOUSE_BUTTON_STATE_ON) ||        \
    (S_mouse_button_states[index] == CONTROLS_MOUSE_BUTTON_STATE_PRESSED))

#define CONTROLS_MOUSE_BUTTON_IS_OFF_OR_RELEASED(index)                        \
  ( (S_mouse_button_states[index] == CONTROLS_MOUSE_BUTTON_STATE_OFF) ||       \
    (S_mouse_button_states[index] == CONTROLS_MOUSE_BUTTON_STATE_RELEASED))

#define CONTROLS_MOUSE_BUTTON_IS_PRESSED_BY_NAME(name)                         \
  CONTROLS_MOUSE_BUTTON_IS_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_##name)

#define CONTROLS_MOUSE_BUTTON_IS_RELEASED_BY_NAME(name)                        \
  CONTROLS_MOUSE_BUTTON_IS_RELEASED(CONTROLS_MOUSE_BUTTON_INDEX_##name)

#define CONTROLS_MOUSE_BUTTON_IS_ON_OR_PRESSED_BY_NAME(name)                   \
  CONTROLS_MOUSE_BUTTON_IS_ON_OR_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_##name)

#define CONTROLS_MOUSE_BUTTON_IS_OFF_OR_RELEASED_BY_NAME(name)                 \
  CONTROLS_MOUSE_BUTTON_IS_OFF_OR_RELEASED(CONTROLS_MOUSE_BUTTON_INDEX_##name)

/* keyboard key mapping */
#define CONTROLS_UPDATE_KEY_STATES_PRESS(scancode, name)                       \
  if ((code == scancode) && (CONTROLS_KEY_IS_OFF_OR_RELEASED_BY_NAME(name)))   \
    S_key_states[CONTROLS_KEY_INDEX_##name] = CONTROLS_KEY_STATE_PRESSED;

#define CONTROLS_UPDATE_KEY_STATES_RELEASE(scancode, name)                     \
  if ((code == scancode) && (CONTROLS_KEY_IS_ON_OR_PRESSED_BY_NAME(name)))     \
    S_key_states[CONTROLS_KEY_INDEX_##name] = CONTROLS_KEY_STATE_RELEASED;

/* mouse button mapping */
#define CONTROLS_UPDATE_MOUSE_BUTTON_STATES_PRESS(sdl_name, name)                       \
  if ((button == sdl_name) && (CONTROLS_MOUSE_BUTTON_IS_OFF_OR_RELEASED_BY_NAME(name))) \
    S_mouse_button_states[CONTROLS_MOUSE_BUTTON_INDEX_##name] = CONTROLS_MOUSE_BUTTON_STATE_PRESSED;

#define CONTROLS_UPDATE_MOUSE_BUTTON_STATES_RELEASE(sdl_name, name)                     \
  if ((button == sdl_name) && (CONTROLS_MOUSE_BUTTON_IS_ON_OR_PRESSED_BY_NAME(name)))   \
    S_mouse_button_states[CONTROLS_MOUSE_BUTTON_INDEX_##name] = CONTROLS_MOUSE_BUTTON_STATE_RELEASED;

/* screen region macros */

/* note that the center, width, and height are in 8x8 cells     */
/* (i.e., in 224p). since the mouse positions are in 448p,      */
/* the right side of each inequality has been multiplied by 2.  */
#define CONTROLS_MOUSE_CURSOR_IS_OVER_SCREEN_REGION(rgn)                     \
  ( (S_mouse_remapped_pos_x >= 2 * rgn->x)                &&                 \
    (S_mouse_remapped_pos_x <  2 * (rgn->x + 8 * rgn->w)) &&                 \
    (S_mouse_remapped_pos_y >= 2 * rgn->y)                &&                 \
    (S_mouse_remapped_pos_y <  2 * (rgn->y + 8 * rgn->h)))

#define CONTROLS_MOUSE_CURSOR_IS_OVER_SCROLLED_ELEMENT(e, scroll)            \
  ( (S_mouse_remapped_pos_x >= 2 * e->x)                &&                   \
    (S_mouse_remapped_pos_x <  2 * (e->x + 8 * e->w))   &&                   \
    (S_mouse_remapped_pos_y >= 2 * (e->y - scroll))     &&                   \
    (S_mouse_remapped_pos_y <  2 * (e->y + 8 * e->h - scroll)))

#define CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_SLIDER(wdg, scroll)                                                       \
  ( (S_mouse_remapped_pos_x >= 2 * (wdg->x + LAYOUT_PARAM_SLIDER_TRACK_X))                                        &&  \
    (S_mouse_remapped_pos_x <  2 * (wdg->x + LAYOUT_PARAM_SLIDER_TRACK_X + 8 * LAYOUT_PARAM_SLIDER_TRACK_WIDTH))  &&  \
    (S_mouse_remapped_pos_y >= 2 * (wdg->y - scroll))                                                             &&  \
    (S_mouse_remapped_pos_y <  2 * (wdg->y + 8 * wdg->h - scroll)))

#define CONTROLS_MOUSE_LAST_CLICK_WAS_OVER_PARAM_SLIDER(wdg, scroll)                                                    \
  ( (S_mouse_last_click_pos_x >= 2 * (wdg->x + LAYOUT_PARAM_SLIDER_TRACK_X))                                        &&  \
    (S_mouse_last_click_pos_x <  2 * (wdg->x + LAYOUT_PARAM_SLIDER_TRACK_X + 8 * LAYOUT_PARAM_SLIDER_TRACK_WIDTH))  &&  \
    (S_mouse_last_click_pos_y >= 2 * (wdg->y - scroll))                                                             &&  \
    (S_mouse_last_click_pos_y <  2 * (wdg->y + 8 * wdg->h - scroll)))

#define CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_LEFT_ARROW(wdg, scroll)                   \
  ( (S_mouse_remapped_pos_x >= 2 * (wdg->x + LAYOUT_PARAM_ARROWS_LEFT_X))         &&  \
    (S_mouse_remapped_pos_x <  2 * (wdg->x + LAYOUT_PARAM_ARROWS_LEFT_X + 8 * 1)) &&  \
    (S_mouse_remapped_pos_y >= 2 * (wdg->y - scroll))                             &&  \
    (S_mouse_remapped_pos_y <  2 * (wdg->y + 8 * wdg->h - scroll)))

#define CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_RIGHT_ARROW(wdg, scroll)                    \
  ( (S_mouse_remapped_pos_x >= 2 * (wdg->x + LAYOUT_PARAM_ARROWS_RIGHT_X))          &&  \
    (S_mouse_remapped_pos_x <  2 * (wdg->x + LAYOUT_PARAM_ARROWS_RIGHT_X + 8 * 1))  &&  \
    (S_mouse_remapped_pos_y >= 2 * (wdg->y - scroll))                               &&  \
    (S_mouse_remapped_pos_y <  2 * (wdg->y + 8 * wdg->h - scroll)))

#define CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_RADIO_BUTTON(wdg, scroll)                   \
  ( (S_mouse_remapped_pos_x >= 2 * (wdg->x + LAYOUT_PARAM_RADIO_BUTTON_X))          &&  \
    (S_mouse_remapped_pos_x <  2 * (wdg->x + LAYOUT_PARAM_RADIO_BUTTON_X + 8 * 1))  &&  \
    (S_mouse_remapped_pos_y >= 2 * (wdg->y - scroll))                               &&  \
    (S_mouse_remapped_pos_y <  2 * (wdg->y + 8 * wdg->h - scroll)))

static int S_key_states[CONTROLS_NUM_KEY_INDICES];
static int S_mouse_button_states[CONTROLS_NUM_MOUSE_BUTTON_INDICES];

static int S_mouse_remapped_pos_x;
static int S_mouse_remapped_pos_y;

static int S_mouse_last_click_pos_x;
static int S_mouse_last_click_pos_y;

static int S_mouse_wheel_movement;

/*******************************************************************************
** controls_setup()
*******************************************************************************/
short int controls_setup()
{
  int m;

  for (m = 0; m < CONTROLS_NUM_KEY_INDICES; m++)
    S_key_states[m] = CONTROLS_KEY_STATE_OFF;

  for (m = 0; m < CONTROLS_NUM_MOUSE_BUTTON_INDICES; m++)
    S_mouse_button_states[m] = CONTROLS_MOUSE_BUTTON_STATE_OFF;

  S_mouse_remapped_pos_x = 0;
  S_mouse_remapped_pos_y = 0;

  S_mouse_last_click_pos_x = 0;
  S_mouse_last_click_pos_y = 0;

  S_mouse_wheel_movement = 0;

  return 0;
}

/*******************************************************************************
** controls_keyboard_key_pressed()
*******************************************************************************/
short int controls_keyboard_key_pressed(SDL_Scancode code)
{
  /* common keys */
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_ESCAPE, COMMON_ESCAPE)

  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_LEFTBRACKET,  COMMON_DECREASE_WINDOW_SIZE)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_RIGHTBRACKET, COMMON_INCREASE_WINDOW_SIZE)

  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_UP,   COMMON_SCROLL_UP)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_DOWN, COMMON_SCROLL_DOWN)

  /* musical keyboard keys */
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_Z,          MUSIC_KEYS_OCTAVE_PLUS_ZERO_C)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_S,          MUSIC_KEYS_OCTAVE_PLUS_ZERO_C_SHARP)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_X,          MUSIC_KEYS_OCTAVE_PLUS_ZERO_D)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_D,          MUSIC_KEYS_OCTAVE_PLUS_ZERO_D_SHARP)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_C,          MUSIC_KEYS_OCTAVE_PLUS_ZERO_E)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_V,          MUSIC_KEYS_OCTAVE_PLUS_ZERO_F)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_G,          MUSIC_KEYS_OCTAVE_PLUS_ZERO_F_SHARP)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_B,          MUSIC_KEYS_OCTAVE_PLUS_ZERO_G)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_H,          MUSIC_KEYS_OCTAVE_PLUS_ZERO_G_SHARP)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_N,          MUSIC_KEYS_OCTAVE_PLUS_ZERO_A)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_J,          MUSIC_KEYS_OCTAVE_PLUS_ZERO_A_SHARP)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_M,          MUSIC_KEYS_OCTAVE_PLUS_ZERO_B)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_COMMA,      MUSIC_KEYS_OCTAVE_PLUS_ONE_C)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_L,          MUSIC_KEYS_OCTAVE_PLUS_ONE_C_SHARP)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_PERIOD,     MUSIC_KEYS_OCTAVE_PLUS_ONE_D)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_SEMICOLON,  MUSIC_KEYS_OCTAVE_PLUS_ONE_D_SHARP)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_SLASH,      MUSIC_KEYS_OCTAVE_PLUS_ONE_E)

  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_Q,          MUSIC_KEYS_OCTAVE_PLUS_ONE_C)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_2,          MUSIC_KEYS_OCTAVE_PLUS_ONE_C_SHARP)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_W,          MUSIC_KEYS_OCTAVE_PLUS_ONE_D)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_3,          MUSIC_KEYS_OCTAVE_PLUS_ONE_D_SHARP)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_E,          MUSIC_KEYS_OCTAVE_PLUS_ONE_E)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_R,          MUSIC_KEYS_OCTAVE_PLUS_ONE_F)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_5,          MUSIC_KEYS_OCTAVE_PLUS_ONE_F_SHARP)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_T,          MUSIC_KEYS_OCTAVE_PLUS_ONE_G)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_6,          MUSIC_KEYS_OCTAVE_PLUS_ONE_G_SHARP)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_Y,          MUSIC_KEYS_OCTAVE_PLUS_ONE_A)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_7,          MUSIC_KEYS_OCTAVE_PLUS_ONE_A_SHARP)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_U,          MUSIC_KEYS_OCTAVE_PLUS_ONE_B)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_I,          MUSIC_KEYS_OCTAVE_PLUS_TWO_C)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_9,          MUSIC_KEYS_OCTAVE_PLUS_TWO_C_SHARP)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_O,          MUSIC_KEYS_OCTAVE_PLUS_TWO_D)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_0,          MUSIC_KEYS_OCTAVE_PLUS_TWO_D_SHARP)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_P,          MUSIC_KEYS_OCTAVE_PLUS_TWO_E)

  /* text entry keys */
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_A,          TEXT_ENTRY_A)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_B,          TEXT_ENTRY_B)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_C,          TEXT_ENTRY_C)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_D,          TEXT_ENTRY_D)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_E,          TEXT_ENTRY_E)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_F,          TEXT_ENTRY_F)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_G,          TEXT_ENTRY_G)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_H,          TEXT_ENTRY_H)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_I,          TEXT_ENTRY_I)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_J,          TEXT_ENTRY_J)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_K,          TEXT_ENTRY_K)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_L,          TEXT_ENTRY_L)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_M,          TEXT_ENTRY_M)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_N,          TEXT_ENTRY_N)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_O,          TEXT_ENTRY_O)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_P,          TEXT_ENTRY_P)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_Q,          TEXT_ENTRY_Q)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_R,          TEXT_ENTRY_R)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_S,          TEXT_ENTRY_S)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_T,          TEXT_ENTRY_T)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_U,          TEXT_ENTRY_U)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_V,          TEXT_ENTRY_V)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_W,          TEXT_ENTRY_W)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_X,          TEXT_ENTRY_X)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_Y,          TEXT_ENTRY_Y)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_Z,          TEXT_ENTRY_Z)

  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_0,          TEXT_ENTRY_0)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_1,          TEXT_ENTRY_1)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_2,          TEXT_ENTRY_2)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_3,          TEXT_ENTRY_3)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_4,          TEXT_ENTRY_4)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_5,          TEXT_ENTRY_5)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_6,          TEXT_ENTRY_6)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_7,          TEXT_ENTRY_7)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_8,          TEXT_ENTRY_8)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_9,          TEXT_ENTRY_9)

  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_SPACE,      TEXT_ENTRY_SPACE)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_PERIOD,     TEXT_ENTRY_PERIOD)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_MINUS,      TEXT_ENTRY_HYPHEN)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_RETURN,     TEXT_ENTRY_ENTER)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_BACKSPACE,  TEXT_ENTRY_BACKSPACE)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_LSHIFT,     TEXT_ENTRY_LEFT_SHIFT)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_RSHIFT,     TEXT_ENTRY_RIGHT_SHIFT)

  return 0;
}

/*******************************************************************************
** controls_keyboard_key_released()
*******************************************************************************/
short int controls_keyboard_key_released(SDL_Scancode code)
{
  /* common keys */
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_ESCAPE, COMMON_ESCAPE)

  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_LEFTBRACKET,  COMMON_DECREASE_WINDOW_SIZE)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_RIGHTBRACKET, COMMON_INCREASE_WINDOW_SIZE)

  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_UP,   COMMON_SCROLL_UP)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_DOWN, COMMON_SCROLL_DOWN)

  /* musical keyboard keys */
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_Z,          MUSIC_KEYS_OCTAVE_PLUS_ZERO_C)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_S,          MUSIC_KEYS_OCTAVE_PLUS_ZERO_C_SHARP)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_X,          MUSIC_KEYS_OCTAVE_PLUS_ZERO_D)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_D,          MUSIC_KEYS_OCTAVE_PLUS_ZERO_D_SHARP)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_C,          MUSIC_KEYS_OCTAVE_PLUS_ZERO_E)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_V,          MUSIC_KEYS_OCTAVE_PLUS_ZERO_F)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_G,          MUSIC_KEYS_OCTAVE_PLUS_ZERO_F_SHARP)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_B,          MUSIC_KEYS_OCTAVE_PLUS_ZERO_G)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_H,          MUSIC_KEYS_OCTAVE_PLUS_ZERO_G_SHARP)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_N,          MUSIC_KEYS_OCTAVE_PLUS_ZERO_A)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_J,          MUSIC_KEYS_OCTAVE_PLUS_ZERO_A_SHARP)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_M,          MUSIC_KEYS_OCTAVE_PLUS_ZERO_B)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_COMMA,      MUSIC_KEYS_OCTAVE_PLUS_ONE_C)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_L,          MUSIC_KEYS_OCTAVE_PLUS_ONE_C_SHARP)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_PERIOD,     MUSIC_KEYS_OCTAVE_PLUS_ONE_D)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_SEMICOLON,  MUSIC_KEYS_OCTAVE_PLUS_ONE_D_SHARP)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_SLASH,      MUSIC_KEYS_OCTAVE_PLUS_ONE_E)

  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_Q,          MUSIC_KEYS_OCTAVE_PLUS_ONE_C)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_2,          MUSIC_KEYS_OCTAVE_PLUS_ONE_C_SHARP)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_W,          MUSIC_KEYS_OCTAVE_PLUS_ONE_D)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_3,          MUSIC_KEYS_OCTAVE_PLUS_ONE_D_SHARP)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_E,          MUSIC_KEYS_OCTAVE_PLUS_ONE_E)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_R,          MUSIC_KEYS_OCTAVE_PLUS_ONE_F)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_5,          MUSIC_KEYS_OCTAVE_PLUS_ONE_F_SHARP)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_T,          MUSIC_KEYS_OCTAVE_PLUS_ONE_G)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_6,          MUSIC_KEYS_OCTAVE_PLUS_ONE_G_SHARP)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_Y,          MUSIC_KEYS_OCTAVE_PLUS_ONE_A)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_7,          MUSIC_KEYS_OCTAVE_PLUS_ONE_A_SHARP)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_U,          MUSIC_KEYS_OCTAVE_PLUS_ONE_B)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_I,          MUSIC_KEYS_OCTAVE_PLUS_TWO_C)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_9,          MUSIC_KEYS_OCTAVE_PLUS_TWO_C_SHARP)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_O,          MUSIC_KEYS_OCTAVE_PLUS_TWO_D)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_0,          MUSIC_KEYS_OCTAVE_PLUS_TWO_D_SHARP)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_P,          MUSIC_KEYS_OCTAVE_PLUS_TWO_E)

  /* text entry keys */
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_A,          TEXT_ENTRY_A)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_B,          TEXT_ENTRY_B)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_C,          TEXT_ENTRY_C)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_D,          TEXT_ENTRY_D)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_E,          TEXT_ENTRY_E)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_F,          TEXT_ENTRY_F)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_G,          TEXT_ENTRY_G)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_H,          TEXT_ENTRY_H)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_I,          TEXT_ENTRY_I)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_J,          TEXT_ENTRY_J)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_K,          TEXT_ENTRY_K)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_L,          TEXT_ENTRY_L)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_M,          TEXT_ENTRY_M)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_N,          TEXT_ENTRY_N)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_O,          TEXT_ENTRY_O)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_P,          TEXT_ENTRY_P)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_Q,          TEXT_ENTRY_Q)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_R,          TEXT_ENTRY_R)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_S,          TEXT_ENTRY_S)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_T,          TEXT_ENTRY_T)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_U,          TEXT_ENTRY_U)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_V,          TEXT_ENTRY_V)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_W,          TEXT_ENTRY_W)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_X,          TEXT_ENTRY_X)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_Y,          TEXT_ENTRY_Y)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_Z,          TEXT_ENTRY_Z)

  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_0,          TEXT_ENTRY_0)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_1,          TEXT_ENTRY_1)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_2,          TEXT_ENTRY_2)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_3,          TEXT_ENTRY_3)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_4,          TEXT_ENTRY_4)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_5,          TEXT_ENTRY_5)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_6,          TEXT_ENTRY_6)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_7,          TEXT_ENTRY_7)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_8,          TEXT_ENTRY_8)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_9,          TEXT_ENTRY_9)

  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_SPACE,      TEXT_ENTRY_SPACE)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_PERIOD,     TEXT_ENTRY_PERIOD)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_MINUS,      TEXT_ENTRY_HYPHEN)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_RETURN,     TEXT_ENTRY_ENTER)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_BACKSPACE,  TEXT_ENTRY_BACKSPACE)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_LSHIFT,     TEXT_ENTRY_LEFT_SHIFT)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_RSHIFT,     TEXT_ENTRY_RIGHT_SHIFT)

  return 0;
}

/*******************************************************************************
** controls_mouse_button_pressed()
*******************************************************************************/
short int controls_mouse_button_pressed(Uint8 button, Sint32 x, Sint32 y)
{
  /* buttons */
  CONTROLS_UPDATE_MOUSE_BUTTON_STATES_PRESS(SDL_BUTTON_LEFT,  LEFT)
  CONTROLS_UPDATE_MOUSE_BUTTON_STATES_PRESS(SDL_BUTTON_RIGHT, RIGHT)

  /* update last click position if necessary */
  if (CONTROLS_MOUSE_BUTTON_IS_PRESSED_BY_NAME(LEFT))
  {
    S_mouse_last_click_pos_x = (x * 2 * GRAPHICS_OVERSCAN_WIDTH) / G_viewport_w;
    S_mouse_last_click_pos_y = (y * 2 * GRAPHICS_OVERSCAN_HEIGHT) / G_viewport_h;
  }

  return 0;
}

/*******************************************************************************
** controls_mouse_button_released()
*******************************************************************************/
short int controls_mouse_button_released(Uint8 button)
{
  /* buttons */
  CONTROLS_UPDATE_MOUSE_BUTTON_STATES_RELEASE(SDL_BUTTON_LEFT,  LEFT)
  CONTROLS_UPDATE_MOUSE_BUTTON_STATES_RELEASE(SDL_BUTTON_RIGHT, RIGHT)

  return 0;
}

/*******************************************************************************
** controls_mouse_cursor_moved()
*******************************************************************************/
short int controls_mouse_cursor_moved(Sint32 x, Sint32 y)
{
  /* find the mouse button location in remapped coordinates (448p) */
  S_mouse_remapped_pos_x = (x * 2 * GRAPHICS_OVERSCAN_WIDTH) / G_viewport_w;
  S_mouse_remapped_pos_y = (y * 2 * GRAPHICS_OVERSCAN_HEIGHT) / G_viewport_h;

  return 0;
}

/*******************************************************************************
** controls_mouse_wheel_moved()
*******************************************************************************/
short int controls_mouse_wheel_moved(Sint32 movement)
{
  /* increment mouse wheel movement */
  S_mouse_wheel_movement += movement;

  return 0;
}

/*******************************************************************************
** controls_remove_edges()
*******************************************************************************/
short int controls_remove_edges()
{
  int m;

  for (m = 0; m < CONTROLS_NUM_KEY_INDICES; m++)
  {
    if (S_key_states[m] == CONTROLS_KEY_STATE_PRESSED)
      S_key_states[m] = CONTROLS_KEY_STATE_ON;

    if (S_key_states[m] == CONTROLS_KEY_STATE_RELEASED)
      S_key_states[m] = CONTROLS_KEY_STATE_OFF;
  }

  for (m = 0; m < CONTROLS_NUM_MOUSE_BUTTON_INDICES; m++)
  {
    if (S_mouse_button_states[m] == CONTROLS_MOUSE_BUTTON_STATE_PRESSED)
      S_mouse_button_states[m] = CONTROLS_MOUSE_BUTTON_STATE_ON;

    if (S_mouse_button_states[m] == CONTROLS_MOUSE_BUTTON_STATE_RELEASED)
      S_mouse_button_states[m] = CONTROLS_MOUSE_BUTTON_STATE_OFF;
  }

  return 0;
}

/*******************************************************************************
** controls_process_user_input_top_bar()
*******************************************************************************/
short int controls_process_user_input_top_bar()
{
  int m;

  layout_button* b;

  /**********************/
  /* check mouse clicks */
  /**********************/

  /* buttons */
  for (m = 0; m < LAYOUT_NUM_TOP_BAR_BUTTONS; m++)
  {
    b = &G_layout_top_bar_buttons[m];

    if (CONTROLS_MOUSE_CURSOR_IS_OVER_SCROLLED_ELEMENT(b, 0) && 
        CONTROLS_MOUSE_BUTTON_IS_PRESSED_BY_NAME(LEFT))
    {
      layout_reset_top_bar_states();

      if (m == LAYOUT_TOP_BAR_BUTTON_CART)
        program_set_screen(PROGRAM_SCREEN_CART);
      else if (m == LAYOUT_TOP_BAR_BUTTON_INSTRUMENTS)
        program_set_screen(PROGRAM_SCREEN_INSTRUMENTS);
      else if (m == LAYOUT_TOP_BAR_BUTTON_SONG)
        program_set_screen(PROGRAM_SCREEN_SONG);
      else if (m == LAYOUT_TOP_BAR_BUTTON_MIXER)
        program_set_screen(PROGRAM_SCREEN_MIXER);
      else if (m == LAYOUT_TOP_BAR_BUTTON_SOUND_FX)
        program_set_screen(PROGRAM_SCREEN_SOUND_FX);
      else if (m == LAYOUT_TOP_BAR_BUTTON_DPCM)
        program_set_screen(PROGRAM_SCREEN_DPCM);

      break;
    }
  }

  return 0;
}

/*******************************************************************************
** controls_process_user_input_vertical_scrollbar()
*******************************************************************************/
short int controls_process_user_input_vertical_scrollbar()
{
  /**********************/
  /* check key presses  */
  /**********************/

  /* scroll up */
  if (CONTROLS_KEY_IS_PRESSED_BY_NAME(COMMON_SCROLL_UP))
    G_current_scroll_amount -= 8;

  /* scroll down */
  if (CONTROLS_KEY_IS_PRESSED_BY_NAME(COMMON_SCROLL_DOWN))
    G_current_scroll_amount += 8;

  /********************************/
  /* check mouse clicks and drags */
  /********************************/

  /* mouse wheel scrolling */
  if (S_mouse_wheel_movement != 0)
  {
    if (S_mouse_wheel_movement > 0)
      G_current_scroll_amount -= (S_mouse_wheel_movement / 2) + 4;
    else if (S_mouse_wheel_movement < 0)
      G_current_scroll_amount += (-S_mouse_wheel_movement / 2) + 4;
  }

  if (G_current_scroll_amount < 0)
    G_current_scroll_amount = 0;
  else if (G_current_scroll_amount > G_max_scroll_amount)
    G_current_scroll_amount = G_max_scroll_amount;

  return 0;
}

/*******************************************************************************
** controls_process_user_input_cart_screen()
*******************************************************************************/
short int controls_process_user_input_cart_screen()
{
  int m;

  cart* cr;
  patch* pt;

  layout_region* rgn;

  layout_widget* wdg;

  int val;
  int changed;

  /**********************/
  /* check key presses  */
  /**********************/

  /* notes */
  for ( m = CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_ZERO_C; 
        m <= CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_TWO_E; 
        m++)
  {
    if (CONTROLS_KEY_IS_PRESSED(m))
    {
      instrument_key_pressed( G_patch_edit_instrument_index, 
                              12 * G_patch_edit_octave + m - 
                              CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_ZERO_C);
    }
    else if (CONTROLS_KEY_IS_RELEASED(m))
    {
      instrument_key_released(G_patch_edit_instrument_index, 
                              12 * G_patch_edit_octave + m - 
                              CONTROLS_KEY_INDEX_MUSIC_KEYS_OCTAVE_PLUS_ZERO_C);
    }
  }

  /********************************/
  /* check mouse clicks and drags */
  /********************************/

  /* obtain cart & patch pointers */
  cr = &G_cart_bank[G_patch_edit_cart_number - PATCH_CART_NUMBER_LOWER_BOUND];
  pt = &(cr->patches[G_patch_edit_patch_number - PATCH_PATCH_NUMBER_LOWER_BOUND]);

  /* set the region to the main area */
  rgn = &G_layout_cart_regions[LAYOUT_CART_REGION_MAIN_AREA];

  /* parameters */
  for (m = 0; m < PATCH_NUM_PARAMS; m++)
  {
    wdg = &G_layout_cart_param_widgets[m];

    if (!(LAYOUT_SCROLLED_ELEMENT_IS_IN_REGION(wdg, rgn, G_current_scroll_amount)))
      continue;

    /* reset parameter changed flag */
    changed = 0;

    if (wdg->kind == LAYOUT_WIDGET_KIND_SLIDER)
    {
      if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_SLIDER(wdg, G_current_scroll_amount)      && 
          CONTROLS_MOUSE_LAST_CLICK_WAS_OVER_PARAM_SLIDER(wdg, G_current_scroll_amount) && 
          CONTROLS_MOUSE_BUTTON_IS_ON_OR_PRESSED_BY_NAME(LEFT))
      {
        val = S_mouse_remapped_pos_x - 2 * (wdg->x + LAYOUT_PARAM_SLIDER_TRACK_X);
        val *= G_patch_param_bounds[m];
        val /= 2 * (8 * (LAYOUT_PARAM_SLIDER_TRACK_WIDTH - 1));

        if (pt->values[m] != val)
        {
          pt->values[m] = val;
          changed = 1;
        }
      }
    }
    else if (wdg->kind == LAYOUT_WIDGET_KIND_ARROWS)
    {
      if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_LEFT_ARROW(wdg, G_current_scroll_amount) && 
          CONTROLS_MOUSE_BUTTON_IS_PRESSED_BY_NAME(LEFT))
      {
        if (pt->values[m] > 0)
        {
          pt->values[m] -= 1;
          changed = 1;
        }
      }
      else if ( CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_RIGHT_ARROW(wdg, G_current_scroll_amount) && 
                CONTROLS_MOUSE_BUTTON_IS_PRESSED_BY_NAME(LEFT))
      {
        if (pt->values[m] < G_patch_param_bounds[m])
        {
          pt->values[m] += 1;
          changed = 1;
        }
      }
    }
    else if (wdg->kind == LAYOUT_WIDGET_KIND_RADIO)
    {
      if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_RADIO_BUTTON(wdg, G_current_scroll_amount) && 
          CONTROLS_MOUSE_BUTTON_IS_PRESSED_BY_NAME(LEFT))
      {
        if (pt->values[m] == 0)
          pt->values[m] = 1;
        else if (pt->values[m] == 1)
          pt->values[m] = 0;

        changed = 1;
      }
    }

    /* reload patch if a parameter was changed */
    if (changed == 1)
    {
      if (pt->values[m] < 0)
        pt->values[m] = 0;
      else if (pt->values[m] > G_patch_param_bounds[m])
        pt->values[m] = G_patch_param_bounds[m];

      instrument_load_patch(G_patch_edit_instrument_index, 
                            G_patch_edit_cart_number - PATCH_CART_NUMBER_LOWER_BOUND, 
                            G_patch_edit_patch_number - PATCH_PATCH_NUMBER_LOWER_BOUND);
    }
  }

  return 0;
}

/*******************************************************************************
** controls_process_user_input_standard()
*******************************************************************************/
short int controls_process_user_input_standard()
{
  /* decrease window size */
  if (CONTROLS_KEY_IS_PRESSED_BY_NAME(COMMON_DECREASE_WINDOW_SIZE))
    graphics_decrease_window_size();

  /* increase window size */
  if (CONTROLS_KEY_IS_PRESSED_BY_NAME(COMMON_INCREASE_WINDOW_SIZE))
    graphics_increase_window_size();

  /* top panel */
  controls_process_user_input_top_bar();

  /* cart screen */
  if (G_program_screen == PROGRAM_SCREEN_CART)
  {
    controls_process_user_input_vertical_scrollbar();
    controls_process_user_input_cart_screen();
  }

  /* remove edges */
  controls_remove_edges();

  /* reset mouse wheel movement */
  S_mouse_wheel_movement = 0;

  return 0;
}

