/*******************************************************************************
** controls.c (keyboard / mouse input)
*******************************************************************************/

#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdlib.h>

#include "controls.h"
#include "global.h"
#include "graphics.h"
#include "layout.h"
#include "patch.h"
#include "pattern.h"
#include "progloop.h"
#include "screen.h"
#include "synth.h"

enum
{
  CONTROLS_KEY_INDEX_ESCAPE = 0, 
  CONTROLS_KEY_INDEX_DECREASE_WINDOW_SIZE, 
  CONTROLS_KEY_INDEX_INCREASE_WINDOW_SIZE, 
  CONTROLS_KEY_INDEX_SCROLL_UP, 
  CONTROLS_KEY_INDEX_SCROLL_DOWN, 
  CONTROLS_KEY_INDEX_DEGREE_1, 
  CONTROLS_KEY_INDEX_DEGREE_2, 
  CONTROLS_KEY_INDEX_DEGREE_3, 
  CONTROLS_KEY_INDEX_DEGREE_4, 
  CONTROLS_KEY_INDEX_DEGREE_5, 
  CONTROLS_KEY_INDEX_DEGREE_6, 
  CONTROLS_KEY_INDEX_DEGREE_7, 
  CONTROLS_KEY_INDEX_DEGREE_8, 
  CONTROLS_KEY_INDEX_VALUE_0_OR_KEY_OFF, 
  CONTROLS_KEY_INDEX_VALUE_1, 
  CONTROLS_KEY_INDEX_VALUE_2, 
  CONTROLS_KEY_INDEX_VALUE_3, 
  CONTROLS_KEY_INDEX_VALUE_4, 
  CONTROLS_KEY_INDEX_VALUE_5, 
  CONTROLS_KEY_INDEX_VALUE_6, 
  CONTROLS_KEY_INDEX_VALUE_7, 
  CONTROLS_KEY_INDEX_VALUE_8, 
  CONTROLS_KEY_INDEX_VALUE_9, 
  CONTROLS_KEY_INDEX_VALUE_10, 
  CONTROLS_KEY_INDEX_VALUE_11, 
  CONTROLS_KEY_INDEX_VALUE_12, 
  CONTROLS_KEY_INDEX_VALUE_13, 
  CONTROLS_KEY_INDEX_VALUE_14, 
  CONTROLS_KEY_INDEX_VALUE_15, 
  CONTROLS_KEY_INDEX_VALUE_16, 
  CONTROLS_KEY_INDEX_BLANK, 
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

/* screen regions */
#define CONTROLS_MOUSE_CURSOR_IS_OVER_TOP_PANEL()                                   \
  ( (S_mouse_remapped_pos_x >= 0)                                               &&  \
    (S_mouse_remapped_pos_x <  GRAPHICS_OVERSCAN_WIDTH)                         &&  \
    (S_mouse_remapped_pos_y >= LAYOUT_SCREEN_AREA_TOP_PANEL_LOWER_BOUND_PIXELS) &&  \
    (S_mouse_remapped_pos_y <  LAYOUT_SCREEN_AREA_TOP_PANEL_UPPER_BOUND_PIXELS))

#define CONTROLS_MOUSE_CURSOR_IS_OVER_BOTTOM_PANEL()                                    \
  ( (S_mouse_remapped_pos_x >= 0)                                                   &&  \
    (S_mouse_remapped_pos_x <  GRAPHICS_OVERSCAN_WIDTH)                             &&  \
    (S_mouse_remapped_pos_y >= LAYOUT_SCREEN_AREA_BOTTOM_PANEL_LOWER_BOUND_PIXELS)  &&  \
    (S_mouse_remapped_pos_y <  LAYOUT_SCREEN_AREA_BOTTOM_PANEL_UPPER_BOUND_PIXELS))

#define CONTROLS_MOUSE_CURSOR_IS_OVER_MAIN_AREA()                               \
  ( (S_mouse_remapped_pos_x >= 0)                                           &&  \
    (S_mouse_remapped_pos_x <  GRAPHICS_OVERSCAN_WIDTH)                     &&  \
    (S_mouse_remapped_pos_y >= LAYOUT_SCREEN_AREA_MAIN_LOWER_BOUND_PIXELS)  &&  \
    (S_mouse_remapped_pos_y <  LAYOUT_SCREEN_AREA_MAIN_UPPER_BOUND_PIXELS))

/* vertical scrollbar */
#define CONTROLS_MOUSE_CURSOR_IS_OVER_VERTICAL_SCROLLBAR_UP_ARROW()                                                               \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (LAYOUT_SCROLLBAR_X - 1))                                  &&  \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (LAYOUT_SCROLLBAR_X + 1))                                  &&  \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (LAYOUT_SCROLLBAR_Y - (LAYOUT_SCROLLBAR_HEIGHT - 1) - 1)) &&  \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (LAYOUT_SCROLLBAR_Y - (LAYOUT_SCROLLBAR_HEIGHT - 1) + 1)))

#define CONTROLS_MOUSE_CURSOR_IS_OVER_VERTICAL_SCROLLBAR_DOWN_ARROW()                                                             \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (LAYOUT_SCROLLBAR_X - 1))                                  &&  \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (LAYOUT_SCROLLBAR_X + 1))                                  &&  \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (LAYOUT_SCROLLBAR_Y + (LAYOUT_SCROLLBAR_HEIGHT - 1) - 1)) &&  \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (LAYOUT_SCROLLBAR_Y + (LAYOUT_SCROLLBAR_HEIGHT - 1) + 1)))

#define CONTROLS_MOUSE_CURSOR_IS_OVER_VERTICAL_SCROLLBAR_TRACK()                                                                  \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (LAYOUT_SCROLLBAR_X - 1))                                  &&  \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (LAYOUT_SCROLLBAR_X + 1))                                  &&  \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (LAYOUT_SCROLLBAR_Y - (LAYOUT_SCROLLBAR_HEIGHT - 1) + 1)) &&  \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (LAYOUT_SCROLLBAR_Y + (LAYOUT_SCROLLBAR_HEIGHT - 1) - 1)))

#define CONTROLS_MOUSE_LAST_CLICK_WAS_OVER_VERTICAL_SCROLLBAR_TRACK()                                                               \
  ( (S_mouse_last_click_pos_x >= (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (LAYOUT_SCROLLBAR_X - 1))                                  &&  \
    (S_mouse_last_click_pos_x <  (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (LAYOUT_SCROLLBAR_X + 1))                                  &&  \
    (S_mouse_last_click_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (LAYOUT_SCROLLBAR_Y - (LAYOUT_SCROLLBAR_HEIGHT - 1) + 1)) &&  \
    (S_mouse_last_click_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (LAYOUT_SCROLLBAR_Y + (LAYOUT_SCROLLBAR_HEIGHT - 1) - 1)))

#define CONTROLS_VERTICAL_SCROLLBAR_POS_Y_LOWER_BOUND                          \
  ((GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (LAYOUT_SCROLLBAR_Y - (LAYOUT_SCROLLBAR_HEIGHT - 3) + 1))

/* patch edit parameter silders, adjustment arrows, and radio buttons */
#define CONTROLS_MOUSE_CURSOR_IS_OVER_PATCH_PARAM_SLIDER()                                                                                                              \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (pr->center_x + LAYOUT_PATCH_EDIT_PARAM_SLIDER_TRACK_X - LAYOUT_PATCH_EDIT_PARAM_SLIDER_WIDTH))  &&  \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (pr->center_x + LAYOUT_PATCH_EDIT_PARAM_SLIDER_TRACK_X + LAYOUT_PATCH_EDIT_PARAM_SLIDER_WIDTH))  &&  \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (pr->center_y - G_current_scroll_amount - 1))                                                   &&  \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (pr->center_y - G_current_scroll_amount + 1)))

#define CONTROLS_MOUSE_LAST_CLICK_WAS_OVER_PATCH_PARAM_SLIDER()                                                                                                           \
  ( (S_mouse_last_click_pos_x >= (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (pr->center_x + LAYOUT_PATCH_EDIT_PARAM_SLIDER_TRACK_X - LAYOUT_PATCH_EDIT_PARAM_SLIDER_WIDTH))  &&  \
    (S_mouse_last_click_pos_x <  (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (pr->center_x + LAYOUT_PATCH_EDIT_PARAM_SLIDER_TRACK_X + LAYOUT_PATCH_EDIT_PARAM_SLIDER_WIDTH))  &&  \
    (S_mouse_last_click_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (pr->center_y - G_current_scroll_amount - 1))                                                   &&  \
    (S_mouse_last_click_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (pr->center_y - G_current_scroll_amount + 1)))

#define CONTROLS_PATCH_PARAM_SLIDER_POS_X_LOWER_BOUND                          \
  ((GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (pr->center_x + LAYOUT_PATCH_EDIT_PARAM_SLIDER_TRACK_X - LAYOUT_PATCH_EDIT_PARAM_SLIDER_WIDTH))

#define CONTROLS_MOUSE_CURSOR_IS_OVER_PATCH_PARAM_ARROWS_LEFT()                                                                     \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (pr->center_x + LAYOUT_PATCH_EDIT_PARAM_ARROWS_LEFT_X - 1))  &&  \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (pr->center_x + LAYOUT_PATCH_EDIT_PARAM_ARROWS_LEFT_X + 1))  &&  \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (pr->center_y - G_current_scroll_amount - 1))               &&  \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (pr->center_y - G_current_scroll_amount + 1)))

#define CONTROLS_MOUSE_CURSOR_IS_OVER_PATCH_PARAM_ARROWS_RIGHT()                                                                    \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (pr->center_x + LAYOUT_PATCH_EDIT_PARAM_ARROWS_RIGHT_X - 1)) &&  \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (pr->center_x + LAYOUT_PATCH_EDIT_PARAM_ARROWS_RIGHT_X + 1)) &&  \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (pr->center_y - G_current_scroll_amount - 1))               &&  \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (pr->center_y - G_current_scroll_amount + 1)))

#define CONTROLS_MOUSE_CURSOR_IS_OVER_PATCH_PARAM_RADIO_BUTTON()                                                                    \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (pr->center_x + LAYOUT_PATCH_EDIT_PARAM_RADIO_BUTTON_X - 1)) &&  \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (pr->center_x + LAYOUT_PATCH_EDIT_PARAM_RADIO_BUTTON_X + 1)) &&  \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (pr->center_y - G_current_scroll_amount - 1))               &&  \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (pr->center_y - G_current_scroll_amount + 1)))

/* pattern edit parameter step spaces and arrows */
#define CONTROLS_MOUSE_CURSOR_IS_OVER_PATTERN_PARAM_STEP_SPACE_STANDARD()                                                                       \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (pr->center_x + 0 - LAYOUT_PATTERN_EDIT_INSTRUMENT_STEP_WIDTH_STANDARD)) &&  \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (pr->center_x + 0 + LAYOUT_PATTERN_EDIT_INSTRUMENT_STEP_WIDTH_STANDARD)) &&  \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (pr->center_y - G_current_scroll_amount - 1))                           &&  \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (pr->center_y - G_current_scroll_amount + 1)))

#define CONTROLS_MOUSE_CURSOR_IS_OVER_PATTERN_PARAM_STEP_SPACE_WIDE()                                                                       \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (pr->center_x + 0 - LAYOUT_PATTERN_EDIT_INSTRUMENT_STEP_WIDTH_WIDE)) &&  \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (pr->center_x + 0 + LAYOUT_PATTERN_EDIT_INSTRUMENT_STEP_WIDTH_WIDE)) &&  \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (pr->center_y - G_current_scroll_amount - 1))                       &&  \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (pr->center_y - G_current_scroll_amount + 1)))

/* parameter adjustment */
#define CONTROLS_SET_PATCH_PARAMETER(param, lower, upper)                      \
  if ((pr->type == LAYOUT_PATCH_EDIT_PARAM_TYPE_SLIDER) &&                     \
      (param != value))                                                        \
  {                                                                            \
    param = value;                                                             \
    param_changed = 1;                                                         \
  }                                                                            \
  else if (pr->type == LAYOUT_PATCH_EDIT_PARAM_TYPE_ARROWS)                    \
  {                                                                            \
    param += value;                                                            \
    param_changed = 1;                                                         \
  }                                                                            \
  else if (pr->type == LAYOUT_PATCH_EDIT_PARAM_TYPE_RADIO)                     \
  {                                                                            \
    if (param != lower)                                                        \
      param = lower;                                                           \
    else                                                                       \
      param = upper;                                                           \
                                                                               \
    param_changed = 1;                                                         \
  }                                                                            \
                                                                               \
  if (param < lower)                                                           \
    param = lower;                                                             \
  else if (param > upper)                                                      \
    param = upper;

#define CONTROLS_SET_PATTERN_PARAMETER(param, lower, upper)                    \
  if ((pr->type == LAYOUT_PATTERN_EDIT_PARAM_TYPE_STEP_SPACE_STANDARD) &&      \
      (param != value))                                                        \
  {                                                                            \
    param = value;                                                             \
    param_changed = 1;                                                         \
  }                                                                            \
  else if ( (pr->type == LAYOUT_PATTERN_EDIT_PARAM_TYPE_STEP_SPACE_WIDE) &&    \
            (param != value))                                                  \
  {                                                                            \
    param = value;                                                             \
    param_changed = 1;                                                         \
  }                                                                            \
  else if (pr->type == LAYOUT_PATTERN_EDIT_PARAM_TYPE_ARROWS)                  \
  {                                                                            \
    param += value;                                                            \
    param_changed = 1;                                                         \
  }                                                                            \
                                                                               \
  if (param < lower)                                                           \
    param = lower;                                                             \
  else if (param > upper)                                                      \
    param = upper;

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
short int controls_patch_parameter_adjust(int patch_index, int param_index, int amount)
{
  int value;
  int param_changed;

  patch* pc;
  param* pr;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* make sure that the parameter index is valid */
  if (LAYOUT_PARAM_INDEX_IS_NOT_VALID(param_index))
    return 1;

  /* obtain patch and parameter pointers */
  pc = &G_patch_bank[patch_index];
  pr = &G_layout_params[param_index];

  /* determine value and adjustment mode */
  if (pr->type == LAYOUT_PATCH_EDIT_PARAM_TYPE_SLIDER)
    value = (amount * (pr->upper_bound - pr->lower_bound)) / (8 * (LAYOUT_PATCH_EDIT_PARAM_SLIDER_WIDTH - 1)) + pr->lower_bound;
  else if (pr->type == LAYOUT_PATCH_EDIT_PARAM_TYPE_ARROWS)
    value = amount;
  else if (pr->type == LAYOUT_PATCH_EDIT_PARAM_TYPE_RADIO)
    value = 0;
  else
    return 0;

  /* initialize change flag */
  param_changed = 0;

  /* algorithm */
  if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_ALGORITHM)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->algorithm, 
                                  PATCH_ALGORITHM_LOWER_BOUND, 
                                  PATCH_ALGORITHM_UPPER_BOUND)
  }
  /* oscillator */
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_OSC_WAVEFORM)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->osc_waveform[pr->num], 
                                  PATCH_OSC_WAVEFORM_LOWER_BOUND, 
                                  PATCH_OSC_WAVEFORM_UPPER_BOUND) 
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_OSC_FEEDBACK)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->osc_feedback[pr->num], 
                                  PATCH_OSC_FEEDBACK_LOWER_BOUND, 
                                  PATCH_OSC_FEEDBACK_UPPER_BOUND) 
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_OSC_SYNC)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->osc_sync[pr->num], 
                                  PATCH_OSC_SYNC_LOWER_BOUND, 
                                  PATCH_OSC_SYNC_UPPER_BOUND) 
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_OSC_FREQ_MODE)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->osc_freq_mode[pr->num], 
                                  PATCH_OSC_FREQ_MODE_LOWER_BOUND, 
                                  PATCH_OSC_FREQ_MODE_UPPER_BOUND)
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_OSC_MULTIPLE)
  {
    if (pc->osc_freq_mode[pr->num] == 0)
    {
      CONTROLS_SET_PATCH_PARAMETER( pc->osc_multiple[pr->num], 
                                    PATCH_OSC_MULTIPLE_LOWER_BOUND, 
                                    PATCH_OSC_MULTIPLE_UPPER_BOUND)
    }
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_OSC_DIVISOR)
  {
    if (pc->osc_freq_mode[pr->num] == 0)
    {
      CONTROLS_SET_PATCH_PARAMETER( pc->osc_divisor[pr->num], 
                                    PATCH_OSC_DIVISOR_LOWER_BOUND, 
                                    PATCH_OSC_DIVISOR_UPPER_BOUND)
    }
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_OSC_NOTE)
  {
    if (pc->osc_freq_mode[pr->num] == 1)
    {
      CONTROLS_SET_PATCH_PARAMETER( pc->osc_note[pr->num], 
                                    PATCH_OSC_NOTE_LOWER_BOUND, 
                                    PATCH_OSC_NOTE_UPPER_BOUND)
    }
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_OSC_OCTAVE)
  {
    if (pc->osc_freq_mode[pr->num] == 1)
    {
      CONTROLS_SET_PATCH_PARAMETER( pc->osc_octave[pr->num], 
                                    PATCH_OSC_OCTAVE_LOWER_BOUND, 
                                    PATCH_OSC_OCTAVE_UPPER_BOUND)
    }
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_OSC_DETUNE)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->osc_detune[pr->num], 
                                  PATCH_OSC_DETUNE_LOWER_BOUND, 
                                  PATCH_OSC_DETUNE_UPPER_BOUND)
  }
  /* envelope */
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_ENV_ATTACK)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->env_attack[pr->num], 
                                  PATCH_ENV_RATE_LOWER_BOUND, 
                                  PATCH_ENV_RATE_UPPER_BOUND)
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_ENV_DECAY_1)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->env_decay_1[pr->num], 
                                  PATCH_ENV_RATE_LOWER_BOUND, 
                                  PATCH_ENV_RATE_UPPER_BOUND)
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_ENV_DECAY_2)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->env_decay_2[pr->num], 
                                  PATCH_ENV_RATE_LOWER_BOUND, 
                                  PATCH_ENV_RATE_UPPER_BOUND)
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_ENV_RELEASE)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->env_release[pr->num], 
                                  PATCH_ENV_RATE_LOWER_BOUND, 
                                  PATCH_ENV_RATE_UPPER_BOUND)
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_ENV_AMPLITUDE)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->env_amplitude[pr->num], 
                                  PATCH_ENV_AMPLITUDE_LOWER_BOUND, 
                                  PATCH_ENV_AMPLITUDE_UPPER_BOUND)
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_ENV_SUSTAIN)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->env_sustain[pr->num], 
                                  PATCH_ENV_SUSTAIN_LOWER_BOUND, 
                                  PATCH_ENV_SUSTAIN_UPPER_BOUND)
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_ENV_RATE_KS)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->env_rate_ks[pr->num], 
                                  PATCH_ENV_KEYSCALE_LOWER_BOUND, 
                                  PATCH_ENV_KEYSCALE_UPPER_BOUND)
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_ENV_LEVEL_KS)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->env_level_ks[pr->num], 
                                  PATCH_ENV_KEYSCALE_LOWER_BOUND, 
                                  PATCH_ENV_KEYSCALE_UPPER_BOUND)
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_ENV_TRIGGER)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->env_trigger[pr->num], 
                                  PATCH_ENV_TRIGGER_LOWER_BOUND, 
                                  PATCH_ENV_TRIGGER_UPPER_BOUND)
  }
  /* lfo & boost enable */
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_VIBRATO_ENABLE)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->vibrato_enable[pr->num], 
                                  PATCH_MOD_ENABLE_LOWER_BOUND, 
                                  PATCH_MOD_ENABLE_UPPER_BOUND)
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_TREMOLO_ENABLE)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->tremolo_enable[pr->num], 
                                  PATCH_MOD_ENABLE_LOWER_BOUND, 
                                  PATCH_MOD_ENABLE_UPPER_BOUND)
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_BOOST_ENABLE)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->boost_enable[pr->num], 
                                  PATCH_MOD_ENABLE_LOWER_BOUND, 
                                  PATCH_MOD_ENABLE_UPPER_BOUND)
  }
  /* lfo */
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_LFO_WAVEFORM)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->lfo_waveform, 
                                  PATCH_LFO_WAVEFORM_LOWER_BOUND, 
                                  PATCH_LFO_WAVEFORM_UPPER_BOUND)
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_LFO_FREQUENCY)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->lfo_frequency, 
                                  PATCH_LFO_FREQUENCY_LOWER_BOUND, 
                                  PATCH_LFO_FREQUENCY_UPPER_BOUND)
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_LFO_DELAY)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->lfo_delay, 
                                  PATCH_LFO_DELAY_LOWER_BOUND, 
                                  PATCH_LFO_DELAY_UPPER_BOUND)
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_LFO_VIBRATO_MODE)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->lfo_vibrato_mode, 
                                  PATCH_LFO_VIBRATO_MODE_LOWER_BOUND, 
                                  PATCH_LFO_VIBRATO_MODE_UPPER_BOUND)
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_LFO_SYNC)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->lfo_sync, 
                                  PATCH_LFO_SYNC_LOWER_BOUND, 
                                  PATCH_LFO_SYNC_UPPER_BOUND)
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_LFO_TEMPO)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->lfo_tempo, 
                                  PATCH_LFO_TEMPO_LOWER_BOUND, 
                                  PATCH_LFO_TEMPO_UPPER_BOUND)
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_LFO_BASE_VIBRATO)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->lfo_base_vibrato, 
                                  PATCH_MOD_BASE_LOWER_BOUND, 
                                  PATCH_MOD_BASE_UPPER_BOUND)
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_LFO_BASE_TREMOLO)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->lfo_base_tremolo, 
                                  PATCH_MOD_BASE_LOWER_BOUND, 
                                  PATCH_MOD_BASE_UPPER_BOUND)
  }
  /* sweep */
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_PORTAMENTO_MODE)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->portamento_mode, 
                                  PATCH_PORTAMENTO_MODE_LOWER_BOUND, 
                                  PATCH_PORTAMENTO_MODE_UPPER_BOUND)
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_PORTAMENTO_SPEED)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->portamento_speed, 
                                  PATCH_PORTAMENTO_SPEED_LOWER_BOUND, 
                                  PATCH_PORTAMENTO_SPEED_UPPER_BOUND)
  }
  /* filters */
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_HIGHPASS_CUTOFF)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->highpass_cutoff, 
                                  PATCH_HIGHPASS_CUTOFF_LOWER_BOUND, 
                                  PATCH_HIGHPASS_CUTOFF_UPPER_BOUND) 
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_LOWPASS_CUTOFF)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->lowpass_cutoff, 
                                  PATCH_LOWPASS_CUTOFF_LOWER_BOUND, 
                                  PATCH_LOWPASS_CUTOFF_UPPER_BOUND) 
  }
  /* depths */
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_VIBRATO_DEPTH)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->vibrato_depth, 
                                  PATCH_MOD_DEPTH_LOWER_BOUND, 
                                  PATCH_MOD_DEPTH_UPPER_BOUND)
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_TREMOLO_DEPTH)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->tremolo_depth, 
                                  PATCH_MOD_DEPTH_LOWER_BOUND, 
                                  PATCH_MOD_DEPTH_UPPER_BOUND) 
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_BOOST_DEPTH)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->boost_depth, 
                                  PATCH_MOD_DEPTH_LOWER_BOUND, 
                                  PATCH_MOD_DEPTH_UPPER_BOUND)
  }
  /* mod wheel */
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_MOD_WHEEL_VIBRATO)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->mod_wheel_vibrato, 
                                  PATCH_MOD_CONTROLLER_LOWER_BOUND, 
                                  PATCH_MOD_CONTROLLER_UPPER_BOUND)
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_MOD_WHEEL_TREMOLO)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->mod_wheel_tremolo, 
                                  PATCH_MOD_CONTROLLER_LOWER_BOUND, 
                                  PATCH_MOD_CONTROLLER_UPPER_BOUND)
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_MOD_WHEEL_BOOST)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->mod_wheel_boost, 
                                  PATCH_MOD_CONTROLLER_LOWER_BOUND, 
                                  PATCH_MOD_CONTROLLER_UPPER_BOUND)
  }
  /* aftertouch */
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_AFTERTOUCH_VIBRATO)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->aftertouch_vibrato, 
                                  PATCH_MOD_CONTROLLER_LOWER_BOUND, 
                                  PATCH_MOD_CONTROLLER_UPPER_BOUND)
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_AFTERTOUCH_TREMOLO)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->aftertouch_tremolo, 
                                  PATCH_MOD_CONTROLLER_LOWER_BOUND, 
                                  PATCH_MOD_CONTROLLER_UPPER_BOUND) 
  }
  else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_AFTERTOUCH_BOOST)
  {
    CONTROLS_SET_PATCH_PARAMETER( pc->aftertouch_boost, 
                                  PATCH_MOD_CONTROLLER_LOWER_BOUND, 
                                  PATCH_MOD_CONTROLLER_UPPER_BOUND)
  }
  else
    return 0;

  /* reload patch if a parameter was changed */
  if (param_changed == 1)
  {
    synth_load_patch(0, G_patch_edit_patch_index);
  }

  return 0;
}

/*******************************************************************************
** controls_pattern_parameter_adjust()
*******************************************************************************/
short int controls_pattern_parameter_adjust(int pattern_index, int param_index, int amount)
{
  int value;
  int param_changed;

  pattern* pt;
  param* pr;

  /* make sure that the pattern index is valid */
  if (BANK_PATTERN_INDEX_IS_NOT_VALID(pattern_index))
    return 1;

  /* make sure that the parameter index is valid */
  if (LAYOUT_PARAM_INDEX_IS_NOT_VALID(param_index))
    return 1;

  /* obtain patch and parameter pointers */
  pt = &G_pattern_bank[pattern_index];
  pr = &G_layout_params[param_index];

  /* determine value and adjustment mode */
  if ((pr->type == LAYOUT_PATTERN_EDIT_PARAM_TYPE_STEP_SPACE_STANDARD) || 
      (pr->type == LAYOUT_PATTERN_EDIT_PARAM_TYPE_STEP_SPACE_WIDE))
  {
    value = amount;
  }
  else if (pr->type == LAYOUT_PATTERN_EDIT_PARAM_TYPE_ARROWS)
    value = amount;
  else
    return 0;

  /* initialize change flag */
  param_changed = 0;

  /* swing */
  if (pr->label == LAYOUT_PATTERN_EDIT_PARAM_LABEL_SWING)
  {
    CONTROLS_SET_PATTERN_PARAMETER( pt->swings[0][0], 
                                    PATTERN_SWING_LOWER_BOUND, 
                                    PATTERN_SWING_UPPER_BOUND)
  }
  /* notes */
  else if (pr->label == LAYOUT_PATTERN_EDIT_PARAM_LABEL_NOTE_1)
  {
    CONTROLS_SET_PATTERN_PARAMETER( pt->in_steps[0][0].note[0], 
                                    PATTERN_INSTRUMENT_NOTE_LOWER_BOUND, 
                                    PATTERN_INSTRUMENT_NOTE_UPPER_BOUND)
  }
  else if (pr->label == LAYOUT_PATTERN_EDIT_PARAM_LABEL_NOTE_2)
  {
    CONTROLS_SET_PATTERN_PARAMETER( pt->in_steps[0][0].note[1], 
                                    PATTERN_INSTRUMENT_NOTE_LOWER_BOUND, 
                                    PATTERN_INSTRUMENT_NOTE_UPPER_BOUND)
  }
  else if (pr->label == LAYOUT_PATTERN_EDIT_PARAM_LABEL_NOTE_3)
  {
    CONTROLS_SET_PATTERN_PARAMETER( pt->in_steps[0][0].note[2], 
                                    PATTERN_INSTRUMENT_NOTE_LOWER_BOUND, 
                                    PATTERN_INSTRUMENT_NOTE_UPPER_BOUND)
  }
  else if (pr->label == LAYOUT_PATTERN_EDIT_PARAM_LABEL_NOTE_4)
  {
    CONTROLS_SET_PATTERN_PARAMETER( pt->in_steps[0][0].note[3], 
                                    PATTERN_INSTRUMENT_NOTE_LOWER_BOUND, 
                                    PATTERN_INSTRUMENT_NOTE_UPPER_BOUND)
  }
  /* key */
  else if (pr->label == LAYOUT_PATTERN_EDIT_PARAM_LABEL_KEY)
  {
    CONTROLS_SET_PATTERN_PARAMETER( pt->in_steps[0][0].key, 
                                    PATTERN_INSTRUMENT_KEY_LOWER_BOUND, 
                                    PATTERN_INSTRUMENT_KEY_UPPER_BOUND)
  }
  /* volume */
  else if (pr->label == LAYOUT_PATTERN_EDIT_PARAM_LABEL_VOLUME)
  {
    CONTROLS_SET_PATTERN_PARAMETER( pt->in_steps[0][0].volume, 
                                    PATTERN_INSTRUMENT_VOLUME_LOWER_BOUND, 
                                    PATTERN_INSTRUMENT_VOLUME_UPPER_BOUND)
  }
  /* mod wheel */
  else if (pr->label == LAYOUT_PATTERN_EDIT_PARAM_LABEL_MOD_WHEEL)
  {
    CONTROLS_SET_PATTERN_PARAMETER( pt->in_steps[0][0].mod_wheel_amount, 
                                    PATTERN_INSTRUMENT_MOD_WHEEL_LOWER_BOUND, 
                                    PATTERN_INSTRUMENT_MOD_WHEEL_UPPER_BOUND)
  }
  /* aftertouch */
  else if (pr->label == LAYOUT_PATTERN_EDIT_PARAM_LABEL_AFTERTOUCH)
  {
    CONTROLS_SET_PATTERN_PARAMETER( pt->in_steps[0][0].aftertouch_amount, 
                                    PATTERN_INSTRUMENT_AFTERTOUCH_LOWER_BOUND, 
                                    PATTERN_INSTRUMENT_AFTERTOUCH_UPPER_BOUND)
  }
  /* arpeggio / portamento */
  else if (pr->label == LAYOUT_PATTERN_EDIT_PARAM_LABEL_ARP_PORTA_MODE)
  {
    CONTROLS_SET_PATTERN_PARAMETER( pt->in_steps[0][0].arp_porta_mode, 
                                    PATTERN_INSTRUMENT_ARP_PORTA_MODE_LOWER_BOUND, 
                                    PATTERN_INSTRUMENT_ARP_PORTA_MODE_UPPER_BOUND)
  }
  else
    return 0;

#if 0
  /* reload pattern if a parameter was changed */
  if (param_changed == 1)
  {
    synth_load_pattern(G_patch_edit_pattern_index);
  }
#endif

  return 0;
}

/*******************************************************************************
** controls_keyboard_key_pressed()
*******************************************************************************/
short int controls_keyboard_key_pressed(SDL_Scancode code)
{
  /* escape */
  if ((code == SDL_SCANCODE_ESCAPE) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_ESCAPE)))
  {
    S_key_states[CONTROLS_KEY_INDEX_ESCAPE] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* decrease window size */
  if ((code == SDL_SCANCODE_LEFTBRACKET) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_DECREASE_WINDOW_SIZE)))
  {
    S_key_states[CONTROLS_KEY_INDEX_DECREASE_WINDOW_SIZE] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* increase window size */
  if ((code == SDL_SCANCODE_RIGHTBRACKET) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_INCREASE_WINDOW_SIZE)))
  {
    S_key_states[CONTROLS_KEY_INDEX_INCREASE_WINDOW_SIZE] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* scroll up */
  if ((code == SDL_SCANCODE_SEMICOLON) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_SCROLL_UP)))
  {
    S_key_states[CONTROLS_KEY_INDEX_SCROLL_UP] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* scroll down */
  if ((code == SDL_SCANCODE_APOSTROPHE) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_SCROLL_DOWN)))
  {
    S_key_states[CONTROLS_KEY_INDEX_SCROLL_DOWN] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* degree 1 */
  if ((code == SDL_SCANCODE_Z) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_DEGREE_1)))
  {
    S_key_states[CONTROLS_KEY_INDEX_DEGREE_1] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* degree 2 */
  if ((code == SDL_SCANCODE_X) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_DEGREE_2)))
  {
    S_key_states[CONTROLS_KEY_INDEX_DEGREE_2] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* degree 3 */
  if ((code == SDL_SCANCODE_C) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_DEGREE_3)))
  {
    S_key_states[CONTROLS_KEY_INDEX_DEGREE_3] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* degree 4 */
  if ((code == SDL_SCANCODE_V) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_DEGREE_4)))
  {
    S_key_states[CONTROLS_KEY_INDEX_DEGREE_4] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* degree 5 */
  if ((code == SDL_SCANCODE_B) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_DEGREE_5)))
  {
    S_key_states[CONTROLS_KEY_INDEX_DEGREE_5] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* degree 6 */
  if ((code == SDL_SCANCODE_N) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_DEGREE_6)))
  {
    S_key_states[CONTROLS_KEY_INDEX_DEGREE_6] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* degree 7 */
  if ((code == SDL_SCANCODE_M) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_DEGREE_7)))
  {
    S_key_states[CONTROLS_KEY_INDEX_DEGREE_7] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* degree 8 */
  if ((code == SDL_SCANCODE_COMMA) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_DEGREE_8)))
  {
    S_key_states[CONTROLS_KEY_INDEX_DEGREE_8] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* value 0 or key-off */
  if ((code == SDL_SCANCODE_GRAVE) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_VALUE_0_OR_KEY_OFF)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_0_OR_KEY_OFF] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* value 1 */
  if ((code == SDL_SCANCODE_1) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_VALUE_1)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_1] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* value 2 */
  if ((code == SDL_SCANCODE_2) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_VALUE_2)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_2] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* value 3 */
  if ((code == SDL_SCANCODE_3) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_VALUE_3)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_3] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* value 4 */
  if ((code == SDL_SCANCODE_4) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_VALUE_4)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_4] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* value 5 */
  if ((code == SDL_SCANCODE_5) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_VALUE_5)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_5] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* value 6 */
  if ((code == SDL_SCANCODE_6) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_VALUE_6)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_6] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* value 7 */
  if ((code == SDL_SCANCODE_7) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_VALUE_7)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_7] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* value 8 */
  if ((code == SDL_SCANCODE_8) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_VALUE_8)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_8] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* value 9 */
  if ((code == SDL_SCANCODE_Q) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_VALUE_9)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_9] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* value 10 */
  if ((code == SDL_SCANCODE_W) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_VALUE_10)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_10] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* value 11 */
  if ((code == SDL_SCANCODE_E) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_VALUE_11)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_11] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* value 12 */
  if ((code == SDL_SCANCODE_R) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_VALUE_12)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_12] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* value 13 */
  if ((code == SDL_SCANCODE_T) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_VALUE_13)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_13] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* value 14 */
  if ((code == SDL_SCANCODE_Y) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_VALUE_14)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_14] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* value 15 */
  if ((code == SDL_SCANCODE_U) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_VALUE_15)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_15] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* value 16 */
  if ((code == SDL_SCANCODE_I) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_VALUE_16)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_16] = CONTROLS_KEY_STATE_PRESSED;
  }

  /* blank */
  if ((code == SDL_SCANCODE_BACKSPACE) && 
      (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_BLANK)))
  {
    S_key_states[CONTROLS_KEY_INDEX_BLANK] = CONTROLS_KEY_STATE_PRESSED;
  }

  return 0;
}

/*******************************************************************************
** controls_keyboard_key_released()
*******************************************************************************/
short int controls_keyboard_key_released(SDL_Scancode code)
{
  /* escape */
  if ((code == SDL_SCANCODE_ESCAPE) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_ESCAPE)))
  {
    S_key_states[CONTROLS_KEY_INDEX_ESCAPE] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* decrease window size */
  if ((code == SDL_SCANCODE_LEFTBRACKET) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_DECREASE_WINDOW_SIZE)))
  {
    S_key_states[CONTROLS_KEY_INDEX_DECREASE_WINDOW_SIZE] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* increase window size */
  if ((code == SDL_SCANCODE_RIGHTBRACKET) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_INCREASE_WINDOW_SIZE)))
  {
    S_key_states[CONTROLS_KEY_INDEX_INCREASE_WINDOW_SIZE] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* scroll up */
  if ((code == SDL_SCANCODE_SEMICOLON) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_SCROLL_UP)))
  {
    S_key_states[CONTROLS_KEY_INDEX_SCROLL_UP] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* scroll down */
  if ((code == SDL_SCANCODE_APOSTROPHE) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_SCROLL_DOWN)))
  {
    S_key_states[CONTROLS_KEY_INDEX_SCROLL_DOWN] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* degree 1 */
  if ((code == SDL_SCANCODE_Z) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_DEGREE_1)))
  {
    S_key_states[CONTROLS_KEY_INDEX_DEGREE_1] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* degree 2 */
  if ((code == SDL_SCANCODE_X) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_DEGREE_2)))
  {
    S_key_states[CONTROLS_KEY_INDEX_DEGREE_2] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* degree 3 */
  if ((code == SDL_SCANCODE_C) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_DEGREE_3)))
  {
    S_key_states[CONTROLS_KEY_INDEX_DEGREE_3] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* degree 4 */
  if ((code == SDL_SCANCODE_V) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_DEGREE_4)))
  {
    S_key_states[CONTROLS_KEY_INDEX_DEGREE_4] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* degree 5 */
  if ((code == SDL_SCANCODE_B) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_DEGREE_5)))
  {
    S_key_states[CONTROLS_KEY_INDEX_DEGREE_5] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* degree 6 */
  if ((code == SDL_SCANCODE_N) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_DEGREE_6)))
  {
    S_key_states[CONTROLS_KEY_INDEX_DEGREE_6] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* degree 7 */
  if ((code == SDL_SCANCODE_M) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_DEGREE_7)))
  {
    S_key_states[CONTROLS_KEY_INDEX_DEGREE_7] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* degree 8 */
  if ((code == SDL_SCANCODE_COMMA) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_DEGREE_8)))
  {
    S_key_states[CONTROLS_KEY_INDEX_DEGREE_8] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* value 0 or key-off */
  if ((code == SDL_SCANCODE_GRAVE) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_VALUE_0_OR_KEY_OFF)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_0_OR_KEY_OFF] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* value 1 */
  if ((code == SDL_SCANCODE_1) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_VALUE_1)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_1] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* value 2 */
  if ((code == SDL_SCANCODE_2) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_VALUE_2)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_2] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* value 3 */
  if ((code == SDL_SCANCODE_3) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_VALUE_3)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_3] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* value 4 */
  if ((code == SDL_SCANCODE_4) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_VALUE_4)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_4] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* value 5 */
  if ((code == SDL_SCANCODE_5) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_VALUE_5)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_5] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* value 6 */
  if ((code == SDL_SCANCODE_6) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_VALUE_6)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_6] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* value 7 */
  if ((code == SDL_SCANCODE_7) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_VALUE_7)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_7] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* value 8 */
  if ((code == SDL_SCANCODE_8) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_VALUE_8)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_8] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* value 9 */
  if ((code == SDL_SCANCODE_Q) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_VALUE_9)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_9] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* value 10 */
  if ((code == SDL_SCANCODE_W) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_VALUE_10)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_10] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* value 11 */
  if ((code == SDL_SCANCODE_E) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_VALUE_11)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_11] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* value 12 */
  if ((code == SDL_SCANCODE_R) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_VALUE_12)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_12] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* value 13 */
  if ((code == SDL_SCANCODE_T) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_VALUE_13)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_13] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* value 14 */
  if ((code == SDL_SCANCODE_Y) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_VALUE_14)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_14] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* value 15 */
  if ((code == SDL_SCANCODE_U) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_VALUE_15)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_15] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* value 16 */
  if ((code == SDL_SCANCODE_I) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_VALUE_16)))
  {
    S_key_states[CONTROLS_KEY_INDEX_VALUE_16] = CONTROLS_KEY_STATE_RELEASED;
  }

  /* blank */
  if ((code == SDL_SCANCODE_BACKSPACE) && 
      (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_BLANK)))
  {
    S_key_states[CONTROLS_KEY_INDEX_BLANK] = CONTROLS_KEY_STATE_RELEASED;
  }

  return 0;
}

/*******************************************************************************
** controls_mouse_button_pressed()
*******************************************************************************/
short int controls_mouse_button_pressed(Uint8 button, Sint32 x, Sint32 y)
{
  /* find the mouse button location in overscan coordinates */
  S_mouse_remapped_pos_x = (x * GRAPHICS_OVERSCAN_WIDTH) / G_viewport_w;
  S_mouse_remapped_pos_y = (y * GRAPHICS_OVERSCAN_HEIGHT) / G_viewport_h;

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
  /* find the mouse button location in overscan coordinates */
  S_mouse_remapped_pos_x = (x * GRAPHICS_OVERSCAN_WIDTH) / G_viewport_w;
  S_mouse_remapped_pos_y = (y * GRAPHICS_OVERSCAN_HEIGHT) / G_viewport_h;

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

  int step;

  button* b;
  param*  pr;

  int value;

  /* check key states */

  /* escape */
  if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_ESCAPE))
  {

  }

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

  /* patches screen */
  if (G_game_screen == PROGRAM_SCREEN_PATCHES)
  {
    /* degrees */
    if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_DEGREE_1))
      G_common_edit_degree = 0;
    else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_DEGREE_2))
      G_common_edit_degree = 1;
    else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_DEGREE_3))
      G_common_edit_degree = 2;
    else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_DEGREE_4))
      G_common_edit_degree = 3;
    else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_DEGREE_5))
      G_common_edit_degree = 4;
    else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_DEGREE_6))
      G_common_edit_degree = 5;
    else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_DEGREE_7))
      G_common_edit_degree = 6;
    else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_DEGREE_8))
      G_common_edit_degree = 7;

    if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_DEGREE_1) || 
        CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_DEGREE_2) || 
        CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_DEGREE_3) || 
        CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_DEGREE_4) || 
        CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_DEGREE_5) || 
        CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_DEGREE_6) || 
        CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_DEGREE_7) || 
        CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_DEGREE_8))
    {
      synth_key_on( G_patch_edit_voice_index, 
                    G_common_edit_octave, 
                    G_common_edit_degree);
    }

    if (((CONTROLS_KEY_IS_RELEASED(CONTROLS_KEY_INDEX_DEGREE_1)) && (G_common_edit_degree == 0)) || 
        ((CONTROLS_KEY_IS_RELEASED(CONTROLS_KEY_INDEX_DEGREE_2)) && (G_common_edit_degree == 1)) || 
        ((CONTROLS_KEY_IS_RELEASED(CONTROLS_KEY_INDEX_DEGREE_3)) && (G_common_edit_degree == 2)) || 
        ((CONTROLS_KEY_IS_RELEASED(CONTROLS_KEY_INDEX_DEGREE_4)) && (G_common_edit_degree == 3)) || 
        ((CONTROLS_KEY_IS_RELEASED(CONTROLS_KEY_INDEX_DEGREE_5)) && (G_common_edit_degree == 4)) || 
        ((CONTROLS_KEY_IS_RELEASED(CONTROLS_KEY_INDEX_DEGREE_6)) && (G_common_edit_degree == 5)) || 
        ((CONTROLS_KEY_IS_RELEASED(CONTROLS_KEY_INDEX_DEGREE_7)) && (G_common_edit_degree == 6)) || 
        ((CONTROLS_KEY_IS_RELEASED(CONTROLS_KEY_INDEX_DEGREE_8)) && (G_common_edit_degree == 7)))
    {
      synth_key_off(G_patch_edit_voice_index);
    }
  }
  /* patterns screen */
  else if (G_game_screen == PROGRAM_SCREEN_PATTERNS)
  {
    /* instrument patterns */
    if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_0_OR_KEY_OFF))
      value = 0;
    else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_1))
      value = 1;
    else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_2))
      value = 2;
    else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_3))
      value = 3;
    else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_4))
      value = 4;
    else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_5))
      value = 5;
    else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_6))
      value = 6;
    else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_7))
      value = 7;
    else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_8))
      value = 8;
    else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_9))
      value = 9;
    else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_10))
      value = 10;
    else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_11))
      value = 11;
    else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_12))
      value = 12;
    else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_13))
      value = 13;
    else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_14))
      value = 14;
    else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_15))
      value = 15;
    else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_16))
      value = 16;
    else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_BLANK))
      value = -1;
    else
      value = 0;

    if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_0_OR_KEY_OFF)  || 
        CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_1)             || 
        CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_2)             || 
        CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_3)             || 
        CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_4)             || 
        CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_5)             || 
        CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_6)             || 
        CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_7)             || 
        CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_8)             || 
        CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_9)             || 
        CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_10)            || 
        CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_11)            || 
        CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_12)            || 
        CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_13)            || 
        CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_14)            || 
        CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_15)            || 
        CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_VALUE_16)            || 
        CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_BLANK))
    {
      controls_pattern_parameter_adjust(0, 
                                        G_pattern_edit_highlight_column + LAYOUT_PATTERN_EDIT_PARAM_FIRST_STEP_SPACE_LABEL, 
                                        value);
    }
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
      if ((S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH - 8 * b->width) / 2 + (4 * b->center_x))  && 
          (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH + 8 * b->width) / 2 + (4 * b->center_x))  && 
          (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT - 16 * 1) / 2 + (4 * b->center_y))       && 
          (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT + 16 * 1) / 2 + (4 * b->center_y)))
      {
        if (CONTROLS_MOUSE_BUTTON_IS_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT))
        {
          layout_reset_top_panel_button_states();

          b->state = LAYOUT_BUTTON_STATE_ON;

          if (b->label == LAYOUT_BUTTON_LABEL_PATCHES)
            program_loop_change_screen(PROGRAM_SCREEN_PATCHES);
          else if (b->label == LAYOUT_BUTTON_LABEL_PATTERNS)
            program_loop_change_screen(PROGRAM_SCREEN_PATTERNS);
        }

        break;
      }
    }
  }
  /* check for main area mouse button clicks */
  else if (CONTROLS_MOUSE_CURSOR_IS_OVER_MAIN_AREA())
  {
    /* vertical scrollbar */
    if (CONTROLS_MOUSE_CURSOR_IS_OVER_VERTICAL_SCROLLBAR_UP_ARROW() && 
        CONTROLS_MOUSE_BUTTON_IS_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT))
    {
      G_current_scroll_amount -= 2;

      if (G_current_scroll_amount < 0)
        G_current_scroll_amount = 0;
    }
    else if ( CONTROLS_MOUSE_CURSOR_IS_OVER_VERTICAL_SCROLLBAR_DOWN_ARROW() && 
              CONTROLS_MOUSE_BUTTON_IS_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT))
    {
      G_current_scroll_amount += 2;

      if (G_current_scroll_amount > G_max_scroll_amount)
        G_current_scroll_amount = G_max_scroll_amount;
    }
    else if ( CONTROLS_MOUSE_LAST_CLICK_WAS_OVER_VERTICAL_SCROLLBAR_TRACK() && 
              CONTROLS_MOUSE_BUTTON_IS_ON_OR_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT))
    {
      G_current_scroll_amount = G_max_scroll_amount;

      G_current_scroll_amount *= S_mouse_remapped_pos_y - CONTROLS_VERTICAL_SCROLLBAR_POS_Y_LOWER_BOUND;
      G_current_scroll_amount /= 8 * (LAYOUT_SCROLLBAR_HEIGHT - 4);

      if (G_current_scroll_amount < 0)
        G_current_scroll_amount = 0;

      if (G_current_scroll_amount > G_max_scroll_amount)
        G_current_scroll_amount = G_max_scroll_amount;
    }

    /* patches screen */
    if (G_game_screen == PROGRAM_SCREEN_PATCHES)
    {
      /* check for mouse wheel scrolling */
      if (S_mouse_wheel_movement != 0)
      {
        if (S_mouse_wheel_movement > 0)
          G_current_scroll_amount -= (S_mouse_wheel_movement / 8) + 1;
        else if (S_mouse_wheel_movement < 0)
          G_current_scroll_amount += (-S_mouse_wheel_movement / 8) + 1;

        if (G_current_scroll_amount < 0)
          G_current_scroll_amount = 0;
        else if (G_current_scroll_amount > G_max_scroll_amount)
          G_current_scroll_amount = G_max_scroll_amount;
      }

      /* check for parameter adjustment clicks */
      for ( k = LAYOUT_PATCH_EDIT_PARAMS_START_INDEX; 
            k < LAYOUT_PATCH_EDIT_PARAMS_END_INDEX; 
            k++)
      {
        pr = &G_layout_params[k];

        /* make sure this parameter is within the viewable area */
        if (LAYOUT_PATCH_PARAM_IS_NOT_IN_MAIN_AREA(pr))
          continue;

        /* if the cursor is over this parameter, adjust it! */
        if (pr->type == LAYOUT_PATCH_EDIT_PARAM_TYPE_SLIDER)
        {
          if (CONTROLS_MOUSE_LAST_CLICK_WAS_OVER_PATCH_PARAM_SLIDER() && 
              CONTROLS_MOUSE_BUTTON_IS_ON_OR_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT))
          {
            controls_patch_parameter_adjust(0, k, S_mouse_remapped_pos_x - CONTROLS_PATCH_PARAM_SLIDER_POS_X_LOWER_BOUND);
          }
        }
        else if (pr->type == LAYOUT_PATCH_EDIT_PARAM_TYPE_ARROWS)
        {
          if (CONTROLS_MOUSE_CURSOR_IS_OVER_PATCH_PARAM_ARROWS_LEFT() && 
              CONTROLS_MOUSE_BUTTON_IS_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT))
          {
            controls_patch_parameter_adjust(0, k, -1);
          }
          else if ( CONTROLS_MOUSE_CURSOR_IS_OVER_PATCH_PARAM_ARROWS_RIGHT() && 
                    CONTROLS_MOUSE_BUTTON_IS_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT))
          {
            controls_patch_parameter_adjust(0, k, 1);
          }
        }
        else if (pr->type == LAYOUT_PATCH_EDIT_PARAM_TYPE_RADIO)
        {
          if (CONTROLS_MOUSE_CURSOR_IS_OVER_PATCH_PARAM_RADIO_BUTTON() && 
              CONTROLS_MOUSE_BUTTON_IS_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT))
          {
            controls_patch_parameter_adjust(0, k, 0);
          }
        }
      }
    }
    /* pattern screen */
    else if (G_game_screen == PROGRAM_SCREEN_PATTERNS)
    {
      /* check for mouse wheel scrolling */
      if (S_mouse_wheel_movement != 0)
      {
        if (S_mouse_wheel_movement > 0)
          G_current_scroll_amount -= (S_mouse_wheel_movement / 8) + 1;
        else if (S_mouse_wheel_movement < 0)
          G_current_scroll_amount += (-S_mouse_wheel_movement / 8) + 1;

        if (G_current_scroll_amount < 0)
          G_current_scroll_amount = 0;
        else if (G_current_scroll_amount > G_max_scroll_amount)
          G_current_scroll_amount = G_max_scroll_amount;
      }

      /* check for parameter adjustment clicks */
      for ( k = LAYOUT_PATTERN_EDIT_PARAMS_START_INDEX; 
            k < LAYOUT_PATTERN_EDIT_PARAMS_END_INDEX; 
            k++)
      {
        pr = &G_layout_params[k];

        /* make sure this parameter is within the viewable area */
        if (LAYOUT_PATTERN_PARAM_IS_NOT_IN_MAIN_AREA(pr))
          continue;

        /* if the cursor is over this parameter, adjust it! */
        if (pr->type == LAYOUT_PATTERN_EDIT_PARAM_TYPE_STEP_SPACE_STANDARD)
        {
          if (CONTROLS_MOUSE_CURSOR_IS_OVER_PATTERN_PARAM_STEP_SPACE_STANDARD() && 
              CONTROLS_MOUSE_BUTTON_IS_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT))
          {
            G_pattern_edit_highlight_column = pr->num;
          }
        }
        else if (pr->type == LAYOUT_PATTERN_EDIT_PARAM_TYPE_STEP_SPACE_WIDE)
        {
          if (CONTROLS_MOUSE_CURSOR_IS_OVER_PATTERN_PARAM_STEP_SPACE_WIDE() && 
              CONTROLS_MOUSE_BUTTON_IS_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT))
          {
            G_pattern_edit_highlight_column = pr->num;
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

