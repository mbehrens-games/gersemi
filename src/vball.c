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

#define VB_ALL_BACKGROUND_TILE_SIZE       16
#define VB_ALL_BACKGROUND_TILE_SIZE_HALF  (VB_ALL_BACKGROUND_TILE_SIZE / 2)

#define VB_ALL_BACKGROUND_WIDTH   (GRAPHICS_OVERSCAN_WIDTH / VB_ALL_BACKGROUND_TILE_SIZE)
#define VB_ALL_BACKGROUND_HEIGHT  (GRAPHICS_OVERSCAN_HEIGHT / VB_ALL_BACKGROUND_TILE_SIZE)

#define VB_ALL_PALETTE_1    0
#define VB_ALL_PALETTE_2    1
#define VB_ALL_PALETTE_3    4
#define VB_ALL_PALETTE_GRAY 6

#define VB_ALL_BUTTON_MAX_TEXT_SIZE   12
#define VB_ALL_HEADER_MAX_TEXT_SIZE   16

#define VB_ALL_TEXT_BOX_COLUMN_MAX_TEXT_SIZE  16
#define VB_ALL_TEXT_BOX_FULL_MAX_TEXT_SIZE    16

#define VB_ALL_PARAM_NAME_MAX_TEXT_SIZE     4
#define VB_ALL_PARAM_NUMBER_MAX_TEXT_SIZE   4
#define VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE   12

#define VB_ALL_FILENAME_MAX_TEXT_SIZE 44

enum
{
  VB_ALL_ALIGN_LEFT = 0,
  VB_ALL_ALIGN_CENTER,
  VB_ALL_ALIGN_RIGHT
};

enum
{
  VB_ALL_SPRITE_NAME_PARAM_ARROWS_LEFT = 0,
  VB_ALL_SPRITE_NAME_PARAM_ARROWS_RIGHT, 
  VB_ALL_SPRITE_NAME_PARAM_RADIO_BUTTON_OFF, 
  VB_ALL_SPRITE_NAME_PARAM_RADIO_BUTTON_ON
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
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_TILES][12 * tile_index +  0] = pos_x - (4 * width);       \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_TILES][12 * tile_index +  1] = pos_y - (4 * height);      \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_TILES][12 * tile_index +  2] = GRAPHICS_Z_LEVEL_##layer;  \
                                                                                                              \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_TILES][12 * tile_index +  3] = pos_x + (4 * width);       \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_TILES][12 * tile_index +  4] = pos_y - (4 * height);      \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_TILES][12 * tile_index +  5] = GRAPHICS_Z_LEVEL_##layer;  \
                                                                                                              \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_TILES][12 * tile_index +  6] = pos_x - (4 * width);       \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_TILES][12 * tile_index +  7] = pos_y + (4 * height);      \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_TILES][12 * tile_index +  8] = GRAPHICS_Z_LEVEL_##layer;  \
                                                                                                              \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_TILES][12 * tile_index +  9] = pos_x + (4 * width);       \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_TILES][12 * tile_index + 10] = pos_y + (4 * height);      \
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

#define VB_ALL_ADD_TILE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette, layer)  \
  if (VB_ALL_TILE_LAYER_INDEX(layer) < VB_ALL_TILE_LAYER_MAX(layer))                        \
  {                                                                                         \
    tile_index =  VB_ALL_TILE_LAYER_INDEX(layer) +                                          \
                  VB_ALL_TILE_LAYER_COUNT(layer);                                           \
                                                                                            \
    VB_ALL_ADD_TILE_TO_VERTEX_BUFFER(pos_x, pos_y, 2, 2, layer)                             \
    VB_ALL_ADD_TILE_TO_TEXTURE_COORD_BUFFER(cell_x, cell_y, 2, 2)                           \
    VB_ALL_ADD_TILE_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)                       \
    VB_ALL_ADD_TILE_TO_ELEMENT_BUFFER()                                                     \
                                                                                            \
    VB_ALL_TILE_LAYER_COUNT(layer) += 1;                                                    \
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
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][12 * sprite_index +  0] = pos_x - (4 * width);       \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][12 * sprite_index +  1] = pos_y - (4 * height);      \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][12 * sprite_index +  2] = GRAPHICS_Z_LEVEL_##layer;  \
                                                                                                                  \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][12 * sprite_index +  3] = pos_x + (4 * width);       \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][12 * sprite_index +  4] = pos_y - (4 * height);      \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][12 * sprite_index +  5] = GRAPHICS_Z_LEVEL_##layer;  \
                                                                                                                  \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][12 * sprite_index +  6] = pos_x - (4 * width);       \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][12 * sprite_index +  7] = pos_y + (4 * height);      \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][12 * sprite_index +  8] = GRAPHICS_Z_LEVEL_##layer;  \
                                                                                                                  \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][12 * sprite_index +  9] = pos_x + (4 * width);       \
  G_overscan_vertex_buffer_data[GRAPHICS_BUFFER_SET_SPRITES][12 * sprite_index + 10] = pos_y + (4 * height);      \
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

#define VB_ALL_ADD_SPRITE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, width, height, lighting, palette, layer) \
  if (VB_ALL_SPRITE_LAYER_COUNT(layer) < VB_ALL_SPRITE_LAYER_MAX(layer))                                    \
  {                                                                                                         \
    sprite_index =  VB_ALL_SPRITE_LAYER_INDEX(layer) +                                                      \
                    VB_ALL_SPRITE_LAYER_COUNT(layer);                                                       \
                                                                                                            \
    VB_ALL_ADD_SPRITE_TO_VERTEX_BUFFER(pos_x, pos_y, width, height, layer)                                  \
    VB_ALL_ADD_SPRITE_TO_TEXTURE_COORD_BUFFER(cell_x, cell_y, width, height)                                \
    VB_ALL_ADD_SPRITE_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)                                     \
    VB_ALL_ADD_SPRITE_TO_ELEMENT_BUFFER()                                                                   \
                                                                                                            \
    VB_ALL_SPRITE_LAYER_COUNT(layer) += 1;                                                                  \
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

/* cart edit screen layout macros */
#define VB_ALL_CART_PARAM_CASE_INT(name, param, table)                       \
  case LAYOUT_CART_PARAM_##name:                                             \
  {                                                                          \
    value = param;                                                           \
    value_string = S_##table##_values[value - pr->lower_bound];              \
    break;                                                                   \
  }

#define VB_ALL_CART_PARAM_CASE_FLAG(name, param, table)                      \
  case LAYOUT_CART_PARAM_##name:                                             \
  {                                                                          \
    value = (param & pr->upper_bound);                                       \
    value_string = S_##table##_values[value / pr->upper_bound];              \
    break;                                                                   \
  }

#define VB_ALL_AUDITION_PARAM_CASE(name, param, table)                       \
  case LAYOUT_AUDITION_PARAM_##name:                                         \
  {                                                                          \
    value = param;                                                           \
    value_string = S_##table##_values[value - pr->lower_bound];              \
    break;                                                                   \
  }

/* common edit tables */
static char S_common_edit_1_to_100_values[100][VB_ALL_PARAM_NUMBER_MAX_TEXT_SIZE] = 
  {  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9",  "10", 
    "11", "12", "13", "14", "15", "16", "17", "18", "19",  "20", 
    "21", "22", "23", "24", "25", "26", "27", "28", "29",  "30", 
    "31", "32", "33", "34", "35", "36", "37", "38", "39",  "40", 
    "41", "42", "43", "44", "45", "46", "47", "48", "49",  "50", 
    "51", "52", "53", "54", "55", "56", "57", "58", "59",  "60", 
    "61", "62", "63", "64", "65", "66", "67", "68", "69",  "70", 
    "71", "72", "73", "74", "75", "76", "77", "78", "79",  "80", 
    "81", "82", "83", "84", "85", "86", "87", "88", "89",  "90", 
    "91", "92", "93", "94", "95", "96", "97", "98", "99", "100" 
  };

static char S_common_edit_0_to_100_values[101][VB_ALL_PARAM_NUMBER_MAX_TEXT_SIZE] = 
  {  "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9",  "10", 
          "11", "12", "13", "14", "15", "16", "17", "18", "19",  "20", 
          "21", "22", "23", "24", "25", "26", "27", "28", "29",  "30", 
          "31", "32", "33", "34", "35", "36", "37", "38", "39",  "40", 
          "41", "42", "43", "44", "45", "46", "47", "48", "49",  "50", 
          "51", "52", "53", "54", "55", "56", "57", "58", "59",  "60", 
          "61", "62", "63", "64", "65", "66", "67", "68", "69",  "70", 
          "71", "72", "73", "74", "75", "76", "77", "78", "79",  "80", 
          "81", "82", "83", "84", "85", "86", "87", "88", "89",  "90", 
          "91", "92", "93", "94", "95", "96", "97", "98", "99", "100" 
  };

/* top panel tables */
static char S_top_panel_button_names[LAYOUT_NUM_TOP_PANEL_BUTTONS][VB_ALL_BUTTON_MAX_TEXT_SIZE] = 
  { "Cart", "Instruments", "Song", "Mixer", "Sound FX", "DPCM" };

static char S_top_panel_header_names[LAYOUT_NUM_TOP_PANEL_HEADERS][VB_ALL_HEADER_MAX_TEXT_SIZE] = 
  { "Gersemi", "v0.9" };

/* cart edit tables */
static char S_cart_button_names[LAYOUT_NUM_CART_BUTTONS][VB_ALL_BUTTON_MAX_TEXT_SIZE] = 
  { "Load", "Save", /* load/save cart */
    "Copy", "Zap"   /* copy/zap patch */
  };

static char S_cart_header_names[LAYOUT_NUM_CART_HEADERS][VB_ALL_HEADER_MAX_TEXT_SIZE] = 
  { "Osc 1", "Osc 2", "Osc 3", 
    "Env 1", "Env 2", "Env 3", 
    "LFO Vib", "LFO Trem", "Chorus", 
    "Boost", "Velocity", "Filters", 
    "Pitch Env", "Pitch Wheel", 
    "Arpeggio", "Portamento", 
    "Mod Wheel", "Aftertouch", "Exp Pedal" 
  };

static char S_cart_param_names[LAYOUT_NUM_CART_PARAMS][VB_ALL_PARAM_NAME_MAX_TEXT_SIZE] = 
  { "Car", "Pat",                                     /* cart/patch numbers */
    "Alg",                                            /* algorithm */
    "Syn",                                            /* osc sync */
    "Wav", "Phi", "Frq", "Mul", "Div", "Oct", "Nte",  /* oscillator 1 */
    "Det", "Vib", "PtE", "PtW", 
    "Wav", "Phi", "Frq", "Mul", "Div", "Oct", "Nte",  /* oscillator 2 */
    "Det", "Vib", "PtE", "PtW", 
    "Wav", "Phi", "Frq", "Mul", "Div", "Oct", "Nte",  /* oscillator 3 */
    "Det", "Vib", "PtE", "PtW", 
    "Att", "Dec", "Sus", "Rel", "Lev", "Hld", "Mde",  /* envelope 1 */
    "RtS", "LvS", "Trm", "Bst", "Vel", 
    "Att", "Dec", "Sus", "Rel", "Lev", "Hld", "Mde",  /* envelope 2 */
    "RtS", "LvS", "Trm", "Bst", "Vel", 
    "Att", "Dec", "Sus", "Rel", "Lev", "Hld", "Mde",  /* envelope 3 */
    "RtS", "LvS", "Trm", "Bst", "Vel", 
    "Wav", "Dly", "Spd", "Dep", "Sns", "Syn", "Pol",  /* vibrato lfo */
    "Wav", "Dly", "Spd", "Dep", "Sns", "Syn",         /* tremolo lfo */
    "Wav", "Dly", "Spd", "Dep", "Sns", "Syn",         /* chorus lfo */
    "Sns",                                            /* boost */
    "Sns",                                            /* velocity */
     "HP",  "LP",                                     /* filters */
    "Att", "Dec", "Rel", "Max", "Fin",                /* pitch env */
    "Mde", "Rng",                                     /* pitch wheel */
    "Mde", "Pat", "Oct", "Spd",                       /* arpeggio */
    "Mde", "Leg", "Spd",                              /* portamento */
    "Vib", "Trm", "Bst", "Chr",                       /* mod wheel */
    "Vib", "Trm", "Bst", "Chr",                       /* aftertouch */
    "Vib", "Trm", "Bst", "Chr"                        /* exp pedal */
  };

static char S_audition_param_names[LAYOUT_NUM_AUDITION_PARAMS][VB_ALL_PARAM_NAME_MAX_TEXT_SIZE] = 
  { "Oct", "Vel", "PW", "MW", "AT", "EP" };

static char S_patch_edit_algorithm_values[PATCH_ALGORITHM_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "1>2>3", "(1+2)>3", "1+(2>3)", "1+2+3" };

static char S_patch_edit_sync_values[PATCH_SYNC_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "Off", "On" };

static char S_patch_edit_osc_waveform_values[PATCH_OSC_WAVEFORM_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  {  "Sine", "HalfRect", "FullRect", "QuarRect", "EPO Sine", "EPO Full",   "Square",  "SawDown", 
    "SawUp", "Triangle", "EPO Squa", "EPO SawD", "EPO SawU",  "EPO Tri", "NoiseSqu", "NoiseSaw" 
  };

static char S_patch_edit_osc_phi_values[PATCH_OSC_PHI_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "0", "90", "180", "270" };

static char S_patch_edit_osc_freq_mode_values[PATCH_OSC_FREQ_MODE_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "Ratio", "Fixed" };

static char S_patch_edit_osc_octave_values[PATCH_OSC_OCTAVE_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static char S_patch_edit_osc_note_values[PATCH_OSC_NOTE_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

static char S_patch_edit_osc_detune_values[PATCH_OSC_DETUNE_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "-3", "-2", "-1", "0", "1", "2", "3" };

static char S_patch_edit_env_hold_mode_values[PATCH_ENV_HOLD_MODE_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "Off", "Pedal", "Invert", "Always" };

static char S_patch_edit_lfo_waveform_values[PATCH_LFO_WAVEFORM_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "Triangle", "Square", "SawUp", "SawDown" };

static char S_patch_edit_lfo_polarity_values[PATCH_LFO_POLARITY_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "U/D", "Up Only" };

static char S_patch_edit_highpass_cutoff_values[PATCH_HIGHPASS_CUTOFF_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "A0", "A1", "A2", "A3" };

static char S_patch_edit_lowpass_cutoff_values[PATCH_LOWPASS_CUTOFF_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "E7", "G7", "A7", "C8" };

static char S_patch_edit_peg_level_values[PATCH_PEG_LEVEL_NUM_VALUES][VB_ALL_PARAM_NUMBER_MAX_TEXT_SIZE] = 
  { "-50", "-49", "-48", "-47", "-46", "-45", "-44", "-43", "-42", "-41", 
    "-40", "-39", "-38", "-37", "-36", "-35", "-34", "-33", "-32", "-31", 
    "-30", "-29", "-28", "-27", "-26", "-25", "-24", "-23", "-22", "-21", 
    "-20", "-19", "-18", "-17", "-16", "-15", "-14", "-13", "-12", "-11", 
    "-10",  "-9",  "-8",  "-7",  "-6",  "-5",  "-4",  "-3",  "-2",  "-1", 
      "0", 
      "1",   "2",   "3",   "4",   "5",   "6",   "7",   "8",   "9",  "10", 
     "11",  "12",  "13",  "14",  "15",  "16",  "17",  "18",  "19",  "20", 
     "21",  "22",  "23",  "24",  "25",  "26",  "27",  "28",  "29",  "30", 
     "31",  "32",  "33",  "34",  "35",  "36",  "37",  "38",  "39",  "40", 
     "41",  "42",  "43",  "44",  "45",  "46",  "47",  "48",  "49",  "50" 
  };

static char S_patch_edit_pitch_wheel_mode_values[PATCH_PITCH_WHEEL_MODE_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "Bend", "Half" };

static char S_patch_edit_arpeggio_mode_values[PATCH_ARPEGGIO_MODE_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "Harp", "Rolled" };

static char S_patch_edit_arpeggio_pattern_values[PATCH_ARPEGGIO_PATTERN_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "Up", "Down", "Up/Down", "U/D Alt" };

static char S_patch_edit_portamento_mode_values[PATCH_PORTAMENTO_MODE_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "Bend", "Half" };

static char S_patch_edit_portamento_legato_values[PATCH_PORTAMENTO_LEGATO_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "Off", "Follow", "Hammer" };

static char S_patch_edit_routing_values[2][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "Off", "On" };

static char S_audition_bi_wheel_values[MIDI_CONT_BI_WHEEL_NUM_VALUES][VB_ALL_PARAM_NUMBER_MAX_TEXT_SIZE] = 
  { "-64", "-63", "-62", "-61", "-60", "-59", "-58", "-57", 
    "-56", "-55", "-54", "-53", "-52", "-51", "-50", "-49", 
    "-48", "-47", "-46", "-45", "-44", "-43", "-42", "-41", 
    "-40", "-39", "-38", "-37", "-36", "-35", "-34", "-33", 
    "-32", "-31", "-30", "-29", "-28", "-27", "-26", "-25", 
    "-24", "-23", "-22", "-21", "-20", "-19", "-18", "-17", 
    "-16", "-15", "-14", "-13", "-12", "-11", "-10",  "-9", 
     "-8",  "-7",  "-6",  "-5",  "-4",  "-3",  "-2",  "-1", 
      "0", 
      "1",   "2",   "3",   "4",   "5",   "6",   "7",   "8", 
      "9",  "10",  "11",  "12",  "13",  "14",  "15",  "16", 
     "17",  "18",  "19",  "20",  "21",  "22",  "23",  "24", 
     "25",  "26",  "27",  "28",  "29",  "30",  "31",  "32", 
     "33",  "34",  "35",  "36",  "37",  "38",  "39",  "40", 
     "41",  "42",  "43",  "44",  "45",  "46",  "47",  "48", 
     "49",  "50",  "51",  "52",  "53",  "54",  "55",  "56", 
     "57",  "58",  "59",  "60",  "61",  "62",  "63",  "64" 
  };

static char S_audition_uni_wheel_values[MIDI_CONT_UNI_WHEEL_NUM_VALUES][VB_ALL_PARAM_NUMBER_MAX_TEXT_SIZE] = 
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
  int k;

  int m;
  int n;

  int tile_index;

  int pos_x;
  int pos_y;

  int cell_x;
  int cell_y;

  int lighting;
  int palette;

  /* reset tile vbo counts */
  for (k = 0; k < GRAPHICS_NUM_TILE_LAYERS; k++)
    G_tile_layer_count[k] = 0;

  /* draw the background */
  for (n = 0; n < VB_ALL_BACKGROUND_HEIGHT; n++)
  {
    for (m = 0; m < VB_ALL_BACKGROUND_WIDTH; m++)
    {
      /* determine position */
      pos_x = VB_ALL_BACKGROUND_TILE_SIZE * m + VB_ALL_BACKGROUND_TILE_SIZE_HALF;
      pos_y = VB_ALL_BACKGROUND_TILE_SIZE * n + VB_ALL_BACKGROUND_TILE_SIZE_HALF;

      /* determine texture coordinates */
      cell_x = 1;
      cell_y = 13;

      /* determine lighting and palette */
      lighting = -1;
      palette = 6;

      /* add this tile to the buffers */
      VB_ALL_ADD_TILE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette, BACKGROUND)
    }
  }

  /* update vbos */
  VB_ALL_UPDATE_TILES_IN_VBOS(BACKGROUND)

  return 0;
}

/*******************************************************************************
** vb_all_load_panel()
*******************************************************************************/
short int vb_all_load_panel(int offset_x, int offset_y, 
                            int width, int height, int type)
{
  int m;
  int n;

  int sprite_index;

  int corner_x;
  int corner_y;

  int pos_x;
  int pos_y;

  int cell_x;
  int cell_y;

  int lighting;
  int palette;

  /* make sure the width and height are valid     */
  /* the width & height are in terms of 8x8 cells */
  if ((width < 2) || (width > LAYOUT_OVERSCAN_WIDTH / 8))
    return 1;

  if ((height < 2) || (height > LAYOUT_OVERSCAN_HEIGHT / 8))
    return 1;

  /* determine coordinates of the center of the top left corner   */
  /* 8x8 cell. the offsets from the screen center are in pixels.  */
  corner_x = LAYOUT_OVERSCAN_CENTER_X + offset_x - 4 * (width - 1);
  corner_y = LAYOUT_OVERSCAN_CENTER_Y + offset_y - 4 * (height - 1);

  /* set lighting and palette */
  lighting = 0;
  palette = VB_ALL_PALETTE_1;

  /* draw the panel */
  for (n = 0; n < height; n++)
  {
    for (m = 0; m < width; m++)
    {
      /* determine center of this piece */
      pos_x = corner_x + (8 * m);
      pos_y = corner_y + (8 * n);

      /* left / right / middle piece */
      if (m == 0)
        cell_x = 0;
      else if (m == width - 1)
        cell_x = 2;
      else
        cell_x = 1;

      /* top / bottom / middle piece */
      if (n == 0)
        cell_y = 6;
      else if (n == height - 1)
        cell_y = 8;
      else
        cell_y = 7;

      /* adjust cells based on type */
      if (type == LAYOUT_PANEL_TYPE_1)
        cell_x += 0;
      else if (type == LAYOUT_PANEL_TYPE_2)
        cell_x += 3;
      else
        cell_x += 0;

      VB_ALL_ADD_SPRITE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, 1, 1, lighting, palette, PANELS)
    }
  }

  return 0;
}

/*******************************************************************************
** vb_all_load_button()
*******************************************************************************/
short int vb_all_load_button(int offset_x, int offset_y, int width, int state)
{
  int m;
  int n;

  int sprite_index;

  int corner_x;
  int corner_y;

  int pos_x;
  int pos_y;

  int cell_x;
  int cell_y;

  int lighting;
  int palette;

  /* make sure the width is valid       */
  /* the width is in terms of 8x8 cells */
  if ((width < 2) || (width > LAYOUT_OVERSCAN_WIDTH / 8))
    return 1;

  /* determine coordinates of the center of the top left corner   */
  /* 8x8 cell. the offsets from the screen center are in pixels.  */
  corner_x = LAYOUT_OVERSCAN_CENTER_X + offset_x - 4 * (width - 1);
  corner_y = LAYOUT_OVERSCAN_CENTER_Y + offset_y - 4 * (2 - 1);

  /* set lighting and palette */
  if (state == LAYOUT_BUTTON_STATE_OFF)
  {
    lighting = 0;
    palette = 6;
  }
  else if (state == LAYOUT_BUTTON_STATE_ON)
  {
    lighting = 1;
    palette = VB_ALL_PALETTE_1;
  }
  else
  {
    lighting = 0;
    palette = 6;
  }

  /* draw the button */
  for (n = 0; n < 2; n++)
  {
    for (m = 0; m < width; m++)
    {
      /* determine center of this piece */
      pos_x = corner_x + (8 * m);
      pos_y = corner_y + (8 * n);

      /* left / right / middle piece */
      if (m == 0)
        cell_x = 0;
      else if (m == width - 1)
        cell_x = 2;
      else
        cell_x = 1;

      /* top / bottom piece */
      if (n == 0)
        cell_y = 9;
      else
        cell_y = 10;

      VB_ALL_ADD_SPRITE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, 1, 1, lighting, palette, WIDGETS)
    }
  }

  return 0;
}

/*******************************************************************************
** vb_all_load_text_box()
*******************************************************************************/
short int vb_all_load_text_box(int offset_x, int offset_y, int width, int state)
{
  int m;
  int n;

  int sprite_index;

  int corner_x;
  int corner_y;

  int pos_x;
  int pos_y;

  int cell_x;
  int cell_y;

  int lighting;
  int palette;

  /* make sure the width is valid       */
  /* the width is in terms of 8x8 cells */
  if ((width < 2) || (width > LAYOUT_OVERSCAN_WIDTH / 8))
    return 1;

  /* determine coordinates of the center of the top left corner   */
  /* 8x8 cell. the offsets from the screen center are in pixels.  */
  corner_x = LAYOUT_OVERSCAN_CENTER_X + offset_x - 4 * (width - 1);
  corner_y = LAYOUT_OVERSCAN_CENTER_Y + offset_y - 4 * (2 - 1);

  /* set lighting and palette */
  if (state == LAYOUT_TEXT_BOX_STATE_NORMAL)
    lighting = 0;
  else if (state == LAYOUT_TEXT_BOX_STATE_HIGHLIGHT)
    lighting = 1;
  else
    lighting = 0;

  palette = VB_ALL_PALETTE_1;

  /* draw the button */
  for (n = 0; n < 2; n++)
  {
    for (m = 0; m < width; m++)
    {
      /* determine center of this piece */
      pos_x = corner_x + (8 * m);
      pos_y = corner_y + (8 * n);

      /* left / right / middle piece */
      if (m == 0)
        cell_x = 3;
      else if (m == width - 1)
        cell_x = 5;
      else
        cell_x = 4;

      /* top / bottom piece */
      if (n == 0)
        cell_y = 9;
      else
        cell_y = 10;

      VB_ALL_ADD_SPRITE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, 1, 1, lighting, palette, WIDGETS)
    }
  }

  return 0;
}

/*******************************************************************************
** vb_all_load_vertical_scrollbar()
*******************************************************************************/
short int vb_all_load_vertical_scrollbar( int current_scroll_amount, 
                                          int max_scroll_amount)
{
  int n;

  int sprite_index;

  int top_x;
  int top_y;

  int pos_x;
  int pos_y;

  int cell_x;
  int cell_y;

  int lighting;
  int palette;

  /* make sure the scroll amount is valid */
  if ((current_scroll_amount < 0) || (current_scroll_amount > max_scroll_amount))
    return 1;

  /* set lighting and palette */
  lighting = 0;
  palette = VB_ALL_PALETTE_1;

  /* draw corner block */
  pos_x = LAYOUT_OVERSCAN_CENTER_X;
  pos_x += LAYOUT_SCROLLBAR_BLOCK_X;

  pos_y = LAYOUT_OVERSCAN_CENTER_Y;
  pos_y += LAYOUT_SCROLLBAR_BLOCK_Y;

  cell_x = 11;
  cell_y = 7;

  VB_ALL_ADD_SPRITE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, 1, 1, lighting, palette, WIDGETS)

  /* draw up arrow */
  pos_x = LAYOUT_OVERSCAN_CENTER_X;
  pos_x += LAYOUT_VERT_SCROLLBAR_UP_ARROW_X;

  pos_y = LAYOUT_OVERSCAN_CENTER_Y;
  pos_y += LAYOUT_VERT_SCROLLBAR_UP_ARROW_Y;

  cell_x = 14;
  cell_y = 7;

  VB_ALL_ADD_SPRITE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, 1, 1, lighting, palette, WIDGETS)

  /* draw down arrow */
  pos_x = LAYOUT_OVERSCAN_CENTER_X;
  pos_x += LAYOUT_VERT_SCROLLBAR_DOWN_ARROW_X;

  pos_y = LAYOUT_OVERSCAN_CENTER_Y;
  pos_y += LAYOUT_VERT_SCROLLBAR_DOWN_ARROW_Y;

  cell_x = 15;
  cell_y = 7;

  VB_ALL_ADD_SPRITE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, 1, 1, lighting, palette, WIDGETS)

  /************************/
  /* draw scrollbar track */
  /************************/

  /* determine coordinates of the center of the top 8x8 cell. */
  /* the offsets from the screen center are in pixels.        */
  top_x = LAYOUT_OVERSCAN_CENTER_X;
  top_x += LAYOUT_VERT_SCROLLBAR_TRACK_X;

  top_y = LAYOUT_OVERSCAN_CENTER_Y;
  top_y += LAYOUT_VERT_SCROLLBAR_TRACK_Y;
  top_y -= 4 * LAYOUT_VERT_SCROLLBAR_TRACK_HEIGHT;
  top_y += 4;

  /* draw the scrollbar */
  for (n = 0; n < LAYOUT_VERT_SCROLLBAR_TRACK_HEIGHT; n++)
  {
    /* determine center of this piece */
    pos_x = top_x + (8 * 0);
    pos_y = top_y + (8 * n);

    /* top piece */
    if (n == 0)
    {
      cell_x = 7;
      cell_y = 6;
    }
    /* bottom piece */
    else if (n == LAYOUT_VERT_SCROLLBAR_TRACK_HEIGHT - 1)
    {
      cell_x = 7;
      cell_y = 8;
    }
    /* middle piece */
    else
    {
      cell_x = 7;
      cell_y = 7;
    }

    VB_ALL_ADD_SPRITE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, 1, 1, lighting, palette, WIDGETS)
  }

  /**************************/
  /* draw scrollbar slider  */
  /**************************/

  /* set lighting and palette */
  lighting = 0;
  palette = VB_ALL_PALETTE_1;

  /* determine coordinates of slider's top 8x8 cell */
  pos_x = LAYOUT_OVERSCAN_CENTER_X;
  pos_x += LAYOUT_VERT_SCROLLBAR_TRACK_X;

  pos_y = LAYOUT_OVERSCAN_CENTER_Y;
  pos_y += LAYOUT_VERT_SCROLLBAR_TRACK_Y;
  pos_y -= 4 * (LAYOUT_VERT_SCROLLBAR_TRACK_HEIGHT - 2);

  if (max_scroll_amount > 0)
    pos_y += (current_scroll_amount * 8 * (LAYOUT_VERT_SCROLLBAR_TRACK_HEIGHT - 2)) / max_scroll_amount;

  cell_x = 8;
  cell_y = 7;

  VB_ALL_ADD_SPRITE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, 1, 2, lighting, palette, TEXT)

  return 0;
}

/*******************************************************************************
** vb_all_load_horizontal_divider()
*******************************************************************************/
short int vb_all_load_horizontal_divider( int offset_x, int offset_y, 
                                          int width)
{
  int m;

  int sprite_index;

  int corner_x;
  int corner_y;

  int pos_x;
  int pos_y;

  int cell_x;
  int cell_y;

  int lighting;
  int palette;

  /* make sure the width and height are valid     */
  /* the width & height are in terms of 8x8 cells */
  if ((width < 3) || (width > LAYOUT_OVERSCAN_WIDTH / 8))
    return 1;

  /* determine coordinates of the center of the top left corner   */
  /* 8x8 cell. the offsets from the screen center are in pixels.  */
  corner_x = LAYOUT_OVERSCAN_CENTER_X + offset_x - 4 * (width - 1);
  corner_y = LAYOUT_OVERSCAN_CENTER_Y + offset_y - 4 * (1 - 1);

  /* set lighting and palette */
  lighting = 0;
  palette = VB_ALL_PALETTE_1;

  /* draw the divider */
  for (m = 0; m < width; m++)
  {
    /* determine center of this piece */
    pos_x = corner_x + (8 * m);
    pos_y = corner_y + (8 * 0);

    /* 1st / 2nd / 3rd piece */
    if (m % 3 == 0)
      cell_x = 9;
    else if (m % 3 == 1)
      cell_x = 10;
    else
      cell_x = 11;

    cell_y = 10;

    VB_ALL_ADD_SPRITE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, 1, 1, lighting, palette, WIDGETS)
  }

  return 0;
}

/*******************************************************************************
** vb_all_load_text()
*******************************************************************************/
short int vb_all_load_text( int offset_x, int offset_y, int align, 
                            int lighting, int palette, int max, char* str)
{
  int k;

  int length;

  int sprite_index;

  int start_x;
  int start_y;

  int pos_x;
  int pos_y;

  int cell_x;
  int cell_y;

  if (str == NULL)
    return 1;

  /* bound max length of text string */
  if (max < 1)
    max = 1;
  else if (max > 32)
    max = 32;

  /* determine length of text string */
  length = 0;

  while ((length < max) && (str[length] != '\0'))
    length++;

  if (length == 0)
    return 0;

  /* determine coordinates of center of first character */
  /* note that each character is 8x8                    */
  start_x = LAYOUT_OVERSCAN_CENTER_X + offset_x;
  start_y = LAYOUT_OVERSCAN_CENTER_Y + offset_y;

  /* left:    the center of the first character is at the offset coords */
  /* center:  the center of the whole string is at the offset coords    */
  /* right:   the center of the last character is at the offset coords  */
  if (align == VB_ALL_ALIGN_LEFT)
    start_x += 0;
  else if (align == VB_ALL_ALIGN_CENTER)
    start_x -= 4 * (length - 1);
  else if (align == VB_ALL_ALIGN_RIGHT)
    start_x -= 8 * (length - 1);

  /* bound lighting and palette */
  if ((lighting < -4) || (lighting > 3))
    lighting = 0;

  if ((palette < 0) || (palette > 15))
    palette = 0;

  /* draw the string */
  for (k = 0; k < length; k++)
  {
    /* determine center of this character */
    pos_x = start_x + (8 * k);
    pos_y = start_y;

    /* determine texture coordinates for this character */
    if ((str[k] > 32) && (str[k] < 48))
    {
      cell_x = str[k] - 32;
      cell_y = 0;
    }
    else if ((str[k] >= 48) && (str[k] < 64))
    {
      cell_x = str[k] - 48;
      cell_y = 1;
    }
    else if ((str[k] >= 64) && (str[k] < 80))
    {
      cell_x = str[k] - 64;
      cell_y = 2;
    }
    else if ((str[k] >= 80) && (str[k] < 96))
    {
      cell_x = str[k] - 80;
      cell_y = 3;
    }
    else if ((str[k] >= 96) && (str[k] < 112))
    {
      cell_x = str[k] - 96;
      cell_y = 4;
    }
    else if ((str[k] >= 112) && (str[k] < 127))
    {
      cell_x = str[k] - 112;
      cell_y = 5;
    }
    else
      continue;

    VB_ALL_ADD_SPRITE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, 1, 1, lighting, palette, TEXT)
  }

  return 0;
}

/*******************************************************************************
** vb_all_load_named_sprite()
*******************************************************************************/
short int vb_all_load_named_sprite( int name, 
                                    int offset_x, int offset_y, 
                                    int lighting, int palette)
{
  int sprite_index;

  int pos_x;
  int pos_y;

  int cell_x;
  int cell_y;

  int width;
  int height;

  /* determine coordinates of center of sprite */
  pos_x = LAYOUT_OVERSCAN_CENTER_X + offset_x;
  pos_y = LAYOUT_OVERSCAN_CENTER_Y + offset_y;

  /* determine cell coordinates, width & height */
  if (name == VB_ALL_SPRITE_NAME_PARAM_ARROWS_LEFT)
  {
    cell_x = 11;
    cell_y = 8;
    width = 1;
    height = 1;
  }
  else if (name == VB_ALL_SPRITE_NAME_PARAM_ARROWS_RIGHT)
  {
    cell_x = 12;
    cell_y = 8;
    width = 1;
    height = 1;
  }
  else if (name == VB_ALL_SPRITE_NAME_PARAM_RADIO_BUTTON_OFF)
  {
    cell_x = 9;
    cell_y = 8;
    width = 1;
    height = 1;
  }
  else if (name == VB_ALL_SPRITE_NAME_PARAM_RADIO_BUTTON_ON)
  {
    cell_x = 10;
    cell_y = 8;
    width = 1;
    height = 1;
  }
  else
    return 1;

  /* bound lighting and palette */
  if ((lighting < -4) || (lighting > 3))
    lighting = 0;

  if ((palette < 0) || (palette > 15))
    palette = 0;

  /* draw the sprite */
  VB_ALL_ADD_SPRITE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, width, height, lighting, palette, TEXT)

  return 0;
}

/*******************************************************************************
** vb_all_load_slider()
*******************************************************************************/
short int vb_all_load_slider( int offset_x, int offset_y, int width, 
                              short int value, 
                              short int lower_bound, short int upper_bound)
{
  int m;

  int sprite_index;

  int corner_x;
  int corner_y;

  int pos_x;
  int pos_y;

  int cell_x;
  int cell_y;

  int lighting;
  int palette;

  /* make sure the width is valid       */
  /* the width is in terms of 8x8 cells */
  if ((width < 2) || (width > LAYOUT_OVERSCAN_WIDTH / 8))
    return 1;

  /* make sure the bounds are valid */
  if (upper_bound <= lower_bound)
    return 1;

  if ((value < lower_bound) || (value > upper_bound))
    return 1;

  /* determine coordinates of the center of the top left corner   */
  /* 8x8 cell. the offsets from the screen center are in pixels.  */
  corner_x = LAYOUT_OVERSCAN_CENTER_X + offset_x - 4 * (width - 1);
  corner_y = LAYOUT_OVERSCAN_CENTER_Y + offset_y - 4 * (1 - 1);

  /* set lighting and palette */
  lighting = 0;
  palette = VB_ALL_PALETTE_1;

  /* draw the slider track */
  for (m = 0; m < width; m++)
  {
    /* determine center of this piece */
    pos_x = corner_x + (8 * m);
    pos_y = corner_y + (8 * 0);

    /* left / right / middle piece */
    if (m == 0)
      cell_x = 6;
    else if (m == width - 1)
      cell_x = 8;
    else
      cell_x = 7;

    cell_y = 9;

    VB_ALL_ADD_SPRITE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, 1, 1, lighting, palette, WIDGETS)
  }

  /* draw the slider itself */
  pos_x = corner_x;
  pos_y = corner_y;

  pos_x += ((value - lower_bound) * 8 * (width - 1)) / (upper_bound - lower_bound);

  cell_x = 9;
  cell_y = 9;

  lighting = 0;
  palette = VB_ALL_PALETTE_1;

  VB_ALL_ADD_SPRITE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, 1, 1, lighting, palette, TEXT)

  return 0;
}

/*******************************************************************************
** vb_all_load_top_panel()
*******************************************************************************/
short int vb_all_load_top_panel()
{
  int m;

  button* b;
  header* hd;

  /* reset sprite vbo counts */
  G_sprite_layer_count[GRAPHICS_SPRITE_LAYER_PANELS] = 0;
  G_sprite_layer_count[GRAPHICS_SPRITE_LAYER_WIDGETS] = 0;
  G_sprite_layer_count[GRAPHICS_SPRITE_LAYER_TEXT] = 0;

  /* top panel */
  vb_all_load_panel(LAYOUT_TOP_PANEL_AREA_X, LAYOUT_TOP_PANEL_AREA_Y, 
                    LAYOUT_TOP_PANEL_AREA_WIDTH, LAYOUT_TOP_PANEL_AREA_HEIGHT, 
                    LAYOUT_PANEL_TYPE_1);

  /* headers */
  for (m = 0; m < LAYOUT_NUM_TOP_PANEL_HEADERS; m++)
  {
    hd = &G_layout_top_panel_headers[m];

    vb_all_load_text( hd->center_x, hd->center_y, 
                      VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_GRAY, VB_ALL_HEADER_MAX_TEXT_SIZE, 
                      S_top_panel_header_names[m]);
  }

  /* buttons */
  for (m = 0; m < LAYOUT_NUM_TOP_PANEL_BUTTONS; m++)
  {
    b = &G_layout_top_panel_buttons[m];

    vb_all_load_button( b->center_x, b->center_y, 
                        b->width, b->state);

    vb_all_load_text( b->center_x, b->center_y, 
                      VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_GRAY, VB_ALL_BUTTON_MAX_TEXT_SIZE, 
                      S_top_panel_button_names[m]);
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

  button*   b;
  text_box* t;
  header*   hd;
  param*    pr;

  short int value;
  char*     value_string;

  int cart_index;
  int patch_index;

  /* determine cart & patch indices */
  cart_index = G_patch_edit_cart_number - PATCH_CART_NUMBER_LOWER_BOUND;
  patch_index = G_patch_edit_patch_number - PATCH_PATCH_NUMBER_LOWER_BOUND;

  /* make sure that the cart & patch indices are valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(cart_index))
    return 1;

  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain cart & patch pointers */
  cr = &G_cart_bank[cart_index];
  pt = &(cr->patches[patch_index]);

  /* vertical scrollbar track & slider */
  vb_all_load_vertical_scrollbar( G_current_scroll_amount, 
                                  G_max_scroll_amount);

  /* horizontal divider */
  if (LAYOUT_CART_HEADER_OR_PARAM_IS_IN_MAIN_AREA(LAYOUT_CART_MAIN_DIVIDER_Y))
  {
    vb_all_load_horizontal_divider( LAYOUT_CART_MAIN_DIVIDER_X, 
                                    LAYOUT_SCROLLED_POSITION_Y(LAYOUT_CART_MAIN_DIVIDER_Y), 
                                    LAYOUT_CART_MAIN_DIVIDER_WIDTH);
  }

  /* buttons */
  for (m = 0; m < LAYOUT_NUM_CART_BUTTONS; m++)
  {
    b = &G_layout_cart_buttons[m];

    if (LAYOUT_CART_BUTTON_OR_TEXT_BOX_IS_NOT_IN_MAIN_AREA(b->center_y))
      continue;

    /* load button & text */
    vb_all_load_button( b->center_x, LAYOUT_SCROLLED_POSITION_Y(b->center_y), 
                        b->width, b->state);

    vb_all_load_text( b->center_x, LAYOUT_SCROLLED_POSITION_Y(b->center_y), 
                      VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_GRAY, VB_ALL_BUTTON_MAX_TEXT_SIZE, 
                      S_cart_button_names[m]);
  }

  /* text boxes */
  for (m = 0; m < LAYOUT_NUM_CART_TEXT_BOXES; m++)
  {
    t = &G_layout_cart_text_boxes[m];

    if (LAYOUT_CART_BUTTON_OR_TEXT_BOX_IS_NOT_IN_MAIN_AREA(t->center_y))
      continue;

    /* set value string */
    if (m == LAYOUT_CART_TEXT_BOX_CART_NAME)
      value_string = &cr->name[0];
    else if (m == LAYOUT_CART_TEXT_BOX_PATCH_NAME)
      value_string = &pt->name[0];
    else
      value_string = NULL;

    /* load text box & text */
    vb_all_load_text_box( t->center_x, LAYOUT_SCROLLED_POSITION_Y(t->center_y), 
                          t->width, t->state);

    vb_all_load_text( t->center_x, LAYOUT_SCROLLED_POSITION_Y(t->center_y), 
                      VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_GRAY, VB_ALL_TEXT_BOX_COLUMN_MAX_TEXT_SIZE, 
                      value_string);
  }

  /* headers */
  for (m = 0; m < LAYOUT_NUM_CART_HEADERS; m++)
  {
    hd = &G_layout_cart_headers[m];

    if (LAYOUT_CART_HEADER_OR_PARAM_IS_NOT_IN_MAIN_AREA(hd->center_y))
      continue;

    /* load the header! */
    vb_all_load_text( hd->center_x, LAYOUT_SCROLLED_POSITION_Y(hd->center_y), 
                      VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_3, VB_ALL_HEADER_MAX_TEXT_SIZE, 
                      S_cart_header_names[m]);
  }

  /* parameters */
  for (m = 0; m < LAYOUT_NUM_CART_PARAMS; m++)
  {
    pr = &G_layout_cart_params[m];

    if (LAYOUT_CART_HEADER_OR_PARAM_IS_NOT_IN_MAIN_AREA(pr->center_y))
      continue;

    /* skip oscillator multiple/divisor or octave/note based on frequency mode */
    if (((pt->osc_freq_mode[0] == PATCH_OSC_FREQ_MODE_FIXED) && (m == LAYOUT_CART_PARAM_OSC_1_MULTIPLE))  || 
        ((pt->osc_freq_mode[1] == PATCH_OSC_FREQ_MODE_FIXED) && (m == LAYOUT_CART_PARAM_OSC_2_MULTIPLE))  || 
        ((pt->osc_freq_mode[2] == PATCH_OSC_FREQ_MODE_FIXED) && (m == LAYOUT_CART_PARAM_OSC_3_MULTIPLE))  || 
        ((pt->osc_freq_mode[0] == PATCH_OSC_FREQ_MODE_FIXED) && (m == LAYOUT_CART_PARAM_OSC_1_DIVISOR))   || 
        ((pt->osc_freq_mode[1] == PATCH_OSC_FREQ_MODE_FIXED) && (m == LAYOUT_CART_PARAM_OSC_2_DIVISOR))   || 
        ((pt->osc_freq_mode[2] == PATCH_OSC_FREQ_MODE_FIXED) && (m == LAYOUT_CART_PARAM_OSC_3_DIVISOR))   || 
        ((pt->osc_freq_mode[0] == PATCH_OSC_FREQ_MODE_RATIO) && (m == LAYOUT_CART_PARAM_OSC_1_OCTAVE))    || 
        ((pt->osc_freq_mode[1] == PATCH_OSC_FREQ_MODE_RATIO) && (m == LAYOUT_CART_PARAM_OSC_2_OCTAVE))    || 
        ((pt->osc_freq_mode[2] == PATCH_OSC_FREQ_MODE_RATIO) && (m == LAYOUT_CART_PARAM_OSC_3_OCTAVE))    || 
        ((pt->osc_freq_mode[0] == PATCH_OSC_FREQ_MODE_RATIO) && (m == LAYOUT_CART_PARAM_OSC_1_NOTE))      || 
        ((pt->osc_freq_mode[1] == PATCH_OSC_FREQ_MODE_RATIO) && (m == LAYOUT_CART_PARAM_OSC_2_NOTE))      || 
        ((pt->osc_freq_mode[2] == PATCH_OSC_FREQ_MODE_RATIO) && (m == LAYOUT_CART_PARAM_OSC_3_NOTE)))
    {
      continue;
    }

    /* determine parameter value & string */
    switch (m)
    {
      VB_ALL_CART_PARAM_CASE_INT(CART_NUMBER,   G_patch_edit_cart_number,   common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(PATCH_NUMBER,  G_patch_edit_patch_number,  common_edit_1_to_100)

      VB_ALL_CART_PARAM_CASE_INT(ALGORITHM, pt->algorithm,  patch_edit_algorithm)
      VB_ALL_CART_PARAM_CASE_INT(OSC_SYNC,  pt->osc_sync,   patch_edit_sync)

      VB_ALL_CART_PARAM_CASE_INT(OSC_1_WAVEFORM,  pt->osc_waveform[0],  patch_edit_osc_waveform)
      VB_ALL_CART_PARAM_CASE_INT(OSC_1_PHI,       pt->osc_phi[0],       patch_edit_osc_phi)
      VB_ALL_CART_PARAM_CASE_INT(OSC_1_FREQ_MODE, pt->osc_freq_mode[0], patch_edit_osc_freq_mode)
      VB_ALL_CART_PARAM_CASE_INT(OSC_1_MULTIPLE,  pt->osc_multiple[0],  common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(OSC_1_DIVISOR,   pt->osc_divisor[0],   common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(OSC_1_OCTAVE,    pt->osc_octave[0],    patch_edit_osc_octave)
      VB_ALL_CART_PARAM_CASE_INT(OSC_1_NOTE,      pt->osc_note[0],      patch_edit_osc_note)
      VB_ALL_CART_PARAM_CASE_INT(OSC_1_DETUNE,    pt->osc_detune[0],    patch_edit_osc_detune)

      VB_ALL_CART_PARAM_CASE_FLAG(OSC_1_ROUTING_VIBRATO,      pt->osc_routing[0], patch_edit_routing)
      VB_ALL_CART_PARAM_CASE_FLAG(OSC_1_ROUTING_PITCH_ENV,    pt->osc_routing[0], patch_edit_routing)
      VB_ALL_CART_PARAM_CASE_FLAG(OSC_1_ROUTING_PITCH_WHEEL,  pt->osc_routing[0], patch_edit_routing)

      VB_ALL_CART_PARAM_CASE_INT(OSC_2_WAVEFORM,  pt->osc_waveform[1],  patch_edit_osc_waveform)
      VB_ALL_CART_PARAM_CASE_INT(OSC_2_PHI,       pt->osc_phi[1],       patch_edit_osc_phi)
      VB_ALL_CART_PARAM_CASE_INT(OSC_2_FREQ_MODE, pt->osc_freq_mode[1], patch_edit_osc_freq_mode)
      VB_ALL_CART_PARAM_CASE_INT(OSC_2_MULTIPLE,  pt->osc_multiple[1],  common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(OSC_2_DIVISOR,   pt->osc_divisor[1],   common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(OSC_2_OCTAVE,    pt->osc_octave[1],    patch_edit_osc_octave)
      VB_ALL_CART_PARAM_CASE_INT(OSC_2_NOTE,      pt->osc_note[1],      patch_edit_osc_note)
      VB_ALL_CART_PARAM_CASE_INT(OSC_2_DETUNE,    pt->osc_detune[1],    patch_edit_osc_detune)

      VB_ALL_CART_PARAM_CASE_FLAG(OSC_2_ROUTING_VIBRATO,      pt->osc_routing[1], patch_edit_routing)
      VB_ALL_CART_PARAM_CASE_FLAG(OSC_2_ROUTING_PITCH_ENV,    pt->osc_routing[1], patch_edit_routing)
      VB_ALL_CART_PARAM_CASE_FLAG(OSC_2_ROUTING_PITCH_WHEEL,  pt->osc_routing[1], patch_edit_routing)

      VB_ALL_CART_PARAM_CASE_INT(OSC_3_WAVEFORM,  pt->osc_waveform[2],  patch_edit_osc_waveform)
      VB_ALL_CART_PARAM_CASE_INT(OSC_3_PHI,       pt->osc_phi[2],       patch_edit_osc_phi)
      VB_ALL_CART_PARAM_CASE_INT(OSC_3_FREQ_MODE, pt->osc_freq_mode[2], patch_edit_osc_freq_mode)
      VB_ALL_CART_PARAM_CASE_INT(OSC_3_MULTIPLE,  pt->osc_multiple[2],  common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(OSC_3_DIVISOR,   pt->osc_divisor[2],   common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(OSC_3_OCTAVE,    pt->osc_octave[2],    patch_edit_osc_octave)
      VB_ALL_CART_PARAM_CASE_INT(OSC_3_NOTE,      pt->osc_note[2],      patch_edit_osc_note)
      VB_ALL_CART_PARAM_CASE_INT(OSC_3_DETUNE,    pt->osc_detune[2],    patch_edit_osc_detune)

      VB_ALL_CART_PARAM_CASE_FLAG(OSC_3_ROUTING_VIBRATO,      pt->osc_routing[2], patch_edit_routing)
      VB_ALL_CART_PARAM_CASE_FLAG(OSC_3_ROUTING_PITCH_ENV,    pt->osc_routing[2], patch_edit_routing)
      VB_ALL_CART_PARAM_CASE_FLAG(OSC_3_ROUTING_PITCH_WHEEL,  pt->osc_routing[2], patch_edit_routing)

      VB_ALL_CART_PARAM_CASE_INT(ENV_1_ATTACK,      pt->env_attack[0],      common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(ENV_1_DECAY,       pt->env_decay[0],       common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(ENV_1_SUSTAIN,     pt->env_sustain[0],     common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(ENV_1_RELEASE,     pt->env_release[0],     common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(ENV_1_AMPLITUDE,   pt->env_amplitude[0],   common_edit_0_to_100)
      VB_ALL_CART_PARAM_CASE_INT(ENV_1_HOLD_LEVEL,  pt->env_hold_level[0],  common_edit_0_to_100)
      VB_ALL_CART_PARAM_CASE_INT(ENV_1_HOLD_MODE,   pt->env_hold_mode[0],   patch_edit_env_hold_mode)
      VB_ALL_CART_PARAM_CASE_INT(ENV_1_RATE_KS,     pt->env_rate_ks[0],     common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(ENV_1_LEVEL_KS,    pt->env_level_ks[0],    common_edit_1_to_100)

      VB_ALL_CART_PARAM_CASE_FLAG(ENV_1_ROUTING_TREMOLO,  pt->env_routing[0], patch_edit_routing)
      VB_ALL_CART_PARAM_CASE_FLAG(ENV_1_ROUTING_BOOST,    pt->env_routing[0], patch_edit_routing)
      VB_ALL_CART_PARAM_CASE_FLAG(ENV_1_ROUTING_VELOCITY, pt->env_routing[0], patch_edit_routing)

      VB_ALL_CART_PARAM_CASE_INT(ENV_2_ATTACK,      pt->env_attack[1],      common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(ENV_2_DECAY,       pt->env_decay[1],       common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(ENV_2_SUSTAIN,     pt->env_sustain[1],     common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(ENV_2_RELEASE,     pt->env_release[1],     common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(ENV_2_AMPLITUDE,   pt->env_amplitude[1],   common_edit_0_to_100)
      VB_ALL_CART_PARAM_CASE_INT(ENV_2_HOLD_LEVEL,  pt->env_hold_level[1],  common_edit_0_to_100)
      VB_ALL_CART_PARAM_CASE_INT(ENV_2_HOLD_MODE,   pt->env_hold_mode[1],   patch_edit_env_hold_mode)
      VB_ALL_CART_PARAM_CASE_INT(ENV_2_RATE_KS,     pt->env_rate_ks[1],     common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(ENV_2_LEVEL_KS,    pt->env_level_ks[1],    common_edit_1_to_100)

      VB_ALL_CART_PARAM_CASE_FLAG(ENV_2_ROUTING_TREMOLO,  pt->env_routing[1], patch_edit_routing)
      VB_ALL_CART_PARAM_CASE_FLAG(ENV_2_ROUTING_BOOST,    pt->env_routing[1], patch_edit_routing)
      VB_ALL_CART_PARAM_CASE_FLAG(ENV_2_ROUTING_VELOCITY, pt->env_routing[1], patch_edit_routing)

      VB_ALL_CART_PARAM_CASE_INT(ENV_3_ATTACK,      pt->env_attack[2],      common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(ENV_3_DECAY,       pt->env_decay[2],       common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(ENV_3_SUSTAIN,     pt->env_sustain[2],     common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(ENV_3_RELEASE,     pt->env_release[2],     common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(ENV_3_AMPLITUDE,   pt->env_amplitude[2],   common_edit_0_to_100)
      VB_ALL_CART_PARAM_CASE_INT(ENV_3_HOLD_LEVEL,  pt->env_hold_level[2],  common_edit_0_to_100)
      VB_ALL_CART_PARAM_CASE_INT(ENV_3_HOLD_MODE,   pt->env_hold_mode[2],   patch_edit_env_hold_mode)
      VB_ALL_CART_PARAM_CASE_INT(ENV_3_RATE_KS,     pt->env_rate_ks[2],     common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(ENV_3_LEVEL_KS,    pt->env_level_ks[2],    common_edit_1_to_100)

      VB_ALL_CART_PARAM_CASE_FLAG(ENV_3_ROUTING_TREMOLO,  pt->env_routing[2], patch_edit_routing)
      VB_ALL_CART_PARAM_CASE_FLAG(ENV_3_ROUTING_BOOST,    pt->env_routing[2], patch_edit_routing)
      VB_ALL_CART_PARAM_CASE_FLAG(ENV_3_ROUTING_VELOCITY, pt->env_routing[2], patch_edit_routing)

      VB_ALL_CART_PARAM_CASE_INT(VIBRATO_WAVEFORM,    pt->lfo_waveform[0],    patch_edit_lfo_waveform)
      VB_ALL_CART_PARAM_CASE_INT(VIBRATO_DELAY,       pt->lfo_delay[0],       common_edit_0_to_100)
      VB_ALL_CART_PARAM_CASE_INT(VIBRATO_SPEED,       pt->lfo_speed[0],       common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(VIBRATO_DEPTH,       pt->lfo_depth[0],       common_edit_0_to_100)
      VB_ALL_CART_PARAM_CASE_INT(VIBRATO_SENSITIVITY, pt->lfo_sensitivity[0], common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(VIBRATO_SYNC,        pt->lfo_sync[0],        patch_edit_sync)
      VB_ALL_CART_PARAM_CASE_INT(VIBRATO_POLARITY,    pt->lfo_polarity[0],    patch_edit_lfo_polarity)

      VB_ALL_CART_PARAM_CASE_INT(TREMOLO_WAVEFORM,    pt->lfo_waveform[1],    patch_edit_lfo_waveform)
      VB_ALL_CART_PARAM_CASE_INT(TREMOLO_DELAY,       pt->lfo_delay[1],       common_edit_0_to_100)
      VB_ALL_CART_PARAM_CASE_INT(TREMOLO_SPEED,       pt->lfo_speed[1],       common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(TREMOLO_DEPTH,       pt->lfo_depth[1],       common_edit_0_to_100)
      VB_ALL_CART_PARAM_CASE_INT(TREMOLO_SENSITIVITY, pt->lfo_sensitivity[1], common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(TREMOLO_SYNC,        pt->lfo_sync[1],        patch_edit_sync)

      VB_ALL_CART_PARAM_CASE_INT(CHORUS_WAVEFORM,     pt->lfo_waveform[2],    patch_edit_lfo_waveform)
      VB_ALL_CART_PARAM_CASE_INT(CHORUS_DELAY,        pt->lfo_delay[2],       common_edit_0_to_100)
      VB_ALL_CART_PARAM_CASE_INT(CHORUS_SPEED,        pt->lfo_speed[2],       common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(CHORUS_DEPTH,        pt->lfo_depth[2],       common_edit_0_to_100)
      VB_ALL_CART_PARAM_CASE_INT(CHORUS_SENSITIVITY,  pt->lfo_sensitivity[2], common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(CHORUS_SYNC,         pt->lfo_sync[2],        patch_edit_sync)

      VB_ALL_CART_PARAM_CASE_INT(BOOST_SENSITIVITY,     pt->boost_sensitivity,    common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(VELOCITY_SENSITIVITY,  pt->velocity_sensitivity, common_edit_1_to_100)

      VB_ALL_CART_PARAM_CASE_INT(FILTERS_HIGHPASS,  pt->highpass_cutoff,  patch_edit_highpass_cutoff)
      VB_ALL_CART_PARAM_CASE_INT(FILTERS_LOWPASS,   pt->lowpass_cutoff,   patch_edit_lowpass_cutoff)

      VB_ALL_CART_PARAM_CASE_INT(PITCH_ENV_ATTACK,  pt->peg_attack,   common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(PITCH_ENV_DECAY,   pt->peg_decay,    common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(PITCH_ENV_RELEASE, pt->peg_release,  common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(PITCH_ENV_MAXIMUM, pt->peg_maximum,  patch_edit_peg_level)
      VB_ALL_CART_PARAM_CASE_INT(PITCH_ENV_FINALE,  pt->peg_finale,   patch_edit_peg_level)

      VB_ALL_CART_PARAM_CASE_INT(PITCH_WHEEL_MODE,  pt->pitch_wheel_mode,   patch_edit_pitch_wheel_mode)
      VB_ALL_CART_PARAM_CASE_INT(PITCH_WHEEL_RANGE, pt->pitch_wheel_range,  common_edit_1_to_100)

      VB_ALL_CART_PARAM_CASE_INT(ARPEGGIO_MODE,     pt->arpeggio_mode,      patch_edit_arpeggio_mode)
      VB_ALL_CART_PARAM_CASE_INT(ARPEGGIO_PATTERN,  pt->arpeggio_pattern,   patch_edit_arpeggio_pattern)
      VB_ALL_CART_PARAM_CASE_INT(ARPEGGIO_OCTAVES,  pt->arpeggio_octaves,   common_edit_1_to_100)
      VB_ALL_CART_PARAM_CASE_INT(ARPEGGIO_SPEED,    pt->arpeggio_speed,     common_edit_1_to_100)

      VB_ALL_CART_PARAM_CASE_INT(PORTAMENTO_MODE,   pt->portamento_mode,    patch_edit_portamento_mode)
      VB_ALL_CART_PARAM_CASE_INT(PORTAMENTO_LEGATO, pt->portamento_legato,  patch_edit_portamento_legato)
      VB_ALL_CART_PARAM_CASE_INT(PORTAMENTO_SPEED,  pt->portamento_speed,   common_edit_1_to_100)

      VB_ALL_CART_PARAM_CASE_FLAG(MOD_WHEEL_ROUTING_VIBRATO,  pt->mod_wheel_routing,  patch_edit_routing)
      VB_ALL_CART_PARAM_CASE_FLAG(MOD_WHEEL_ROUTING_TREMOLO,  pt->mod_wheel_routing,  patch_edit_routing)
      VB_ALL_CART_PARAM_CASE_FLAG(MOD_WHEEL_ROUTING_BOOST,    pt->mod_wheel_routing,  patch_edit_routing)
      VB_ALL_CART_PARAM_CASE_FLAG(MOD_WHEEL_ROUTING_CHORUS,   pt->mod_wheel_routing,  patch_edit_routing)

      VB_ALL_CART_PARAM_CASE_FLAG(AFTERTOUCH_ROUTING_VIBRATO, pt->aftertouch_routing, patch_edit_routing)
      VB_ALL_CART_PARAM_CASE_FLAG(AFTERTOUCH_ROUTING_TREMOLO, pt->aftertouch_routing, patch_edit_routing)
      VB_ALL_CART_PARAM_CASE_FLAG(AFTERTOUCH_ROUTING_BOOST,   pt->aftertouch_routing, patch_edit_routing)
      VB_ALL_CART_PARAM_CASE_FLAG(AFTERTOUCH_ROUTING_CHORUS,  pt->aftertouch_routing, patch_edit_routing)

      VB_ALL_CART_PARAM_CASE_FLAG(EXP_PEDAL_ROUTING_VIBRATO,  pt->exp_pedal_routing,  patch_edit_routing)
      VB_ALL_CART_PARAM_CASE_FLAG(EXP_PEDAL_ROUTING_TREMOLO,  pt->exp_pedal_routing,  patch_edit_routing)
      VB_ALL_CART_PARAM_CASE_FLAG(EXP_PEDAL_ROUTING_BOOST,    pt->exp_pedal_routing,  patch_edit_routing)
      VB_ALL_CART_PARAM_CASE_FLAG(EXP_PEDAL_ROUTING_CHORUS,   pt->exp_pedal_routing,  patch_edit_routing)

      default:
      {
        value = 0;
        value_string = NULL;
        break;
      }
    }

    /* load the parameter name, value, and slider, arrows, or radio button */
    vb_all_load_text( pr->center_x + LAYOUT_PARAM_NAME_X, 
                      LAYOUT_SCROLLED_POSITION_Y(pr->center_y), 
                      VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_2, VB_ALL_PARAM_NAME_MAX_TEXT_SIZE, 
                      S_cart_param_names[m]);

    if (pr->type == LAYOUT_PARAM_TYPE_SLIDER)
    {
      vb_all_load_text( pr->center_x + LAYOUT_PARAM_SLIDER_VALUE_X, 
                        LAYOUT_SCROLLED_POSITION_Y(pr->center_y), 
                        VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_GRAY, VB_ALL_PARAM_NUMBER_MAX_TEXT_SIZE, 
                        value_string);

      vb_all_load_slider( pr->center_x + LAYOUT_PARAM_SLIDER_TRACK_X, 
                          LAYOUT_SCROLLED_POSITION_Y(pr->center_y), 
                          LAYOUT_PARAM_SLIDER_WIDTH, 
                          value, pr->lower_bound, pr->upper_bound);
    }
    else if (pr->type == LAYOUT_PARAM_TYPE_ARROWS)
    {
      vb_all_load_text( pr->center_x + LAYOUT_PARAM_ARROWS_VALUE_X, 
                        LAYOUT_SCROLLED_POSITION_Y(pr->center_y), 
                        VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_GRAY, VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE, 
                        value_string);

      if (value > pr->lower_bound)
      {
        vb_all_load_named_sprite( VB_ALL_SPRITE_NAME_PARAM_ARROWS_LEFT, 
                                  pr->center_x + LAYOUT_PARAM_ARROWS_LEFT_X, 
                                  LAYOUT_SCROLLED_POSITION_Y(pr->center_y), 
                                  0, VB_ALL_PALETTE_1);
      }

      if (value < pr->upper_bound)
      {
        vb_all_load_named_sprite( VB_ALL_SPRITE_NAME_PARAM_ARROWS_RIGHT, 
                                  pr->center_x + LAYOUT_PARAM_ARROWS_RIGHT_X, 
                                  LAYOUT_SCROLLED_POSITION_Y(pr->center_y), 
                                  0, VB_ALL_PALETTE_1);
      }
    }
    else if (pr->type == LAYOUT_PARAM_TYPE_RADIO)
    {
      vb_all_load_text( pr->center_x + LAYOUT_PARAM_RADIO_VALUE_X, 
                        LAYOUT_SCROLLED_POSITION_Y(pr->center_y), 
                        VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_GRAY, VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE, 
                        value_string);

      if (value == pr->lower_bound)
      {
        vb_all_load_named_sprite( VB_ALL_SPRITE_NAME_PARAM_RADIO_BUTTON_OFF, 
                                  pr->center_x + LAYOUT_PARAM_RADIO_BUTTON_X, 
                                  LAYOUT_SCROLLED_POSITION_Y(pr->center_y), 
                                  0, VB_ALL_PALETTE_1);
      }
      else
      {
        vb_all_load_named_sprite( VB_ALL_SPRITE_NAME_PARAM_RADIO_BUTTON_ON, 
                                  pr->center_x + LAYOUT_PARAM_RADIO_BUTTON_X, 
                                  LAYOUT_SCROLLED_POSITION_Y(pr->center_y), 
                                  0, VB_ALL_PALETTE_1);
      }
    }
    else if (pr->type == LAYOUT_PARAM_TYPE_FLAG)
    {
      vb_all_load_text( pr->center_x + LAYOUT_PARAM_RADIO_VALUE_X, 
                        LAYOUT_SCROLLED_POSITION_Y(pr->center_y), 
                        VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_GRAY, VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE, 
                        value_string);

      if ((value & pr->upper_bound) == 0)
      {
        vb_all_load_named_sprite( VB_ALL_SPRITE_NAME_PARAM_RADIO_BUTTON_OFF, 
                                  pr->center_x + LAYOUT_PARAM_RADIO_BUTTON_X, 
                                  LAYOUT_SCROLLED_POSITION_Y(pr->center_y), 
                                  0, VB_ALL_PALETTE_1);
      }
      else
      {
        vb_all_load_named_sprite( VB_ALL_SPRITE_NAME_PARAM_RADIO_BUTTON_ON, 
                                  pr->center_x + LAYOUT_PARAM_RADIO_BUTTON_X, 
                                  LAYOUT_SCROLLED_POSITION_Y(pr->center_y), 
                                  0, VB_ALL_PALETTE_1);
      }
    }
  }

  /* update vbos */
  VB_ALL_UPDATE_SPRITES_IN_VBOS(PANELS)
  VB_ALL_UPDATE_SPRITES_IN_VBOS(WIDGETS)
  VB_ALL_UPDATE_SPRITES_IN_VBOS(TEXT)

  return 0;
}

/*******************************************************************************
** vb_all_load_audition_panel()
*******************************************************************************/
short int vb_all_load_audition_panel()
{
  int m;

  param*    pr;

  short int value;
  char*     value_string;

  /* panel */
  vb_all_load_panel(LAYOUT_CART_AUDITION_PANEL_X, LAYOUT_CART_AUDITION_PANEL_Y, 
                    LAYOUT_CART_AUDITION_PANEL_WIDTH, LAYOUT_CART_AUDITION_PANEL_HEIGHT, 
                    LAYOUT_PANEL_TYPE_2);

  /* parameters */
  for (m = 0; m < LAYOUT_NUM_AUDITION_PARAMS; m++)
  {
    pr = &G_layout_audition_params[m];

    /* determine parameter value & string */
    switch (m)
    {
      VB_ALL_AUDITION_PARAM_CASE(OCTAVE,      G_patch_edit_octave,          common_edit_1_to_100)
      VB_ALL_AUDITION_PARAM_CASE(VELOCITY,    G_patch_edit_note_velocity,   audition_uni_wheel)
      VB_ALL_AUDITION_PARAM_CASE(PITCH_WHEEL, G_patch_edit_pitch_wheel_pos, audition_bi_wheel)
      VB_ALL_AUDITION_PARAM_CASE(MOD_WHEEL,   G_patch_edit_mod_wheel_pos,   audition_uni_wheel)
      VB_ALL_AUDITION_PARAM_CASE(AFTERTOUCH,  G_patch_edit_aftertouch_pos,  audition_uni_wheel)
      VB_ALL_AUDITION_PARAM_CASE(EXP_PEDAL,   G_patch_edit_exp_pedal_pos,   audition_uni_wheel)

      default:
      {
        value = 0;
        value_string = NULL;
        break;
      }
    }

    /* load the parameter name, value, and slider, arrows, or radio button */
    vb_all_load_text( pr->center_x + LAYOUT_PARAM_NAME_X, 
                      pr->center_y, 
                      VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_2, VB_ALL_PARAM_NAME_MAX_TEXT_SIZE, 
                      S_audition_param_names[m]);

    if (pr->type == LAYOUT_PARAM_TYPE_SLIDER)
    {
      vb_all_load_text( pr->center_x + LAYOUT_PARAM_SLIDER_VALUE_X, 
                        pr->center_y, 
                        VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_GRAY, VB_ALL_PARAM_NUMBER_MAX_TEXT_SIZE, 
                        value_string);

      vb_all_load_slider( pr->center_x + LAYOUT_PARAM_SLIDER_TRACK_X, 
                          pr->center_y, 
                          LAYOUT_PARAM_SLIDER_WIDTH, 
                          value, pr->lower_bound, pr->upper_bound);
    }
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

