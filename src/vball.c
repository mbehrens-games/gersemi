/*******************************************************************************
** vball.c (vbo population functions - all)
*******************************************************************************/

#include <glad/glad.h>

#include <stdio.h>
#include <stdlib.h>

#include "bank.h"
#include "controls.h"
#include "global.h"
#include "graphics.h"
#include "layout.h"
#include "midicont.h"
#include "palette.h"
#include "patch.h"
#include "screen.h"
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

#define VB_ALL_BUTTON_MAX_TEXT_SIZE 12
#define VB_ALL_HEADER_MAX_TEXT_SIZE 16

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
/* position is the center, width & height are in 8x8 cells */
#define VB_ALL_ADD_TILE_TO_VERTEX_BUFFER(pos_x, pos_y, width, height, z)     \
  G_vertex_buffer_tiles[12 * tile_index + 0]   = pos_x - (4 * width);        \
  G_vertex_buffer_tiles[12 * tile_index + 1]   = pos_y - (4 * height);       \
  G_vertex_buffer_tiles[12 * tile_index + 2]   = z;                          \
                                                                             \
  G_vertex_buffer_tiles[12 * tile_index + 3]   = pos_x + (4 * width);        \
  G_vertex_buffer_tiles[12 * tile_index + 4]   = pos_y - (4 * height);       \
  G_vertex_buffer_tiles[12 * tile_index + 5]   = z;                          \
                                                                             \
  G_vertex_buffer_tiles[12 * tile_index + 6]   = pos_x - (4 * width);        \
  G_vertex_buffer_tiles[12 * tile_index + 7]   = pos_y + (4 * height);       \
  G_vertex_buffer_tiles[12 * tile_index + 8]   = z;                          \
                                                                             \
  G_vertex_buffer_tiles[12 * tile_index + 9]   = pos_x + (4 * width);        \
  G_vertex_buffer_tiles[12 * tile_index + 10]  = pos_y + (4 * height);       \
  G_vertex_buffer_tiles[12 * tile_index + 11]  = z;

/* cell_x and cell_y are the top left corner, width & height are in 8x8 cells */
#define VB_ALL_ADD_TILE_TO_TEXTURE_COORD_BUFFER(cell_x, cell_y, width, height)                                    \
  G_texture_coord_buffer_tiles[8 * tile_index + 0] = G_texture_coord_table[cell_x];                               \
  G_texture_coord_buffer_tiles[8 * tile_index + 1] = G_texture_coord_table[TEXTURE_NUM_CELLS - cell_y];           \
                                                                                                                  \
  G_texture_coord_buffer_tiles[8 * tile_index + 2] = G_texture_coord_table[cell_x + width];                       \
  G_texture_coord_buffer_tiles[8 * tile_index + 3] = G_texture_coord_table[TEXTURE_NUM_CELLS - cell_y];           \
                                                                                                                  \
  G_texture_coord_buffer_tiles[8 * tile_index + 4] = G_texture_coord_table[cell_x];                               \
  G_texture_coord_buffer_tiles[8 * tile_index + 5] = G_texture_coord_table[TEXTURE_NUM_CELLS - height - cell_y];  \
                                                                                                                  \
  G_texture_coord_buffer_tiles[8 * tile_index + 6] = G_texture_coord_table[cell_x + width];                       \
  G_texture_coord_buffer_tiles[8 * tile_index + 7] = G_texture_coord_table[TEXTURE_NUM_CELLS - height - cell_y];

#define VB_ALL_ADD_TILE_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)                                           \
  G_lighting_and_palette_buffer_tiles[8 * tile_index + 0] = G_lighting_coord_table[PALETTE_BASE_LEVEL + lighting];  \
  G_lighting_and_palette_buffer_tiles[8 * tile_index + 1] = G_palette_coord_table[palette];                         \
                                                                                                                    \
  G_lighting_and_palette_buffer_tiles[8 * tile_index + 2] = G_lighting_coord_table[PALETTE_BASE_LEVEL + lighting];  \
  G_lighting_and_palette_buffer_tiles[8 * tile_index + 3] = G_palette_coord_table[palette];                         \
                                                                                                                    \
  G_lighting_and_palette_buffer_tiles[8 * tile_index + 4] = G_lighting_coord_table[PALETTE_BASE_LEVEL + lighting];  \
  G_lighting_and_palette_buffer_tiles[8 * tile_index + 5] = G_palette_coord_table[palette];                         \
                                                                                                                    \
  G_lighting_and_palette_buffer_tiles[8 * tile_index + 6] = G_lighting_coord_table[PALETTE_BASE_LEVEL + lighting];  \
  G_lighting_and_palette_buffer_tiles[8 * tile_index + 7] = G_palette_coord_table[palette];

#define VB_ALL_ADD_TILE_TO_ELEMENT_BUFFER()                                    \
  G_index_buffer_tiles[6 * tile_index + 0] = 4 * tile_index + 0;               \
  G_index_buffer_tiles[6 * tile_index + 1] = 4 * tile_index + 2;               \
  G_index_buffer_tiles[6 * tile_index + 2] = 4 * tile_index + 1;               \
                                                                               \
  G_index_buffer_tiles[6 * tile_index + 3] = 4 * tile_index + 1;               \
  G_index_buffer_tiles[6 * tile_index + 4] = 4 * tile_index + 2;               \
  G_index_buffer_tiles[6 * tile_index + 5] = 4 * tile_index + 3;

#define VB_ALL_ADD_BACKGROUND_TILE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette)  \
  if (tile_index < GRAPHICS_BACKGROUND_TILES_END_INDEX)                                         \
  {                                                                                             \
    VB_ALL_ADD_TILE_TO_VERTEX_BUFFER(pos_x, pos_y, 2, 2, GRAPHICS_Z_LEVEL_BACKGROUND)           \
    VB_ALL_ADD_TILE_TO_TEXTURE_COORD_BUFFER(cell_x, cell_y, 2, 2)                               \
    VB_ALL_ADD_TILE_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)                           \
    VB_ALL_ADD_TILE_TO_ELEMENT_BUFFER()                                                         \
                                                                                                \
    tile_index += 1;                                                                            \
  }

#define VB_ALL_UPDATE_BACKGROUND_TILES_IN_VBOS()                                                    \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_tiles);                                          \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                  \
                  GRAPHICS_BACKGROUND_TILES_START_INDEX * 12 * sizeof(GLfloat),                     \
                  G_tile_layer_counts[GRAPHICS_TILE_LAYER_BACKGROUND] * 12 * sizeof(GLfloat),       \
                  &G_vertex_buffer_tiles[GRAPHICS_BACKGROUND_TILES_START_INDEX * 12]);              \
                                                                                                    \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_tiles);                                   \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                  \
                  GRAPHICS_BACKGROUND_TILES_START_INDEX * 8 * sizeof(GLfloat),                      \
                  G_tile_layer_counts[GRAPHICS_TILE_LAYER_BACKGROUND] * 8 * sizeof(GLfloat),        \
                  &G_texture_coord_buffer_tiles[GRAPHICS_BACKGROUND_TILES_START_INDEX * 8]);        \
                                                                                                    \
  glBindBuffer(GL_ARRAY_BUFFER, G_lighting_and_palette_buffer_id_tiles);                            \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                  \
                  GRAPHICS_BACKGROUND_TILES_START_INDEX * 8 * sizeof(GLfloat),                      \
                  G_tile_layer_counts[GRAPHICS_TILE_LAYER_BACKGROUND] * 8 * sizeof(GLfloat),        \
                  &G_lighting_and_palette_buffer_tiles[GRAPHICS_BACKGROUND_TILES_START_INDEX * 8]); \
                                                                                                    \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_tiles);                                   \
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,                                                          \
                  GRAPHICS_BACKGROUND_TILES_START_INDEX * 6 * sizeof(unsigned short),               \
                  G_tile_layer_counts[GRAPHICS_TILE_LAYER_BACKGROUND] * 6 * sizeof(unsigned short), \
                  &G_index_buffer_tiles[GRAPHICS_BACKGROUND_TILES_START_INDEX * 6]);

/* sprites */
/* position is the center, width & height are in 8x8 cells */
#define VB_ALL_ADD_SPRITE_TO_VERTEX_BUFFER(pos_x, pos_y, width, height, z)     \
  G_vertex_buffer_sprites[12 * sprite_index + 0]   = pos_x - (4 * width);      \
  G_vertex_buffer_sprites[12 * sprite_index + 1]   = pos_y - (4 * height);     \
  G_vertex_buffer_sprites[12 * sprite_index + 2]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 3]   = pos_x + (4 * width);      \
  G_vertex_buffer_sprites[12 * sprite_index + 4]   = pos_y - (4 * height);     \
  G_vertex_buffer_sprites[12 * sprite_index + 5]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 6]   = pos_x - (4 * width);      \
  G_vertex_buffer_sprites[12 * sprite_index + 7]   = pos_y + (4 * height);     \
  G_vertex_buffer_sprites[12 * sprite_index + 8]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 9]   = pos_x + (4 * width);      \
  G_vertex_buffer_sprites[12 * sprite_index + 10]  = pos_y + (4 * height);     \
  G_vertex_buffer_sprites[12 * sprite_index + 11]  = z;

/* cell_x and cell_y are the top left corner, width & height are in 8x8 cells */
#define VB_ALL_ADD_SPRITE_TO_TEXTURE_COORD_BUFFER(cell_x, cell_y, width, height)                                      \
  G_texture_coord_buffer_sprites[8 * sprite_index + 0] = G_texture_coord_table[cell_x];                               \
  G_texture_coord_buffer_sprites[8 * sprite_index + 1] = G_texture_coord_table[TEXTURE_NUM_CELLS - cell_y];           \
                                                                                                                      \
  G_texture_coord_buffer_sprites[8 * sprite_index + 2] = G_texture_coord_table[cell_x + width];                       \
  G_texture_coord_buffer_sprites[8 * sprite_index + 3] = G_texture_coord_table[TEXTURE_NUM_CELLS - cell_y];           \
                                                                                                                      \
  G_texture_coord_buffer_sprites[8 * sprite_index + 4] = G_texture_coord_table[cell_x];                               \
  G_texture_coord_buffer_sprites[8 * sprite_index + 5] = G_texture_coord_table[TEXTURE_NUM_CELLS - height - cell_y];  \
                                                                                                                      \
  G_texture_coord_buffer_sprites[8 * sprite_index + 6] = G_texture_coord_table[cell_x + width];                       \
  G_texture_coord_buffer_sprites[8 * sprite_index + 7] = G_texture_coord_table[TEXTURE_NUM_CELLS - height - cell_y];

#define VB_ALL_ADD_SPRITE_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)                                             \
  G_lighting_and_palette_buffer_sprites[8 * sprite_index + 0] = G_lighting_coord_table[PALETTE_BASE_LEVEL + lighting];  \
  G_lighting_and_palette_buffer_sprites[8 * sprite_index + 1] = G_palette_coord_table[palette];                         \
                                                                                                                        \
  G_lighting_and_palette_buffer_sprites[8 * sprite_index + 2] = G_lighting_coord_table[PALETTE_BASE_LEVEL + lighting];  \
  G_lighting_and_palette_buffer_sprites[8 * sprite_index + 3] = G_palette_coord_table[palette];                         \
                                                                                                                        \
  G_lighting_and_palette_buffer_sprites[8 * sprite_index + 4] = G_lighting_coord_table[PALETTE_BASE_LEVEL + lighting];  \
  G_lighting_and_palette_buffer_sprites[8 * sprite_index + 5] = G_palette_coord_table[palette];                         \
                                                                                                                        \
  G_lighting_and_palette_buffer_sprites[8 * sprite_index + 6] = G_lighting_coord_table[PALETTE_BASE_LEVEL + lighting];  \
  G_lighting_and_palette_buffer_sprites[8 * sprite_index + 7] = G_palette_coord_table[palette];

#define VB_ALL_ADD_SPRITE_TO_ELEMENT_BUFFER()                                  \
  G_index_buffer_sprites[6 * sprite_index + 0] = 4 * sprite_index + 0;         \
  G_index_buffer_sprites[6 * sprite_index + 1] = 4 * sprite_index + 2;         \
  G_index_buffer_sprites[6 * sprite_index + 2] = 4 * sprite_index + 1;         \
                                                                               \
  G_index_buffer_sprites[6 * sprite_index + 3] = 4 * sprite_index + 1;         \
  G_index_buffer_sprites[6 * sprite_index + 4] = 4 * sprite_index + 2;         \
  G_index_buffer_sprites[6 * sprite_index + 5] = 4 * sprite_index + 3;

#define VB_ALL_ADD_PANEL_PIECE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette)  \
  if (sprite_index < GRAPHICS_PANELS_SPRITES_END_INDEX)                                     \
  {                                                                                         \
    VB_ALL_ADD_SPRITE_TO_VERTEX_BUFFER(pos_x, pos_y, 1, 1, GRAPHICS_Z_LEVEL_PANELS)         \
    VB_ALL_ADD_SPRITE_TO_TEXTURE_COORD_BUFFER(cell_x, cell_y, 1, 1)                         \
    VB_ALL_ADD_SPRITE_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)                     \
    VB_ALL_ADD_SPRITE_TO_ELEMENT_BUFFER()                                                   \
                                                                                            \
    sprite_index += 1;                                                                      \
  }

#define VB_ALL_ADD_BUTTON_PIECE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette) \
  if (sprite_index < GRAPHICS_WIDGETS_SPRITES_END_INDEX)                                    \
  {                                                                                         \
    VB_ALL_ADD_SPRITE_TO_VERTEX_BUFFER(pos_x, pos_y, 1, 1, GRAPHICS_Z_LEVEL_WIDGETS)        \
    VB_ALL_ADD_SPRITE_TO_TEXTURE_COORD_BUFFER(cell_x, cell_y, 1, 1)                         \
    VB_ALL_ADD_SPRITE_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)                     \
    VB_ALL_ADD_SPRITE_TO_ELEMENT_BUFFER()                                                   \
                                                                                            \
    sprite_index += 1;                                                                      \
  }

#define VB_ALL_ADD_SCROLLBAR_TRACK_PIECE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette)  \
  if (sprite_index < GRAPHICS_WIDGETS_SPRITES_END_INDEX)                                              \
  {                                                                                                   \
    VB_ALL_ADD_SPRITE_TO_VERTEX_BUFFER(pos_x, pos_y, 1, 1, GRAPHICS_Z_LEVEL_WIDGETS)                  \
    VB_ALL_ADD_SPRITE_TO_TEXTURE_COORD_BUFFER(cell_x, cell_y, 1, 1)                                   \
    VB_ALL_ADD_SPRITE_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)                               \
    VB_ALL_ADD_SPRITE_TO_ELEMENT_BUFFER()                                                             \
                                                                                                      \
    sprite_index += 1;                                                                                \
  }

#define VB_ALL_ADD_SCROLLBAR_MOVING_PART_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette)  \
  if (sprite_index < GRAPHICS_TEXT_SPRITES_END_INDEX)                                                 \
  {                                                                                                   \
    VB_ALL_ADD_SPRITE_TO_VERTEX_BUFFER(pos_x, pos_y, 1, 1, GRAPHICS_Z_LEVEL_TEXT)                     \
    VB_ALL_ADD_SPRITE_TO_TEXTURE_COORD_BUFFER(cell_x, cell_y, 1, 1)                                   \
    VB_ALL_ADD_SPRITE_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)                               \
    VB_ALL_ADD_SPRITE_TO_ELEMENT_BUFFER()                                                             \
                                                                                                      \
    sprite_index += 1;                                                                                \
  }

#define VB_ALL_ADD_DIVIDER_PIECE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette)  \
  if (sprite_index < GRAPHICS_WIDGETS_SPRITES_END_INDEX)                                      \
  {                                                                                           \
    VB_ALL_ADD_SPRITE_TO_VERTEX_BUFFER(pos_x, pos_y, 1, 1, GRAPHICS_Z_LEVEL_WIDGETS)          \
    VB_ALL_ADD_SPRITE_TO_TEXTURE_COORD_BUFFER(cell_x, cell_y, 1, 1)                           \
    VB_ALL_ADD_SPRITE_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)                       \
    VB_ALL_ADD_SPRITE_TO_ELEMENT_BUFFER()                                                     \
                                                                                              \
    sprite_index += 1;                                                                        \
  }

#define VB_ALL_ADD_FONT_CHARACTER_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette) \
  if (sprite_index < GRAPHICS_TEXT_SPRITES_END_INDEX)                                         \
  {                                                                                           \
    VB_ALL_ADD_SPRITE_TO_VERTEX_BUFFER(pos_x, pos_y, 1, 1, GRAPHICS_Z_LEVEL_TEXT)             \
    VB_ALL_ADD_SPRITE_TO_TEXTURE_COORD_BUFFER(cell_x, cell_y, 1, 1)                           \
    VB_ALL_ADD_SPRITE_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)                       \
    VB_ALL_ADD_SPRITE_TO_ELEMENT_BUFFER()                                                     \
                                                                                              \
    sprite_index += 1;                                                                        \
  }

#define VB_ALL_ADD_SLIDER_TRACK_PIECE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette) \
  if (sprite_index < GRAPHICS_WIDGETS_SPRITES_END_INDEX)                                          \
  {                                                                                               \
    VB_ALL_ADD_SPRITE_TO_VERTEX_BUFFER(pos_x, pos_y, 1, 1, GRAPHICS_Z_LEVEL_WIDGETS)              \
    VB_ALL_ADD_SPRITE_TO_TEXTURE_COORD_BUFFER(cell_x, cell_y, 1, 1)                               \
    VB_ALL_ADD_SPRITE_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)                           \
    VB_ALL_ADD_SPRITE_TO_ELEMENT_BUFFER()                                                         \
                                                                                                  \
    sprite_index += 1;                                                                            \
  }

#define VB_ALL_ADD_SLIDER_MOVING_PART_TO_BUFFERS(pos_x, pos_y, lighting, palette)            \
  if (sprite_index < GRAPHICS_TEXT_SPRITES_END_INDEX)                                        \
  {                                                                                          \
    VB_ALL_ADD_SPRITE_TO_VERTEX_BUFFER(pos_x, pos_y, 1, 1, GRAPHICS_Z_LEVEL_TEXT)            \
    VB_ALL_ADD_SPRITE_TO_TEXTURE_COORD_BUFFER(0, 7, 1, 1)                                    \
    VB_ALL_ADD_SPRITE_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)                      \
    VB_ALL_ADD_SPRITE_TO_ELEMENT_BUFFER()                                                    \
                                                                                             \
    sprite_index += 1;                                                                       \
  }

#define VB_ALL_ADD_NAMED_SPRITE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, width, height, lighting, palette)  \
  if (sprite_index < GRAPHICS_TEXT_SPRITES_END_INDEX)                                                       \
  {                                                                                                         \
    VB_ALL_ADD_SPRITE_TO_VERTEX_BUFFER(pos_x, pos_y, width, height, GRAPHICS_Z_LEVEL_TEXT)                  \
    VB_ALL_ADD_SPRITE_TO_TEXTURE_COORD_BUFFER(cell_x, cell_y, width, height)                                \
    VB_ALL_ADD_SPRITE_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)                                     \
    VB_ALL_ADD_SPRITE_TO_ELEMENT_BUFFER()                                                                   \
                                                                                                            \
    sprite_index += 1;                                                                                      \
  }

#define VB_ALL_UPDATE_PANELS_SPRITES_IN_VBOS()                                                      \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_sprites);                                        \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                  \
                  GRAPHICS_PANELS_SPRITES_START_INDEX * 12 * sizeof(GLfloat),                       \
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_PANELS] * 12 * sizeof(GLfloat),       \
                  &G_vertex_buffer_sprites[GRAPHICS_PANELS_SPRITES_START_INDEX * 12]);              \
                                                                                                    \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_sprites);                                 \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                  \
                  GRAPHICS_PANELS_SPRITES_START_INDEX * 8 * sizeof(GLfloat),                        \
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_PANELS] * 8 * sizeof(GLfloat),        \
                  &G_texture_coord_buffer_sprites[GRAPHICS_PANELS_SPRITES_START_INDEX * 8]);        \
                                                                                                    \
  glBindBuffer(GL_ARRAY_BUFFER, G_lighting_and_palette_buffer_id_sprites);                          \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                  \
                  GRAPHICS_PANELS_SPRITES_START_INDEX * 8 * sizeof(GLfloat),                        \
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_PANELS] * 8 * sizeof(GLfloat),        \
                  &G_lighting_and_palette_buffer_sprites[GRAPHICS_PANELS_SPRITES_START_INDEX * 8]); \
                                                                                                    \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_sprites);                                 \
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,                                                          \
                  GRAPHICS_PANELS_SPRITES_START_INDEX * 6 * sizeof(unsigned short),                 \
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_PANELS] * 6 * sizeof(unsigned short), \
                  &G_index_buffer_sprites[GRAPHICS_PANELS_SPRITES_START_INDEX * 6]);

#define VB_ALL_UPDATE_WIDGETS_SPRITES_IN_VBOS()                                                       \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_sprites);                                          \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                    \
                  GRAPHICS_WIDGETS_SPRITES_START_INDEX * 12 * sizeof(GLfloat),                        \
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_WIDGETS] * 12 * sizeof(GLfloat),        \
                  &G_vertex_buffer_sprites[GRAPHICS_WIDGETS_SPRITES_START_INDEX * 12]);               \
                                                                                                      \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_sprites);                                   \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                    \
                  GRAPHICS_WIDGETS_SPRITES_START_INDEX * 8 * sizeof(GLfloat),                         \
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_WIDGETS] * 8 * sizeof(GLfloat),         \
                  &G_texture_coord_buffer_sprites[GRAPHICS_WIDGETS_SPRITES_START_INDEX * 8]);         \
                                                                                                      \
  glBindBuffer(GL_ARRAY_BUFFER, G_lighting_and_palette_buffer_id_sprites);                            \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                    \
                  GRAPHICS_WIDGETS_SPRITES_START_INDEX * 8 * sizeof(GLfloat),                         \
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_WIDGETS] * 8 * sizeof(GLfloat),         \
                  &G_lighting_and_palette_buffer_sprites[GRAPHICS_WIDGETS_SPRITES_START_INDEX * 8]);  \
                                                                                                      \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_sprites);                                   \
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,                                                            \
                  GRAPHICS_WIDGETS_SPRITES_START_INDEX * 6 * sizeof(unsigned short),                  \
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_WIDGETS] * 6 * sizeof(unsigned short),  \
                  &G_index_buffer_sprites[GRAPHICS_WIDGETS_SPRITES_START_INDEX * 6]);

#define VB_ALL_UPDATE_TEXT_SPRITES_IN_VBOS()                                                          \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_sprites);                                          \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                    \
                  GRAPHICS_TEXT_SPRITES_START_INDEX * 12 * sizeof(GLfloat),                           \
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_TEXT] * 12 * sizeof(GLfloat),           \
                  &G_vertex_buffer_sprites[GRAPHICS_TEXT_SPRITES_START_INDEX * 12]);                  \
                                                                                                      \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_sprites);                                   \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                    \
                  GRAPHICS_TEXT_SPRITES_START_INDEX * 8 * sizeof(GLfloat),                            \
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_TEXT] * 8 * sizeof(GLfloat),            \
                  &G_texture_coord_buffer_sprites[GRAPHICS_TEXT_SPRITES_START_INDEX * 8]);            \
                                                                                                      \
  glBindBuffer(GL_ARRAY_BUFFER, G_lighting_and_palette_buffer_id_sprites);                            \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                    \
                  GRAPHICS_TEXT_SPRITES_START_INDEX * 8 * sizeof(GLfloat),                            \
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_TEXT] * 8 * sizeof(GLfloat),            \
                  &G_lighting_and_palette_buffer_sprites[GRAPHICS_TEXT_SPRITES_START_INDEX * 8]);     \
                                                                                                      \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_sprites);                                   \
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,                                                            \
                  GRAPHICS_TEXT_SPRITES_START_INDEX * 6 * sizeof(unsigned short),                     \
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_TEXT] * 6 * sizeof(unsigned short),     \
                  &G_index_buffer_sprites[GRAPHICS_TEXT_SPRITES_START_INDEX * 6]);

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
static char S_top_panel_button_labels[LAYOUT_TOP_PANEL_BUTTON_NUM_LABELS][VB_ALL_BUTTON_MAX_TEXT_SIZE] = 
  { "Cart", "Instruments", "Song", "Mixer", "Sound FX", "DPCM" };

static char S_top_panel_header_labels[LAYOUT_TOP_PANEL_HEADER_NUM_LABELS][VB_ALL_HEADER_MAX_TEXT_SIZE] = 
  { "Gersemi", "v0.9" };

/* cart edit tables */
static char S_cart_button_labels[LAYOUT_CART_BUTTON_NUM_LABELS][VB_ALL_BUTTON_MAX_TEXT_SIZE] = 
  { "Load", "Save", "Copy", "Zap", 
    "Load", "Save", "Copy", "Zap" 
  };

static char S_cart_header_labels[LAYOUT_CART_HEADER_NUM_LABELS][VB_ALL_HEADER_MAX_TEXT_SIZE] = 
  { "Osc 1", "Osc 2", "Osc 3", 
    "Filters", 
    "Env 1", "Env 2", "Env 3", 
    "LFO Vib", "LFO Trem", "Chorus", 
    "Sync", "Sensitivity", "Pitch Env", 
    "Arpeggio", "Portamento", "Pitch Wheel", 
    "Trem Routing", "Boost Routing", "Vel Routing", 
    "Mod Wheel", "Aftertouch", "Exp Pedal" 
  };

static char S_cart_param_labels[LAYOUT_CART_PARAM_NUM_LABELS][VB_ALL_PARAM_NAME_MAX_TEXT_SIZE] = 
  { "Car", "Pat", 
    "Alg",                                      /* algorithm */
    "Wav", "Phi", "Frq", "Mul", "Div", "Det",   /* oscillator 1 */
    "Wav", "Phi", "Frq", "Mul", "Div", "Det",   /* oscillator 2 */
    "Wav", "Phi", "Det",                        /* oscillator 3 */
     "LP", "KTr",  "HP",                        /* filters */
    "Att", "Dec", "Rel", "Lev", "Sus",          /* envelope 1 */
    "Hld", "Pdl", "RtS", "LvS", 
    "Att", "Dec", "Rel", "Lev", "Sus",          /* envelope 2 */
    "Hld", "Pdl", "RtS", "LvS", 
    "Att", "Dec", "Rel", "Lev", "Sus",          /* envelope 3 */
    "Hld", "Pdl", "RtS", "LvS", 
    "Wav", "Dly", "Spd", "Dep",                 /* vibrato lfo */
    "Wav", "Dly", "Spd", "Dep",                 /* tremolo lfo */
    "Mde", "Dly", "Spd", "Dep",                 /* chorus */
    "Vib", "Trm", "Chr", "Osc",                 /* sync */
    "Vib", "Trm", "Chr", "Bst", "Vel",          /* sensitivity */
    "Att", "Dec", "Rel", "Max", "Fin",          /* pitch env */
    "Mde", "Pat", "Oct", "Spd",                 /* arpeggio */
    "Mde", "Leg", "Spd",                        /* portamento */
    "Mde", "Rng",                               /* pitch wheel */
    "En1", "En2", "En3",                        /* tremolo routing */
    "En1", "En2", "En3",                        /* boost routing */
    "En1", "En2", "En3",                        /* velocity routing */
    "Vib", "Trm", "Bst", "Chr",                 /* mod wheel routing */
    "Vib", "Trm", "Bst", "Chr",                 /* aftertouch routing */
    "Vib", "Trm", "Bst", "Chr",                 /* exp pedal routing */
    "Oct", "Vel", "PW", "MW", "AT", "EP" 
  };

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

static char S_patch_edit_osc_detune_values[PATCH_OSC_DETUNE_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "-3", "-2", "-1", "0", "1", "2", "3" };

static char S_patch_edit_osc_freq_mode_values[PATCH_OSC_FREQ_MODE_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "Ratio", "Fixed" };

static char S_patch_edit_highpass_cutoff_values[PATCH_HIGHPASS_CUTOFF_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "A0", "A1", "A2", "A3" };

static char S_patch_edit_env_hold_values[PATCH_ENV_HOLD_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "1", "2", "3", "4", "5", "6", "7", "8", "Inf" };

static char S_patch_edit_vibrato_waveform_values[PATCH_VIBRATO_WAVEFORM_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "Triangle", "Square", "SawUp", "SawDown", "UniTri", "UniSqua", "UniSawU", "UniSawD" };

static char S_patch_edit_tremolo_waveform_values[PATCH_TREMOLO_WAVEFORM_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "Triangle", "Square", "SawUp", "SawDown", "NoiseSqu", "NoiseSaw" };

static char S_patch_edit_chorus_mode_values[PATCH_CHORUS_MODE_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "Chorus", "Detune" };

static char S_patch_edit_arpeggio_mode_values[PATCH_ARPEGGIO_MODE_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "Harp", "Rolled" };

static char S_patch_edit_arpeggio_pattern_values[PATCH_ARPEGGIO_PATTERN_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "Up", "Down", "Up/Down", "U/D Alt" };

static char S_patch_edit_portamento_mode_values[PATCH_PORTAMENTO_MODE_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "Bend", "Half" };

static char S_patch_edit_portamento_legato_values[PATCH_PORTAMENTO_LEGATO_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "Off", "Follow", "Hammer" };

static char S_patch_edit_pitch_wheel_mode_values[PATCH_PITCH_WHEEL_MODE_NUM_VALUES][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "Bend", "Half" };

static char S_patch_edit_routing_values[2][VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE] = 
  { "Off", "On" };

static char S_patch_edit_audition_bi_wheel_values[MIDI_CONT_BI_WHEEL_NUM_VALUES][VB_ALL_PARAM_NUMBER_MAX_TEXT_SIZE] = 
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

static char S_patch_edit_audition_uni_wheel_values[MIDI_CONT_UNI_WHEEL_NUM_VALUES][VB_ALL_PARAM_NUMBER_MAX_TEXT_SIZE] = 
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

/* load tables */
static char S_load_button_labels[LAYOUT_LOAD_BUTTON_NUM_LABELS][VB_ALL_BUTTON_MAX_TEXT_SIZE] = 
  { "Load" };

static char S_load_header_labels[LAYOUT_LOAD_HEADER_NUM_LABELS][VB_ALL_HEADER_MAX_TEXT_SIZE] = 
  { "Load Cart" };

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
    G_tile_layer_counts[k] = 0;

  /* draw the background */
  tile_index = GRAPHICS_BACKGROUND_TILES_START_INDEX;

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
      VB_ALL_ADD_BACKGROUND_TILE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette)
    }
  }

  /* update sky tile layer count */
  G_tile_layer_counts[GRAPHICS_TILE_LAYER_BACKGROUND] = 
    tile_index - GRAPHICS_BACKGROUND_TILES_START_INDEX;

  /* update vbos */
  VB_ALL_UPDATE_BACKGROUND_TILES_IN_VBOS()

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
  sprite_index =  GRAPHICS_PANELS_SPRITES_START_INDEX + 
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_PANELS];

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
        cell_x = 3;
      else
        cell_x = 1;

      /* top / bottom / middle piece */
      if (n == 0)
        cell_y = 12;
      else if (n == height - 1)
        cell_y = 15;
      else
        cell_y = 13;

      /* adjust cells based on type */
      if (type == LAYOUT_PANEL_TYPE_1)
        cell_x += 0;
      else if (type == LAYOUT_PANEL_TYPE_2)
        cell_x += 4;
      else
        cell_x += 0;

      VB_ALL_ADD_PANEL_PIECE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette)
    }
  }

  /* update panels sprite layer count */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_PANELS] = 
    sprite_index - GRAPHICS_PANELS_SPRITES_START_INDEX;

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
  sprite_index =  GRAPHICS_WIDGETS_SPRITES_START_INDEX + 
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_WIDGETS];

  for (n = 0; n < 2; n++)
  {
    for (m = 0; m < width; m++)
    {
      /* determine center of this piece */
      pos_x = corner_x + (8 * m);
      pos_y = corner_y + (8 * n);

      /* left / right / middle piece */
      if (m == 0)
        cell_x = 10;
      else if (m == width - 1)
        cell_x = 13;
      else
        cell_x = 11;

      /* top / bottom piece */
      if (n == 0)
        cell_y = 6;
      else
        cell_y = 7;

      VB_ALL_ADD_BUTTON_PIECE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette)
    }
  }

  /* update buttons sprite layer count */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_WIDGETS] = 
    sprite_index - GRAPHICS_WIDGETS_SPRITES_START_INDEX;

  return 0;
}

/*******************************************************************************
** vb_all_load_vertical_scrollbar_area()
*******************************************************************************/
short int vb_all_load_vertical_scrollbar_area(int offset_x, int offset_y, 
                                              int height)
{
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
  if ((height < 4) || (height > LAYOUT_OVERSCAN_HEIGHT / 8))
    return 1;

  /* determine coordinates of the center of the top left corner   */
  /* 8x8 cell. the offsets from the screen center are in pixels.  */
  corner_x = LAYOUT_OVERSCAN_CENTER_X + offset_x - 4 * (1 - 1);
  corner_y = LAYOUT_OVERSCAN_CENTER_Y + offset_y - 4 * (height - 1);

  /* set lighting and palette */
  lighting = 0;
  palette = VB_ALL_PALETTE_1;

  /* draw the scrollbar track */
  sprite_index =  GRAPHICS_WIDGETS_SPRITES_START_INDEX + 
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_WIDGETS];

  for (n = 0; n < height; n++)
  {
    /* determine center of this piece */
    pos_x = corner_x + (8 * 0);
    pos_y = corner_y + (8 * n);

    /* up arrow piece */
    if (n == 0)
    {
      cell_x = 14;
      cell_y = 6;
    }
    /* bar top piece */
    else if (n == 1)
    {
      cell_x = 15;
      cell_y = 6;
    }
    /* bar bottom piece */
    else if (n == height - 2)
    {
      cell_x = 15;
      cell_y = 9;
    }
    /* bottom arrow piece */
    else if (n == height - 1)
    {
      cell_x = 14;
      cell_y = 7;
    }
    /* bar middle piece */
    else
    {
      cell_x = 15;
      cell_y = 7;
    }

    VB_ALL_ADD_SCROLLBAR_TRACK_PIECE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette)
  }

  /* update panels tile layer count */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_WIDGETS] = 
    sprite_index - GRAPHICS_WIDGETS_SPRITES_START_INDEX;

  return 0;
}

/*******************************************************************************
** vb_all_load_vertical_scrollbar_slider()
*******************************************************************************/
short int vb_all_load_vertical_scrollbar_slider(int offset_x, 
                                                int offset_y, 
                                                int height, 
                                                int current_scroll_amount, 
                                                int max_scroll_amount)
{
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

  /* make sure the height is valid        */
  /* the height is in terms of 8x8 cells  */
  if ((height < 4) || (height > LAYOUT_OVERSCAN_HEIGHT / 8))
    return 1;

  /* make sure the scroll amount is valid */
  if ((current_scroll_amount < 0) || (current_scroll_amount > max_scroll_amount))
    return 1;

  /* determine coordinates of the center of the top left corner   */
  /* 8x8 cell. the offsets from the screen center are in pixels.  */
  corner_x = LAYOUT_OVERSCAN_CENTER_X + offset_x - 4 * (1 - 1);
  corner_y = LAYOUT_OVERSCAN_CENTER_Y + offset_y - 4 * (height - 1);

  if (max_scroll_amount > 0)
    corner_y += (current_scroll_amount * 8 * (height - 2)) / max_scroll_amount;

  /* set lighting and palette */
  lighting = 0;
  palette = VB_ALL_PALETTE_1;

  /* draw the scrollbar's slider */
  sprite_index =  GRAPHICS_TEXT_SPRITES_START_INDEX + 
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_TEXT];

  for (n = 0; n < 2; n++)
  {
    /* determine vertical position */
    pos_x = corner_x + (8 * 0);
    pos_y = corner_y + (8 * n);

    /* top / bottom half */
    cell_x = 14;

    if (n == 0)
      cell_y = 8;
    else
      cell_y = 9;

    VB_ALL_ADD_SCROLLBAR_MOVING_PART_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette)
  }

  /* update overlay sprite layer count */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_TEXT] = 
    sprite_index - GRAPHICS_TEXT_SPRITES_START_INDEX;

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
  sprite_index =  GRAPHICS_WIDGETS_SPRITES_START_INDEX + 
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_WIDGETS];

  for (m = 0; m < width; m++)
  {
    /* determine center of this piece */
    pos_x = corner_x + (8 * m);
    pos_y = corner_y + (8 * 0);

    /* 1st / 2nd / 3rd piece */
    if (m % 3 == 0)
      cell_x = 7;
    else if (m % 3 == 1)
      cell_x = 8;
    else
      cell_x = 9;

    cell_y = 7;

    VB_ALL_ADD_DIVIDER_PIECE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette)
  }

  /* update panels tile layer count */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_WIDGETS] = 
    sprite_index - GRAPHICS_WIDGETS_SPRITES_START_INDEX;

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
  sprite_index =  GRAPHICS_TEXT_SPRITES_START_INDEX + 
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_TEXT];

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

    VB_ALL_ADD_FONT_CHARACTER_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette)
  }

  /* update overlay sprite layer count */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_TEXT] = 
    sprite_index - GRAPHICS_TEXT_SPRITES_START_INDEX;

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
    cell_x = 4;
    cell_y = 6;
    width = 1;
    height = 1;
  }
  else if (name == VB_ALL_SPRITE_NAME_PARAM_ARROWS_RIGHT)
  {
    cell_x = 5;
    cell_y = 6;
    width = 1;
    height = 1;
  }
  else if (name == VB_ALL_SPRITE_NAME_PARAM_RADIO_BUTTON_OFF)
  {
    cell_x = 8;
    cell_y = 6;
    width = 1;
    height = 1;
  }
  else if (name == VB_ALL_SPRITE_NAME_PARAM_RADIO_BUTTON_ON)
  {
    cell_x = 9;
    cell_y = 6;
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
  sprite_index =  GRAPHICS_TEXT_SPRITES_START_INDEX + 
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_TEXT];

  VB_ALL_ADD_NAMED_SPRITE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, width, height, lighting, palette)

  /* update overlay sprite layer count */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_TEXT] = 
    sprite_index - GRAPHICS_TEXT_SPRITES_START_INDEX;

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
  sprite_index =  GRAPHICS_WIDGETS_SPRITES_START_INDEX + 
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_WIDGETS];

  for (m = 0; m < width; m++)
  {
    /* determine center of this piece */
    pos_x = corner_x + (8 * m);
    pos_y = corner_y + (8 * 0);

    /* left / right / middle piece */
    if (m == 0)
      cell_x = 1;
    else if (m == width - 1)
      cell_x = 3;
    else
      cell_x = 2;

    cell_y = 7;

    VB_ALL_ADD_SLIDER_TRACK_PIECE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette)
  }

  /* update underlay sprite layer count */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_WIDGETS] = 
    sprite_index - GRAPHICS_WIDGETS_SPRITES_START_INDEX;

  /* draw the slider itself */
  sprite_index =  GRAPHICS_TEXT_SPRITES_START_INDEX + 
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_TEXT];

  pos_x = corner_x;
  pos_y = corner_y;

  pos_x += ((value - lower_bound) * 8 * (width - 1)) / (upper_bound - lower_bound);

  lighting = 0;
  palette = VB_ALL_PALETTE_1;

  VB_ALL_ADD_SLIDER_MOVING_PART_TO_BUFFERS(pos_x, pos_y, lighting, palette)

  /* update text sprite layer count */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_TEXT] = 
    sprite_index - GRAPHICS_TEXT_SPRITES_START_INDEX;

  return 0;
}

/*******************************************************************************
** vb_all_load_top_panel()
*******************************************************************************/
short int vb_all_load_top_panel()
{
  int k;

  button* b;
  header* hd;

  /* reset sprite vbo counts */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_PANELS] = 0;
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_WIDGETS] = 0;
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_TEXT] = 0;

  /* top panel */
  vb_all_load_panel(LAYOUT_TOP_PANEL_AREA_X, LAYOUT_TOP_PANEL_AREA_Y, 
                    LAYOUT_TOP_PANEL_AREA_WIDTH, LAYOUT_TOP_PANEL_AREA_HEIGHT, 
                    LAYOUT_PANEL_TYPE_1);

  /* vertical scrollbar track & slider */
  vb_all_load_vertical_scrollbar_area(LAYOUT_VERT_SCROLLBAR_AREA_X, 
                                      LAYOUT_VERT_SCROLLBAR_AREA_Y, 
                                      LAYOUT_VERT_SCROLLBAR_AREA_HEIGHT);

  vb_all_load_vertical_scrollbar_slider(LAYOUT_VERT_SCROLLBAR_TRACK_X, 
                                        LAYOUT_VERT_SCROLLBAR_TRACK_Y, 
                                        LAYOUT_VERT_SCROLLBAR_TRACK_HEIGHT, 
                                        G_current_scroll_amount, 
                                        G_max_scroll_amount);

  /* headers */
  for ( k = LAYOUT_TOP_PANEL_HEADERS_START_INDEX; 
        k < LAYOUT_TOP_PANEL_HEADERS_END_INDEX; 
        k++)
  {
    hd = &G_layout_headers[k];

    /* make sure the label is valid */
    if ((hd->label < 0) || (hd->label >= LAYOUT_TOP_PANEL_HEADER_NUM_LABELS))
      continue;

    /* load the header */
    vb_all_load_text( hd->center_x, hd->center_y, 
                      VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_GRAY, VB_ALL_HEADER_MAX_TEXT_SIZE, 
                      S_top_panel_header_labels[hd->label]);
  }

  /* buttons */
  for ( k = LAYOUT_TOP_PANEL_BUTTONS_START_INDEX; 
        k < LAYOUT_TOP_PANEL_BUTTONS_END_INDEX; 
        k++)
  {
    b = &G_layout_buttons[k];

    /* make sure the label is valid */
    if ((b->label < 0) || (b->label >= LAYOUT_TOP_PANEL_BUTTON_NUM_LABELS))
      continue;

    /* load button & text */
    vb_all_load_button( b->center_x, b->center_y, 
                        b->width, b->state);

    vb_all_load_text( b->center_x, b->center_y, 
                      VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_GRAY, VB_ALL_BUTTON_MAX_TEXT_SIZE, 
                      S_top_panel_button_labels[b->label]);
  }

  /* update vbos */
  VB_ALL_UPDATE_PANELS_SPRITES_IN_VBOS()
  VB_ALL_UPDATE_WIDGETS_SPRITES_IN_VBOS()
  VB_ALL_UPDATE_TEXT_SPRITES_IN_VBOS()

  return 0;
}

/*******************************************************************************
** vb_all_load_cart_screen()
*******************************************************************************/
short int vb_all_load_cart_screen()
{
  int k;

  patch* p;

  button* b;
  header* hd;
  param*  pr;

  short int value;
  char*     value_string;

  int pos_y;

  int cart_index;
  int patch_index;

  /* determine cart & patch indices */
  PATCH_COMPUTE_CART_INDEX(G_patch_edit_cart_number)
  PATCH_COMPUTE_PATCH_INDEX(G_patch_edit_cart_number, G_patch_edit_patch_number)

  /* make sure that the cart & patch indices are valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(cart_index))
    return 1;

  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain patch pointer */
  p = &G_patch_bank[patch_index];

  /* horizontal divider */
  pos_y = LAYOUT_CART_MAIN_DIVIDER_Y - G_current_scroll_amount;

  if (LAYOUT_CART_HEADER_OR_PARAM_IS_IN_MAIN_AREA(pos_y))
  {
    vb_all_load_horizontal_divider( LAYOUT_CART_MAIN_DIVIDER_X, 
                                    pos_y, 
                                    LAYOUT_CART_MAIN_DIVIDER_WIDTH);
  }

  /* audition panel */
  vb_all_load_panel(LAYOUT_CART_AUDITION_PANEL_X, LAYOUT_CART_AUDITION_PANEL_Y, 
                    LAYOUT_CART_AUDITION_PANEL_WIDTH, LAYOUT_CART_AUDITION_PANEL_HEIGHT, 
                    LAYOUT_PANEL_TYPE_2);

  /* buttons */
  for ( k = LAYOUT_CART_BUTTONS_START_INDEX; 
        k < LAYOUT_CART_BUTTONS_END_INDEX; 
        k++)
  {
    b = &G_layout_buttons[k];

    /* make sure the label is valid */
    if ((b->label < 0) || (b->label >= LAYOUT_CART_BUTTON_NUM_LABELS))
      continue;

    /* determine vertical position for this button */
    pos_y = b->center_y - G_current_scroll_amount;

    if (LAYOUT_CART_BUTTON_IS_NOT_IN_MAIN_AREA(pos_y))
      continue;

    /* load button & text */
    vb_all_load_button( b->center_x, pos_y, 
                        b->width, b->state);

    vb_all_load_text( b->center_x, pos_y, 
                      VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_GRAY, VB_ALL_BUTTON_MAX_TEXT_SIZE, 
                      S_cart_button_labels[b->label]);
  }

  /* headers */
  for ( k = LAYOUT_CART_HEADERS_START_INDEX; 
        k < LAYOUT_CART_HEADERS_END_INDEX; 
        k++)
  {
    hd = &G_layout_headers[k];

    /* make sure the label is valid */
    if ((hd->label < 0) || (hd->label >= LAYOUT_CART_HEADER_NUM_LABELS))
      continue;

    /* determine vertical position for this header */
    pos_y = hd->center_y - G_current_scroll_amount;

    if (LAYOUT_CART_HEADER_OR_PARAM_IS_NOT_IN_MAIN_AREA(pos_y))
      continue;

    /* load the header! */
    vb_all_load_text( hd->center_x, pos_y, 
                      VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_3, VB_ALL_HEADER_MAX_TEXT_SIZE, 
                      S_cart_header_labels[hd->label]);
  }

  /* parameters */
  for ( k = LAYOUT_CART_PARAMS_START_INDEX; 
        k < LAYOUT_CART_PARAMS_END_INDEX; 
        k++)
  {
    pr = &G_layout_params[k];

    /* make sure the label is valid */
    if ((pr->label < 0) || (pr->label >= LAYOUT_CART_PARAM_NUM_LABELS))
      continue;

    /* determine vertical position for audition bar params (remain stationary) */
    if ((pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_OCTAVE)      || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_VELOCITY)    || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_PITCH_WHEEL) || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_MOD_WHEEL)   || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_AFTERTOUCH)  || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_EXP_PEDAL))
    {
      pos_y = pr->center_y;
    }
    /* determine vertical position for other params (can be scrolled up/down) */
    else
    {
      pos_y = pr->center_y - G_current_scroll_amount;

      if (LAYOUT_CART_HEADER_OR_PARAM_IS_NOT_IN_MAIN_AREA(pos_y))
        continue;
    }

    /* determine parameter value & string */
    if (pr->label == LAYOUT_CART_PARAM_LABEL_CART_NUMBER)
    {
      value = G_patch_edit_cart_number;
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PATCH_NUMBER)
    {
      value = G_patch_edit_patch_number;
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    /* algorithm */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ALGORITHM)
    {
      value = p->algorithm;
      value_string = S_patch_edit_algorithm_values[value - pr->lower_bound];
    }
    /* oscillator 1 */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_1_WAVEFORM)
    {
      value = p->osc_1_waveform;
      value_string = S_patch_edit_osc_waveform_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_1_PHI)
    {
      value = p->osc_1_phi;
      value_string = S_patch_edit_osc_phi_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_1_FREQ_MODE)
    {
      value = p->osc_1_freq_mode;
      value_string = S_patch_edit_osc_freq_mode_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_1_MULTIPLE)
    {
      value = p->osc_1_multiple;
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_1_DIVISOR)
    {
      value = p->osc_1_divisor;
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_1_DETUNE)
    {
      value = p->osc_1_detune;
      value_string = S_patch_edit_osc_detune_values[value - pr->lower_bound];
    }
    /* oscillator 2 */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_2_WAVEFORM)
    {
      value = p->osc_2_waveform;
      value_string = S_patch_edit_osc_waveform_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_2_PHI)
    {
      value = p->osc_2_phi;
      value_string = S_patch_edit_osc_phi_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_2_FREQ_MODE)
    {
      value = p->osc_2_freq_mode;
      value_string = S_patch_edit_osc_freq_mode_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_2_MULTIPLE)
    {
      value = p->osc_2_multiple;
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_2_DIVISOR)
    {
      value = p->osc_2_divisor;
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_2_DETUNE)
    {
      value = p->osc_2_detune;
      value_string = S_patch_edit_osc_detune_values[value - pr->lower_bound];
    }
    /* oscillator 3 */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_3_WAVEFORM)
    {
      value = p->osc_3_waveform;
      value_string = S_patch_edit_osc_waveform_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_3_PHI)
    {
      value = p->osc_3_phi;
      value_string = S_patch_edit_osc_phi_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_3_DETUNE)
    {
      value = p->osc_3_detune;
      value_string = S_patch_edit_osc_detune_values[value - pr->lower_bound];
    }
    /* filters */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_LOWPASS_MULTIPLE)
    {
      value = p->lowpass_multiple;
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_LOWPASS_KEYTRACKING)
    {
      value = p->lowpass_keytracking;
      value_string = S_common_edit_0_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_HIGHPASS_CUTOFF)
    {
      value = p->highpass_cutoff;
      value_string = S_patch_edit_highpass_cutoff_values[value - pr->lower_bound];
    }
    /* amplitude envelope 1 */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_1_ATTACK)
    {
      value = p->env_attack[0];
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_1_DECAY)
    {
      value = p->env_decay[0];
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_1_RELEASE)
    {
      value = p->env_release[0];
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_1_AMPLITUDE)
    {
      value = p->env_amplitude[0];
      value_string = S_common_edit_0_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_1_SUSTAIN)
    {
      value = p->env_sustain[0];
      value_string = S_common_edit_0_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_1_HOLD)
    {
      value = p->env_hold[0];
      value_string = S_patch_edit_env_hold_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_1_PEDAL)
    {
      value = p->env_pedal[0];
      value_string = S_patch_edit_env_hold_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_1_RATE_KS)
    {
      value = p->env_rate_ks[0];
      value_string = S_common_edit_0_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_1_LEVEL_KS)
    {
      value = p->env_level_ks[0];
      value_string = S_common_edit_0_to_100_values[value - pr->lower_bound];
    }
    /* amplitude envelope 2 */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_2_ATTACK)
    {
      value = p->env_attack[1];
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_2_DECAY)
    {
      value = p->env_decay[1];
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_2_RELEASE)
    {
      value = p->env_release[1];
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_2_AMPLITUDE)
    {
      value = p->env_amplitude[1];
      value_string = S_common_edit_0_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_2_SUSTAIN)
    {
      value = p->env_sustain[1];
      value_string = S_common_edit_0_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_2_HOLD)
    {
      value = p->env_hold[1];
      value_string = S_patch_edit_env_hold_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_2_PEDAL)
    {
      value = p->env_pedal[1];
      value_string = S_patch_edit_env_hold_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_2_RATE_KS)
    {
      value = p->env_rate_ks[1];
      value_string = S_common_edit_0_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_2_LEVEL_KS)
    {
      value = p->env_level_ks[1];
      value_string = S_common_edit_0_to_100_values[value - pr->lower_bound];
    }
    /* amplitude envelope 3 */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_3_ATTACK)
    {
      value = p->env_attack[2];
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_3_DECAY)
    {
      value = p->env_decay[2];
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_3_RELEASE)
    {
      value = p->env_release[2];
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_3_AMPLITUDE)
    {
      value = p->env_amplitude[2];
      value_string = S_common_edit_0_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_3_SUSTAIN)
    {
      value = p->env_sustain[2];
      value_string = S_common_edit_0_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_3_HOLD)
    {
      value = p->env_hold[2];
      value_string = S_patch_edit_env_hold_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_3_PEDAL)
    {
      value = p->env_pedal[2];
      value_string = S_patch_edit_env_hold_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_3_RATE_KS)
    {
      value = p->env_rate_ks[2];
      value_string = S_common_edit_0_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_3_LEVEL_KS)
    {
      value = p->env_level_ks[2];
      value_string = S_common_edit_0_to_100_values[value - pr->lower_bound];
    }
    /* vibrato lfo */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_VIBRATO_WAVEFORM)
    {
      value = p->vibrato_waveform;
      value_string = S_patch_edit_vibrato_waveform_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_VIBRATO_DELAY)
    {
      value = p->vibrato_delay;
      value_string = S_common_edit_0_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_VIBRATO_SPEED)
    {
      value = p->vibrato_speed;
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_VIBRATO_DEPTH)
    {
      value = p->vibrato_depth;
      value_string = S_common_edit_0_to_100_values[value - pr->lower_bound];
    }
    /* tremolo lfo */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_WAVEFORM)
    {
      value = p->tremolo_waveform;
      value_string = S_patch_edit_tremolo_waveform_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_DELAY)
    {
      value = p->tremolo_delay;
      value_string = S_common_edit_0_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_SPEED)
    {
      value = p->tremolo_speed;
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_DEPTH)
    {
      value = p->tremolo_depth;
      value_string = S_common_edit_0_to_100_values[value - pr->lower_bound];
    }
    /* chorus */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_CHORUS_MODE)
    {
      value = p->chorus_mode;
      value_string = S_patch_edit_chorus_mode_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_CHORUS_DELAY)
    {
      value = p->chorus_delay;
      value_string = S_common_edit_0_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_CHORUS_SPEED)
    {
      value = p->chorus_speed;
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_CHORUS_DEPTH)
    {
      value = p->chorus_depth;
      value_string = S_common_edit_0_to_100_values[value - pr->lower_bound];
    }
    /* sync */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_SYNC_VIBRATO)
    {
      value = p->sync_vibrato;
      value_string = S_patch_edit_sync_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_SYNC_TREMOLO)
    {
      value = p->sync_tremolo;
      value_string = S_patch_edit_sync_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_SYNC_CHORUS)
    {
      value = p->sync_chorus;
      value_string = S_patch_edit_sync_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_SYNC_OSC)
    {
      value = p->sync_osc;
      value_string = S_patch_edit_sync_values[value - pr->lower_bound];
    }
    /* sensitivity */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_SENSITIVITY_VIBRATO)
    {
      value = p->sensitivity_vibrato;
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_SENSITIVITY_TREMOLO)
    {
      value = p->sensitivity_tremolo;
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_SENSITIVITY_CHORUS)
    {
      value = p->sensitivity_chorus;
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_SENSITIVITY_BOOST)
    {
      value = p->sensitivity_boost;
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_SENSITIVITY_VELOCITY)
    {
      value = p->sensitivity_velocity;
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    /* pitch envelope */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_ENV_ATTACK)
    {
      value = p->peg_attack;
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_ENV_DECAY)
    {
      value = p->peg_decay;
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_ENV_RELEASE)
    {
      value = p->peg_release;
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_ENV_MAXIMUM)
    {
      value = p->peg_maximum;
      value_string = S_common_edit_0_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_ENV_FINALE)
    {
      value = p->peg_finale;
      value_string = S_common_edit_0_to_100_values[value - pr->lower_bound];
    }
    /* arpeggio */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ARPEGGIO_MODE)
    {
      value = p->arpeggio_mode;
      value_string = S_patch_edit_arpeggio_mode_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ARPEGGIO_PATTERN)
    {
      value = p->arpeggio_pattern;
      value_string = S_patch_edit_arpeggio_pattern_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ARPEGGIO_OCTAVES)
    {
      value = p->arpeggio_octaves;
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ARPEGGIO_SPEED)
    {
      value = p->arpeggio_speed;
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    /* portamento */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PORTAMENTO_MODE)
    {
      value = p->portamento_mode;
      value_string = S_patch_edit_portamento_mode_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PORTAMENTO_LEGATO)
    {
      value = p->portamento_legato;
      value_string = S_patch_edit_portamento_legato_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PORTAMENTO_SPEED)
    {
      value = p->portamento_speed;
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    /* pitch wheel */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_WHEEL_MODE)
    {
      value = p->pitch_wheel_mode;
      value_string = S_patch_edit_pitch_wheel_mode_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_WHEEL_RANGE)
    {
      value = p->pitch_wheel_range;
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    /* envelope adjustment routing */
    else if ( (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_ROUTING_ENV_1) || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_ROUTING_ENV_2) || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_ROUTING_ENV_3))
    {
      value = (p->tremolo_routing & pr->upper_bound);
      value_string = S_patch_edit_routing_values[value / pr->upper_bound];
    }
    else if ( (pr->label == LAYOUT_CART_PARAM_LABEL_BOOST_ROUTING_ENV_1) || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_BOOST_ROUTING_ENV_2) || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_BOOST_ROUTING_ENV_3))
    {
      value = (p->boost_routing & pr->upper_bound);
      value_string = S_patch_edit_routing_values[value / pr->upper_bound];
    }
    else if ( (pr->label == LAYOUT_CART_PARAM_LABEL_VELOCITY_ROUTING_ENV_1) || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_VELOCITY_ROUTING_ENV_2) || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_VELOCITY_ROUTING_ENV_3))
    {
      value = (p->velocity_routing & pr->upper_bound);
      value_string = S_patch_edit_routing_values[value / pr->upper_bound];
    }
    /* midi controller routing */
    else if ( (pr->label == LAYOUT_CART_PARAM_LABEL_MOD_WHEEL_ROUTING_VIBRATO)  || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_MOD_WHEEL_ROUTING_TREMOLO)  || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_MOD_WHEEL_ROUTING_BOOST)    || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_MOD_WHEEL_ROUTING_CHORUS))
    {
      value = (p->mod_wheel_routing & pr->upper_bound);
      value_string = S_patch_edit_routing_values[value / pr->upper_bound];
    }
    else if ( (pr->label == LAYOUT_CART_PARAM_LABEL_AFTERTOUCH_ROUTING_VIBRATO)  || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_AFTERTOUCH_ROUTING_TREMOLO)  || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_AFTERTOUCH_ROUTING_BOOST)    || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_AFTERTOUCH_ROUTING_CHORUS))
    {
      value = (p->aftertouch_routing & pr->upper_bound);
      value_string = S_patch_edit_routing_values[value / pr->upper_bound];
    }
    else if ( (pr->label == LAYOUT_CART_PARAM_LABEL_EXP_PEDAL_ROUTING_VIBRATO)  || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_EXP_PEDAL_ROUTING_TREMOLO)  || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_EXP_PEDAL_ROUTING_BOOST)    || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_EXP_PEDAL_ROUTING_CHORUS))
    {
      value = (p->exp_pedal_routing & pr->upper_bound);
      value_string = S_patch_edit_routing_values[value / pr->upper_bound];
    }
    /* audition */
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_OCTAVE)
    {
      value = G_patch_edit_octave;
      value_string = S_common_edit_1_to_100_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_VELOCITY)
    {
      value = G_patch_edit_note_velocity;
      value_string = S_patch_edit_audition_uni_wheel_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_PITCH_WHEEL)
    {
      value = G_patch_edit_pitch_wheel_pos;
      value_string = S_patch_edit_audition_bi_wheel_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_MOD_WHEEL)
    {
      value = G_patch_edit_mod_wheel_pos;
      value_string = S_patch_edit_audition_uni_wheel_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_AFTERTOUCH)
    {
      value = G_patch_edit_aftertouch_pos;
      value_string = S_patch_edit_audition_uni_wheel_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_EXP_PEDAL)
    {
      value = G_patch_edit_exp_pedal_pos;
      value_string = S_patch_edit_audition_uni_wheel_values[value - pr->lower_bound];
    }
    else
    {
      value = 0;
      value_string = NULL;
    }

    /* load the parameter name, value, and slider, arrows, or radio button */
    if (pr->type == LAYOUT_PARAM_TYPE_SLIDER)
    {
      vb_all_load_text( pr->center_x + LAYOUT_CART_PARAM_SLIDER_NAME_X, pos_y, 
                        VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_2, VB_ALL_PARAM_NAME_MAX_TEXT_SIZE, 
                        S_cart_param_labels[pr->label]);

      vb_all_load_text( pr->center_x + LAYOUT_CART_PARAM_SLIDER_VALUE_X, pos_y, 
                        VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_GRAY, VB_ALL_PARAM_NUMBER_MAX_TEXT_SIZE, 
                        value_string);

      vb_all_load_slider( pr->center_x + LAYOUT_CART_PARAM_SLIDER_TRACK_X, pos_y, 
                          LAYOUT_CART_PARAM_SLIDER_WIDTH, 
                          value, pr->lower_bound, pr->upper_bound);
    }
    else if (pr->type == LAYOUT_PARAM_TYPE_ARROWS)
    {
      vb_all_load_text( pr->center_x + LAYOUT_CART_PARAM_ARROWS_NAME_X, pos_y, 
                        VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_2, VB_ALL_PARAM_NAME_MAX_TEXT_SIZE, 
                        S_cart_param_labels[pr->label]);

      vb_all_load_text( pr->center_x + LAYOUT_CART_PARAM_ARROWS_VALUE_X, pos_y, 
                        VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_GRAY, VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE, 
                        value_string);

      if (value > pr->lower_bound)
      {
        vb_all_load_named_sprite( VB_ALL_SPRITE_NAME_PARAM_ARROWS_LEFT, 
                                  pr->center_x + LAYOUT_CART_PARAM_ARROWS_LEFT_X, pos_y, 
                                  0, VB_ALL_PALETTE_1);
      }

      if (value < pr->upper_bound)
      {
        vb_all_load_named_sprite( VB_ALL_SPRITE_NAME_PARAM_ARROWS_RIGHT, 
                                  pr->center_x + LAYOUT_CART_PARAM_ARROWS_RIGHT_X, pos_y, 
                                  0, VB_ALL_PALETTE_1);
      }
    }
    else if (pr->type == LAYOUT_PARAM_TYPE_RADIO)
    {
      vb_all_load_text( pr->center_x + LAYOUT_CART_PARAM_RADIO_NAME_X, pos_y, 
                        VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_2, VB_ALL_PARAM_NAME_MAX_TEXT_SIZE, 
                        S_cart_param_labels[pr->label]);

      vb_all_load_text( pr->center_x + LAYOUT_CART_PARAM_RADIO_VALUE_X, pos_y, 
                        VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_GRAY, VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE, 
                        value_string);

      if (value == pr->lower_bound)
      {
        vb_all_load_named_sprite( VB_ALL_SPRITE_NAME_PARAM_RADIO_BUTTON_OFF, 
                                  pr->center_x + LAYOUT_CART_PARAM_RADIO_BUTTON_X, pos_y, 
                                  0, VB_ALL_PALETTE_1);
      }
      else
      {
        vb_all_load_named_sprite( VB_ALL_SPRITE_NAME_PARAM_RADIO_BUTTON_ON, 
                                  pr->center_x + LAYOUT_CART_PARAM_RADIO_BUTTON_X, pos_y, 
                                  0, VB_ALL_PALETTE_1);
      }
    }
    else if (pr->type == LAYOUT_PARAM_TYPE_FLAG)
    {
      vb_all_load_text( pr->center_x + LAYOUT_CART_PARAM_RADIO_NAME_X, pos_y, 
                        VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_2, VB_ALL_PARAM_NAME_MAX_TEXT_SIZE, 
                        S_cart_param_labels[pr->label]);

      vb_all_load_text( pr->center_x + LAYOUT_CART_PARAM_RADIO_VALUE_X, pos_y, 
                        VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_GRAY, VB_ALL_PARAM_VALUE_MAX_TEXT_SIZE, 
                        value_string);

      if ((value & pr->upper_bound) == 0)
      {
        vb_all_load_named_sprite( VB_ALL_SPRITE_NAME_PARAM_RADIO_BUTTON_OFF, 
                                  pr->center_x + LAYOUT_CART_PARAM_RADIO_BUTTON_X, pos_y, 
                                  0, VB_ALL_PALETTE_1);
      }
      else
      {
        vb_all_load_named_sprite( VB_ALL_SPRITE_NAME_PARAM_RADIO_BUTTON_ON, 
                                  pr->center_x + LAYOUT_CART_PARAM_RADIO_BUTTON_X, pos_y, 
                                  0, VB_ALL_PALETTE_1);
      }
    }
  }

  /* update vbos */
  VB_ALL_UPDATE_PANELS_SPRITES_IN_VBOS()
  VB_ALL_UPDATE_WIDGETS_SPRITES_IN_VBOS()
  VB_ALL_UPDATE_TEXT_SPRITES_IN_VBOS()

  return 0;
}

/*******************************************************************************
** vb_all_load_instruments_screen()
*******************************************************************************/
short int vb_all_load_instruments_screen()
{

  /* update vbos */
  VB_ALL_UPDATE_PANELS_SPRITES_IN_VBOS()
  VB_ALL_UPDATE_WIDGETS_SPRITES_IN_VBOS()
  VB_ALL_UPDATE_TEXT_SPRITES_IN_VBOS()

  return 0;
}

/*******************************************************************************
** vb_all_load_song_screen()
*******************************************************************************/
short int vb_all_load_song_screen()
{

  /* update vbos */
  VB_ALL_UPDATE_PANELS_SPRITES_IN_VBOS()
  VB_ALL_UPDATE_WIDGETS_SPRITES_IN_VBOS()
  VB_ALL_UPDATE_TEXT_SPRITES_IN_VBOS()

  return 0;
}

/*******************************************************************************
** vb_all_load_mixer_screen()
*******************************************************************************/
short int vb_all_load_mixer_screen()
{

  /* update vbos */
  VB_ALL_UPDATE_PANELS_SPRITES_IN_VBOS()
  VB_ALL_UPDATE_WIDGETS_SPRITES_IN_VBOS()
  VB_ALL_UPDATE_TEXT_SPRITES_IN_VBOS()

  return 0;
}

/*******************************************************************************
** vb_all_load_sound_fx_screen()
*******************************************************************************/
short int vb_all_load_sound_fx_screen()
{

  /* update vbos */
  VB_ALL_UPDATE_PANELS_SPRITES_IN_VBOS()
  VB_ALL_UPDATE_WIDGETS_SPRITES_IN_VBOS()
  VB_ALL_UPDATE_TEXT_SPRITES_IN_VBOS()

  return 0;
}

/*******************************************************************************
** vb_all_load_dpcm_screen()
*******************************************************************************/
short int vb_all_load_dpcm_screen()
{

  /* update vbos */
  VB_ALL_UPDATE_PANELS_SPRITES_IN_VBOS()
  VB_ALL_UPDATE_WIDGETS_SPRITES_IN_VBOS()
  VB_ALL_UPDATE_TEXT_SPRITES_IN_VBOS()

  return 0;
}

/*******************************************************************************
** vb_all_load_bar_screen()
*******************************************************************************/
short int vb_all_load_bar_screen()
{

  /* update vbos */
  VB_ALL_UPDATE_PANELS_SPRITES_IN_VBOS()
  VB_ALL_UPDATE_WIDGETS_SPRITES_IN_VBOS()
  VB_ALL_UPDATE_TEXT_SPRITES_IN_VBOS()

  return 0;
}

