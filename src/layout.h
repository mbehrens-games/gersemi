/*******************************************************************************
** layout.h (screen layout widget positions)
*******************************************************************************/

#ifndef LAYOUT_H
#define LAYOUT_H

#include "cart.h"

/* kinds */
enum
{
  LAYOUT_WIDGET_KIND_SLIDER = 0, 
  LAYOUT_WIDGET_KIND_ARROWS, 
  LAYOUT_WIDGET_KIND_RADIO, 
  LAYOUT_NUM_WIDGET_KINDS
};

/* top bar */
enum
{
  LAYOUT_TOP_BAR_REGION_TOP_BAR = 0, 
  LAYOUT_NUM_TOP_BAR_REGIONS 
};

enum
{
  LAYOUT_TOP_BAR_BUTTON_CART = 0, 
  LAYOUT_TOP_BAR_BUTTON_INSTRUMENTS, 
  LAYOUT_TOP_BAR_BUTTON_SONG, 
  LAYOUT_TOP_BAR_BUTTON_MIXER, 
  LAYOUT_TOP_BAR_BUTTON_SOUND_FX, 
  LAYOUT_TOP_BAR_BUTTON_DPCM, 
  LAYOUT_NUM_TOP_BAR_BUTTONS 
};

/* cart screen */
enum
{
  LAYOUT_CART_REGION_MAIN_AREA = 0, 
  LAYOUT_CART_REGION_SCROLLBAR, 
  LAYOUT_CART_REGION_AUDITION_PANEL, 
  LAYOUT_NUM_CART_REGIONS 
};

enum
{
  LAYOUT_CART_HEADER_OSC_1 = 0, 
  LAYOUT_CART_HEADER_OSC_2, 
  LAYOUT_CART_HEADER_OSC_3, 
  LAYOUT_CART_HEADER_OSC_4, 
  LAYOUT_CART_HEADER_ENV_1, 
  LAYOUT_CART_HEADER_ENV_2, 
  LAYOUT_CART_HEADER_ENV_3, 
  LAYOUT_CART_HEADER_ENV_4, 
  LAYOUT_CART_HEADER_LFO, 
  LAYOUT_CART_HEADER_VIBRATO, 
  LAYOUT_CART_HEADER_TREMOLO, 
  LAYOUT_CART_HEADER_VELOCITY, 
  LAYOUT_CART_HEADER_BOOST, 
  LAYOUT_CART_HEADER_FILTERS, 
  LAYOUT_NUM_CART_HEADERS 
};

#define LAYOUT_OVERSCAN_WIDTH   400
#define LAYOUT_OVERSCAN_HEIGHT  224

#define LAYOUT_TEXT_MAX_NAME_SIZE   (11 + 1)
#define LAYOUT_BUTTON_MAX_NAME_SIZE (11 + 1)

#define LAYOUT_SCROLLED_ELEMENT_IS_IN_REGION(e, rgn, scroll)                              \
  ( (e->x          >= rgn->x) && ((e->x + 8 * e->w)          <= (rgn->x + 8 * rgn->w)) && \
    (e->y - scroll >= rgn->y) && ((e->y + 8 * e->h - scroll) <= (rgn->y + 8 * rgn->h)))

/* vertical scrollbar amounts */
#define LAYOUT_CART_MAX_SCROLL_AMOUNT         160
#define LAYOUT_INSTRUMENTS_MAX_SCROLL_AMOUNT    0
#define LAYOUT_SONG_MAX_SCROLL_AMOUNT           0
#define LAYOUT_MIXER_MAX_SCROLL_AMOUNT          0
#define LAYOUT_SOUND_FX_MAX_SCROLL_AMOUNT       0
#define LAYOUT_DPCM_MAX_SCROLL_AMOUNT           0

/* parameter types */
#define LAYOUT_PARAM_SLIDER_VALUE_X     (0 * 8 + 0)
#define LAYOUT_PARAM_SLIDER_TRACK_X     (3 * 8 + 0)
#define LAYOUT_PARAM_SLIDER_VALUE_WIDTH 3
#define LAYOUT_PARAM_SLIDER_TRACK_WIDTH 5

#define LAYOUT_PARAM_ARROWS_LEFT_X      (0 * 8 + 4)
#define LAYOUT_PARAM_ARROWS_VALUE_X     (2 * 8 + 0)
#define LAYOUT_PARAM_ARROWS_RIGHT_X     (6 * 8 + 4)
#define LAYOUT_PARAM_ARROWS_VALUE_WIDTH 4

#define LAYOUT_PARAM_RADIO_BUTTON_X     (0 * 8 + 4)
#define LAYOUT_PARAM_RADIO_VALUE_X      (2 * 8 + 4)
#define LAYOUT_PARAM_RADIO_VALUE_WIDTH  5

typedef struct layout_region
{
  short int x;
  short int y;
  short int w;
  short int h;
} layout_region;

typedef struct layout_text
{
  short int x;
  short int y;
  short int w;
  short int h;
  char      name[LAYOUT_TEXT_MAX_NAME_SIZE];
} layout_text;

typedef struct layout_button
{
  short int x;
  short int y;
  short int w;
  short int h;
  char      name[LAYOUT_BUTTON_MAX_NAME_SIZE];
} layout_button;

typedef struct layout_widget
{
  short int x;
  short int y;
  short int w;
  short int h;
  short int kind;
} layout_widget;

extern layout_region  G_layout_top_bar_regions[LAYOUT_NUM_TOP_BAR_REGIONS];
extern layout_button  G_layout_top_bar_buttons[LAYOUT_NUM_TOP_BAR_BUTTONS];

extern layout_region  G_layout_cart_regions[LAYOUT_NUM_CART_REGIONS];
extern layout_text    G_layout_cart_headers[LAYOUT_NUM_CART_HEADERS];
extern layout_text    G_layout_cart_param_names[PATCH_NUM_PARAMS];
extern layout_widget  G_layout_cart_param_widgets[PATCH_NUM_PARAMS];

/* function declarations */
short int layout_generate_tables();

short int layout_reset_top_bar_states();

short int layout_reset_cart_states();

#endif
