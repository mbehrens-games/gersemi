/*******************************************************************************
** layout.c (screen layout widget positions)
*******************************************************************************/

#include <stdio.h>  /* testing */
#include <stdlib.h>

#include "layout.h"
#include "midicont.h"
#include "patch.h"
#include "tuning.h"

#define LAYOUT_SETUP_BUTTON_CASE(area, name)                                   \
  case LAYOUT_##area##_BUTTON_##name:                                          \
  {                                                                            \
    b->center_x = LAYOUT_##area##_BUTTON_##name##_X;                           \
    b->center_y = LAYOUT_##area##_BUTTON_##name##_Y;                           \
    b->width = LAYOUT_##area##_BUTTON_##name##_WIDTH;                          \
    b->state = LAYOUT_BUTTON_STATE_OFF;                                        \
                                                                               \
    break;                                                                     \
  }

#define LAYOUT_SETUP_TEXT_BOX_CASE(area, name)                                 \
  case LAYOUT_##area##_TEXT_BOX_##name:                                        \
  {                                                                            \
    t->center_x = LAYOUT_##area##_TEXT_BOX_##name##_X;                         \
    t->center_y = LAYOUT_##area##_TEXT_BOX_##name##_Y;                         \
    t->width = LAYOUT_##area##_TEXT_BOX_##name##_WIDTH;                        \
    t->state = LAYOUT_TEXT_BOX_STATE_NORMAL;                                   \
                                                                               \
    break;                                                                     \
  }

#define LAYOUT_SETUP_HEADER_CASE(area, name)                                   \
  case LAYOUT_##area##_HEADER_##name:                                          \
  {                                                                            \
    hd->center_x = LAYOUT_##area##_HEADER_##name##_X;                          \
    hd->center_y = LAYOUT_##area##_HEADER_##name##_Y;                          \
                                                                               \
    break;                                                                     \
  }

#define LAYOUT_SETUP_CART_PARAM_CASE_NO_HEADER(param_name, bound_name)         \
  case LAYOUT_CART_PARAM_##param_name:                                         \
  {                                                                            \
    pr->center_x = LAYOUT_CART_##param_name##_X;                               \
    pr->center_y = LAYOUT_CART_##param_name##_Y;                               \
    pr->lower_bound = PATCH_##bound_name##_LOWER_BOUND;                        \
    pr->upper_bound = PATCH_##bound_name##_UPPER_BOUND;                        \
                                                                               \
    break;                                                                     \
  }

#define LAYOUT_SETUP_CART_PARAM_CASE_INT(header_name, param_name, bound_name)  \
  case LAYOUT_CART_PARAM_##header_name##_##param_name:                         \
  {                                                                            \
    pr->center_x = LAYOUT_CART_HEADER_##header_name##_X;                       \
    pr->center_y = LAYOUT_CART_##header_name##_##param_name##_Y;               \
    pr->lower_bound = PATCH_##bound_name##_LOWER_BOUND;                        \
    pr->upper_bound = PATCH_##bound_name##_UPPER_BOUND;                        \
                                                                               \
    break;                                                                     \
  }

#define LAYOUT_SETUP_CART_PARAM_CASE_FLAG(header_name, param_name, bound_name, bound_flag)  \
  case LAYOUT_CART_PARAM_##header_name##_##param_name:                                      \
  {                                                                                         \
    pr->center_x = LAYOUT_CART_HEADER_##header_name##_X;                                    \
    pr->center_y = LAYOUT_CART_##header_name##_##param_name##_Y;                            \
    pr->lower_bound = PATCH_##bound_name##_CLEAR;                                           \
    pr->upper_bound = PATCH_##bound_name##_FLAG_##bound_flag;                               \
                                                                                            \
    break;                                                                                  \
  }

#define LAYOUT_SETUP_AUDITION_PARAM_CASE(param_name, bound_name)               \
  case LAYOUT_AUDITION_PARAM_##param_name:                                     \
  {                                                                            \
    pr->center_x = LAYOUT_AUDITION_##param_name##_X;                           \
    pr->center_y = LAYOUT_AUDITION_##param_name##_Y;                           \
    pr->lower_bound = MIDI_CONT_##bound_name##_LOWER_BOUND;                    \
    pr->upper_bound = MIDI_CONT_##bound_name##_UPPER_BOUND;                    \
                                                                               \
    break;                                                                     \
  }

button    G_layout_top_panel_buttons[LAYOUT_NUM_TOP_PANEL_BUTTONS];
header    G_layout_top_panel_headers[LAYOUT_NUM_TOP_PANEL_HEADERS];

button    G_layout_cart_buttons[LAYOUT_NUM_CART_BUTTONS];
text_box  G_layout_cart_text_boxes[LAYOUT_NUM_CART_TEXT_BOXES];
header    G_layout_cart_headers[LAYOUT_NUM_CART_HEADERS];
param     G_layout_cart_params[LAYOUT_NUM_CART_PARAMS];

param     G_layout_audition_params[LAYOUT_NUM_AUDITION_PARAMS];

/*******************************************************************************
** layout_setup_top_panel()
*******************************************************************************/
short int layout_setup_top_panel()
{
  int m;

  button* b;
  header* hd;

  /* top panel buttons */
  for (m = 0; m < LAYOUT_NUM_TOP_PANEL_BUTTONS; m++)
  {
    b = &G_layout_top_panel_buttons[m];

    switch (m)
    {
      LAYOUT_SETUP_BUTTON_CASE(TOP_PANEL, CART)
      LAYOUT_SETUP_BUTTON_CASE(TOP_PANEL, INSTRUMENTS)
      LAYOUT_SETUP_BUTTON_CASE(TOP_PANEL, SONG)
      LAYOUT_SETUP_BUTTON_CASE(TOP_PANEL, MIXER)
      LAYOUT_SETUP_BUTTON_CASE(TOP_PANEL, SOUND_FX)
      LAYOUT_SETUP_BUTTON_CASE(TOP_PANEL, DPCM)

      default:
      {
        b->center_x = 0;
        b->center_y = 0;
        b->width = 2;
        b->state = LAYOUT_BUTTON_STATE_OFF;

        break;
      }
    }

    if (m == LAYOUT_TOP_PANEL_BUTTON_CART)
      b->state = LAYOUT_BUTTON_STATE_ON;
  }

  /* top panel headers */
  for (m = 0; m < LAYOUT_NUM_TOP_PANEL_HEADERS; m++)
  {
    hd = &G_layout_top_panel_headers[m];

    switch (m)
    {
      LAYOUT_SETUP_HEADER_CASE(TOP_PANEL, NAME)
      LAYOUT_SETUP_HEADER_CASE(TOP_PANEL, VERSION)

      default:
      {
        hd->center_x = 0;
        hd->center_y = 0;

        break;
      }
    }
  }

  return 0;
}

/*******************************************************************************
** layout_setup_cart()
*******************************************************************************/
short int layout_setup_cart()
{
  int m;

  button* b;
  text_box* t;
  header* hd;
  param* pr;

  /* cart buttons */
  for (m = 0; m < LAYOUT_NUM_CART_BUTTONS; m++)
  {
    b = &G_layout_cart_buttons[m];

    switch (m)
    {
      LAYOUT_SETUP_BUTTON_CASE(CART, LOAD_CART)
      LAYOUT_SETUP_BUTTON_CASE(CART, SAVE_CART)
      LAYOUT_SETUP_BUTTON_CASE(CART, COPY_PATCH)
      LAYOUT_SETUP_BUTTON_CASE(CART, ZAP_PATCH)

      default:
      {
        b->center_x = 0;
        b->center_y = 0;
        b->width = 2;
        b->state = LAYOUT_BUTTON_STATE_OFF;

        break;
      }
    }
  }

  /* cart edit text boxes */
  for (m = 0; m < LAYOUT_NUM_CART_TEXT_BOXES; m++)
  {
    t = &G_layout_cart_text_boxes[m];

    switch (m)
    {
      LAYOUT_SETUP_TEXT_BOX_CASE(CART, CART_NAME)
      LAYOUT_SETUP_TEXT_BOX_CASE(CART, PATCH_NAME)

      default:
      {
        t->center_x = 0;
        t->center_y = 0;
        t->width = 2;
        t->state = LAYOUT_TEXT_BOX_STATE_NORMAL;

        break;
      }
    }
  }

  /* cart headers */
  for (m = 0; m < LAYOUT_NUM_CART_HEADERS; m++)
  {
    hd = &G_layout_cart_headers[m];

    switch (m)
    {
      LAYOUT_SETUP_HEADER_CASE(CART, OSC_1)
      LAYOUT_SETUP_HEADER_CASE(CART, OSC_2)
      LAYOUT_SETUP_HEADER_CASE(CART, OSC_3)
      LAYOUT_SETUP_HEADER_CASE(CART, ENV_1)
      LAYOUT_SETUP_HEADER_CASE(CART, ENV_2)
      LAYOUT_SETUP_HEADER_CASE(CART, ENV_3)
      LAYOUT_SETUP_HEADER_CASE(CART, VIBRATO)
      LAYOUT_SETUP_HEADER_CASE(CART, TREMOLO)
      LAYOUT_SETUP_HEADER_CASE(CART, CHORUS)
      LAYOUT_SETUP_HEADER_CASE(CART, BOOST)
      LAYOUT_SETUP_HEADER_CASE(CART, VELOCITY)
      LAYOUT_SETUP_HEADER_CASE(CART, FILTERS)
      LAYOUT_SETUP_HEADER_CASE(CART, PITCH_ENV)
      LAYOUT_SETUP_HEADER_CASE(CART, PITCH_WHEEL)
      LAYOUT_SETUP_HEADER_CASE(CART, ARPEGGIO)
      LAYOUT_SETUP_HEADER_CASE(CART, PORTAMENTO)
      LAYOUT_SETUP_HEADER_CASE(CART, MOD_WHEEL)
      LAYOUT_SETUP_HEADER_CASE(CART, AFTERTOUCH)
      LAYOUT_SETUP_HEADER_CASE(CART, EXP_PEDAL)

      default:
      {
        hd->center_x = 0;
        hd->center_y = 0;

        break;
      }
    }
  }

  /* cart parameters */
  for (m = 0; m < LAYOUT_NUM_CART_PARAMS; m++)
  {
    pr = &G_layout_cart_params[m];

    /* set type */
    switch (m)
    {
      case LAYOUT_CART_PARAM_ALGORITHM:
      case LAYOUT_CART_PARAM_OSC_1_WAVEFORM:
      case LAYOUT_CART_PARAM_OSC_2_WAVEFORM:
      case LAYOUT_CART_PARAM_OSC_3_WAVEFORM:
      case LAYOUT_CART_PARAM_ENV_1_HOLD_MODE:
      case LAYOUT_CART_PARAM_ENV_2_HOLD_MODE:
      case LAYOUT_CART_PARAM_ENV_3_HOLD_MODE:
      case LAYOUT_CART_PARAM_VIBRATO_WAVEFORM:
      case LAYOUT_CART_PARAM_TREMOLO_WAVEFORM:
      case LAYOUT_CART_PARAM_CHORUS_WAVEFORM:
      case LAYOUT_CART_PARAM_ARPEGGIO_PATTERN:
      case LAYOUT_CART_PARAM_PORTAMENTO_LEGATO:
      {
        pr->type = LAYOUT_PARAM_TYPE_ARROWS;
        break;
      }

      case LAYOUT_CART_PARAM_OSC_SYNC:
      case LAYOUT_CART_PARAM_OSC_1_FREQ_MODE:
      case LAYOUT_CART_PARAM_OSC_2_FREQ_MODE:
      case LAYOUT_CART_PARAM_OSC_3_FREQ_MODE:
      case LAYOUT_CART_PARAM_VIBRATO_SYNC:
      case LAYOUT_CART_PARAM_VIBRATO_POLARITY:
      case LAYOUT_CART_PARAM_TREMOLO_SYNC:
      case LAYOUT_CART_PARAM_CHORUS_SYNC:
      case LAYOUT_CART_PARAM_PITCH_WHEEL_MODE:
      case LAYOUT_CART_PARAM_ARPEGGIO_MODE:
      case LAYOUT_CART_PARAM_PORTAMENTO_MODE:
      {
        pr->type = LAYOUT_PARAM_TYPE_RADIO;
        break;
      }

      case LAYOUT_CART_PARAM_OSC_1_ROUTING_VIBRATO:
      case LAYOUT_CART_PARAM_OSC_1_ROUTING_PITCH_ENV:
      case LAYOUT_CART_PARAM_OSC_1_ROUTING_PITCH_WHEEL:
      case LAYOUT_CART_PARAM_OSC_2_ROUTING_VIBRATO:
      case LAYOUT_CART_PARAM_OSC_2_ROUTING_PITCH_ENV:
      case LAYOUT_CART_PARAM_OSC_2_ROUTING_PITCH_WHEEL:
      case LAYOUT_CART_PARAM_OSC_3_ROUTING_VIBRATO:
      case LAYOUT_CART_PARAM_OSC_3_ROUTING_PITCH_ENV:
      case LAYOUT_CART_PARAM_OSC_3_ROUTING_PITCH_WHEEL:
      case LAYOUT_CART_PARAM_ENV_1_ROUTING_TREMOLO:
      case LAYOUT_CART_PARAM_ENV_1_ROUTING_BOOST:
      case LAYOUT_CART_PARAM_ENV_1_ROUTING_VELOCITY:
      case LAYOUT_CART_PARAM_ENV_2_ROUTING_TREMOLO:
      case LAYOUT_CART_PARAM_ENV_2_ROUTING_BOOST:
      case LAYOUT_CART_PARAM_ENV_2_ROUTING_VELOCITY:
      case LAYOUT_CART_PARAM_ENV_3_ROUTING_TREMOLO:
      case LAYOUT_CART_PARAM_ENV_3_ROUTING_BOOST:
      case LAYOUT_CART_PARAM_ENV_3_ROUTING_VELOCITY:
      case LAYOUT_CART_PARAM_MOD_WHEEL_ROUTING_VIBRATO:
      case LAYOUT_CART_PARAM_MOD_WHEEL_ROUTING_TREMOLO:
      case LAYOUT_CART_PARAM_MOD_WHEEL_ROUTING_BOOST:
      case LAYOUT_CART_PARAM_MOD_WHEEL_ROUTING_CHORUS:
      case LAYOUT_CART_PARAM_AFTERTOUCH_ROUTING_VIBRATO:
      case LAYOUT_CART_PARAM_AFTERTOUCH_ROUTING_TREMOLO:
      case LAYOUT_CART_PARAM_AFTERTOUCH_ROUTING_BOOST:
      case LAYOUT_CART_PARAM_AFTERTOUCH_ROUTING_CHORUS:
      case LAYOUT_CART_PARAM_EXP_PEDAL_ROUTING_VIBRATO:
      case LAYOUT_CART_PARAM_EXP_PEDAL_ROUTING_TREMOLO:
      case LAYOUT_CART_PARAM_EXP_PEDAL_ROUTING_BOOST:
      case LAYOUT_CART_PARAM_EXP_PEDAL_ROUTING_CHORUS:
      {
        pr->type = LAYOUT_PARAM_TYPE_FLAG;
        break;
      }

      default:
      {
        pr->type = LAYOUT_PARAM_TYPE_SLIDER;
        break;
      }
    }

    /* set position and bounds */
    switch (m)
    {
      LAYOUT_SETUP_CART_PARAM_CASE_NO_HEADER(CART_NUMBER,   CART_NUMBER)
      LAYOUT_SETUP_CART_PARAM_CASE_NO_HEADER(PATCH_NUMBER,  PATCH_NUMBER)

      LAYOUT_SETUP_CART_PARAM_CASE_NO_HEADER(ALGORITHM, ALGORITHM)
      LAYOUT_SETUP_CART_PARAM_CASE_NO_HEADER(OSC_SYNC,  SYNC)

      LAYOUT_SETUP_CART_PARAM_CASE_INT(OSC_1, WAVEFORM,   OSC_WAVEFORM)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(OSC_1, PHI,        OSC_PHI)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(OSC_1, FREQ_MODE,  OSC_FREQ_MODE)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(OSC_1, MULTIPLE,   OSC_MULTIPLE)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(OSC_1, DIVISOR,    OSC_DIVISOR)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(OSC_1, OCTAVE,     OSC_OCTAVE)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(OSC_1, NOTE,       OSC_NOTE)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(OSC_1, DETUNE,     OSC_DETUNE)

      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(OSC_1,  ROUTING_VIBRATO,      OSC_ROUTING,  VIBRATO)
      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(OSC_1,  ROUTING_PITCH_ENV,    OSC_ROUTING,  PITCH_ENV)
      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(OSC_1,  ROUTING_PITCH_WHEEL,  OSC_ROUTING,  PITCH_WHEEL)

      LAYOUT_SETUP_CART_PARAM_CASE_INT(OSC_2, WAVEFORM,   OSC_WAVEFORM)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(OSC_2, PHI,        OSC_PHI)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(OSC_2, FREQ_MODE,  OSC_FREQ_MODE)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(OSC_2, MULTIPLE,   OSC_MULTIPLE)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(OSC_2, DIVISOR,    OSC_DIVISOR)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(OSC_2, OCTAVE,     OSC_OCTAVE)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(OSC_2, NOTE,       OSC_NOTE)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(OSC_2, DETUNE,     OSC_DETUNE)

      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(OSC_2,  ROUTING_VIBRATO,      OSC_ROUTING,  VIBRATO)
      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(OSC_2,  ROUTING_PITCH_ENV,    OSC_ROUTING,  PITCH_ENV)
      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(OSC_2,  ROUTING_PITCH_WHEEL,  OSC_ROUTING,  PITCH_WHEEL)

      LAYOUT_SETUP_CART_PARAM_CASE_INT(OSC_3, WAVEFORM,   OSC_WAVEFORM)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(OSC_3, PHI,        OSC_PHI)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(OSC_3, FREQ_MODE,  OSC_FREQ_MODE)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(OSC_3, MULTIPLE,   OSC_MULTIPLE)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(OSC_3, DIVISOR,    OSC_DIVISOR)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(OSC_3, OCTAVE,     OSC_OCTAVE)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(OSC_3, NOTE,       OSC_NOTE)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(OSC_3, DETUNE,     OSC_DETUNE)

      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(OSC_3,  ROUTING_VIBRATO,      OSC_ROUTING,  VIBRATO)
      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(OSC_3,  ROUTING_PITCH_ENV,    OSC_ROUTING,  PITCH_ENV)
      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(OSC_3,  ROUTING_PITCH_WHEEL,  OSC_ROUTING,  PITCH_WHEEL)

      LAYOUT_SETUP_CART_PARAM_CASE_INT(ENV_1, ATTACK,     ENV_TIME)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(ENV_1, DECAY,      ENV_TIME)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(ENV_1, SUSTAIN,    ENV_TIME)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(ENV_1, RELEASE,    ENV_TIME)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(ENV_1, AMPLITUDE,  ENV_LEVEL)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(ENV_1, HOLD_LEVEL, ENV_LEVEL)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(ENV_1, HOLD_MODE,  ENV_HOLD_MODE)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(ENV_1, RATE_KS,    ENV_KEYSCALING)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(ENV_1, LEVEL_KS,   ENV_KEYSCALING)

      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(ENV_1,  ROUTING_TREMOLO,  ENV_ROUTING,  TREMOLO)
      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(ENV_1,  ROUTING_BOOST,    ENV_ROUTING,  BOOST)
      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(ENV_1,  ROUTING_VELOCITY, ENV_ROUTING,  VELOCITY)

      LAYOUT_SETUP_CART_PARAM_CASE_INT(ENV_2, ATTACK,     ENV_TIME)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(ENV_2, DECAY,      ENV_TIME)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(ENV_2, SUSTAIN,    ENV_TIME)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(ENV_2, RELEASE,    ENV_TIME)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(ENV_2, AMPLITUDE,  ENV_LEVEL)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(ENV_2, HOLD_LEVEL, ENV_LEVEL)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(ENV_2, HOLD_MODE,  ENV_HOLD_MODE)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(ENV_2, RATE_KS,    ENV_KEYSCALING)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(ENV_2, LEVEL_KS,   ENV_KEYSCALING)

      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(ENV_2,  ROUTING_TREMOLO,  ENV_ROUTING,  TREMOLO)
      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(ENV_2,  ROUTING_BOOST,    ENV_ROUTING,  BOOST)
      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(ENV_2,  ROUTING_VELOCITY, ENV_ROUTING,  VELOCITY)

      LAYOUT_SETUP_CART_PARAM_CASE_INT(ENV_3, ATTACK,     ENV_TIME)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(ENV_3, DECAY,      ENV_TIME)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(ENV_3, SUSTAIN,    ENV_TIME)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(ENV_3, RELEASE,    ENV_TIME)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(ENV_3, AMPLITUDE,  ENV_LEVEL)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(ENV_3, HOLD_LEVEL, ENV_LEVEL)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(ENV_3, HOLD_MODE,  ENV_HOLD_MODE)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(ENV_3, RATE_KS,    ENV_KEYSCALING)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(ENV_3, LEVEL_KS,   ENV_KEYSCALING)

      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(ENV_3,  ROUTING_TREMOLO,  ENV_ROUTING,  TREMOLO)
      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(ENV_3,  ROUTING_BOOST,    ENV_ROUTING,  BOOST)
      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(ENV_3,  ROUTING_VELOCITY, ENV_ROUTING,  VELOCITY)

      LAYOUT_SETUP_CART_PARAM_CASE_INT(VIBRATO, WAVEFORM,     LFO_WAVEFORM)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(VIBRATO, DELAY,        LFO_DELAY)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(VIBRATO, SPEED,        LFO_SPEED)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(VIBRATO, DEPTH,        LFO_DEPTH)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(VIBRATO, SENSITIVITY,  SENSITIVITY)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(VIBRATO, SYNC,         SYNC)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(VIBRATO, POLARITY,     LFO_POLARITY)

      LAYOUT_SETUP_CART_PARAM_CASE_INT(TREMOLO, WAVEFORM,     LFO_WAVEFORM)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(TREMOLO, DELAY,        LFO_DELAY)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(TREMOLO, SPEED,        LFO_SPEED)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(TREMOLO, DEPTH,        LFO_DEPTH)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(TREMOLO, SENSITIVITY,  SENSITIVITY)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(TREMOLO, SYNC,         SYNC)

      LAYOUT_SETUP_CART_PARAM_CASE_INT(CHORUS,  WAVEFORM,     LFO_WAVEFORM)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(CHORUS,  DELAY,        LFO_DELAY)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(CHORUS,  SPEED,        LFO_SPEED)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(CHORUS,  DEPTH,        LFO_DEPTH)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(CHORUS,  SENSITIVITY,  SENSITIVITY)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(CHORUS,  SYNC,         SYNC)

      LAYOUT_SETUP_CART_PARAM_CASE_INT(BOOST,     SENSITIVITY,  SENSITIVITY)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(VELOCITY,  SENSITIVITY,  SENSITIVITY)

      LAYOUT_SETUP_CART_PARAM_CASE_INT(FILTERS, HIGHPASS, HIGHPASS_CUTOFF)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(FILTERS, LOWPASS,  LOWPASS_CUTOFF)

      LAYOUT_SETUP_CART_PARAM_CASE_INT(PITCH_ENV, ATTACK,   PEG_TIME)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(PITCH_ENV, DECAY,    PEG_TIME)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(PITCH_ENV, RELEASE,  PEG_TIME)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(PITCH_ENV, MAXIMUM,  PEG_LEVEL)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(PITCH_ENV, FINALE,   PEG_LEVEL)

      LAYOUT_SETUP_CART_PARAM_CASE_INT(PITCH_WHEEL, MODE,   PITCH_WHEEL_MODE)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(PITCH_WHEEL, RANGE,  PITCH_WHEEL_RANGE)

      LAYOUT_SETUP_CART_PARAM_CASE_INT(ARPEGGIO,  MODE,     ARPEGGIO_MODE)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(ARPEGGIO,  PATTERN,  ARPEGGIO_PATTERN)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(ARPEGGIO,  OCTAVES,  ARPEGGIO_OCTAVES)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(ARPEGGIO,  SPEED,    ARPEGGIO_SPEED)

      LAYOUT_SETUP_CART_PARAM_CASE_INT(PORTAMENTO,  MODE,   PORTAMENTO_MODE)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(PORTAMENTO,  LEGATO, PORTAMENTO_LEGATO)
      LAYOUT_SETUP_CART_PARAM_CASE_INT(PORTAMENTO,  SPEED,  PORTAMENTO_SPEED)

      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(MOD_WHEEL,  ROUTING_VIBRATO,  MIDI_CONT_ROUTING,  VIBRATO)
      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(MOD_WHEEL,  ROUTING_TREMOLO,  MIDI_CONT_ROUTING,  TREMOLO)
      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(MOD_WHEEL,  ROUTING_BOOST,    MIDI_CONT_ROUTING,  BOOST)
      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(MOD_WHEEL,  ROUTING_CHORUS,   MIDI_CONT_ROUTING,  CHORUS)

      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(AFTERTOUCH, ROUTING_VIBRATO,  MIDI_CONT_ROUTING,  VIBRATO)
      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(AFTERTOUCH, ROUTING_TREMOLO,  MIDI_CONT_ROUTING,  TREMOLO)
      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(AFTERTOUCH, ROUTING_BOOST,    MIDI_CONT_ROUTING,  BOOST)
      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(AFTERTOUCH, ROUTING_CHORUS,   MIDI_CONT_ROUTING,  CHORUS)

      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(EXP_PEDAL,  ROUTING_VIBRATO,  MIDI_CONT_ROUTING,  VIBRATO)
      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(EXP_PEDAL,  ROUTING_TREMOLO,  MIDI_CONT_ROUTING,  TREMOLO)
      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(EXP_PEDAL,  ROUTING_BOOST,    MIDI_CONT_ROUTING,  BOOST)
      LAYOUT_SETUP_CART_PARAM_CASE_FLAG(EXP_PEDAL,  ROUTING_CHORUS,   MIDI_CONT_ROUTING,  CHORUS)

      default:
      {
        pr->center_x = 0;
        pr->center_y = 0;
        pr->lower_bound = 0;
        pr->upper_bound = 0;

        break;
      }
    }
  }

  return 0;
}

/*******************************************************************************
** layout_setup_audition_panel()
*******************************************************************************/
short int layout_setup_audition_panel()
{
  int m;

  param* pr;

  /* audition panel parameters */
  for (m = 0; m < LAYOUT_NUM_AUDITION_PARAMS; m++)
  {
    pr = &G_layout_audition_params[m];

    /* set type */
    switch (m)
    {
      case LAYOUT_AUDITION_PARAM_OCTAVE:
      case LAYOUT_AUDITION_PARAM_VELOCITY:
      case LAYOUT_AUDITION_PARAM_PITCH_WHEEL:
      case LAYOUT_AUDITION_PARAM_MOD_WHEEL:
      case LAYOUT_AUDITION_PARAM_AFTERTOUCH:
      case LAYOUT_AUDITION_PARAM_EXP_PEDAL:
      {
        pr->type = LAYOUT_PARAM_TYPE_SLIDER;
        break;
      }

      default:
      {
        pr->type = LAYOUT_PARAM_TYPE_SLIDER;
        break;
      }
    }

    /* set position and bounds */
    switch (m)
    {
      LAYOUT_SETUP_AUDITION_PARAM_CASE(OCTAVE,      OCTAVE)
      LAYOUT_SETUP_AUDITION_PARAM_CASE(VELOCITY,    NOTE_VELOCITY)
      LAYOUT_SETUP_AUDITION_PARAM_CASE(PITCH_WHEEL, BI_WHEEL)
      LAYOUT_SETUP_AUDITION_PARAM_CASE(MOD_WHEEL,   UNI_WHEEL)
      LAYOUT_SETUP_AUDITION_PARAM_CASE(AFTERTOUCH,  UNI_WHEEL)
      LAYOUT_SETUP_AUDITION_PARAM_CASE(EXP_PEDAL,   UNI_WHEEL)

      default:
      {
        pr->center_x = 0;
        pr->center_y = 0;
        pr->lower_bound = 0;
        pr->upper_bound = 0;

        break;
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
  layout_setup_top_panel();

  layout_setup_cart();
  layout_setup_audition_panel();

  return 0;
}

/*******************************************************************************
** layout_reset_top_panel_button_states()
*******************************************************************************/
short int layout_reset_top_panel_button_states()
{
  int m;

  button* b;

  /* reset all button states */
  for (m = 0; m < LAYOUT_NUM_TOP_PANEL_BUTTONS; m++)
  {
    b = &G_layout_top_panel_buttons[m];

    b->state = LAYOUT_BUTTON_STATE_OFF;
  }

  return 0;
}

/*******************************************************************************
** layout_reset_cart_button_and_text_box_states()
*******************************************************************************/
short int layout_reset_cart_button_and_text_box_states()
{
  int m;

  button* b;
  text_box* t;

  /* reset all button states */
  for (m = 0; m < LAYOUT_NUM_CART_BUTTONS; m++)
  {
    b = &G_layout_cart_buttons[m];

    b->state = LAYOUT_BUTTON_STATE_OFF;
  }

  /* reset all text box states */
  for (m = 0; m < LAYOUT_NUM_CART_TEXT_BOXES; m++)
  {
    t = &G_layout_cart_text_boxes[m];

    t->state = LAYOUT_TEXT_BOX_STATE_NORMAL;
  }

  return 0;
}

