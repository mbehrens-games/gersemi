/*******************************************************************************
** vball.c (vbo population functions - all)
*******************************************************************************/

#include <glad/glad.h>

#include <stdio.h>
#include <stdlib.h>

#include "bank.h"
#include "cart.h"
#include "controls.h"
#include "global.h"
#include "graphics.h"
#include "layout.h"
#include "midicont.h"
#include "palette.h"
#include "sequence.h"
#include "texture.h"
#include "vball.h"

#define VB_ALL_BACKGROUND_TILE_SIZE 8

#define VB_ALL_BACKGROUND_WIDTH   (GRAPHICS_OVERSCAN_WIDTH / VB_ALL_BACKGROUND_TILE_SIZE)
#define VB_ALL_BACKGROUND_HEIGHT  (GRAPHICS_OVERSCAN_HEIGHT / VB_ALL_BACKGROUND_TILE_SIZE)

#define VB_ALL_PALETTE_1    0
#define VB_ALL_PALETTE_2    1
#define VB_ALL_PALETTE_3    4
#define VB_ALL_PALETTE_GRAY 6

#define VB_ALL_MAX_TEXT_SIZE 16

enum
{
  VB_ALL_TEXT_ALIGN_LEFT = 0,
  VB_ALL_TEXT_ALIGN_CENTER,
  VB_ALL_TEXT_ALIGN_RIGHT
};

/* tiles */
#define VB_ALL_TILE_LAYER_INDEX(layer)                                     \
  G_tile_layer_index[GRAPHICS_TILE_LAYER_##layer]

#define VB_ALL_TILE_LAYER_COUNT(layer)                                     \
  G_tile_layer_count[GRAPHICS_TILE_LAYER_##layer]

#define VB_ALL_TILE_LAYER_MAX(layer)                                       \
  G_tile_layer_max[GRAPHICS_TILE_LAYER_##layer]

/* position is the center, width & height are in 8x8 cells */
#define VB_ALL_ADD_TILE_TO_VERTEX_BUFFER(pos_x, pos_y, width, height, layer)                                  \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_TILES][12 * tile_index +  0] = pos_x;                     \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_TILES][12 * tile_index +  1] = pos_y;                     \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_TILES][12 * tile_index +  2] = GRAPHICS_Z_LEVEL_##layer;  \
                                                                                                              \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_TILES][12 * tile_index +  3] = pos_x + 8 * width;         \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_TILES][12 * tile_index +  4] = pos_y;                     \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_TILES][12 * tile_index +  5] = GRAPHICS_Z_LEVEL_##layer;  \
                                                                                                              \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_TILES][12 * tile_index +  6] = pos_x;                     \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_TILES][12 * tile_index +  7] = pos_y + 8 * height;        \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_TILES][12 * tile_index +  8] = GRAPHICS_Z_LEVEL_##layer;  \
                                                                                                              \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_TILES][12 * tile_index +  9] = pos_x + 8 * width;         \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_TILES][12 * tile_index + 10] = pos_y + 8 * height;        \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_TILES][12 * tile_index + 11] = GRAPHICS_Z_LEVEL_##layer;

/* cell_x and cell_y are the top left corner, width & height are in 8x8 cells */
#define VB_ALL_ADD_TILE_TO_TEXTURE_COORD_BUFFER(cell_x, cell_y, width, height)                                                                  \
  G_overscan_tex_coord_buffer_data[GRAPHICS_BUFFER_SET_TILES][8 * tile_index + 0] = G_texture_coord_table[cell_x];                              \
  G_overscan_tex_coord_buffer_data[GRAPHICS_BUFFER_SET_TILES][8 * tile_index + 1] = G_texture_coord_table[TEXTURE_NUM_CELLS - cell_y];          \
                                                                                                                                                \
  G_overscan_tex_coord_buffer_data[GRAPHICS_BUFFER_SET_TILES][8 * tile_index + 2] = G_texture_coord_table[cell_x + width];                      \
  G_overscan_tex_coord_buffer_data[GRAPHICS_BUFFER_SET_TILES][8 * tile_index + 3] = G_texture_coord_table[TEXTURE_NUM_CELLS - cell_y];          \
                                                                                                                                                \
  G_overscan_tex_coord_buffer_data[GRAPHICS_BUFFER_SET_TILES][8 * tile_index + 4] = G_texture_coord_table[cell_x];                              \
  G_overscan_tex_coord_buffer_data[GRAPHICS_BUFFER_SET_TILES][8 * tile_index + 5] = G_texture_coord_table[TEXTURE_NUM_CELLS - height - cell_y]; \
                                                                                                                                                \
  G_overscan_tex_coord_buffer_data[GRAPHICS_BUFFER_SET_TILES][8 * tile_index + 6] = G_texture_coord_table[cell_x + width];                      \
  G_overscan_tex_coord_buffer_data[GRAPHICS_BUFFER_SET_TILES][8 * tile_index + 7] = G_texture_coord_table[TEXTURE_NUM_CELLS - height - cell_y];

#define VB_ALL_ADD_TILE_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)                                                                   \
  G_overscan_pal_coord_buffer_data[GRAPHICS_BUFFER_SET_TILES][8 * tile_index + 0] = G_lighting_coord_table[PALETTE_BASE_LEVEL + lighting];  \
  G_overscan_pal_coord_buffer_data[GRAPHICS_BUFFER_SET_TILES][8 * tile_index + 1] = G_palette_coord_table[palette];                         \
                                                                                                                                            \
  G_overscan_pal_coord_buffer_data[GRAPHICS_BUFFER_SET_TILES][8 * tile_index + 2] = G_lighting_coord_table[PALETTE_BASE_LEVEL + lighting];  \
  G_overscan_pal_coord_buffer_data[GRAPHICS_BUFFER_SET_TILES][8 * tile_index + 3] = G_palette_coord_table[palette];                         \
                                                                                                                                            \
  G_overscan_pal_coord_buffer_data[GRAPHICS_BUFFER_SET_TILES][8 * tile_index + 4] = G_lighting_coord_table[PALETTE_BASE_LEVEL + lighting];  \
  G_overscan_pal_coord_buffer_data[GRAPHICS_BUFFER_SET_TILES][8 * tile_index + 5] = G_palette_coord_table[palette];                         \
                                                                                                                                            \
  G_overscan_pal_coord_buffer_data[GRAPHICS_BUFFER_SET_TILES][8 * tile_index + 6] = G_lighting_coord_table[PALETTE_BASE_LEVEL + lighting];  \
  G_overscan_pal_coord_buffer_data[GRAPHICS_BUFFER_SET_TILES][8 * tile_index + 7] = G_palette_coord_table[palette];

#define VB_ALL_ADD_TILE_TO_ELEMENT_BUFFER()                                                         \
  G_overscan_index_buffer_data[GRAPHICS_BUFFER_SET_TILES][6 * tile_index + 0] = 4 * tile_index + 0; \
  G_overscan_index_buffer_data[GRAPHICS_BUFFER_SET_TILES][6 * tile_index + 1] = 4 * tile_index + 2; \
  G_overscan_index_buffer_data[GRAPHICS_BUFFER_SET_TILES][6 * tile_index + 2] = 4 * tile_index + 1; \
                                                                                                    \
  G_overscan_index_buffer_data[GRAPHICS_BUFFER_SET_TILES][6 * tile_index + 3] = 4 * tile_index + 1; \
  G_overscan_index_buffer_data[GRAPHICS_BUFFER_SET_TILES][6 * tile_index + 4] = 4 * tile_index + 2; \
  G_overscan_index_buffer_data[GRAPHICS_BUFFER_SET_TILES][6 * tile_index + 5] = 4 * tile_index + 3;

#define VB_ALL_ADD_TILE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, palette, layer)  \
  if (VB_ALL_TILE_LAYER_INDEX(layer) < VB_ALL_TILE_LAYER_MAX(layer))              \
  {                                                                               \
    tile_index =  VB_ALL_TILE_LAYER_INDEX(layer) +                                \
                  VB_ALL_TILE_LAYER_COUNT(layer);                                 \
                                                                                  \
    VB_ALL_ADD_TILE_TO_VERTEX_BUFFER(pos_x, pos_y, 1, 1, layer)                   \
    VB_ALL_ADD_TILE_TO_TEXTURE_COORD_BUFFER(cell_x, cell_y, 1, 1)                 \
    VB_ALL_ADD_TILE_TO_LIGHTING_AND_PALETTE_BUFFER(0, palette)                    \
    VB_ALL_ADD_TILE_TO_ELEMENT_BUFFER()                                           \
                                                                                  \
    VB_ALL_TILE_LAYER_COUNT(layer) += 1;                                          \
  }

#define VB_ALL_UPDATE_TILES_IN_VBOS(layer)                                                                            \
  glBindBuffer(GL_ARRAY_BUFFER, G_overscan_vertex_buffer_id[GRAPHICS_BUFFER_SET_TILES]);                              \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                                    \
                  VB_ALL_TILE_LAYER_INDEX(layer) * 12 * sizeof(GLfloat),                                              \
                  VB_ALL_TILE_LAYER_COUNT(layer) * 12 * sizeof(GLfloat),                                              \
                  &G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_TILES][VB_ALL_TILE_LAYER_INDEX(layer) * 12]);    \
                                                                                                                      \
  glBindBuffer(GL_ARRAY_BUFFER, G_overscan_tex_coord_buffer_id[GRAPHICS_BUFFER_SET_TILES]);                           \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                                    \
                  VB_ALL_TILE_LAYER_INDEX(layer) * 8 * sizeof(GLfloat),                                               \
                  VB_ALL_TILE_LAYER_COUNT(layer) * 8 * sizeof(GLfloat),                                               \
                  &G_overscan_tex_coord_buffer_data[GRAPHICS_BUFFER_SET_TILES][VB_ALL_TILE_LAYER_INDEX(layer) * 8]);  \
                                                                                                                      \
  glBindBuffer(GL_ARRAY_BUFFER, G_overscan_pal_coord_buffer_id[GRAPHICS_BUFFER_SET_TILES]);                           \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                                    \
                  VB_ALL_TILE_LAYER_INDEX(layer) * 8 * sizeof(GLfloat),                                               \
                  VB_ALL_TILE_LAYER_COUNT(layer) * 8 * sizeof(GLfloat),                                               \
                  &G_overscan_pal_coord_buffer_data[GRAPHICS_BUFFER_SET_TILES][VB_ALL_TILE_LAYER_INDEX(layer) * 8]);  \
                                                                                                                      \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_overscan_index_buffer_id[GRAPHICS_BUFFER_SET_TILES]);                       \
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,                                                                            \
                  VB_ALL_TILE_LAYER_INDEX(layer) * 6 * sizeof(unsigned short),                                        \
                  VB_ALL_TILE_LAYER_COUNT(layer) * 6 * sizeof(unsigned short),                                        \
                  &G_overscan_index_buffer_data[GRAPHICS_BUFFER_SET_TILES][VB_ALL_TILE_LAYER_INDEX(layer) * 6]);

/* sprites */
#define VB_ALL_SPRITE_LAYER_INDEX(layer)                                     \
  G_sprite_layer_index[GRAPHICS_SPRITE_LAYER_##layer]

#define VB_ALL_SPRITE_LAYER_COUNT(layer)                                     \
  G_sprite_layer_count[GRAPHICS_SPRITE_LAYER_##layer]

#define VB_ALL_SPRITE_LAYER_MAX(layer)                                       \
  G_sprite_layer_max[GRAPHICS_SPRITE_LAYER_##layer]

/* position is the center, width & height are in 8x8 cells */
#define VB_ALL_ADD_SPRITE_TO_VERTEX_BUFFER(pos_x, pos_y, width, height, layer)                                    \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][12 * sprite_index +  0] = pos_x;                     \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][12 * sprite_index +  1] = pos_y;                     \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][12 * sprite_index +  2] = GRAPHICS_Z_LEVEL_##layer;  \
                                                                                                                  \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][12 * sprite_index +  3] = pos_x + 8 * width;         \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][12 * sprite_index +  4] = pos_y;                     \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][12 * sprite_index +  5] = GRAPHICS_Z_LEVEL_##layer;  \
                                                                                                                  \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][12 * sprite_index +  6] = pos_x;                     \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][12 * sprite_index +  7] = pos_y + 8 * height;        \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][12 * sprite_index +  8] = GRAPHICS_Z_LEVEL_##layer;  \
                                                                                                                  \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][12 * sprite_index +  9] = pos_x + 8 * width;         \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][12 * sprite_index + 10] = pos_y + 8 * height;        \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][12 * sprite_index + 11] = GRAPHICS_Z_LEVEL_##layer;

/* cell_x and cell_y are the top left corner, width & height are in 8x8 cells */
#define VB_ALL_ADD_SPRITE_TO_TEXTURE_COORD_BUFFER(cell_x, cell_y, width, height)                                                                    \
  G_overscan_tex_coord_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][8 * sprite_index + 0] = G_texture_coord_table[cell_x];                              \
  G_overscan_tex_coord_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][8 * sprite_index + 1] = G_texture_coord_table[TEXTURE_NUM_CELLS - cell_y];          \
                                                                                                                                                    \
  G_overscan_tex_coord_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][8 * sprite_index + 2] = G_texture_coord_table[cell_x + width];                      \
  G_overscan_tex_coord_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][8 * sprite_index + 3] = G_texture_coord_table[TEXTURE_NUM_CELLS - cell_y];          \
                                                                                                                                                    \
  G_overscan_tex_coord_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][8 * sprite_index + 4] = G_texture_coord_table[cell_x];                              \
  G_overscan_tex_coord_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][8 * sprite_index + 5] = G_texture_coord_table[TEXTURE_NUM_CELLS - height - cell_y]; \
                                                                                                                                                    \
  G_overscan_tex_coord_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][8 * sprite_index + 6] = G_texture_coord_table[cell_x + width];                      \
  G_overscan_tex_coord_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][8 * sprite_index + 7] = G_texture_coord_table[TEXTURE_NUM_CELLS - height - cell_y];

#define VB_ALL_ADD_SPRITE_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)                                                                     \
  G_overscan_pal_coord_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][8 * sprite_index + 0] = G_lighting_coord_table[PALETTE_BASE_LEVEL + lighting];  \
  G_overscan_pal_coord_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][8 * sprite_index + 1] = G_palette_coord_table[palette];                         \
                                                                                                                                                \
  G_overscan_pal_coord_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][8 * sprite_index + 2] = G_lighting_coord_table[PALETTE_BASE_LEVEL + lighting];  \
  G_overscan_pal_coord_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][8 * sprite_index + 3] = G_palette_coord_table[palette];                         \
                                                                                                                                                \
  G_overscan_pal_coord_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][8 * sprite_index + 4] = G_lighting_coord_table[PALETTE_BASE_LEVEL + lighting];  \
  G_overscan_pal_coord_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][8 * sprite_index + 5] = G_palette_coord_table[palette];                         \
                                                                                                                                                \
  G_overscan_pal_coord_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][8 * sprite_index + 6] = G_lighting_coord_table[PALETTE_BASE_LEVEL + lighting];  \
  G_overscan_pal_coord_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][8 * sprite_index + 7] = G_palette_coord_table[palette];

#define VB_ALL_ADD_SPRITE_TO_ELEMENT_BUFFER()                                                             \
  G_overscan_index_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][6 * sprite_index + 0] = 4 * sprite_index + 0; \
  G_overscan_index_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][6 * sprite_index + 1] = 4 * sprite_index + 2; \
  G_overscan_index_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][6 * sprite_index + 2] = 4 * sprite_index + 1; \
                                                                                                          \
  G_overscan_index_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][6 * sprite_index + 3] = 4 * sprite_index + 1; \
  G_overscan_index_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][6 * sprite_index + 4] = 4 * sprite_index + 2; \
  G_overscan_index_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][6 * sprite_index + 5] = 4 * sprite_index + 3;

#define VB_ALL_ADD_SPRITE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, width, height, palette, layer) \
  if (VB_ALL_SPRITE_LAYER_COUNT(layer) < VB_ALL_SPRITE_LAYER_MAX(layer))                          \
  {                                                                                               \
    sprite_index =  VB_ALL_SPRITE_LAYER_INDEX(layer) +                                            \
                    VB_ALL_SPRITE_LAYER_COUNT(layer);                                             \
                                                                                                  \
    VB_ALL_ADD_SPRITE_TO_VERTEX_BUFFER(pos_x, pos_y, width, height, layer)                        \
    VB_ALL_ADD_SPRITE_TO_TEXTURE_COORD_BUFFER(cell_x, cell_y, width, height)                      \
    VB_ALL_ADD_SPRITE_TO_LIGHTING_AND_PALETTE_BUFFER(0, palette)                                  \
    VB_ALL_ADD_SPRITE_TO_ELEMENT_BUFFER()                                                         \
                                                                                                  \
    VB_ALL_SPRITE_LAYER_COUNT(layer) += 1;                                                        \
  }

#define VB_ALL_UPDATE_SPRITES_IN_VBOS(layer)                                                                              \
  glBindBuffer(GL_ARRAY_BUFFER, G_overscan_vertex_buffer_id[GRAPHICS_BUFFER_SET_SPRITES]);                                \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                                        \
                  VB_ALL_SPRITE_LAYER_INDEX(layer) * 12 * sizeof(GLfloat),                                                \
                  VB_ALL_SPRITE_LAYER_COUNT(layer) * 12 * sizeof(GLfloat),                                                \
                  &G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][VB_ALL_SPRITE_LAYER_INDEX(layer) * 12]);    \
                                                                                                                          \
  glBindBuffer(GL_ARRAY_BUFFER, G_overscan_tex_coord_buffer_id[GRAPHICS_BUFFER_SET_SPRITES]);                             \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                                        \
                  VB_ALL_SPRITE_LAYER_INDEX(layer) * 8 * sizeof(GLfloat),                                                 \
                  VB_ALL_SPRITE_LAYER_COUNT(layer) * 8 * sizeof(GLfloat),                                                 \
                  &G_overscan_tex_coord_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][VB_ALL_SPRITE_LAYER_INDEX(layer) * 8]);  \
                                                                                                                          \
  glBindBuffer(GL_ARRAY_BUFFER, G_overscan_pal_coord_buffer_id[GRAPHICS_BUFFER_SET_SPRITES]);                             \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                                        \
                  VB_ALL_SPRITE_LAYER_INDEX(layer) * 8 * sizeof(GLfloat),                                                 \
                  VB_ALL_SPRITE_LAYER_COUNT(layer) * 8 * sizeof(GLfloat),                                                 \
                  &G_overscan_pal_coord_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][VB_ALL_SPRITE_LAYER_INDEX(layer) * 8]);  \
                                                                                                                          \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_overscan_index_buffer_id[GRAPHICS_BUFFER_SET_SPRITES]);                         \
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,                                                                                \
                  VB_ALL_SPRITE_LAYER_INDEX(layer) * 6 * sizeof(unsigned short),                                          \
                  VB_ALL_SPRITE_LAYER_COUNT(layer) * 6 * sizeof(unsigned short),                                          \
                  &G_overscan_index_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][VB_ALL_SPRITE_LAYER_INDEX(layer) * 6]);

/* common edit tables */
static char S_common_edit_1_to_128_values[128][4] = 
  {   "1",   "2",   "3",   "4",   "5",   "6",   "7",   "8",  
      "9",  "10",  "11",  "12",  "13",  "14",  "15",  "16", 
     "17",  "18",  "19",  "20",  "21",  "22",  "23",  "24", 
     "25",  "26",  "27",  "28",  "29",  "30",  "31",  "32", 
     "33",  "34",  "35",  "36",  "37",  "38",  "39",  "40", 
     "41",  "42",  "43",  "44",  "45",  "46",  "47",  "48", 
     "49",  "50",  "51",  "52",  "53",  "54",  "55",  "56", 
     "57",  "58",  "59",  "60",  "61",  "62",  "63",  "64", 
     "65",  "66",  "67",  "68",  "69",  "70",  "71",  "72", 
     "73",  "74",  "75",  "76",  "77",  "78",  "79",  "80", 
     "81",  "82",  "83",  "84",  "85",  "86",  "87",  "88", 
     "89",  "90",  "91",  "92",  "93",  "94",  "95",  "96", 
     "97",  "98",  "99", "100", "101", "102", "103", "104", 
    "105", "106", "107", "108", "109", "110", "111", "112", 
    "113", "114", "115", "116", "117", "118", "119", "120", 
    "121", "122", "123", "124", "125", "126", "127", "128"  
  };

static char S_common_edit_0_to_127_values[128][4] = 
  {   "0",   "1",   "2",   "3",   "4",   "5",   "6",   "7", 
      "8",   "9",  "10",  "11",  "12",  "13",  "14",  "15", 
     "16",  "17",  "18",  "19",  "20",  "21",  "22",  "23", 
     "24",  "25",  "26",  "27",  "28",  "29",  "30",  "31", 
     "32",  "33",  "34",  "35",  "36",  "37",  "38",  "39", 
     "40",  "41",  "42",  "43",  "44",  "45",  "46",  "47", 
     "48",  "49",  "50",  "51",  "52",  "53",  "54",  "55", 
     "56",  "57",  "58",  "59",  "60",  "61",  "62",  "63", 
     "64",  "65",  "66",  "67",  "68",  "69",  "70",  "71", 
     "72",  "73",  "74",  "75",  "76",  "77",  "78",  "79", 
     "80",  "81",  "82",  "83",  "84",  "85",  "86",  "87", 
     "88",  "89",  "90",  "91",  "92",  "93",  "94",  "95", 
     "96",  "97",  "98",  "99", "100", "101", "102", "103", 
    "104", "105", "106", "107", "108", "109", "110", "111", 
    "112", "113", "114", "115", "116", "117", "118", "119", 
    "120", "121", "122", "123", "124", "125", "126", "127" 
  };

static char S_patch_edit_legacy_keyscale_values[PATCH_NUM_LEGACY_KEYSCALE_VALS][9] = 
  { "Pitch", "Key" };

static char S_patch_edit_algorithm_values[PATCH_NUM_ALGORITHM_VALS][9] = 
  { "1>2>3", "(1+2)>3", "(1>2)+3", "1>(2+3)", "1+2+3" };

static char S_patch_edit_sync_values[PATCH_NUM_SYNC_VALS][9] = 
  { "Off", "On" };

static char S_patch_edit_osc_waveform_values[PATCH_NUM_OSC_WAVEFORM_VALS][9] = 
  {  "Sine", "Half", "Full", "Quarter" };

static char S_patch_edit_osc_freq_mode_values[PATCH_NUM_OSC_FREQ_MODE_VALS][9] = 
  { "Ratio", "Fixed" };

static char S_patch_edit_osc_note_values[PATCH_NUM_OSC_NOTE_VALS][9] = 
  { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

static char S_patch_edit_osc_detune_values[PATCH_NUM_OSC_DETUNE_VALS][4] = 
  { "-3", "-2", "-1", "0", "1", "2", "3" };

static char S_patch_edit_lfo_waveform_values[PATCH_NUM_LFO_WAVEFORM_VALS][9] = 
  { "Triangle", "Square", "Saw Up", "Saw Down" };

static char S_patch_edit_polarity_values[PATCH_NUM_POLARITY_VALS][9] = 
  { "Up/Down", "Up Only" };

static char S_patch_edit_am_mode_values[PATCH_NUM_AM_MODE_VALS][9] = 
  { "Carriers", "Mods" };

static char S_patch_edit_highpass_cutoff_values[PATCH_NUM_CUTOFF_VALS][4] = 
  { "A0", "A1", "A2", "A3" };

static char S_patch_edit_lowpass_cutoff_values[PATCH_NUM_CUTOFF_VALS][4] = 
  { "E7", "G7", "A7", "C8" };

static char S_audition_uni_wheel_values[MIDI_CONT_UNI_WHEEL_NUM_VALUES][4] = 
  {  "0",   "1",   "2",   "3",   "4",   "5",   "6",   "7",   "8", 
            "9",  "10",  "11",  "12",  "13",  "14",  "15",  "16", 
           "17",  "18",  "19",  "20",  "21",  "22",  "23",  "24", 
           "25",  "26",  "27",  "28",  "29",  "30",  "31",  "32", 
           "33",  "34",  "35",  "36",  "37",  "38",  "39",  "40", 
           "41",  "42",  "43",  "44",  "45",  "46",  "47",  "48", 
           "49",  "50",  "51",  "52",  "53",  "54",  "55",  "56", 
           "57",  "58",  "59",  "60",  "61",  "62",  "63",  "64", 
           "65",  "66",  "67",  "68",  "69",  "70",  "71",  "72", 
           "73",  "74",  "75",  "76",  "77",  "78",  "79",  "80", 
           "81",  "82",  "83",  "84",  "85",  "86",  "87",  "88", 
           "89",  "90",  "91",  "92",  "93",  "94",  "95",  "96", 
           "97",  "98",  "99", "100", "101", "102", "103", "104", 
          "105", "106", "107", "108", "109", "110", "111", "112", 
          "113", "114", "115", "116", "117", "118", "119", "120", 
          "121", "122", "123", "124", "125", "126", "127", "128" 
  };

/*******************************************************************************
** vb_all_load_background()
*******************************************************************************/
short int vb_all_load_background()
{
  int m;
  int n;

  int tile_index;

  int pos_x;
  int pos_y;

  int cell_x;
  int cell_y;

  int palette;

  /* reset tile vbo counts */
  for (m = 0; m < GRAPHICS_NUM_TILE_LAYERS; m++)
    G_tile_layer_count[m] = 0;

  /* draw the background */
  for (n = 0; n < VB_ALL_BACKGROUND_HEIGHT; n++)
  {
    for (m = 0; m < VB_ALL_BACKGROUND_WIDTH; m++)
    {
      pos_x = VB_ALL_BACKGROUND_TILE_SIZE * m;
      pos_y = VB_ALL_BACKGROUND_TILE_SIZE * n;

      cell_x = 1;
      cell_y = 13;

      palette = VB_ALL_PALETTE_GRAY;

      /* add this tile to the buffers */
      VB_ALL_ADD_TILE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, palette, BACKGROUND)
    }
  }

  /* update vbos */
  VB_ALL_UPDATE_TILES_IN_VBOS(BACKGROUND)

  return 0;
}

/*******************************************************************************
** vb_all_load_text()
*******************************************************************************/
short int vb_all_load_text(layout_text* t, int align, int palette, int scroll_y)
{
  int m;

  int sprite_index;

  int pos_x;
  int pos_y;

  int cell_x;
  int cell_y;

  int num_chars;

  /* make sure the button is valid */
  if (t == NULL)
    return 1;

  /* determine number of printable characters in text string */
  num_chars = 0;

  while ((num_chars < t->w) && (t->name[num_chars] != '\0'))
    num_chars++;

  if (num_chars == 0)
    return 0;

  /* determine coordinates of top left corner of first character */
  pos_x = t->x;
  pos_y = t->y - scroll_y;

  if (align == VB_ALL_TEXT_ALIGN_LEFT)
    pos_x += 0;
  else if (align == VB_ALL_TEXT_ALIGN_CENTER)
    pos_x += 4 * (t->w - num_chars);
  else if (align == VB_ALL_TEXT_ALIGN_RIGHT)
    pos_x += 8 * (t->w - num_chars);

  /* bound palette */
  if ((palette < 0) || (palette > 15))
    palette = 0;

  /* draw the string */
  for (m = 0; m < num_chars; m++)
  {
    if ((t->name[m] >= 32) && (t->name[m] < 127))
    {
      cell_x = (t->name[m] - 32) % 16;
      cell_y = (t->name[m] - 32) / 16;
    }
    else
      continue;

    VB_ALL_ADD_SPRITE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, 1, 1, palette, TEXT)

    /* increment the position for the next character */
    pos_x += 8;
  }

  return 0;
}

/*******************************************************************************
** vb_all_load_button()
*******************************************************************************/
short int vb_all_load_button(layout_button* b, int scroll_y)
{
  int m;
  int n;

  int sprite_index;

  int pos_x;
  int pos_y;

  int cell_x;
  int cell_y;

  int palette;

  int num_chars;

  /* make sure the button is valid */
  if (b == NULL)
    return 1;

  /* draw the button */
  for (m = 0; m < b->w; m++)
  {
    for (n = 0; n < 2; n++)
    {
      /* determine position of this piece */
      pos_x = b->x + (8 * m);
      pos_y = b->y + (8 * n) - scroll_y;

      /* left / right / middle piece */
      if (m == 0)
        cell_x = 0;
      else if (m == b->w - 1)
        cell_x = 2;
      else
        cell_x = 1;

      /* top / bottom piece */
      if (n == 0)
        cell_y = 9;
      else
        cell_y = 10;

      palette = VB_ALL_PALETTE_GRAY;

      VB_ALL_ADD_SPRITE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, 1, 1, palette, WIDGETS)
    }
  }

  /* determine number of printable characters in button name */
  num_chars = 0;

  while ((num_chars < b->w) && (b->name[num_chars] != '\0'))
    num_chars++;

  if (num_chars == 0)
    return 0;

  /* determine coordinates of top left corner of first character  */
  /* of button name. the text should be centered on the button.   */
  pos_x = b->x + 4 * (b->w - num_chars);
  pos_y = b->y + 4 * 1 - scroll_y;

  /* draw the text on the button */
  for (m = 0; m < num_chars; m++)
  {
    if ((b->name[m] >= 32) && (b->name[m] < 127))
    {
      cell_x = (b->name[m] - 32) % 16;
      cell_y = (b->name[m] - 32) / 16;
    }
    else
      continue;

    VB_ALL_ADD_SPRITE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, 1, 1, VB_ALL_PALETTE_GRAY, TEXT)

    /* increment the position for the next character */
    pos_x += 8;
  }

  return 0;
}

/*******************************************************************************
** vb_all_load_widget()
*******************************************************************************/
short int vb_all_load_widget( layout_widget* wdg, int val, int bound, 
                              char* str, int scroll_y)
{
  int m;

  int sprite_index;

  int pos_x;
  int pos_y;

  int cell_x;
  int cell_y;

  int num_chars;
  int max_chars;

  /* make sure the widget is valid */
  if (wdg == NULL)
    return 1;

  /* slider */
  if (wdg->kind == LAYOUT_WIDGET_KIND_SLIDER)
  {
    for (m = 0; m < LAYOUT_PARAM_SLIDER_TRACK_WIDTH; m++)
    {
      pos_x = wdg->x + LAYOUT_PARAM_SLIDER_TRACK_X + 8 * m;
      pos_y = wdg->y - scroll_y;

      if (m == 0)
        cell_x = 6;
      else if (m == LAYOUT_PARAM_SLIDER_TRACK_WIDTH - 1)
        cell_x = 8;
      else
        cell_x = 7;

      cell_y = 9;

      VB_ALL_ADD_SPRITE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, 1, 1, VB_ALL_PALETTE_1, TEXT)
    }

    pos_x = wdg->x + LAYOUT_PARAM_SLIDER_TRACK_X;
    pos_y = wdg->y - scroll_y;

    pos_x += (val * 8 * (LAYOUT_PARAM_SLIDER_TRACK_WIDTH - 1)) / bound;

    cell_x = 9;
    cell_y = 9;

    VB_ALL_ADD_SPRITE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, 1, 1, VB_ALL_PALETTE_1, TEXT)
  }
  /* arrows */
  else if (wdg->kind == LAYOUT_WIDGET_KIND_ARROWS)
  {
    if (val > 0)
    {
      pos_x = wdg->x + LAYOUT_PARAM_ARROWS_LEFT_X;
      pos_y = wdg->y - scroll_y;

      cell_x = 11;
      cell_y = 8;

      VB_ALL_ADD_SPRITE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, 1, 1, VB_ALL_PALETTE_1, TEXT)
    }

    if (val < bound)
    {
      pos_x = wdg->x + LAYOUT_PARAM_ARROWS_RIGHT_X;
      pos_y = wdg->y - scroll_y;

      cell_x = 12;
      cell_y = 8;

      VB_ALL_ADD_SPRITE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, 1, 1, VB_ALL_PALETTE_1, TEXT)
    }
  }
  /* radio button */
  else if (wdg->kind == LAYOUT_WIDGET_KIND_RADIO)
  {
    pos_x = wdg->x + LAYOUT_PARAM_RADIO_BUTTON_X;
    pos_y = wdg->y - scroll_y;

    if (val == 0)
      cell_x = 9;
    else
      cell_x = 10;

    cell_y = 8;

    VB_ALL_ADD_SPRITE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, 1, 1, VB_ALL_PALETTE_1, TEXT)
  }

  /* determine number of printable characters in value string */
  num_chars = 0;

  if (wdg->kind == LAYOUT_WIDGET_KIND_SLIDER)
    max_chars = LAYOUT_PARAM_SLIDER_VALUE_WIDTH;
  else if (wdg->kind == LAYOUT_WIDGET_KIND_ARROWS)
    max_chars = LAYOUT_PARAM_ARROWS_VALUE_WIDTH;
  else if (wdg->kind == LAYOUT_WIDGET_KIND_RADIO)
    max_chars = LAYOUT_PARAM_RADIO_VALUE_WIDTH;
  else
    max_chars = 0;

  while ((num_chars < max_chars) && (str[num_chars] != '\0'))
    num_chars++;

  if (num_chars == 0)
    return 0;

  /* determine coordinates of top left corner of first character */
  pos_x = wdg->x;
  pos_y = wdg->y - scroll_y;

  if (wdg->kind == LAYOUT_WIDGET_KIND_SLIDER)
    pos_x += LAYOUT_PARAM_SLIDER_VALUE_X;
  else if (wdg->kind == LAYOUT_WIDGET_KIND_ARROWS)
    pos_x += LAYOUT_PARAM_ARROWS_VALUE_X;
  else if (wdg->kind == LAYOUT_WIDGET_KIND_RADIO)
    pos_x += LAYOUT_PARAM_RADIO_VALUE_X;

  pos_x += 4 * (max_chars - num_chars);

  /* draw the string */
  for (m = 0; m < num_chars; m++)
  {
    if ((str[m] >= 32) && (str[m] < 127))
    {
      cell_x = (str[m] - 32) % 16;
      cell_y = (str[m] - 32) / 16;
    }
    else
      continue;

    VB_ALL_ADD_SPRITE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, 1, 1, VB_ALL_PALETTE_GRAY, TEXT)

    /* increment the position for the next character */
    pos_x += 8;
  }

  return 0;
}

/*******************************************************************************
** vb_all_load_top_bar()
*******************************************************************************/
short int vb_all_load_top_bar()
{
  int m;

  layout_button* b;

  /* reset sprite vbo counts */
  G_sprite_layer_count[GRAPHICS_SPRITE_LAYER_PANELS] = 0;
  G_sprite_layer_count[GRAPHICS_SPRITE_LAYER_WIDGETS] = 0;
  G_sprite_layer_count[GRAPHICS_SPRITE_LAYER_TEXT] = 0;

  /* buttons */
  for (m = 0; m < LAYOUT_NUM_TOP_BAR_BUTTONS; m++)
  {
    b = &G_layout_top_bar_buttons[m];

    vb_all_load_button(b, 0);
  }

  /* update vbos */
  VB_ALL_UPDATE_SPRITES_IN_VBOS(PANELS)
  VB_ALL_UPDATE_SPRITES_IN_VBOS(WIDGETS)
  VB_ALL_UPDATE_SPRITES_IN_VBOS(TEXT)

  return 0;
}

/*******************************************************************************
** vb_all_load_cart_screen()
*******************************************************************************/
short int vb_all_load_cart_screen()
{
  int m;

  cart* cr;
  patch* pt;

  layout_region*  rgn;

  layout_text*    t;
  layout_widget*  wdg;

  char* value_str;

  /* obtain cart & patch pointers */
  cr = &G_cart_bank[G_patch_edit_cart_number - PATCH_CART_NUMBER_LOWER_BOUND];
  pt = &(cr->patches[G_patch_edit_patch_number - PATCH_PATCH_NUMBER_LOWER_BOUND]);

  /* set the region to the main area */
  rgn = &G_layout_cart_regions[LAYOUT_CART_REGION_MAIN_AREA];

  /* headers */
  for (m = 0; m < LAYOUT_NUM_CART_HEADERS; m++)
  {
    t = &G_layout_cart_headers[m];

    if (!(LAYOUT_SCROLLED_ELEMENT_IS_IN_REGION(t, rgn, G_current_scroll_amount)))
      continue;

    vb_all_load_text(t, VB_ALL_TEXT_ALIGN_CENTER, VB_ALL_PALETTE_3, G_current_scroll_amount);
  }

  /* parameter names */
  for (m = 0; m < PATCH_NUM_PARAMS; m++)
  {
    t = &G_layout_cart_param_names[m];

    if (!(LAYOUT_SCROLLED_ELEMENT_IS_IN_REGION(t, rgn, G_current_scroll_amount)))
      continue;

    /* skip multiple/divisor or octave/note depending on frequency mode */
    if (((m == PATCH_PARAM_OSC_1_MULTIPLE) && (pt->values[PATCH_PARAM_OSC_1_FREQ_MODE] == PATCH_OSC_FREQ_MODE_VAL_FIXED)) || 
        ((m == PATCH_PARAM_OSC_1_DIVISOR)  && (pt->values[PATCH_PARAM_OSC_1_FREQ_MODE] == PATCH_OSC_FREQ_MODE_VAL_FIXED)) || 
        ((m == PATCH_PARAM_OSC_1_OCTAVE)   && (pt->values[PATCH_PARAM_OSC_1_FREQ_MODE] == PATCH_OSC_FREQ_MODE_VAL_RATIO)) || 
        ((m == PATCH_PARAM_OSC_1_NOTE)     && (pt->values[PATCH_PARAM_OSC_1_FREQ_MODE] == PATCH_OSC_FREQ_MODE_VAL_RATIO)) || 
        ((m == PATCH_PARAM_OSC_2_MULTIPLE) && (pt->values[PATCH_PARAM_OSC_2_FREQ_MODE] == PATCH_OSC_FREQ_MODE_VAL_FIXED)) || 
        ((m == PATCH_PARAM_OSC_2_DIVISOR)  && (pt->values[PATCH_PARAM_OSC_2_FREQ_MODE] == PATCH_OSC_FREQ_MODE_VAL_FIXED)) || 
        ((m == PATCH_PARAM_OSC_2_OCTAVE)   && (pt->values[PATCH_PARAM_OSC_2_FREQ_MODE] == PATCH_OSC_FREQ_MODE_VAL_RATIO)) || 
        ((m == PATCH_PARAM_OSC_2_NOTE)     && (pt->values[PATCH_PARAM_OSC_2_FREQ_MODE] == PATCH_OSC_FREQ_MODE_VAL_RATIO)) || 
        ((m == PATCH_PARAM_OSC_3_MULTIPLE) && (pt->values[PATCH_PARAM_OSC_3_FREQ_MODE] == PATCH_OSC_FREQ_MODE_VAL_FIXED)) || 
        ((m == PATCH_PARAM_OSC_3_DIVISOR)  && (pt->values[PATCH_PARAM_OSC_3_FREQ_MODE] == PATCH_OSC_FREQ_MODE_VAL_FIXED)) || 
        ((m == PATCH_PARAM_OSC_3_OCTAVE)   && (pt->values[PATCH_PARAM_OSC_3_FREQ_MODE] == PATCH_OSC_FREQ_MODE_VAL_RATIO)) || 
        ((m == PATCH_PARAM_OSC_3_NOTE)     && (pt->values[PATCH_PARAM_OSC_3_FREQ_MODE] == PATCH_OSC_FREQ_MODE_VAL_RATIO)))
    {
      continue;
    }

    vb_all_load_text(t, VB_ALL_TEXT_ALIGN_CENTER, VB_ALL_PALETTE_2, G_current_scroll_amount);
  }

  /* parameter values */
  for (m = 0; m < PATCH_NUM_PARAMS; m++)
  {
    wdg = &G_layout_cart_param_widgets[m];

    if (!(LAYOUT_SCROLLED_ELEMENT_IS_IN_REGION(wdg, rgn, G_current_scroll_amount)))
      continue;

    /* skip multiple/divisor or octave/note depending on frequency mode */
    if (((m == PATCH_PARAM_OSC_1_MULTIPLE) && (pt->values[PATCH_PARAM_OSC_1_FREQ_MODE] == PATCH_OSC_FREQ_MODE_VAL_FIXED)) || 
        ((m == PATCH_PARAM_OSC_1_DIVISOR)  && (pt->values[PATCH_PARAM_OSC_1_FREQ_MODE] == PATCH_OSC_FREQ_MODE_VAL_FIXED)) || 
        ((m == PATCH_PARAM_OSC_1_OCTAVE)   && (pt->values[PATCH_PARAM_OSC_1_FREQ_MODE] == PATCH_OSC_FREQ_MODE_VAL_RATIO)) || 
        ((m == PATCH_PARAM_OSC_1_NOTE)     && (pt->values[PATCH_PARAM_OSC_1_FREQ_MODE] == PATCH_OSC_FREQ_MODE_VAL_RATIO)) || 
        ((m == PATCH_PARAM_OSC_2_MULTIPLE) && (pt->values[PATCH_PARAM_OSC_2_FREQ_MODE] == PATCH_OSC_FREQ_MODE_VAL_FIXED)) || 
        ((m == PATCH_PARAM_OSC_2_DIVISOR)  && (pt->values[PATCH_PARAM_OSC_2_FREQ_MODE] == PATCH_OSC_FREQ_MODE_VAL_FIXED)) || 
        ((m == PATCH_PARAM_OSC_2_OCTAVE)   && (pt->values[PATCH_PARAM_OSC_2_FREQ_MODE] == PATCH_OSC_FREQ_MODE_VAL_RATIO)) || 
        ((m == PATCH_PARAM_OSC_2_NOTE)     && (pt->values[PATCH_PARAM_OSC_2_FREQ_MODE] == PATCH_OSC_FREQ_MODE_VAL_RATIO)) || 
        ((m == PATCH_PARAM_OSC_3_MULTIPLE) && (pt->values[PATCH_PARAM_OSC_3_FREQ_MODE] == PATCH_OSC_FREQ_MODE_VAL_FIXED)) || 
        ((m == PATCH_PARAM_OSC_3_DIVISOR)  && (pt->values[PATCH_PARAM_OSC_3_FREQ_MODE] == PATCH_OSC_FREQ_MODE_VAL_FIXED)) || 
        ((m == PATCH_PARAM_OSC_3_OCTAVE)   && (pt->values[PATCH_PARAM_OSC_3_FREQ_MODE] == PATCH_OSC_FREQ_MODE_VAL_RATIO)) || 
        ((m == PATCH_PARAM_OSC_3_NOTE)     && (pt->values[PATCH_PARAM_OSC_3_FREQ_MODE] == PATCH_OSC_FREQ_MODE_VAL_RATIO)))
    {
      continue;
    }

    /* determine parameter value string */
    switch(m)
    {
      case PATCH_PARAM_FEEDBACK:
      case PATCH_PARAM_OSC_1_OCTAVE:
      case PATCH_PARAM_OSC_2_OCTAVE:
      case PATCH_PARAM_OSC_3_OCTAVE:
      case PATCH_PARAM_ENV_1_MAX_LEVEL:
      case PATCH_PARAM_ENV_2_MAX_LEVEL:
      case PATCH_PARAM_ENV_3_MAX_LEVEL:
      case PATCH_PARAM_ENV_1_HOLD_LEVEL:
      case PATCH_PARAM_ENV_2_HOLD_LEVEL:
      case PATCH_PARAM_ENV_3_HOLD_LEVEL:
      case PATCH_PARAM_LFO_DELAY:
      case PATCH_PARAM_VIBRATO_DEPTH:
      case PATCH_PARAM_TREMOLO_DEPTH:
      {
        value_str = S_common_edit_0_to_127_values[pt->values[m]];
        break;
      }
      case PATCH_PARAM_OSC_1_MULTIPLE:
      case PATCH_PARAM_OSC_2_MULTIPLE:
      case PATCH_PARAM_OSC_3_MULTIPLE:
      case PATCH_PARAM_OSC_1_DIVISOR:
      case PATCH_PARAM_OSC_2_DIVISOR:
      case PATCH_PARAM_OSC_3_DIVISOR:
      case PATCH_PARAM_ENV_1_ATTACK:
      case PATCH_PARAM_ENV_2_ATTACK:
      case PATCH_PARAM_ENV_3_ATTACK:
      case PATCH_PARAM_ENV_1_DECAY:
      case PATCH_PARAM_ENV_2_DECAY:
      case PATCH_PARAM_ENV_3_DECAY:
      case PATCH_PARAM_ENV_1_SUSTAIN:
      case PATCH_PARAM_ENV_2_SUSTAIN:
      case PATCH_PARAM_ENV_3_SUSTAIN:
      case PATCH_PARAM_ENV_1_RELEASE:
      case PATCH_PARAM_ENV_2_RELEASE:
      case PATCH_PARAM_ENV_3_RELEASE:
      case PATCH_PARAM_ENV_1_RATE_KS:
      case PATCH_PARAM_ENV_2_RATE_KS:
      case PATCH_PARAM_ENV_3_RATE_KS:
      case PATCH_PARAM_ENV_1_LEVEL_KS:
      case PATCH_PARAM_ENV_2_LEVEL_KS:
      case PATCH_PARAM_ENV_3_LEVEL_KS:
      case PATCH_PARAM_LFO_SPEED:
      case PATCH_PARAM_VIBRATO_SENSITIVITY:
      case PATCH_PARAM_TREMOLO_SENSITIVITY:
      case PATCH_PARAM_BOOST_SENSITIVITY:
      {
        value_str = S_common_edit_1_to_128_values[pt->values[m]];
        break;
      }
      case PATCH_PARAM_LEGACY_KEYSCALE:
      {
        value_str = S_patch_edit_legacy_keyscale_values[pt->values[m]];
        break;
      }
      case PATCH_PARAM_ALGORITHM:
      {
        value_str = S_patch_edit_algorithm_values[pt->values[m]];
        break;
      }
      case PATCH_PARAM_OSC_SYNC:
      case PATCH_PARAM_LFO_SYNC:
      {
        value_str = S_patch_edit_sync_values[pt->values[m]];
        break;
      }
      case PATCH_PARAM_OSC_1_WAVEFORM:
      case PATCH_PARAM_OSC_2_WAVEFORM:
      case PATCH_PARAM_OSC_3_WAVEFORM:
      {
        value_str = S_patch_edit_osc_waveform_values[pt->values[m]];
        break;
      }
      case PATCH_PARAM_OSC_1_FREQ_MODE:
      case PATCH_PARAM_OSC_2_FREQ_MODE:
      case PATCH_PARAM_OSC_3_FREQ_MODE:
      {
        value_str = S_patch_edit_osc_freq_mode_values[pt->values[m]];
        break;
      }
      case PATCH_PARAM_OSC_1_NOTE:
      case PATCH_PARAM_OSC_2_NOTE:
      case PATCH_PARAM_OSC_3_NOTE:
      {
        value_str = S_patch_edit_osc_note_values[pt->values[m]];
        break;
      }
      case PATCH_PARAM_OSC_1_DETUNE:
      case PATCH_PARAM_OSC_2_DETUNE:
      case PATCH_PARAM_OSC_3_DETUNE:
      {
        value_str = S_patch_edit_osc_detune_values[pt->values[m]];
        break;
      }
      case PATCH_PARAM_LFO_WAVEFORM:
      {
        value_str = S_patch_edit_lfo_waveform_values[pt->values[m]];
        break;
      }
      case PATCH_PARAM_VIBRATO_POLARITY:
      {
        value_str = S_patch_edit_polarity_values[pt->values[m]];
        break;
      }
      case PATCH_PARAM_TREMOLO_MODE:
      case PATCH_PARAM_BOOST_MODE:
      {
        value_str = S_patch_edit_am_mode_values[pt->values[m]];
        break;
      }
      case PATCH_PARAM_HIGHPASS_CUTOFF:
      {
        value_str = S_patch_edit_highpass_cutoff_values[pt->values[m]];
        break;
      }
      case PATCH_PARAM_LOWPASS_CUTOFF:
      {
        value_str = S_patch_edit_lowpass_cutoff_values[pt->values[m]];
        break;
      }
      default:
      {
        value_str = S_common_edit_0_to_127_values[pt->values[m]];
        break;
      }
    }

    vb_all_load_widget(wdg, pt->values[m], G_patch_param_bounds[m], value_str, G_current_scroll_amount);
  }

  /* update vbos */
  VB_ALL_UPDATE_SPRITES_IN_VBOS(PANELS)
  VB_ALL_UPDATE_SPRITES_IN_VBOS(WIDGETS)
  VB_ALL_UPDATE_SPRITES_IN_VBOS(TEXT)

  return 0;
}

/*******************************************************************************
** vb_all_load_instruments_screen()
*******************************************************************************/
short int vb_all_load_instruments_screen()
{

  /* update vbos */
  VB_ALL_UPDATE_SPRITES_IN_VBOS(PANELS)
  VB_ALL_UPDATE_SPRITES_IN_VBOS(WIDGETS)
  VB_ALL_UPDATE_SPRITES_IN_VBOS(TEXT)

  return 0;
}

/*******************************************************************************
** vb_all_load_song_screen()
*******************************************************************************/
short int vb_all_load_song_screen()
{

  /* update vbos */
  VB_ALL_UPDATE_SPRITES_IN_VBOS(PANELS)
  VB_ALL_UPDATE_SPRITES_IN_VBOS(WIDGETS)
  VB_ALL_UPDATE_SPRITES_IN_VBOS(TEXT)

  return 0;
}

/*******************************************************************************
** vb_all_load_mixer_screen()
*******************************************************************************/
short int vb_all_load_mixer_screen()
{

  /* update vbos */
  VB_ALL_UPDATE_SPRITES_IN_VBOS(PANELS)
  VB_ALL_UPDATE_SPRITES_IN_VBOS(WIDGETS)
  VB_ALL_UPDATE_SPRITES_IN_VBOS(TEXT)

  return 0;
}

/*******************************************************************************
** vb_all_load_sound_fx_screen()
*******************************************************************************/
short int vb_all_load_sound_fx_screen()
{

  /* update vbos */
  VB_ALL_UPDATE_SPRITES_IN_VBOS(PANELS)
  VB_ALL_UPDATE_SPRITES_IN_VBOS(WIDGETS)
  VB_ALL_UPDATE_SPRITES_IN_VBOS(TEXT)

  return 0;
}

/*******************************************************************************
** vb_all_load_dpcm_screen()
*******************************************************************************/
short int vb_all_load_dpcm_screen()
{

  /* update vbos */
  VB_ALL_UPDATE_SPRITES_IN_VBOS(PANELS)
  VB_ALL_UPDATE_SPRITES_IN_VBOS(WIDGETS)
  VB_ALL_UPDATE_SPRITES_IN_VBOS(TEXT)

  return 0;
}

/*******************************************************************************
** vb_all_load_bar_screen()
*******************************************************************************/
short int vb_all_load_bar_screen()
{

  /* update vbos */
  VB_ALL_UPDATE_SPRITES_IN_VBOS(PANELS)
  VB_ALL_UPDATE_SPRITES_IN_VBOS(WIDGETS)
  VB_ALL_UPDATE_SPRITES_IN_VBOS(TEXT)

  return 0;
}

