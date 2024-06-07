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
#include "progloop.h"
#include "screen.h"
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
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_C, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_C_SHARP, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_D, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_D_SHARP, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_E, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_F, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_F_SHARP, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_G, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_G_SHARP, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_A, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_A_SHARP, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_B, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_C, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_C_SHARP, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_D, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_D_SHARP, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_E, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_F, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_F_SHARP, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_G, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_G_SHARP, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_A, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_A_SHARP, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_B, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_TWO_C, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_TWO_C_SHARP, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_TWO_D, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_TWO_D_SHARP, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_TWO_E, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_PORTAMENTO_SWITCH, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_ARPEGGIO_SWITCH, 
  CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_SUSTAIN_PEDAL, 
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
  (S_key_states[CONTROLS_KEY_INDEX_##index] == CONTROLS_KEY_STATE_PRESSED)

#define CONTROLS_KEY_IS_RELEASED(index)                                        \
  (S_key_states[CONTROLS_KEY_INDEX_##index] == CONTROLS_KEY_STATE_RELEASED)

#define CONTROLS_KEY_IS_ON_OR_PRESSED(index)                                   \
  ( (S_key_states[CONTROLS_KEY_INDEX_##index] == CONTROLS_KEY_STATE_ON) ||     \
    (S_key_states[CONTROLS_KEY_INDEX_##index] == CONTROLS_KEY_STATE_PRESSED))

#define CONTROLS_KEY_IS_OFF_OR_RELEASED(index)                                 \
  ( (S_key_states[CONTROLS_KEY_INDEX_##index] == CONTROLS_KEY_STATE_OFF) ||    \
    (S_key_states[CONTROLS_KEY_INDEX_##index] == CONTROLS_KEY_STATE_RELEASED))

#define CONTROLS_KEYBOARD_KEY_PRESSED(scancode, index)                         \
  if ((code == scancode) && (CONTROLS_KEY_IS_OFF_OR_RELEASED(index)))          \
    S_key_states[CONTROLS_KEY_INDEX_##index] = CONTROLS_KEY_STATE_PRESSED;

#define CONTROLS_KEYBOARD_KEY_RELEASED(scancode, index)                        \
  if ((code == scancode) && (CONTROLS_KEY_IS_ON_OR_PRESSED(index)))            \
    S_key_states[CONTROLS_KEY_INDEX_##index] = CONTROLS_KEY_STATE_RELEASED;

#define CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(index)                               \
  if (CONTROLS_KEY_IS_PRESSED(index))                                                   \
  {                                                                                     \
    instrument_key_pressed( G_patch_edit_instrument_index,                              \
                            12 * G_patch_edit_octave + CONTROLS_KEY_INDEX_##index -     \
                            CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_C);    \
  }                                                                                     \
  else if (CONTROLS_KEY_IS_RELEASED(index))                                             \
  {                                                                                     \
    instrument_key_released(G_patch_edit_instrument_index,                              \
                            12 * G_patch_edit_octave + CONTROLS_KEY_INDEX_##index -     \
                            CONTROLS_KEY_INDEX_MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_C);    \
  }

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
#define CONTROLS_MOUSE_CURSOR_IS_OVER_BUTTON()                                 \
  CONTROLS_MOUSE_CURSOR_IS_OVER_SCREEN_REGION(remapped,                        \
                                              b->center_x, b->center_y,        \
                                              b->width, 2)

/* patch edit parameter silders, adjustment arrows, and radio buttons */
#define CONTROLS_MOUSE_CURSOR_IS_OVER_PATCH_PARAM_SLIDER()                                              \
  CONTROLS_MOUSE_CURSOR_IS_OVER_SCREEN_REGION(remapped,                                                 \
                                              (pr->center_x + LAYOUT_CART_PARAM_SLIDER_TRACK_X), pos_y, \
                                              LAYOUT_CART_PARAM_SLIDER_WIDTH, 1)

#define CONTROLS_MOUSE_LAST_CLICK_WAS_OVER_PATCH_PARAM_SLIDER()                                         \
  CONTROLS_MOUSE_CURSOR_IS_OVER_SCREEN_REGION(last_click,                                               \
                                              (pr->center_x + LAYOUT_CART_PARAM_SLIDER_TRACK_X), pos_y, \
                                              LAYOUT_CART_PARAM_SLIDER_WIDTH, 1)

#define CONTROLS_MOUSE_CURSOR_IS_OVER_PATCH_PARAM_ARROWS_LEFT()                                         \
  CONTROLS_MOUSE_CURSOR_IS_OVER_SCREEN_REGION(remapped,                                                 \
                                              (pr->center_x + LAYOUT_CART_PARAM_ARROWS_LEFT_X), pos_y,  \
                                              1, 1)

#define CONTROLS_MOUSE_CURSOR_IS_OVER_PATCH_PARAM_ARROWS_RIGHT()                                        \
  CONTROLS_MOUSE_CURSOR_IS_OVER_SCREEN_REGION(remapped,                                                 \
                                              (pr->center_x + LAYOUT_CART_PARAM_ARROWS_RIGHT_X), pos_y, \
                                              1, 1)

#define CONTROLS_MOUSE_CURSOR_IS_OVER_PATCH_PARAM_RADIO_BUTTON()                                        \
  CONTROLS_MOUSE_CURSOR_IS_OVER_SCREEN_REGION(remapped,                                                 \
                                              (pr->center_x + LAYOUT_CART_PARAM_RADIO_BUTTON_X), pos_y, \
                                              1, 1)

/* cart edit screen parameter adjust macros */
#define CONTROLS_PATCH_PARAM_ADJUST_CASE(name, param)                          \
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
** controls_patch_parameter_adjust()
*******************************************************************************/
short int controls_patch_parameter_adjust(int cart_index, int patch_index, 
                                          int param_index)
{
  int m;

  cart* cr;
  patch* pt;

  param* pr;

  short int val;
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

  /* determine parameter adjustment value */
  if (pr->type == LAYOUT_PARAM_TYPE_SLIDER)
  {
    /* (new_val - lower_bound) / (upper_bound - lower_bound) = (mouse_x - SLIDER_POS_LOWER_BOUND) / SLIDER_WIDTH */
    val = 
      S_mouse_remapped_pos_x - 2 * (LAYOUT_OVERSCAN_CENTER_X + pr->center_x + LAYOUT_CART_PARAM_SLIDER_TRACK_X - 4 * (LAYOUT_CART_PARAM_SLIDER_WIDTH - 1));

    val *= pr->upper_bound - pr->lower_bound;
    val /= 2 * (8 * (LAYOUT_CART_PARAM_SLIDER_WIDTH - 1));
    val += pr->lower_bound;

    if (val < pr->lower_bound)
      val = pr->lower_bound;
    else if (val > pr->upper_bound)
      val = pr->upper_bound;
  }
  else if (pr->type == LAYOUT_PARAM_TYPE_ARROWS)
  {
    if ((S_mouse_remapped_pos_x >= (2 * (LAYOUT_OVERSCAN_CENTER_X + pr->center_x + LAYOUT_CART_PARAM_ARROWS_LEFT_X - 4))) && 
        (S_mouse_remapped_pos_x <  (2 * (LAYOUT_OVERSCAN_CENTER_X + pr->center_x + LAYOUT_CART_PARAM_ARROWS_LEFT_X + 4))))
    {
      val = -1;
    }
    else if ( (S_mouse_remapped_pos_x >= (2 * (LAYOUT_OVERSCAN_CENTER_X + pr->center_x + LAYOUT_CART_PARAM_ARROWS_RIGHT_X - 4))) && 
              (S_mouse_remapped_pos_x <  (2 * (LAYOUT_OVERSCAN_CENTER_X + pr->center_x + LAYOUT_CART_PARAM_ARROWS_RIGHT_X + 4))))
    {
      val = 1;
    }
    else
      val = 0;
  }
  else
    val = 0;

  /* apply adjustment */
  switch (param_index)
  {
    CONTROLS_PATCH_PARAM_ADJUST_CASE(CART_NUMBER,   G_patch_edit_cart_number)
    CONTROLS_PATCH_PARAM_ADJUST_CASE(PATCH_NUMBER,  G_patch_edit_patch_number)

    CONTROLS_PATCH_PARAM_ADJUST_CASE(ALGORITHM, pt->algorithm)
    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_SYNC,  pt->osc_sync)

    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_1_WAVEFORM,  pt->osc_waveform[0])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_1_PHI,       pt->osc_phi[0])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_1_FREQ_MODE, pt->osc_freq_mode[0])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_1_MULTIPLE,  pt->osc_multiple[0])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_1_DIVISOR,   pt->osc_divisor[0])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_1_OCTAVE,    pt->osc_octave[0])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_1_NOTE,      pt->osc_note[0])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_1_DETUNE,    pt->osc_detune[0])

    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_1_ROUTING_VIBRATO,     pt->osc_routing[0])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_1_ROUTING_PITCH_ENV,   pt->osc_routing[0])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_1_ROUTING_PITCH_WHEEL, pt->osc_routing[0])

    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_2_WAVEFORM,  pt->osc_waveform[1])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_2_PHI,       pt->osc_phi[1])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_2_FREQ_MODE, pt->osc_freq_mode[1])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_2_MULTIPLE,  pt->osc_multiple[1])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_2_DIVISOR,   pt->osc_divisor[1])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_2_OCTAVE,    pt->osc_octave[1])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_2_NOTE,      pt->osc_note[1])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_2_DETUNE,    pt->osc_detune[1])

    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_2_ROUTING_VIBRATO,     pt->osc_routing[1])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_2_ROUTING_PITCH_ENV,   pt->osc_routing[1])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_2_ROUTING_PITCH_WHEEL, pt->osc_routing[1])

    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_3_WAVEFORM,  pt->osc_waveform[2])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_3_PHI,       pt->osc_phi[2])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_3_FREQ_MODE, pt->osc_freq_mode[2])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_3_MULTIPLE,  pt->osc_multiple[2])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_3_DIVISOR,   pt->osc_divisor[2])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_3_OCTAVE,    pt->osc_octave[2])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_3_NOTE,      pt->osc_note[2])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_3_DETUNE,    pt->osc_detune[2])

    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_3_ROUTING_VIBRATO,     pt->osc_routing[2])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_3_ROUTING_PITCH_ENV,   pt->osc_routing[2])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(OSC_3_ROUTING_PITCH_WHEEL, pt->osc_routing[2])

    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_1_ATTACK,      pt->env_attack[0])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_1_DECAY,       pt->env_decay[0])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_1_SUSTAIN,     pt->env_sustain[0])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_1_RELEASE,     pt->env_release[0])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_1_AMPLITUDE,   pt->env_amplitude[0])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_1_HOLD_LEVEL,  pt->env_hold_level[0])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_1_HOLD_MODE,   pt->env_hold_mode[0])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_1_RATE_KS,     pt->env_rate_ks[0])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_1_LEVEL_KS,    pt->env_level_ks[0])

    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_1_ROUTING_TREMOLO,   pt->env_routing[0])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_1_ROUTING_BOOST,     pt->env_routing[0])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_1_ROUTING_VELOCITY,  pt->env_routing[0])

    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_2_ATTACK,      pt->env_attack[1])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_2_DECAY,       pt->env_decay[1])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_2_SUSTAIN,     pt->env_sustain[1])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_2_RELEASE,     pt->env_release[1])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_2_AMPLITUDE,   pt->env_amplitude[1])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_2_HOLD_LEVEL,  pt->env_hold_level[1])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_2_HOLD_MODE,   pt->env_hold_mode[1])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_2_RATE_KS,     pt->env_rate_ks[1])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_2_LEVEL_KS,    pt->env_level_ks[1])

    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_2_ROUTING_TREMOLO,   pt->env_routing[1])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_2_ROUTING_BOOST,     pt->env_routing[1])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_2_ROUTING_VELOCITY,  pt->env_routing[1])

    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_3_ATTACK,      pt->env_attack[2])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_3_DECAY,       pt->env_decay[2])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_3_SUSTAIN,     pt->env_sustain[2])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_3_RELEASE,     pt->env_release[2])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_3_AMPLITUDE,   pt->env_amplitude[2])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_3_HOLD_LEVEL,  pt->env_hold_level[2])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_3_HOLD_MODE,   pt->env_hold_mode[2])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_3_RATE_KS,     pt->env_rate_ks[2])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_3_LEVEL_KS,    pt->env_level_ks[2])

    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_3_ROUTING_TREMOLO,   pt->env_routing[2])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_3_ROUTING_BOOST,     pt->env_routing[2])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ENV_3_ROUTING_VELOCITY,  pt->env_routing[2])

    CONTROLS_PATCH_PARAM_ADJUST_CASE(VIBRATO_WAVEFORM,    pt->lfo_waveform[0])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(VIBRATO_DELAY,       pt->lfo_delay[0])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(VIBRATO_SPEED,       pt->lfo_speed[0])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(VIBRATO_DEPTH,       pt->lfo_depth[0])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(VIBRATO_SENSITIVITY, pt->lfo_sensitivity[0])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(VIBRATO_SYNC,        pt->lfo_sync[0])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(VIBRATO_POLARITY,    pt->lfo_polarity[0])

    CONTROLS_PATCH_PARAM_ADJUST_CASE(TREMOLO_WAVEFORM,    pt->lfo_waveform[1])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(TREMOLO_DELAY,       pt->lfo_delay[1])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(TREMOLO_SPEED,       pt->lfo_speed[1])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(TREMOLO_DEPTH,       pt->lfo_depth[1])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(TREMOLO_SENSITIVITY, pt->lfo_sensitivity[1])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(TREMOLO_SYNC,        pt->lfo_sync[1])

    CONTROLS_PATCH_PARAM_ADJUST_CASE(CHORUS_WAVEFORM,     pt->lfo_waveform[2])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(CHORUS_DELAY,        pt->lfo_delay[2])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(CHORUS_SPEED,        pt->lfo_speed[2])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(CHORUS_DEPTH,        pt->lfo_depth[2])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(CHORUS_SENSITIVITY,  pt->lfo_sensitivity[2])
    CONTROLS_PATCH_PARAM_ADJUST_CASE(CHORUS_SYNC,         pt->lfo_sync[2])

    CONTROLS_PATCH_PARAM_ADJUST_CASE(BOOST_SENSITIVITY,     pt->boost_sensitivity)
    CONTROLS_PATCH_PARAM_ADJUST_CASE(VELOCITY_SENSITIVITY,  pt->velocity_sensitivity)

    CONTROLS_PATCH_PARAM_ADJUST_CASE(FILTERS_HIGHPASS,  pt->highpass_cutoff)
    CONTROLS_PATCH_PARAM_ADJUST_CASE(FILTERS_LOWPASS,   pt->lowpass_cutoff)

    CONTROLS_PATCH_PARAM_ADJUST_CASE(PITCH_ENV_ATTACK,  pt->peg_attack)
    CONTROLS_PATCH_PARAM_ADJUST_CASE(PITCH_ENV_DECAY,   pt->peg_decay)
    CONTROLS_PATCH_PARAM_ADJUST_CASE(PITCH_ENV_RELEASE, pt->peg_release)
    CONTROLS_PATCH_PARAM_ADJUST_CASE(PITCH_ENV_MAXIMUM, pt->peg_maximum)
    CONTROLS_PATCH_PARAM_ADJUST_CASE(PITCH_ENV_FINALE,  pt->peg_finale)

    CONTROLS_PATCH_PARAM_ADJUST_CASE(PITCH_WHEEL_MODE,  pt->pitch_wheel_mode)
    CONTROLS_PATCH_PARAM_ADJUST_CASE(PITCH_WHEEL_RANGE, pt->pitch_wheel_range)

    CONTROLS_PATCH_PARAM_ADJUST_CASE(ARPEGGIO_MODE,     pt->arpeggio_mode)
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ARPEGGIO_PATTERN,  pt->arpeggio_pattern)
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ARPEGGIO_OCTAVES,  pt->arpeggio_octaves)
    CONTROLS_PATCH_PARAM_ADJUST_CASE(ARPEGGIO_SPEED,    pt->arpeggio_speed)

    CONTROLS_PATCH_PARAM_ADJUST_CASE(PORTAMENTO_MODE,   pt->portamento_mode)
    CONTROLS_PATCH_PARAM_ADJUST_CASE(PORTAMENTO_LEGATO, pt->portamento_legato)
    CONTROLS_PATCH_PARAM_ADJUST_CASE(PORTAMENTO_SPEED,  pt->portamento_speed)

    CONTROLS_PATCH_PARAM_ADJUST_CASE(MOD_WHEEL_ROUTING_VIBRATO,   pt->mod_wheel_routing)
    CONTROLS_PATCH_PARAM_ADJUST_CASE(MOD_WHEEL_ROUTING_TREMOLO,   pt->mod_wheel_routing)
    CONTROLS_PATCH_PARAM_ADJUST_CASE(MOD_WHEEL_ROUTING_BOOST,     pt->mod_wheel_routing)
    CONTROLS_PATCH_PARAM_ADJUST_CASE(MOD_WHEEL_ROUTING_CHORUS,    pt->mod_wheel_routing)

    CONTROLS_PATCH_PARAM_ADJUST_CASE(AFTERTOUCH_ROUTING_VIBRATO,  pt->aftertouch_routing)
    CONTROLS_PATCH_PARAM_ADJUST_CASE(AFTERTOUCH_ROUTING_TREMOLO,  pt->aftertouch_routing)
    CONTROLS_PATCH_PARAM_ADJUST_CASE(AFTERTOUCH_ROUTING_BOOST,    pt->aftertouch_routing)
    CONTROLS_PATCH_PARAM_ADJUST_CASE(AFTERTOUCH_ROUTING_CHORUS,   pt->aftertouch_routing)

    CONTROLS_PATCH_PARAM_ADJUST_CASE(EXP_PEDAL_ROUTING_VIBRATO,   pt->exp_pedal_routing)
    CONTROLS_PATCH_PARAM_ADJUST_CASE(EXP_PEDAL_ROUTING_TREMOLO,   pt->exp_pedal_routing)
    CONTROLS_PATCH_PARAM_ADJUST_CASE(EXP_PEDAL_ROUTING_BOOST,     pt->exp_pedal_routing)
    CONTROLS_PATCH_PARAM_ADJUST_CASE(EXP_PEDAL_ROUTING_CHORUS,    pt->exp_pedal_routing)

    CONTROLS_PATCH_PARAM_ADJUST_CASE(AUDITION_OCTAVE,       G_patch_edit_octave)
    CONTROLS_PATCH_PARAM_ADJUST_CASE(AUDITION_VELOCITY,     G_patch_edit_note_velocity)
    CONTROLS_PATCH_PARAM_ADJUST_CASE(AUDITION_PITCH_WHEEL,  G_patch_edit_pitch_wheel_pos)
    CONTROLS_PATCH_PARAM_ADJUST_CASE(AUDITION_MOD_WHEEL,    G_patch_edit_mod_wheel_pos)
    CONTROLS_PATCH_PARAM_ADJUST_CASE(AUDITION_AFTERTOUCH,   G_patch_edit_aftertouch_pos)
    CONTROLS_PATCH_PARAM_ADJUST_CASE(AUDITION_EXP_PEDAL,    G_patch_edit_exp_pedal_pos)

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
    else if (param_index == LAYOUT_CART_PARAM_AUDITION_OCTAVE)
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
    else if (param_index == LAYOUT_CART_PARAM_AUDITION_VELOCITY)
      instrument_set_note_velocity(G_patch_edit_instrument_index, G_patch_edit_note_velocity);
    else if (param_index == LAYOUT_CART_PARAM_AUDITION_PITCH_WHEEL)
      instrument_set_pitch_wheel_position(G_patch_edit_instrument_index, G_patch_edit_pitch_wheel_pos);
    else if (param_index == LAYOUT_CART_PARAM_AUDITION_MOD_WHEEL)
      instrument_set_mod_wheel_position(G_patch_edit_instrument_index, G_patch_edit_mod_wheel_pos);
    else if (param_index == LAYOUT_CART_PARAM_AUDITION_AFTERTOUCH)
      instrument_set_aftertouch_position(G_patch_edit_instrument_index, G_patch_edit_aftertouch_pos);
    else if (param_index == LAYOUT_CART_PARAM_AUDITION_EXP_PEDAL)
      instrument_set_exp_pedal_position(G_patch_edit_instrument_index, G_patch_edit_exp_pedal_pos);
    else
    {
      instrument_load_patch(G_patch_edit_instrument_index, 
                            cart_index, patch_index);
    }
  }

  return 0;
}

/*******************************************************************************
** controls_keyboard_key_pressed()
*******************************************************************************/
short int controls_keyboard_key_pressed(SDL_Scancode code)
{
  /* common keys */
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_ESCAPE,  COMMON_ESCAPE)

  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_LEFTBRACKET,   COMMON_DECREASE_WINDOW_SIZE)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_RIGHTBRACKET,  COMMON_INCREASE_WINDOW_SIZE)

  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_UP,    COMMON_SCROLL_UP)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_DOWN,  COMMON_SCROLL_DOWN)

  /* musical keyboard keys */
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_Z,         MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_C)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_S,         MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_C_SHARP)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_X,         MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_D)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_D,         MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_D_SHARP)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_C,         MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_E)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_V,         MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_F)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_G,         MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_F_SHARP)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_B,         MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_G)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_H,         MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_G_SHARP)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_N,         MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_A)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_J,         MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_A_SHARP)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_M,         MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_B)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_COMMA,     MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_C)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_L,         MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_C_SHARP)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_PERIOD,    MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_D)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_SEMICOLON, MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_D_SHARP)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_SLASH,     MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_E)

  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_Q,         MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_C)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_2,         MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_C_SHARP)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_W,         MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_D)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_3,         MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_D_SHARP)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_E,         MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_E)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_R,         MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_F)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_5,         MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_F_SHARP)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_T,         MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_G)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_6,         MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_G_SHARP)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_Y,         MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_A)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_7,         MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_A_SHARP)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_U,         MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_B)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_I,         MUSICAL_KEYBOARD_OCTAVE_PLUS_TWO_C)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_9,         MUSICAL_KEYBOARD_OCTAVE_PLUS_TWO_C_SHARP)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_O,         MUSICAL_KEYBOARD_OCTAVE_PLUS_TWO_D)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_0,         MUSICAL_KEYBOARD_OCTAVE_PLUS_TWO_D_SHARP)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_P,         MUSICAL_KEYBOARD_OCTAVE_PLUS_TWO_E)

  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_LSHIFT,    MUSICAL_KEYBOARD_PORTAMENTO_SWITCH)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_RSHIFT,    MUSICAL_KEYBOARD_ARPEGGIO_SWITCH)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_SPACE,     MUSICAL_KEYBOARD_SUSTAIN_PEDAL)

  /* text entry keys */
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_A,       TEXT_ENTRY_A)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_B,       TEXT_ENTRY_B)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_C,       TEXT_ENTRY_C)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_D,       TEXT_ENTRY_D)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_E,       TEXT_ENTRY_E)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_F,       TEXT_ENTRY_F)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_G,       TEXT_ENTRY_G)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_H,       TEXT_ENTRY_H)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_I,       TEXT_ENTRY_I)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_J,       TEXT_ENTRY_J)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_K,       TEXT_ENTRY_K)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_L,       TEXT_ENTRY_L)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_M,       TEXT_ENTRY_M)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_N,       TEXT_ENTRY_N)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_O,       TEXT_ENTRY_O)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_P,       TEXT_ENTRY_P)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_Q,       TEXT_ENTRY_Q)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_R,       TEXT_ENTRY_R)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_S,       TEXT_ENTRY_S)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_T,       TEXT_ENTRY_T)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_U,       TEXT_ENTRY_U)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_V,       TEXT_ENTRY_V)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_W,       TEXT_ENTRY_W)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_X,       TEXT_ENTRY_X)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_Y,       TEXT_ENTRY_Y)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_Z,       TEXT_ENTRY_Z)

  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_0,       TEXT_ENTRY_0)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_1,       TEXT_ENTRY_1)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_2,       TEXT_ENTRY_2)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_3,       TEXT_ENTRY_3)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_4,       TEXT_ENTRY_4)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_5,       TEXT_ENTRY_5)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_6,       TEXT_ENTRY_6)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_7,       TEXT_ENTRY_7)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_8,       TEXT_ENTRY_8)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_9,       TEXT_ENTRY_9)

  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_SPACE,   TEXT_ENTRY_SPACE)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_PERIOD,  TEXT_ENTRY_PERIOD)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_MINUS,   TEXT_ENTRY_HYPHEN)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_RETURN,  TEXT_ENTRY_ENTER)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_LSHIFT,  TEXT_ENTRY_LEFT_SHIFT)
  CONTROLS_KEYBOARD_KEY_PRESSED(SDL_SCANCODE_RSHIFT,  TEXT_ENTRY_RIGHT_SHIFT)

  return 0;
}

/*******************************************************************************
** controls_keyboard_key_released()
*******************************************************************************/
short int controls_keyboard_key_released(SDL_Scancode code)
{
  /* common keys */
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_ESCAPE, COMMON_ESCAPE)

  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_LEFTBRACKET,  COMMON_DECREASE_WINDOW_SIZE)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_RIGHTBRACKET, COMMON_INCREASE_WINDOW_SIZE)

  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_UP,   COMMON_SCROLL_UP)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_DOWN, COMMON_SCROLL_DOWN)

  /* musical keyboard keys */
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_Z,          MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_C)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_S,          MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_C_SHARP)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_X,          MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_D)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_D,          MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_D_SHARP)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_C,          MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_E)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_V,          MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_F)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_G,          MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_F_SHARP)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_B,          MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_G)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_H,          MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_G_SHARP)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_N,          MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_A)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_J,          MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_A_SHARP)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_M,          MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_B)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_COMMA,      MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_C)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_L,          MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_C_SHARP)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_PERIOD,     MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_D)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_SEMICOLON,  MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_D_SHARP)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_SLASH,      MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_E)

  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_Q,          MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_C)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_2,          MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_C_SHARP)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_W,          MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_D)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_3,          MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_D_SHARP)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_E,          MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_E)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_R,          MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_F)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_5,          MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_F_SHARP)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_T,          MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_G)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_6,          MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_G_SHARP)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_Y,          MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_A)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_7,          MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_A_SHARP)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_U,          MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_B)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_I,          MUSICAL_KEYBOARD_OCTAVE_PLUS_TWO_C)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_9,          MUSICAL_KEYBOARD_OCTAVE_PLUS_TWO_C_SHARP)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_O,          MUSICAL_KEYBOARD_OCTAVE_PLUS_TWO_D)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_0,          MUSICAL_KEYBOARD_OCTAVE_PLUS_TWO_D_SHARP)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_P,          MUSICAL_KEYBOARD_OCTAVE_PLUS_TWO_E)

  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_LSHIFT,     MUSICAL_KEYBOARD_PORTAMENTO_SWITCH)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_RSHIFT,     MUSICAL_KEYBOARD_ARPEGGIO_SWITCH)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_SPACE,      MUSICAL_KEYBOARD_SUSTAIN_PEDAL)

  /* text entry keys */
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_A,      TEXT_ENTRY_A)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_B,      TEXT_ENTRY_B)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_C,      TEXT_ENTRY_C)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_D,      TEXT_ENTRY_D)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_E,      TEXT_ENTRY_E)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_F,      TEXT_ENTRY_F)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_G,      TEXT_ENTRY_G)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_H,      TEXT_ENTRY_H)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_I,      TEXT_ENTRY_I)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_J,      TEXT_ENTRY_J)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_K,      TEXT_ENTRY_K)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_L,      TEXT_ENTRY_L)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_M,      TEXT_ENTRY_M)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_N,      TEXT_ENTRY_N)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_O,      TEXT_ENTRY_O)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_P,      TEXT_ENTRY_P)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_Q,      TEXT_ENTRY_Q)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_R,      TEXT_ENTRY_R)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_S,      TEXT_ENTRY_S)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_T,      TEXT_ENTRY_T)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_U,      TEXT_ENTRY_U)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_V,      TEXT_ENTRY_V)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_W,      TEXT_ENTRY_W)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_X,      TEXT_ENTRY_X)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_Y,      TEXT_ENTRY_Y)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_Z,      TEXT_ENTRY_Z)

  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_0,      TEXT_ENTRY_0)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_1,      TEXT_ENTRY_1)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_2,      TEXT_ENTRY_2)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_3,      TEXT_ENTRY_3)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_4,      TEXT_ENTRY_4)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_5,      TEXT_ENTRY_5)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_6,      TEXT_ENTRY_6)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_7,      TEXT_ENTRY_7)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_8,      TEXT_ENTRY_8)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_9,      TEXT_ENTRY_9)

  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_SPACE,  TEXT_ENTRY_SPACE)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_PERIOD, TEXT_ENTRY_PERIOD)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_MINUS,  TEXT_ENTRY_HYPHEN)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_RETURN, TEXT_ENTRY_ENTER)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_LSHIFT, TEXT_ENTRY_LEFT_SHIFT)
  CONTROLS_KEYBOARD_KEY_RELEASED(SDL_SCANCODE_RSHIFT, TEXT_ENTRY_RIGHT_SHIFT)

  return 0;
}

/*******************************************************************************
** controls_mouse_button_pressed()
*******************************************************************************/
short int controls_mouse_button_pressed(Uint8 button, Sint32 x, Sint32 y)
{
  /* find the mouse button location in remapped coordinates (448p) */
  S_mouse_remapped_pos_x = (x * 2 * GRAPHICS_OVERSCAN_WIDTH) / G_viewport_w;
  S_mouse_remapped_pos_y = (y * 2 * GRAPHICS_OVERSCAN_HEIGHT) / G_viewport_h;

  /* left mouse button */
  if ((button == SDL_BUTTON_LEFT) && 
      (CONTROLS_MOUSE_BUTTON_IS_OFF_OR_RELEASED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT)))
  {
    S_mouse_button_states[CONTROLS_MOUSE_BUTTON_INDEX_LEFT] = CONTROLS_MOUSE_BUTTON_STATE_PRESSED;

    S_mouse_last_click_pos_x = S_mouse_remapped_pos_x;
    S_mouse_last_click_pos_y = S_mouse_remapped_pos_y;
  }

  /* right mouse button */
  if ((button == SDL_BUTTON_RIGHT) && 
      (CONTROLS_MOUSE_BUTTON_IS_OFF_OR_RELEASED(CONTROLS_MOUSE_BUTTON_INDEX_RIGHT)))
  {
    S_mouse_button_states[CONTROLS_MOUSE_BUTTON_INDEX_RIGHT] = CONTROLS_MOUSE_BUTTON_STATE_PRESSED;
  }

  return 0;
}

/*******************************************************************************
** controls_mouse_button_released()
*******************************************************************************/
short int controls_mouse_button_released(Uint8 button)
{
  /* left mouse button */
  if ((button == SDL_BUTTON_LEFT) && 
      (CONTROLS_MOUSE_BUTTON_IS_ON_OR_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT)))
  {
    S_mouse_button_states[CONTROLS_MOUSE_BUTTON_INDEX_LEFT] = CONTROLS_MOUSE_BUTTON_STATE_RELEASED;
  }

  /* right mouse button */
  if ((button == SDL_BUTTON_RIGHT) && 
      (CONTROLS_MOUSE_BUTTON_IS_ON_OR_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_RIGHT)))
  {
    S_mouse_button_states[CONTROLS_MOUSE_BUTTON_INDEX_RIGHT] = CONTROLS_MOUSE_BUTTON_STATE_RELEASED;
  }

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
  int i;

  for (i = 0; i < CONTROLS_NUM_KEY_INDICES; i++)
  {
    if (S_key_states[i] == CONTROLS_KEY_STATE_PRESSED)
      S_key_states[i] = CONTROLS_KEY_STATE_ON;

    if (S_key_states[i] == CONTROLS_KEY_STATE_RELEASED)
      S_key_states[i] = CONTROLS_KEY_STATE_OFF;
  }

  for (i = 0; i < CONTROLS_NUM_MOUSE_BUTTON_INDICES; i++)
  {
    if (S_mouse_button_states[i] == CONTROLS_MOUSE_BUTTON_STATE_PRESSED)
      S_mouse_button_states[i] = CONTROLS_MOUSE_BUTTON_STATE_ON;

    if (S_mouse_button_states[i] == CONTROLS_MOUSE_BUTTON_STATE_RELEASED)
      S_mouse_button_states[i] = CONTROLS_MOUSE_BUTTON_STATE_OFF;
  }

  return 0;
}

/*******************************************************************************
** controls_process_user_input_standard()
*******************************************************************************/
short int controls_process_user_input_standard()
{
  int m;

  button* b;
  param*  pr;

  int pos_y;

  /* check key states */

  /* decrease window size */
  if (CONTROLS_KEY_IS_PRESSED(COMMON_DECREASE_WINDOW_SIZE))
    graphics_decrease_window_size();

  /* increase window size */
  if (CONTROLS_KEY_IS_PRESSED(COMMON_INCREASE_WINDOW_SIZE))
    graphics_increase_window_size();

  /* scroll up */
  if (CONTROLS_KEY_IS_PRESSED(COMMON_SCROLL_UP))
  {
    G_current_scroll_amount -= 2;

    if (G_current_scroll_amount < 0)
      G_current_scroll_amount = 0;
  }

  /* scroll down */
  if (CONTROLS_KEY_IS_PRESSED(COMMON_SCROLL_DOWN))
  {
    G_current_scroll_amount += 2;

    if (G_current_scroll_amount > G_max_scroll_amount)
      G_current_scroll_amount = G_max_scroll_amount;
  }

  /* cart screen */
  if (G_prog_screen == PROGRAM_SCREEN_CART)
  {
    /* notes */
    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_C)
    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_C_SHARP)
    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_D)
    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_D_SHARP)
    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_E)
    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_F)
    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_F_SHARP)
    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_G)
    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_G_SHARP)
    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_A)
    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_A_SHARP)
    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_ZERO_B)

    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_C)
    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_C_SHARP)
    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_D)
    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_D_SHARP)
    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_E)
    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_F)
    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_F_SHARP)
    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_G)
    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_G_SHARP)
    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_A)
    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_A_SHARP)
    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_ONE_B)

    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_TWO_C)
    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_TWO_C_SHARP)
    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_TWO_D)
    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_TWO_D_SHARP)
    CONTROLS_KEYBOARD_NOTE_PRESSED_OR_RELEASED(MUSICAL_KEYBOARD_OCTAVE_PLUS_TWO_E)

    /* portamento / arpeggio switches */
    if (CONTROLS_KEY_IS_PRESSED(MUSICAL_KEYBOARD_PORTAMENTO_SWITCH)   || 
        CONTROLS_KEY_IS_PRESSED(MUSICAL_KEYBOARD_ARPEGGIO_SWITCH)     || 
        CONTROLS_KEY_IS_RELEASED(MUSICAL_KEYBOARD_PORTAMENTO_SWITCH)  || 
        CONTROLS_KEY_IS_RELEASED(MUSICAL_KEYBOARD_ARPEGGIO_SWITCH))
    {
      if (CONTROLS_KEY_IS_ON_OR_PRESSED(MUSICAL_KEYBOARD_PORTAMENTO_SWITCH) && 
          CONTROLS_KEY_IS_ON_OR_PRESSED(MUSICAL_KEYBOARD_ARPEGGIO_SWITCH))
      {
        instrument_set_port_arp_switch(G_patch_edit_instrument_index, MIDI_CONT_PORT_ARP_SWITCH_OFF);
      }
      else if ( CONTROLS_KEY_IS_ON_OR_PRESSED(MUSICAL_KEYBOARD_PORTAMENTO_SWITCH) && 
                CONTROLS_KEY_IS_OFF_OR_RELEASED(MUSICAL_KEYBOARD_ARPEGGIO_SWITCH))
      {
        instrument_set_port_arp_switch(G_patch_edit_instrument_index, MIDI_CONT_PORT_ARP_SWITCH_PORTAMENTO);
      }
      else if ( CONTROLS_KEY_IS_OFF_OR_RELEASED(MUSICAL_KEYBOARD_PORTAMENTO_SWITCH) && 
                CONTROLS_KEY_IS_ON_OR_PRESSED(MUSICAL_KEYBOARD_ARPEGGIO_SWITCH))
      {
        instrument_set_port_arp_switch(G_patch_edit_instrument_index, MIDI_CONT_PORT_ARP_SWITCH_ARPEGGIO);
      }
      else
        instrument_set_port_arp_switch(G_patch_edit_instrument_index, MIDI_CONT_PORT_ARP_SWITCH_OFF);
    }

    /* sustain pedal */
    if (CONTROLS_KEY_IS_PRESSED(MUSICAL_KEYBOARD_SUSTAIN_PEDAL))
      instrument_set_sustain_pedal(G_patch_edit_instrument_index, MIDI_CONT_SUSTAIN_PEDAL_DOWN);
    else if (CONTROLS_KEY_IS_RELEASED(MUSICAL_KEYBOARD_SUSTAIN_PEDAL))
      instrument_set_sustain_pedal(G_patch_edit_instrument_index, MIDI_CONT_SUSTAIN_PEDAL_UP);
  }

  /* check mouse button clicks */

  /* check top panel mouse button clicks */
  if (CONTROLS_MOUSE_CURSOR_IS_OVER_TOP_PANEL())
  {
    for (m = 0; m < LAYOUT_NUM_TOP_PANEL_BUTTONS; m++)
    {
      b = &G_layout_top_panel_buttons[m];

      /* if the cursor is over this button, click it! */
      if (CONTROLS_MOUSE_CURSOR_IS_OVER_BUTTON())
      {
        if (CONTROLS_MOUSE_BUTTON_IS_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT))
        {
          layout_reset_top_panel_button_states();

          b->state = LAYOUT_BUTTON_STATE_ON;

          if (m == LAYOUT_TOP_PANEL_BUTTON_CART)
            program_loop_change_screen(PROGRAM_SCREEN_CART);
          else if (m == LAYOUT_TOP_PANEL_BUTTON_INSTRUMENTS)
            program_loop_change_screen(PROGRAM_SCREEN_INSTRUMENTS);
          else if (m == LAYOUT_TOP_PANEL_BUTTON_SONG)
            program_loop_change_screen(PROGRAM_SCREEN_SONG);
          else if (m == LAYOUT_TOP_PANEL_BUTTON_MIXER)
            program_loop_change_screen(PROGRAM_SCREEN_MIXER);
          else if (m == LAYOUT_TOP_PANEL_BUTTON_SOUND_FX)
            program_loop_change_screen(PROGRAM_SCREEN_SOUND_FX);
          else if (m == LAYOUT_TOP_PANEL_BUTTON_DPCM)
            program_loop_change_screen(PROGRAM_SCREEN_DPCM);
        }

        break;
      }
    }
  }

  /* cart screen */
  if (G_prog_screen == PROGRAM_SCREEN_CART)
  {
    /* check for scrollbar clicks */
    if (CONTROLS_MOUSE_CURSOR_IS_OVER_VERTICAL_SCROLLBAR_UP_ARROW() && 
        CONTROLS_MOUSE_BUTTON_IS_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT))
    {
      G_current_scroll_amount -= 8;

      if (G_current_scroll_amount < 0)
        G_current_scroll_amount = 0;
    }
    else if ( CONTROLS_MOUSE_CURSOR_IS_OVER_VERTICAL_SCROLLBAR_DOWN_ARROW() && 
              CONTROLS_MOUSE_BUTTON_IS_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT))
    {
      G_current_scroll_amount += 8;

      if (G_current_scroll_amount > G_max_scroll_amount)
        G_current_scroll_amount = G_max_scroll_amount;
    }
    else if ( CONTROLS_MOUSE_LAST_CLICK_WAS_OVER_VERTICAL_SCROLLBAR_TRACK() && 
              CONTROLS_MOUSE_BUTTON_IS_ON_OR_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT))
    {
      G_current_scroll_amount = G_max_scroll_amount;

      /* note that the actual scrollbar slider has a height of 2 cells, so    */
      /* we subtract 2 from the height of the scrollbar track when dividing.  */
      G_current_scroll_amount *= 
        (S_mouse_remapped_pos_y - LAYOUT_OVERSCAN_HEIGHT - (LAYOUT_VERT_SCROLLBAR_TRACK_Y - 8 * (LAYOUT_VERT_SCROLLBAR_TRACK_HEIGHT - 2))) / 2;
      G_current_scroll_amount /= 8 * (LAYOUT_VERT_SCROLLBAR_TRACK_HEIGHT - 2);

      if (G_current_scroll_amount < 0)
        G_current_scroll_amount = 0;

      if (G_current_scroll_amount > G_max_scroll_amount)
        G_current_scroll_amount = G_max_scroll_amount;
    }

    /* check for cart edit area clicks */
    if (CONTROLS_MOUSE_CURSOR_IS_OVER_CART_MAIN_AREA() || 
        CONTROLS_MOUSE_CURSOR_IS_OVER_CART_AUDITION_PANEL())
    {
      /* check for mouse wheel scrolling */
      if (S_mouse_wheel_movement != 0)
      {
        if (S_mouse_wheel_movement > 0)
          G_current_scroll_amount -= (S_mouse_wheel_movement / 2) + 4;
        else if (S_mouse_wheel_movement < 0)
          G_current_scroll_amount += (-S_mouse_wheel_movement / 2) + 4;

        if (G_current_scroll_amount < 0)
          G_current_scroll_amount = 0;
        else if (G_current_scroll_amount > G_max_scroll_amount)
          G_current_scroll_amount = G_max_scroll_amount;
      }

      /* check for button clicks */
      for (m = 0; m < LAYOUT_NUM_CART_BUTTONS; m++)
      {
        b = &G_layout_cart_buttons[m];

        /* determine vertical position for this button */
        pos_y = b->center_y - G_current_scroll_amount;

        if (LAYOUT_CART_BUTTON_OR_TEXT_BOX_IS_NOT_IN_MAIN_AREA(pos_y))
          continue;

        /* if the cursor is over this button, click it! */
        if (CONTROLS_MOUSE_CURSOR_IS_OVER_BUTTON())
        {
          if (CONTROLS_MOUSE_BUTTON_IS_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT))
          {
            layout_reset_cart_button_states();

            b->state = LAYOUT_BUTTON_STATE_ON;
          }
        }
      }

      /* check for parameter adjustment clicks */
      for (m = 0; m < LAYOUT_NUM_CART_PARAMS; m++)
      {
        pr = &G_layout_cart_params[m];

        /* determine vertical position for audition bar params (remain stationary) */
        if ((m == LAYOUT_CART_PARAM_AUDITION_OCTAVE)      || 
            (m == LAYOUT_CART_PARAM_AUDITION_VELOCITY)    || 
            (m == LAYOUT_CART_PARAM_AUDITION_PITCH_WHEEL) || 
            (m == LAYOUT_CART_PARAM_AUDITION_MOD_WHEEL)   || 
            (m == LAYOUT_CART_PARAM_AUDITION_AFTERTOUCH)  || 
            (m == LAYOUT_CART_PARAM_AUDITION_EXP_PEDAL))
        {
          pos_y = pr->center_y;
        }
        /* determine vertical position for other params (can be scrolled up/down) */
        else
        {
          pos_y = pr->center_y - G_current_scroll_amount;

          if (LAYOUT_CART_HEADER_OR_PARAM_IS_NOT_IN_MAIN_AREA(pos_y))
            continue;
        }

        /* if the cursor is over this parameter, adjust it! */
        if (pr->type == LAYOUT_PARAM_TYPE_SLIDER)
        {
          if (CONTROLS_MOUSE_LAST_CLICK_WAS_OVER_PATCH_PARAM_SLIDER() && 
              CONTROLS_MOUSE_BUTTON_IS_ON_OR_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT))
          {
            controls_patch_parameter_adjust(G_patch_edit_cart_number - PATCH_CART_NUMBER_LOWER_BOUND, 
                                            G_patch_edit_patch_number - PATCH_PATCH_NUMBER_LOWER_BOUND, 
                                            m);
          }
        }
        else if (pr->type == LAYOUT_PARAM_TYPE_ARROWS)
        {
          if (CONTROLS_MOUSE_CURSOR_IS_OVER_PATCH_PARAM_ARROWS_LEFT() && 
              CONTROLS_MOUSE_BUTTON_IS_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT))
          {
            controls_patch_parameter_adjust(G_patch_edit_cart_number - PATCH_CART_NUMBER_LOWER_BOUND, 
                                            G_patch_edit_patch_number - PATCH_PATCH_NUMBER_LOWER_BOUND, 
                                            m);
          }
          else if ( CONTROLS_MOUSE_CURSOR_IS_OVER_PATCH_PARAM_ARROWS_RIGHT() && 
                    CONTROLS_MOUSE_BUTTON_IS_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT))
          {
            controls_patch_parameter_adjust(G_patch_edit_cart_number - PATCH_CART_NUMBER_LOWER_BOUND, 
                                            G_patch_edit_patch_number - PATCH_PATCH_NUMBER_LOWER_BOUND, 
                                            m);
          }
        }
        else if (pr->type == LAYOUT_PARAM_TYPE_RADIO)
        {
          if (CONTROLS_MOUSE_CURSOR_IS_OVER_PATCH_PARAM_RADIO_BUTTON() && 
              CONTROLS_MOUSE_BUTTON_IS_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT))
          {
            controls_patch_parameter_adjust(G_patch_edit_cart_number - PATCH_CART_NUMBER_LOWER_BOUND, 
                                            G_patch_edit_patch_number - PATCH_PATCH_NUMBER_LOWER_BOUND, 
                                            m);
          }
        }
        else if (pr->type == LAYOUT_PARAM_TYPE_FLAG)
        {
          if (CONTROLS_MOUSE_CURSOR_IS_OVER_PATCH_PARAM_RADIO_BUTTON() && 
              CONTROLS_MOUSE_BUTTON_IS_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT))
          {
            controls_patch_parameter_adjust(G_patch_edit_cart_number - PATCH_CART_NUMBER_LOWER_BOUND, 
                                            G_patch_edit_patch_number - PATCH_PATCH_NUMBER_LOWER_BOUND, 
                                            m);
          }
        }
      }
    }
  }

  /* remove edges */
  controls_remove_edges();

  /* reset mouse wheel movement */
  S_mouse_wheel_movement = 0;

  return 0;
}

