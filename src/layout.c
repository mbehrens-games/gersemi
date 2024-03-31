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
      b->center_y = LAYOUT_CART_CART_NUMBER_Y;

      b->width = LAYOUT_CART_BUTTON_CART_LOAD_WIDTH;
      b->state = LAYOUT_BUTTON_STATE_OFF;
    }
    else if (b->label == LAYOUT_CART_BUTTON_LABEL_CART_SAVE)
    {
      b->center_x = LAYOUT_CART_BUTTON_CART_SAVE_X;
      b->center_y = LAYOUT_CART_CART_NUMBER_Y;

      b->width = LAYOUT_CART_BUTTON_CART_SAVE_WIDTH;
      b->state = LAYOUT_BUTTON_STATE_OFF;
    }
    else if (b->label == LAYOUT_CART_BUTTON_LABEL_CART_COPY)
    {
      b->center_x = LAYOUT_CART_BUTTON_CART_COPY_X;
      b->center_y = LAYOUT_CART_CART_NUMBER_Y;

      b->width = LAYOUT_CART_BUTTON_CART_COPY_WIDTH;
      b->state = LAYOUT_BUTTON_STATE_OFF;
    }
    else if (b->label == LAYOUT_CART_BUTTON_LABEL_CART_ZAP)
    {
      b->center_x = LAYOUT_CART_BUTTON_CART_ZAP_X;
      b->center_y = LAYOUT_CART_CART_NUMBER_Y;

      b->width = LAYOUT_CART_BUTTON_CART_ZAP_WIDTH;
      b->state = LAYOUT_BUTTON_STATE_OFF;
    }
    else if (b->label == LAYOUT_CART_BUTTON_LABEL_PATCH_LOAD)
    {
      b->center_x = LAYOUT_CART_BUTTON_PATCH_LOAD_X;
      b->center_y = LAYOUT_CART_PATCH_NUMBER_Y;

      b->width = LAYOUT_CART_BUTTON_PATCH_LOAD_WIDTH;
      b->state = LAYOUT_BUTTON_STATE_OFF;
    }
    else if (b->label == LAYOUT_CART_BUTTON_LABEL_PATCH_SAVE)
    {
      b->center_x = LAYOUT_CART_BUTTON_PATCH_SAVE_X;
      b->center_y = LAYOUT_CART_PATCH_NUMBER_Y;

      b->width = LAYOUT_CART_BUTTON_PATCH_SAVE_WIDTH;
      b->state = LAYOUT_BUTTON_STATE_OFF;
    }
    else if (b->label == LAYOUT_CART_BUTTON_LABEL_PATCH_COPY)
    {
      b->center_x = LAYOUT_CART_BUTTON_PATCH_COPY_X;
      b->center_y = LAYOUT_CART_PATCH_NUMBER_Y;

      b->width = LAYOUT_CART_BUTTON_PATCH_COPY_WIDTH;
      b->state = LAYOUT_BUTTON_STATE_OFF;
    }
    else if (b->label == LAYOUT_CART_BUTTON_LABEL_PATCH_ZAP)
    {
      b->center_x = LAYOUT_CART_BUTTON_PATCH_ZAP_X;
      b->center_y = LAYOUT_CART_PATCH_NUMBER_Y;

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
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_LOWPASS)
    {
      hd->center_x = LAYOUT_CART_HEADER_LOWPASS_X;
      hd->center_y = LAYOUT_CART_HEADER_LOWPASS_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_HIGHPASS)
    {
      hd->center_x = LAYOUT_CART_HEADER_HIGHPASS_X;
      hd->center_y = LAYOUT_CART_HEADER_HIGHPASS_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_AMP_ENV)
    {
      hd->center_x = LAYOUT_CART_HEADER_AMP_ENV_X;
      hd->center_y = LAYOUT_CART_HEADER_AMP_ENV_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_FILTER_ENV)
    {
      hd->center_x = LAYOUT_CART_HEADER_FILTER_ENV_X;
      hd->center_y = LAYOUT_CART_HEADER_FILTER_ENV_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_EXTRA_ENV)
    {
      hd->center_x = LAYOUT_CART_HEADER_EXTRA_ENV_X;
      hd->center_y = LAYOUT_CART_HEADER_EXTRA_ENV_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_PORTAMENTO)
    {
      hd->center_x = LAYOUT_CART_HEADER_PORTAMENTO_X;
      hd->center_y = LAYOUT_CART_HEADER_PORTAMENTO_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_ARPEGGIO)
    {
      hd->center_x = LAYOUT_CART_HEADER_ARPEGGIO_X;
      hd->center_y = LAYOUT_CART_HEADER_ARPEGGIO_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_LFO)
    {
      hd->center_x = LAYOUT_CART_HEADER_LFO_X;
      hd->center_y = LAYOUT_CART_HEADER_LFO_Y;
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
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_EFFECTS)
    {
      hd->center_x = LAYOUT_CART_HEADER_EFFECTS_X;
      hd->center_y = LAYOUT_CART_HEADER_EFFECTS_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_PEDAL)
    {
      hd->center_x = LAYOUT_CART_HEADER_PEDAL_X;
      hd->center_y = LAYOUT_CART_HEADER_PEDAL_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_VELOCITY)
    {
      hd->center_x = LAYOUT_CART_HEADER_VELOCITY_X;
      hd->center_y = LAYOUT_CART_HEADER_VELOCITY_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_SYNC)
    {
      hd->center_x = LAYOUT_CART_HEADER_SYNC_X;
      hd->center_y = LAYOUT_CART_HEADER_SYNC_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_BOOST)
    {
      hd->center_x = LAYOUT_CART_HEADER_BOOST_X;
      hd->center_y = LAYOUT_CART_HEADER_BOOST_Y;
    }
    else if (hd->label == LAYOUT_CART_HEADER_LABEL_PITCH_WHEEL)
    {
      hd->center_x = LAYOUT_CART_HEADER_PITCH_WHEEL_X;
      hd->center_y = LAYOUT_CART_HEADER_PITCH_WHEEL_Y;
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
    if ((pr->label == LAYOUT_CART_PARAM_LABEL_OSC_1_WAVEFORM)     || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_2_WAVEFORM)     || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_ARPEGGIO_PATTERN)   || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_LFO_WAVEFORM)       || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_MOD_WHEEL_EFFECT)   || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_AFTERTOUCH_EFFECT)  || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_EXP_PEDAL_EFFECT))
    {
      pr->type = LAYOUT_PARAM_TYPE_ARROWS;
    }
    else if ( (pr->label == LAYOUT_CART_PARAM_LABEL_PORTAMENTO_MODE)    || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_PORTAMENTO_FOLLOW)  || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_PORTAMENTO_LEGATO)  || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_ARPEGGIO_MODE)      || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_VIBRATO_MODE)       || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_MODE)       || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_VELOCITY_MODE)      || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_SYNC_OSC)           || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_SYNC_LFO)           || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_BOOST_MODE)         || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_WHEEL_MODE))
    {
      pr->type = LAYOUT_PARAM_TYPE_RADIO;
    }
    else if ( (pr->label == LAYOUT_CART_PARAM_LABEL_CART_NAME)  || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_PATCH_NAME) || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_PROG_NAME))
    {
      pr->type = LAYOUT_PARAM_TYPE_NAME;
    }
    else
      pr->type = LAYOUT_PARAM_TYPE_SLIDER;

    /* set position and bounds */

    /* cart number */
    if (pr->label == LAYOUT_CART_PARAM_LABEL_CART_NUMBER)
    {
      pr->center_x = LAYOUT_CART_COLUMN_1_CENTER_X;
      pr->center_y = LAYOUT_CART_CART_NUMBER_Y;

      pr->lower_bound = PATCH_CART_NO_LOWER_BOUND;
      pr->upper_bound = PATCH_CART_NO_UPPER_BOUND;
    }
    /* cart name */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_CART_NAME)
    {
      pr->center_x = LAYOUT_CART_COLUMN_2_CENTER_X;
      pr->center_y = LAYOUT_CART_CART_NUMBER_Y;

      pr->lower_bound = PATCH_CART_NO_LOWER_BOUND;
      pr->upper_bound = PATCH_CART_NO_UPPER_BOUND;
    }
    /* patch number */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PATCH_NUMBER)
    {
      pr->center_x = LAYOUT_CART_COLUMN_1_CENTER_X;
      pr->center_y = LAYOUT_CART_PATCH_NUMBER_Y;

      pr->lower_bound = PATCH_PATCH_NO_LOWER_BOUND;
      pr->upper_bound = PATCH_PATCH_NO_UPPER_BOUND;
    }
    /* patch name */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PATCH_NAME)
    {
      pr->center_x = LAYOUT_CART_COLUMN_2_CENTER_X;
      pr->center_y = LAYOUT_CART_PATCH_NUMBER_Y;

      pr->lower_bound = PATCH_PATCH_NO_LOWER_BOUND;
      pr->upper_bound = PATCH_PATCH_NO_UPPER_BOUND;
    }
    /* program */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PROGRAM)
    {
      pr->center_x = LAYOUT_CART_PROGRAM_X;
      pr->center_y = LAYOUT_CART_PROGRAM_Y;

      pr->lower_bound = PATCH_PROGRAM_LOWER_BOUND;
      pr->upper_bound = PATCH_PROGRAM_UPPER_BOUND;
    }
    /* program name */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PROG_NAME)
    {
      pr->center_x = LAYOUT_CART_PROG_NAME_X;
      pr->center_y = LAYOUT_CART_PROG_NAME_Y;

      pr->lower_bound = PATCH_PROGRAM_LOWER_BOUND;
      pr->upper_bound = PATCH_PROGRAM_UPPER_BOUND;
    }
    /* oscillator 1 */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_1_WAVEFORM)
    {
      pr->center_x = LAYOUT_CART_HEADER_OSC_1_X;
      pr->center_y = LAYOUT_CART_OSC_1_WAVEFORM_Y;

      pr->lower_bound = PATCH_OSC_WAVEFORM_LOWER_BOUND;
      pr->upper_bound = PATCH_OSC_WAVEFORM_UPPER_BOUND;
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
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_2_DETUNE)
    {
      pr->center_x = LAYOUT_CART_HEADER_OSC_2_X;
      pr->center_y = LAYOUT_CART_OSC_2_DETUNE_Y;

      pr->lower_bound = PATCH_OSC_DETUNE_LOWER_BOUND;
      pr->upper_bound = PATCH_OSC_DETUNE_UPPER_BOUND;
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
    /* lowpass */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_LOWPASS_MULTIPLE)
    {
      pr->center_x = LAYOUT_CART_HEADER_LOWPASS_X;
      pr->center_y = LAYOUT_CART_LOWPASS_MULTIPLE_Y;

      pr->lower_bound = PATCH_LOWPASS_MULTIPLE_LOWER_BOUND;
      pr->upper_bound = PATCH_LOWPASS_MULTIPLE_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_LOWPASS_DIVISOR)
    {
      pr->center_x = LAYOUT_CART_HEADER_LOWPASS_X;
      pr->center_y = LAYOUT_CART_LOWPASS_DIVISOR_Y;

      pr->lower_bound = PATCH_LOWPASS_DIVISOR_LOWER_BOUND;
      pr->upper_bound = PATCH_LOWPASS_DIVISOR_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_LOWPASS_KEYTRACKING)
    {
      pr->center_x = LAYOUT_CART_HEADER_LOWPASS_X;
      pr->center_y = LAYOUT_CART_LOWPASS_KEYTRACKING_Y;

      pr->lower_bound = PATCH_LOWPASS_KEYTRACKING_LOWER_BOUND;
      pr->upper_bound = PATCH_LOWPASS_KEYTRACKING_UPPER_BOUND;
    }
    /* highpass */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_HIGHPASS_CUTOFF)
    {
      pr->center_x = LAYOUT_CART_HEADER_HIGHPASS_X;
      pr->center_y = LAYOUT_CART_HIGHPASS_CUTOFF_Y;

      pr->lower_bound = PATCH_HIGHPASS_CUTOFF_LOWER_BOUND;
      pr->upper_bound = PATCH_HIGHPASS_CUTOFF_UPPER_BOUND;
    }
    /* amplitude envelope */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AMP_ENV_ATTACK)
    {
      pr->center_x = LAYOUT_CART_HEADER_AMP_ENV_X;
      pr->center_y = LAYOUT_CART_AMP_ENV_ATTACK_Y;

      pr->lower_bound = PATCH_ENV_TIME_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_TIME_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AMP_ENV_DECAY_1)
    {
      pr->center_x = LAYOUT_CART_HEADER_AMP_ENV_X;
      pr->center_y = LAYOUT_CART_AMP_ENV_DECAY_1_Y;

      pr->lower_bound = PATCH_ENV_TIME_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_TIME_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AMP_ENV_DECAY_2)
    {
      pr->center_x = LAYOUT_CART_HEADER_AMP_ENV_X;
      pr->center_y = LAYOUT_CART_AMP_ENV_DECAY_2_Y;

      pr->lower_bound = PATCH_ENV_TIME_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_TIME_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AMP_ENV_RELEASE)
    {
      pr->center_x = LAYOUT_CART_HEADER_AMP_ENV_X;
      pr->center_y = LAYOUT_CART_AMP_ENV_RELEASE_Y;

      pr->lower_bound = PATCH_ENV_TIME_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_TIME_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AMP_ENV_SUSTAIN)
    {
      pr->center_x = LAYOUT_CART_HEADER_AMP_ENV_X;
      pr->center_y = LAYOUT_CART_AMP_ENV_SUSTAIN_Y;

      pr->lower_bound = PATCH_ENV_LEVEL_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_LEVEL_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AMP_ENV_RATE_KS)
    {
      pr->center_x = LAYOUT_CART_HEADER_AMP_ENV_X;
      pr->center_y = LAYOUT_CART_AMP_ENV_RATE_KS_Y;

      pr->lower_bound = PATCH_ENV_KEYSCALING_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_KEYSCALING_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AMP_ENV_LEFT_KS)
    {
      pr->center_x = LAYOUT_CART_HEADER_AMP_ENV_X;
      pr->center_y = LAYOUT_CART_AMP_ENV_LEFT_KS_Y;

      pr->lower_bound = PATCH_ENV_KEYSCALING_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_KEYSCALING_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AMP_ENV_RIGHT_KS)
    {
      pr->center_x = LAYOUT_CART_HEADER_AMP_ENV_X;
      pr->center_y = LAYOUT_CART_AMP_ENV_RIGHT_KS_Y;

      pr->lower_bound = PATCH_ENV_KEYSCALING_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_KEYSCALING_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AMP_ENV_BREAK_POINT)
    {
      pr->center_x = LAYOUT_CART_HEADER_AMP_ENV_X;
      pr->center_y = LAYOUT_CART_AMP_ENV_BREAK_POINT_Y;

      pr->lower_bound = PATCH_ENV_BREAK_POINT_LOWER_BOUND;
      pr->upper_bound = PATCH_ENV_BREAK_POINT_UPPER_BOUND;
    }
    /* filter envelope */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_FILTER_ENV_ATTACK)
    {
      pr->center_x = LAYOUT_CART_HEADER_FILTER_ENV_X;
      pr->center_y = LAYOUT_CART_FILTER_ENV_ATTACK_Y;

      pr->lower_bound = PATCH_PEG_TIME_LOWER_BOUND;
      pr->upper_bound = PATCH_PEG_TIME_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_FILTER_ENV_DECAY_1)
    {
      pr->center_x = LAYOUT_CART_HEADER_FILTER_ENV_X;
      pr->center_y = LAYOUT_CART_FILTER_ENV_DECAY_1_Y;

      pr->lower_bound = PATCH_PEG_TIME_LOWER_BOUND;
      pr->upper_bound = PATCH_PEG_TIME_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_FILTER_ENV_DECAY_2)
    {
      pr->center_x = LAYOUT_CART_HEADER_FILTER_ENV_X;
      pr->center_y = LAYOUT_CART_FILTER_ENV_DECAY_2_Y;

      pr->lower_bound = PATCH_PEG_TIME_LOWER_BOUND;
      pr->upper_bound = PATCH_PEG_TIME_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_FILTER_ENV_RELEASE)
    {
      pr->center_x = LAYOUT_CART_HEADER_FILTER_ENV_X;
      pr->center_y = LAYOUT_CART_FILTER_ENV_RELEASE_Y;

      pr->lower_bound = PATCH_PEG_TIME_LOWER_BOUND;
      pr->upper_bound = PATCH_PEG_TIME_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_FILTER_ENV_AMPLITUDE)
    {
      pr->center_x = LAYOUT_CART_HEADER_FILTER_ENV_X;
      pr->center_y = LAYOUT_CART_FILTER_ENV_AMPLITUDE_Y;

      pr->lower_bound = PATCH_PEG_LEVEL_LOWER_BOUND;
      pr->upper_bound = PATCH_PEG_LEVEL_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_FILTER_ENV_SUSTAIN)
    {
      pr->center_x = LAYOUT_CART_HEADER_FILTER_ENV_X;
      pr->center_y = LAYOUT_CART_FILTER_ENV_SUSTAIN_Y;

      pr->lower_bound = PATCH_PEG_LEVEL_LOWER_BOUND;
      pr->upper_bound = PATCH_PEG_LEVEL_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_FILTER_ENV_HOLD)
    {
      pr->center_x = LAYOUT_CART_HEADER_FILTER_ENV_X;
      pr->center_y = LAYOUT_CART_FILTER_ENV_HOLD_Y;

      pr->lower_bound = PATCH_PEG_LEVEL_LOWER_BOUND;
      pr->upper_bound = PATCH_PEG_LEVEL_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_FILTER_ENV_FINALE)
    {
      pr->center_x = LAYOUT_CART_HEADER_FILTER_ENV_X;
      pr->center_y = LAYOUT_CART_FILTER_ENV_FINALE_Y;

      pr->lower_bound = PATCH_PEG_LEVEL_LOWER_BOUND;
      pr->upper_bound = PATCH_PEG_LEVEL_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_FILTER_ENV_RATE_KS)
    {
      pr->center_x = LAYOUT_CART_HEADER_FILTER_ENV_X;
      pr->center_y = LAYOUT_CART_FILTER_ENV_RATE_KS_Y;

      pr->lower_bound = PATCH_PEG_KEYSCALING_LOWER_BOUND;
      pr->upper_bound = PATCH_PEG_KEYSCALING_UPPER_BOUND;
    }
    /* extra envelope */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_EXTRA_ENV_ATTACK)
    {
      pr->center_x = LAYOUT_CART_HEADER_EXTRA_ENV_X;
      pr->center_y = LAYOUT_CART_EXTRA_ENV_ATTACK_Y;

      pr->lower_bound = PATCH_PEG_TIME_LOWER_BOUND;
      pr->upper_bound = PATCH_PEG_TIME_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_EXTRA_ENV_DECAY_1)
    {
      pr->center_x = LAYOUT_CART_HEADER_EXTRA_ENV_X;
      pr->center_y = LAYOUT_CART_EXTRA_ENV_DECAY_1_Y;

      pr->lower_bound = PATCH_PEG_TIME_LOWER_BOUND;
      pr->upper_bound = PATCH_PEG_TIME_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_EXTRA_ENV_DECAY_2)
    {
      pr->center_x = LAYOUT_CART_HEADER_EXTRA_ENV_X;
      pr->center_y = LAYOUT_CART_EXTRA_ENV_DECAY_2_Y;

      pr->lower_bound = PATCH_PEG_TIME_LOWER_BOUND;
      pr->upper_bound = PATCH_PEG_TIME_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_EXTRA_ENV_RELEASE)
    {
      pr->center_x = LAYOUT_CART_HEADER_EXTRA_ENV_X;
      pr->center_y = LAYOUT_CART_EXTRA_ENV_RELEASE_Y;

      pr->lower_bound = PATCH_PEG_TIME_LOWER_BOUND;
      pr->upper_bound = PATCH_PEG_TIME_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_EXTRA_ENV_AMPLITUDE)
    {
      pr->center_x = LAYOUT_CART_HEADER_EXTRA_ENV_X;
      pr->center_y = LAYOUT_CART_EXTRA_ENV_AMPLITUDE_Y;

      pr->lower_bound = PATCH_PEG_LEVEL_LOWER_BOUND;
      pr->upper_bound = PATCH_PEG_LEVEL_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_EXTRA_ENV_SUSTAIN)
    {
      pr->center_x = LAYOUT_CART_HEADER_EXTRA_ENV_X;
      pr->center_y = LAYOUT_CART_EXTRA_ENV_SUSTAIN_Y;

      pr->lower_bound = PATCH_PEG_LEVEL_LOWER_BOUND;
      pr->upper_bound = PATCH_PEG_LEVEL_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_EXTRA_ENV_HOLD)
    {
      pr->center_x = LAYOUT_CART_HEADER_EXTRA_ENV_X;
      pr->center_y = LAYOUT_CART_EXTRA_ENV_HOLD_Y;

      pr->lower_bound = PATCH_PEG_LEVEL_LOWER_BOUND;
      pr->upper_bound = PATCH_PEG_LEVEL_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_EXTRA_ENV_FINALE)
    {
      pr->center_x = LAYOUT_CART_HEADER_EXTRA_ENV_X;
      pr->center_y = LAYOUT_CART_EXTRA_ENV_FINALE_Y;

      pr->lower_bound = PATCH_PEG_LEVEL_LOWER_BOUND;
      pr->upper_bound = PATCH_PEG_LEVEL_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_EXTRA_ENV_RATE_KS)
    {
      pr->center_x = LAYOUT_CART_HEADER_EXTRA_ENV_X;
      pr->center_y = LAYOUT_CART_EXTRA_ENV_RATE_KS_Y;

      pr->lower_bound = PATCH_PEG_KEYSCALING_LOWER_BOUND;
      pr->upper_bound = PATCH_PEG_KEYSCALING_UPPER_BOUND;
    }
    /* portamento */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PORTAMENTO_MODE)
    {
      pr->center_x = LAYOUT_CART_HEADER_PORTAMENTO_X;
      pr->center_y = LAYOUT_CART_PORTAMENTO_MODE_Y;

      pr->lower_bound = PATCH_PORTAMENTO_MODE_LOWER_BOUND;
      pr->upper_bound = PATCH_PORTAMENTO_MODE_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PORTAMENTO_FOLLOW)
    {
      pr->center_x = LAYOUT_CART_HEADER_PORTAMENTO_X;
      pr->center_y = LAYOUT_CART_PORTAMENTO_FOLLOW_Y;

      pr->lower_bound = PATCH_PORTAMENTO_FOLLOW_LOWER_BOUND;
      pr->upper_bound = PATCH_PORTAMENTO_FOLLOW_UPPER_BOUND;
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
    /* lfo */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_LFO_WAVEFORM)
    {
      pr->center_x = LAYOUT_CART_HEADER_LFO_X;
      pr->center_y = LAYOUT_CART_LFO_WAVEFORM_Y;

      pr->lower_bound = PATCH_LFO_WAVEFORM_LOWER_BOUND;
      pr->upper_bound = PATCH_LFO_WAVEFORM_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_LFO_FREQUENCY)
    {
      pr->center_x = LAYOUT_CART_HEADER_LFO_X;
      pr->center_y = LAYOUT_CART_LFO_FREQUENCY_Y;

      pr->lower_bound = PATCH_LFO_FREQUENCY_LOWER_BOUND;
      pr->upper_bound = PATCH_LFO_FREQUENCY_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_LFO_DELAY)
    {
      pr->center_x = LAYOUT_CART_HEADER_LFO_X;
      pr->center_y = LAYOUT_CART_LFO_DELAY_Y;

      pr->lower_bound = PATCH_LFO_DELAY_LOWER_BOUND;
      pr->upper_bound = PATCH_LFO_DELAY_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_LFO_QUANTIZE)
    {
      pr->center_x = LAYOUT_CART_HEADER_LFO_X;
      pr->center_y = LAYOUT_CART_LFO_QUANTIZE_Y;

      pr->lower_bound = PATCH_LFO_QUANTIZE_LOWER_BOUND;
      pr->upper_bound = PATCH_LFO_QUANTIZE_UPPER_BOUND;
    }
    /* vibrato */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_VIBRATO_MODE)
    {
      pr->center_x = LAYOUT_CART_HEADER_VIBRATO_X;
      pr->center_y = LAYOUT_CART_VIBRATO_MODE_Y;

      pr->lower_bound = PATCH_VIBRATO_MODE_LOWER_BOUND;
      pr->upper_bound = PATCH_VIBRATO_MODE_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_VIBRATO_DEPTH)
    {
      pr->center_x = LAYOUT_CART_HEADER_VIBRATO_X;
      pr->center_y = LAYOUT_CART_VIBRATO_DEPTH_Y;

      pr->lower_bound = PATCH_EFFECT_DEPTH_LOWER_BOUND;
      pr->upper_bound = PATCH_EFFECT_DEPTH_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_VIBRATO_BASE)
    {
      pr->center_x = LAYOUT_CART_HEADER_VIBRATO_X;
      pr->center_y = LAYOUT_CART_VIBRATO_BASE_Y;

      pr->lower_bound = PATCH_EFFECT_BASE_LOWER_BOUND;
      pr->upper_bound = PATCH_EFFECT_BASE_UPPER_BOUND;
    }
    /* tremolo */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_MODE)
    {
      pr->center_x = LAYOUT_CART_HEADER_TREMOLO_X;
      pr->center_y = LAYOUT_CART_TREMOLO_MODE_Y;

      pr->lower_bound = PATCH_TREMOLO_MODE_LOWER_BOUND;
      pr->upper_bound = PATCH_TREMOLO_MODE_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_DEPTH)
    {
      pr->center_x = LAYOUT_CART_HEADER_TREMOLO_X;
      pr->center_y = LAYOUT_CART_TREMOLO_DEPTH_Y;

      pr->lower_bound = PATCH_EFFECT_DEPTH_LOWER_BOUND;
      pr->upper_bound = PATCH_EFFECT_DEPTH_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_BASE)
    {
      pr->center_x = LAYOUT_CART_HEADER_TREMOLO_X;
      pr->center_y = LAYOUT_CART_TREMOLO_BASE_Y;

      pr->lower_bound = PATCH_EFFECT_BASE_LOWER_BOUND;
      pr->upper_bound = PATCH_EFFECT_BASE_UPPER_BOUND;
    }
    /* effects */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_MOD_WHEEL_EFFECT)
    {
      pr->center_x = LAYOUT_CART_HEADER_EFFECTS_X;
      pr->center_y = LAYOUT_CART_MOD_WHEEL_EFFECT_Y;

      pr->lower_bound = PATCH_CONTROLLER_EFFECT_LOWER_BOUND;
      pr->upper_bound = PATCH_CONTROLLER_EFFECT_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AFTERTOUCH_EFFECT)
    {
      pr->center_x = LAYOUT_CART_HEADER_EFFECTS_X;
      pr->center_y = LAYOUT_CART_AFTERTOUCH_EFFECT_Y;

      pr->lower_bound = PATCH_CONTROLLER_EFFECT_LOWER_BOUND;
      pr->upper_bound = PATCH_CONTROLLER_EFFECT_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_EXP_PEDAL_EFFECT)
    {
      pr->center_x = LAYOUT_CART_HEADER_EFFECTS_X;
      pr->center_y = LAYOUT_CART_EXP_PEDAL_EFFECT_Y;

      pr->lower_bound = PATCH_CONTROLLER_EFFECT_LOWER_BOUND;
      pr->upper_bound = PATCH_CONTROLLER_EFFECT_UPPER_BOUND;
    }
    /* sustain pedal */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PEDAL_ADJUST)
    {
      pr->center_x = LAYOUT_CART_HEADER_PEDAL_X;
      pr->center_y = LAYOUT_CART_PEDAL_ADJUST_Y;

      pr->lower_bound = PATCH_PEDAL_ADJUST_LOWER_BOUND;
      pr->upper_bound = PATCH_PEDAL_ADJUST_UPPER_BOUND;
    }
    /* velocity */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_VELOCITY_MODE)
    {
      pr->center_x = LAYOUT_CART_HEADER_VELOCITY_X;
      pr->center_y = LAYOUT_CART_VELOCITY_MODE_Y;

      pr->lower_bound = PATCH_VELOCITY_MODE_LOWER_BOUND;
      pr->upper_bound = PATCH_VELOCITY_MODE_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_VELOCITY_SCALING)
    {
      pr->center_x = LAYOUT_CART_HEADER_VELOCITY_X;
      pr->center_y = LAYOUT_CART_VELOCITY_SCALING_Y;

      pr->lower_bound = PATCH_VELOCITY_SCALING_LOWER_BOUND;
      pr->upper_bound = PATCH_VELOCITY_SCALING_UPPER_BOUND;
    }
    /* sync */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_SYNC_OSC)
    {
      pr->center_x = LAYOUT_CART_HEADER_SYNC_X;
      pr->center_y = LAYOUT_CART_SYNC_OSC_Y;

      pr->lower_bound = PATCH_SYNC_LOWER_BOUND;
      pr->upper_bound = PATCH_SYNC_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_SYNC_LFO)
    {
      pr->center_x = LAYOUT_CART_HEADER_SYNC_X;
      pr->center_y = LAYOUT_CART_SYNC_LFO_Y;

      pr->lower_bound = PATCH_SYNC_LOWER_BOUND;
      pr->upper_bound = PATCH_SYNC_UPPER_BOUND;
    }
    /* boost */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_BOOST_MODE)
    {
      pr->center_x = LAYOUT_CART_HEADER_BOOST_X;
      pr->center_y = LAYOUT_CART_BOOST_MODE_Y;

      pr->lower_bound = PATCH_BOOST_MODE_LOWER_BOUND;
      pr->upper_bound = PATCH_BOOST_MODE_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_BOOST_DEPTH)
    {
      pr->center_x = LAYOUT_CART_HEADER_BOOST_X;
      pr->center_y = LAYOUT_CART_BOOST_DEPTH_Y;

      pr->lower_bound = PATCH_EFFECT_DEPTH_LOWER_BOUND;
      pr->upper_bound = PATCH_EFFECT_DEPTH_UPPER_BOUND;
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
    /* audition */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_VELOCITY)
    {
      pr->center_x = LAYOUT_CART_COLUMN_1_CENTER_X;
      pr->center_y = LAYOUT_CART_AUDITION_VELOCITY_Y;

      pr->lower_bound = MIDI_CONT_NOTE_VELOCITY_LOWER_BOUND;
      pr->upper_bound = MIDI_CONT_NOTE_VELOCITY_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_MOD_WHEEL)
    {
      pr->center_x = LAYOUT_CART_COLUMN_2_CENTER_X;
      pr->center_y = LAYOUT_CART_AUDITION_MOD_WHEEL_Y;

      pr->lower_bound = MIDI_CONT_MOD_WHEEL_LOWER_BOUND;
      pr->upper_bound = MIDI_CONT_MOD_WHEEL_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_AFTERTOUCH)
    {
      pr->center_x = LAYOUT_CART_COLUMN_3_CENTER_X;
      pr->center_y = LAYOUT_CART_AUDITION_AFTERTOUCH_Y;

      pr->lower_bound = MIDI_CONT_AFTERTOUCH_LOWER_BOUND;
      pr->upper_bound = MIDI_CONT_AFTERTOUCH_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_PITCH_WHEEL)
    {
      pr->center_x = LAYOUT_CART_COLUMN_4_CENTER_X;
      pr->center_y = LAYOUT_CART_AUDITION_PITCH_WHEEL_Y;

      pr->lower_bound = MIDI_CONT_PITCH_WHEEL_LOWER_BOUND;
      pr->upper_bound = MIDI_CONT_PITCH_WHEEL_UPPER_BOUND;
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

