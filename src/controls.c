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
#include "progloop.h"
#include "screen.h"
#include "synth.h"

enum
{
  CONTROLS_KEY_INDEX_ESCAPE = 0, 
  CONTROLS_KEY_INDEX_DECREASE_WINDOW_SIZE, 
  CONTROLS_KEY_INDEX_INCREASE_WINDOW_SIZE, 
  CONTROLS_KEY_INDEX_DEGREE_1, 
  CONTROLS_KEY_INDEX_DEGREE_2, 
  CONTROLS_KEY_INDEX_DEGREE_3, 
  CONTROLS_KEY_INDEX_DEGREE_4, 
  CONTROLS_KEY_INDEX_DEGREE_5, 
  CONTROLS_KEY_INDEX_DEGREE_6, 
  CONTROLS_KEY_INDEX_DEGREE_7, 
  CONTROLS_KEY_INDEX_DEGREE_8, 
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
  CONTROLS_MOUSE_ACTION_NONE = 0,
  CONTROLS_MOUSE_ACTION_LEFT_CLICK, 
  CONTROLS_MOUSE_ACTION_RIGHT_CLICK, 
  CONTROLS_MOUSE_ACTION_WHEEL_MOVED
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

/* buttons */
#define CONTROLS_MOUSE_CURSOR_IS_OVER_BUTTON(name)                                                                                    \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH - 8 * LAYOUT_##name##_BUTTON_WIDTH) / 2 + (4 * LAYOUT_##name##_BUTTON_X)) &&  \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH + 8 * LAYOUT_##name##_BUTTON_WIDTH) / 2 + (4 * LAYOUT_##name##_BUTTON_X)) &&  \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT - 16 * 1) / 2 + (4 * LAYOUT_##name##_BUTTON_Y))                          &&  \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT + 16 * 1) / 2 + (4 * LAYOUT_##name##_BUTTON_Y)))

/* patch editor algorithm */
#define CONTROLS_MOUSE_CURSOR_IS_OVER_ALGORITHM_VALUE()                                                                 \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH - 8 * 11) / 2 + (4 * LAYOUT_PATCH_EDIT_ALGORITHM_PARAM_X))  &&  \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH + 8 * 11) / 2 + (4 * LAYOUT_PATCH_EDIT_ALGORITHM_PARAM_X))  &&  \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT - 8 * 1) / 2 + (4 * LAYOUT_PATCH_EDIT_ALGORITHM_Y))        &&  \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT + 8 * 1) / 2 + (4 * LAYOUT_PATCH_EDIT_ALGORITHM_Y)))

#define CONTROLS_MOUSE_CURSOR_IS_OVER_ALGORITHM_ADJUST_LEFT()                                                               \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH - 8 * 1) / 2 + (4 * LAYOUT_PATCH_EDIT_ALGORITHM_ADJUST_LEFT_X)) &&  \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH + 8 * 1) / 2 + (4 * LAYOUT_PATCH_EDIT_ALGORITHM_ADJUST_LEFT_X)) &&  \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT - 8 * 1) / 2 + (4 * LAYOUT_PATCH_EDIT_ALGORITHM_Y))            &&  \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT + 8 * 1) / 2 + (4 * LAYOUT_PATCH_EDIT_ALGORITHM_Y)))

#define CONTROLS_MOUSE_CURSOR_IS_OVER_ALGORITHM_ADJUST_RIGHT()                                                                \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH - 8 * 1) / 2 + (4 * LAYOUT_PATCH_EDIT_ALGORITHM_ADJUST_RIGHT_X))  &&  \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH + 8 * 1) / 2 + (4 * LAYOUT_PATCH_EDIT_ALGORITHM_ADJUST_RIGHT_X))  &&  \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT - 8 * 1) / 2 + (4 * LAYOUT_PATCH_EDIT_ALGORITHM_Y))              &&  \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT + 8 * 1) / 2 + (4 * LAYOUT_PATCH_EDIT_ALGORITHM_Y)))

#define CONTROLS_MOUSE_CURSOR_IS_OVER_ALGORITHM_ENTIRE()                                                                \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH - 8 * 13) / 2 + (4 * LAYOUT_PATCH_EDIT_ALGORITHM_PARAM_X))  &&  \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH + 8 * 13) / 2 + (4 * LAYOUT_PATCH_EDIT_ALGORITHM_PARAM_X))  &&  \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT - 8 * 1) / 2 + (4 * LAYOUT_PATCH_EDIT_ALGORITHM_Y))        &&  \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT + 8 * 1) / 2 + (4 * LAYOUT_PATCH_EDIT_ALGORITHM_Y)))

/* patch editor parameter */
#define CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(column, name)                                                         \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH - 8 * 3) / 2 + (4 * LAYOUT_PATCH_EDIT_##column##_PARAM_X))  &&  \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH + 8 * 3) / 2 + (4 * LAYOUT_PATCH_EDIT_##column##_PARAM_X))  &&  \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT - 8 * 1) / 2 + (4 * LAYOUT_PATCH_EDIT_PARAM_##name##_Y))   &&  \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT + 8 * 1) / 2 + (4 * LAYOUT_PATCH_EDIT_PARAM_##name##_Y)))

#define CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(column, name)                                                         \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH - 8 * 1) / 2 + (4 * LAYOUT_PATCH_EDIT_##column##_ADJUST_LEFT_X)) &&   \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH + 8 * 1) / 2 + (4 * LAYOUT_PATCH_EDIT_##column##_ADJUST_LEFT_X)) &&   \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT - 8 * 1) / 2 + (4 * LAYOUT_PATCH_EDIT_PARAM_##name##_Y))        &&   \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT + 8 * 1) / 2 + (4 * LAYOUT_PATCH_EDIT_PARAM_##name##_Y)))

#define CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(column, name)                                                        \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH - 8 * 1) / 2 + (4 * LAYOUT_PATCH_EDIT_##column##_ADJUST_RIGHT_X)) &&  \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH + 8 * 1) / 2 + (4 * LAYOUT_PATCH_EDIT_##column##_ADJUST_RIGHT_X)) &&  \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT - 8 * 1) / 2 + (4 * LAYOUT_PATCH_EDIT_PARAM_##name##_Y))         &&  \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT + 8 * 1) / 2 + (4 * LAYOUT_PATCH_EDIT_PARAM_##name##_Y)))

#define CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(column, name)                                                        \
  ( (S_mouse_remapped_pos_x >= (GRAPHICS_OVERSCAN_WIDTH - 8 * 5) / 2 + (4 * LAYOUT_PATCH_EDIT_##column##_PARAM_X))  &&  \
    (S_mouse_remapped_pos_x <  (GRAPHICS_OVERSCAN_WIDTH + 8 * 5) / 2 + (4 * LAYOUT_PATCH_EDIT_##column##_PARAM_X))  &&  \
    (S_mouse_remapped_pos_y >= (GRAPHICS_OVERSCAN_HEIGHT - 8 * 1) / 2 + (4 * LAYOUT_PATCH_EDIT_PARAM_##name##_Y))   &&  \
    (S_mouse_remapped_pos_y <  (GRAPHICS_OVERSCAN_HEIGHT + 8 * 1) / 2 + (4 * LAYOUT_PATCH_EDIT_PARAM_##name##_Y)))

static int S_key_states[CONTROLS_NUM_KEY_INDICES];

static int S_mouse_remapped_pos_x;
static int S_mouse_remapped_pos_y;

static int S_mouse_action;
static int S_mouse_wheel_movement;

/*******************************************************************************
** controls_setup()
*******************************************************************************/
short int controls_setup()
{
  int i;

  for (i = 0; i < CONTROLS_NUM_KEY_INDICES; i++)
  {
    S_key_states[i] = CONTROLS_KEY_STATE_OFF;
  }

  S_mouse_remapped_pos_x = 0;
  S_mouse_remapped_pos_y = 0;

  S_mouse_action = CONTROLS_MOUSE_ACTION_NONE;
  S_mouse_wheel_movement = 0;

  return 0;
}

/*******************************************************************************
** controls_patch_parameter_adjustment()
*******************************************************************************/
short int controls_patch_parameter_adjustment(int amount)
{
  /* patches screen */
  if (G_game_screen == PROGRAM_SCREEN_PATCHES)
  {
    /* algorithm */
    if (CONTROLS_MOUSE_CURSOR_IS_OVER_ALGORITHM_ENTIRE())
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_ALGORITHM, amount);
    /* filters */
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_1, FILTERS_HIGHPASS))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_HIGHPASS_CUTOFF, amount);
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_2, FILTERS_LOWPASS))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_LOWPASS_CUTOFF, amount);
    /* modulator envelope */
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_1, MOD_ENV_ATTACK))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_MOD_ENV_ATTACK, amount);
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_1, MOD_ENV_DECAY_1))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_MOD_ENV_DECAY_1, amount);
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_1, MOD_ENV_DECAY_2))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_MOD_ENV_DECAY_2, amount);
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_1, MOD_ENV_RELEASE))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_MOD_ENV_RELEASE, amount);
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_1, MOD_ENV_SUSTAIN))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_MOD_ENV_SUSTAIN, amount);
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_1, MOD_ENV_RATE_KS))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_MOD_ENV_RATE_KS, amount);
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_1, MOD_ENV_LEVEL_KS))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_MOD_ENV_LEVEL_KS, amount);
    /* carrier envelope */
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_2, CAR_ENV_ATTACK))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_CAR_ENV_ATTACK, amount);
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_2, CAR_ENV_DECAY_1))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_CAR_ENV_DECAY_1, amount);
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_2, CAR_ENV_DECAY_2))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_CAR_ENV_DECAY_2, amount);
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_2, CAR_ENV_RELEASE))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_CAR_ENV_RELEASE, amount);
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_2, CAR_ENV_SUSTAIN))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_CAR_ENV_SUSTAIN, amount);
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_2, CAR_ENV_RATE_KS))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_CAR_ENV_RATE_KS, amount);
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_2, CAR_ENV_LEVEL_KS))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_CAR_ENV_LEVEL_KS, amount);
    /* oscillator 1 */
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_3, OSC_1_FEEDBACK))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_OSC_1_FEEDBACK, amount);
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_3, OSC_1_MULTIPLE))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_OSC_1_MULTIPLE, amount);
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_3, OSC_1_DETUNE))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_OSC_1_DETUNE, amount);
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_3, OSC_1_LEVEL))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_OSC_1_AMPLITUDE, amount);
    /* oscillator 2 */
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_4, OSC_2_FEEDBACK))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_OSC_2_FEEDBACK, amount);
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_4, OSC_2_MULTIPLE))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_OSC_2_MULTIPLE, amount);
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_4, OSC_2_DETUNE))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_OSC_2_DETUNE, amount);
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_4, OSC_2_LEVEL))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_OSC_2_AMPLITUDE, amount);
    /* oscillator 3 */
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_3, OSC_3_FEEDBACK))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_OSC_3_FEEDBACK, amount);
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_3, OSC_3_MULTIPLE))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_OSC_3_MULTIPLE, amount);
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_3, OSC_3_DETUNE))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_OSC_3_DETUNE, amount);
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_3, OSC_3_LEVEL))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_OSC_3_AMPLITUDE, amount);
    /* oscillator 4 */
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_4, OSC_4_FEEDBACK))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_OSC_4_FEEDBACK, amount);
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_4, OSC_4_MULTIPLE))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_OSC_4_MULTIPLE, amount);
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_4, OSC_4_DETUNE))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_OSC_4_DETUNE, amount);
    else if (CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ENTIRE(COLUMN_4, OSC_4_LEVEL))
      patch_adjust_parameter(G_patch_edit_patch_index, PATCH_PARAM_OSC_4_AMPLITUDE, amount);

    /* testing */
    synth_load_patch(0, G_patch_edit_patch_index);
  }

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

  /* set mouse action */
  if (button == SDL_BUTTON_LEFT)
    S_mouse_action = CONTROLS_MOUSE_ACTION_LEFT_CLICK;
  else if (button == SDL_BUTTON_RIGHT)
    S_mouse_action = CONTROLS_MOUSE_ACTION_RIGHT_CLICK;

  S_mouse_wheel_movement = 0;

  return 0;
}

/*******************************************************************************
** controls_mouse_wheel_moved()
*******************************************************************************/
short int controls_mouse_wheel_moved(Sint32 movement)
{
  int mouse_x;
  int mouse_y;

  /* obtain current mouse position */
  SDL_GetMouseState(&mouse_x, &mouse_y);

  /* find the mouse button location in overscan coordinates */
  S_mouse_remapped_pos_x = (mouse_x * GRAPHICS_OVERSCAN_WIDTH) / G_viewport_w;
  S_mouse_remapped_pos_y = (mouse_y * GRAPHICS_OVERSCAN_HEIGHT) / G_viewport_h;

  /* set mouse action */
  S_mouse_action = CONTROLS_MOUSE_ACTION_WHEEL_MOVED;

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

  return 0;
}

/*******************************************************************************
** controls_process_user_input_standard()
*******************************************************************************/
short int controls_process_user_input_standard()
{
  int amount;

  /* check top panel mouse button clicks */
  if (G_game_screen == PROGRAM_SCREEN_PATCHES)
  {
    if (S_mouse_action == CONTROLS_MOUSE_ACTION_LEFT_CLICK)
    {
      if (CONTROLS_MOUSE_CURSOR_IS_OVER_BUTTON(TOP_PANEL_PATTERNS))
        program_loop_change_screen(PROGRAM_SCREEN_PATTERNS);
    }
  }
  else if (G_game_screen == PROGRAM_SCREEN_PATTERNS)
  {
    if (S_mouse_action == CONTROLS_MOUSE_ACTION_LEFT_CLICK)
    {
      if (CONTROLS_MOUSE_CURSOR_IS_OVER_BUTTON(TOP_PANEL_PATCHES))
        program_loop_change_screen(PROGRAM_SCREEN_PATCHES);
    }
  }

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

  /* degree 1 */
  if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_DEGREE_1))
  {
    G_patch_edit_degree = 0;

    synth_key_on( G_patch_edit_voice_index, 
                  G_patch_edit_octave, 
                  G_patch_edit_degree, 
                  G_patch_edit_volume, 
                  G_patch_edit_brightness);
  }
  else if ( (CONTROLS_KEY_IS_RELEASED(CONTROLS_KEY_INDEX_DEGREE_1)) && 
            (G_patch_edit_degree == 0))
  {
    synth_key_off(G_patch_edit_voice_index);
  }

  /* degree 2 */
  if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_DEGREE_2))
  {
    G_patch_edit_degree = 1;

    synth_key_on( G_patch_edit_voice_index, 
                  G_patch_edit_octave, 
                  G_patch_edit_degree, 
                  G_patch_edit_volume, 
                  G_patch_edit_brightness);
  }
  else if ( (CONTROLS_KEY_IS_RELEASED(CONTROLS_KEY_INDEX_DEGREE_2)) && 
            (G_patch_edit_degree == 1))
  {
    synth_key_off(G_patch_edit_voice_index);
  }

  /* degree 3 */
  if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_DEGREE_3))
  {
    G_patch_edit_degree = 2;

    synth_key_on( G_patch_edit_voice_index, 
                  G_patch_edit_octave, 
                  G_patch_edit_degree, 
                  G_patch_edit_volume, 
                  G_patch_edit_brightness);
  }
  else if ( (CONTROLS_KEY_IS_RELEASED(CONTROLS_KEY_INDEX_DEGREE_3)) && 
            (G_patch_edit_degree == 2))
  {
    synth_key_off(G_patch_edit_voice_index);
  }

  /* degree 4 */
  if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_DEGREE_4))
  {
    G_patch_edit_degree = 3;

    synth_key_on( G_patch_edit_voice_index, 
                  G_patch_edit_octave, 
                  G_patch_edit_degree, 
                  G_patch_edit_volume, 
                  G_patch_edit_brightness);
  }
  else if ( (CONTROLS_KEY_IS_RELEASED(CONTROLS_KEY_INDEX_DEGREE_4)) && 
            (G_patch_edit_degree == 3))
  {
    synth_key_off(G_patch_edit_voice_index);
  }

  /* degree 5 */
  if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_DEGREE_5))
  {
    G_patch_edit_degree = 4;

    synth_key_on( G_patch_edit_voice_index, 
                  G_patch_edit_octave, 
                  G_patch_edit_degree, 
                  G_patch_edit_volume, 
                  G_patch_edit_brightness);
  }
  else if ( (CONTROLS_KEY_IS_RELEASED(CONTROLS_KEY_INDEX_DEGREE_5)) && 
            (G_patch_edit_degree == 4))
  {
    synth_key_off(G_patch_edit_voice_index);
  }

  /* degree 6 */
  if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_DEGREE_6))
  {
    G_patch_edit_degree = 5;

    synth_key_on( G_patch_edit_voice_index, 
                  G_patch_edit_octave, 
                  G_patch_edit_degree, 
                  G_patch_edit_volume, 
                  G_patch_edit_brightness);
  }
  else if ( (CONTROLS_KEY_IS_RELEASED(CONTROLS_KEY_INDEX_DEGREE_6)) && 
            (G_patch_edit_degree == 5))
  {
    synth_key_off(G_patch_edit_voice_index);
  }

  /* degree 7 */
  if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_DEGREE_7))
  {
    G_patch_edit_degree = 6;

    synth_key_on( G_patch_edit_voice_index, 
                  G_patch_edit_octave, 
                  G_patch_edit_degree, 
                  G_patch_edit_volume, 
                  G_patch_edit_brightness);
  }
  else if ( (CONTROLS_KEY_IS_RELEASED(CONTROLS_KEY_INDEX_DEGREE_7)) && 
            (G_patch_edit_degree == 6))
  {
    synth_key_off(G_patch_edit_voice_index);
  }

  /* degree 8 */
  if (CONTROLS_KEY_IS_PRESSED(CONTROLS_KEY_INDEX_DEGREE_8))
  {
    G_patch_edit_degree = 7;

    synth_key_on( G_patch_edit_voice_index, 
                  G_patch_edit_octave, 
                  G_patch_edit_degree, 
                  G_patch_edit_volume, 
                  G_patch_edit_brightness);
  }
  else if ( (CONTROLS_KEY_IS_RELEASED(CONTROLS_KEY_INDEX_DEGREE_8)) && 
            (G_patch_edit_degree == 7))
  {
    synth_key_off(G_patch_edit_voice_index);
  }

  /* remove edges */
  controls_remove_edges();

  /* check mouse state */
  if ((S_mouse_action == CONTROLS_MOUSE_ACTION_LEFT_CLICK)  || 
      (S_mouse_action == CONTROLS_MOUSE_ACTION_RIGHT_CLICK) || 
      (S_mouse_action == CONTROLS_MOUSE_ACTION_WHEEL_MOVED))
  {
    /* set initial amount */
    if (S_mouse_action == CONTROLS_MOUSE_ACTION_LEFT_CLICK)
      amount = 1;
    else if (S_mouse_action == CONTROLS_MOUSE_ACTION_RIGHT_CLICK)
      amount = 4;
    else if (S_mouse_action == CONTROLS_MOUSE_ACTION_WHEEL_MOVED)
    {
      if (S_mouse_wheel_movement > 0)
        amount = (S_mouse_wheel_movement / 8) + 1;
      else if (S_mouse_wheel_movement < 0)
        amount = (S_mouse_wheel_movement / 8) - 1;
      else
        amount = 0;
    }
    else
      amount = 0;

    /* patches screen */
    if (G_game_screen == PROGRAM_SCREEN_PATCHES)
    {
      /* left / right click */
      if ((S_mouse_action == CONTROLS_MOUSE_ACTION_LEFT_CLICK) || 
          (S_mouse_action == CONTROLS_MOUSE_ACTION_RIGHT_CLICK))
      {
        if (CONTROLS_MOUSE_CURSOR_IS_OVER_ALGORITHM_ADJUST_LEFT()                       || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_1, MOD_ENV_ATTACK)   || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_1, MOD_ENV_DECAY_1)  || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_1, MOD_ENV_DECAY_2)  || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_1, MOD_ENV_RELEASE)  || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_1, MOD_ENV_SUSTAIN)  || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_1, MOD_ENV_RATE_KS)  || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_1, MOD_ENV_LEVEL_KS) || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_2, CAR_ENV_ATTACK)   || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_2, CAR_ENV_DECAY_1)  || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_2, CAR_ENV_DECAY_2)  || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_2, CAR_ENV_RELEASE)  || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_2, CAR_ENV_SUSTAIN)  || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_2, CAR_ENV_RATE_KS)  || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_2, CAR_ENV_LEVEL_KS) || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_1, FILTERS_HIGHPASS) || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_2, FILTERS_LOWPASS)  || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_3, OSC_1_FEEDBACK)   || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_3, OSC_1_MULTIPLE)   || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_3, OSC_1_DETUNE)     || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_3, OSC_1_LEVEL)      || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_4, OSC_2_FEEDBACK)   || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_4, OSC_2_MULTIPLE)   || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_4, OSC_2_DETUNE)     || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_4, OSC_2_LEVEL)      || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_3, OSC_3_FEEDBACK)   || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_3, OSC_3_MULTIPLE)   || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_3, OSC_3_DETUNE)     || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_3, OSC_3_LEVEL)      || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_4, OSC_4_FEEDBACK)   || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_4, OSC_4_MULTIPLE)   || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_4, OSC_4_DETUNE)     || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_LEFT(COLUMN_4, OSC_4_LEVEL))
        {
          amount *= -1;
        }
        else if ( CONTROLS_MOUSE_CURSOR_IS_OVER_ALGORITHM_ADJUST_RIGHT()                        || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_1, MOD_ENV_ATTACK)    || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_1, MOD_ENV_DECAY_1)   || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_1, MOD_ENV_DECAY_2)   || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_1, MOD_ENV_RELEASE)   || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_1, MOD_ENV_SUSTAIN)   || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_1, MOD_ENV_RATE_KS)   || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_1, MOD_ENV_LEVEL_KS)  || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_2, CAR_ENV_ATTACK)    || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_2, CAR_ENV_DECAY_1)   || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_2, CAR_ENV_DECAY_2)   || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_2, CAR_ENV_RELEASE)   || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_2, CAR_ENV_SUSTAIN)   || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_2, CAR_ENV_RATE_KS)   || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_2, CAR_ENV_LEVEL_KS)  || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_1, FILTERS_HIGHPASS)  || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_2, FILTERS_LOWPASS)   || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_3, OSC_1_FEEDBACK)    || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_3, OSC_1_MULTIPLE)    || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_3, OSC_1_DETUNE)      || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_3, OSC_1_LEVEL)       || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_4, OSC_2_FEEDBACK)    || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_4, OSC_2_MULTIPLE)    || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_4, OSC_2_DETUNE)      || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_4, OSC_2_LEVEL)       || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_3, OSC_3_FEEDBACK)    || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_3, OSC_3_MULTIPLE)    || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_3, OSC_3_DETUNE)      || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_3, OSC_3_LEVEL)       || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_4, OSC_4_FEEDBACK)    || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_4, OSC_4_MULTIPLE)    || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_4, OSC_4_DETUNE)      || 
                  CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_ADJUST_RIGHT(COLUMN_4, OSC_4_LEVEL))
        {
          amount *= 1;
        }
        else
          amount = 0;
      }
      /* mouse wheel movement */
      else if (S_mouse_action == CONTROLS_MOUSE_ACTION_WHEEL_MOVED)
      {
        if (CONTROLS_MOUSE_CURSOR_IS_OVER_ALGORITHM_VALUE()                         || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_1, MOD_ENV_ATTACK)     || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_1, MOD_ENV_DECAY_1)    || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_1, MOD_ENV_DECAY_2)    || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_1, MOD_ENV_RELEASE)    || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_1, MOD_ENV_SUSTAIN)    || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_1, MOD_ENV_RATE_KS)    || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_1, MOD_ENV_LEVEL_KS)   || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_2, CAR_ENV_ATTACK)     || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_2, CAR_ENV_DECAY_1)    || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_2, CAR_ENV_DECAY_2)    || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_2, CAR_ENV_RELEASE)    || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_2, CAR_ENV_SUSTAIN)    || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_2, CAR_ENV_RATE_KS)    || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_2, CAR_ENV_LEVEL_KS)   || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_1, FILTERS_HIGHPASS)   || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_2, FILTERS_LOWPASS)    || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_3, OSC_1_FEEDBACK)     || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_3, OSC_1_MULTIPLE)     || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_3, OSC_1_DETUNE)       || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_3, OSC_1_LEVEL)        || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_4, OSC_2_FEEDBACK)     || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_4, OSC_2_MULTIPLE)     || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_4, OSC_2_DETUNE)       || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_4, OSC_2_LEVEL)        || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_3, OSC_3_FEEDBACK)     || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_3, OSC_3_MULTIPLE)     || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_3, OSC_3_DETUNE)       || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_3, OSC_3_LEVEL)        || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_4, OSC_4_FEEDBACK)     || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_4, OSC_4_MULTIPLE)     || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_4, OSC_4_DETUNE)       || 
            CONTROLS_MOUSE_CURSOR_IS_OVER_PARAM_VALUE(COLUMN_4, OSC_4_LEVEL))
        {
          amount *= 1;
        }
        else
          amount = 0;
      }

      /* update parameter if necessary */
      if (amount != 0)
        controls_patch_parameter_adjustment(amount);
    }
  }

  /* reset mouse action */
  S_mouse_action = CONTROLS_MOUSE_ACTION_NONE;
  S_mouse_wheel_movement = 0;

  return 0;
}

