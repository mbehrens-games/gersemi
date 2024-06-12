/*******************************************************************************
** controls.c (keyboard / mouse input)
*******************************************************************************/

#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdlib.h>

#include "bank.h"
#include "controls.h"
#include "global.h"
#include "graphics.h"
#include "instrument.h"
#include "layout.h"
#include "midicont.h"
#include "patch.h"
#include "program.h"
#include "synth.h"
#include "text.h"
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
  CONTROLS_KEY_INDEX_MUSIC_KEYS_PORTAMENTO_SWITCH, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_ARPEGGIO_SWITCH, 
  CONTROLS_KEY_INDEX_MUSIC_KEYS_SUSTAIN_PEDAL, 
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
#define CONTROLS_MOUSE_CURSOR_IS_OVER_SCREEN_REGION(type, center_x, center_y, width, height)  \
  ( (S_mouse_##type##_pos_x >= 2 * (LAYOUT_OVERSCAN_CENTER_X + center_x - 4 * width))   &&    \
    (S_mouse_##type##_pos_x <  2 * (LAYOUT_OVERSCAN_CENTER_X + center_x + 4 * width))   &&    \
    (S_mouse_##type##_pos_y >= 2 * (LAYOUT_OVERSCAN_CENTER_Y + center_y - 4 * height))  &&    \
    (S_mouse_##type##_pos_y <  2 * (LAYOUT_OVERSCAN_CENTER_Y + center_y + 4 * height)))

/* screen regions */
#define CONTROLS_MOUSE_CURSOR_IS_OVER_TOP_PANEL()                              \
  CONTROLS_MOUSE_CURSOR_IS_OVER_SCREEN_REGION(remapped,                        \
                                              LAYOUT_TOP_PANEL_AREA_X,         \
                                              LAYOUT_TOP_PANEL_AREA_Y,         \
                                              LAYOUT_TOP_PANEL_AREA_WIDTH,     \
                                              LAYOUT_TOP_PANEL_AREA_HEIGHT)

#define CONTROLS_MOUSE_CURSOR_IS_OVER_CART_MAIN_AREA()                         \
  CONTROLS_MOUSE_CURSOR_IS_OVER_SCREEN_REGION(remapped,                        \
                                              LAYOUT_CART_MAIN_AREA_X,         \
                                              LAYOUT_CART_MAIN_AREA_Y,         \
                                              LAYOUT_CART_MAIN_AREA_WIDTH,     \
                                              LAYOUT_CART_MAIN_AREA_HEIGHT)

#define CONTROLS_MOUSE_CURSOR_IS_OVER_CART_AUDITION_PANEL()                       \
  CONTROLS_MOUSE_CURSOR_IS_OVER_SCREEN_REGION(remapped,                           \
                                              LAYOUT_CART_AUDITION_PANEL_X,       \
                                              LAYOUT_CART_AUDITION_PANEL_Y,       \
                                              LAYOUT_CART_AUDITION_PANEL_WIDTH,   \
                                              LAYOUT_CART_AUDITION_PANEL_HEIGHT)

/* vertical scrollbar */
#define CONTROLS_MOUSE_CURSOR_IS_OVER_VERTICAL_SCROLLBAR_UP_ARROW()                   \
  CONTROLS_MOUSE_CURSOR_IS_OVER_SCREEN_REGION(remapped,                               \
                                              LAYOUT_VERT_SCROLLBAR_UP_ARROW_X,       \
                                              LAYOUT_VERT_SCROLLBAR_UP_ARROW_Y,       \
                                              LAYOUT_VERT_SCROLLBAR_UP_ARROW_WIDTH,   \
                                              LAYOUT_VERT_SCROLLBAR_UP_ARROW_HEIGHT)

#define CONTROLS_MOUSE_CURSOR_IS_OVER_VERTICAL_SCROLLBAR_DOWN_ARROW()                 \
  CONTROLS_MOUSE_CURSOR_IS_OVER_SCREEN_REGION(remapped,                               \
                                              LAYOUT_VERT_SCROLLBAR_DOWN_ARROW_X,     \
                                              LAYOUT_VERT_SCROLLBAR_DOWN_ARROW_Y,     \
                                              LAYOUT_VERT_SCROLLBAR_DOWN_ARROW_WIDTH, \
                                              LAYOUT_VERT_SCROLLBAR_DOWN_ARROW_HEIGHT)

#define CONTROLS_MOUSE_CURSOR_IS_OVER_VERTICAL_SCROLLBAR_TRACK()                  \
  CONTROLS_MOUSE_CURSOR_IS_OVER_SCREEN_REGION(remapped,                           \
                                              LAYOUT_VERT_SCROLLBAR_TRACK_X,      \
                                              LAYOUT_VERT_SCROLLBAR_TRACK_Y,      \
                                              LAYOUT_VERT_SCROLLBAR_TRACK_WIDTH,  \
                                              LAYOUT_VERT_SCROLLBAR_TRACK_HEIGHT)

#define CONTROLS_MOUSE_LAST_CLICK_WAS_OVER_VERTICAL_SCROLLBAR_TRACK()             \
  CONTROLS_MOUSE_CURSOR_IS_OVER_SCREEN_REGION(last_click,                         \
                                              LAYOUT_VERT_SCROLLBAR_TRACK_X,      \
                                              LAYOUT_VERT_SCROLLBAR_TRACK_Y,      \
                                              LAYOUT_VERT_SCROLLBAR_TRACK_WIDTH,  \
                                              LAYOUT_VERT_SCROLLBAR_TRACK_HEIGHT)

/* buttons */
#define CONTROLS_MOUSE_CURSOR_IS_OVER_TOP_PANEL_BUTTON()                       \
  CONTROLS_MOUSE_CURSOR_IS_OVER_SCREEN_REGION(remapped,                        \
                                              b->center_x, b->center_y,        \
                                              b->width, 2)

#define CONTROLS_MOUSE_CURSOR_IS_OVER_CART_BUTTON()                                                 \
  CONTROLS_MOUSE_CURSOR_IS_OVER_SCREEN_REGION(remapped,                                             \
                                              b->center_x, LAYOUT_SCROLLED_POSITION_Y(b->center_y), \
                                              b->width, 2)

/* text boxes */
#define CONTROLS_MOUSE_CURSOR_IS_OVER_CART_TEXT_BOX()                                               \
  CONTROLS_MOUSE_CURSOR_IS_OVER_SCREEN_REGION(remapped,                                             \
                                              t->center_x, LAYOUT_SCROLLED_POSITION_Y(t->center_y), \
                                              t->width, 2)

/* parameter silders, adjustment arrows, and radio buttons */
#define CONTROLS_MOUSE_CURSOR_IS_OVER_CART_PARAM_SLIDER()                                   \
  CONTROLS_MOUSE_CURSOR_IS_OVER_SCREEN_REGION(remapped,                                     \
                                              (pr->center_x + LAYOUT_PARAM_SLIDER_TRACK_X), \
                                              LAYOUT_SCROLLED_POSITION_Y(pr->center_y),     \
                                              LAYOUT_PARAM_SLIDER_WIDTH, 1)

#define CONTROLS_MOUSE_LAST_CLICK_WAS_OVER_CART_PARAM_SLIDER()                              \
  CONTROLS_MOUSE_CURSOR_IS_OVER_SCREEN_REGION(last_click,                                   \
                                              (pr->center_x + LAYOUT_PARAM_SLIDER_TRACK_X), \
                                              LAYOUT_SCROLLED_POSITION_Y(pr->center_y),     \
                                              LAYOUT_PARAM_SLIDER_WIDTH, 1)

#define CONTROLS_MOUSE_CURSOR_IS_OVER_CART_PARAM_ARROWS_LEFT()                              \
  CONTROLS_MOUSE_CURSOR_IS_OVER_SCREEN_REGION(remapped,                                     \
                                              (pr->center_x + LAYOUT_PARAM_ARROWS_LEFT_X),  \
                                              LAYOUT_SCROLLED_POSITION_Y(pr->center_y),     \
                                              1, 1)

#define CONTROLS_MOUSE_CURSOR_IS_OVER_CART_PARAM_ARROWS_RIGHT()                             \
  CONTROLS_MOUSE_CURSOR_IS_OVER_SCREEN_REGION(remapped,                                     \
                                              (pr->center_x + LAYOUT_PARAM_ARROWS_RIGHT_X), \
                                              LAYOUT_SCROLLED_POSITION_Y(pr->center_y),     \
                                              1, 1)

#define CONTROLS_MOUSE_CURSOR_IS_OVER_CART_PARAM_RADIO_BUTTON()                             \
  CONTROLS_MOUSE_CURSOR_IS_OVER_SCREEN_REGION(remapped,                                     \
                                              (pr->center_x + LAYOUT_PARAM_RADIO_BUTTON_X), \
                                              LAYOUT_SCROLLED_POSITION_Y(pr->center_y),     \
                                              1, 1)

#define CONTROLS_MOUSE_CURSOR_IS_OVER_AUDITION_PARAM_SLIDER()                               \
  CONTROLS_MOUSE_CURSOR_IS_OVER_SCREEN_REGION(remapped,                                     \
                                              (pr->center_x + LAYOUT_PARAM_SLIDER_TRACK_X), \
                                              pr->center_y,                                 \
                                              LAYOUT_PARAM_SLIDER_WIDTH, 1)

#define CONTROLS_MOUSE_LAST_CLICK_WAS_OVER_AUDITION_PARAM_SLIDER()                          \
  CONTROLS_MOUSE_CURSOR_IS_OVER_SCREEN_REGION(last_click,                                   \
                                              (pr->center_x + LAYOUT_PARAM_SLIDER_TRACK_X), \
                                              pr->center_y,                                 \
                                              LAYOUT_PARAM_SLIDER_WIDTH, 1)

/* scrollbar & slider bounds & widths */
#define CONTROLS_VERT_SCROLLBAR_MOUSE_Y_BASE  (LAYOUT_OVERSCAN_CENTER_Y + LAYOUT_VERT_SCROLLBAR_TRACK_Y - 4 * (LAYOUT_VERT_SCROLLBAR_TRACK_HEIGHT - 2))
#define CONTROLS_VERT_SCROLLBAR_MOUSE_Y_BOUND (8 * (LAYOUT_VERT_SCROLLBAR_TRACK_HEIGHT - 2))

#define CONTROLS_PARAM_SLIDER_MOUSE_X_BASE    (LAYOUT_OVERSCAN_CENTER_X + pr->center_x + LAYOUT_PARAM_SLIDER_TRACK_X - 4 * (LAYOUT_PARAM_SLIDER_WIDTH - 1))
#define CONTROLS_PARAM_SLIDER_MOUSE_X_BOUND   (8 * (LAYOUT_PARAM_SLIDER_WIDTH - 1))

/* cart edit screen parameter adjust macros */
#define CONTROLS_CART_PARAM_ADJUST_CASE(name, param)                           \
  case LAYOUT_CART_PARAM_##name:                                               \
  {                                                                            \
    if (pr->type == LAYOUT_PARAM_TYPE_SLIDER)                                  \
    {                                                                          \
      if (param != val)                                                        \
      {                                                                        \
        param = val;                                                           \
        changed = 1;                                                           \
                                                                               \
        if (param < pr->lower_bound)                                           \
          param = pr->lower_bound;                                             \
        else if (param > pr->upper_bound)                                      \
          param = pr->upper_bound;                                             \
      }                                                                        \
    }                                                                          \
    else if (pr->type == LAYOUT_PARAM_TYPE_ARROWS)                             \
    {                                                                          \
      param += val;                                                            \
      changed = 1;                                                             \
                                                                               \
      if (param < pr->lower_bound)                                             \
        param = pr->lower_bound;                                               \
      else if (param > pr->upper_bound)                                        \
        param = pr->upper_bound;                                               \
    }                                                                          \
    else if (pr->type == LAYOUT_PARAM_TYPE_RADIO)                              \
    {                                                                          \
      if (param == pr->lower_bound)                                            \
        param = pr->upper_bound;                                               \
      else                                                                     \
        param = pr->lower_bound;                                               \
                                                                               \
      changed = 1;                                                             \
    }                                                                          \
    else if (pr->type == LAYOUT_PARAM_TYPE_FLAG)                               \
    {                                                                          \
      if ((param & pr->upper_bound) == 0)                                      \
        param |= pr->upper_bound;                                              \
      else                                                                     \
        param &= ~pr->upper_bound;                                             \
                                                                               \
      changed = 1;                                                             \
    }                                                                          \
                                                                               \
    break;                                                                     \
  }

#define CONTROLS_AUDITION_PARAM_ADJUST_CASE(name, param)                       \
  case LAYOUT_AUDITION_PARAM_##name:                                           \
  {                                                                            \
    if (pr->type == LAYOUT_PARAM_TYPE_SLIDER)                                  \
    {                                                                          \
      if (param != val)                                                        \
      {                                                                        \
        param = val;                                                           \
        changed = 1;                                                           \
                                                                               \
        if (param < pr->lower_bound)                                           \
          param = pr->lower_bound;                                             \
        else if (param > pr->upper_bound)                                      \
          param = pr->upper_bound;                                             \
      }                                                                        \
    }                                                                          \
                                                                               \
    break;                                                                     \
  }

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
** controls_cart_parameter_adjust()
*******************************************************************************/
short int controls_cart_parameter_adjust( int cart_index, int patch_index, 
                                          int param_index, int val)
{
  cart* cr;
  patch* pt;

  param* pr;

  short int changed;

  /* make sure that the cart & patch indices are valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(cart_index))
    return 1;

  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* make sure that the parameter index is valid */
  if ((param_index < 0) || (param_index >= LAYOUT_NUM_CART_PARAMS))
    return 1;

  /* obtain cart, patch & parameter pointers */
  cr = &G_cart_bank[cart_index];
  pt = &(cr->patches[patch_index]);

  pr = &G_layout_cart_params[param_index];

  /* skip oscillator multiple/divisor or octave/note based on frequency mode */
  if (((pt->osc_freq_mode[0] == PATCH_OSC_FREQ_MODE_FIXED) && (param_index == LAYOUT_CART_PARAM_OSC_1_MULTIPLE))  || 
      ((pt->osc_freq_mode[1] == PATCH_OSC_FREQ_MODE_FIXED) && (param_index == LAYOUT_CART_PARAM_OSC_2_MULTIPLE))  || 
      ((pt->osc_freq_mode[2] == PATCH_OSC_FREQ_MODE_FIXED) && (param_index == LAYOUT_CART_PARAM_OSC_3_MULTIPLE))  || 
      ((pt->osc_freq_mode[0] == PATCH_OSC_FREQ_MODE_FIXED) && (param_index == LAYOUT_CART_PARAM_OSC_1_DIVISOR))   || 
      ((pt->osc_freq_mode[1] == PATCH_OSC_FREQ_MODE_FIXED) && (param_index == LAYOUT_CART_PARAM_OSC_2_DIVISOR))   || 
      ((pt->osc_freq_mode[2] == PATCH_OSC_FREQ_MODE_FIXED) && (param_index == LAYOUT_CART_PARAM_OSC_3_DIVISOR))   || 
      ((pt->osc_freq_mode[0] == PATCH_OSC_FREQ_MODE_RATIO) && (param_index == LAYOUT_CART_PARAM_OSC_1_OCTAVE))    || 
      ((pt->osc_freq_mode[1] == PATCH_OSC_FREQ_MODE_RATIO) && (param_index == LAYOUT_CART_PARAM_OSC_2_OCTAVE))    || 
      ((pt->osc_freq_mode[2] == PATCH_OSC_FREQ_MODE_RATIO) && (param_index == LAYOUT_CART_PARAM_OSC_3_OCTAVE))    || 
      ((pt->osc_freq_mode[0] == PATCH_OSC_FREQ_MODE_RATIO) && (param_index == LAYOUT_CART_PARAM_OSC_1_NOTE))      || 
      ((pt->osc_freq_mode[1] == PATCH_OSC_FREQ_MODE_RATIO) && (param_index == LAYOUT_CART_PARAM_OSC_2_NOTE))      || 
      ((pt->osc_freq_mode[2] == PATCH_OSC_FREQ_MODE_RATIO) && (param_index == LAYOUT_CART_PARAM_OSC_3_NOTE)))
  {
    return 0;
  }

  /* reset parameter changed flag */
  changed = 0;

  /* apply adjustment */
  switch (param_index)
  {
    CONTROLS_CART_PARAM_ADJUST_CASE(CART_NUMBER,  G_patch_edit_cart_number)
    CONTROLS_CART_PARAM_ADJUST_CASE(PATCH_NUMBER, G_patch_edit_patch_number)

    CONTROLS_CART_PARAM_ADJUST_CASE(ALGORITHM,  pt->algorithm)
    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_SYNC,   pt->osc_sync)

    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_1_WAVEFORM,   pt->osc_waveform[0])
    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_1_PHI,        pt->osc_phi[0])
    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_1_FREQ_MODE,  pt->osc_freq_mode[0])
    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_1_MULTIPLE,   pt->osc_multiple[0])
    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_1_DIVISOR,    pt->osc_divisor[0])
    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_1_OCTAVE,     pt->osc_octave[0])
    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_1_NOTE,       pt->osc_note[0])
    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_1_DETUNE,     pt->osc_detune[0])

    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_1_ROUTING_VIBRATO,      pt->osc_routing[0])
    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_1_ROUTING_PITCH_ENV,    pt->osc_routing[0])
    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_1_ROUTING_PITCH_WHEEL,  pt->osc_routing[0])

    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_2_WAVEFORM,   pt->osc_waveform[1])
    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_2_PHI,        pt->osc_phi[1])
    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_2_FREQ_MODE,  pt->osc_freq_mode[1])
    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_2_MULTIPLE,   pt->osc_multiple[1])
    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_2_DIVISOR,    pt->osc_divisor[1])
    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_2_OCTAVE,     pt->osc_octave[1])
    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_2_NOTE,       pt->osc_note[1])
    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_2_DETUNE,     pt->osc_detune[1])

    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_2_ROUTING_VIBRATO,      pt->osc_routing[1])
    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_2_ROUTING_PITCH_ENV,    pt->osc_routing[1])
    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_2_ROUTING_PITCH_WHEEL,  pt->osc_routing[1])

    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_3_WAVEFORM,   pt->osc_waveform[2])
    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_3_PHI,        pt->osc_phi[2])
    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_3_FREQ_MODE,  pt->osc_freq_mode[2])
    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_3_MULTIPLE,   pt->osc_multiple[2])
    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_3_DIVISOR,    pt->osc_divisor[2])
    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_3_OCTAVE,     pt->osc_octave[2])
    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_3_NOTE,       pt->osc_note[2])
    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_3_DETUNE,     pt->osc_detune[2])

    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_3_ROUTING_VIBRATO,      pt->osc_routing[2])
    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_3_ROUTING_PITCH_ENV,    pt->osc_routing[2])
    CONTROLS_CART_PARAM_ADJUST_CASE(OSC_3_ROUTING_PITCH_WHEEL,  pt->osc_routing[2])

    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_1_ATTACK,     pt->env_attack[0])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_1_DECAY,      pt->env_decay[0])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_1_SUSTAIN,    pt->env_sustain[0])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_1_RELEASE,    pt->env_release[0])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_1_AMPLITUDE,  pt->env_amplitude[0])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_1_HOLD_LEVEL, pt->env_hold_level[0])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_1_HOLD_MODE,  pt->env_hold_mode[0])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_1_RATE_KS,    pt->env_rate_ks[0])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_1_LEVEL_KS,   pt->env_level_ks[0])

    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_1_ROUTING_TREMOLO,  pt->env_routing[0])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_1_ROUTING_BOOST,    pt->env_routing[0])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_1_ROUTING_VELOCITY, pt->env_routing[0])

    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_2_ATTACK,     pt->env_attack[1])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_2_DECAY,      pt->env_decay[1])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_2_SUSTAIN,    pt->env_sustain[1])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_2_RELEASE,    pt->env_release[1])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_2_AMPLITUDE,  pt->env_amplitude[1])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_2_HOLD_LEVEL, pt->env_hold_level[1])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_2_HOLD_MODE,  pt->env_hold_mode[1])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_2_RATE_KS,    pt->env_rate_ks[1])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_2_LEVEL_KS,   pt->env_level_ks[1])

    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_2_ROUTING_TREMOLO,  pt->env_routing[1])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_2_ROUTING_BOOST,    pt->env_routing[1])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_2_ROUTING_VELOCITY, pt->env_routing[1])

    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_3_ATTACK,     pt->env_attack[2])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_3_DECAY,      pt->env_decay[2])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_3_SUSTAIN,    pt->env_sustain[2])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_3_RELEASE,    pt->env_release[2])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_3_AMPLITUDE,  pt->env_amplitude[2])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_3_HOLD_LEVEL, pt->env_hold_level[2])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_3_HOLD_MODE,  pt->env_hold_mode[2])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_3_RATE_KS,    pt->env_rate_ks[2])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_3_LEVEL_KS,   pt->env_level_ks[2])

    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_3_ROUTING_TREMOLO,  pt->env_routing[2])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_3_ROUTING_BOOST,    pt->env_routing[2])
    CONTROLS_CART_PARAM_ADJUST_CASE(ENV_3_ROUTING_VELOCITY, pt->env_routing[2])

    CONTROLS_CART_PARAM_ADJUST_CASE(VIBRATO_WAVEFORM,     pt->lfo_waveform[0])
    CONTROLS_CART_PARAM_ADJUST_CASE(VIBRATO_DELAY,        pt->lfo_delay[0])
    CONTROLS_CART_PARAM_ADJUST_CASE(VIBRATO_SPEED,        pt->lfo_speed[0])
    CONTROLS_CART_PARAM_ADJUST_CASE(VIBRATO_DEPTH,        pt->lfo_depth[0])
    CONTROLS_CART_PARAM_ADJUST_CASE(VIBRATO_SENSITIVITY,  pt->lfo_sensitivity[0])
    CONTROLS_CART_PARAM_ADJUST_CASE(VIBRATO_SYNC,         pt->lfo_sync[0])
    CONTROLS_CART_PARAM_ADJUST_CASE(VIBRATO_POLARITY,     pt->lfo_polarity[0])

    CONTROLS_CART_PARAM_ADJUST_CASE(TREMOLO_WAVEFORM,     pt->lfo_waveform[1])
    CONTROLS_CART_PARAM_ADJUST_CASE(TREMOLO_DELAY,        pt->lfo_delay[1])
    CONTROLS_CART_PARAM_ADJUST_CASE(TREMOLO_SPEED,        pt->lfo_speed[1])
    CONTROLS_CART_PARAM_ADJUST_CASE(TREMOLO_DEPTH,        pt->lfo_depth[1])
    CONTROLS_CART_PARAM_ADJUST_CASE(TREMOLO_SENSITIVITY,  pt->lfo_sensitivity[1])
    CONTROLS_CART_PARAM_ADJUST_CASE(TREMOLO_SYNC,         pt->lfo_sync[1])

    CONTROLS_CART_PARAM_ADJUST_CASE(CHORUS_WAVEFORM,      pt->lfo_waveform[2])
    CONTROLS_CART_PARAM_ADJUST_CASE(CHORUS_DELAY,         pt->lfo_delay[2])
    CONTROLS_CART_PARAM_ADJUST_CASE(CHORUS_SPEED,         pt->lfo_speed[2])
    CONTROLS_CART_PARAM_ADJUST_CASE(CHORUS_DEPTH,         pt->lfo_depth[2])
    CONTROLS_CART_PARAM_ADJUST_CASE(CHORUS_SENSITIVITY,   pt->lfo_sensitivity[2])
    CONTROLS_CART_PARAM_ADJUST_CASE(CHORUS_SYNC,          pt->lfo_sync[2])

    CONTROLS_CART_PARAM_ADJUST_CASE(BOOST_SENSITIVITY,    pt->boost_sensitivity)
    CONTROLS_CART_PARAM_ADJUST_CASE(VELOCITY_SENSITIVITY, pt->velocity_sensitivity)

    CONTROLS_CART_PARAM_ADJUST_CASE(FILTERS_HIGHPASS,   pt->highpass_cutoff)
    CONTROLS_CART_PARAM_ADJUST_CASE(FILTERS_LOWPASS,    pt->lowpass_cutoff)

    CONTROLS_CART_PARAM_ADJUST_CASE(PITCH_ENV_ATTACK,   pt->peg_attack)
    CONTROLS_CART_PARAM_ADJUST_CASE(PITCH_ENV_DECAY,    pt->peg_decay)
    CONTROLS_CART_PARAM_ADJUST_CASE(PITCH_ENV_RELEASE,  pt->peg_release)
    CONTROLS_CART_PARAM_ADJUST_CASE(PITCH_ENV_MAXIMUM,  pt->peg_maximum)
    CONTROLS_CART_PARAM_ADJUST_CASE(PITCH_ENV_FINALE,   pt->peg_finale)

    CONTROLS_CART_PARAM_ADJUST_CASE(PITCH_WHEEL_MODE,   pt->pitch_wheel_mode)
    CONTROLS_CART_PARAM_ADJUST_CASE(PITCH_WHEEL_RANGE,  pt->pitch_wheel_range)

    CONTROLS_CART_PARAM_ADJUST_CASE(ARPEGGIO_MODE,      pt->arpeggio_mode)
    CONTROLS_CART_PARAM_ADJUST_CASE(ARPEGGIO_PATTERN,   pt->arpeggio_pattern)
    CONTROLS_CART_PARAM_ADJUST_CASE(ARPEGGIO_OCTAVES,   pt->arpeggio_octaves)
    CONTROLS_CART_PARAM_ADJUST_CASE(ARPEGGIO_SPEED,     pt->arpeggio_speed)

    CONTROLS_CART_PARAM_ADJUST_CASE(PORTAMENTO_MODE,    pt->portamento_mode)
    CONTROLS_CART_PARAM_ADJUST_CASE(PORTAMENTO_LEGATO,  pt->portamento_legato)
    CONTROLS_CART_PARAM_ADJUST_CASE(PORTAMENTO_SPEED,   pt->portamento_speed)

    CONTROLS_CART_PARAM_ADJUST_CASE(MOD_WHEEL_ROUTING_VIBRATO,  pt->mod_wheel_routing)
    CONTROLS_CART_PARAM_ADJUST_CASE(MOD_WHEEL_ROUTING_TREMOLO,  pt->mod_wheel_routing)
    CONTROLS_CART_PARAM_ADJUST_CASE(MOD_WHEEL_ROUTING_BOOST,    pt->mod_wheel_routing)
    CONTROLS_CART_PARAM_ADJUST_CASE(MOD_WHEEL_ROUTING_CHORUS,   pt->mod_wheel_routing)

    CONTROLS_CART_PARAM_ADJUST_CASE(AFTERTOUCH_ROUTING_VIBRATO, pt->aftertouch_routing)
    CONTROLS_CART_PARAM_ADJUST_CASE(AFTERTOUCH_ROUTING_TREMOLO, pt->aftertouch_routing)
    CONTROLS_CART_PARAM_ADJUST_CASE(AFTERTOUCH_ROUTING_BOOST,   pt->aftertouch_routing)
    CONTROLS_CART_PARAM_ADJUST_CASE(AFTERTOUCH_ROUTING_CHORUS,  pt->aftertouch_routing)

    CONTROLS_CART_PARAM_ADJUST_CASE(EXP_PEDAL_ROUTING_VIBRATO,  pt->exp_pedal_routing)
    CONTROLS_CART_PARAM_ADJUST_CASE(EXP_PEDAL_ROUTING_TREMOLO,  pt->exp_pedal_routing)
    CONTROLS_CART_PARAM_ADJUST_CASE(EXP_PEDAL_ROUTING_BOOST,    pt->exp_pedal_routing)
    CONTROLS_CART_PARAM_ADJUST_CASE(EXP_PEDAL_ROUTING_CHORUS,   pt->exp_pedal_routing)

    default:
      break;
  }

  /* reload patch if a parameter was changed */
  if (changed == 1)
  {
    if ((param_index == LAYOUT_CART_PARAM_CART_NUMBER) || 
        (param_index == LAYOUT_CART_PARAM_PATCH_NUMBER))
    {
      instrument_load_patch(G_patch_edit_instrument_index, 
                            G_patch_edit_cart_number - PATCH_CART_NUMBER_LOWER_BOUND, 
                            G_patch_edit_patch_number - PATCH_PATCH_NUMBER_LOWER_BOUND);
    }
    else
    {
      instrument_load_patch(G_patch_edit_instrument_index, 
                            cart_index, patch_index);
    }
  }

  return 0;
}

/*******************************************************************************
** controls_audition_parameter_adjust()
*******************************************************************************/
short int controls_audition_parameter_adjust(int param_index, int val)
{
  int m;

  param* pr;

  short int changed;

  /* make sure that the parameter index is valid */
  if ((param_index < 0) || (param_index >= LAYOUT_NUM_AUDITION_PARAMS))
    return 1;

  /* obtain parameter pointer */
  pr = &G_layout_audition_params[param_index];

  /* reset parameter changed flag */
  changed = 0;

  /* apply adjustment */
  switch (param_index)
  {
    CONTROLS_AUDITION_PARAM_ADJUST_CASE(OCTAVE,       G_patch_edit_octave)
    CONTROLS_AUDITION_PARAM_ADJUST_CASE(VELOCITY,     G_patch_edit_note_velocity)
    CONTROLS_AUDITION_PARAM_ADJUST_CASE(PITCH_WHEEL,  G_patch_edit_pitch_wheel_pos)
    CONTROLS_AUDITION_PARAM_ADJUST_CASE(MOD_WHEEL,    G_patch_edit_mod_wheel_pos)
    CONTROLS_AUDITION_PARAM_ADJUST_CASE(AFTERTOUCH,   G_patch_edit_aftertouch_pos)
    CONTROLS_AUDITION_PARAM_ADJUST_CASE(EXP_PEDAL,    G_patch_edit_exp_pedal_pos)

    default:
      break;
  }

  /* send instrument command if a parameter was changed */
  if (changed == 1)
  {
    if (param_index == LAYOUT_AUDITION_PARAM_OCTAVE)
    {
      /* send note-offs to all available & nearby octaves */
      for (m = 0; m < 12; m++)
      {
        instrument_key_released(G_patch_edit_instrument_index, 12 * (G_patch_edit_octave - 1) + m);
        instrument_key_released(G_patch_edit_instrument_index, 12 * (G_patch_edit_octave + 0) + m);
        instrument_key_released(G_patch_edit_instrument_index, 12 * (G_patch_edit_octave + 1) + m);
        instrument_key_released(G_patch_edit_instrument_index, 12 * (G_patch_edit_octave + 2) + m);
      }

      for (m = 0; m < 5; m++)
        instrument_key_released(G_patch_edit_instrument_index, 12 * (G_patch_edit_octave + 3) + m);
    }
    else if (param_index == LAYOUT_AUDITION_PARAM_VELOCITY)
      instrument_set_note_velocity(G_patch_edit_instrument_index, G_patch_edit_note_velocity);
    else if (param_index == LAYOUT_AUDITION_PARAM_PITCH_WHEEL)
      instrument_set_pitch_wheel_position(G_patch_edit_instrument_index, G_patch_edit_pitch_wheel_pos);
    else if (param_index == LAYOUT_AUDITION_PARAM_MOD_WHEEL)
      instrument_set_mod_wheel_position(G_patch_edit_instrument_index, G_patch_edit_mod_wheel_pos);
    else if (param_index == LAYOUT_AUDITION_PARAM_AFTERTOUCH)
      instrument_set_aftertouch_position(G_patch_edit_instrument_index, G_patch_edit_aftertouch_pos);
    else if (param_index == LAYOUT_AUDITION_PARAM_EXP_PEDAL)
      instrument_set_exp_pedal_position(G_patch_edit_instrument_index, G_patch_edit_exp_pedal_pos);
  }

  return 0;
}

/*******************************************************************************
** controls_cart_text_entry()
*******************************************************************************/
short int controls_cart_text_entry(int cart_index, int patch_index, char c)
{
  int m;

  cart* cr;
  patch* pt;

  char* name;

  short int index;
  short int bound;

  /* make sure that the cart & patch indices are valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(cart_index))
    return 1;

  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain cart & patch pointers */
  cr = &G_cart_bank[cart_index];
  pt = &(cr->patches[patch_index]);

  /* make sure the character is valid */
  if ((c != '\b') && 
      (!(TEXT_CHARACTER_IS_VALID_IN_CART_OR_PATCH_NAME(c))))
  {
    return 0;
  }

  /* set the name pointer */
  if (G_program_screen == PROGRAM_SCREEN_TEXT_ENTRY_CART_NAME)
  {
    name = &cr->name[0];
    bound = PATCH_CART_NAME_SIZE;
  }
  else if (G_program_screen == PROGRAM_SCREEN_TEXT_ENTRY_PATCH_NAME)
  {
    name = &pt->name[0];
    bound = PATCH_PATCH_NAME_SIZE;
  }
  else
    return 0;

  /* determine index of terminating null character of string */
  index = bound - 1;

  for (m = 0; m < bound - 1; m++)
  {
    if (name[m] == '\0')
    {
      index = m;
      break;
    }
  }

  /* if this is a backspace character, remove the last text character */
  if ((c == '\b') && (index > 0))
    name[index - 1] = '\0';
  /* otherwise, add this character to the end of the string */
  else if (index < bound - 1)
  {
    name[index] = c;
    name[index + 1] = '\0';
  }

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

  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_LSHIFT,     MUSIC_KEYS_PORTAMENTO_SWITCH)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_RSHIFT,     MUSIC_KEYS_ARPEGGIO_SWITCH)
  CONTROLS_UPDATE_KEY_STATES_PRESS(SDL_SCANCODE_SPACE,      MUSIC_KEYS_SUSTAIN_PEDAL)

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

  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_LSHIFT,     MUSIC_KEYS_PORTAMENTO_SWITCH)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_RSHIFT,     MUSIC_KEYS_ARPEGGIO_SWITCH)
  CONTROLS_UPDATE_KEY_STATES_RELEASE(SDL_SCANCODE_SPACE,      MUSIC_KEYS_SUSTAIN_PEDAL)

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
** controls_process_user_input_top_panel()
*******************************************************************************/
short int controls_process_user_input_top_panel()
{
  int m;

  button* b;

  /**********************/
  /* check mouse clicks */
  /**********************/

  /* buttons */
  for (m = 0; m < LAYOUT_NUM_TOP_PANEL_BUTTONS; m++)
  {
    b = &G_layout_top_panel_buttons[m];

    if (CONTROLS_MOUSE_CURSOR_IS_OVER_TOP_PANEL_BUTTON() && 
        CONTROLS_MOUSE_BUTTON_IS_PRESSED_BY_NAME(LEFT))
    {
      layout_reset_top_panel_button_states();

      b->state = LAYOUT_BUTTON_STATE_ON;

      if (m == LAYOUT_TOP_PANEL_BUTTON_CART)
        program_set_screen(PROGRAM_SCREEN_CART);
      else if (m == LAYOUT_TOP_PANEL_BUTTON_INSTRUMENTS)
        program_set_screen(PROGRAM_SCREEN_INSTRUMENTS);
      else if (m == LAYOUT_TOP_PANEL_BUTTON_SONG)
        program_set_screen(PROGRAM_SCREEN_SONG);
      else if (m == LAYOUT_TOP_PANEL_BUTTON_MIXER)
        program_set_screen(PROGRAM_SCREEN_MIXER);
      else if (m == LAYOUT_TOP_PANEL_BUTTON_SOUND_FX)
        program_set_screen(PROGRAM_SCREEN_SOUND_FX);
      else if (m == LAYOUT_TOP_PANEL_BUTTON_DPCM)
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

  /* mouse clicks or drags on the scrollbar */
  if (CONTROLS_MOUSE_LAST_CLICK_WAS_OVER_VERTICAL_SCROLLBAR_TRACK() && 
      CONTROLS_MOUSE_BUTTON_IS_ON_OR_PRESSED_BY_NAME(LEFT))
  {
    G_current_scroll_amount = S_mouse_remapped_pos_y - 2 * CONTROLS_VERT_SCROLLBAR_MOUSE_Y_BASE;
    G_current_scroll_amount *= G_max_scroll_amount;
    G_current_scroll_amount /= 2 * CONTROLS_VERT_SCROLLBAR_MOUSE_Y_BOUND;
  }

  /* mouse clicks on the scrollbar arrows */
  if (CONTROLS_MOUSE_CURSOR_IS_OVER_VERTICAL_SCROLLBAR_UP_ARROW() && 
      CONTROLS_MOUSE_BUTTON_IS_PRESSED_BY_NAME(LEFT))
  {
    G_current_scroll_amount -= 8;
  }

  if (CONTROLS_MOUSE_CURSOR_IS_OVER_VERTICAL_SCROLLBAR_DOWN_ARROW() && 
      CONTROLS_MOUSE_BUTTON_IS_PRESSED_BY_NAME(LEFT))
  {
    G_current_scroll_amount += 8;
  }

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

  button*   b;
  text_box* t;
  param*    pr;

  int val;

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

  /* portamento / arpeggio switches */
  if (CONTROLS_KEY_IS_PRESSED_BY_NAME(MUSIC_KEYS_PORTAMENTO_SWITCH)  || 
      CONTROLS_KEY_IS_PRESSED_BY_NAME(MUSIC_KEYS_ARPEGGIO_SWITCH)    || 
      CONTROLS_KEY_IS_RELEASED_BY_NAME(MUSIC_KEYS_PORTAMENTO_SWITCH) || 
      CONTROLS_KEY_IS_RELEASED_BY_NAME(MUSIC_KEYS_ARPEGGIO_SWITCH))
  {
    if (CONTROLS_KEY_IS_ON_OR_PRESSED_BY_NAME(MUSIC_KEYS_PORTAMENTO_SWITCH) && 
        CONTROLS_KEY_IS_ON_OR_PRESSED_BY_NAME(MUSIC_KEYS_ARPEGGIO_SWITCH))
    {
      instrument_set_port_arp_switch(G_patch_edit_instrument_index, MIDI_CONT_PORT_ARP_SWITCH_OFF);
    }
    else if ( CONTROLS_KEY_IS_ON_OR_PRESSED_BY_NAME(MUSIC_KEYS_PORTAMENTO_SWITCH) && 
              CONTROLS_KEY_IS_OFF_OR_RELEASED_BY_NAME(MUSIC_KEYS_ARPEGGIO_SWITCH))
    {
      instrument_set_port_arp_switch(G_patch_edit_instrument_index, MIDI_CONT_PORT_ARP_SWITCH_PORTAMENTO);
    }
    else if ( CONTROLS_KEY_IS_OFF_OR_RELEASED_BY_NAME(MUSIC_KEYS_PORTAMENTO_SWITCH) && 
              CONTROLS_KEY_IS_ON_OR_PRESSED_BY_NAME(MUSIC_KEYS_ARPEGGIO_SWITCH))
    {
      instrument_set_port_arp_switch(G_patch_edit_instrument_index, MIDI_CONT_PORT_ARP_SWITCH_ARPEGGIO);
    }
    else
      instrument_set_port_arp_switch(G_patch_edit_instrument_index, MIDI_CONT_PORT_ARP_SWITCH_OFF);
  }

  /* sustain pedal */
  if (CONTROLS_KEY_IS_PRESSED_BY_NAME(MUSIC_KEYS_SUSTAIN_PEDAL))
    instrument_set_sustain_pedal(G_patch_edit_instrument_index, MIDI_CONT_SUSTAIN_PEDAL_DOWN);
  else if (CONTROLS_KEY_IS_RELEASED_BY_NAME(MUSIC_KEYS_SUSTAIN_PEDAL))
    instrument_set_sustain_pedal(G_patch_edit_instrument_index, MIDI_CONT_SUSTAIN_PEDAL_UP);

  /********************************/
  /* check mouse clicks and drags */
  /********************************/

  /* buttons */
  for (m = 0; m < LAYOUT_NUM_CART_BUTTONS; m++)
  {
    b = &G_layout_cart_buttons[m];

    if (LAYOUT_CART_BUTTON_OR_TEXT_BOX_IS_NOT_IN_MAIN_AREA(b->center_y))
      continue;

    if (CONTROLS_MOUSE_CURSOR_IS_OVER_CART_BUTTON() && 
        CONTROLS_MOUSE_BUTTON_IS_PRESSED_BY_NAME(LEFT))
    {
      if (b->state == LAYOUT_BUTTON_STATE_OFF)
        b->state = LAYOUT_BUTTON_STATE_ON;
      else
        b->state = LAYOUT_BUTTON_STATE_OFF;
    }
  }

  /* text boxes */
  for (m = 0; m < LAYOUT_NUM_CART_TEXT_BOXES; m++)
  {
    t = &G_layout_cart_text_boxes[m];

    if (LAYOUT_CART_BUTTON_OR_TEXT_BOX_IS_NOT_IN_MAIN_AREA(t->center_y))
      continue;

    if (CONTROLS_MOUSE_CURSOR_IS_OVER_CART_TEXT_BOX() && 
        CONTROLS_MOUSE_BUTTON_IS_PRESSED_BY_NAME(LEFT))
    {
      t->state = LAYOUT_TEXT_BOX_STATE_HIGHLIGHT;

      if (m == LAYOUT_CART_TEXT_BOX_CART_NAME)
        program_set_screen(PROGRAM_SCREEN_TEXT_ENTRY_CART_NAME);
      else if (m == LAYOUT_CART_TEXT_BOX_PATCH_NAME)
        program_set_screen(PROGRAM_SCREEN_TEXT_ENTRY_PATCH_NAME);
    }
  }

  /* parameters */
  for (m = 0; m < LAYOUT_NUM_CART_PARAMS; m++)
  {
    pr = &G_layout_cart_params[m];

    if (LAYOUT_CART_HEADER_OR_PARAM_IS_NOT_IN_MAIN_AREA(pr->center_y))
      continue;

    if (pr->type == LAYOUT_PARAM_TYPE_SLIDER)
    {
      if (CONTROLS_MOUSE_LAST_CLICK_WAS_OVER_CART_PARAM_SLIDER() && 
          CONTROLS_MOUSE_BUTTON_IS_ON_OR_PRESSED_BY_NAME(LEFT))
      {
        /* (new_val - lower_bound) / (upper_bound - lower_bound) = (mouse_x - SLIDER_POS_LOWER_BOUND) / SLIDER_WIDTH */
        val = S_mouse_remapped_pos_x - 2 * CONTROLS_PARAM_SLIDER_MOUSE_X_BASE;
        val *= pr->upper_bound - pr->lower_bound;
        val /= 2 * CONTROLS_PARAM_SLIDER_MOUSE_X_BOUND;
        val += pr->lower_bound;

        if (val < pr->lower_bound)
          val = pr->lower_bound;
        else if (val > pr->upper_bound)
          val = pr->upper_bound;

        controls_cart_parameter_adjust( G_patch_edit_cart_number - PATCH_CART_NUMBER_LOWER_BOUND, 
                                        G_patch_edit_patch_number - PATCH_PATCH_NUMBER_LOWER_BOUND, 
                                        m, val);
      }
    }
    else if (pr->type == LAYOUT_PARAM_TYPE_ARROWS)
    {
      if (CONTROLS_MOUSE_CURSOR_IS_OVER_CART_PARAM_ARROWS_LEFT() && 
          CONTROLS_MOUSE_BUTTON_IS_PRESSED_BY_NAME(LEFT))
      {
        controls_cart_parameter_adjust( G_patch_edit_cart_number - PATCH_CART_NUMBER_LOWER_BOUND, 
                                        G_patch_edit_patch_number - PATCH_PATCH_NUMBER_LOWER_BOUND, 
                                        m, -1);
      }
      else if ( CONTROLS_MOUSE_CURSOR_IS_OVER_CART_PARAM_ARROWS_RIGHT() && 
                CONTROLS_MOUSE_BUTTON_IS_PRESSED_BY_NAME(LEFT))
      {
        controls_cart_parameter_adjust( G_patch_edit_cart_number - PATCH_CART_NUMBER_LOWER_BOUND, 
                                        G_patch_edit_patch_number - PATCH_PATCH_NUMBER_LOWER_BOUND, 
                                        m, 1);
      }
    }
    else if ( (pr->type == LAYOUT_PARAM_TYPE_RADIO) || 
              (pr->type == LAYOUT_PARAM_TYPE_FLAG))
    {
      if (CONTROLS_MOUSE_CURSOR_IS_OVER_CART_PARAM_RADIO_BUTTON() && 
          CONTROLS_MOUSE_BUTTON_IS_PRESSED_BY_NAME(LEFT))
      {
        controls_cart_parameter_adjust( G_patch_edit_cart_number - PATCH_CART_NUMBER_LOWER_BOUND, 
                                        G_patch_edit_patch_number - PATCH_PATCH_NUMBER_LOWER_BOUND, 
                                        m, 0);
      }
    }
  }

  return 0;
}

/*******************************************************************************
** controls_process_user_input_audition_panel()
*******************************************************************************/
short int controls_process_user_input_audition_panel()
{
  int m;

  param* pr;

  int val;

  /* audition parameters */
  for (m = 0; m < LAYOUT_NUM_AUDITION_PARAMS; m++)
  {
    pr = &G_layout_audition_params[m];

    if (pr->type == LAYOUT_PARAM_TYPE_SLIDER)
    {
      if (CONTROLS_MOUSE_LAST_CLICK_WAS_OVER_AUDITION_PARAM_SLIDER() && 
          CONTROLS_MOUSE_BUTTON_IS_ON_OR_PRESSED_BY_NAME(LEFT))
      {
        /* (new_val - lower_bound) / (upper_bound - lower_bound) = (mouse_x - SLIDER_POS_LOWER_BOUND) / SLIDER_WIDTH */
        val = S_mouse_remapped_pos_x - 2 * CONTROLS_PARAM_SLIDER_MOUSE_X_BASE;
        val *= pr->upper_bound - pr->lower_bound;
        val /= 2 * CONTROLS_PARAM_SLIDER_MOUSE_X_BOUND;
        val += pr->lower_bound;

        if (val < pr->lower_bound)
          val = pr->lower_bound;
        else if (val > pr->upper_bound)
          val = pr->upper_bound;

        controls_audition_parameter_adjust(m, val);
      }
    }
  }

  return 0;
}

/*******************************************************************************
** controls_process_user_input_text_entry()
*******************************************************************************/
short int controls_process_user_input_text_entry()
{
  int m;

  text_box* t;

  char c;

  /* initialize character */
  c = '\0';

  /* letters */
  for ( m = CONTROLS_KEY_INDEX_TEXT_ENTRY_A; 
        m <= CONTROLS_KEY_INDEX_TEXT_ENTRY_Z; 
        m++)
  {
    if (CONTROLS_KEY_IS_PRESSED(m))
    {
      if (CONTROLS_KEY_IS_ON_OR_PRESSED_BY_NAME(TEXT_ENTRY_LEFT_SHIFT) || 
          CONTROLS_KEY_IS_ON_OR_PRESSED_BY_NAME(TEXT_ENTRY_RIGHT_SHIFT))
      {
        c = 'A' + m - CONTROLS_KEY_INDEX_TEXT_ENTRY_A;
      }
      else
        c = 'a' + m - CONTROLS_KEY_INDEX_TEXT_ENTRY_A;
    }
  }

  /* digits */
  for ( m = CONTROLS_KEY_INDEX_TEXT_ENTRY_0; 
        m <= CONTROLS_KEY_INDEX_TEXT_ENTRY_9; 
        m++)
  {
    if (CONTROLS_KEY_IS_PRESSED(m))
    {
      c = '0' + m - CONTROLS_KEY_INDEX_TEXT_ENTRY_0;
    }
  }

  /* punctuation */
  if (CONTROLS_KEY_IS_PRESSED_BY_NAME(TEXT_ENTRY_SPACE))
    c = ' ';
  else if (CONTROLS_KEY_IS_PRESSED_BY_NAME(TEXT_ENTRY_PERIOD))
    c = '.';
  else if (CONTROLS_KEY_IS_PRESSED_BY_NAME(TEXT_ENTRY_HYPHEN))
  {
    if (CONTROLS_KEY_IS_ON_OR_PRESSED_BY_NAME(TEXT_ENTRY_LEFT_SHIFT) || 
        CONTROLS_KEY_IS_ON_OR_PRESSED_BY_NAME(TEXT_ENTRY_RIGHT_SHIFT))
    {
      c = '_';
    }
    else
      c = '-';
  }
  else if (CONTROLS_KEY_IS_PRESSED_BY_NAME(TEXT_ENTRY_BACKSPACE))
    c = '\b';

  /* cart screen text boxes */
  if ((G_program_screen == PROGRAM_SCREEN_TEXT_ENTRY_CART_NAME) || 
      (G_program_screen == PROGRAM_SCREEN_TEXT_ENTRY_PATCH_NAME))
  {
    /* key presses */
    if (CONTROLS_KEY_IS_PRESSED_BY_NAME(COMMON_ESCAPE))
      program_set_screen(PROGRAM_SCREEN_CART);
    else if (CONTROLS_KEY_IS_PRESSED_BY_NAME(TEXT_ENTRY_ENTER))
      program_set_screen(PROGRAM_SCREEN_CART);

    /* mouse clicks */
    for (m = 0; m < LAYOUT_NUM_CART_TEXT_BOXES; m++)
    {
      t = &G_layout_cart_text_boxes[m];

      /* if the mouse was clicked outside the current text box, */
      /* deselect the text box and go back to the cart screen   */
      if ((!(CONTROLS_MOUSE_CURSOR_IS_OVER_CART_TEXT_BOX())) && 
          CONTROLS_MOUSE_BUTTON_IS_PRESSED_BY_NAME(LEFT))
      {
        if ((m == LAYOUT_CART_TEXT_BOX_CART_NAME) && 
            (G_program_screen == PROGRAM_SCREEN_TEXT_ENTRY_CART_NAME))
        {
          program_set_screen(PROGRAM_SCREEN_CART);
          break;
        }

        if ((m == LAYOUT_CART_TEXT_BOX_PATCH_NAME) && 
            (G_program_screen == PROGRAM_SCREEN_TEXT_ENTRY_PATCH_NAME))
        {
          program_set_screen(PROGRAM_SCREEN_CART);
          break;
        }
      }
    }
  }

  /* update text box string */
  if (c != '\0')
  {
    controls_cart_text_entry( G_patch_edit_cart_number - PATCH_CART_NUMBER_LOWER_BOUND, 
                              G_patch_edit_patch_number - PATCH_PATCH_NUMBER_LOWER_BOUND, 
                              c);
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
  controls_process_user_input_top_panel();

  /* cart screen */
  if (G_program_screen == PROGRAM_SCREEN_CART)
  {
    controls_process_user_input_vertical_scrollbar();
    controls_process_user_input_cart_screen();
    controls_process_user_input_audition_panel();
  }
  /* text entry (on cart screen) */
  else if ( (G_program_screen == PROGRAM_SCREEN_TEXT_ENTRY_CART_NAME) || 
            (G_program_screen == PROGRAM_SCREEN_TEXT_ENTRY_PATCH_NAME))
  {
    controls_process_user_input_text_entry();
  }

  /* remove edges */
  controls_remove_edges();

  /* reset mouse wheel movement */
  S_mouse_wheel_movement = 0;

  return 0;
}

