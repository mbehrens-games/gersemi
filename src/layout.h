/*******************************************************************************
** layout.h (screen layout widget positions)
*******************************************************************************/

#ifndef LAYOUT_H
#define LAYOUT_H

enum
{
  LAYOUT_PANEL_TYPE_NORMAL = 0,
  LAYOUT_PANEL_TYPE_THIN
};

enum
{
  LAYOUT_BUTTON_LABEL_PATCHES = 0, 
  LAYOUT_BUTTON_LABEL_PATTERNS 
};

enum
{
  LAYOUT_BUTTON_STATE_OFF = 0, 
  LAYOUT_BUTTON_STATE_ON 
};

enum
{
  LAYOUT_HEADER_TOP_PANEL_LABEL_NAME = 0,
  LAYOUT_HEADER_TOP_PANEL_LABEL_VERSION, 
  LAYOUT_HEADER_TOP_PANEL_NUM_LABELS
};

enum
{
  LAYOUT_HEADER_BOTTOM_PANEL_LABEL_OCTAVE = 0, 
  LAYOUT_HEADER_BOTTOM_PANEL_LABEL_KEY, 
  LAYOUT_HEADER_BOTTOM_PANEL_NUM_LABELS
};

enum
{
  LAYOUT_HEADER_PATCH_EDIT_LABEL_OSC_1 = 0, 
  LAYOUT_HEADER_PATCH_EDIT_LABEL_OSC_2, 
  LAYOUT_HEADER_PATCH_EDIT_LABEL_OSC_3, 
  LAYOUT_HEADER_PATCH_EDIT_LABEL_OSC_4, 
  LAYOUT_HEADER_PATCH_EDIT_LABEL_ENV_1, 
  LAYOUT_HEADER_PATCH_EDIT_LABEL_ENV_2, 
  LAYOUT_HEADER_PATCH_EDIT_LABEL_ENV_3, 
  LAYOUT_HEADER_PATCH_EDIT_LABEL_ENV_4, 
  LAYOUT_HEADER_PATCH_EDIT_LABEL_LFO, 
  LAYOUT_HEADER_PATCH_EDIT_LABEL_PORTAMENTO, 
  LAYOUT_HEADER_PATCH_EDIT_LABEL_FILTERS, 
  LAYOUT_HEADER_PATCH_EDIT_LABEL_NOISE, 
  LAYOUT_HEADER_PATCH_EDIT_LABEL_DEPTHS, 
  LAYOUT_HEADER_PATCH_EDIT_LABEL_MOD_WHEEL, 
  LAYOUT_HEADER_PATCH_EDIT_LABEL_AFTERTOUCH, 
  LAYOUT_HEADER_PATCH_EDIT_NUM_LABELS
};

enum
{
  LAYOUT_PARAM_PATCH_EDIT_LABEL_ALGORITHM = 0, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_WAVEFORM, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_FEEDBACK, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_SYNC, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_FREQ_MODE, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_MULTIPLE, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_DIVISOR, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_DETUNE, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_ATTACK, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_DECAY_1, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_DECAY_2, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_RELEASE, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_AMPLITUDE, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_SUSTAIN, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_RATE_KS, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_LEVEL_KS, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_VIBRATO_ENABLE, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_TREMOLO_ENABLE, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_BOOST_ENABLE, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_LFO_WAVEFORM, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_LFO_FREQUENCY, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_LFO_SYNC, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_LFO_DELAY, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_LFO_BASE_VIBRATO, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_LFO_BASE_TREMOLO, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_PORTAMENTO_MODE, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_PORTAMENTO_SPEED, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_HIGHPASS_CUTOFF, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_LOWPASS_CUTOFF, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_NOISE_MIX, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_NOISE_FREQUENCY, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_VIBRATO_DEPTH, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_TREMOLO_DEPTH, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_BOOST_DEPTH, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_MOD_WHEEL_VIBRATO, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_MOD_WHEEL_TREMOLO, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_MOD_WHEEL_BOOST, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_AFTERTOUCH_VIBRATO, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_AFTERTOUCH_TREMOLO, 
  LAYOUT_PARAM_PATCH_EDIT_LABEL_AFTERTOUCH_BOOST, 
  LAYOUT_PARAM_PATCH_EDIT_NUM_LABELS
};

enum
{
  LAYOUT_PARAM_PATCH_EDIT_ADJUST_TYPE_SLIDER = 0, 
  LAYOUT_PARAM_PATCH_EDIT_ADJUST_TYPE_ARROWS
};

#define LAYOUT_PARAM_PATCH_EDIT_FIRST_OSC_ENV_LABEL LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_WAVEFORM
#define LAYOUT_PARAM_PATCH_EDIT_LAST_OSC_ENV_LABEL  LAYOUT_PARAM_PATCH_EDIT_LABEL_BOOST_ENABLE

#define LAYOUT_PARAM_PATCH_EDIT_NUM_OSC_ENV_LABELS  (LAYOUT_PARAM_PATCH_EDIT_LAST_OSC_ENV_LABEL - LAYOUT_PARAM_PATCH_EDIT_FIRST_OSC_ENV_LABEL + 1)
#define LAYOUT_PARAM_PATCH_EDIT_NUM_OTHER_LABELS    (LAYOUT_PARAM_PATCH_EDIT_NUM_LABELS - LAYOUT_PARAM_PATCH_EDIT_NUM_OSC_ENV_LABELS)

#define LAYOUT_PARAM_PATCH_EDIT_FIRST_OSC_ENV_INDEX LAYOUT_PARAM_PATCH_EDIT_FIRST_OSC_ENV_LABEL
#define LAYOUT_PARAM_PATCH_EDIT_LAST_OSC_ENV_INDEX  (LAYOUT_PARAM_PATCH_EDIT_FIRST_OSC_ENV_LABEL + 4 * LAYOUT_PARAM_PATCH_EDIT_NUM_OSC_ENV_LABELS)

/* buttons */
#define LAYOUT_NUM_TOP_PANEL_BUTTONS  2
#define LAYOUT_NUM_PATCHES_BUTTONS    0

#define LAYOUT_NUM_BUTTONS    ( LAYOUT_NUM_TOP_PANEL_BUTTONS +                 \
                                LAYOUT_NUM_PATCHES_BUTTONS)

#define LAYOUT_TOP_PANEL_BUTTONS_START_INDEX   0
#define LAYOUT_TOP_PANEL_BUTTONS_END_INDEX     (LAYOUT_TOP_PANEL_BUTTONS_START_INDEX + \
                                                LAYOUT_NUM_TOP_PANEL_BUTTONS)

#define LAYOUT_PATCHES_BUTTONS_START_INDEX   LAYOUT_TOP_PANEL_BUTTONS_END_INDEX
#define LAYOUT_PATCHES_BUTTONS_END_INDEX     (LAYOUT_PATCHES_BUTTONS_START_INDEX + \
                                              LAYOUT_NUM_PATCHES_BUTTONS)

#define LAYOUT_BUTTON_INDEX_IS_VALID(index)                                    \
  ((index >= 0) && (index < LAYOUT_NUM_BUTTONS))

#define LAYOUT_BUTTON_INDEX_IS_NOT_VALID(index)                                \
  (!(LAYOUT_BUTTON_INDEX_IS_VALID(index)))

/* headers */
#define LAYOUT_NUM_TOP_PANEL_HEADERS    LAYOUT_HEADER_TOP_PANEL_NUM_LABELS
#define LAYOUT_NUM_BOTTOM_PANEL_HEADERS LAYOUT_HEADER_BOTTOM_PANEL_NUM_LABELS
#define LAYOUT_NUM_PATCH_EDIT_HEADERS   LAYOUT_HEADER_PATCH_EDIT_NUM_LABELS

#define LAYOUT_NUM_HEADERS    ( LAYOUT_NUM_TOP_PANEL_HEADERS +                 \
                                LAYOUT_NUM_BOTTOM_PANEL_HEADERS +              \
                                LAYOUT_NUM_PATCH_EDIT_HEADERS)

#define LAYOUT_TOP_PANEL_HEADERS_START_INDEX   0
#define LAYOUT_TOP_PANEL_HEADERS_END_INDEX     (LAYOUT_TOP_PANEL_HEADERS_START_INDEX + \
                                                LAYOUT_NUM_TOP_PANEL_HEADERS)

#define LAYOUT_BOTTOM_PANEL_HEADERS_START_INDEX   LAYOUT_TOP_PANEL_HEADERS_END_INDEX
#define LAYOUT_BOTTOM_PANEL_HEADERS_END_INDEX     ( LAYOUT_BOTTOM_PANEL_HEADERS_START_INDEX + \
                                                    LAYOUT_NUM_BOTTOM_PANEL_HEADERS)

#define LAYOUT_PATCH_EDIT_HEADERS_START_INDEX   LAYOUT_BOTTOM_PANEL_HEADERS_END_INDEX
#define LAYOUT_PATCH_EDIT_HEADERS_END_INDEX     ( LAYOUT_PATCH_EDIT_HEADERS_START_INDEX + \
                                                  LAYOUT_NUM_PATCH_EDIT_HEADERS)

#define LAYOUT_HEADER_INDEX_IS_VALID(index)                                    \
  ((index >= 0) && (index < LAYOUT_NUM_HEADERS))

#define LAYOUT_HEADER_INDEX_IS_NOT_VALID(index)                                \
  (!(LAYOUT_HEADER_INDEX_IS_VALID(index)))

/* params */
#define LAYOUT_NUM_BOTTOM_PANEL_PARAMS 0
#define LAYOUT_NUM_PATCH_EDIT_PARAMS   (LAYOUT_PARAM_PATCH_EDIT_NUM_OTHER_LABELS + (4 * LAYOUT_PARAM_PATCH_EDIT_NUM_OSC_ENV_LABELS))

#define LAYOUT_NUM_PARAMS    (LAYOUT_NUM_BOTTOM_PANEL_PARAMS +                 \
                              LAYOUT_NUM_PATCH_EDIT_PARAMS)

#define LAYOUT_BOTTOM_PANEL_PARAMS_START_INDEX  0
#define LAYOUT_BOTTOM_PANEL_PARAMS_END_INDEX    ( LAYOUT_BOTTOM_PANEL_PARAMS_START_INDEX + \
                                                  LAYOUT_NUM_BOTTOM_PANEL_PARAMS)

#define LAYOUT_PATCH_EDIT_PARAMS_START_INDEX  LAYOUT_BOTTOM_PANEL_PARAMS_END_INDEX
#define LAYOUT_PATCH_EDIT_PARAMS_END_INDEX    ( LAYOUT_PATCH_EDIT_PARAMS_START_INDEX + \
                                                LAYOUT_NUM_PATCH_EDIT_PARAMS)

#define LAYOUT_PARAM_INDEX_IS_VALID(index)                                     \
  ((index >= 0) && (index < LAYOUT_NUM_PARAMS))

#define LAYOUT_PARAM_INDEX_IS_NOT_VALID(index)                                 \
  (!(LAYOUT_PARAM_INDEX_IS_VALID(index)))

/* panel and scrollbar positions & sizes */
#define LAYOUT_PANEL_TOP_X           0
#define LAYOUT_PANEL_TOP_Y         -22
#define LAYOUT_PANEL_TOP_WIDTH      50
#define LAYOUT_PANEL_TOP_HEIGHT      6

#define LAYOUT_PANEL_BOTTOM_X        0
#define LAYOUT_PANEL_BOTTOM_Y       25
#define LAYOUT_PANEL_BOTTOM_WIDTH   50
#define LAYOUT_PANEL_BOTTOM_HEIGHT   3

#define LAYOUT_SCROLLBAR_X          49
#define LAYOUT_SCROLLBAR_Y           3
#define LAYOUT_SCROLLBAR_HEIGHT     19

/* button positions & sizes (top & bottom panels) */
#define LAYOUT_TOP_PANEL_BUTTON_PATCHES_X       -40
#define LAYOUT_TOP_PANEL_BUTTON_PATCHES_Y       -20
#define LAYOUT_TOP_PANEL_BUTTON_PATCHES_WIDTH     8

#define LAYOUT_TOP_PANEL_BUTTON_PATTERNS_X      -22
#define LAYOUT_TOP_PANEL_BUTTON_PATTERNS_Y      -20
#define LAYOUT_TOP_PANEL_BUTTON_PATTERNS_WIDTH   10

/* header positions (top & bottom panels) */
#define LAYOUT_TOP_PANEL_HEADER_NAME_X       -5
#define LAYOUT_TOP_PANEL_HEADER_NAME_Y      -24

#define LAYOUT_TOP_PANEL_HEADER_VERSION_X     9
#define LAYOUT_TOP_PANEL_HEADER_VERSION_Y   -24

#define LAYOUT_BOTTOM_PANEL_HEADER_OCTAVE_X -42
#define LAYOUT_BOTTOM_PANEL_HEADER_OCTAVE_Y  25

#define LAYOUT_BOTTOM_PANEL_HEADER_KEY_X      2
#define LAYOUT_BOTTOM_PANEL_HEADER_KEY_Y     25

/* screen regions (divided vertically) */
#define LAYOUT_SCREEN_AREA_TOP_PANEL_LOWER_BOUND_CELLS     0
#define LAYOUT_SCREEN_AREA_TOP_PANEL_UPPER_BOUND_CELLS     LAYOUT_PANEL_TOP_HEIGHT

#define LAYOUT_SCREEN_AREA_BOTTOM_PANEL_LOWER_BOUND_CELLS  (28 - LAYOUT_PANEL_BOTTOM_HEIGHT)
#define LAYOUT_SCREEN_AREA_BOTTOM_PANEL_UPPER_BOUND_CELLS  28

#define LAYOUT_SCREEN_AREA_MAIN_LOWER_BOUND_CELLS  LAYOUT_PANEL_TOP_HEIGHT
#define LAYOUT_SCREEN_AREA_MAIN_UPPER_BOUND_CELLS  (28 - LAYOUT_PANEL_BOTTOM_HEIGHT)

#define LAYOUT_SCREEN_AREA_TOP_PANEL_LOWER_BOUND_PIXELS     (8 * LAYOUT_SCREEN_AREA_TOP_PANEL_LOWER_BOUND_CELLS)
#define LAYOUT_SCREEN_AREA_TOP_PANEL_UPPER_BOUND_PIXELS     (8 * LAYOUT_SCREEN_AREA_TOP_PANEL_UPPER_BOUND_CELLS)

#define LAYOUT_SCREEN_AREA_BOTTOM_PANEL_LOWER_BOUND_PIXELS  (8 * LAYOUT_SCREEN_AREA_BOTTOM_PANEL_LOWER_BOUND_CELLS)
#define LAYOUT_SCREEN_AREA_BOTTOM_PANEL_UPPER_BOUND_PIXELS  (8 * LAYOUT_SCREEN_AREA_BOTTOM_PANEL_UPPER_BOUND_CELLS)

#define LAYOUT_SCREEN_AREA_MAIN_LOWER_BOUND_PIXELS  (8 * LAYOUT_SCREEN_AREA_MAIN_LOWER_BOUND_CELLS)
#define LAYOUT_SCREEN_AREA_MAIN_UPPER_BOUND_PIXELS  (8 * LAYOUT_SCREEN_AREA_MAIN_UPPER_BOUND_CELLS)

#define LAYOUT_PATCH_HEADER_IS_IN_MAIN_AREA(hd)                                                                                                     \
  ( ((GRAPHICS_OVERSCAN_HEIGHT - 8 * 1) / 2 + (4 * (hd->center_y - G_current_scroll_amount)) >= LAYOUT_SCREEN_AREA_MAIN_LOWER_BOUND_PIXELS + 4) &&  \
    ((GRAPHICS_OVERSCAN_HEIGHT + 8 * 1) / 2 + (4 * (hd->center_y - G_current_scroll_amount)) <= LAYOUT_SCREEN_AREA_MAIN_UPPER_BOUND_PIXELS - 4))

#define LAYOUT_PATCH_HEADER_IS_NOT_IN_MAIN_AREA(hd)                             \
  (!(LAYOUT_PATCH_HEADER_IS_IN_MAIN_AREA(hd)))

#define LAYOUT_PATCH_PARAM_IS_IN_MAIN_AREA(pr)                                                                                                      \
  ( ((GRAPHICS_OVERSCAN_HEIGHT - 8 * 1) / 2 + (4 * (pr->center_y - G_current_scroll_amount)) >= LAYOUT_SCREEN_AREA_MAIN_LOWER_BOUND_PIXELS + 4) &&  \
    ((GRAPHICS_OVERSCAN_HEIGHT + 8 * 1) / 2 + (4 * (pr->center_y - G_current_scroll_amount)) <= LAYOUT_SCREEN_AREA_MAIN_UPPER_BOUND_PIXELS - 4))

#define LAYOUT_PATCH_PARAM_IS_NOT_IN_MAIN_AREA(pr)                              \
  (!(LAYOUT_PATCH_PARAM_IS_IN_MAIN_AREA(pr)))

/* vertical scrollbar amounts */
#define LAYOUT_PATCH_EDIT_MAX_SCROLL_AMOUNT   64
#define LAYOUT_PATTERN_EDIT_MAX_SCROLL_AMOUNT  0

/* patch edit headers & adjustable parameters */
#define LAYOUT_PATCH_EDIT_PATCH_NUMBER_X     -47
#define LAYOUT_PATCH_EDIT_PATCH_NUMBER_Y     -13

#define LAYOUT_PATCH_EDIT_COLUMN_1_CENTER_X  -37
#define LAYOUT_PATCH_EDIT_COLUMN_2_CENTER_X  -13
#define LAYOUT_PATCH_EDIT_COLUMN_3_CENTER_X   11
#define LAYOUT_PATCH_EDIT_COLUMN_4_CENTER_X   35

#define LAYOUT_PATCH_EDIT_ALGORITHM_Y         -9

#define LAYOUT_PATCH_EDIT_HEADER_OSC_Y        -5

#define LAYOUT_PATCH_EDIT_OSC_WAVEFORM_Y      -1
#define LAYOUT_PATCH_EDIT_OSC_FEEDBACK_Y       2
#define LAYOUT_PATCH_EDIT_OSC_SYNC_Y           5
#define LAYOUT_PATCH_EDIT_OSC_FREQ_MODE_Y      8
#define LAYOUT_PATCH_EDIT_OSC_MULTIPLE_Y      11
#define LAYOUT_PATCH_EDIT_OSC_DIVISOR_Y       14
#define LAYOUT_PATCH_EDIT_OSC_DETUNE_Y        17
#define LAYOUT_PATCH_EDIT_VIBRATO_ENABLE_Y    20

#define LAYOUT_PATCH_EDIT_HEADER_ENV_Y        24

#define LAYOUT_PATCH_EDIT_ENV_ATTACK_Y        28
#define LAYOUT_PATCH_EDIT_ENV_DECAY_1_Y       31
#define LAYOUT_PATCH_EDIT_ENV_DECAY_2_Y       34
#define LAYOUT_PATCH_EDIT_ENV_RELEASE_Y       37
#define LAYOUT_PATCH_EDIT_ENV_AMPLITUDE_Y     40
#define LAYOUT_PATCH_EDIT_ENV_SUSTAIN_Y       43
#define LAYOUT_PATCH_EDIT_ENV_RATE_KS_Y       46
#define LAYOUT_PATCH_EDIT_ENV_LEVEL_KS_Y      49
#define LAYOUT_PATCH_EDIT_TREMOLO_ENABLE_Y    52
#define LAYOUT_PATCH_EDIT_BOOST_ENABLE_Y      55

#define LAYOUT_PATCH_EDIT_HEADER_LFO_Y        62

#define LAYOUT_PATCH_EDIT_LFO_WAVEFORM_Y      66
#define LAYOUT_PATCH_EDIT_LFO_FREQUENCY_Y     69
#define LAYOUT_PATCH_EDIT_LFO_SYNC_Y          72
#define LAYOUT_PATCH_EDIT_LFO_DELAY_Y         75

#define LAYOUT_PATCH_EDIT_LFO_BASE_VIBRATO_Y  78
#define LAYOUT_PATCH_EDIT_LFO_BASE_TREMOLO_Y  81

#define LAYOUT_PATCH_EDIT_HEADER_PORTAMENTO_Y 62

#define LAYOUT_PATCH_EDIT_PORTAMENTO_MODE_Y   66
#define LAYOUT_PATCH_EDIT_PORTAMENTO_SPEED_Y  69

#define LAYOUT_PATCH_EDIT_HEADER_FILTERS_Y    62

#define LAYOUT_PATCH_EDIT_HIGHPASS_CUTOFF_Y   66
#define LAYOUT_PATCH_EDIT_LOWPASS_CUTOFF_Y    69

#define LAYOUT_PATCH_EDIT_HEADER_NOISE_Y      62

#define LAYOUT_PATCH_EDIT_NOISE_MIX_Y         66
#define LAYOUT_PATCH_EDIT_NOISE_FREQUENCY_Y   69

#define LAYOUT_PATCH_EDIT_HEADER_DEPTHS_Y     73

#define LAYOUT_PATCH_EDIT_VIBRATO_DEPTH_Y     77
#define LAYOUT_PATCH_EDIT_TREMOLO_DEPTH_Y     80
#define LAYOUT_PATCH_EDIT_BOOST_DEPTH_Y       83

#define LAYOUT_PATCH_EDIT_HEADER_MOD_WHEEL_Y  73

#define LAYOUT_PATCH_EDIT_MOD_WHEEL_VIBRATO_Y 77
#define LAYOUT_PATCH_EDIT_MOD_WHEEL_TREMOLO_Y 80
#define LAYOUT_PATCH_EDIT_MOD_WHEEL_BOOST_Y   83

#define LAYOUT_PATCH_EDIT_HEADER_AFTERTOUCH_Y   73

#define LAYOUT_PATCH_EDIT_AFTERTOUCH_VIBRATO_Y  77
#define LAYOUT_PATCH_EDIT_AFTERTOUCH_TREMOLO_Y  80
#define LAYOUT_PATCH_EDIT_AFTERTOUCH_BOOST_Y    83

#define LAYOUT_PATCH_EDIT_PARAM_NAME_X          -8

#define LAYOUT_PATCH_EDIT_PARAM_SLIDER_VALUE_X  -2
#define LAYOUT_PATCH_EDIT_PARAM_SLIDER_TRACK_X   6
#define LAYOUT_PATCH_EDIT_PARAM_SLIDER_WIDTH     5

#define LAYOUT_PATCH_EDIT_PARAM_ADJUST_LEFT_X   -4
#define LAYOUT_PATCH_EDIT_PARAM_ADJUST_VALUE_X   3
#define LAYOUT_PATCH_EDIT_PARAM_ADJUST_RIGHT_X  10

typedef struct button
{
  int label;

  int center_x;
  int center_y;

  int width;
  int state;
} button;

typedef struct header
{
  int label;

  int center_x;
  int center_y;
} header;

typedef struct param
{
  int label;
  int num;

  int adjust_type;

  int center_x;
  int center_y;

  short int lower_bound;
  short int upper_bound;
} param;

extern button G_layout_buttons[LAYOUT_NUM_BUTTONS];
extern header G_layout_headers[LAYOUT_NUM_HEADERS];
extern param  G_layout_params[LAYOUT_NUM_PARAMS];

/* function declarations */
short int layout_setup_all();

short int layout_reset_top_panel_button_states();

#endif
