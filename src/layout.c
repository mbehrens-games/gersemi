/*******************************************************************************
** layout.c (screen layout widget positions)
*******************************************************************************/

#include <stdio.h>  /* testing */
#include <stdlib.h>

#include "layout.h"

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

  int quotient;
  int remainder;

  param* pr;

  /* patch edit params */
  for ( k = LAYOUT_PATCH_EDIT_PARAMS_START_INDEX; 
        k < LAYOUT_PATCH_EDIT_PARAMS_END_INDEX; 
        k++)
  {
    pr = &G_layout_params[k];

    quotient = (k - LAYOUT_PATCH_EDIT_PARAMS_START_INDEX) / (LAYOUT_NUM_PATCH_EDIT_PARAMS / 4);
    remainder = (k - LAYOUT_PATCH_EDIT_PARAMS_START_INDEX) % (LAYOUT_NUM_PATCH_EDIT_PARAMS / 4);

    /* the quotient determines the oscillator / envelope number */
    if (quotient == 0)
    {
      pr->num = 0;
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_1_CENTER_X;
    }
    else if (quotient == 1)
    {
      pr->num = 1;
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_2_CENTER_X;
    }
    else if (quotient == 2)
    {
      pr->num = 2;
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_3_CENTER_X;
    }
    else if (quotient == 3)
    {
      pr->num = 3;
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_4_CENTER_X;
    }
    else
    {
      pr->num = 0;
      pr->center_x = 0;
    }

    /* the remainder determines the parameter */
    if (remainder == 0)
    {
      pr->label = LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_FEEDBACK;
      pr->center_y = LAYOUT_PATCH_EDIT_OSC_FEEDBACK_Y;
    }
    else if (remainder == 1)
    {
      pr->label = LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_MULTIPLE;
      pr->center_y = LAYOUT_PATCH_EDIT_OSC_MULTIPLE_Y;
    }
    else if (remainder == 2)
    {
      pr->label = LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_DETUNE;
      pr->center_y = LAYOUT_PATCH_EDIT_OSC_DETUNE_Y;
    }
    else if (remainder == 3)
    {
      pr->label = LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_AMPLITUDE;
      pr->center_y = LAYOUT_PATCH_EDIT_OSC_AMPLITUDE_Y;
    }
    else if (remainder == 4)
    {
      pr->label = LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_ATTACK;
      pr->center_y = LAYOUT_PATCH_EDIT_ENV_ATTACK_Y;
    }
    else if (remainder == 5)
    {
      pr->label = LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_DECAY_1;
      pr->center_y = LAYOUT_PATCH_EDIT_ENV_DECAY_1_Y;
    }
    else if (remainder == 6)
    {
      pr->label = LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_DECAY_2;
      pr->center_y = LAYOUT_PATCH_EDIT_ENV_DECAY_2_Y;
    }
    else if (remainder == 7)
    {
      pr->label = LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_RELEASE;
      pr->center_y = LAYOUT_PATCH_EDIT_ENV_RELEASE_Y;
    }
    else if (remainder == 8)
    {
      pr->label = LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_SUSTAIN;
      pr->center_y = LAYOUT_PATCH_EDIT_ENV_SUSTAIN_Y;
    }
    else if (remainder == 9)
    {
      pr->label = LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_RATE_KS;
      pr->center_y = LAYOUT_PATCH_EDIT_ENV_RATE_KS_Y;
    }
    else if (remainder == 10)
    {
      pr->label = LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_LEVEL_KS;
      pr->center_y = LAYOUT_PATCH_EDIT_ENV_LEVEL_KS_Y;
    }
    else
    {
      pr->label = LAYOUT_PARAM_PATCH_EDIT_LABEL_NONE;
      pr->center_y = 0;
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

