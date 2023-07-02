/*******************************************************************************
** layout.c (screen layout widget positions)
*******************************************************************************/

#include <stdio.h>  /* testing */
#include <stdlib.h>

#include "layout.h"
#include "patch.h"

panel  G_layout_panels[LAYOUT_NUM_PANELS];
button G_layout_buttons[LAYOUT_NUM_BUTTONS];

header G_layout_headers[LAYOUT_NUM_HEADERS];
param  G_layout_params[LAYOUT_NUM_PARAMS];

/*******************************************************************************
** layout_setup_panels()
*******************************************************************************/
short int layout_setup_panels()
{
  int k;

  panel* pn;

  /* top & bottom panels */
  for (k = 0; k < LAYOUT_NUM_PANELS; k++)
  {
    pn = &G_layout_panels[k];

    if (k == 0)
    {
      pn->type = LAYOUT_PANEL_TYPE_NORMAL;

      pn->center_x = LAYOUT_PANEL_TOP_X;
      pn->center_y = LAYOUT_PANEL_TOP_Y;

      pn->width = LAYOUT_PANEL_TOP_WIDTH;
      pn->height = LAYOUT_PANEL_TOP_HEIGHT;
    }
    else if (k == 1)
    {
      pn->type = LAYOUT_PANEL_TYPE_THIN;

      pn->center_x = LAYOUT_PANEL_BOTTOM_X;
      pn->center_y = LAYOUT_PANEL_BOTTOM_Y;

      pn->width = LAYOUT_PANEL_BOTTOM_WIDTH;
      pn->height = LAYOUT_PANEL_BOTTOM_HEIGHT;
    }
    else
    {
      pn->type = LAYOUT_PANEL_TYPE_NORMAL;

      pn->center_x = LAYOUT_PANEL_TOP_X;
      pn->center_y = LAYOUT_PANEL_TOP_Y;

      pn->width = LAYOUT_PANEL_TOP_WIDTH;
      pn->height = LAYOUT_PANEL_TOP_HEIGHT;
    }
  }

  return 0;
}

/*******************************************************************************
** layout_setup_buttons()
*******************************************************************************/
short int layout_setup_buttons()
{
  int k;

  button* b;

  /* top panel buttons */
  for ( k = LAYOUT_TOP_PANEL_BUTTONS_START_INDEX; 
        k < LAYOUT_TOP_PANEL_BUTTONS_END_INDEX; 
        k++)
  {
    b = &G_layout_buttons[k];

    if (k == LAYOUT_TOP_PANEL_BUTTONS_START_INDEX + 0)
    {
      b->label = LAYOUT_BUTTON_LABEL_PATCHES;

      b->center_x = LAYOUT_TOP_PANEL_BUTTON_PATCHES_X;
      b->center_y = LAYOUT_TOP_PANEL_BUTTON_PATCHES_Y;

      b->width = LAYOUT_TOP_PANEL_BUTTON_PATCHES_WIDTH;
      b->state = LAYOUT_BUTTON_STATE_ON;
    }
    else if (k == LAYOUT_TOP_PANEL_BUTTONS_START_INDEX + 1)
    {
      b->label = LAYOUT_BUTTON_LABEL_PATTERNS;

      b->center_x = LAYOUT_TOP_PANEL_BUTTON_PATTERNS_X;
      b->center_y = LAYOUT_TOP_PANEL_BUTTON_PATTERNS_Y;

      b->width = LAYOUT_TOP_PANEL_BUTTON_PATTERNS_WIDTH;
      b->state = LAYOUT_BUTTON_STATE_OFF;
    }
    else
    {
      b->label = LAYOUT_BUTTON_LABEL_NONE;

      b->center_x = 0;
      b->center_y = 0;

      b->width = 2;
      b->state = LAYOUT_BUTTON_STATE_OFF;
    }
  }

  return 0;
}

/*******************************************************************************
** layout_setup_headers()
*******************************************************************************/
short int layout_setup_headers()
{
  int k;

  header* hd;

  /* top panel headers */
  for ( k = LAYOUT_TOP_PANEL_HEADERS_START_INDEX; 
        k < LAYOUT_TOP_PANEL_HEADERS_END_INDEX; 
        k++)
  {
    hd = &G_layout_headers[k];

    if (k == LAYOUT_TOP_PANEL_HEADERS_START_INDEX + 0)
    {
      hd->label = LAYOUT_HEADER_TOP_PANEL_LABEL_NAME;

      hd->center_x = LAYOUT_TOP_PANEL_HEADER_NAME_X;
      hd->center_y = LAYOUT_TOP_PANEL_HEADER_NAME_Y;
    }
    else if (k == LAYOUT_TOP_PANEL_HEADERS_START_INDEX + 1)
    {
      hd->label = LAYOUT_HEADER_TOP_PANEL_LABEL_VERSION;

      hd->center_x = LAYOUT_TOP_PANEL_HEADER_VERSION_X;
      hd->center_y = LAYOUT_TOP_PANEL_HEADER_VERSION_Y;
    }
    else
    {
      hd->label = LAYOUT_HEADER_TOP_PANEL_LABEL_NONE;

      hd->center_x = 0;
      hd->center_y = 0;
    }
  }

  /* bottom panel headers */
  for ( k = LAYOUT_BOTTOM_PANEL_HEADERS_START_INDEX; 
        k < LAYOUT_BOTTOM_PANEL_HEADERS_END_INDEX; 
        k++)
  {
    hd = &G_layout_headers[k];

    if (k == LAYOUT_BOTTOM_PANEL_HEADERS_START_INDEX + 0)
    {
      hd->label = LAYOUT_HEADER_BOTTOM_PANEL_LABEL_OCTAVE;

      hd->center_x = LAYOUT_BOTTOM_PANEL_HEADER_OCTAVE_X;
      hd->center_y = LAYOUT_BOTTOM_PANEL_HEADER_OCTAVE_Y;
    }
    else if (k == LAYOUT_BOTTOM_PANEL_HEADERS_START_INDEX + 1)
    {
      hd->label = LAYOUT_HEADER_BOTTOM_PANEL_LABEL_KEY;

      hd->center_x = LAYOUT_BOTTOM_PANEL_HEADER_KEY_X;
      hd->center_y = LAYOUT_BOTTOM_PANEL_HEADER_KEY_Y;
    }
    else
    {
      hd->label = LAYOUT_HEADER_BOTTOM_PANEL_LABEL_NONE;

      hd->center_x = 0;
      hd->center_y = 0;
    }
  }

  /* patch edit headers */
  for ( k = LAYOUT_PATCH_EDIT_HEADERS_START_INDEX; 
        k < LAYOUT_PATCH_EDIT_HEADERS_END_INDEX; 
        k++)
  {
    hd = &G_layout_headers[k];

    if (k == LAYOUT_PATCH_EDIT_HEADERS_START_INDEX + 0)
    {
      hd->label = LAYOUT_HEADER_PATCH_EDIT_LABEL_OSC_1;

      hd->center_x = LAYOUT_PATCH_EDIT_COLUMN_1_CENTER_X;
      hd->center_y = LAYOUT_PATCH_EDIT_HEADER_OSC_Y;
    }
    else if (k == LAYOUT_PATCH_EDIT_HEADERS_START_INDEX + 1)
    {
      hd->label = LAYOUT_HEADER_PATCH_EDIT_LABEL_OSC_2;

      hd->center_x = LAYOUT_PATCH_EDIT_COLUMN_2_CENTER_X;
      hd->center_y = LAYOUT_PATCH_EDIT_HEADER_OSC_Y;
    }
    else if (k == LAYOUT_PATCH_EDIT_HEADERS_START_INDEX + 2)
    {
      hd->label = LAYOUT_HEADER_PATCH_EDIT_LABEL_OSC_3;

      hd->center_x = LAYOUT_PATCH_EDIT_COLUMN_3_CENTER_X;
      hd->center_y = LAYOUT_PATCH_EDIT_HEADER_OSC_Y;
    }
    else if (k == LAYOUT_PATCH_EDIT_HEADERS_START_INDEX + 3)
    {
      hd->label = LAYOUT_HEADER_PATCH_EDIT_LABEL_OSC_4;

      hd->center_x = LAYOUT_PATCH_EDIT_COLUMN_4_CENTER_X;
      hd->center_y = LAYOUT_PATCH_EDIT_HEADER_OSC_Y;
    }
    else if (k == LAYOUT_PATCH_EDIT_HEADERS_START_INDEX + 4)
    {
      hd->label = LAYOUT_HEADER_PATCH_EDIT_LABEL_ENV_1;

      hd->center_x = LAYOUT_PATCH_EDIT_COLUMN_1_CENTER_X;
      hd->center_y = LAYOUT_PATCH_EDIT_HEADER_ENV_Y;
    }
    else if (k == LAYOUT_PATCH_EDIT_HEADERS_START_INDEX + 5)
    {
      hd->label = LAYOUT_HEADER_PATCH_EDIT_LABEL_ENV_2;

      hd->center_x = LAYOUT_PATCH_EDIT_COLUMN_2_CENTER_X;
      hd->center_y = LAYOUT_PATCH_EDIT_HEADER_ENV_Y;
    }
    else if (k == LAYOUT_PATCH_EDIT_HEADERS_START_INDEX + 6)
    {
      hd->label = LAYOUT_HEADER_PATCH_EDIT_LABEL_ENV_3;

      hd->center_x = LAYOUT_PATCH_EDIT_COLUMN_3_CENTER_X;
      hd->center_y = LAYOUT_PATCH_EDIT_HEADER_ENV_Y;
    }
    else if (k == LAYOUT_PATCH_EDIT_HEADERS_START_INDEX + 7)
    {
      hd->label = LAYOUT_HEADER_PATCH_EDIT_LABEL_ENV_4;

      hd->center_x = LAYOUT_PATCH_EDIT_COLUMN_4_CENTER_X;
      hd->center_y = LAYOUT_PATCH_EDIT_HEADER_ENV_Y;
    }
    else
    {
      hd->label = LAYOUT_HEADER_PATCH_EDIT_LABEL_NONE;

      hd->center_x = 0;
      hd->center_y = 0;
    }
  }

  return 0;
}
/*******************************************************************************
** layout_setup_params()
*******************************************************************************/
short int layout_setup_params()
{
  int k;

  int shifted_index;

  param* pr;

  /* patch edit params */
  for ( k = LAYOUT_PATCH_EDIT_PARAMS_START_INDEX; 
        k < LAYOUT_PATCH_EDIT_PARAMS_END_INDEX; 
        k++)
  {
    pr = &G_layout_params[k];

    /* determine label and oscillator / envelope number */
    shifted_index = k - LAYOUT_PATCH_EDIT_PARAMS_START_INDEX;

    if ((shifted_index >= 0) && (shifted_index < LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_FEEDBACK))
    {
      pr->label = shifted_index;
      pr->num = 0;
    }
    else
    {
      pr->label = (shifted_index - LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_FEEDBACK) % LAYOUT_PARAM_PATCH_EDIT_NUM_OSC_ENV_LABELS;
      pr->label += LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_FEEDBACK;

      pr->num = (shifted_index - LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_FEEDBACK) / LAYOUT_PARAM_PATCH_EDIT_NUM_OSC_ENV_LABELS;
    }

    /* set position and bounds */
    if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_ALGORITHM)
    {
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_1_CENTER_X;
      pr->center_y = LAYOUT_PATCH_EDIT_ALGORITHM_Y;

      pr->lower_bound = PATCH_ALGORITHM_LOWER_BOUND;
      pr->upper_bound = PATCH_ALGORITHM_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_LOWPASS_CUTOFF)
    {
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_4_CENTER_X;
      pr->center_y = LAYOUT_PATCH_EDIT_FILTER_CUTOFF_Y;

      pr->lower_bound = PATCH_LOWPASS_CUTOFF_LOWER_BOUND;
      pr->upper_bound = PATCH_LOWPASS_CUTOFF_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_HIGHPASS_CUTOFF)
    {
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_3_CENTER_X;
      pr->center_y = LAYOUT_PATCH_EDIT_FILTER_CUTOFF_Y;

      pr->lower_bound = PATCH_HIGHPASS_CUTOFF_LOWER_BOUND;
      pr->upper_bound = PATCH_HIGHPASS_CUTOFF_UPPER_BOUND;
    }
    else
    {
      /* determine column */
      if (pr->num == 0)
        pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_1_CENTER_X;
      else if (pr->num == 1)
        pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_2_CENTER_X;
      else if (pr->num == 2)
        pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_3_CENTER_X;
      else if (pr->num == 3)
        pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_4_CENTER_X;
      else
        pr->center_x = 0;

      /* determine vertical position and bounds */
      if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_FEEDBACK)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_OSC_FEEDBACK_Y;

        pr->lower_bound = PATCH_OSC_FEEDBACK_LOWER_BOUND;
        pr->upper_bound = PATCH_OSC_FEEDBACK_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_MULTIPLE)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_OSC_MULTIPLE_Y;

        pr->lower_bound = PATCH_OSC_MULTIPLE_LOWER_BOUND;
        pr->upper_bound = PATCH_OSC_MULTIPLE_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_DETUNE)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_OSC_DETUNE_Y;

        pr->lower_bound = PATCH_OSC_DETUNE_LOWER_BOUND;
        pr->upper_bound = PATCH_OSC_DETUNE_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_AMPLITUDE)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_OSC_AMPLITUDE_Y;

        pr->lower_bound = PATCH_OSC_AMPLITUDE_LOWER_BOUND;
        pr->upper_bound = PATCH_OSC_AMPLITUDE_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_ATTACK)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_ENV_ATTACK_Y;

        pr->lower_bound = PATCH_ENV_ATTACK_LOWER_BOUND;
        pr->upper_bound = PATCH_ENV_ATTACK_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_DECAY_1)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_ENV_DECAY_1_Y;

        pr->lower_bound = PATCH_ENV_DECAY_1_LOWER_BOUND;
        pr->upper_bound = PATCH_ENV_DECAY_1_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_DECAY_2)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_ENV_DECAY_2_Y;

        pr->lower_bound = PATCH_ENV_DECAY_2_LOWER_BOUND;
        pr->upper_bound = PATCH_ENV_DECAY_2_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_RELEASE)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_ENV_RELEASE_Y;

        pr->lower_bound = PATCH_ENV_RELEASE_LOWER_BOUND;
        pr->upper_bound = PATCH_ENV_RELEASE_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_SUSTAIN)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_ENV_SUSTAIN_Y;

        pr->lower_bound = PATCH_ENV_SUSTAIN_LOWER_BOUND;
        pr->upper_bound = PATCH_ENV_SUSTAIN_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_RATE_KS)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_ENV_RATE_KS_Y;

        pr->lower_bound = PATCH_ENV_RATE_KS_LOWER_BOUND;
        pr->upper_bound = PATCH_ENV_RATE_KS_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_LEVEL_KS)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_ENV_LEVEL_KS_Y;

        pr->lower_bound = PATCH_ENV_LEVEL_KS_LOWER_BOUND;
        pr->upper_bound = PATCH_ENV_LEVEL_KS_UPPER_BOUND;
      }
      else
      {
        printf("Unknown parameter found during setup.\n");
      }
    }
  }

  return 0;
}

/*******************************************************************************
** layout_setup_all()
*******************************************************************************/
short int layout_setup_all()
{
  /* setup layouts */
  layout_setup_panels();
  layout_setup_buttons();
  layout_setup_headers();
  layout_setup_params();

  return 0;
}

/*******************************************************************************
** layout_reset_top_panel_button_states()
*******************************************************************************/
short int layout_reset_top_panel_button_states()
{
  int k;

  button* b;

  /* reset all button states */
  for ( k = LAYOUT_TOP_PANEL_BUTTONS_START_INDEX; 
        k < LAYOUT_TOP_PANEL_BUTTONS_END_INDEX; 
        k++)
  {
    b = &G_layout_buttons[k];

    b->state = LAYOUT_BUTTON_STATE_OFF;
  }

  return 0;
}

