/*******************************************************************************
** layout.c (screen layout widget positions)
*******************************************************************************/

#include <stdio.h>  /* testing */
#include <stdlib.h>

#include "key.h"
#include "layout.h"
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
    if (b->label == LAYOUT_BUTTON_LABEL_PATCHES)
    {
      b->center_x = LAYOUT_TOP_PANEL_BUTTON_PATCHES_X;
      b->center_y = LAYOUT_TOP_PANEL_BUTTON_PATCHES_Y;

      b->width = LAYOUT_TOP_PANEL_BUTTON_PATCHES_WIDTH;
      b->state = LAYOUT_BUTTON_STATE_ON;
    }
    else if (b->label == LAYOUT_BUTTON_LABEL_PATTERNS)
    {
      b->center_x = LAYOUT_TOP_PANEL_BUTTON_PATTERNS_X;
      b->center_y = LAYOUT_TOP_PANEL_BUTTON_PATTERNS_Y;

      b->width = LAYOUT_TOP_PANEL_BUTTON_PATTERNS_WIDTH;
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
  for ( k = LAYOUT_PATCH_EDIT_HEADERS_START_INDEX; 
        k < LAYOUT_PATCH_EDIT_HEADERS_END_INDEX; 
        k++)
  {
    hd = &G_layout_headers[k];

    /* determine label */
    hd->label = k - LAYOUT_PATCH_EDIT_HEADERS_START_INDEX;

    /* set position */
    if (hd->label == LAYOUT_PATCH_EDIT_HEADER_LABEL_OSC_1)
    {
      hd->center_x = LAYOUT_PATCH_EDIT_COLUMN_1_CENTER_X;
      hd->center_y = LAYOUT_PATCH_EDIT_HEADER_OSC_Y;
    }
    else if (hd->label == LAYOUT_PATCH_EDIT_HEADER_LABEL_OSC_2)
    {
      hd->center_x = LAYOUT_PATCH_EDIT_COLUMN_2_CENTER_X;
      hd->center_y = LAYOUT_PATCH_EDIT_HEADER_OSC_Y;
    }
    else if (hd->label == LAYOUT_PATCH_EDIT_HEADER_LABEL_OSC_3)
    {
      hd->center_x = LAYOUT_PATCH_EDIT_COLUMN_3_CENTER_X;
      hd->center_y = LAYOUT_PATCH_EDIT_HEADER_OSC_Y;
    }
    else if (hd->label == LAYOUT_PATCH_EDIT_HEADER_LABEL_OSC_4)
    {
      hd->center_x = LAYOUT_PATCH_EDIT_COLUMN_4_CENTER_X;
      hd->center_y = LAYOUT_PATCH_EDIT_HEADER_OSC_Y;
    }
    else if (hd->label == LAYOUT_PATCH_EDIT_HEADER_LABEL_ENV_1)
    {
      hd->center_x = LAYOUT_PATCH_EDIT_COLUMN_1_CENTER_X;
      hd->center_y = LAYOUT_PATCH_EDIT_HEADER_ENV_Y;
    }
    else if (hd->label == LAYOUT_PATCH_EDIT_HEADER_LABEL_ENV_2)
    {
      hd->center_x = LAYOUT_PATCH_EDIT_COLUMN_2_CENTER_X;
      hd->center_y = LAYOUT_PATCH_EDIT_HEADER_ENV_Y;
    }
    else if (hd->label == LAYOUT_PATCH_EDIT_HEADER_LABEL_ENV_3)
    {
      hd->center_x = LAYOUT_PATCH_EDIT_COLUMN_3_CENTER_X;
      hd->center_y = LAYOUT_PATCH_EDIT_HEADER_ENV_Y;
    }
    else if (hd->label == LAYOUT_PATCH_EDIT_HEADER_LABEL_ENV_4)
    {
      hd->center_x = LAYOUT_PATCH_EDIT_COLUMN_4_CENTER_X;
      hd->center_y = LAYOUT_PATCH_EDIT_HEADER_ENV_Y;
    }
    else if (hd->label == LAYOUT_PATCH_EDIT_HEADER_LABEL_LFO)
    {
      hd->center_x = LAYOUT_PATCH_EDIT_COLUMN_1_CENTER_X;
      hd->center_y = LAYOUT_PATCH_EDIT_HEADER_LFO_Y;
    }
    else if (hd->label == LAYOUT_PATCH_EDIT_HEADER_LABEL_BASES)
    {
      hd->center_x = LAYOUT_PATCH_EDIT_COLUMN_2_CENTER_X;
      hd->center_y = LAYOUT_PATCH_EDIT_HEADER_BASES_Y;
    }
    else if (hd->label == LAYOUT_PATCH_EDIT_HEADER_LABEL_PORTAMENTO)
    {
      hd->center_x = LAYOUT_PATCH_EDIT_COLUMN_3_CENTER_X;
      hd->center_y = LAYOUT_PATCH_EDIT_HEADER_PORTAMENTO_Y;
    }
    else if (hd->label == LAYOUT_PATCH_EDIT_HEADER_LABEL_FILTERS)
    {
      hd->center_x = LAYOUT_PATCH_EDIT_COLUMN_4_CENTER_X;
      hd->center_y = LAYOUT_PATCH_EDIT_HEADER_FILTERS_Y;
    }
    else if (hd->label == LAYOUT_PATCH_EDIT_HEADER_LABEL_DEPTHS)
    {
      hd->center_x = LAYOUT_PATCH_EDIT_COLUMN_2_CENTER_X;
      hd->center_y = LAYOUT_PATCH_EDIT_HEADER_DEPTHS_Y;
    }
    else if (hd->label == LAYOUT_PATCH_EDIT_HEADER_LABEL_MOD_WHEEL)
    {
      hd->center_x = LAYOUT_PATCH_EDIT_COLUMN_3_CENTER_X;
      hd->center_y = LAYOUT_PATCH_EDIT_HEADER_MOD_WHEEL_Y;
    }
    else if (hd->label == LAYOUT_PATCH_EDIT_HEADER_LABEL_AFTERTOUCH)
    {
      hd->center_x = LAYOUT_PATCH_EDIT_COLUMN_4_CENTER_X;
      hd->center_y = LAYOUT_PATCH_EDIT_HEADER_AFTERTOUCH_Y;
    }
    else if (hd->label == LAYOUT_PATCH_EDIT_HEADER_LABEL_AUDITION)
    {
      hd->center_x = LAYOUT_PATCH_EDIT_COLUMN_1_CENTER_X;
      hd->center_y = LAYOUT_PATCH_EDIT_HEADER_AUDITION_Y;
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

    if ((shifted_index >= 0) && (shifted_index < LAYOUT_PATCH_EDIT_PARAM_FIRST_OSC_ENV_INDEX))
    {
      pr->label = shifted_index;
      pr->num = 0;
    }
    else if ( (shifted_index >= LAYOUT_PATCH_EDIT_PARAM_FIRST_OSC_ENV_INDEX) && 
              (shifted_index <  LAYOUT_PATCH_EDIT_PARAM_LAST_OSC_ENV_INDEX))
    {
      pr->label = (shifted_index - LAYOUT_PATCH_EDIT_PARAM_FIRST_OSC_ENV_INDEX) % LAYOUT_PATCH_EDIT_PARAM_NUM_OSC_ENV_LABELS;
      pr->label += LAYOUT_PATCH_EDIT_PARAM_FIRST_OSC_ENV_LABEL;

      pr->num = (shifted_index - LAYOUT_PATCH_EDIT_PARAM_FIRST_OSC_ENV_INDEX) / LAYOUT_PATCH_EDIT_PARAM_NUM_OSC_ENV_LABELS;
    }
    else
    {
      pr->label = shifted_index - LAYOUT_PATCH_EDIT_PARAM_LAST_OSC_ENV_INDEX;
      pr->label += LAYOUT_PATCH_EDIT_PARAM_LAST_OSC_ENV_LABEL + 1;

      pr->num = 0;
    }

    /* set type */
    if ((pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_OSC_WAVEFORM) || 
        (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_OSC_SYNC)     || 
        (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_ENV_TRIGGER)  || 
        (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_LFO_WAVEFORM) || 
        (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_LFO_TEMPO)    || 
        (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_PORTAMENTO_MODE))
    {
      pr->type = LAYOUT_PATCH_EDIT_PARAM_TYPE_ARROWS;
    }
    else if ( (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_OSC_FREQ_MODE)    || 
              (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_VIBRATO_ENABLE)   || 
              (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_TREMOLO_ENABLE)   || 
              (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_BOOST_ENABLE)     || 
              (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_LFO_VIBRATO_MODE) || 
              (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_LFO_SYNC)         || 
              (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_AUDITION_MODE))
    {
      pr->type = LAYOUT_PATCH_EDIT_PARAM_TYPE_RADIO;
    }
    else
      pr->type = LAYOUT_PATCH_EDIT_PARAM_TYPE_SLIDER;

    /* set position and bounds */
    if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_ALGORITHM)
    {
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_1_CENTER_X;
      pr->center_y = LAYOUT_PATCH_EDIT_ALGORITHM_Y;

      pr->lower_bound = PATCH_ALGORITHM_LOWER_BOUND;
      pr->upper_bound = PATCH_ALGORITHM_UPPER_BOUND;
    }
    else if ( (pr->label >= LAYOUT_PATCH_EDIT_PARAM_FIRST_OSC_ENV_LABEL) && 
              (pr->label <= LAYOUT_PATCH_EDIT_PARAM_LAST_OSC_ENV_LABEL))
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
      if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_OSC_WAVEFORM)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_OSC_WAVEFORM_Y;

        pr->lower_bound = PATCH_OSC_WAVEFORM_LOWER_BOUND;
        pr->upper_bound = PATCH_OSC_WAVEFORM_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_OSC_FEEDBACK)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_OSC_FEEDBACK_Y;

        pr->lower_bound = PATCH_OSC_FEEDBACK_LOWER_BOUND;
        pr->upper_bound = PATCH_OSC_FEEDBACK_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_OSC_SYNC)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_OSC_SYNC_Y;

        pr->lower_bound = PATCH_OSC_SYNC_LOWER_BOUND;
        pr->upper_bound = PATCH_OSC_SYNC_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_OSC_FREQ_MODE)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_OSC_FREQ_MODE_Y;

        pr->lower_bound = PATCH_OSC_FREQ_MODE_LOWER_BOUND;
        pr->upper_bound = PATCH_OSC_FREQ_MODE_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_OSC_MULTIPLE)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_OSC_MULTIPLE_Y;

        pr->lower_bound = PATCH_OSC_MULTIPLE_LOWER_BOUND;
        pr->upper_bound = PATCH_OSC_MULTIPLE_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_OSC_DIVISOR)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_OSC_DIVISOR_Y;

        pr->lower_bound = PATCH_OSC_DIVISOR_LOWER_BOUND;
        pr->upper_bound = PATCH_OSC_DIVISOR_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_OSC_NOTE)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_OSC_NOTE_Y;

        pr->lower_bound = PATCH_OSC_NOTE_LOWER_BOUND;
        pr->upper_bound = PATCH_OSC_NOTE_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_OSC_OCTAVE)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_OSC_OCTAVE_Y;

        pr->lower_bound = PATCH_OSC_OCTAVE_LOWER_BOUND;
        pr->upper_bound = PATCH_OSC_OCTAVE_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_OSC_DETUNE)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_OSC_DETUNE_Y;

        pr->lower_bound = PATCH_OSC_DETUNE_LOWER_BOUND;
        pr->upper_bound = PATCH_OSC_DETUNE_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_ENV_ATTACK)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_ENV_ATTACK_Y;

        pr->lower_bound = PATCH_ENV_RATE_LOWER_BOUND;
        pr->upper_bound = PATCH_ENV_RATE_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_ENV_DECAY_1)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_ENV_DECAY_1_Y;

        pr->lower_bound = PATCH_ENV_RATE_LOWER_BOUND;
        pr->upper_bound = PATCH_ENV_RATE_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_ENV_DECAY_2)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_ENV_DECAY_2_Y;

        pr->lower_bound = PATCH_ENV_RATE_LOWER_BOUND;
        pr->upper_bound = PATCH_ENV_RATE_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_ENV_RELEASE)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_ENV_RELEASE_Y;

        pr->lower_bound = PATCH_ENV_RATE_LOWER_BOUND;
        pr->upper_bound = PATCH_ENV_RATE_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_ENV_AMPLITUDE)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_ENV_AMPLITUDE_Y;

        pr->lower_bound = PATCH_ENV_AMPLITUDE_LOWER_BOUND;
        pr->upper_bound = PATCH_ENV_AMPLITUDE_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_ENV_SUSTAIN)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_ENV_SUSTAIN_Y;

        pr->lower_bound = PATCH_ENV_SUSTAIN_LOWER_BOUND;
        pr->upper_bound = PATCH_ENV_SUSTAIN_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_ENV_RATE_KS)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_ENV_RATE_KS_Y;

        pr->lower_bound = PATCH_ENV_KEYSCALE_LOWER_BOUND;
        pr->upper_bound = PATCH_ENV_KEYSCALE_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_ENV_LEVEL_KS)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_ENV_LEVEL_KS_Y;

        pr->lower_bound = PATCH_ENV_KEYSCALE_LOWER_BOUND;
        pr->upper_bound = PATCH_ENV_KEYSCALE_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_ENV_TRIGGER)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_ENV_TRIGGER_Y;

        pr->lower_bound = PATCH_ENV_TRIGGER_LOWER_BOUND;
        pr->upper_bound = PATCH_ENV_TRIGGER_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_VIBRATO_ENABLE)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_VIBRATO_ENABLE_Y;

        pr->lower_bound = PATCH_MOD_ENABLE_LOWER_BOUND;
        pr->upper_bound = PATCH_MOD_ENABLE_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_TREMOLO_ENABLE)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_TREMOLO_ENABLE_Y;

        pr->lower_bound = PATCH_MOD_ENABLE_LOWER_BOUND;
        pr->upper_bound = PATCH_MOD_ENABLE_UPPER_BOUND;
      }
      else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_BOOST_ENABLE)
      {
        pr->center_y = LAYOUT_PATCH_EDIT_BOOST_ENABLE_Y;

        pr->lower_bound = PATCH_MOD_ENABLE_LOWER_BOUND;
        pr->upper_bound = PATCH_MOD_ENABLE_UPPER_BOUND;
      }
      else
      {
        pr->center_x = 0;
        pr->center_y = 0;

        pr->lower_bound = 0;
        pr->upper_bound = 0;
      }
    }
    else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_LFO_WAVEFORM)
    {
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_1_CENTER_X;
      pr->center_y = LAYOUT_PATCH_EDIT_LFO_WAVEFORM_Y;

      pr->lower_bound = PATCH_LFO_WAVEFORM_LOWER_BOUND;
      pr->upper_bound = PATCH_LFO_WAVEFORM_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_LFO_FREQUENCY)
    {
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_1_CENTER_X;
      pr->center_y = LAYOUT_PATCH_EDIT_LFO_FREQUENCY_Y;

      pr->lower_bound = PATCH_LFO_FREQUENCY_LOWER_BOUND;
      pr->upper_bound = PATCH_LFO_FREQUENCY_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_LFO_DELAY)
    {
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_1_CENTER_X;
      pr->center_y = LAYOUT_PATCH_EDIT_LFO_DELAY_Y;

      pr->lower_bound = PATCH_LFO_DELAY_LOWER_BOUND;
      pr->upper_bound = PATCH_LFO_DELAY_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_LFO_VIBRATO_MODE)
    {
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_1_CENTER_X;
      pr->center_y = LAYOUT_PATCH_EDIT_LFO_VIBRATO_MODE_Y;

      pr->lower_bound = PATCH_LFO_VIBRATO_MODE_LOWER_BOUND;
      pr->upper_bound = PATCH_LFO_VIBRATO_MODE_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_LFO_SYNC)
    {
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_1_CENTER_X;
      pr->center_y = LAYOUT_PATCH_EDIT_LFO_SYNC_Y;

      pr->lower_bound = PATCH_LFO_SYNC_LOWER_BOUND;
      pr->upper_bound = PATCH_LFO_SYNC_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_LFO_TEMPO)
    {
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_1_CENTER_X;
      pr->center_y = LAYOUT_PATCH_EDIT_LFO_TEMPO_Y;

      pr->lower_bound = PATCH_LFO_TEMPO_LOWER_BOUND;
      pr->upper_bound = PATCH_LFO_TEMPO_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_LFO_BASE_VIBRATO)
    {
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_2_CENTER_X;
      pr->center_y = LAYOUT_PATCH_EDIT_LFO_BASE_VIBRATO_Y;

      pr->lower_bound = PATCH_MOD_BASE_LOWER_BOUND;
      pr->upper_bound = PATCH_MOD_BASE_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_LFO_BASE_TREMOLO)
    {
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_2_CENTER_X;
      pr->center_y = LAYOUT_PATCH_EDIT_LFO_BASE_TREMOLO_Y;

      pr->lower_bound = PATCH_MOD_BASE_LOWER_BOUND;
      pr->upper_bound = PATCH_MOD_BASE_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_PORTAMENTO_MODE)
    {
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_3_CENTER_X;
      pr->center_y = LAYOUT_PATCH_EDIT_PORTAMENTO_MODE_Y;

      pr->lower_bound = PATCH_PORTAMENTO_MODE_LOWER_BOUND;
      pr->upper_bound = PATCH_PORTAMENTO_MODE_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_PORTAMENTO_SPEED)
    {
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_3_CENTER_X;
      pr->center_y = LAYOUT_PATCH_EDIT_PORTAMENTO_SPEED_Y;

      pr->lower_bound = PATCH_PORTAMENTO_SPEED_LOWER_BOUND;
      pr->upper_bound = PATCH_PORTAMENTO_SPEED_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_HIGHPASS_CUTOFF)
    {
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_4_CENTER_X;
      pr->center_y = LAYOUT_PATCH_EDIT_HIGHPASS_CUTOFF_Y;

      pr->lower_bound = PATCH_HIGHPASS_CUTOFF_LOWER_BOUND;
      pr->upper_bound = PATCH_HIGHPASS_CUTOFF_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_LOWPASS_CUTOFF)
    {
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_4_CENTER_X;
      pr->center_y = LAYOUT_PATCH_EDIT_LOWPASS_CUTOFF_Y;

      pr->lower_bound = PATCH_LOWPASS_CUTOFF_LOWER_BOUND;
      pr->upper_bound = PATCH_LOWPASS_CUTOFF_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_VIBRATO_DEPTH)
    {
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_2_CENTER_X;
      pr->center_y = LAYOUT_PATCH_EDIT_VIBRATO_DEPTH_Y;

      pr->lower_bound = PATCH_MOD_DEPTH_LOWER_BOUND;
      pr->upper_bound = PATCH_MOD_DEPTH_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_TREMOLO_DEPTH)
    {
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_2_CENTER_X;
      pr->center_y = LAYOUT_PATCH_EDIT_TREMOLO_DEPTH_Y;

      pr->lower_bound = PATCH_MOD_DEPTH_LOWER_BOUND;
      pr->upper_bound = PATCH_MOD_DEPTH_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_BOOST_DEPTH)
    {
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_2_CENTER_X;
      pr->center_y = LAYOUT_PATCH_EDIT_BOOST_DEPTH_Y;

      pr->lower_bound = PATCH_MOD_DEPTH_LOWER_BOUND;
      pr->upper_bound = PATCH_MOD_DEPTH_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_MOD_WHEEL_VIBRATO)
    {
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_3_CENTER_X;
      pr->center_y = LAYOUT_PATCH_EDIT_MOD_WHEEL_VIBRATO_Y;

      pr->lower_bound = PATCH_MOD_CONTROLLER_LOWER_BOUND;
      pr->upper_bound = PATCH_MOD_CONTROLLER_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_MOD_WHEEL_TREMOLO)
    {
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_3_CENTER_X;
      pr->center_y = LAYOUT_PATCH_EDIT_MOD_WHEEL_TREMOLO_Y;

      pr->lower_bound = PATCH_MOD_CONTROLLER_LOWER_BOUND;
      pr->upper_bound = PATCH_MOD_CONTROLLER_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_MOD_WHEEL_BOOST)
    {
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_3_CENTER_X;
      pr->center_y = LAYOUT_PATCH_EDIT_MOD_WHEEL_BOOST_Y;

      pr->lower_bound = PATCH_MOD_CONTROLLER_LOWER_BOUND;
      pr->upper_bound = PATCH_MOD_CONTROLLER_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_AFTERTOUCH_VIBRATO)
    {
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_4_CENTER_X;
      pr->center_y = LAYOUT_PATCH_EDIT_AFTERTOUCH_VIBRATO_Y;

      pr->lower_bound = PATCH_MOD_CONTROLLER_LOWER_BOUND;
      pr->upper_bound = PATCH_MOD_CONTROLLER_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_AFTERTOUCH_TREMOLO)
    {
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_4_CENTER_X;
      pr->center_y = LAYOUT_PATCH_EDIT_AFTERTOUCH_TREMOLO_Y;

      pr->lower_bound = PATCH_MOD_CONTROLLER_LOWER_BOUND;
      pr->upper_bound = PATCH_MOD_CONTROLLER_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_AFTERTOUCH_BOOST)
    {
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_4_CENTER_X;
      pr->center_y = LAYOUT_PATCH_EDIT_AFTERTOUCH_BOOST_Y;

      pr->lower_bound = PATCH_MOD_CONTROLLER_LOWER_BOUND;
      pr->upper_bound = PATCH_MOD_CONTROLLER_UPPER_BOUND;
    }
    else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_AUDITION_OCTAVE)
    {
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_2_CENTER_X;
      pr->center_y = LAYOUT_PATCH_EDIT_AUDITION_OCTAVE_Y;

      pr->lower_bound = (TUNING_MIDDLE_OCTAVE - 3);
      pr->upper_bound = (TUNING_MIDDLE_OCTAVE + 3);
    }
    else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_AUDITION_KEY)
    {
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_3_CENTER_X;
      pr->center_y = LAYOUT_PATCH_EDIT_AUDITION_KEY_Y;

      pr->lower_bound = 0;
      pr->upper_bound = KEY_NUM_SIGS - 1;
    }
    else if (pr->label == LAYOUT_PATCH_EDIT_PARAM_LABEL_AUDITION_MODE)
    {
      pr->center_x = LAYOUT_PATCH_EDIT_COLUMN_4_CENTER_X;
      pr->center_y = LAYOUT_PATCH_EDIT_AUDITION_MODE_Y;

      pr->lower_bound = KEY_MODE_MAJOR;
      pr->upper_bound = KEY_MODE_MINOR;
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

