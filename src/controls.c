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
  CONTROLS_KEY_INDEX_PATCH_EDIT_MODIFIER_OCTAVE, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_SET_OCTAVE_1, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_SET_OCTAVE_2, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_SET_OCTAVE_3, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_SET_OCTAVE_4, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_SET_OCTAVE_5, 
  CONTROLS_KEY_INDEX_PATCH_EDIT_SET_OCTAVE_6, 
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

/* screen regions */
#define CONTROLS_MOUSE_CURSOR_IS_OVER_TOP_PANEL()                                                                                 \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (LAYOUT_TOP_PANEL_AREA_X - LAYOUT_TOP_PANEL_AREA_WIDTH))   &&  \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (LAYOUT_TOP_PANEL_AREA_X + LAYOUT_TOP_PANEL_AREA_WIDTH))   &&  \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (LAYOUT_TOP_PANEL_AREA_Y - LAYOUT_TOP_PANEL_AREA_HEIGHT)) &&  \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (LAYOUT_TOP_PANEL_AREA_Y + LAYOUT_TOP_PANEL_AREA_HEIGHT)))

#define CONTROLS_MOUSE_CURSOR_IS_OVER_CART_MAIN_AREA()                                                                            \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (LAYOUT_CART_MAIN_AREA_X - LAYOUT_CART_MAIN_AREA_WIDTH))   &&  \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (LAYOUT_CART_MAIN_AREA_X + LAYOUT_CART_MAIN_AREA_WIDTH))   &&  \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (LAYOUT_CART_MAIN_AREA_Y - LAYOUT_CART_MAIN_AREA_HEIGHT)) &&  \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (LAYOUT_CART_MAIN_AREA_Y + LAYOUT_CART_MAIN_AREA_HEIGHT)))

#define CONTROLS_MOUSE_CURSOR_IS_OVER_CART_AUDITION_PANEL()                                                                                 \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (LAYOUT_CART_AUDITION_PANEL_X - LAYOUT_CART_AUDITION_PANEL_WIDTH))   &&  \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (LAYOUT_CART_AUDITION_PANEL_X + LAYOUT_CART_AUDITION_PANEL_WIDTH))   &&  \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (LAYOUT_CART_AUDITION_PANEL_Y - LAYOUT_CART_AUDITION_PANEL_HEIGHT)) &&  \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (LAYOUT_CART_AUDITION_PANEL_Y + LAYOUT_CART_AUDITION_PANEL_HEIGHT)))

/* vertical scrollbar */
#define CONTROLS_MOUSE_CURSOR_IS_OVER_VERTICAL_SCROLLBAR_UP_ARROW()                                                                 \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (LAYOUT_SCROLLBAR_AREA_X - LAYOUT_SCROLLBAR_AREA_WIDTH))     &&  \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (LAYOUT_SCROLLBAR_AREA_X + LAYOUT_SCROLLBAR_AREA_WIDTH))     &&  \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (LAYOUT_SCROLLBAR_AREA_Y - LAYOUT_SCROLLBAR_AREA_HEIGHT))   &&  \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (LAYOUT_SCROLLBAR_AREA_Y - LAYOUT_SCROLLBAR_AREA_HEIGHT + 2)))

#define CONTROLS_MOUSE_CURSOR_IS_OVER_VERTICAL_SCROLLBAR_DOWN_ARROW()                                                                 \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (LAYOUT_SCROLLBAR_AREA_X - LAYOUT_SCROLLBAR_AREA_WIDTH))       &&  \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (LAYOUT_SCROLLBAR_AREA_X + LAYOUT_SCROLLBAR_AREA_WIDTH))       &&  \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (LAYOUT_SCROLLBAR_AREA_Y + LAYOUT_SCROLLBAR_AREA_HEIGHT - 2)) &&  \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (LAYOUT_SCROLLBAR_AREA_Y + LAYOUT_SCROLLBAR_AREA_HEIGHT)))

#define CONTROLS_MOUSE_CURSOR_IS_OVER_VERTICAL_SCROLLBAR_TRACK()                                                                      \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (LAYOUT_SCROLLBAR_AREA_X - LAYOUT_SCROLLBAR_AREA_WIDTH))       &&  \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (LAYOUT_SCROLLBAR_AREA_X + LAYOUT_SCROLLBAR_AREA_WIDTH))       &&  \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (LAYOUT_SCROLLBAR_AREA_Y - LAYOUT_SCROLLBAR_AREA_HEIGHT + 2)) &&  \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (LAYOUT_SCROLLBAR_AREA_Y + LAYOUT_SCROLLBAR_AREA_HEIGHT - 2)))

#define CONTROLS_MOUSE_LAST_CLICK_WAS_OVER_VERTICAL_SCROLLBAR_TRACK()                                                                   \
  ( (S_mouse_last_click_pos_x >= (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (LAYOUT_SCROLLBAR_AREA_X - LAYOUT_SCROLLBAR_AREA_WIDTH))       &&  \
    (S_mouse_last_click_pos_x <  (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (LAYOUT_SCROLLBAR_AREA_X + LAYOUT_SCROLLBAR_AREA_WIDTH))       &&  \
    (S_mouse_last_click_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (LAYOUT_SCROLLBAR_AREA_Y - LAYOUT_SCROLLBAR_AREA_HEIGHT + 2)) &&  \
    (S_mouse_last_click_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (LAYOUT_SCROLLBAR_AREA_Y + LAYOUT_SCROLLBAR_AREA_HEIGHT - 2)))

#define CONTROLS_VERTICAL_SCROLLBAR_POS_Y_LOWER_BOUND                          \
  ((GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (LAYOUT_SCROLLBAR_AREA_Y - (LAYOUT_SCROLLBAR_AREA_HEIGHT - 3) + 1))

/* buttons */
#define CONTROLS_MOUSE_CURSOR_IS_OVER_BUTTON()                                                        \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH - 8 * b->width) / 2 + (4 * b->center_x))  &&  \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH + 8 * b->width) / 2 + (4 * b->center_x))  &&  \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT - 16 * 1) / 2 + (4 * b->center_y))       &&  \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT + 16 * 1) / 2 + (4 * b->center_y)))

/* patch edit parameter silders, adjustment arrows, and radio buttons */
#define CONTROLS_MOUSE_CURSOR_IS_OVER_PATCH_PARAM_SLIDER()                                                                                                              \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (pr->center_x + LAYOUT_CART_PARAM_SLIDER_TRACK_X - LAYOUT_CART_PARAM_SLIDER_WIDTH))  &&  \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (pr->center_x + LAYOUT_CART_PARAM_SLIDER_TRACK_X + LAYOUT_CART_PARAM_SLIDER_WIDTH))  &&  \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (pos_y - 1))                                                                                    &&  \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (pos_y + 1)))

#define CONTROLS_MOUSE_LAST_CLICK_WAS_OVER_PATCH_PARAM_SLIDER()                                                                                                           \
  ( (S_mouse_last_click_pos_x >= (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (pr->center_x + LAYOUT_CART_PARAM_SLIDER_TRACK_X - LAYOUT_CART_PARAM_SLIDER_WIDTH))  &&  \
    (S_mouse_last_click_pos_x <  (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (pr->center_x + LAYOUT_CART_PARAM_SLIDER_TRACK_X + LAYOUT_CART_PARAM_SLIDER_WIDTH))  &&  \
    (S_mouse_last_click_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (pos_y - 1))                                                                                    &&  \
    (S_mouse_last_click_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (pos_y + 1)))

#define CONTROLS_PATCH_PARAM_SLIDER_POS_X_LOWER_BOUND                          \
  ((GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (pr->center_x + LAYOUT_CART_PARAM_SLIDER_TRACK_X - LAYOUT_CART_PARAM_SLIDER_WIDTH))

#define CONTROLS_MOUSE_CURSOR_IS_OVER_PATCH_PARAM_ARROWS_LEFT()                                                                     \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (pr->center_x + LAYOUT_CART_PARAM_ARROWS_LEFT_X - 1))  &&  \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (pr->center_x + LAYOUT_CART_PARAM_ARROWS_LEFT_X + 1))  &&  \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (pos_y - 1))                                                &&  \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (pos_y + 1)))

#define CONTROLS_MOUSE_CURSOR_IS_OVER_PATCH_PARAM_ARROWS_RIGHT()                                                                    \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (pr->center_x + LAYOUT_CART_PARAM_ARROWS_RIGHT_X - 1)) &&  \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (pr->center_x + LAYOUT_CART_PARAM_ARROWS_RIGHT_X + 1)) &&  \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (pos_y - 1))                                                &&  \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (pos_y + 1)))

#define CONTROLS_MOUSE_CURSOR_IS_OVER_PATCH_PARAM_RADIO_BUTTON()                                                                    \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (pr->center_x + LAYOUT_CART_PARAM_RADIO_BUTTON_X - 1)) &&  \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH / 2) + 4 * (pr->center_x + LAYOUT_CART_PARAM_RADIO_BUTTON_X + 1)) &&  \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (pos_y - 1))                                                &&  \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT / 2) + 4 * (pos_y + 1)))

/* parameter adjustment */
#define CONTROLS_SET_PATCH_PARAMETER(param, name)                              \
  if ((pr->type == LAYOUT_PARAM_TYPE_SLIDER) &&                     \
      (param != value))                                                        \
  {                                                                            \
    param = value;                                                             \
    param_changed = 1;                                                         \
  }                                                                            \
  else if (pr->type == LAYOUT_PARAM_TYPE_ARROWS)                    \
  {                                                                            \
    param += value;                                                            \
    param_changed = 1;                                                         \
  }                                                                            \
  else if (pr->type == LAYOUT_PARAM_TYPE_RADIO)                     \
  {                                                                            \
    if (param != name##_LOWER_BOUND)                                           \
      param = name##_LOWER_BOUND;                                              \
    else                                                                       \
      param = name##_UPPER_BOUND;                                              \
                                                                               \
    param_changed = 1;                                                         \
  }                                                                            \
                                                                               \
  if (param < name##_LOWER_BOUND)                                              \
    param = name##_LOWER_BOUND;                                                \
  else if (param > name##_UPPER_BOUND)                                         \
    param = name##_UPPER_BOUND;

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
                                          int param_index, int amount)
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
    value = (amount * (pr->upper_bound - pr->lower_bound)) / (8 * (LAYOUT_CART_PARAM_SLIDER_WIDTH - 1)) + pr->lower_bound;
  else if (pr->type == LAYOUT_PARAM_TYPE_ARROWS)
    value = amount;
  else if (pr->type == LAYOUT_PARAM_TYPE_RADIO)
    value = 0;
  else
    return 0;

  /* initialize change flag */
  param_changed = 0;

  /* cart number */
  if (pr->label == LAYOUT_CART_PARAM_LABEL_CART_NUMBER)
  {
    CONTROLS_SET_PATCH_PARAMETER(G_patch_edit_cart_number, PATCH_CART_NO)
  }
  /* patch number */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_PATCH_NUMBER)
  {
    CONTROLS_SET_PATCH_PARAMETER(G_patch_edit_patch_number, PATCH_PATCH_NO)
  }
  /* algorithm */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ALGORITHM)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->algorithm, PATCH_ALGORITHM)
  }
  /* filters */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_HIGHPASS_CUTOFF)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->highpass_cutoff, PATCH_HIGHPASS_CUTOFF)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_LOWPASS_CUTOFF)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->lowpass_cutoff, PATCH_LOWPASS_CUTOFF)
  }
  /* oscillator */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_WAVEFORM)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->osc_waveform[pr->num], PATCH_OSC_WAVEFORM)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_FEEDBACK)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->osc_feedback[pr->num], PATCH_OSC_FEEDBACK)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_PHI)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->osc_phi[pr->num], PATCH_OSC_PHI)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_FREQ_MODE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->osc_freq_mode[pr->num], PATCH_OSC_FREQ_MODE)
  }
  else if ( (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_MULTIPLE) && 
            (pc->osc_freq_mode[pr->num] == PATCH_OSC_FREQ_MODE_RATIO))
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->osc_multiple[pr->num], PATCH_OSC_MULTIPLE)
  }
  else if ( (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_DIVISOR) && 
            (pc->osc_freq_mode[pr->num] == PATCH_OSC_FREQ_MODE_RATIO))
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->osc_divisor[pr->num], PATCH_OSC_DIVISOR)
  }
  else if ( (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_NOTE) && 
            (pc->osc_freq_mode[pr->num] == PATCH_OSC_FREQ_MODE_FIXED))
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->osc_note[pr->num], PATCH_OSC_NOTE)
  }
  else if ( (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_OCTAVE) && 
            (pc->osc_freq_mode[pr->num] == PATCH_OSC_FREQ_MODE_FIXED))
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->osc_octave[pr->num], PATCH_OSC_OCTAVE)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_DETUNE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->osc_detune[pr->num], PATCH_OSC_DETUNE)
  }
  /* envelope */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_ATTACK)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_attack[pr->num], PATCH_ENV_RATE)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_DECAY_1)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_decay_1[pr->num], PATCH_ENV_RATE)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_DECAY_2)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_decay_2[pr->num], PATCH_ENV_RATE)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_RELEASE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_release[pr->num], PATCH_ENV_RATE)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_AMPLITUDE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_amplitude[pr->num], PATCH_ENV_AMPLITUDE)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_SUSTAIN)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_sustain[pr->num], PATCH_ENV_SUSTAIN)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_RATE_KS)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_rate_ks[pr->num], PATCH_ENV_KEYSCALING)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_LEVEL_KS)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_level_ks[pr->num], PATCH_ENV_KEYSCALING)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_BREAK_POINT)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->env_break_point[pr->num], PATCH_ENV_BREAK_POINT)
  }
  /* lfo */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_LFO_WAVEFORM)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->lfo_waveform, PATCH_LFO_WAVEFORM)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_LFO_FREQUENCY)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->lfo_frequency, PATCH_LFO_FREQUENCY)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_LFO_DELAY)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->lfo_delay, PATCH_LFO_DELAY)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_LFO_QUANTIZE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->lfo_quantize, PATCH_LFO_QUANTIZE)
  }
  /* portamento */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_PORTAMENTO_MODE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->portamento_mode, PATCH_PORTAMENTO_MODE)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_PORTAMENTO_LEGATO)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->portamento_legato, PATCH_PORTAMENTO_LEGATO)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_PORTAMENTO_SPEED)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->portamento_speed, PATCH_PORTAMENTO_SPEED)
  }
  /* arpeggio */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ARPEGGIO_MODE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->arpeggio_mode, PATCH_ARPEGGIO_MODE)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ARPEGGIO_PATTERN)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->arpeggio_pattern, PATCH_ARPEGGIO_PATTERN)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_ARPEGGIO_SPEED)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->arpeggio_speed, PATCH_ARPEGGIO_SPEED)
  }
  /* sync */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_SYNC_OSC)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->sync_osc, PATCH_SYNC)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_SYNC_LFO)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->sync_lfo, PATCH_SYNC)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_SYNC_ARP)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->sync_arp, PATCH_SYNC)
  }
  /* sustain pedal */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_PEDAL_ADJUST)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->pedal_adjust, PATCH_PEDAL_ADJUST)
  }
  /* pitch wheel */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_WHEEL_MODE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->pitch_wheel_mode, PATCH_PITCH_WHEEL_MODE)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_WHEEL_RANGE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->pitch_wheel_range, PATCH_PITCH_WHEEL_RANGE)
  }
  /* velocity */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_VELOCITY_MODE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->velocity_mode, PATCH_VELOCITY_MODE)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_VELOCITY_SCALING)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->velocity_scaling, PATCH_VELOCITY_SCALING)
  }
  /* noise */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_NOISE_MODE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->noise_mode, PATCH_NOISE_MODE)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_NOISE_FREQUENCY)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->noise_frequency, PATCH_NOISE_FREQUENCY)
  }
  /* vibrato */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_VIBRATO_MODE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->vibrato_mode, PATCH_VIBRATO_MODE)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_VIBRATO_DEPTH)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->vibrato_depth, PATCH_EFFECT_DEPTH)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_VIBRATO_BASE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->vibrato_base, PATCH_EFFECT_BASE)
  }
  /* tremolo */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_MODE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->tremolo_mode, PATCH_TREMOLO_MODE)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_DEPTH)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->tremolo_depth, PATCH_EFFECT_DEPTH)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_BASE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->tremolo_base, PATCH_EFFECT_BASE)
  }
  /* boost */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_BOOST_MODE)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->boost_mode, PATCH_BOOST_MODE)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_BOOST_DEPTH)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->boost_depth, PATCH_EFFECT_DEPTH)
  }
  /* effects */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_MOD_WHEEL_EFFECT)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->mod_wheel_effect, PATCH_CONTROLLER_EFFECT)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_AFTERTOUCH_EFFECT)
  {
    CONTROLS_SET_PATCH_PARAMETER(pc->aftertouch_effect, PATCH_CONTROLLER_EFFECT)
  }
  /* audition */
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_VELOCITY)
  {
    CONTROLS_SET_PATCH_PARAMETER(G_patch_edit_note_velocity, MIDI_CONT_NOTE_VELOCITY)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_MOD_WHEEL)
  {
    CONTROLS_SET_PATCH_PARAMETER(G_patch_edit_mod_wheel_pos, MIDI_CONT_MOD_WHEEL)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_AFTERTOUCH)
  {
    CONTROLS_SET_PATCH_PARAMETER(G_patch_edit_aftertouch_pos, MIDI_CONT_AFTERTOUCH)
  }
  else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_PITCH_WHEEL)
  {
    CONTROLS_SET_PATCH_PARAMETER(G_patch_edit_pitch_wheel_pos, MIDI_CONT_PITCH_WHEEL)
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
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_MOD_WHEEL)
      instrument_set_mod_wheel_position(G_patch_edit_instrument_index, G_patch_edit_mod_wheel_pos);
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_AFTERTOUCH)
      instrument_set_aftertouch_position(G_patch_edit_instrument_index, G_patch_edit_aftertouch_pos);
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_PITCH_WHEEL)
      instrument_set_pitch_wheel_position(G_patch_edit_instrument_index, G_patch_edit_pitch_wheel_pos);
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

  /* patch edit: set octave */
  CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_LCTRL, 
                                            CONTROLS_KEY_INDEX_PATCH_EDIT_MODIFIER_OCTAVE)

  if (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_PATCH_EDIT_MODIFIER_OCTAVE))
  {
    CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_1, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_SET_OCTAVE_1)

    CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_2, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_SET_OCTAVE_2)

    CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_3, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_SET_OCTAVE_3)

    CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_4, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_SET_OCTAVE_4)

    CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_5, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_SET_OCTAVE_5)

    CONTROLS_KEYBOARD_PRESS_UPDATE_KEY_STATES(SDL_SCANCODE_6, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_SET_OCTAVE_6)
  }

  /* patch edit: notes (this octave) */
  if (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_PATCH_EDIT_MODIFIER_OCTAVE))
  {
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
  }

  /* patch edit: notes (next octave) */
  if (CONTROLS_KEY_IS_OFF_OR_RELEASED(CONTROLS_KEY_INDEX_PATCH_EDIT_MODIFIER_OCTAVE))
  {
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
  }

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

  /* patch edit: set octave */
  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_LCTRL, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_MODIFIER_OCTAVE)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_1, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_SET_OCTAVE_1)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_2, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_SET_OCTAVE_2)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_3, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_SET_OCTAVE_3)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_4, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_SET_OCTAVE_4)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_5, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_SET_OCTAVE_5)

  CONTROLS_KEYBOARD_RELEASE_UPDATE_KEY_STATES(SDL_SCANCODE_6, 
                                              CONTROLS_KEY_INDEX_PATCH_EDIT_SET_OCTAVE_6)

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
    if (CONTROLS_KEY_IS_ON_OR_PRESSED(CONTROLS_KEY_INDEX_PATCH_EDIT_MODIFIER_OCTAVE))
    {
      if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_PATCH_EDIT_SET_OCTAVE_1))
        G_patch_edit_octave = TUNING_MIDDLE_OCTAVE - 3;
      else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_PATCH_EDIT_SET_OCTAVE_2))
        G_patch_edit_octave = TUNING_MIDDLE_OCTAVE - 2;
      else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_PATCH_EDIT_SET_OCTAVE_3))
        G_patch_edit_octave = TUNING_MIDDLE_OCTAVE - 1;
      else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_PATCH_EDIT_SET_OCTAVE_4))
        G_patch_edit_octave = TUNING_MIDDLE_OCTAVE + 0;
      else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_PATCH_EDIT_SET_OCTAVE_5))
        G_patch_edit_octave = TUNING_MIDDLE_OCTAVE + 1;
      else if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_PATCH_EDIT_SET_OCTAVE_6))
        G_patch_edit_octave = TUNING_MIDDLE_OCTAVE + 2;
    }

    for ( k = CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_C; 
          k <= CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_D_PRIME;
          k++)
    {
      if (CONTROLS_KEY_IS_PRESSED(k))
      {
        instrument_key_on(G_patch_edit_instrument_index, 
                          12 * G_patch_edit_octave + (k - CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_C));
      }
      else if (CONTROLS_KEY_IS_RELEASED(k))
      {
        instrument_key_off( G_patch_edit_instrument_index, 
                            12 * G_patch_edit_octave + (k - CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_THIS_OCTAVE_C));
      }
    }

    for ( k = CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_C; 
          k <= CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_D_PRIME;
          k++)
    {
      if (CONTROLS_KEY_IS_PRESSED(k))
      {
        instrument_key_on(G_patch_edit_instrument_index, 
                          12 * (G_patch_edit_octave + 1) + (k - CONTROLS_KEY_INDEX_PATCH_EDIT_NOTE_NEXT_OCTAVE_C));
      }
      else if (CONTROLS_KEY_IS_RELEASED(k))
      {
        instrument_key_off( G_patch_edit_instrument_index, 
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
      instrument_set_sustain_pedal(G_patch_edit_instrument_index, MIDI_CONT_PEDAL_STATE_DOWN);
    else if (CONTROLS_KEY_IS_RELEASED(CONTROLS_KEY_INDEX_PATCH_EDIT_SUSTAIN_PEDAL))
      instrument_set_sustain_pedal(G_patch_edit_instrument_index, MIDI_CONT_PEDAL_STATE_UP);
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
      G_current_scroll_amount /= 8 * (LAYOUT_SCROLLBAR_AREA_HEIGHT - 4);

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
          G_current_scroll_amount -= (S_mouse_wheel_movement / 8) + 1;
        else if (S_mouse_wheel_movement < 0)
          G_current_scroll_amount += (-S_mouse_wheel_movement / 8) + 1;

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
        if ((pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_VELOCITY)    || 
            (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_MOD_WHEEL)   || 
            (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_AFTERTOUCH)  || 
            (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_PITCH_WHEEL))
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
            controls_patch_parameter_adjust(G_patch_edit_cart_number, G_patch_edit_patch_number, 
                                            k, S_mouse_remapped_pos_x - CONTROLS_PATCH_PARAM_SLIDER_POS_X_LOWER_BOUND);
          }
        }
        else if (pr->type == LAYOUT_PARAM_TYPE_ARROWS)
        {
          if (CONTROLS_MOUSE_CURSOR_IS_OVER_PATCH_PARAM_ARROWS_LEFT() && 
              CONTROLS_MOUSE_BUTTON_IS_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT))
          {
            controls_patch_parameter_adjust(G_patch_edit_cart_number, G_patch_edit_patch_number, 
                                            k, -1);
          }
          else if ( CONTROLS_MOUSE_CURSOR_IS_OVER_PATCH_PARAM_ARROWS_RIGHT() && 
                    CONTROLS_MOUSE_BUTTON_IS_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT))
          {
            controls_patch_parameter_adjust(G_patch_edit_cart_number, G_patch_edit_patch_number, 
                                            k, 1);
          }
        }
        else if (pr->type == LAYOUT_PARAM_TYPE_RADIO)
        {
          if (CONTROLS_MOUSE_CURSOR_IS_OVER_PATCH_PARAM_RADIO_BUTTON() && 
              CONTROLS_MOUSE_BUTTON_IS_PRESSED(CONTROLS_MOUSE_BUTTON_INDEX_LEFT))
          {
            controls_patch_parameter_adjust(G_patch_edit_cart_number, G_patch_edit_patch_number, 
                                            k, 0);
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

