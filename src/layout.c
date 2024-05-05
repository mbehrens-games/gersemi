/*******************************************************************************
** layout.c (screen layout widget positions)
*******************************************************************************/

#include <stdio.h>  /* testing */
#include <stdlib.h>

#include "layout.h"
#include "midicont.h"
#include "patch.h"
#include "tuning.h"

button G_layout_buttons[LAYOUT_NUM_BUTTONS];
header G_layout_headers[LAYOUT_NUM_HEADERS];
param  G_layout_params[LAYOUT_NUM_PARAMS];

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

    /* determine label */
    b->label = k - LAYOUT_TOP_PANEL_BUTTONS_START_INDEX;

    /* set position, width, default state */
    if (b->label == LAYOUT_TOP_PANEL_BUTTON_LABEL_CART)
    {
      b->center_x = LAYOUT_TOP_PANEL_BUTTON_CART_X;
      b->center_y = LAYOUT_TOP_PANEL_BUTTON_Y;

      b->width = LAYOUT_TOP_PANEL_BUTTON_CART_WIDTH;
      b->state = LAYOUT_BUTTON_STATE_ON;
    }
    else if (b->label == LAYOUT_TOP_PANEL_BUTTON_LABEL_INSTRUMENTS)
    {
      b->center_x = LAYOUT_TOP_PANEL_BUTTON_INSTRUMENTS_X;
      b->center_y = LAYOUT_TOP_PANEL_BUTTON_Y;

      b->width = LAYOUT_TOP_PANEL_BUTTON_INSTRUMENTS_WIDTH;
      b->state = LAYOUT_BUTTON_STATE_OFF;
    }
    else if (b->label == LAYOUT_TOP_PANEL_BUTTON_LABEL_SONG)
    {
      b->center_x = LAYOUT_TOP_PANEL_BUTTON_SONG_X;
      b->center_y = LAYOUT_TOP_PANEL_BUTTON_Y;

      b->width = LAYOUT_TOP_PANEL_BUTTON_SONG_WIDTH;
      b->state = LAYOUT_BUTTON_STATE_OFF;
    }
    else if (b->label == LAYOUT_TOP_PANEL_BUTTON_LABEL_MIXER)
    {
      b->center_x = LAYOUT_TOP_PANEL_BUTTON_MIXER_X;
      b->center_y = LAYOUT_TOP_PANEL_BUTTON_Y;

      b->width = LAYOUT_TOP_PANEL_BUTTON_MIXER_WIDTH;
      b->state = LAYOUT_BUTTON_STATE_OFF;
    }
    else if (b->label == LAYOUT_TOP_PANEL_BUTTON_LABEL_SOUND_FX)
    {
      b->center_x = LAYOUT_TOP_PANEL_BUTTON_SOUND_FX_X;
      b->center_y = LAYOUT_TOP_PANEL_BUTTON_Y;

      b->width = LAYOUT_TOP_PANEL_BUTTON_SOUND_FX_WIDTH;
      b->state = LAYOUT_BUTTON_STATE_OFF;
    }
    else if (b->label == LAYOUT_TOP_PANEL_BUTTON_LABEL_DPCM)
    {
      b->center_x = LAYOUT_TOP_PANEL_BUTTON_DPCM_X;
      b->center_y = LAYOUT_TOP_PANEL_BUTTON_Y;

      b->width = LAYOUT_TOP_PANEL_BUTTON_DPCM_WIDTH;
      b->state = LAYOUT_BUTTON_STATE_OFF;
    }
    else
    {
      b->center_x = 0;
      b->center_y = 0;

      b->width = 2;
      b->state = LAYOUT_BUTTON_STATE_OFF;
    }
  }

  /* cart edit buttons */
  for ( k = LAYOUT_CART_BUTTONS_START_INDEX; 
        k < LAYOUT_CART_BUTTONS_END_INDEX; 
        k++)
  {
    b = &G_layout_buttons[k];

    /* determine label */
    b->label = k - LAYOUT_CART_BUTTONS_START_INDEX;

    /* set position, width, default state */
    if (b->label == LAYOUT_CART_BUTTON_LABEL_CART_LOAD)
    {
      b->center_x = LAYOUT_CART_BUTTON_CART_LOAD_X;
      b->center_y = LAYOUT_CART_BUTTON_CART_LOAD_Y;

      b->width = LAYOUT_CART_BUTTON_CART_LOAD_WIDTH;
      b->state = LAYOUT_BUTTON_STATE_OFF;
    }
    else if (b->label == LAYOUT_CART_BUTTON_LABEL_CART_SAVE)
    {
      b->center_x = LAYOUT_CART_BUTTON_CART_SAVE_X;
      b->center_y = LAYOUT_CART_BUTTON_CART_SAVE_Y;

      b->width = LAYOUT_CART_BUTTON_CART_SAVE_WIDTH;
      b->state = LAYOUT_BUTTON_STATE_OFF;
    }
    else if (b->label == LAYOUT_CART_BUTTON_LABEL_CART_COPY)
    {
      b->center_x = LAYOUT_CART_BUTTON_CART_COPY_X;
      b->center_y = LAYOUT_CART_BUTTON_CART_COPY_Y;

      b->width = LAYOUT_CART_BUTTON_CART_COPY_WIDTH;
      b->state = LAYOUT_BUTTON_STATE_OFF;
    }
    else if (b->label == LAYOUT_CART_BUTTON_LABEL_CART_ZAP)
    {
      b->center_x = LAYOUT_CART_BUTTON_CART_ZAP_X;
      b->center_y = LAYOUT_CART_BUTTON_CART_ZAP_Y;

      b->width = LAYOUT_CART_BUTTON_CART_ZAP_WIDTH;
      b->state = LAYOUT_BUTTON_STATE_OFF;
    }
    else if (b->label == LAYOUT_CART_BUTTON_LABEL_PATCH_LOAD)
    {
      b->center_x = LAYOUT_CART_BUTTON_PATCH_LOAD_X;
      b->center_y = LAYOUT_CART_BUTTON_PATCH_LOAD_Y;

      b->width = LAYOUT_CART_BUTTON_PATCH_LOAD_WIDTH;
      b->state = LAYOUT_BUTTON_STATE_OFF;
    }
    else if (b->label == LAYOUT_CART_BUTTON_LABEL_PATCH_SAVE)
    {
      b->center_x = LAYOUT_CART_BUTTON_PATCH_SAVE_X;
      b->center_y = LAYOUT_CART_BUTTON_PATCH_SAVE_Y;

      b->width = LAYOUT_CART_BUTTON_PATCH_SAVE_WIDTH;
      b->state = LAYOUT_BUTTON_STATE_OFF;
    }
    else if (b->label == LAYOUT_CART_BUTTON_LABEL_PATCH_COPY)
    {
      b->center_x = LAYOUT_CART_BUTTON_PATCH_COPY_X;
      b->center_y = LAYOUT_CART_BUTTON_PATCH_COPY_Y;

      b->width = LAYOUT_CART_BUTTON_PATCH_COPY_WIDTH;
      b->state = LAYOUT_BUTTON_STATE_OFF;
    }
    else if (b->label == LAYOUT_CART_BUTTON_LABEL_PATCH_ZAP)
    {
      b->center_x = LAYOUT_CART_BUTTON_PATCH_ZAP_X;
      b->center_y = LAYOUT_CART_BUTTON_PATCH_ZAP_Y;

      b->width = LAYOUT_CART_BUTTON_PATCH_ZAP_WIDTH;
      b->state = LAYOUT_BUTTON_STATE_OFF;
    }
    else
    {
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

    /* determine label */
    hd->label = k - LAYOUT_TOP_PANEL_HEADERS_START_INDEX;

    /* set position */
    if (hd->label == LAYOUT_TOP_PANEL_HEADER_LABEL_NAME)
    {
      hd->center_x = LAYOUT_TOP_PANEL_HEADER_NAME_X;
      hd->center_y = LAYOUT_TOP_PANEL_HEADER_NAME_Y;
    }
    else if (hd->label == LAYOUT_TOP_PANEL_HEADER_LABEL_VERSION)
    {
      hd->center_x = LAYOUT_TOP_PANEL_HEADER_VERSION_X;
      hd->center_y = LAYOUT_TOP_PANEL_HEADER_VERSION_Y;
    }
    else
    {
      hd->center_x = 0;
      hd->center_y = 0;
    }
  }

  /* patch edit headers */
  for ( k = LAYOUT_CART_HEADERS_START_INDEX; 
        k < LAYOUT_CART_HEADERS_END_INDEX; 
        k++)
  {
    hd = &G_layout_headers[k];

    /* determine label */
    hd->label = k - LAYOUT_CART_HEADERS_START_INDEX;

    /* set position */
    if (hd->label == LAYOUT_CART_HEADER_LABEL_OSC_1)
    {
      hd->center_x = LAYOUT_CART_HEADER_OSC_1_X;
      hd->center_y = LAYOUT_CART_HEADER_OSC_1_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_OSC_2)
    {
      hd->center_x = LAYOUT_CART_HEADER_OSC_2_X;
      hd->center_y = LAYOUT_CART_HEADER_OSC_2_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_OSC_3)
    {
      hd->center_x = LAYOUT_CART_HEADER_OSC_3_X;
      hd->center_y = LAYOUT_CART_HEADER_OSC_3_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_FILTERS)
    {
      hd->center_x = LAYOUT_CART_HEADER_FILTERS_X;
      hd->center_y = LAYOUT_CART_HEADER_FILTERS_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_ENV_1)
    {
      hd->center_x = LAYOUT_CART_HEADER_ENV_1_X;
      hd->center_y = LAYOUT_CART_HEADER_ENV_1_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_ENV_2)
    {
      hd->center_x = LAYOUT_CART_HEADER_ENV_2_X;
      hd->center_y = LAYOUT_CART_HEADER_ENV_2_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_ENV_3)
    {
      hd->center_x = LAYOUT_CART_HEADER_ENV_3_X;
      hd->center_y = LAYOUT_CART_HEADER_ENV_3_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_VIBRATO)
    {
      hd->center_x = LAYOUT_CART_HEADER_VIBRATO_X;
      hd->center_y = LAYOUT_CART_HEADER_VIBRATO_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_TREMOLO)
    {
      hd->center_x = LAYOUT_CART_HEADER_TREMOLO_X;
      hd->center_y = LAYOUT_CART_HEADER_TREMOLO_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_CHORUS)
    {
      hd->center_x = LAYOUT_CART_HEADER_CHORUS_X;
      hd->center_y = LAYOUT_CART_HEADER_CHORUS_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_SYNC)
    {
      hd->center_x = LAYOUT_CART_HEADER_SYNC_X;
      hd->center_y = LAYOUT_CART_HEADER_SYNC_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_SENSITIVITY)
    {
      hd->center_x = LAYOUT_CART_HEADER_SENSITIVITY_X;
      hd->center_y = LAYOUT_CART_HEADER_SENSITIVITY_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_PITCH_ENV)
    {
      hd->center_x = LAYOUT_CART_HEADER_PITCH_ENV_X;
      hd->center_y = LAYOUT_CART_HEADER_PITCH_ENV_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_ARPEGGIO)
    {
      hd->center_x = LAYOUT_CART_HEADER_ARPEGGIO_X;
      hd->center_y = LAYOUT_CART_HEADER_ARPEGGIO_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_PORTAMENTO)
    {
      hd->center_x = LAYOUT_CART_HEADER_PORTAMENTO_X;
      hd->center_y = LAYOUT_CART_HEADER_PORTAMENTO_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_PITCH_WHEEL)
    {
      hd->center_x = LAYOUT_CART_HEADER_PITCH_WHEEL_X;
      hd->center_y = LAYOUT_CART_HEADER_PITCH_WHEEL_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_TREMOLO_ROUTING)
    {
      hd->center_x = LAYOUT_CART_HEADER_TREMOLO_ROUTING_X;
      hd->center_y = LAYOUT_CART_HEADER_TREMOLO_ROUTING_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_BOOST_ROUTING)
    {
      hd->center_x = LAYOUT_CART_HEADER_BOOST_ROUTING_X;
      hd->center_y = LAYOUT_CART_HEADER_BOOST_ROUTING_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_VELOCITY_ROUTING)
    {
      hd->center_x = LAYOUT_CART_HEADER_VELOCITY_ROUTING_X;
      hd->center_y = LAYOUT_CART_HEADER_VELOCITY_ROUTING_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_MOD_WHEEL_ROUTING)
    {
      hd->center_x = LAYOUT_CART_HEADER_MOD_WHEEL_ROUTING_X;
      hd->center_y = LAYOUT_CART_HEADER_MOD_WHEEL_ROUTING_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_AFTERTOUCH_ROUTING)
    {
      hd->center_x = LAYOUT_CART_HEADER_AFTERTOUCH_ROUTING_X;
      hd->center_y = LAYOUT_CART_HEADER_AFTERTOUCH_ROUTING_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_EXP_PEDAL_ROUTING)
    {
      hd->center_x = LAYOUT_CART_HEADER_EXP_PEDAL_ROUTING_X;
      hd->center_y = LAYOUT_CART_HEADER_EXP_PEDAL_ROUTING_Y;
    }
    else
    {
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

  param* pr;

  /* patch edit params */
  for ( k = LAYOUT_CART_PARAMS_START_INDEX; 
        k < LAYOUT_CART_PARAMS_END_INDEX; 
        k++)
  {
    pr = &G_layout_params[k];

    /* determine label */
    pr->label = k - LAYOUT_CART_PARAMS_START_INDEX;

    /* set type */
    if ((pr->label == LAYOUT_CART_PARAM_LABEL_ALGORITHM)          || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_1_WAVEFORM)     || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_2_WAVEFORM)     || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_3_WAVEFORM)     || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_VIBRATO_WAVEFORM)   || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_WAVEFORM)   || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_ARPEGGIO_PATTERN)   || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_PORTAMENTO_LEGATO))
    {
      pr->type = LAYOUT_PARAM_TYPE_ARROWS;
    }
    else if ( (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_1_FREQ_MODE)  || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_2_FREQ_MODE)  || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_CHORUS_MODE)      || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_SYNC_VIBRATO)     || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_SYNC_TREMOLO)     || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_SYNC_CHORUS)      || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_SYNC_OSC)         || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_ARPEGGIO_MODE)    || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_PORTAMENTO_MODE)  || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_WHEEL_MODE))
    {
      pr->type = LAYOUT_PARAM_TYPE_RADIO;
    }
    else if ( (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_ROUTING_ENV_1)      || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_ROUTING_ENV_2)      || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_ROUTING_ENV_3)      || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_BOOST_ROUTING_ENV_1)        || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_BOOST_ROUTING_ENV_2)        || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_BOOST_ROUTING_ENV_3)        || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_VELOCITY_ROUTING_ENV_1)     || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_VELOCITY_ROUTING_ENV_2)     || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_VELOCITY_ROUTING_ENV_3)     || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_MOD_WHEEL_ROUTING_VIBRATO)  || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_MOD_WHEEL_ROUTING_TREMOLO)  || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_MOD_WHEEL_ROUTING_BOOST)    || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_MOD_WHEEL_ROUTING_CHORUS)   || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_AFTERTOUCH_ROUTING_VIBRATO) || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_AFTERTOUCH_ROUTING_TREMOLO) || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_AFTERTOUCH_ROUTING_BOOST)   || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_AFTERTOUCH_ROUTING_CHORUS)  || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_EXP_PEDAL_ROUTING_VIBRATO)  || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_EXP_PEDAL_ROUTING_TREMOLO)  || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_EXP_PEDAL_ROUTING_BOOST)    || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_EXP_PEDAL_ROUTING_CHORUS))
    {
      pr->type = LAYOUT_PARAM_TYPE_FLAG;
    }
    else if ( (pr->label == LAYOUT_CART_PARAM_LABEL_CART_NUMBER) || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_PATCH_NUMBER))
    {
      pr->type = LAYOUT_PARAM_TYPE_EDITABLE;
    }
    else
      pr->type = LAYOUT_PARAM_TYPE_SLIDER;

    /* set position and bounds */

    /* cart number */
    if (pr->label == LAYOUT_CART_PARAM_LABEL_CART_NUMBER)
    {
      pr->center_x = LAYOUT_CART_CART_NUMBER_X;
      pr->center_y = LAYOUT_CART_CART_NUMBER_Y;

      pr->lower_bound = PATCH_CART_NO_LOWER_BOUND;
      pr->upper_bound = PATCH_CART_NO_UPPER_BOUND;
    }
    /* patch number */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PATCH_NUMBER)
    {
      pr->center_x = LAYOUT_CART_PATCH_NUMBER_X;
      pr->center_y = LAYOUT_CART_PATCH_NUMBER_Y;

      pr->lower_bound = PATCH_PATCH_NO_LOWER_BOUND;
      pr->upper_bound = PATCH_PATCH_NO_UPPER_BOUND;
    }
    /* algorithm */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ALGORITHM)
    {
      pr->center_x = LAYOUT_CART_ALGORITHM_X;
      pr->center_y = LAYOUT_CART_ALGORITHM_Y;

      pr->lower_bound = PATCH_ALGORITHM_LOWER_BOUND;
      pr->upper_bound = PATCH_ALGORITHM_UPPER_BOUND;
    }
    /* oscillator 1 */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_1_WAVEFORM)
    {
      pr->center_x = LAYOUT_CART_HEADER_OSC_1_X;
      pr->center_y = LAYOUT_CART_OSC_1_WAVEFORM_Y;

      pr->lower_bound = PATCH_OSC_WAVEFORM_LOWER_BOUND;
      pr->upper_bound = PATCH_OSC_WAVEFORM_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_1_PHI)
    {
      pr->center_x = LAYOUT_CART_HEADER_OSC_1_X;
      pr->center_y = LAYOUT_CART_OSC_1_PHI_Y;

      pr->lower_bound = PATCH_OSC_PHI_LOWER_BOUND;
      pr->upper_bound = PATCH_OSC_PHI_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_1_FREQ_MODE)
    {
      pr->center_x = LAYOUT_CART_HEADER_OSC_1_X;
      pr->center_y = LAYOUT_CART_OSC_1_FREQ_MODE_Y;

      pr->lower_bound = PATCH_OSC_FREQ_MODE_LOWER_BOUND;
      pr->upper_bound = PATCH_OSC_FREQ_MODE_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_1_MULTIPLE)
    {
      pr->center_x = LAYOUT_CART_HEADER_OSC_1_X;
      pr->center_y = LAYOUT_CART_OSC_1_MULTIPLE_Y;

      pr->lower_bound = PATCH_OSC_MULTIPLE_LOWER_BOUND;
      pr->upper_bound = PATCH_OSC_MULTIPLE_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_1_DIVISOR)
    {
      pr->center_x = LAYOUT_CART_HEADER_OSC_1_X;
      pr->center_y = LAYOUT_CART_OSC_1_DIVISOR_Y;

      pr->lower_bound = PATCH_OSC_DIVISOR_LOWER_BOUND;
      pr->upper_bound = PATCH_OSC_DIVISOR_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_1_DETUNE)
    {
      pr->center_x = LAYOUT_CART_HEADER_OSC_1_X;
      pr->center_y = LAYOUT_CART_OSC_1_DETUNE_Y;

      pr->lower_bound = PATCH_OSC_DETUNE_LOWER_BOUND;
      pr->upper_bound = PATCH_OSC_DETUNE_UPPER_BOUND;
    }
    /* oscillator 2 */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_2_WAVEFORM)
    {
      pr->center_x = LAYOUT_CART_HEADER_OSC_2_X;
      pr->center_y = LAYOUT_CART_OSC_2_WAVEFORM_Y;

      pr->lower_bound = PATCH_OSC_WAVEFORM_LOWER_BOUND;
      pr->upper_bound = PATCH_OSC_WAVEFORM_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_2_PHI)
    {
      pr->center_x = LAYOUT_CART_HEADER_OSC_2_X;
      pr->center_y = LAYOUT_CART_OSC_2_PHI_Y;

      pr->lower_bound = PATCH_OSC_PHI_LOWER_BOUND;
      pr->upper_bound = PATCH_OSC_PHI_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_2_FREQ_MODE)
    {
      pr->center_x = LAYOUT_CART_HEADER_OSC_2_X;
      pr->center_y = LAYOUT_CART_OSC_2_FREQ_MODE_Y;

      pr->lower_bound = PATCH_OSC_FREQ_MODE_LOWER_BOUND;
      pr->upper_bound = PATCH_OSC_FREQ_MODE_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_2_MULTIPLE)
    {
      pr->center_x = LAYOUT_CART_HEADER_OSC_2_X;
      pr->center_y = LAYOUT_CART_OSC_2_MULTIPLE_Y;

      pr->lower_bound = PATCH_OSC_MULTIPLE_LOWER_BOUND;
      pr->upper_bound = PATCH_OSC_MULTIPLE_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_2_DIVISOR)
    {
      pr->center_x = LAYOUT_CART_HEADER_OSC_2_X;
      pr->center_y = LAYOUT_CART_OSC_2_DIVISOR_Y;

      pr->lower_bound = PATCH_OSC_DIVISOR_LOWER_BOUND;
      pr->upper_bound = PATCH_OSC_DIVISOR_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_2_DETUNE)
    {
      pr->center_x = LAYOUT_CART_HEADER_OSC_2_X;
      pr->center_y = LAYOUT_CART_OSC_2_DETUNE_Y;

      pr->lower_bound = PATCH_OSC_DETUNE_LOWER_BOUND;
      pr->upper_bound = PATCH_OSC_DETUNE_UPPER_BOUND;
    }
    /* oscillator 3 */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_3_WAVEFORM)
    {
      pr->center_x = LAYOUT_CART_HEADER_OSC_3_X;
      pr->center_y = LAYOUT_CART_OSC_3_WAVEFORM_Y;

      pr->lower_bound = PATCH_OSC_WAVEFORM_LOWER_BOUND;
      pr->upper_bound = PATCH_OSC_WAVEFORM_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_3_PHI)
    {
      pr->center_x = LAYOUT_CART_HEADER_OSC_3_X;
      pr->center_y = LAYOUT_CART_OSC_3_PHI_Y;

      pr->lower_bound = PATCH_OSC_PHI_LOWER_BOUND;
      pr->upper_bound = PATCH_OSC_PHI_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_3_DETUNE)
    {
      pr->center_x = LAYOUT_CART_HEADER_OSC_3_X;
      pr->center_y = LAYOUT_CART_OSC_3_DETUNE_Y;

      pr->lower_bound = PATCH_OSC_DETUNE_LOWER_BOUND;
      pr->upper_bound = PATCH_OSC_DETUNE_UPPER_BOUND;
    }
    /* filters */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_LOWPASS_MULTIPLE)
    {
      pr->center_x = LAYOUT_CART_HEADER_FILTERS_X;
      pr->center_y = LAYOUT_CART_LOWPASS_MULTIPLE_Y;

      pr->lower_bound = PATCH_LOWPASS_MULTIPLE_LOWER_BOUND;
      pr->upper_bound = PATCH_LOWPASS_MULTIPLE_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_LOWPASS_KEYTRACKING)
    {
      pr->center_x = LAYOUT_CART_HEADER_FILTERS_X;
      pr->center_y = LAYOUT_CART_LOWPASS_KEYTRACKING_Y;

      pr->lower_bound = PATCH_LOWPASS_KEYTRACKING_LOWER_BOUND;
      pr->upper_bound = PATCH_LOWPASS_KEYTRACKING_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_HIGHPASS_CUTOFF)
    {
      pr->center_x = LAYOUT_CART_HEADER_FILTERS_X;
      pr->center_y = LAYOUT_CART_HIGHPASS_CUTOFF_Y;

      pr->lower_bound = PATCH_HIGHPASS_CUTOFF_LOWER_BOUND;
      pr->upper_bound = PATCH_HIGHPASS_CUTOFF_UPPER_BOUND;
    }
    /* amplitude envelope 1 */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_1_ATTACK)
    {
      pr->center_x = LAYOUT_CART_HEADER_ENV_1_X;
      pr->center_y = LAYOUT_CART_ENV_1_ATTACK_Y;

      pr->lower_bound = PATCH_ENV_TIME_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_TIME_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_1_DECAY)
    {
      pr->center_x = LAYOUT_CART_HEADER_ENV_1_X;
      pr->center_y = LAYOUT_CART_ENV_1_DECAY_Y;

      pr->lower_bound = PATCH_ENV_TIME_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_TIME_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_1_RELEASE)
    {
      pr->center_x = LAYOUT_CART_HEADER_ENV_1_X;
      pr->center_y = LAYOUT_CART_ENV_1_RELEASE_Y;

      pr->lower_bound = PATCH_ENV_TIME_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_TIME_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_1_AMPLITUDE)
    {
      pr->center_x = LAYOUT_CART_HEADER_ENV_1_X;
      pr->center_y = LAYOUT_CART_ENV_1_AMPLITUDE_Y;

      pr->lower_bound = PATCH_ENV_LEVEL_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_LEVEL_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_1_SUSTAIN)
    {
      pr->center_x = LAYOUT_CART_HEADER_ENV_1_X;
      pr->center_y = LAYOUT_CART_ENV_1_SUSTAIN_Y;

      pr->lower_bound = PATCH_ENV_LEVEL_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_LEVEL_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_1_HOLD)
    {
      pr->center_x = LAYOUT_CART_HEADER_ENV_1_X;
      pr->center_y = LAYOUT_CART_ENV_1_HOLD_Y;

      pr->lower_bound = PATCH_ENV_HOLD_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_HOLD_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_1_PEDAL)
    {
      pr->center_x = LAYOUT_CART_HEADER_ENV_1_X;
      pr->center_y = LAYOUT_CART_ENV_1_PEDAL_Y;

      pr->lower_bound = PATCH_ENV_HOLD_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_HOLD_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_1_RATE_KS)
    {
      pr->center_x = LAYOUT_CART_HEADER_ENV_1_X;
      pr->center_y = LAYOUT_CART_ENV_1_RATE_KS_Y;

      pr->lower_bound = PATCH_ENV_KEYSCALING_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_KEYSCALING_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_1_LEVEL_KS)
    {
      pr->center_x = LAYOUT_CART_HEADER_ENV_1_X;
      pr->center_y = LAYOUT_CART_ENV_1_LEVEL_KS_Y;

      pr->lower_bound = PATCH_ENV_KEYSCALING_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_KEYSCALING_UPPER_BOUND;
    }
    /* amplitude envelope 2 */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_2_ATTACK)
    {
      pr->center_x = LAYOUT_CART_HEADER_ENV_2_X;
      pr->center_y = LAYOUT_CART_ENV_2_ATTACK_Y;

      pr->lower_bound = PATCH_ENV_TIME_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_TIME_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_2_DECAY)
    {
      pr->center_x = LAYOUT_CART_HEADER_ENV_2_X;
      pr->center_y = LAYOUT_CART_ENV_2_DECAY_Y;

      pr->lower_bound = PATCH_ENV_TIME_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_TIME_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_2_RELEASE)
    {
      pr->center_x = LAYOUT_CART_HEADER_ENV_2_X;
      pr->center_y = LAYOUT_CART_ENV_2_RELEASE_Y;

      pr->lower_bound = PATCH_ENV_TIME_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_TIME_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_2_AMPLITUDE)
    {
      pr->center_x = LAYOUT_CART_HEADER_ENV_2_X;
      pr->center_y = LAYOUT_CART_ENV_2_AMPLITUDE_Y;

      pr->lower_bound = PATCH_ENV_LEVEL_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_LEVEL_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_2_SUSTAIN)
    {
      pr->center_x = LAYOUT_CART_HEADER_ENV_2_X;
      pr->center_y = LAYOUT_CART_ENV_2_SUSTAIN_Y;

      pr->lower_bound = PATCH_ENV_LEVEL_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_LEVEL_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_2_HOLD)
    {
      pr->center_x = LAYOUT_CART_HEADER_ENV_2_X;
      pr->center_y = LAYOUT_CART_ENV_2_HOLD_Y;

      pr->lower_bound = PATCH_ENV_HOLD_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_HOLD_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_2_PEDAL)
    {
      pr->center_x = LAYOUT_CART_HEADER_ENV_2_X;
      pr->center_y = LAYOUT_CART_ENV_2_PEDAL_Y;

      pr->lower_bound = PATCH_ENV_HOLD_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_HOLD_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_2_RATE_KS)
    {
      pr->center_x = LAYOUT_CART_HEADER_ENV_2_X;
      pr->center_y = LAYOUT_CART_ENV_2_RATE_KS_Y;

      pr->lower_bound = PATCH_ENV_KEYSCALING_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_KEYSCALING_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_2_LEVEL_KS)
    {
      pr->center_x = LAYOUT_CART_HEADER_ENV_2_X;
      pr->center_y = LAYOUT_CART_ENV_2_LEVEL_KS_Y;

      pr->lower_bound = PATCH_ENV_KEYSCALING_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_KEYSCALING_UPPER_BOUND;
    }
    /* amplitude envelope 3 */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_3_ATTACK)
    {
      pr->center_x = LAYOUT_CART_HEADER_ENV_3_X;
      pr->center_y = LAYOUT_CART_ENV_3_ATTACK_Y;

      pr->lower_bound = PATCH_ENV_TIME_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_TIME_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_3_DECAY)
    {
      pr->center_x = LAYOUT_CART_HEADER_ENV_3_X;
      pr->center_y = LAYOUT_CART_ENV_3_DECAY_Y;

      pr->lower_bound = PATCH_ENV_TIME_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_TIME_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_3_RELEASE)
    {
      pr->center_x = LAYOUT_CART_HEADER_ENV_3_X;
      pr->center_y = LAYOUT_CART_ENV_3_RELEASE_Y;

      pr->lower_bound = PATCH_ENV_TIME_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_TIME_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_3_AMPLITUDE)
    {
      pr->center_x = LAYOUT_CART_HEADER_ENV_3_X;
      pr->center_y = LAYOUT_CART_ENV_3_AMPLITUDE_Y;

      pr->lower_bound = PATCH_ENV_LEVEL_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_LEVEL_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_3_SUSTAIN)
    {
      pr->center_x = LAYOUT_CART_HEADER_ENV_3_X;
      pr->center_y = LAYOUT_CART_ENV_3_SUSTAIN_Y;

      pr->lower_bound = PATCH_ENV_LEVEL_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_LEVEL_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_3_HOLD)
    {
      pr->center_x = LAYOUT_CART_HEADER_ENV_3_X;
      pr->center_y = LAYOUT_CART_ENV_3_HOLD_Y;

      pr->lower_bound = PATCH_ENV_HOLD_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_HOLD_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_3_PEDAL)
    {
      pr->center_x = LAYOUT_CART_HEADER_ENV_3_X;
      pr->center_y = LAYOUT_CART_ENV_3_PEDAL_Y;

      pr->lower_bound = PATCH_ENV_HOLD_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_HOLD_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_3_RATE_KS)
    {
      pr->center_x = LAYOUT_CART_HEADER_ENV_3_X;
      pr->center_y = LAYOUT_CART_ENV_3_RATE_KS_Y;

      pr->lower_bound = PATCH_ENV_KEYSCALING_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_KEYSCALING_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_3_LEVEL_KS)
    {
      pr->center_x = LAYOUT_CART_HEADER_ENV_3_X;
      pr->center_y = LAYOUT_CART_ENV_3_LEVEL_KS_Y;

      pr->lower_bound = PATCH_ENV_KEYSCALING_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_KEYSCALING_UPPER_BOUND;
    }
    /* vibrato lfo */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_VIBRATO_WAVEFORM)
    {
      pr->center_x = LAYOUT_CART_HEADER_VIBRATO_X;
      pr->center_y = LAYOUT_CART_VIBRATO_WAVEFORM_Y;

      pr->lower_bound = PATCH_VIBRATO_WAVEFORM_LOWER_BOUND;
      pr->upper_bound = PATCH_VIBRATO_WAVEFORM_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_VIBRATO_DELAY)
    {
      pr->center_x = LAYOUT_CART_HEADER_VIBRATO_X;
      pr->center_y = LAYOUT_CART_VIBRATO_DELAY_Y;

      pr->lower_bound = PATCH_LFO_DELAY_LOWER_BOUND;
      pr->upper_bound = PATCH_LFO_DELAY_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_VIBRATO_SPEED)
    {
      pr->center_x = LAYOUT_CART_HEADER_VIBRATO_X;
      pr->center_y = LAYOUT_CART_VIBRATO_SPEED_Y;

      pr->lower_bound = PATCH_LFO_SPEED_LOWER_BOUND;
      pr->upper_bound = PATCH_LFO_SPEED_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_VIBRATO_DEPTH)
    {
      pr->center_x = LAYOUT_CART_HEADER_VIBRATO_X;
      pr->center_y = LAYOUT_CART_VIBRATO_DEPTH_Y;

      pr->lower_bound = PATCH_LFO_DEPTH_LOWER_BOUND;
      pr->upper_bound = PATCH_LFO_DEPTH_UPPER_BOUND;
    }
    /* tremolo lfo */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_WAVEFORM)
    {
      pr->center_x = LAYOUT_CART_HEADER_TREMOLO_X;
      pr->center_y = LAYOUT_CART_TREMOLO_WAVEFORM_Y;

      pr->lower_bound = PATCH_TREMOLO_WAVEFORM_LOWER_BOUND;
      pr->upper_bound = PATCH_TREMOLO_WAVEFORM_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_DELAY)
    {
      pr->center_x = LAYOUT_CART_HEADER_TREMOLO_X;
      pr->center_y = LAYOUT_CART_TREMOLO_DELAY_Y;

      pr->lower_bound = PATCH_LFO_DELAY_LOWER_BOUND;
      pr->upper_bound = PATCH_LFO_DELAY_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_SPEED)
    {
      pr->center_x = LAYOUT_CART_HEADER_TREMOLO_X;
      pr->center_y = LAYOUT_CART_TREMOLO_SPEED_Y;

      pr->lower_bound = PATCH_LFO_SPEED_LOWER_BOUND;
      pr->upper_bound = PATCH_LFO_SPEED_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_DEPTH)
    {
      pr->center_x = LAYOUT_CART_HEADER_TREMOLO_X;
      pr->center_y = LAYOUT_CART_TREMOLO_DEPTH_Y;

      pr->lower_bound = PATCH_LFO_DEPTH_LOWER_BOUND;
      pr->upper_bound = PATCH_LFO_DEPTH_UPPER_BOUND;
    }
    /* chorus */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_CHORUS_MODE)
    {
      pr->center_x = LAYOUT_CART_HEADER_CHORUS_X;
      pr->center_y = LAYOUT_CART_CHORUS_MODE_Y;

      pr->lower_bound = PATCH_CHORUS_MODE_LOWER_BOUND;
      pr->upper_bound = PATCH_CHORUS_MODE_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_CHORUS_DELAY)
    {
      pr->center_x = LAYOUT_CART_HEADER_CHORUS_X;
      pr->center_y = LAYOUT_CART_CHORUS_DELAY_Y;

      pr->lower_bound = PATCH_LFO_DELAY_LOWER_BOUND;
      pr->upper_bound = PATCH_LFO_DELAY_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_CHORUS_SPEED)
    {
      pr->center_x = LAYOUT_CART_HEADER_CHORUS_X;
      pr->center_y = LAYOUT_CART_CHORUS_SPEED_Y;

      pr->lower_bound = PATCH_LFO_SPEED_LOWER_BOUND;
      pr->upper_bound = PATCH_LFO_SPEED_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_CHORUS_DEPTH)
    {
      pr->center_x = LAYOUT_CART_HEADER_CHORUS_X;
      pr->center_y = LAYOUT_CART_CHORUS_DEPTH_Y;

      pr->lower_bound = PATCH_LFO_DEPTH_LOWER_BOUND;
      pr->upper_bound = PATCH_LFO_DEPTH_UPPER_BOUND;
    }
    /* sync */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_SYNC_VIBRATO)
    {
      pr->center_x = LAYOUT_CART_HEADER_SYNC_X;
      pr->center_y = LAYOUT_CART_SYNC_VIBRATO_Y;

      pr->lower_bound = PATCH_SYNC_LOWER_BOUND;
      pr->upper_bound = PATCH_SYNC_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_SYNC_TREMOLO)
    {
      pr->center_x = LAYOUT_CART_HEADER_SYNC_X;
      pr->center_y = LAYOUT_CART_SYNC_TREMOLO_Y;

      pr->lower_bound = PATCH_SYNC_LOWER_BOUND;
      pr->upper_bound = PATCH_SYNC_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_SYNC_CHORUS)
    {
      pr->center_x = LAYOUT_CART_HEADER_SYNC_X;
      pr->center_y = LAYOUT_CART_SYNC_CHORUS_Y;

      pr->lower_bound = PATCH_SYNC_LOWER_BOUND;
      pr->upper_bound = PATCH_SYNC_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_SYNC_OSC)
    {
      pr->center_x = LAYOUT_CART_HEADER_SYNC_X;
      pr->center_y = LAYOUT_CART_SYNC_OSC_Y;

      pr->lower_bound = PATCH_SYNC_LOWER_BOUND;
      pr->upper_bound = PATCH_SYNC_UPPER_BOUND;
    }
    /* sensitivity */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_SENSITIVITY_VIBRATO)
    {
      pr->center_x = LAYOUT_CART_HEADER_SENSITIVITY_X;
      pr->center_y = LAYOUT_CART_SENSITIVITY_VIBRATO_Y;

      pr->lower_bound = PATCH_SENSITIVITY_LOWER_BOUND;
      pr->upper_bound = PATCH_SENSITIVITY_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_SENSITIVITY_TREMOLO)
    {
      pr->center_x = LAYOUT_CART_HEADER_SENSITIVITY_X;
      pr->center_y = LAYOUT_CART_SENSITIVITY_TREMOLO_Y;

      pr->lower_bound = PATCH_SENSITIVITY_LOWER_BOUND;
      pr->upper_bound = PATCH_SENSITIVITY_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_SENSITIVITY_CHORUS)
    {
      pr->center_x = LAYOUT_CART_HEADER_SENSITIVITY_X;
      pr->center_y = LAYOUT_CART_SENSITIVITY_CHORUS_Y;

      pr->lower_bound = PATCH_SENSITIVITY_LOWER_BOUND;
      pr->upper_bound = PATCH_SENSITIVITY_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_SENSITIVITY_BOOST)
    {
      pr->center_x = LAYOUT_CART_HEADER_SENSITIVITY_X;
      pr->center_y = LAYOUT_CART_SENSITIVITY_BOOST_Y;

      pr->lower_bound = PATCH_SENSITIVITY_LOWER_BOUND;
      pr->upper_bound = PATCH_SENSITIVITY_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_SENSITIVITY_VELOCITY)
    {
      pr->center_x = LAYOUT_CART_HEADER_SENSITIVITY_X;
      pr->center_y = LAYOUT_CART_SENSITIVITY_VELOCITY_Y;

      pr->lower_bound = PATCH_SENSITIVITY_LOWER_BOUND;
      pr->upper_bound = PATCH_SENSITIVITY_UPPER_BOUND;
    }
    /* pitch envelope */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_ENV_ATTACK)
    {
      pr->center_x = LAYOUT_CART_HEADER_PITCH_ENV_X;
      pr->center_y = LAYOUT_CART_PITCH_ENV_ATTACK_Y;

      pr->lower_bound = PATCH_PEG_TIME_LOWER_BOUND;
      pr->upper_bound = PATCH_PEG_TIME_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_ENV_DECAY)
    {
      pr->center_x = LAYOUT_CART_HEADER_PITCH_ENV_X;
      pr->center_y = LAYOUT_CART_PITCH_ENV_DECAY_Y;

      pr->lower_bound = PATCH_PEG_TIME_LOWER_BOUND;
      pr->upper_bound = PATCH_PEG_TIME_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_ENV_RELEASE)
    {
      pr->center_x = LAYOUT_CART_HEADER_PITCH_ENV_X;
      pr->center_y = LAYOUT_CART_PITCH_ENV_RELEASE_Y;

      pr->lower_bound = PATCH_PEG_TIME_LOWER_BOUND;
      pr->upper_bound = PATCH_PEG_TIME_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_ENV_MAXIMUM)
    {
      pr->center_x = LAYOUT_CART_HEADER_PITCH_ENV_X;
      pr->center_y = LAYOUT_CART_PITCH_ENV_MAXIMUM_Y;

      pr->lower_bound = PATCH_PEG_LEVEL_LOWER_BOUND;
      pr->upper_bound = PATCH_PEG_LEVEL_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_ENV_FINALE)
    {
      pr->center_x = LAYOUT_CART_HEADER_PITCH_ENV_X;
      pr->center_y = LAYOUT_CART_PITCH_ENV_FINALE_Y;

      pr->lower_bound = PATCH_PEG_LEVEL_LOWER_BOUND;
      pr->upper_bound = PATCH_PEG_LEVEL_UPPER_BOUND;
    }
    /* arpeggio */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ARPEGGIO_MODE)
    {
      pr->center_x = LAYOUT_CART_HEADER_ARPEGGIO_X;
      pr->center_y = LAYOUT_CART_ARPEGGIO_MODE_Y;

      pr->lower_bound = PATCH_ARPEGGIO_MODE_LOWER_BOUND;
      pr->upper_bound = PATCH_ARPEGGIO_MODE_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ARPEGGIO_PATTERN)
    {
      pr->center_x = LAYOUT_CART_HEADER_ARPEGGIO_X;
      pr->center_y = LAYOUT_CART_ARPEGGIO_PATTERN_Y;

      pr->lower_bound = PATCH_ARPEGGIO_PATTERN_LOWER_BOUND;
      pr->upper_bound = PATCH_ARPEGGIO_PATTERN_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ARPEGGIO_OCTAVES)
    {
      pr->center_x = LAYOUT_CART_HEADER_ARPEGGIO_X;
      pr->center_y = LAYOUT_CART_ARPEGGIO_OCTAVES_Y;

      pr->lower_bound = PATCH_ARPEGGIO_OCTAVES_LOWER_BOUND;
      pr->upper_bound = PATCH_ARPEGGIO_OCTAVES_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ARPEGGIO_SPEED)
    {
      pr->center_x = LAYOUT_CART_HEADER_ARPEGGIO_X;
      pr->center_y = LAYOUT_CART_ARPEGGIO_SPEED_Y;

      pr->lower_bound = PATCH_ARPEGGIO_SPEED_LOWER_BOUND;
      pr->upper_bound = PATCH_ARPEGGIO_SPEED_UPPER_BOUND;
    }
    /* portamento */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PORTAMENTO_MODE)
    {
      pr->center_x = LAYOUT_CART_HEADER_PORTAMENTO_X;
      pr->center_y = LAYOUT_CART_PORTAMENTO_MODE_Y;

      pr->lower_bound = PATCH_PORTAMENTO_MODE_LOWER_BOUND;
      pr->upper_bound = PATCH_PORTAMENTO_MODE_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PORTAMENTO_LEGATO)
    {
      pr->center_x = LAYOUT_CART_HEADER_PORTAMENTO_X;
      pr->center_y = LAYOUT_CART_PORTAMENTO_LEGATO_Y;

      pr->lower_bound = PATCH_PORTAMENTO_LEGATO_LOWER_BOUND;
      pr->upper_bound = PATCH_PORTAMENTO_LEGATO_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PORTAMENTO_SPEED)
    {
      pr->center_x = LAYOUT_CART_HEADER_PORTAMENTO_X;
      pr->center_y = LAYOUT_CART_PORTAMENTO_SPEED_Y;

      pr->lower_bound = PATCH_PORTAMENTO_SPEED_LOWER_BOUND;
      pr->upper_bound = PATCH_PORTAMENTO_SPEED_UPPER_BOUND;
    }
    /* pitch wheel */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_WHEEL_MODE)
    {
      pr->center_x = LAYOUT_CART_HEADER_PITCH_WHEEL_X;
      pr->center_y = LAYOUT_CART_PITCH_WHEEL_MODE_Y;

      pr->lower_bound = PATCH_PITCH_WHEEL_MODE_LOWER_BOUND;
      pr->upper_bound = PATCH_PITCH_WHEEL_MODE_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_WHEEL_RANGE)
    {
      pr->center_x = LAYOUT_CART_HEADER_PITCH_WHEEL_X;
      pr->center_y = LAYOUT_CART_PITCH_WHEEL_RANGE_Y;

      pr->lower_bound = PATCH_PITCH_WHEEL_RANGE_LOWER_BOUND;
      pr->upper_bound = PATCH_PITCH_WHEEL_RANGE_UPPER_BOUND;
    }
    /* tremolo routing */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_ROUTING_ENV_1)
    {
      pr->center_x = LAYOUT_CART_HEADER_TREMOLO_ROUTING_X;
      pr->center_y = LAYOUT_CART_TREMOLO_ROUTING_ENV_1_Y;

      pr->lower_bound = PATCH_ENV_ADJUST_ROUTING_CLEAR;
      pr->upper_bound = PATCH_ENV_ADJUST_ROUTING_FLAG_1;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_ROUTING_ENV_2)
    {
      pr->center_x = LAYOUT_CART_HEADER_TREMOLO_ROUTING_X;
      pr->center_y = LAYOUT_CART_TREMOLO_ROUTING_ENV_2_Y;

      pr->lower_bound = PATCH_ENV_ADJUST_ROUTING_CLEAR;
      pr->upper_bound = PATCH_ENV_ADJUST_ROUTING_FLAG_2;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_ROUTING_ENV_3)
    {
      pr->center_x = LAYOUT_CART_HEADER_TREMOLO_ROUTING_X;
      pr->center_y = LAYOUT_CART_TREMOLO_ROUTING_ENV_3_Y;

      pr->lower_bound = PATCH_ENV_ADJUST_ROUTING_CLEAR;
      pr->upper_bound = PATCH_ENV_ADJUST_ROUTING_FLAG_3;
    }
    /* boost routing */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_BOOST_ROUTING_ENV_1)
    {
      pr->center_x = LAYOUT_CART_HEADER_BOOST_ROUTING_X;
      pr->center_y = LAYOUT_CART_BOOST_ROUTING_ENV_1_Y;

      pr->lower_bound = PATCH_ENV_ADJUST_ROUTING_CLEAR;
      pr->upper_bound = PATCH_ENV_ADJUST_ROUTING_FLAG_1;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_BOOST_ROUTING_ENV_2)
    {
      pr->center_x = LAYOUT_CART_HEADER_BOOST_ROUTING_X;
      pr->center_y = LAYOUT_CART_BOOST_ROUTING_ENV_2_Y;

      pr->lower_bound = PATCH_ENV_ADJUST_ROUTING_CLEAR;
      pr->upper_bound = PATCH_ENV_ADJUST_ROUTING_FLAG_2;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_BOOST_ROUTING_ENV_3)
    {
      pr->center_x = LAYOUT_CART_HEADER_BOOST_ROUTING_X;
      pr->center_y = LAYOUT_CART_BOOST_ROUTING_ENV_3_Y;

      pr->lower_bound = PATCH_ENV_ADJUST_ROUTING_CLEAR;
      pr->upper_bound = PATCH_ENV_ADJUST_ROUTING_FLAG_3;
    }
    /* velocity routing */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_VELOCITY_ROUTING_ENV_1)
    {
      pr->center_x = LAYOUT_CART_HEADER_VELOCITY_ROUTING_X;
      pr->center_y = LAYOUT_CART_VELOCITY_ROUTING_ENV_1_Y;

      pr->lower_bound = PATCH_ENV_ADJUST_ROUTING_CLEAR;
      pr->upper_bound = PATCH_ENV_ADJUST_ROUTING_FLAG_1;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_VELOCITY_ROUTING_ENV_2)
    {
      pr->center_x = LAYOUT_CART_HEADER_VELOCITY_ROUTING_X;
      pr->center_y = LAYOUT_CART_VELOCITY_ROUTING_ENV_2_Y;

      pr->lower_bound = PATCH_ENV_ADJUST_ROUTING_CLEAR;
      pr->upper_bound = PATCH_ENV_ADJUST_ROUTING_FLAG_2;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_VELOCITY_ROUTING_ENV_3)
    {
      pr->center_x = LAYOUT_CART_HEADER_VELOCITY_ROUTING_X;
      pr->center_y = LAYOUT_CART_VELOCITY_ROUTING_ENV_3_Y;

      pr->lower_bound = PATCH_ENV_ADJUST_ROUTING_CLEAR;
      pr->upper_bound = PATCH_ENV_ADJUST_ROUTING_FLAG_3;
    }
    /* mod wheel routing */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_MOD_WHEEL_ROUTING_VIBRATO)
    {
      pr->center_x = LAYOUT_CART_HEADER_MOD_WHEEL_ROUTING_X;
      pr->center_y = LAYOUT_CART_MOD_WHEEL_ROUTING_VIBRATO_Y;

      pr->lower_bound = PATCH_MIDI_CONT_ROUTING_CLEAR;
      pr->upper_bound = PATCH_MIDI_CONT_ROUTING_FLAG_VIBRATO;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_MOD_WHEEL_ROUTING_TREMOLO)
    {
      pr->center_x = LAYOUT_CART_HEADER_MOD_WHEEL_ROUTING_X;
      pr->center_y = LAYOUT_CART_MOD_WHEEL_ROUTING_TREMOLO_Y;

      pr->lower_bound = PATCH_MIDI_CONT_ROUTING_CLEAR;
      pr->upper_bound = PATCH_MIDI_CONT_ROUTING_FLAG_TREMOLO;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_MOD_WHEEL_ROUTING_BOOST)
    {
      pr->center_x = LAYOUT_CART_HEADER_MOD_WHEEL_ROUTING_X;
      pr->center_y = LAYOUT_CART_MOD_WHEEL_ROUTING_BOOST_Y;

      pr->lower_bound = PATCH_MIDI_CONT_ROUTING_CLEAR;
      pr->upper_bound = PATCH_MIDI_CONT_ROUTING_FLAG_BOOST;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_MOD_WHEEL_ROUTING_CHORUS)
    {
      pr->center_x = LAYOUT_CART_HEADER_MOD_WHEEL_ROUTING_X;
      pr->center_y = LAYOUT_CART_MOD_WHEEL_ROUTING_CHORUS_Y;

      pr->lower_bound = PATCH_MIDI_CONT_ROUTING_CLEAR;
      pr->upper_bound = PATCH_MIDI_CONT_ROUTING_FLAG_CHORUS;
    }
    /* aftertouch routing */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AFTERTOUCH_ROUTING_VIBRATO)
    {
      pr->center_x = LAYOUT_CART_HEADER_AFTERTOUCH_ROUTING_X;
      pr->center_y = LAYOUT_CART_AFTERTOUCH_ROUTING_VIBRATO_Y;

      pr->lower_bound = PATCH_MIDI_CONT_ROUTING_CLEAR;
      pr->upper_bound = PATCH_MIDI_CONT_ROUTING_FLAG_VIBRATO;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AFTERTOUCH_ROUTING_TREMOLO)
    {
      pr->center_x = LAYOUT_CART_HEADER_AFTERTOUCH_ROUTING_X;
      pr->center_y = LAYOUT_CART_AFTERTOUCH_ROUTING_TREMOLO_Y;

      pr->lower_bound = PATCH_MIDI_CONT_ROUTING_CLEAR;
      pr->upper_bound = PATCH_MIDI_CONT_ROUTING_FLAG_TREMOLO;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AFTERTOUCH_ROUTING_BOOST)
    {
      pr->center_x = LAYOUT_CART_HEADER_AFTERTOUCH_ROUTING_X;
      pr->center_y = LAYOUT_CART_AFTERTOUCH_ROUTING_BOOST_Y;

      pr->lower_bound = PATCH_MIDI_CONT_ROUTING_CLEAR;
      pr->upper_bound = PATCH_MIDI_CONT_ROUTING_FLAG_BOOST;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AFTERTOUCH_ROUTING_CHORUS)
    {
      pr->center_x = LAYOUT_CART_HEADER_AFTERTOUCH_ROUTING_X;
      pr->center_y = LAYOUT_CART_AFTERTOUCH_ROUTING_CHORUS_Y;

      pr->lower_bound = PATCH_MIDI_CONT_ROUTING_CLEAR;
      pr->upper_bound = PATCH_MIDI_CONT_ROUTING_FLAG_CHORUS;
    }
    /* exp pedal routing */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_EXP_PEDAL_ROUTING_VIBRATO)
    {
      pr->center_x = LAYOUT_CART_HEADER_EXP_PEDAL_ROUTING_X;
      pr->center_y = LAYOUT_CART_EXP_PEDAL_ROUTING_VIBRATO_Y;

      pr->lower_bound = PATCH_MIDI_CONT_ROUTING_CLEAR;
      pr->upper_bound = PATCH_MIDI_CONT_ROUTING_FLAG_VIBRATO;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_EXP_PEDAL_ROUTING_TREMOLO)
    {
      pr->center_x = LAYOUT_CART_HEADER_EXP_PEDAL_ROUTING_X;
      pr->center_y = LAYOUT_CART_EXP_PEDAL_ROUTING_TREMOLO_Y;

      pr->lower_bound = PATCH_MIDI_CONT_ROUTING_CLEAR;
      pr->upper_bound = PATCH_MIDI_CONT_ROUTING_FLAG_TREMOLO;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_EXP_PEDAL_ROUTING_BOOST)
    {
      pr->center_x = LAYOUT_CART_HEADER_EXP_PEDAL_ROUTING_X;
      pr->center_y = LAYOUT_CART_EXP_PEDAL_ROUTING_BOOST_Y;

      pr->lower_bound = PATCH_MIDI_CONT_ROUTING_CLEAR;
      pr->upper_bound = PATCH_MIDI_CONT_ROUTING_FLAG_BOOST;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_EXP_PEDAL_ROUTING_CHORUS)
    {
      pr->center_x = LAYOUT_CART_HEADER_EXP_PEDAL_ROUTING_X;
      pr->center_y = LAYOUT_CART_EXP_PEDAL_ROUTING_CHORUS_Y;

      pr->lower_bound = PATCH_MIDI_CONT_ROUTING_CLEAR;
      pr->upper_bound = PATCH_MIDI_CONT_ROUTING_FLAG_CHORUS;
    }
    /* audition */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_OCTAVE)
    {
      pr->center_x = LAYOUT_CART_AUDITION_OCTAVE_X;
      pr->center_y = LAYOUT_CART_AUDITION_OCTAVE_Y;

      pr->lower_bound = MIDI_CONT_OCTAVE_LOWER_BOUND;
      pr->upper_bound = MIDI_CONT_OCTAVE_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_VELOCITY)
    {
      pr->center_x = LAYOUT_CART_AUDITION_VELOCITY_X;
      pr->center_y = LAYOUT_CART_AUDITION_VELOCITY_Y;

      pr->lower_bound = MIDI_CONT_NOTE_VELOCITY_LOWER_BOUND;
      pr->upper_bound = MIDI_CONT_NOTE_VELOCITY_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_PITCH_WHEEL)
    {
      pr->center_x = LAYOUT_CART_AUDITION_PITCH_WHEEL_X;
      pr->center_y = LAYOUT_CART_AUDITION_PITCH_WHEEL_Y;

      pr->lower_bound = MIDI_CONT_BI_WHEEL_LOWER_BOUND;
      pr->upper_bound = MIDI_CONT_BI_WHEEL_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_MOD_WHEEL)
    {
      pr->center_x = LAYOUT_CART_AUDITION_MOD_WHEEL_X;
      pr->center_y = LAYOUT_CART_AUDITION_MOD_WHEEL_Y;

      pr->lower_bound = MIDI_CONT_UNI_WHEEL_LOWER_BOUND;
      pr->upper_bound = MIDI_CONT_UNI_WHEEL_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_AFTERTOUCH)
    {
      pr->center_x = LAYOUT_CART_AUDITION_AFTERTOUCH_X;
      pr->center_y = LAYOUT_CART_AUDITION_AFTERTOUCH_Y;

      pr->lower_bound = MIDI_CONT_UNI_WHEEL_LOWER_BOUND;
      pr->upper_bound = MIDI_CONT_UNI_WHEEL_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_EXP_PEDAL)
    {
      pr->center_x = LAYOUT_CART_AUDITION_EXP_PEDAL_X;
      pr->center_y = LAYOUT_CART_AUDITION_EXP_PEDAL_Y;

      pr->lower_bound = MIDI_CONT_UNI_WHEEL_LOWER_BOUND;
      pr->upper_bound = MIDI_CONT_UNI_WHEEL_UPPER_BOUND;
    }
    else
    {
      pr->center_x = 0;
      pr->center_y = 0;

      pr->lower_bound = 0;
      pr->upper_bound = 0;
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

/*******************************************************************************
** layout_reset_cart_button_states()
*******************************************************************************/
short int layout_reset_cart_button_states()
{
  int k;

  button* b;

  /* reset all button states */
  for ( k = LAYOUT_CART_BUTTONS_START_INDEX; 
        k < LAYOUT_CART_BUTTONS_END_INDEX; 
        k++)
  {
    b = &G_layout_buttons[k];

    b->state = LAYOUT_BUTTON_STATE_OFF;
  }

  return 0;
}

