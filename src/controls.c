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
  CONTROLS_KEY_INDEX_ESCAPE = 0, 
  CONTROLS_KEY_INDEX_DECREASE_WINDOW_SIZE, 
  CONTROLS_KEY_INDEX_INCREASE_WINDOW_SIZE, 
  CONTROLS_KEY_INDEX_SCROLL_UP, 
  CONTROLS_KEY_INDEX_SCROLL_DOWN, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_C, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_C_SHARP, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_D, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_D_SHARP, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_E, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_F, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_F_SHARP, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_G, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_G_SHARP, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_A, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_A_SHARP, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_B, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_C_PRIME, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_C_SHARP_PRIME, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_D_PRIME, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_C, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_C_SHARP, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_D, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_D_SHARP, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_E, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_F, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_F_SHARP, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_G, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_G_SHARP, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_A, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_A_SHARP, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_B, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_C_PRIME, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_C_SHARP_PRIME, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_D_PRIME, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_PORTAMENTO_SWITCH, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_ARPEGGIO_SWITCH, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_SUSTAIN_PEDAL, 
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

/* set key state macros */
#define CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(scancode, index)             \
  if ((code == scancode) && (CONTROLS_KEY_IS_OFF_OR_RELEASED(index)))          \
  {                                                                            \
    S_key_states[index] = CONTROLS_KEY_STATE_PRESSED;                          \
  }

#define CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(scancode, index)           \
  if ((code == scancode) && (CONTROLS_KEY_IS_ON_OR_PRESSED(index)))            \
  {                                                                            \
    S_key_states[index] = CONTROLS_KEY_STATE_RELEASED;                         \
  }

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

/* parameter adjustment */
#define CONTROLS_SET_PATCH_PARAMETER(param)                                    \
  if ((pr->type == LAYOUT_PARAM_TYPE_SLIDER) &&                                \
      (param != value))                                                        \
  {                                                                            \
    param = value;                                                             \
    param_changed = 1;                                                         \
                                                                               \
    if (param < pr->lower_bound)                                               \
      param = pr->lower_bound;                                                 \
    else if (param > pr->upper_bound)                                          \
      param = pr->upper_bound;                                                 \
  }                                                                            \
  else if (pr->type == LAYOUT_PARAM_TYPE_ARROWS)                               \
  {                                                                            \
    param += value;                                                            \
    param_changed = 1;                                                         \
                                                                               \
    if (param < pr->lower_bound)                                               \
      param = pr->lower_bound;                                                 \
    else if (param > pr->upper_bound)                                          \
      param = pr->upper_bound;                                                 \
  }                                                                            \
  else if (pr->type == LAYOUT_PARAM_TYPE_RADIO)                                \
  {                                                                            \
    if (param != pr->lower_bound)                                              \
      param = pr->lower_bound;                                                 \
    else                                                                       \
      param = pr->upper_bound;                                                 \
                                                                               \
    param_changed = 1;                                                         \
  }                                                                            \
  else if (pr->type == LAYOUT_PARAM_TYPE_FLAG)                                 \
  {                                                                            \
    if ((param & pr->upper_bound) == 0)                                        \
      param |= pr->upper_bound;                                                \
    else                                                                       \
      param &= ~pr->upper_bound;                                               \
                                                                               \
    param_changed = 1;                                                         \
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
  int i;

  for (i = 0; i < CONTROLS_NUM_KEY_INDICES; i++)
    S_key_states[i] = CONTROLS_KEY_STATE_OFF;

  for (i = 0; i < CONTROLS_NUM_MOUSE_BUTTON_INDICES; i++)
    S_mouse_button_states[i] = CONTROLS_MOUSE_BUTTON_STATE_OFF;

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
short int controls_patch_parameter_adjust(int cart_num, int patch_num, 
                                          int param_index)
{
  int value;
  int param_changed;

  patch* pc;
  param* pr;

  int patch_index;

  /* make sure that the cart & patch numbers is valid */
  if (PATCH_CART_NO_IS_NOT_VALID(cart_num))
    return 1;

  if (PATCH_PATCH_NO_IS_NOT_VALID(patch_num))
    return 1;

  /* determine patch index */
  PATCH_COMPUTE_PATCH_INDEX(cart_num, patch_num)

  /* make sure that the parameter index is valid */
  if (LAYOUT_PARAM_INDEX_IS_NOT_VALID(param_index))
    return 1;

  /* obtain patch and parameter pointers */
  pc = &G_patch_bank[patch_index];
  pr = &G_layout_params[param_index];

  /* determine value and adjustment mode */
  if (pr->type == LAYOUT_PARAM_TYPE_SLIDER)
  {
    /* (value - lower_bound) / (upper_bound - lower_bound) = (mouse_x - SLIDER_POS_LOWER_BOUND) / SLIDER_WIDTH */
    value = 
      S_mouse_remapped_pos_x - 2 * (LAYOUT_OVERSCAN_CENTER_X + pr->center_x + LAYOUT_CART_PARAM_SLIDER_TRACK_X - 4 * LAYOUT_CART_PARAM_SLIDER_WIDTH);

    value *= pr->upper_bound - pr->lower_bound;
    value /= 2 * (8 * LAYOUT_CART_PARAM_SLIDER_WIDTH);
    value += pr->lower_bound;
  }
  else if (pr->type == LAYOUT_PARAM_TYPE_ARROWS)
  {
    if ((S_mouse_remapped_pos_x >= (2 * (LAYOUT_OVERSCAN_CENTER_X + pr->center_x + LAYOUT_CART_PARAM_ARROWS_LEFT_X - 4))) && 
        (S_mouse_remapped_pos_x <  (2 * (LAYOUT_OVERSCAN_CENTER_X + pr->center_x + LAYOUT_CART_PARAM_ARROWS_LEFT_X + 4))))
    {
      value = -1;
    }
    else if ( (S_mouse_remapped_pos_x >= (2 * (LAYOUT_OVERSCAN_CENTER_X + pr->center_x + LAYOUT_CART_PARAM_ARROWS_RIGHT_X - 4))) && 
              (S_mouse_remapped_pos_x <  (2 * (LAYOUT_OVERSCAN_CENTER_X + pr->center_x + LAYOUT_CART_PARAM_ARROWS_RIGHT_X + 4))))
    {
      value = 1;
    }
    else
      value = 0;
  }
  else if (pr->type == LAYOUT_PARAM_TYPE_RADIO)
    value = 0;
  else if (pr->type == LAYOUT_PARAM_TYPE_FLAG)
    value = 0;
  else
    return 0;

  /* initialize change flag */
  param_changed = 0;

  /* cart number */
  if (pr->label == LAYOUT_CART_PARAM_LABEL_CART_NUMBER)
  {
    CONTROLS_SET_PATCH_PARAMETER(G_patch_edit_cart_number)
  }
  /* patch number */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_PATCH_NUMBER)
  {
    CONTROLS_SET_PATCH_PARAMETER(G_patch_edit_patch_number)
  }
  /* algorithm */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ALGORITHM)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->algorithm)
  }
  /* oscillator sync */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_SYNC)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->osc_sync)
  }
  /* oscillator 1 */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_1_WAVEFORM)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->osc_waveform[0])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_1_PHI)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->osc_phi[0])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_1_FREQ_MODE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->osc_freq_mode[0])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_1_MULTIPLE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->osc_multiple[0])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_1_DIVISOR)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->osc_divisor[0])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_1_DETUNE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->osc_detune[0])
  }
  /* oscillator 2 */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_2_WAVEFORM)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->osc_waveform[1])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_2_PHI)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->osc_phi[1])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_2_FREQ_MODE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->osc_freq_mode[1])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_2_MULTIPLE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->osc_multiple[1])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_2_DIVISOR)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->osc_divisor[1])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_2_DETUNE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->osc_detune[1])
  }
  /* oscillator 3 */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_3_WAVEFORM)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->osc_waveform[2])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_3_PHI)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->osc_phi[2])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_3_FREQ_MODE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->osc_freq_mode[2])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_3_MULTIPLE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->osc_multiple[2])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_3_DIVISOR)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->osc_divisor[2])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_3_DETUNE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->osc_detune[2])
  }
  /* amplitude envelope 1 */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_1_ATTACK)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_attack[0])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_1_DECAY)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_decay[0])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_1_RELEASE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_release[0])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_1_AMPLITUDE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_amplitude[0])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_1_SUSTAIN)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_sustain[0])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_1_HOLD)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_hold[0])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_1_PEDAL)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_pedal[0])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_1_RATE_KS)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_rate_ks[0])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_1_LEVEL_KS)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_level_ks[0])
  }
  /* amplitude envelope 2 */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_2_ATTACK)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_attack[1])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_2_DECAY)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_decay[1])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_2_RELEASE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_release[1])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_2_AMPLITUDE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_amplitude[1])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_2_SUSTAIN)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_sustain[1])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_2_HOLD)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_hold[1])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_2_PEDAL)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_pedal[1])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_2_RATE_KS)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_rate_ks[1])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_2_LEVEL_KS)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_level_ks[1])
  }
  /* amplitude envelope 3 */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_3_ATTACK)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_attack[2])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_3_DECAY)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_decay[2])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_3_RELEASE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_release[2])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_3_AMPLITUDE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_amplitude[2])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_3_SUSTAIN)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_sustain[2])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_3_HOLD)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_hold[2])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_3_PEDAL)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_pedal[2])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_3_RATE_KS)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_rate_ks[2])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_3_LEVEL_KS)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_level_ks[2])
  }
  /* vibrato lfo */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_VIBRATO_WAVEFORM)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->lfo_waveform[BANK_LFO_INDEX_VIBRATO])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_VIBRATO_DELAY)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->lfo_delay[BANK_LFO_INDEX_VIBRATO])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_VIBRATO_SPEED)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->lfo_speed[BANK_LFO_INDEX_VIBRATO])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_VIBRATO_DEPTH)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->lfo_depth[BANK_LFO_INDEX_VIBRATO])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_VIBRATO_SENSITIVITY)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->lfo_sensitivity[BANK_LFO_INDEX_VIBRATO])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_VIBRATO_SYNC)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->lfo_sync[BANK_LFO_INDEX_VIBRATO])
  }
  /* tremolo lfo */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_WAVEFORM)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->lfo_waveform[BANK_LFO_INDEX_TREMOLO])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_DELAY)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->lfo_delay[BANK_LFO_INDEX_TREMOLO])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_SPEED)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->lfo_speed[BANK_LFO_INDEX_TREMOLO])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_DEPTH)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->lfo_depth[BANK_LFO_INDEX_TREMOLO])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_SENSITIVITY)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->lfo_sensitivity[BANK_LFO_INDEX_TREMOLO])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_SYNC)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->lfo_sync[BANK_LFO_INDEX_TREMOLO])
  }
  /* chorus */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_CHORUS_WAVEFORM)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->lfo_waveform[BANK_LFO_INDEX_CHORUS])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_CHORUS_DELAY)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->lfo_delay[BANK_LFO_INDEX_CHORUS])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_CHORUS_SPEED)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->lfo_speed[BANK_LFO_INDEX_CHORUS])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_CHORUS_DEPTH)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->lfo_depth[BANK_LFO_INDEX_CHORUS])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_CHORUS_SENSITIVITY)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->lfo_sensitivity[BANK_LFO_INDEX_CHORUS])
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_CHORUS_SYNC)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->lfo_sync[BANK_LFO_INDEX_CHORUS])
  }
  /* arpeggio */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ARPEGGIO_MODE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->arpeggio_mode)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ARPEGGIO_PATTERN)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->arpeggio_pattern)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ARPEGGIO_OCTAVES)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->arpeggio_octaves)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ARPEGGIO_SPEED)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->arpeggio_speed)
  }
  /* boost */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_BOOST_SENSITIVITY)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->boost_sensitivity)
  }
  /* velocity */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_VELOCITY_SENSITIVITY)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->velocity_sensitivity)
  }
  /* pitch envelope */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_ENV_ATTACK)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->peg_attack)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_ENV_DECAY)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->peg_decay)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_ENV_RELEASE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->peg_release)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_ENV_MAXIMUM)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->peg_maximum)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_ENV_FINALE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->peg_finale)
  }
  /* portamento */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_PORTAMENTO_MODE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->portamento_mode)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_PORTAMENTO_LEGATO)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->portamento_legato)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_PORTAMENTO_SPEED)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->portamento_speed)
  }
  /* pitch wheel */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_WHEEL_MODE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->pitch_wheel_mode)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_WHEEL_RANGE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->pitch_wheel_range)
  }
  /* filters */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_HIGHPASS_CUTOFF)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->highpass_cutoff)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_LOWPASS_CUTOFF)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->lowpass_cutoff)
  }
  /* tremolo routing */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_ROUTING_ENV_1)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->tremolo_routing)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_ROUTING_ENV_2)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->tremolo_routing)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_ROUTING_ENV_3)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->tremolo_routing)
  }
  /* boost routing */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_BOOST_ROUTING_ENV_1)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->boost_routing)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_BOOST_ROUTING_ENV_2)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->boost_routing)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_BOOST_ROUTING_ENV_3)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->boost_routing)
  }
  /* velocity routing */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_VELOCITY_ROUTING_ENV_1)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->velocity_routing)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_VELOCITY_ROUTING_ENV_2)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->velocity_routing)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_VELOCITY_ROUTING_ENV_3)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->velocity_routing)
  }
  /* mod wheel routing */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_MOD_WHEEL_ROUTING_VIBRATO)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->mod_wheel_routing)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_MOD_WHEEL_ROUTING_TREMOLO)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->mod_wheel_routing)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_MOD_WHEEL_ROUTING_BOOST)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->mod_wheel_routing)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_MOD_WHEEL_ROUTING_CHORUS)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->mod_wheel_routing)
  }
  /* aftertouch routing */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_AFTERTOUCH_ROUTING_VIBRATO)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->aftertouch_routing)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_AFTERTOUCH_ROUTING_TREMOLO)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->aftertouch_routing)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_AFTERTOUCH_ROUTING_BOOST)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->aftertouch_routing)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_AFTERTOUCH_ROUTING_CHORUS)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->aftertouch_routing)
  }
  /* exp pedal routing */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_EXP_PEDAL_ROUTING_VIBRATO)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->exp_pedal_routing)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_EXP_PEDAL_ROUTING_TREMOLO)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->exp_pedal_routing)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_EXP_PEDAL_ROUTING_BOOST)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->exp_pedal_routing)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_EXP_PEDAL_ROUTING_CHORUS)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->exp_pedal_routing)
  }
  /* audition */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_OCTAVE)
  {
    CONTROLS_SET_PATCH_PARAMETER(G_patch_edit_octave)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_VELOCITY)
  {
    CONTROLS_SET_PATCH_PARAMETER(G_patch_edit_note_velocity)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_PITCH_WHEEL)
  {
    CONTROLS_SET_PATCH_PARAMETER(G_patch_edit_pitch_wheel_pos)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_MOD_WHEEL)
  {
    CONTROLS_SET_PATCH_PARAMETER(G_patch_edit_mod_wheel_pos)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_AFTERTOUCH)
  {
    CONTROLS_SET_PATCH_PARAMETER(G_patch_edit_aftertouch_pos)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_EXP_PEDAL)
  {
    CONTROLS_SET_PATCH_PARAMETER(G_patch_edit_exp_pedal_pos)
  }
  else
    return 0;

  /* reload patch if a parameter was changed */
  if (param_changed == 1)
  {
    if ((pr->label == LAYOUT_CART_PARAM_LABEL_CART_NUMBER) || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_PATCH_NUMBER))
    {
      instrument_load_patch(G_patch_edit_instrument_index, 
                            G_patch_edit_cart_number, G_patch_edit_patch_number);
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_VELOCITY)
      instrument_set_note_velocity(G_patch_edit_instrument_index, G_patch_edit_note_velocity);
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_PITCH_WHEEL)
      instrument_set_pitch_wheel_position(G_patch_edit_instrument_index, G_patch_edit_pitch_wheel_pos);
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_MOD_WHEEL)
      instrument_set_mod_wheel_position(G_patch_edit_instrument_index, G_patch_edit_mod_wheel_pos);
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_AFTERTOUCH)
      instrument_set_aftertouch_position(G_patch_edit_instrument_index, G_patch_edit_aftertouch_pos);
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_EXP_PEDAL)
      instrument_set_exp_pedal_position(G_patch_edit_instrument_index, G_patch_edit_exp_pedal_pos);
    else
    {
      instrument_load_patch(G_patch_edit_instrument_index, 
                            G_patch_edit_cart_number, G_patch_edit_patch_number);
    }
  }

  return 0;
}

/*******************************************************************************
** controls_keyboard_key_pressed()
*******************************************************************************/
short int controls_keyboard_key_pressed(SDL_Scancode code)
{
  /* escape */
  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_ESCAPE, 
                                            CONTROLS_KEY_INDEX_ESCAPE)

  /* change window size */
  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_LEFTBRACKET, 
                                            CONTROLS_KEY_INDEX_DECREASE_WINDOW_SIZE)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_RIGHTBRACKET, 
                                            CONTROLS_KEY_INDEX_INCREASE_WINDOW_SIZE)

  /* scroll up & down */
  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_UP, 
                                            CONTROLS_KEY_INDEX_SCROLL_UP)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_DOWN, 
                                            CONTROLS_KEY_INDEX_SCROLL_DOWN)

  /* patch edit: notes (this octave) */
  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_Z, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_C)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_S, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_C_SHARP)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_X, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_D)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_D, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_D_SHARP)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_C, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_E)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_V, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_F)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_G, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_F_SHARP)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_B, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_G)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_H, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_G_SHARP)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_N, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_A)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_J, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_A_SHARP)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_M, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_B)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_COMMA, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_C_PRIME)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_L, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_C_SHARP_PRIME)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_PERIOD, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_D_PRIME)

  /* patch edit: notes (next octave) */
  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_Q, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_C)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_2, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_C_SHARP)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_W, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_D)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_3, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_D_SHARP)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_E, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_E)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_R, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_F)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_5, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_F_SHARP)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_T, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_G)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_6, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_G_SHARP)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_Y, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_A)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_7, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_A_SHARP)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_U, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_B)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_I, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_C_PRIME)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_9, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_C_SHARP_PRIME)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_O, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_D_PRIME)

  /* patch edit: portamento switch, arpeggio switch, sustain pedal */
  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_LSHIFT, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_PORTAMENTO_SWITCH)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_RSHIFT, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_ARPEGGIO_SWITCH)

  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_SPACE, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_SUSTAIN_PEDAL)

  return 0;
}

/*******************************************************************************
** controls_keyboard_key_released()
*******************************************************************************/
short int controls_keyboard_key_released(SDL_Scancode code)
{
  /* escape */
  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_ESCAPE, 
                                              CONTROLS_KEY_INDEX_ESCAPE)

  /* change window size */
  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_LEFTBRACKET, 
                                              CONTROLS_KEY_INDEX_DECREASE_WINDOW_SIZE)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_RIGHTBRACKET, 
                                              CONTROLS_KEY_INDEX_INCREASE_WINDOW_SIZE)

  /* scroll up & down */
  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_UP, 
                                              CONTROLS_KEY_INDEX_SCROLL_UP)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_DOWN, 
                                              CONTROLS_KEY_INDEX_SCROLL_DOWN)

  /* patch edit: notes (this octave) */
  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_Z, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_C)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_S, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_C_SHARP)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_X, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_D)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_D, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_D_SHARP)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_C, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_E)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_V, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_F)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_G, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_F_SHARP)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_B, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_G)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_H, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_G_SHARP)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_N, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_A)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_J, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_A_SHARP)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_M, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_B)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_COMMA, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_C_PRIME)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_L, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_C_SHARP_PRIME)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_PERIOD, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_D_PRIME)

  /* patch edit: notes (next octave) */
  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_Q, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_C)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_2, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_C_SHARP)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_W, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_D)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_3, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_D_SHARP)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_E, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_E)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_R, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_F)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_5, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_F_SHARP)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_T, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_G)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_6, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_G_SHARP)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_Y, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_A)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_7, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_A_SHARP)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_U, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_B)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_I, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_C_PRIME)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_9, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_C_SHARP_PRIME)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_O, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_D_PRIME)

  /* patch edit: portamento switch, arpeggio switch, sustain pedal */
  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_LSHIFT, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_PORTAMENTO_SWITCH)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_RSHIFT, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_ARPEGGIO_SWITCH)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_SPACE, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_SUSTAIN_PEDAL)

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
  int k;

  button* b;
  param*  pr;

  int pos_y;

  /* check key states */

  /* decrease window size */
  if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_DECREASE_WINDOW_SIZE))
    graphics_decrease_window_size();

  /* increase window size */
  if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_INCREASE_WINDOW_SIZE))
    graphics_increase_window_size();

  /* scroll up */
  if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_SCROLL_UP))
  {
    G_current_scroll_amount -= 2;

    if (G_current_scroll_amount < 0)
      G_current_scroll_amount = 0;
  }

  /* scroll down */
  if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_SCROLL_DOWN))
  {
    G_current_scroll_amount += 2;

    if (G_current_scroll_amount > G_max_scroll_amount)
      G_current_scroll_amount = G_max_scroll_amount;
  }

  /* cart screen */
  if (G_prog_screen == PROGRAM_SCREEN_CART)
  {
    for ( k = CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_C; 
          k <= CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_D_PRIME;
          k++)
    {
      if (CONTROLS_KEY_IS_PRESSED(k))
      {
        instrument_key_pressed( G_patch_edit_instrument_index, 
                                12 * G_patch_edit_octave + (k - CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_C));
      }
      else if (CONTROLS_KEY_IS_RELEASED(k))
      {
        instrument_key_released(G_patch_edit_instrument_index, 
                                12 * G_patch_edit_octave + (k - CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_C));
      }
    }

    for ( k = CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_C; 
          k <= CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_D_PRIME;
          k++)
    {
      if (CONTROLS_KEY_IS_PRESSED(k))
      {
        instrument_key_pressed( G_patch_edit_instrument_index, 
                                12 * (G_patch_edit_octave + 1) + (k - CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_C));
      }
      else if (CONTROLS_KEY_IS_RELEASED(k))
      {
        instrument_key_released(G_patch_edit_instrument_index, 
                                12 * (G_patch_edit_octave + 1) + (k - CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_C));
      }
    }

    /* portamento switch */
    if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_PATCH_EDIT_PORTAMENTO_SWITCH))
      instrument_set_portamento_switch(G_patch_edit_instrument_index, MIDI_CONT_SWITCH_STATE_ON);
    else if (CONTROLS_KEY_IS_RELEASED(CONTROLS_KEY_INDEX_PATCH_EDIT_PORTAMENTO_SWITCH))
      instrument_set_portamento_switch(G_patch_edit_instrument_index, MIDI_CONT_SWITCH_STATE_OFF);

    /* arpeggio switch */
    if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_PATCH_EDIT_ARPEGGIO_SWITCH))
      instrument_set_arpeggio_switch(G_patch_edit_instrument_index, MIDI_CONT_SWITCH_STATE_ON);
    else if (CONTROLS_KEY_IS_RELEASED(CONTROLS_KEY_INDEX_PATCH_EDIT_ARPEGGIO_SWITCH))
      instrument_set_arpeggio_switch(G_patch_edit_instrument_index, MIDI_CONT_SWITCH_STATE_OFF);

    /* sustain pedal */
    if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_PATCH_EDIT_SUSTAIN_PEDAL))
      instrument_set_sustain_pedal(G_patch_edit_instrument_index, MIDI_CONT_SWITCH_STATE_ON);
    else if (CONTROLS_KEY_IS_RELEASED(CONTROLS_KEY_INDEX_PATCH_EDIT_SUSTAIN_PEDAL))
      instrument_set_sustain_pedal(G_patch_edit_instrument_index, MIDI_CONT_SWITCH_STATE_OFF);
  }

  /* check mouse button clicks */

  /* check top panel mouse button clicks */
  if (CONTROLS_MOUSE_CURSOR_IS_OVER_TOP_PANEL())
  {
    for ( k = LAYOUT_TOP_PANEL_BUTTONS_START_INDEX; 
          k < LAYOUT_TOP_PANEL_BUTTONS_END_INDEX; 
          k++)
    {
      b = &G_layout_buttons[k];

      /* if the cursor is over this button, click it! */
      if (CONTROLS_MOUSE_CURSOR_IS_OVER_BUTTON())
      {
        if (CONTROLS_MOUSE_BUTTON_IS_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT))
        {
          layout_reset_top_panel_button_states();

          b->state = LAYOUT_BUTTON_STATE_ON;

          if (b->label == LAYOUT_TOP_PANEL_BUTTON_LABEL_CART)
            program_loop_change_screen(PROGRAM_SCREEN_CART);
          else if (b->label == LAYOUT_TOP_PANEL_BUTTON_LABEL_INSTRUMENTS)
            program_loop_change_screen(PROGRAM_SCREEN_INSTRUMENTS);
          else if (b->label == LAYOUT_TOP_PANEL_BUTTON_LABEL_SONG)
            program_loop_change_screen(PROGRAM_SCREEN_SONG);
          else if (b->label == LAYOUT_TOP_PANEL_BUTTON_LABEL_MIXER)
            program_loop_change_screen(PROGRAM_SCREEN_MIXER);
          else if (b->label == LAYOUT_TOP_PANEL_BUTTON_LABEL_SOUND_FX)
            program_loop_change_screen(PROGRAM_SCREEN_SOUND_FX);
          else if (b->label == LAYOUT_TOP_PANEL_BUTTON_LABEL_DPCM)
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

    /* check for patch edit area clicks */
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
      for ( k = LAYOUT_CART_BUTTONS_START_INDEX; 
            k < LAYOUT_CART_BUTTONS_END_INDEX; 
            k++)
      {
        b = &G_layout_buttons[k];

        /* determine vertical position for this button */
        pos_y = b->center_y - G_current_scroll_amount;

        if (LAYOUT_CART_BUTTON_IS_NOT_IN_MAIN_AREA(pos_y))
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
      for ( k = LAYOUT_CART_PARAMS_START_INDEX; 
            k < LAYOUT_CART_PARAMS_END_INDEX; 
            k++)
      {
        pr = &G_layout_params[k];

        /* determine vertical position for audition bar params (remain stationary) */
        if ((pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_OCTAVE)      || 
            (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_VELOCITY)    || 
            (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_PITCH_WHEEL) || 
            (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_MOD_WHEEL)   || 
            (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_AFTERTOUCH)  || 
            (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_EXP_PEDAL))
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
            controls_patch_parameter_adjust(G_patch_edit_cart_number, G_patch_edit_patch_number, k);
          }
        }
        else if (pr->type == LAYOUT_PARAM_TYPE_ARROWS)
        {
          if (CONTROLS_MOUSE_CURSOR_IS_OVER_PATCH_PARAM_ARROWS_LEFT() && 
              CONTROLS_MOUSE_BUTTON_IS_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT))
          {
            controls_patch_parameter_adjust(G_patch_edit_cart_number, G_patch_edit_patch_number, k);
          }
          else if ( CONTROLS_MOUSE_CURSOR_IS_OVER_PATCH_PARAM_ARROWS_RIGHT() && 
                    CONTROLS_MOUSE_BUTTON_IS_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT))
          {
            controls_patch_parameter_adjust(G_patch_edit_cart_number, G_patch_edit_patch_number, k);
          }
        }
        else if (pr->type == LAYOUT_PARAM_TYPE_RADIO)
        {
          if (CONTROLS_MOUSE_CURSOR_IS_OVER_PATCH_PARAM_RADIO_BUTTON() && 
              CONTROLS_MOUSE_BUTTON_IS_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT))
          {
            controls_patch_parameter_adjust(G_patch_edit_cart_number, G_patch_edit_patch_number, k);
          }
        }
        else if (pr->type == LAYOUT_PARAM_TYPE_FLAG)
        {
          if (CONTROLS_MOUSE_CURSOR_IS_OVER_PATCH_PARAM_RADIO_BUTTON() && 
              CONTROLS_MOUSE_BUTTON_IS_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT))
          {
            controls_patch_parameter_adjust(G_patch_edit_cart_number, G_patch_edit_patch_number, k);
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

