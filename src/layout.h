/*******************************************************************************
** layout.h (screen layout widget positions)
*******************************************************************************/

#ifndef LAYOUT_H
#define LAYOUT_H

enum
{
  LAYOUT_PANEL_TYPE_1 = 1, 
  LAYOUT_PANEL_TYPE_2 
};

enum
{
  LAYOUT_BUTTON_STATE_OFF = 0, 
  LAYOUT_BUTTON_STATE_ON 
};

enum
{
  LAYOUT_PARAM_TYPE_SLIDER = 0, 
  LAYOUT_PARAM_TYPE_ARROWS, 
  LAYOUT_PARAM_TYPE_RADIO, 
  LAYOUT_PARAM_TYPE_FLAG, 
  LAYOUT_PARAM_TYPE_EDITABLE 
};

/* top panel */
enum
{
  LAYOUT_TOP_PANEL_BUTTON_LABEL_CART = 0, 
  LAYOUT_TOP_PANEL_BUTTON_LABEL_INSTRUMENTS, 
  LAYOUT_TOP_PANEL_BUTTON_LABEL_SONG, 
  LAYOUT_TOP_PANEL_BUTTON_LABEL_MIXER, 
  LAYOUT_TOP_PANEL_BUTTON_LABEL_SOUND_FX, 
  LAYOUT_TOP_PANEL_BUTTON_LABEL_DPCM, 
  LAYOUT_TOP_PANEL_BUTTON_NUM_LABELS
};

enum
{
  LAYOUT_TOP_PANEL_HEADER_LABEL_NAME = 0,
  LAYOUT_TOP_PANEL_HEADER_LABEL_VERSION, 
  LAYOUT_TOP_PANEL_HEADER_NUM_LABELS
};

/* cart screen */
enum
{
  LAYOUT_CART_BUTTON_LABEL_CART_LOAD = 0, 
  LAYOUT_CART_BUTTON_LABEL_CART_SAVE, 
  LAYOUT_CART_BUTTON_LABEL_CART_COPY, 
  LAYOUT_CART_BUTTON_LABEL_CART_ZAP, 
  LAYOUT_CART_BUTTON_LABEL_PATCH_LOAD, 
  LAYOUT_CART_BUTTON_LABEL_PATCH_SAVE, 
  LAYOUT_CART_BUTTON_LABEL_PATCH_COPY, 
  LAYOUT_CART_BUTTON_LABEL_PATCH_ZAP, 
  LAYOUT_CART_BUTTON_NUM_LABELS
};

enum
{
  LAYOUT_CART_HEADER_LABEL_OSC_1 = 0, 
  LAYOUT_CART_HEADER_LABEL_OSC_2, 
  LAYOUT_CART_HEADER_LABEL_OSC_3, 
  LAYOUT_CART_HEADER_LABEL_FILTERS, 
  LAYOUT_CART_HEADER_LABEL_ENV_1, 
  LAYOUT_CART_HEADER_LABEL_ENV_2, 
  LAYOUT_CART_HEADER_LABEL_ENV_3, 
  LAYOUT_CART_HEADER_LABEL_VIBRATO, 
  LAYOUT_CART_HEADER_LABEL_TREMOLO, 
  LAYOUT_CART_HEADER_LABEL_CHORUS, 
  LAYOUT_CART_HEADER_LABEL_SYNC, 
  LAYOUT_CART_HEADER_LABEL_SENSITIVITY, 
  LAYOUT_CART_HEADER_LABEL_PITCH_ENV, 
  LAYOUT_CART_HEADER_LABEL_ARPEGGIO, 
  LAYOUT_CART_HEADER_LABEL_PORTAMENTO, 
  LAYOUT_CART_HEADER_LABEL_PITCH_WHEEL, 
  LAYOUT_CART_HEADER_LABEL_TREMOLO_ROUTING, 
  LAYOUT_CART_HEADER_LABEL_BOOST_ROUTING, 
  LAYOUT_CART_HEADER_LABEL_VELOCITY_ROUTING, 
  LAYOUT_CART_HEADER_LABEL_MOD_WHEEL_ROUTING, 
  LAYOUT_CART_HEADER_LABEL_AFTERTOUCH_ROUTING, 
  LAYOUT_CART_HEADER_LABEL_EXP_PEDAL_ROUTING, 
  LAYOUT_CART_HEADER_NUM_LABELS
};

enum
{
  LAYOUT_CART_PARAM_LABEL_CART_NUMBER = 0, 
  LAYOUT_CART_PARAM_LABEL_PATCH_NUMBER, 
  LAYOUT_CART_PARAM_LABEL_ALGORITHM, 
  LAYOUT_CART_PARAM_LABEL_OSC_1_WAVEFORM, 
  LAYOUT_CART_PARAM_LABEL_OSC_1_PHI, 
  LAYOUT_CART_PARAM_LABEL_OSC_1_FREQ_MODE, 
  LAYOUT_CART_PARAM_LABEL_OSC_1_MULTIPLE, 
  LAYOUT_CART_PARAM_LABEL_OSC_1_DIVISOR, 
  LAYOUT_CART_PARAM_LABEL_OSC_1_DETUNE, 
  LAYOUT_CART_PARAM_LABEL_OSC_2_WAVEFORM, 
  LAYOUT_CART_PARAM_LABEL_OSC_2_PHI, 
  LAYOUT_CART_PARAM_LABEL_OSC_2_FREQ_MODE, 
  LAYOUT_CART_PARAM_LABEL_OSC_2_MULTIPLE, 
  LAYOUT_CART_PARAM_LABEL_OSC_2_DIVISOR, 
  LAYOUT_CART_PARAM_LABEL_OSC_2_DETUNE, 
  LAYOUT_CART_PARAM_LABEL_OSC_3_WAVEFORM, 
  LAYOUT_CART_PARAM_LABEL_OSC_3_PHI, 
  LAYOUT_CART_PARAM_LABEL_OSC_3_DETUNE, 
  LAYOUT_CART_PARAM_LABEL_LOWPASS_MULTIPLE, 
  LAYOUT_CART_PARAM_LABEL_LOWPASS_KEYTRACKING, 
  LAYOUT_CART_PARAM_LABEL_HIGHPASS_CUTOFF, 
  LAYOUT_CART_PARAM_LABEL_ENV_1_ATTACK, 
  LAYOUT_CART_PARAM_LABEL_ENV_1_DECAY, 
  LAYOUT_CART_PARAM_LABEL_ENV_1_RELEASE, 
  LAYOUT_CART_PARAM_LABEL_ENV_1_AMPLITUDE, 
  LAYOUT_CART_PARAM_LABEL_ENV_1_SUSTAIN, 
  LAYOUT_CART_PARAM_LABEL_ENV_1_HOLD, 
  LAYOUT_CART_PARAM_LABEL_ENV_1_PEDAL, 
  LAYOUT_CART_PARAM_LABEL_ENV_1_RATE_KS, 
  LAYOUT_CART_PARAM_LABEL_ENV_1_LEVEL_KS, 
  LAYOUT_CART_PARAM_LABEL_ENV_2_ATTACK, 
  LAYOUT_CART_PARAM_LABEL_ENV_2_DECAY, 
  LAYOUT_CART_PARAM_LABEL_ENV_2_RELEASE, 
  LAYOUT_CART_PARAM_LABEL_ENV_2_AMPLITUDE, 
  LAYOUT_CART_PARAM_LABEL_ENV_2_SUSTAIN, 
  LAYOUT_CART_PARAM_LABEL_ENV_2_HOLD, 
  LAYOUT_CART_PARAM_LABEL_ENV_2_PEDAL, 
  LAYOUT_CART_PARAM_LABEL_ENV_2_RATE_KS, 
  LAYOUT_CART_PARAM_LABEL_ENV_2_LEVEL_KS, 
  LAYOUT_CART_PARAM_LABEL_ENV_3_ATTACK, 
  LAYOUT_CART_PARAM_LABEL_ENV_3_DECAY, 
  LAYOUT_CART_PARAM_LABEL_ENV_3_RELEASE, 
  LAYOUT_CART_PARAM_LABEL_ENV_3_AMPLITUDE, 
  LAYOUT_CART_PARAM_LABEL_ENV_3_SUSTAIN, 
  LAYOUT_CART_PARAM_LABEL_ENV_3_HOLD, 
  LAYOUT_CART_PARAM_LABEL_ENV_3_PEDAL, 
  LAYOUT_CART_PARAM_LABEL_ENV_3_RATE_KS, 
  LAYOUT_CART_PARAM_LABEL_ENV_3_LEVEL_KS, 
  LAYOUT_CART_PARAM_LABEL_VIBRATO_WAVEFORM, 
  LAYOUT_CART_PARAM_LABEL_VIBRATO_DELAY, 
  LAYOUT_CART_PARAM_LABEL_VIBRATO_SPEED, 
  LAYOUT_CART_PARAM_LABEL_VIBRATO_DEPTH, 
  LAYOUT_CART_PARAM_LABEL_TREMOLO_WAVEFORM, 
  LAYOUT_CART_PARAM_LABEL_TREMOLO_DELAY, 
  LAYOUT_CART_PARAM_LABEL_TREMOLO_SPEED, 
  LAYOUT_CART_PARAM_LABEL_TREMOLO_DEPTH, 
  LAYOUT_CART_PARAM_LABEL_CHORUS_MODE, 
  LAYOUT_CART_PARAM_LABEL_CHORUS_DELAY, 
  LAYOUT_CART_PARAM_LABEL_CHORUS_SPEED, 
  LAYOUT_CART_PARAM_LABEL_CHORUS_DEPTH, 
  LAYOUT_CART_PARAM_LABEL_SYNC_VIBRATO, 
  LAYOUT_CART_PARAM_LABEL_SYNC_TREMOLO, 
  LAYOUT_CART_PARAM_LABEL_SYNC_CHORUS, 
  LAYOUT_CART_PARAM_LABEL_SYNC_OSC, 
  LAYOUT_CART_PARAM_LABEL_SENSITIVITY_VIBRATO, 
  LAYOUT_CART_PARAM_LABEL_SENSITIVITY_TREMOLO, 
  LAYOUT_CART_PARAM_LABEL_SENSITIVITY_CHORUS, 
  LAYOUT_CART_PARAM_LABEL_SENSITIVITY_BOOST, 
  LAYOUT_CART_PARAM_LABEL_SENSITIVITY_VELOCITY, 
  LAYOUT_CART_PARAM_LABEL_PITCH_ENV_ATTACK, 
  LAYOUT_CART_PARAM_LABEL_PITCH_ENV_DECAY, 
  LAYOUT_CART_PARAM_LABEL_PITCH_ENV_RELEASE, 
  LAYOUT_CART_PARAM_LABEL_PITCH_ENV_MAXIMUM, 
  LAYOUT_CART_PARAM_LABEL_PITCH_ENV_FINALE, 
  LAYOUT_CART_PARAM_LABEL_ARPEGGIO_MODE, 
  LAYOUT_CART_PARAM_LABEL_ARPEGGIO_PATTERN, 
  LAYOUT_CART_PARAM_LABEL_ARPEGGIO_OCTAVES, 
  LAYOUT_CART_PARAM_LABEL_ARPEGGIO_SPEED, 
  LAYOUT_CART_PARAM_LABEL_PORTAMENTO_MODE, 
  LAYOUT_CART_PARAM_LABEL_PORTAMENTO_LEGATO, 
  LAYOUT_CART_PARAM_LABEL_PORTAMENTO_SPEED, 
  LAYOUT_CART_PARAM_LABEL_PITCH_WHEEL_MODE, 
  LAYOUT_CART_PARAM_LABEL_PITCH_WHEEL_RANGE, 
  LAYOUT_CART_PARAM_LABEL_TREMOLO_ROUTING_ENV_1, 
  LAYOUT_CART_PARAM_LABEL_TREMOLO_ROUTING_ENV_2, 
  LAYOUT_CART_PARAM_LABEL_TREMOLO_ROUTING_ENV_3, 
  LAYOUT_CART_PARAM_LABEL_BOOST_ROUTING_ENV_1, 
  LAYOUT_CART_PARAM_LABEL_BOOST_ROUTING_ENV_2, 
  LAYOUT_CART_PARAM_LABEL_BOOST_ROUTING_ENV_3, 
  LAYOUT_CART_PARAM_LABEL_VELOCITY_ROUTING_ENV_1, 
  LAYOUT_CART_PARAM_LABEL_VELOCITY_ROUTING_ENV_2, 
  LAYOUT_CART_PARAM_LABEL_VELOCITY_ROUTING_ENV_3, 
  LAYOUT_CART_PARAM_LABEL_MOD_WHEEL_ROUTING_VIBRATO, 
  LAYOUT_CART_PARAM_LABEL_MOD_WHEEL_ROUTING_TREMOLO, 
  LAYOUT_CART_PARAM_LABEL_MOD_WHEEL_ROUTING_BOOST, 
  LAYOUT_CART_PARAM_LABEL_MOD_WHEEL_ROUTING_CHORUS, 
  LAYOUT_CART_PARAM_LABEL_AFTERTOUCH_ROUTING_VIBRATO, 
  LAYOUT_CART_PARAM_LABEL_AFTERTOUCH_ROUTING_TREMOLO, 
  LAYOUT_CART_PARAM_LABEL_AFTERTOUCH_ROUTING_BOOST, 
  LAYOUT_CART_PARAM_LABEL_AFTERTOUCH_ROUTING_CHORUS, 
  LAYOUT_CART_PARAM_LABEL_EXP_PEDAL_ROUTING_VIBRATO, 
  LAYOUT_CART_PARAM_LABEL_EXP_PEDAL_ROUTING_TREMOLO, 
  LAYOUT_CART_PARAM_LABEL_EXP_PEDAL_ROUTING_BOOST, 
  LAYOUT_CART_PARAM_LABEL_EXP_PEDAL_ROUTING_CHORUS, 
  LAYOUT_CART_PARAM_LABEL_AUDITION_OCTAVE, 
  LAYOUT_CART_PARAM_LABEL_AUDITION_VELOCITY, 
  LAYOUT_CART_PARAM_LABEL_AUDITION_PITCH_WHEEL,
  LAYOUT_CART_PARAM_LABEL_AUDITION_MOD_WHEEL, 
  LAYOUT_CART_PARAM_LABEL_AUDITION_AFTERTOUCH, 
  LAYOUT_CART_PARAM_LABEL_AUDITION_EXP_PEDAL, 
  LAYOUT_CART_PARAM_NUM_LABELS
};

/* load screen */
enum
{
  LAYOUT_LOAD_BUTTON_LABEL_LOAD = 0, 
  LAYOUT_LOAD_BUTTON_NUM_LABELS
};

enum
{
  LAYOUT_LOAD_HEADER_LABEL_TITLE = 0, 
  LAYOUT_LOAD_HEADER_NUM_LABELS
};

/* save screen */
enum
{
  LAYOUT_SAVE_BUTTON_LABEL_SAVE = 0, 
  LAYOUT_SAVE_BUTTON_NUM_LABELS
};

enum
{
  LAYOUT_SAVE_HEADER_LABEL_TITLE = 0, 
  LAYOUT_SAVE_HEADER_NUM_LABELS
};

/* copy screen */
enum
{
  LAYOUT_COPY_BUTTON_LABEL_COPY = 0, 
  LAYOUT_COPY_BUTTON_NUM_LABELS
};

enum
{
  LAYOUT_COPY_HEADER_LABEL_TITLE = 0, 
  LAYOUT_COPY_HEADER_NUM_LABELS
};

/* zap screen */
enum
{
  LAYOUT_ZAP_BUTTON_LABEL_ZAP = 0, 
  LAYOUT_ZAP_BUTTON_NUM_LABELS
};

enum
{
  LAYOUT_ZAP_HEADER_LABEL_TITLE = 0, 
  LAYOUT_ZAP_HEADER_NUM_LABELS
};

/* screen center (in half-cells) */
#define LAYOUT_OVERSCAN_WIDTH   400
#define LAYOUT_OVERSCAN_HEIGHT  224

#define LAYOUT_OVERSCAN_CENTER_X  (LAYOUT_OVERSCAN_WIDTH / 2)
#define LAYOUT_OVERSCAN_CENTER_Y  (LAYOUT_OVERSCAN_HEIGHT / 2)

/* buttons */
#define LAYOUT_NUM_TOP_PANEL_BUTTONS  LAYOUT_TOP_PANEL_BUTTON_NUM_LABELS
#define LAYOUT_NUM_CART_BUTTONS       LAYOUT_CART_BUTTON_NUM_LABELS
#define LAYOUT_NUM_LOAD_BUTTONS       LAYOUT_LOAD_BUTTON_NUM_LABELS
#define LAYOUT_NUM_SAVE_BUTTONS       LAYOUT_SAVE_BUTTON_NUM_LABELS
#define LAYOUT_NUM_COPY_BUTTONS       LAYOUT_COPY_BUTTON_NUM_LABELS
#define LAYOUT_NUM_ZAP_BUTTONS        LAYOUT_ZAP_BUTTON_NUM_LABELS

#define LAYOUT_NUM_BUTTONS    ( LAYOUT_NUM_TOP_PANEL_BUTTONS +                 \
                                LAYOUT_NUM_CART_BUTTONS +                      \
                                LAYOUT_NUM_LOAD_BUTTONS +                      \
                                LAYOUT_NUM_SAVE_BUTTONS +                      \
                                LAYOUT_NUM_COPY_BUTTONS +                      \
                                LAYOUT_NUM_ZAP_BUTTONS)

#define LAYOUT_TOP_PANEL_BUTTONS_START_INDEX   0
#define LAYOUT_TOP_PANEL_BUTTONS_END_INDEX     (LAYOUT_TOP_PANEL_BUTTONS_START_INDEX + \
                                                LAYOUT_NUM_TOP_PANEL_BUTTONS)

#define LAYOUT_CART_BUTTONS_START_INDEX   LAYOUT_TOP_PANEL_BUTTONS_END_INDEX
#define LAYOUT_CART_BUTTONS_END_INDEX     ( LAYOUT_CART_BUTTONS_START_INDEX +  \
                                            LAYOUT_NUM_CART_BUTTONS)

#define LAYOUT_LOAD_BUTTONS_START_INDEX   LAYOUT_CART_BUTTONS_END_INDEX
#define LAYOUT_LOAD_BUTTONS_END_INDEX     ( LAYOUT_LOAD_BUTTONS_START_INDEX +  \
                                            LAYOUT_NUM_LOAD_BUTTONS)

#define LAYOUT_SAVE_BUTTONS_START_INDEX   LAYOUT_LOAD_BUTTONS_END_INDEX
#define LAYOUT_SAVE_BUTTONS_END_INDEX     ( LAYOUT_SAVE_BUTTONS_START_INDEX +  \
                                            LAYOUT_NUM_SAVE_BUTTONS)

#define LAYOUT_COPY_BUTTONS_START_INDEX   LAYOUT_SAVE_BUTTONS_END_INDEX
#define LAYOUT_COPY_BUTTONS_END_INDEX     ( LAYOUT_COPY_BUTTONS_START_INDEX +  \
                                            LAYOUT_NUM_COPY_BUTTONS)

#define LAYOUT_ZAP_BUTTONS_START_INDEX    LAYOUT_COPY_BUTTONS_END_INDEX
#define LAYOUT_ZAP_BUTTONS_END_INDEX      ( LAYOUT_ZAP_BUTTONS_START_INDEX +   \
                                            LAYOUT_NUM_ZAP_BUTTONS)

#define LAYOUT_BUTTON_INDEX_IS_VALID(index)                                    \
  ((index >= 0) && (index < LAYOUT_NUM_BUTTONS))

#define LAYOUT_BUTTON_INDEX_IS_NOT_VALID(index)                                \
  (!(LAYOUT_BUTTON_INDEX_IS_VALID(index)))

/* headers */
#define LAYOUT_NUM_TOP_PANEL_HEADERS    LAYOUT_TOP_PANEL_HEADER_NUM_LABELS
#define LAYOUT_NUM_CART_HEADERS         LAYOUT_CART_HEADER_NUM_LABELS
#define LAYOUT_NUM_LOAD_HEADERS         LAYOUT_LOAD_HEADER_NUM_LABELS
#define LAYOUT_NUM_SAVE_HEADERS         LAYOUT_SAVE_HEADER_NUM_LABELS
#define LAYOUT_NUM_COPY_HEADERS         LAYOUT_COPY_HEADER_NUM_LABELS
#define LAYOUT_NUM_ZAP_HEADERS          LAYOUT_ZAP_HEADER_NUM_LABELS

#define LAYOUT_NUM_HEADERS    ( LAYOUT_NUM_TOP_PANEL_HEADERS +                 \
                                LAYOUT_NUM_CART_HEADERS +                      \
                                LAYOUT_NUM_LOAD_HEADERS +                      \
                                LAYOUT_NUM_SAVE_HEADERS +                      \
                                LAYOUT_NUM_COPY_HEADERS +                      \
                                LAYOUT_NUM_ZAP_HEADERS)

#define LAYOUT_TOP_PANEL_HEADERS_START_INDEX   0
#define LAYOUT_TOP_PANEL_HEADERS_END_INDEX     (LAYOUT_TOP_PANEL_HEADERS_START_INDEX + \
                                                LAYOUT_NUM_TOP_PANEL_HEADERS)

#define LAYOUT_CART_HEADERS_START_INDEX   LAYOUT_TOP_PANEL_HEADERS_END_INDEX
#define LAYOUT_CART_HEADERS_END_INDEX     ( LAYOUT_CART_HEADERS_START_INDEX +  \
                                            LAYOUT_NUM_CART_HEADERS)

#define LAYOUT_LOAD_HEADERS_START_INDEX   LAYOUT_CART_HEADERS_END_INDEX
#define LAYOUT_LOAD_HEADERS_END_INDEX     ( LAYOUT_LOAD_HEADERS_START_INDEX +  \
                                            LAYOUT_NUM_LOAD_HEADERS)

#define LAYOUT_SAVE_HEADERS_START_INDEX   LAYOUT_LOAD_HEADERS_END_INDEX
#define LAYOUT_SAVE_HEADERS_END_INDEX     ( LAYOUT_SAVE_HEADERS_START_INDEX +  \
                                            LAYOUT_NUM_SAVE_HEADERS)

#define LAYOUT_COPY_HEADERS_START_INDEX   LAYOUT_SAVE_HEADERS_END_INDEX
#define LAYOUT_COPY_HEADERS_END_INDEX     ( LAYOUT_COPY_HEADERS_START_INDEX +  \
                                            LAYOUT_NUM_COPY_HEADERS)

#define LAYOUT_ZAP_HEADERS_START_INDEX    LAYOUT_COPY_HEADERS_END_INDEX
#define LAYOUT_ZAP_HEADERS_END_INDEX      ( LAYOUT_ZAP_HEADERS_START_INDEX +   \
                                            LAYOUT_NUM_ZAP_HEADERS)

#define LAYOUT_HEADER_INDEX_IS_VALID(index)                                    \
  ((index >= 0) && (index < LAYOUT_NUM_HEADERS))

#define LAYOUT_HEADER_INDEX_IS_NOT_VALID(index)                                \
  (!(LAYOUT_HEADER_INDEX_IS_VALID(index)))

/* params */
#define LAYOUT_NUM_CART_PARAMS        LAYOUT_CART_PARAM_NUM_LABELS
#define LAYOUT_NUM_INSTRUMENTS_PARAMS 0
#define LAYOUT_NUM_SONG_PARAMS        0
#define LAYOUT_NUM_MIXER_PARAMS       0
#define LAYOUT_NUM_SOUND_FX_PARAMS    0
#define LAYOUT_NUM_DPCM_PARAMS        0

#define LAYOUT_NUM_PARAMS   ( LAYOUT_NUM_CART_PARAMS +                         \
                              LAYOUT_NUM_INSTRUMENTS_PARAMS +                  \
                              LAYOUT_NUM_SONG_PARAMS +                         \
                              LAYOUT_NUM_MIXER_PARAMS +                        \
                              LAYOUT_NUM_SOUND_FX_PARAMS +                     \
                              LAYOUT_NUM_DPCM_PARAMS)

#define LAYOUT_CART_PARAMS_START_INDEX  0
#define LAYOUT_CART_PARAMS_END_INDEX    ( LAYOUT_CART_PARAMS_START_INDEX + \
                                          LAYOUT_NUM_CART_PARAMS)

#define LAYOUT_INSTRUMENTS_PARAMS_START_INDEX   LAYOUT_CART_PARAMS_END_INDEX
#define LAYOUT_INSTRUMENTS_PARAMS_END_INDEX     ( LAYOUT_INSTRUMENTS_PARAMS_START_INDEX + \
                                                  LAYOUT_INSTRUMENTS_PARAMS)

#define LAYOUT_SONG_PARAMS_START_INDEX      LAYOUT_INSTRUMENTS_PARAMS_END_INDEX
#define LAYOUT_SONG_PARAMS_END_INDEX        ( LAYOUT_SONG_PARAMS_START_INDEX + \
                                              LAYOUT_SONG_PARAMS)

#define LAYOUT_MIXER_PARAMS_START_INDEX     LAYOUT_SONG_PARAMS_END_INDEX
#define LAYOUT_MIXER_PARAMS_END_INDEX       ( LAYOUT_MIXER_PARAMS_START_INDEX + \
                                              LAYOUT_MIXER_PARAMS)

#define LAYOUT_SOUND_FX_PARAMS_START_INDEX  LAYOUT_MIXER_PARAMS_END_INDEX
#define LAYOUT_SOUND_FX_PARAMS_END_INDEX    ( LAYOUT_SOUND_FX_PARAMS_START_INDEX + \
                                              LAYOUT_SOUND_FX_PARAMS)

#define LAYOUT_DPCM_PARAMS_START_INDEX      LAYOUT_SOUND_FX_PARAMS_END_INDEX
#define LAYOUT_DPCM_PARAMS_END_INDEX        ( LAYOUT_DPCM_PARAMS_START_INDEX + \
                                              LAYOUT_DPCM_PARAMS)

#define LAYOUT_PARAM_INDEX_IS_VALID(index)                                     \
  ((index >= 0) && (index < LAYOUT_NUM_PARAMS))

#define LAYOUT_PARAM_INDEX_IS_NOT_VALID(index)                                 \
  (!(LAYOUT_PARAM_INDEX_IS_VALID(index)))

/* screen areas positions & sizes */
#define LAYOUT_TOP_PANEL_AREA_X        0
#define LAYOUT_TOP_PANEL_AREA_Y      -88
#define LAYOUT_TOP_PANEL_AREA_WIDTH   50
#define LAYOUT_TOP_PANEL_AREA_HEIGHT   6

#define LAYOUT_VERT_SCROLLBAR_AREA_X      196
#define LAYOUT_VERT_SCROLLBAR_AREA_Y       24
#define LAYOUT_VERT_SCROLLBAR_AREA_WIDTH    1
#define LAYOUT_VERT_SCROLLBAR_AREA_HEIGHT  22

#define LAYOUT_VERT_SCROLLBAR_UP_ARROW_X        196
#define LAYOUT_VERT_SCROLLBAR_UP_ARROW_Y        -60
#define LAYOUT_VERT_SCROLLBAR_UP_ARROW_WIDTH      1
#define LAYOUT_VERT_SCROLLBAR_UP_ARROW_HEIGHT     1

#define LAYOUT_VERT_SCROLLBAR_DOWN_ARROW_X      196
#define LAYOUT_VERT_SCROLLBAR_DOWN_ARROW_Y      108
#define LAYOUT_VERT_SCROLLBAR_DOWN_ARROW_WIDTH    1
#define LAYOUT_VERT_SCROLLBAR_DOWN_ARROW_HEIGHT   1

#define LAYOUT_VERT_SCROLLBAR_TRACK_X           196
#define LAYOUT_VERT_SCROLLBAR_TRACK_Y            24
#define LAYOUT_VERT_SCROLLBAR_TRACK_WIDTH         1
#define LAYOUT_VERT_SCROLLBAR_TRACK_HEIGHT       20

#define LAYOUT_CART_MAIN_AREA_X            -4
#define LAYOUT_CART_MAIN_AREA_Y             8
#define LAYOUT_CART_MAIN_AREA_WIDTH        49
#define LAYOUT_CART_MAIN_AREA_HEIGHT       18

#define LAYOUT_CART_AUDITION_PANEL_X       -4
#define LAYOUT_CART_AUDITION_PANEL_Y       96
#define LAYOUT_CART_AUDITION_PANEL_WIDTH   49
#define LAYOUT_CART_AUDITION_PANEL_HEIGHT   4

#define LAYOUT_BROWSER_DIR_PANEL_X         -4
#define LAYOUT_BROWSER_DIR_PANEL_Y        -48
#define LAYOUT_BROWSER_DIR_PANEL_WIDTH     49
#define LAYOUT_BROWSER_DIR_PANEL_HEIGHT     4

#define LAYOUT_BROWSER_MAIN_AREA_X         -4
#define LAYOUT_BROWSER_MAIN_AREA_Y         12
#define LAYOUT_BROWSER_MAIN_AREA_WIDTH     49
#define LAYOUT_BROWSER_MAIN_AREA_HEIGHT    19

#define LAYOUT_BROWSER_FILE_PANEL_X        -4
#define LAYOUT_BROWSER_FILE_PANEL_Y       100
#define LAYOUT_BROWSER_FILE_PANEL_WIDTH    49
#define LAYOUT_BROWSER_FILE_PANEL_HEIGHT    3

/* cart edit screen regions */
#define LAYOUT_CART_BUTTON_IS_IN_MAIN_AREA(pos_y)                                   \
  ( (pos_y - 4 * 2 > LAYOUT_CART_MAIN_AREA_Y - 4 * LAYOUT_CART_MAIN_AREA_HEIGHT) && \
    (pos_y + 4 * 2 < LAYOUT_CART_MAIN_AREA_Y + 4 * LAYOUT_CART_MAIN_AREA_HEIGHT))

#define LAYOUT_CART_BUTTON_IS_NOT_IN_MAIN_AREA(pos_y)                          \
  (!(LAYOUT_CART_BUTTON_IS_IN_MAIN_AREA(pos_y)))

#define LAYOUT_CART_HEADER_OR_PARAM_IS_IN_MAIN_AREA(pos_y)                          \
  ( (pos_y - 4 * 1 > LAYOUT_CART_MAIN_AREA_Y - 4 * LAYOUT_CART_MAIN_AREA_HEIGHT) && \
    (pos_y + 4 * 1 < LAYOUT_CART_MAIN_AREA_Y + 4 * LAYOUT_CART_MAIN_AREA_HEIGHT))

#define LAYOUT_CART_HEADER_OR_PARAM_IS_NOT_IN_MAIN_AREA(pos_y)                 \
  (!(LAYOUT_CART_HEADER_OR_PARAM_IS_IN_MAIN_AREA(pos_y)))

/* browser screen regions */
#define LAYOUT_BROWSER_FILENAME_IS_IN_MAIN_AREA(pos_y)                                \
  ( (pos_y >  (LAYOUT_BROWSER_MAIN_AREA_Y - 4 * LAYOUT_BROWSER_MAIN_AREA_HEIGHT)) &&  \
    (pos_y <  (LAYOUT_BROWSER_MAIN_AREA_Y + 4 * LAYOUT_BROWSER_MAIN_AREA_HEIGHT)))

#define LAYOUT_BROWSER_FILENAME_IS_NOT_IN_MAIN_AREA(pos_y)                     \
  (!(LAYOUT_BROWSER_FILENAME_IS_IN_MAIN_AREA(pos_y)))

/* vertical scrollbar amounts */
#define LAYOUT_CART_MAX_SCROLL_AMOUNT         528
#define LAYOUT_INSTRUMENTS_MAX_SCROLL_AMOUNT    0
#define LAYOUT_SONG_MAX_SCROLL_AMOUNT           0
#define LAYOUT_MIXER_MAX_SCROLL_AMOUNT          0
#define LAYOUT_SOUND_FX_MAX_SCROLL_AMOUNT       0
#define LAYOUT_DPCM_MAX_SCROLL_AMOUNT           0
#define LAYOUT_BAR_MAX_SCROLL_AMOUNT            0

/* top panel header positions */
#define LAYOUT_TOP_PANEL_HEADER_NAME_X      -20
#define LAYOUT_TOP_PANEL_HEADER_NAME_Y      -96

#define LAYOUT_TOP_PANEL_HEADER_VERSION_X    36
#define LAYOUT_TOP_PANEL_HEADER_VERSION_Y   -96

/* top panel button positions & sizes */
#define LAYOUT_TOP_PANEL_BUTTON_Y                   -80

#define LAYOUT_TOP_PANEL_BUTTON_CART_X             -168
#define LAYOUT_TOP_PANEL_BUTTON_CART_WIDTH            6

#define LAYOUT_TOP_PANEL_BUTTON_INSTRUMENTS_X       -92
#define LAYOUT_TOP_PANEL_BUTTON_INSTRUMENTS_WIDTH    13

#define LAYOUT_TOP_PANEL_BUTTON_SONG_X              -16
#define LAYOUT_TOP_PANEL_BUTTON_SONG_WIDTH            6

#define LAYOUT_TOP_PANEL_BUTTON_MIXER_X              36
#define LAYOUT_TOP_PANEL_BUTTON_MIXER_WIDTH           7

#define LAYOUT_TOP_PANEL_BUTTON_SOUND_FX_X          104
#define LAYOUT_TOP_PANEL_BUTTON_SOUND_FX_WIDTH       10

#define LAYOUT_TOP_PANEL_BUTTON_DPCM_X              168
#define LAYOUT_TOP_PANEL_BUTTON_DPCM_WIDTH            6

/* cart edit position macros */
#define LAYOUT_CART_COMPUTE_NEXT_HEADER_Y(last_header, row)             \
  (last_header + 16 * 2 + 12 * row + 4)

#define LAYOUT_CART_COMPUTE_PARAM_Y(header_name, row)                   \
  (header_name + 16 * 1 + 12 * row)

/* cart edit buttons, headers, and adjustable parameters */
#define LAYOUT_CART_WIDE_COLUMN_1_CENTER_X  -132
#define LAYOUT_CART_WIDE_COLUMN_2_CENTER_X    -4
#define LAYOUT_CART_WIDE_COLUMN_3_CENTER_X   124

/* cart number */
#define LAYOUT_CART_CART_NUMBER_X           -148
#define LAYOUT_CART_CART_NUMBER_Y            -52

#define LAYOUT_CART_BUTTON_CART_LOAD_X        24
#define LAYOUT_CART_BUTTON_CART_LOAD_Y       -52
#define LAYOUT_CART_BUTTON_CART_LOAD_WIDTH     6

#define LAYOUT_CART_BUTTON_CART_SAVE_X        72
#define LAYOUT_CART_BUTTON_CART_SAVE_Y       -52
#define LAYOUT_CART_BUTTON_CART_SAVE_WIDTH     6

#define LAYOUT_CART_BUTTON_CART_COPY_X       120
#define LAYOUT_CART_BUTTON_CART_COPY_Y       -52
#define LAYOUT_CART_BUTTON_CART_COPY_WIDTH     6

#define LAYOUT_CART_BUTTON_CART_ZAP_X        164
#define LAYOUT_CART_BUTTON_CART_ZAP_Y        -52
#define LAYOUT_CART_BUTTON_CART_ZAP_WIDTH      5

/* patch number */
#define LAYOUT_CART_PATCH_NUMBER_X          -148
#define LAYOUT_CART_PATCH_NUMBER_Y           -36

#define LAYOUT_CART_BUTTON_PATCH_LOAD_X       24
#define LAYOUT_CART_BUTTON_PATCH_LOAD_Y      -36
#define LAYOUT_CART_BUTTON_PATCH_LOAD_WIDTH    6

#define LAYOUT_CART_BUTTON_PATCH_SAVE_X       72
#define LAYOUT_CART_BUTTON_PATCH_SAVE_Y      -36
#define LAYOUT_CART_BUTTON_PATCH_SAVE_WIDTH    6

#define LAYOUT_CART_BUTTON_PATCH_COPY_X      120
#define LAYOUT_CART_BUTTON_PATCH_COPY_Y      -36
#define LAYOUT_CART_BUTTON_PATCH_COPY_WIDTH    6

#define LAYOUT_CART_BUTTON_PATCH_ZAP_X       164
#define LAYOUT_CART_BUTTON_PATCH_ZAP_Y       -36
#define LAYOUT_CART_BUTTON_PATCH_ZAP_WIDTH     5

/* divider */
#define LAYOUT_CART_MAIN_DIVIDER_X          -4
#define LAYOUT_CART_MAIN_DIVIDER_Y         -20
#define LAYOUT_CART_MAIN_DIVIDER_WIDTH      46

/* headers & parameters */
#define LAYOUT_CART_ALGORITHM_X             LAYOUT_CART_WIDE_COLUMN_1_CENTER_X
#define LAYOUT_CART_ALGORITHM_Y            -4

#define LAYOUT_CART_HEADER_OSC_1_X          LAYOUT_CART_WIDE_COLUMN_1_CENTER_X
#define LAYOUT_CART_HEADER_OSC_1_Y          16

#define LAYOUT_CART_OSC_1_WAVEFORM_Y        LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_OSC_1_Y, 0)
#define LAYOUT_CART_OSC_1_PHI_Y             LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_OSC_1_Y, 1)
#define LAYOUT_CART_OSC_1_FREQ_MODE_Y       LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_OSC_1_Y, 2)
#define LAYOUT_CART_OSC_1_MULTIPLE_Y        LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_OSC_1_Y, 3)
#define LAYOUT_CART_OSC_1_DIVISOR_Y         LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_OSC_1_Y, 4)
#define LAYOUT_CART_OSC_1_DETUNE_Y          LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_OSC_1_Y, 5)

#define LAYOUT_CART_HEADER_OSC_2_X          LAYOUT_CART_WIDE_COLUMN_2_CENTER_X
#define LAYOUT_CART_HEADER_OSC_2_Y          LAYOUT_CART_HEADER_OSC_1_Y

#define LAYOUT_CART_OSC_2_WAVEFORM_Y        LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_OSC_2_Y, 0)
#define LAYOUT_CART_OSC_2_PHI_Y             LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_OSC_2_Y, 1)
#define LAYOUT_CART_OSC_2_FREQ_MODE_Y       LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_OSC_2_Y, 2)
#define LAYOUT_CART_OSC_2_MULTIPLE_Y        LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_OSC_2_Y, 3)
#define LAYOUT_CART_OSC_2_DIVISOR_Y         LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_OSC_2_Y, 4)
#define LAYOUT_CART_OSC_2_DETUNE_Y          LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_OSC_2_Y, 5)

#define LAYOUT_CART_HEADER_OSC_3_X          LAYOUT_CART_WIDE_COLUMN_3_CENTER_X
#define LAYOUT_CART_HEADER_OSC_3_Y          LAYOUT_CART_HEADER_OSC_1_Y

#define LAYOUT_CART_OSC_3_WAVEFORM_Y        LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_OSC_3_Y, 0)
#define LAYOUT_CART_OSC_3_PHI_Y             LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_OSC_3_Y, 1)
#define LAYOUT_CART_OSC_3_DETUNE_Y          LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_OSC_3_Y, 2)

#define LAYOUT_CART_HEADER_FILTERS_X        LAYOUT_CART_WIDE_COLUMN_3_CENTER_X
#define LAYOUT_CART_HEADER_FILTERS_Y        LAYOUT_CART_COMPUTE_NEXT_HEADER_Y(LAYOUT_CART_HEADER_OSC_3_Y, 2)

#define LAYOUT_CART_LOWPASS_MULTIPLE_Y      LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_FILTERS_Y, 0)
#define LAYOUT_CART_LOWPASS_KEYTRACKING_Y   LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_FILTERS_Y, 1)
#define LAYOUT_CART_HIGHPASS_CUTOFF_Y       LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_FILTERS_Y, 2)

#define LAYOUT_CART_HEADER_ENV_1_X          LAYOUT_CART_WIDE_COLUMN_1_CENTER_X
#define LAYOUT_CART_HEADER_ENV_1_Y          LAYOUT_CART_COMPUTE_NEXT_HEADER_Y(LAYOUT_CART_HEADER_OSC_1_Y, 7)

#define LAYOUT_CART_ENV_1_ATTACK_Y          LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ENV_1_Y, 0)
#define LAYOUT_CART_ENV_1_DECAY_Y           LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ENV_1_Y, 1)
#define LAYOUT_CART_ENV_1_RELEASE_Y         LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ENV_1_Y, 2)
#define LAYOUT_CART_ENV_1_AMPLITUDE_Y       LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ENV_1_Y, 3)
#define LAYOUT_CART_ENV_1_SUSTAIN_Y         LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ENV_1_Y, 4)
#define LAYOUT_CART_ENV_1_HOLD_Y            LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ENV_1_Y, 5)
#define LAYOUT_CART_ENV_1_PEDAL_Y           LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ENV_1_Y, 6)
#define LAYOUT_CART_ENV_1_RATE_KS_Y         LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ENV_1_Y, 7)
#define LAYOUT_CART_ENV_1_LEVEL_KS_Y        LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ENV_1_Y, 8)

#define LAYOUT_CART_HEADER_ENV_2_X          LAYOUT_CART_WIDE_COLUMN_2_CENTER_X
#define LAYOUT_CART_HEADER_ENV_2_Y          LAYOUT_CART_HEADER_ENV_1_Y

#define LAYOUT_CART_ENV_2_ATTACK_Y          LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ENV_2_Y, 0)
#define LAYOUT_CART_ENV_2_DECAY_Y           LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ENV_2_Y, 1)
#define LAYOUT_CART_ENV_2_RELEASE_Y         LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ENV_2_Y, 2)
#define LAYOUT_CART_ENV_2_AMPLITUDE_Y       LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ENV_2_Y, 3)
#define LAYOUT_CART_ENV_2_SUSTAIN_Y         LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ENV_2_Y, 4)
#define LAYOUT_CART_ENV_2_HOLD_Y            LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ENV_2_Y, 5)
#define LAYOUT_CART_ENV_2_PEDAL_Y           LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ENV_2_Y, 6)
#define LAYOUT_CART_ENV_2_RATE_KS_Y         LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ENV_2_Y, 7)
#define LAYOUT_CART_ENV_2_LEVEL_KS_Y        LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ENV_2_Y, 8)

#define LAYOUT_CART_HEADER_ENV_3_X          LAYOUT_CART_WIDE_COLUMN_3_CENTER_X
#define LAYOUT_CART_HEADER_ENV_3_Y          LAYOUT_CART_HEADER_ENV_1_Y

#define LAYOUT_CART_ENV_3_ATTACK_Y          LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ENV_3_Y, 0)
#define LAYOUT_CART_ENV_3_DECAY_Y           LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ENV_3_Y, 1)
#define LAYOUT_CART_ENV_3_RELEASE_Y         LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ENV_3_Y, 2)
#define LAYOUT_CART_ENV_3_AMPLITUDE_Y       LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ENV_3_Y, 3)
#define LAYOUT_CART_ENV_3_SUSTAIN_Y         LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ENV_3_Y, 4)
#define LAYOUT_CART_ENV_3_HOLD_Y            LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ENV_3_Y, 5)
#define LAYOUT_CART_ENV_3_PEDAL_Y           LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ENV_3_Y, 6)
#define LAYOUT_CART_ENV_3_RATE_KS_Y         LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ENV_3_Y, 7)
#define LAYOUT_CART_ENV_3_LEVEL_KS_Y        LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ENV_3_Y, 8)

#define LAYOUT_CART_HEADER_VIBRATO_X        LAYOUT_CART_WIDE_COLUMN_1_CENTER_X
#define LAYOUT_CART_HEADER_VIBRATO_Y        LAYOUT_CART_COMPUTE_NEXT_HEADER_Y(LAYOUT_CART_HEADER_ENV_1_Y, 8)

#define LAYOUT_CART_VIBRATO_WAVEFORM_Y      LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_VIBRATO_Y, 0)
#define LAYOUT_CART_VIBRATO_DELAY_Y         LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_VIBRATO_Y, 1)
#define LAYOUT_CART_VIBRATO_SPEED_Y         LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_VIBRATO_Y, 2)
#define LAYOUT_CART_VIBRATO_DEPTH_Y         LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_VIBRATO_Y, 3)

#define LAYOUT_CART_HEADER_TREMOLO_X        LAYOUT_CART_WIDE_COLUMN_2_CENTER_X
#define LAYOUT_CART_HEADER_TREMOLO_Y        LAYOUT_CART_HEADER_VIBRATO_Y

#define LAYOUT_CART_TREMOLO_WAVEFORM_Y      LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_TREMOLO_Y, 0)
#define LAYOUT_CART_TREMOLO_DELAY_Y         LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_TREMOLO_Y, 1)
#define LAYOUT_CART_TREMOLO_SPEED_Y         LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_TREMOLO_Y, 2)
#define LAYOUT_CART_TREMOLO_DEPTH_Y         LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_TREMOLO_Y, 3)

#define LAYOUT_CART_HEADER_CHORUS_X         LAYOUT_CART_WIDE_COLUMN_3_CENTER_X
#define LAYOUT_CART_HEADER_CHORUS_Y         LAYOUT_CART_HEADER_VIBRATO_Y

#define LAYOUT_CART_CHORUS_MODE_Y           LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_CHORUS_Y, 0)
#define LAYOUT_CART_CHORUS_DELAY_Y          LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_CHORUS_Y, 1)
#define LAYOUT_CART_CHORUS_SPEED_Y          LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_CHORUS_Y, 2)
#define LAYOUT_CART_CHORUS_DEPTH_Y          LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_CHORUS_Y, 3)

#define LAYOUT_CART_HEADER_SYNC_X           LAYOUT_CART_WIDE_COLUMN_1_CENTER_X
#define LAYOUT_CART_HEADER_SYNC_Y           LAYOUT_CART_COMPUTE_NEXT_HEADER_Y(LAYOUT_CART_HEADER_VIBRATO_Y, 3)

#define LAYOUT_CART_SYNC_VIBRATO_Y          LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_SYNC_Y, 0)
#define LAYOUT_CART_SYNC_TREMOLO_Y          LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_SYNC_Y, 1)
#define LAYOUT_CART_SYNC_CHORUS_Y           LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_SYNC_Y, 2)
#define LAYOUT_CART_SYNC_OSC_Y              LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_SYNC_Y, 3)

#define LAYOUT_CART_HEADER_SENSITIVITY_X    LAYOUT_CART_WIDE_COLUMN_2_CENTER_X
#define LAYOUT_CART_HEADER_SENSITIVITY_Y    LAYOUT_CART_HEADER_SYNC_Y

#define LAYOUT_CART_SENSITIVITY_VIBRATO_Y   LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_SENSITIVITY_Y, 0)
#define LAYOUT_CART_SENSITIVITY_TREMOLO_Y   LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_SENSITIVITY_Y, 1)
#define LAYOUT_CART_SENSITIVITY_CHORUS_Y    LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_SENSITIVITY_Y, 2)
#define LAYOUT_CART_SENSITIVITY_BOOST_Y     LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_SENSITIVITY_Y, 3)
#define LAYOUT_CART_SENSITIVITY_VELOCITY_Y  LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_SENSITIVITY_Y, 4)

#define LAYOUT_CART_HEADER_PITCH_ENV_X      LAYOUT_CART_WIDE_COLUMN_3_CENTER_X
#define LAYOUT_CART_HEADER_PITCH_ENV_Y      LAYOUT_CART_HEADER_SYNC_Y

#define LAYOUT_CART_PITCH_ENV_ATTACK_Y      LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_PITCH_ENV_Y, 0)
#define LAYOUT_CART_PITCH_ENV_DECAY_Y       LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_PITCH_ENV_Y, 1)
#define LAYOUT_CART_PITCH_ENV_RELEASE_Y     LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_PITCH_ENV_Y, 2)
#define LAYOUT_CART_PITCH_ENV_MAXIMUM_Y     LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_PITCH_ENV_Y, 3)
#define LAYOUT_CART_PITCH_ENV_FINALE_Y      LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_PITCH_ENV_Y, 4)

#define LAYOUT_CART_HEADER_ARPEGGIO_X       LAYOUT_CART_WIDE_COLUMN_1_CENTER_X
#define LAYOUT_CART_HEADER_ARPEGGIO_Y       LAYOUT_CART_COMPUTE_NEXT_HEADER_Y(LAYOUT_CART_HEADER_SYNC_Y, 3)

#define LAYOUT_CART_ARPEGGIO_MODE_Y         LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ARPEGGIO_Y, 0)
#define LAYOUT_CART_ARPEGGIO_PATTERN_Y      LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ARPEGGIO_Y, 1)
#define LAYOUT_CART_ARPEGGIO_OCTAVES_Y      LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ARPEGGIO_Y, 2)
#define LAYOUT_CART_ARPEGGIO_SPEED_Y        LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_ARPEGGIO_Y, 3)

#define LAYOUT_CART_HEADER_PORTAMENTO_X     LAYOUT_CART_WIDE_COLUMN_2_CENTER_X
#define LAYOUT_CART_HEADER_PORTAMENTO_Y     LAYOUT_CART_COMPUTE_NEXT_HEADER_Y(LAYOUT_CART_HEADER_SENSITIVITY_Y, 4)

#define LAYOUT_CART_PORTAMENTO_MODE_Y       LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_PORTAMENTO_Y, 0)
#define LAYOUT_CART_PORTAMENTO_LEGATO_Y     LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_PORTAMENTO_Y, 1)
#define LAYOUT_CART_PORTAMENTO_SPEED_Y      LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_PORTAMENTO_Y, 2)

#define LAYOUT_CART_HEADER_PITCH_WHEEL_X    LAYOUT_CART_WIDE_COLUMN_3_CENTER_X
#define LAYOUT_CART_HEADER_PITCH_WHEEL_Y    LAYOUT_CART_HEADER_PORTAMENTO_Y

#define LAYOUT_CART_PITCH_WHEEL_MODE_Y      LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_PITCH_WHEEL_Y, 0)
#define LAYOUT_CART_PITCH_WHEEL_RANGE_Y     LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_PITCH_WHEEL_Y, 1)

#define LAYOUT_CART_HEADER_TREMOLO_ROUTING_X      LAYOUT_CART_WIDE_COLUMN_1_CENTER_X
#define LAYOUT_CART_HEADER_TREMOLO_ROUTING_Y      LAYOUT_CART_COMPUTE_NEXT_HEADER_Y(LAYOUT_CART_HEADER_ARPEGGIO_Y, 3)

#define LAYOUT_CART_TREMOLO_ROUTING_ENV_1_Y       LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_TREMOLO_ROUTING_Y, 0)
#define LAYOUT_CART_TREMOLO_ROUTING_ENV_2_Y       LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_TREMOLO_ROUTING_Y, 1)
#define LAYOUT_CART_TREMOLO_ROUTING_ENV_3_Y       LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_TREMOLO_ROUTING_Y, 2)

#define LAYOUT_CART_HEADER_BOOST_ROUTING_X        LAYOUT_CART_WIDE_COLUMN_2_CENTER_X
#define LAYOUT_CART_HEADER_BOOST_ROUTING_Y        LAYOUT_CART_HEADER_TREMOLO_ROUTING_Y

#define LAYOUT_CART_BOOST_ROUTING_ENV_1_Y         LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_BOOST_ROUTING_Y, 0)
#define LAYOUT_CART_BOOST_ROUTING_ENV_2_Y         LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_BOOST_ROUTING_Y, 1)
#define LAYOUT_CART_BOOST_ROUTING_ENV_3_Y         LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_BOOST_ROUTING_Y, 2)

#define LAYOUT_CART_HEADER_VELOCITY_ROUTING_X     LAYOUT_CART_WIDE_COLUMN_3_CENTER_X
#define LAYOUT_CART_HEADER_VELOCITY_ROUTING_Y     LAYOUT_CART_HEADER_TREMOLO_ROUTING_Y

#define LAYOUT_CART_VELOCITY_ROUTING_ENV_1_Y      LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_VELOCITY_ROUTING_Y, 0)
#define LAYOUT_CART_VELOCITY_ROUTING_ENV_2_Y      LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_VELOCITY_ROUTING_Y, 1)
#define LAYOUT_CART_VELOCITY_ROUTING_ENV_3_Y      LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_VELOCITY_ROUTING_Y, 2)

#define LAYOUT_CART_HEADER_MOD_WHEEL_ROUTING_X    LAYOUT_CART_WIDE_COLUMN_1_CENTER_X
#define LAYOUT_CART_HEADER_MOD_WHEEL_ROUTING_Y    LAYOUT_CART_COMPUTE_NEXT_HEADER_Y(LAYOUT_CART_HEADER_TREMOLO_ROUTING_Y, 2)

#define LAYOUT_CART_MOD_WHEEL_ROUTING_VIBRATO_Y   LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_MOD_WHEEL_ROUTING_Y, 0)
#define LAYOUT_CART_MOD_WHEEL_ROUTING_TREMOLO_Y   LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_MOD_WHEEL_ROUTING_Y, 1)
#define LAYOUT_CART_MOD_WHEEL_ROUTING_BOOST_Y     LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_MOD_WHEEL_ROUTING_Y, 2)
#define LAYOUT_CART_MOD_WHEEL_ROUTING_CHORUS_Y    LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_MOD_WHEEL_ROUTING_Y, 3)

#define LAYOUT_CART_HEADER_AFTERTOUCH_ROUTING_X   LAYOUT_CART_WIDE_COLUMN_2_CENTER_X
#define LAYOUT_CART_HEADER_AFTERTOUCH_ROUTING_Y   LAYOUT_CART_HEADER_MOD_WHEEL_ROUTING_Y

#define LAYOUT_CART_AFTERTOUCH_ROUTING_VIBRATO_Y  LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_AFTERTOUCH_ROUTING_Y, 0)
#define LAYOUT_CART_AFTERTOUCH_ROUTING_TREMOLO_Y  LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_AFTERTOUCH_ROUTING_Y, 1)
#define LAYOUT_CART_AFTERTOUCH_ROUTING_BOOST_Y    LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_AFTERTOUCH_ROUTING_Y, 2)
#define LAYOUT_CART_AFTERTOUCH_ROUTING_CHORUS_Y   LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_AFTERTOUCH_ROUTING_Y, 3)

#define LAYOUT_CART_HEADER_EXP_PEDAL_ROUTING_X    LAYOUT_CART_WIDE_COLUMN_3_CENTER_X
#define LAYOUT_CART_HEADER_EXP_PEDAL_ROUTING_Y    LAYOUT_CART_HEADER_MOD_WHEEL_ROUTING_Y

#define LAYOUT_CART_EXP_PEDAL_ROUTING_VIBRATO_Y   LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_EXP_PEDAL_ROUTING_Y, 0)
#define LAYOUT_CART_EXP_PEDAL_ROUTING_TREMOLO_Y   LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_EXP_PEDAL_ROUTING_Y, 1)
#define LAYOUT_CART_EXP_PEDAL_ROUTING_BOOST_Y     LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_EXP_PEDAL_ROUTING_Y, 2)
#define LAYOUT_CART_EXP_PEDAL_ROUTING_CHORUS_Y    LAYOUT_CART_COMPUTE_PARAM_Y(LAYOUT_CART_HEADER_EXP_PEDAL_ROUTING_Y, 3)

/* audition panel parameters */
#define LAYOUT_CART_AUDITION_OCTAVE_X       LAYOUT_CART_WIDE_COLUMN_1_CENTER_X
#define LAYOUT_CART_AUDITION_OCTAVE_Y       90

#define LAYOUT_CART_AUDITION_VELOCITY_X     LAYOUT_CART_WIDE_COLUMN_2_CENTER_X
#define LAYOUT_CART_AUDITION_VELOCITY_Y     LAYOUT_CART_AUDITION_OCTAVE_Y

#define LAYOUT_CART_AUDITION_PITCH_WHEEL_X  LAYOUT_CART_WIDE_COLUMN_3_CENTER_X
#define LAYOUT_CART_AUDITION_PITCH_WHEEL_Y  LAYOUT_CART_AUDITION_OCTAVE_Y

#define LAYOUT_CART_AUDITION_MOD_WHEEL_X    LAYOUT_CART_WIDE_COLUMN_1_CENTER_X
#define LAYOUT_CART_AUDITION_MOD_WHEEL_Y    102

#define LAYOUT_CART_AUDITION_AFTERTOUCH_X   LAYOUT_CART_WIDE_COLUMN_2_CENTER_X
#define LAYOUT_CART_AUDITION_AFTERTOUCH_Y   LAYOUT_CART_AUDITION_MOD_WHEEL_Y

#define LAYOUT_CART_AUDITION_EXP_PEDAL_X    LAYOUT_CART_WIDE_COLUMN_3_CENTER_X
#define LAYOUT_CART_AUDITION_EXP_PEDAL_Y    LAYOUT_CART_AUDITION_MOD_WHEEL_Y

/* parameter types */
#define LAYOUT_CART_PARAM_SLIDER_NAME_X     -48
#define LAYOUT_CART_PARAM_SLIDER_VALUE_X    -24
#define LAYOUT_CART_PARAM_SLIDER_TRACK_X     24
#define LAYOUT_CART_PARAM_SLIDER_WIDTH        9

#define LAYOUT_CART_PARAM_ARROWS_NAME_X     -48
#define LAYOUT_CART_PARAM_ARROWS_LEFT_X     -24
#define LAYOUT_CART_PARAM_ARROWS_VALUE_X     16
#define LAYOUT_CART_PARAM_ARROWS_RIGHT_X     56

#define LAYOUT_CART_PARAM_RADIO_NAME_X      -48
#define LAYOUT_CART_PARAM_RADIO_BUTTON_X    -24
#define LAYOUT_CART_PARAM_RADIO_VALUE_X      16

#define LAYOUT_CART_PARAM_EDITABLE_NAME_X   -48
#define LAYOUT_CART_PARAM_EDITABLE_VALUE_X  -20
#define LAYOUT_CART_PARAM_EDITABLE_LEFT_X    -4
#define LAYOUT_CART_PARAM_EDITABLE_FIELD_X   56
#define LAYOUT_CART_PARAM_EDITABLE_RIGHT_X  116

/* browser buttons, headers, and filenames */
#define LAYOUT_BROWSER_MAIN_HEADER_X       -4
#define LAYOUT_BROWSER_MAIN_HEADER_Y      -52

#define LAYOUT_BROWSER_DIR_PANEL_NAME_X  -176
#define LAYOUT_BROWSER_DIR_PANEL_NAME_Y   -36

#define LAYOUT_BROWSER_FILE_PANEL_NAME_X -176
#define LAYOUT_BROWSER_FILE_PANEL_NAME_Y  100

#define LAYOUT_BROWSER_LIST_NAME_X       -188
#define LAYOUT_BROWSER_LIST_NAME_START_Y  -20
#define LAYOUT_BROWSER_LIST_NAME_STEP_Y    12

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

  int type;

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
short int layout_reset_cart_button_states();

#endif
