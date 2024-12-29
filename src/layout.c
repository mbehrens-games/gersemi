/*******************************************************************************
** layout.c (screen layout widget positions)
*******************************************************************************/

#include <stdio.h>  /* testing */
#include <stdlib.h>
#include <string.h>

#include "cart.h"
#include "layout.h"
#include "midicont.h"
#include "tuning.h"

#define LAYOUT_TOP_BAR_ADD_REGION(label, corner_x, corner_y, width, height)    \
  G_layout_top_bar_regions[LAYOUT_TOP_BAR_REGION_##label].x = corner_x;        \
  G_layout_top_bar_regions[LAYOUT_TOP_BAR_REGION_##label].y = corner_y;        \
  G_layout_top_bar_regions[LAYOUT_TOP_BAR_REGION_##label].w = width;           \
  G_layout_top_bar_regions[LAYOUT_TOP_BAR_REGION_##label].h = height;

#define LAYOUT_TOP_BAR_ADD_BUTTON(label, width, str)                           \
  G_layout_top_bar_buttons[LAYOUT_TOP_BAR_BUTTON_##label].x = next_x;          \
  G_layout_top_bar_buttons[LAYOUT_TOP_BAR_BUTTON_##label].y = 0;               \
  G_layout_top_bar_buttons[LAYOUT_TOP_BAR_BUTTON_##label].w = width;           \
  G_layout_top_bar_buttons[LAYOUT_TOP_BAR_BUTTON_##label].h = 2;               \
                                                                               \
  strcpy(G_layout_top_bar_buttons[LAYOUT_TOP_BAR_BUTTON_##label].name, str);   \
                                                                               \
  next_x += 8 * width;

#define LAYOUT_CART_ADD_REGION(label, corner_x, corner_y, width, height)       \
  G_layout_cart_regions[LAYOUT_CART_REGION_##label].x = corner_x;              \
  G_layout_cart_regions[LAYOUT_CART_REGION_##label].y = corner_y;              \
  G_layout_cart_regions[LAYOUT_CART_REGION_##label].w = width;                 \
  G_layout_cart_regions[LAYOUT_CART_REGION_##label].h = height;

#define LAYOUT_CART_ADD_HEADER_TO_COLUMN(col, label, str)                      \
  next_y[col] += 4;                                                            \
                                                                               \
  G_layout_cart_headers[LAYOUT_CART_HEADER_##label].x = columns_x[col];        \
  G_layout_cart_headers[LAYOUT_CART_HEADER_##label].y = next_y[col];           \
  G_layout_cart_headers[LAYOUT_CART_HEADER_##label].w = 11;                    \
  G_layout_cart_headers[LAYOUT_CART_HEADER_##label].h = 1;                     \
                                                                               \
  strcpy(G_layout_cart_headers[LAYOUT_CART_HEADER_##label].name, str);         \
                                                                               \
  next_y[col] += 1 * 8 + 4;

#define LAYOUT_CART_ADD_PARAM_TO_COLUMN(col, label, str, type)                        \
  G_layout_cart_param_names[PATCH_PARAM_##label].x = columns_x[col];                  \
  G_layout_cart_param_names[PATCH_PARAM_##label].y = next_y[col];                     \
  G_layout_cart_param_names[PATCH_PARAM_##label].w = 3;                               \
  G_layout_cart_param_names[PATCH_PARAM_##label].h = 1;                               \
                                                                                      \
  strcpy(G_layout_cart_param_names[PATCH_PARAM_##label].name, str);                   \
                                                                                      \
  G_layout_cart_param_widgets[PATCH_PARAM_##label].x = columns_x[col] + 3 * 8;        \
  G_layout_cart_param_widgets[PATCH_PARAM_##label].y = next_y[col];                   \
  G_layout_cart_param_widgets[PATCH_PARAM_##label].w = 8;                             \
  G_layout_cart_param_widgets[PATCH_PARAM_##label].h = 1;                             \
  G_layout_cart_param_widgets[PATCH_PARAM_##label].kind = LAYOUT_WIDGET_KIND_##type;  \
                                                                                      \
  next_y[col] += 1 * 8 + 4;

layout_region G_layout_top_bar_regions[LAYOUT_NUM_TOP_BAR_REGIONS];
layout_button G_layout_top_bar_buttons[LAYOUT_NUM_TOP_BAR_BUTTONS];

layout_region G_layout_cart_regions[LAYOUT_NUM_CART_REGIONS];
layout_text   G_layout_cart_headers[LAYOUT_NUM_CART_HEADERS];
layout_text   G_layout_cart_param_names[PATCH_NUM_PARAMS];
layout_widget G_layout_cart_param_widgets[PATCH_NUM_PARAMS];

/*******************************************************************************
** layout_generate_top_bar()
*******************************************************************************/
short int layout_generate_top_bar()
{
  int m;
  int n;

  short int next_x;

  /* define screen regions */
  LAYOUT_TOP_BAR_ADD_REGION(TOP_BAR, 0, 0, 50, 2)

  /* initialize strings */
  for (m = 0; m < LAYOUT_NUM_TOP_BAR_BUTTONS; m++)
  {
    for (n = 0; n < LAYOUT_BUTTON_MAX_NAME_SIZE; n++)
      G_layout_top_bar_buttons[m].name[n] = '\0';
  }

  /* initialize x coordinate */
  next_x = 0;

  /* top bar buttons */
  LAYOUT_TOP_BAR_ADD_BUTTON(CART,         7, "Cart")
  LAYOUT_TOP_BAR_ADD_BUTTON(INSTRUMENTS, 13, "Instruments")
  LAYOUT_TOP_BAR_ADD_BUTTON(SONG,         7, "Song")
  LAYOUT_TOP_BAR_ADD_BUTTON(MIXER,        7, "Mixer")
  LAYOUT_TOP_BAR_ADD_BUTTON(SOUND_FX,    10, "Sound FX")
  LAYOUT_TOP_BAR_ADD_BUTTON(DPCM,         6, "DPCM")

  return 0;
}

/*******************************************************************************
** layout_generate_cart_screen()
*******************************************************************************/
short int layout_generate_cart_screen()
{
  int m;
  int n;

  short int columns_x[4];
  short int next_y[4];

  /* define screen regions */
  LAYOUT_CART_ADD_REGION(MAIN_AREA,       0,  16, 49, 22)
  LAYOUT_CART_ADD_REGION(SCROLLBAR,     392,  16,  1, 25)
  LAYOUT_CART_ADD_REGION(AUDITION_PANEL,  0, 192, 49,  4)

  /* initialize strings */
  for (m = 0; m < LAYOUT_NUM_CART_HEADERS; m++)
  {
    for (n = 0; n < LAYOUT_TEXT_MAX_NAME_SIZE; n++)
      G_layout_cart_headers[m].name[n] = '\0';
  }

  for (m = 0; m < PATCH_NUM_PARAMS; m++)
  {
    for (n = 0; n < LAYOUT_TEXT_MAX_NAME_SIZE; n++)
      G_layout_cart_param_names[m].name[n] = '\0';
  }

  /* initialize coordinates */
  columns_x[0] = 1 * 8 + 0;
  columns_x[1] = columns_x[0] + 12 * 8;
  columns_x[2] = columns_x[1] + 12 * 8;
  columns_x[3] = columns_x[2] + 12 * 8;

  next_y[0] = 7 * 8 + 4;
  next_y[1] = next_y[0];
  next_y[2] = next_y[0];
  next_y[3] = next_y[0];

  /* cart headers & params */
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, ALGORITHM,       "Alg", ARROWS)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, OSC_SYNC,        "Syn", RADIO)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, LEGACY_KEYSCALE, "Leg", RADIO)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, NOISE_ENABLE,    "Noi", RADIO)

  LAYOUT_CART_ADD_HEADER_TO_COLUMN(0, OSC_1, "Osc 1")
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, OSC_1_WAVEFORM,        "Wav", ARROWS)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, OSC_1_FEEDBACK,        "Fbk", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, OSC_1_PHI,             "Phi", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, OSC_1_FREQ_MODE,       "Frq", RADIO)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, OSC_1_MULTIPLE,        "Mul", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, OSC_1_DIVISOR,         "Div", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, OSC_1_DETUNE,          "Det", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, VIBRATO_ROUTING_OSC_1, "Vib", RADIO)

  LAYOUT_CART_ADD_HEADER_TO_COLUMN(1, OSC_2, "Osc 2")
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, OSC_2_WAVEFORM,        "Wav", ARROWS)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, OSC_2_FEEDBACK,        "Fbk", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, OSC_2_PHI,             "Phi", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, OSC_2_FREQ_MODE,       "Frq", RADIO)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, OSC_2_MULTIPLE,        "Mul", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, OSC_2_DIVISOR,         "Div", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, OSC_2_DETUNE,          "Det", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, VIBRATO_ROUTING_OSC_2, "Vib", RADIO)

  LAYOUT_CART_ADD_HEADER_TO_COLUMN(2, OSC_3, "Osc 3")
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, OSC_3_WAVEFORM,        "Wav", ARROWS)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, OSC_3_FEEDBACK,        "Fbk", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, OSC_3_PHI,             "Phi", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, OSC_3_FREQ_MODE,       "Frq", RADIO)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, OSC_3_MULTIPLE,        "Mul", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, OSC_3_DIVISOR,         "Div", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, OSC_3_DETUNE,          "Det", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, VIBRATO_ROUTING_OSC_3, "Vib", RADIO)

  LAYOUT_CART_ADD_HEADER_TO_COLUMN(3, OSC_4, "Osc 4")
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, OSC_4_WAVEFORM,        "Wav", ARROWS)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, OSC_4_FEEDBACK,        "Fbk", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, OSC_4_PHI,             "Phi", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, OSC_4_FREQ_MODE,       "Frq", RADIO)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, OSC_4_MULTIPLE,        "Mul", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, OSC_4_DIVISOR,         "Div", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, OSC_4_DETUNE,          "Det", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, VIBRATO_ROUTING_OSC_4, "Vib", RADIO)

  LAYOUT_CART_ADD_HEADER_TO_COLUMN(0, ENV_1, "Env 1")
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, ENV_1_ATTACK,            "Att", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, ENV_1_DECAY,             "Dec", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, ENV_1_SUSTAIN,           "Sus", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, ENV_1_RELEASE,           "Rel", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, ENV_1_MAX_LEVEL,         "Lev", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, ENV_1_HOLD_MODE,         "Mde", ARROWS)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, ENV_1_HOLD_LEVEL,        "Hld", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, ENV_1_RATE_KEYSCALING,   "RtS", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, ENV_1_LEVEL_KEYSCALING,  "LvS", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, TREMOLO_ROUTING_ENV_1,   "Trm", RADIO)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, BOOST_ROUTING_ENV_1,     "Bst", RADIO)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, VELOCITY_ROUTING_ENV_1,  "Vel", RADIO)

  LAYOUT_CART_ADD_HEADER_TO_COLUMN(1, ENV_2, "Env 2")
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, ENV_2_ATTACK,            "Att", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, ENV_2_DECAY,             "Dec", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, ENV_2_SUSTAIN,           "Sus", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, ENV_2_RELEASE,           "Rel", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, ENV_2_MAX_LEVEL,         "Lev", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, ENV_2_HOLD_MODE,         "Mde", ARROWS)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, ENV_2_HOLD_LEVEL,        "Hld", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, ENV_2_RATE_KEYSCALING,   "RtS", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, ENV_2_LEVEL_KEYSCALING,  "LvS", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, TREMOLO_ROUTING_ENV_2,   "Trm", RADIO)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, BOOST_ROUTING_ENV_2,     "Bst", RADIO)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, VELOCITY_ROUTING_ENV_2,  "Vel", RADIO)

  LAYOUT_CART_ADD_HEADER_TO_COLUMN(2, ENV_3, "Env 3")
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, ENV_3_ATTACK,            "Att", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, ENV_3_DECAY,             "Dec", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, ENV_3_SUSTAIN,           "Sus", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, ENV_3_RELEASE,           "Rel", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, ENV_3_MAX_LEVEL,         "Lev", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, ENV_3_HOLD_MODE,         "Mde", ARROWS)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, ENV_3_HOLD_LEVEL,        "Hld", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, ENV_3_RATE_KEYSCALING,   "RtS", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, ENV_3_LEVEL_KEYSCALING,  "LvS", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, TREMOLO_ROUTING_ENV_3,   "Trm", RADIO)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, BOOST_ROUTING_ENV_3,     "Bst", RADIO)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, VELOCITY_ROUTING_ENV_3,  "Vel", RADIO)

  LAYOUT_CART_ADD_HEADER_TO_COLUMN(3, ENV_4, "Env 4")
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, ENV_4_ATTACK,            "Att", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, ENV_4_DECAY,             "Dec", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, ENV_4_SUSTAIN,           "Sus", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, ENV_4_RELEASE,           "Rel", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, ENV_4_MAX_LEVEL,         "Lev", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, ENV_4_HOLD_MODE,         "Mde", ARROWS)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, ENV_4_HOLD_LEVEL,        "Hld", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, ENV_4_RATE_KEYSCALING,   "RtS", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, ENV_4_LEVEL_KEYSCALING,  "LvS", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, TREMOLO_ROUTING_ENV_4,   "Trm", RADIO)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, BOOST_ROUTING_ENV_4,     "Bst", RADIO)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, VELOCITY_ROUTING_ENV_4,  "Vel", RADIO)

  LAYOUT_CART_ADD_HEADER_TO_COLUMN(0, VIBRATO, "LFO Vib")
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, VIBRATO_WAVEFORM,    "Wav", ARROWS)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, VIBRATO_DELAY,       "Dly", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, VIBRATO_SPEED,       "Spd", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, VIBRATO_SENSITIVITY, "Sns", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, VIBRATO_DEPTH,       "Dep", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, VIBRATO_SYNC,        "Syn", RADIO)

  LAYOUT_CART_ADD_HEADER_TO_COLUMN(0, BOOST, "Boost")
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, BOOST_SENSITIVITY, "Sns", SLIDER)

  LAYOUT_CART_ADD_HEADER_TO_COLUMN(0, MOD_WHEEL, "Mod Wheel")
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, MOD_WHEEL_ROUTING_VIBRATO, "Vib", RADIO)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, MOD_WHEEL_ROUTING_TREMOLO, "Trm", RADIO)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(0, MOD_WHEEL_ROUTING_BOOST,   "Bst", RADIO)

  LAYOUT_CART_ADD_HEADER_TO_COLUMN(1, TREMOLO, "LFO Trem")
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, TREMOLO_WAVEFORM,    "Wav", ARROWS)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, TREMOLO_DELAY,       "Dly", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, TREMOLO_SPEED,       "Spd", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, TREMOLO_SENSITIVITY, "Sns", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, TREMOLO_DEPTH,       "Dep", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, TREMOLO_SYNC,        "Syn", RADIO)

  LAYOUT_CART_ADD_HEADER_TO_COLUMN(1, VELOCITY, "Velocity")
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, VELOCITY_SENSITIVITY, "Sns", SLIDER)

  LAYOUT_CART_ADD_HEADER_TO_COLUMN(1, AFTERTOUCH, "Aftertouch")
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, AFTERTOUCH_ROUTING_VIBRATO,  "Vib", RADIO)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, AFTERTOUCH_ROUTING_TREMOLO,  "Trm", RADIO)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(1, AFTERTOUCH_ROUTING_BOOST,    "Bst", RADIO)

  LAYOUT_CART_ADD_HEADER_TO_COLUMN(2, PITCH_ENV, "Pitch Env")
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, PITCH_ENV_ATTACK,  "Att", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, PITCH_ENV_DECAY,   "Dec", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, PITCH_ENV_RELEASE, "Rel", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, PITCH_ENV_MAX,     "Max", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, PITCH_ENV_FINALE,  "Fin", SLIDER)

  LAYOUT_CART_ADD_HEADER_TO_COLUMN(2, PITCH_WHEEL, "Pitch Wheel")
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, PITCH_WHEEL_MODE,  "Mde", RADIO)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, PITCH_WHEEL_RANGE, "Eng", SLIDER)

  LAYOUT_CART_ADD_HEADER_TO_COLUMN(2, EXP_PEDAL, "Exp Pedal")
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, EXP_PEDAL_ROUTING_VIBRATO, "Vib", RADIO)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, EXP_PEDAL_ROUTING_TREMOLO, "Trm", RADIO)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(2, EXP_PEDAL_ROUTING_BOOST,   "Bst", RADIO)

  LAYOUT_CART_ADD_HEADER_TO_COLUMN(3, ARPEGGIO, "Arpeggio")
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, ARPEGGIO_MODE,     "Mde", RADIO)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, ARPEGGIO_PATTERN,  "Pat", ARROWS)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, ARPEGGIO_OCTAVE,   "Oct", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, ARPEGGIO_SPEED,    "Spd", SLIDER)

  LAYOUT_CART_ADD_HEADER_TO_COLUMN(3, PORTAMENTO, "Portamento")
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, PORTAMENTO_MODE,   "Mde", RADIO)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, PORTAMENTO_LEGATO, "Leg", ARROWS)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, PORTAMENTO_SPEED,  "Spd", SLIDER)

  LAYOUT_CART_ADD_HEADER_TO_COLUMN(3, FILTERS, "Filters")
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, HIGHPASS_CUTOFF, "HPF", SLIDER)
  LAYOUT_CART_ADD_PARAM_TO_COLUMN(3, LOWPASS_CUTOFF,  "LPF", SLIDER)

  return 0;
}

/*******************************************************************************
** layout_generate_tables()
*******************************************************************************/
short int layout_generate_tables()
{
  layout_generate_top_bar();
  layout_generate_cart_screen();

  return 0;
}

/*******************************************************************************
** layout_reset_top_bar_states()
*******************************************************************************/
short int layout_reset_top_bar_states()
{

  return 0;
}

/*******************************************************************************
** layout_reset_cart_states()
*******************************************************************************/
short int layout_reset_cart_states()
{

  return 0;
}

