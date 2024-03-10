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
#define VB_ALL_HEADER_MAX_TEXT_SIZE 12
#define VB_ALL_PARAM_MAX_TEXT_SIZE  10

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
    VB_ALL_ADD_SPRITE_TO_VERTEX_BUFFER(pos_x, pos_y, 1, 2, GRAPHICS_Z_LEVEL_WIDGETS)        \
    VB_ALL_ADD_SPRITE_TO_TEXTURE_COORD_BUFFER(cell_x, cell_y, 1, 2)                         \
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

#define VB_ALL_ADD_SCROLLBAR_MOVING_PART_TO_BUFFERS(pos_x, pos_y, lighting, palette)  \
  if (sprite_index < GRAPHICS_TEXT_SPRITES_END_INDEX)                                 \
  {                                                                                   \
    VB_ALL_ADD_SPRITE_TO_VERTEX_BUFFER(pos_x, pos_y, 1, 2, GRAPHICS_Z_LEVEL_TEXT)     \
    VB_ALL_ADD_SPRITE_TO_TEXTURE_COORD_BUFFER(14, 8, 1, 2)                            \
    VB_ALL_ADD_SPRITE_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)               \
    VB_ALL_ADD_SPRITE_TO_ELEMENT_BUFFER()                                             \
                                                                                      \
    sprite_index += 1;                                                                \
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
static char S_common_edit_1_to_64_values[64][4] = 
  { "1",  "2",  "3",  "4",  "5",  "6",  "7", "8", 
    "9", "10", "11", "12", "13", "14", "15", "16", 
   "17", "18", "19", "20", "21", "22", "23", "24", 
   "25", "26", "27", "28", "29", "30", "31", "32", 
   "33", "34", "35", "36", "37", "38", "39", "40", 
   "41", "42", "43", "44", "45", "46", "47", "48", 
   "49", "50", "51", "52", "53", "54", "55", "56", 
   "57", "58", "59", "60", "61", "62", "63", "64" 
  };

static char S_common_edit_0_to_64_values[65][4] = 
  { "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7", "8", 
          "9", "10", "11", "12", "13", "14", "15", "16", 
         "17", "18", "19", "20", "21", "22", "23", "24", 
         "25", "26", "27", "28", "29", "30", "31", "32", 
         "33", "34", "35", "36", "37", "38", "39", "40", 
         "41", "42", "43", "44", "45", "46", "47", "48", 
         "49", "50", "51", "52", "53", "54", "55", "56", 
         "57", "58", "59", "60", "61", "62", "63", "64"
  };

/* top panel tables */
static char S_top_panel_button_labels[LAYOUT_TOP_PANEL_BUTTON_NUM_LABELS][12] = 
  { "Cart", "Instruments", "Song", "Mixer", "Sound FX", "DPCM" };

static char S_top_panel_header_labels[LAYOUT_TOP_PANEL_HEADER_NUM_LABELS][12] = 
  { "Gersemi", "v0.9" };

/* cart edit tables */
static char S_cart_button_labels[LAYOUT_CART_BUTTON_NUM_LABELS][12] = 
  { "Load", "Save", "Copy", "Zap", 
    "Load", "Save", "Copy", "Zap" 
  };

static char S_cart_header_labels[LAYOUT_CART_HEADER_NUM_LABELS][12] = 
  { "Osc 1", "Osc 2", "Osc 3", "Osc 4", 
    "Env 1", "Env 2", "Env 3", "Env 4", 
    "Portamento", "Arpeggio", "PEG Rates", "PEG Levels", 
    "LFO", "Vibrato", "Tremolo", "Effects", 
    "Pedal", "Velocity", "Noise", "Boost", 
    "Pitch Wheel", "Legacy", "Sync", "Filters" 
  };

static char S_cart_param_labels[LAYOUT_CART_PARAM_NUM_LABELS][4] = 
  { "Car", "n/a", "Pat", "n/a", 
    "Alg", "n/a", 
    "Wav", "FBk", "Phi", "Frq", "Mul", "Div", "Det",                /* osc */
    "Att",  "D1",  "D2", "Rel", "Lev", "Sus", "RtS", "LvS", "Brk",  /* env */
    "Mde", "Fol", "Leg", "Spd", /* portamento */
    "Mde", "Pat", "Oct", "Spd", /* arpeggio */
    "Att", "Dec", "Rel", "Leg", /* pitch env rates */
    "Lev", "Sus", "Fin", "Trn", /* pitch env levels */
    "Wav", "Frq", "Dly", "Qua", /* lfo */
    "Mde", "Dep", "Bas",        /* vibrato */
    "Mde", "Dep", "Bas",        /* tremolo */
     "MW",  "AT",  "EP",        /* effects */
    "Adj",                      /* pedal */
    "Mde", "Scl",               /* velocity */
    "Mde", "Frq",               /* noise */
    "Mde", "Dep",               /* boost */
    "Mde", "Rng",               /* pitch wheel */
    "Mul", "RtS",               /* legacy */
    "Osc", "LFO",               /* sync */
     "HP",  "LP",               /* filters */
    "Vel",  "MW",  "AT",  "PW" 
  };

static char S_patch_edit_algorithm_values[PATCH_ALGORITHM_NUM_VALUES][4] = 
  { "1", "2", "3", "4", "5", "6", "7", "8" };

static char S_patch_edit_alg_name_values[PATCH_ALGORITHM_NUM_VALUES][12] = 
  { "1C Chain", 
    "1C The Y", 
    "1C Crab", 
    "2C Twin", 
    "2C Stacked", 
    "3C 1 to 3", 
    "3C 1 to 1", 
    "4C Pipes" 
  };

static char S_patch_edit_osc_waveform_values[PATCH_OSC_WAVEFORM_NUM_VALUES][6] = 
  { "Sine", "Half", "Full", "Quar", "Alt", "Cam", "Squa", "LSaw" };

static char S_patch_edit_osc_phi_values[PATCH_OSC_PHI_NUM_VALUES][4] = 
  {   "0",  "30",  "45",  "60",  "90", "120", "135", "150", 
    "180", "210", "225", "240", "270", "300", "315", "330" 
  };

static char S_patch_edit_osc_freq_mode_values[PATCH_OSC_FREQ_MODE_NUM_VALUES][6] = 
  { "Ratio", "Fixed" };

static char S_patch_edit_osc_detune_values[PATCH_OSC_DETUNE_NUM_VALUES][4] = 
  { "-32", "-31", "-30", "-29", "-28", "-27", "-26", "-25", 
    "-24", "-23", "-22", "-21", "-20", "-19", "-18", "-17", 
    "-16", "-15", "-14", "-13", "-12", "-11", "-10",  "-9", 
     "-8",  "-7",  "-6",  "-5",  "-4",  "-3",  "-2",  "-1", 
      "0", 
      "1",   "2",   "3",   "4",   "5",   "6",   "7",   "8", 
      "9",  "10",  "11",  "12",  "13",  "14",  "15",  "16", 
     "17",  "18",  "19",  "20",  "21",  "22",  "23",  "24", 
     "25",  "26",  "27",  "28",  "29",  "30",  "31",  "32" 
  };

static char S_patch_edit_env_break_point_values[PATCH_ENV_BREAK_POINT_NUM_VALUES][4] = 
  { "A0", "D1", "A1", "D2", "A2", "D3", "A3", "D4", 
    "A4", "D5", "A5", "D6", "A6", "D7", "A7", "C8" 
  };

static char S_patch_edit_portamento_mode_values[PATCH_PORTAMENTO_MODE_NUM_VALUES][6] = 
  { "Bend", "Half" };

static char S_patch_edit_portamento_follow_values[PATCH_PORTAMENTO_FOLLOW_NUM_VALUES][6] = 
  { "Cont", "Retrn" };

static char S_patch_edit_portamento_legato_values[PATCH_PORTAMENTO_LEGATO_NUM_VALUES][6] = 
  { "Off", "On" };

static char S_patch_edit_arpeggio_mode_values[PATCH_ARPEGGIO_MODE_NUM_VALUES][6] = 
  { "Harp", "Roll" };

static char S_patch_edit_arpeggio_pattern_values[PATCH_ARPEGGIO_PATTERN_NUM_VALUES][6] = 
  { "Up", "Down", "U/D", "Alt" };

static char S_patch_edit_peg_level_values[PATCH_PEG_LEVEL_NUM_VALUES][4] = 
  { "-24", "-23", "-22", "-21", "-20", "-19", "-18", "-17", "-16", "-15", "-14", "-13", 
    "-12", "-11", "-10",  "-9",  "-8",  "-7",  "-6",  "-5",  "-4",  "-3",  "-2",  "-1", 
      "0", 
      "1",   "2",   "3",   "4",   "5",   "6",   "7",   "8",   "9",  "10",  "11",  "12", 
     "13",  "14",  "15",  "16",  "17",  "18",  "19",  "20",  "21",  "22",  "23",  "24"
  };

static char S_patch_edit_peg_legato_values[PATCH_PEG_LEGATO_NUM_VALUES][6] = 
  { "Off", "On" };

static char S_patch_edit_peg_transpose_values[PATCH_PEG_TRANSPOSE_NUM_VALUES][6] = 
  { "Off", "On" };

static char S_patch_edit_lfo_waveform_values[PATCH_LFO_WAVEFORM_NUM_VALUES][6] = 
  { "Tri", "Squa", "SawU", "SawD", "NSqu", "NSaw" };

static char S_patch_edit_vibrato_mode_values[PATCH_VIBRATO_MODE_NUM_VALUES][6] = 
  { "U/D", "Up" };

static char S_patch_edit_tremolo_mode_values[PATCH_TREMOLO_MODE_NUM_VALUES][6] = 
  { "Car", "Mod" };

static char S_patch_edit_controller_effect_values[PATCH_CONTROLLER_EFFECT_NUM_VALUES][6] = 
  { "Vib", "Trm", "Bst", "V+T", "V+B", "T+B", "All" };

static char S_patch_edit_pedal_adjust_values[PATCH_PEDAL_ADJUST_NUM_VALUES][4] = 
  {  "0",  "2",  "4",  "6",  "8", "10", "12", "14", "16" };

static char S_patch_edit_velocity_mode_values[PATCH_VELOCITY_MODE_NUM_VALUES][6] = 
  { "Car", "Mod" };

static char S_patch_edit_noise_mode_values[PATCH_NOISE_MODE_NUM_VALUES][6] = 
  { "Off", "Squa", "Saw" };

static char S_patch_edit_boost_mode_values[PATCH_BOOST_MODE_NUM_VALUES][6] = 
  { "Car", "Mod" };

static char S_patch_edit_pitch_wheel_mode_values[PATCH_PITCH_WHEEL_MODE_NUM_VALUES][6] = 
  { "Bend", "Half" };

static char S_patch_edit_legacy_multiple_values[PATCH_LEGACY_MULTIPLE_NUM_VALUES][6] = 
  { "Harm", "Int" };

static char S_patch_edit_legacy_keyscale_values[PATCH_LEGACY_KEYSCALE_NUM_VALUES][6] = 
  { "Pitch", "Key" };

static char S_patch_edit_sync_values[PATCH_SYNC_NUM_VALUES][6] = 
  { "Off", "On" };

static char S_patch_edit_highpass_cutoff_values[PATCH_HIGHPASS_CUTOFF_NUM_VALUES][4] = 
  { "A0", "A1", "A2", "A3" };

static char S_patch_edit_lowpass_cutoff_values[PATCH_LOWPASS_CUTOFF_NUM_VALUES][4] = 
  { "E7", "G7", "A7", "C8" };

static char S_patch_edit_audition_note_velocity_values[MIDI_CONT_NOTE_VELOCITY_NUM_VALUES][4] = 
  { "-12", "-11", "-10",  "-9", 
     "-8",  "-7",  "-6",  "-5", 
     "-4",  "-3",  "-2",  "-1", 
      "0", 
      "1",   "2",   "3",   "4" 
  };

static char S_patch_edit_audition_pitch_wheel_values[MIDI_CONT_PITCH_WHEEL_NUM_VALUES][4] = 
  { "-16", "-15", "-14", "-13", "-12", "-11", "-10",  "-9", 
     "-8",  "-7",  "-6",  "-5",  "-4",  "-3",  "-2",  "-1", 
      "0", 
      "1",   "2",   "3",   "4",   "5",   "6",   "7",   "8", 
      "9",  "10",  "11",  "12",  "13",  "14",  "15",  "16" 
  };

/* load tables */
static char S_load_button_labels[LAYOUT_LOAD_BUTTON_NUM_LABELS][12] = 
  { "Load" };

static char S_load_header_labels[LAYOUT_LOAD_HEADER_NUM_LABELS][12] = 
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
  if ((width < 2) || (width > GRAPHICS_OVERSCAN_WIDTH / 8))
    return 1;

  if ((height < 2) || (height > GRAPHICS_OVERSCAN_HEIGHT / 8))
    return 1;

  /* determine coordinates of top left corner */
  corner_x = (GRAPHICS_OVERSCAN_WIDTH - 8 * width) / 2;
  corner_y = (GRAPHICS_OVERSCAN_HEIGHT - 8 * height) / 2;

  /* the offsets from the screen center are in 4x4 half-cells */
  corner_x += 4 * offset_x;
  corner_y += 4 * offset_y;

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
      pos_x = corner_x + (8 * m) + 4;
      pos_y = corner_y + (8 * n) + 4;

      /* top left corner piece */
      if ((m == 0) && (n == 0))
      {
        cell_x = 0;
        cell_y = 12;
      }
      /* bottom left corner piece */
      else if ((m == 0) && (n == height - 1))
      {
        cell_x = 0;
        cell_y = 15;
      }
      /* top right corner piece */
      else if ((m == width - 1) && (n == 0))
      {
        cell_x = 3;
        cell_y = 12;
      }
      /* bottom right corner piece */
      else if ((m == width - 1) && (n == height - 1))
      {
        cell_x = 3;
        cell_y = 15;
      }
      /* top piece */
      else if (n == 0)
      {
        cell_x = 1;
        cell_y = 12;
      }
      /* bottom piece */
      else if (n == height - 1)
      {
        cell_x = 1;
        cell_y = 15;
      }
      /* left piece */
      else if (m == 0)
      {
        cell_x = 0;
        cell_y = 13;
      }
      /* right piece */
      else if (m == width - 1)
      {
        cell_x = 3;
        cell_y = 13;
      }
      /* middle piece */
      else
      {
        cell_x = 1;
        cell_y = 13;
      }

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
  if ((width < 2) || (width > GRAPHICS_OVERSCAN_WIDTH / 8))
    return 1;

  /* determine coordinates of top left corner */
  corner_x = (GRAPHICS_OVERSCAN_WIDTH - 8 * width) / 2;
  corner_y = (GRAPHICS_OVERSCAN_HEIGHT - 16 * 1) / 2;

  /* the offsets from the screen center are in 4x4 half-cells */
  corner_x += 4 * offset_x;
  corner_y += 4 * offset_y;

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

  for (m = 0; m < width; m++)
  {
    /* determine center of this piece */
    pos_x = corner_x + (8 * m) + 4;
    pos_y = corner_y + (16 * 0) + 8;

    /* left piece */
    if (m == 0)
    {
      cell_x = 10;
      cell_y = 6;
    }
    /* right piece */
    else if (m == width - 1)
    {
      cell_x = 13;
      cell_y = 6;
    }
    /* middle piece */
    else
    {
      cell_x = 11;
      cell_y = 6;
    }

    VB_ALL_ADD_BUTTON_PIECE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette)
  }

  /* update buttons sprite layer count */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_WIDGETS] = 
    sprite_index - GRAPHICS_WIDGETS_SPRITES_START_INDEX;

  return 0;
}

/*******************************************************************************
** vb_all_load_vertical_scrollbar_track()
*******************************************************************************/
short int vb_all_load_vertical_scrollbar_track( int offset_x, int offset_y, 
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
  if ((height < 4) || (height > GRAPHICS_OVERSCAN_HEIGHT / 8))
    return 1;

  /* determine coordinates of top left corner */
  corner_x = (GRAPHICS_OVERSCAN_WIDTH - 8 * 1) / 2;
  corner_y = (GRAPHICS_OVERSCAN_HEIGHT - 8 * height) / 2;

  /* the offsets from the screen center are in 4x4 half-cells */
  corner_x += 4 * offset_x;
  corner_y += 4 * offset_y;

  /* set lighting and palette */
  lighting = 0;
  palette = VB_ALL_PALETTE_1;

  /* draw the scrollbar track */
  sprite_index =  GRAPHICS_WIDGETS_SPRITES_START_INDEX + 
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_WIDGETS];

  for (n = 0; n < height; n++)
  {
    /* determine center of this piece */
    pos_x = corner_x + (8 * 0) + 4;
    pos_y = corner_y + (8 * n) + 4;

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
** vb_all_load_vertical_scrollbar_moving_part()
*******************************************************************************/
short int vb_all_load_vertical_scrollbar_moving_part( int offset_x, int offset_y, 
                                                      int height, 
                                                      int current_scroll_amount, 
                                                      int max_scroll_amount)
{
  int sprite_index;

  int corner_x;
  int corner_y;

  int pos_x;
  int pos_y;

  int lighting;
  int palette;

  /* make sure the height is valid        */
  /* the height is in terms of 8x8 cells  */
  if ((height < 4) || (height > GRAPHICS_OVERSCAN_HEIGHT / 8))
    return 1;

  /* make sure the scroll amount is valid */
  if ((current_scroll_amount < 0) || (current_scroll_amount > max_scroll_amount))
    return 1;

  /* determine coordinates of top left corner */
  corner_x = (GRAPHICS_OVERSCAN_WIDTH - 8 * 1) / 2;
  corner_y = (GRAPHICS_OVERSCAN_HEIGHT - 16 * 1) / 2;

  /* the offsets from the screen center are in 4x4 half-cells */
  corner_x += 4 * offset_x;
  corner_y += 4 * offset_y;

  /* set lighting and palette */
  lighting = 0;
  palette = VB_ALL_PALETTE_1;

  /* draw the scrollbar's slider */
  sprite_index =  GRAPHICS_TEXT_SPRITES_START_INDEX + 
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_TEXT];

  /* determine vertical position */
  pos_x = corner_x + (8 * 0) + 4;
  pos_y = corner_y + (16 * 0) + 8;

  pos_y -= 4 * (height - 4);

  if (max_scroll_amount > 0)
    pos_y += (current_scroll_amount * 8 * (height - 4)) / max_scroll_amount;

  lighting = 0;
  palette = VB_ALL_PALETTE_1;

  VB_ALL_ADD_SCROLLBAR_MOVING_PART_TO_BUFFERS(pos_x, pos_y, lighting, palette)

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
  if ((width < 3) || (width > GRAPHICS_OVERSCAN_WIDTH / 8))
    return 1;

  /* determine coordinates of top left corner */
  corner_x = (GRAPHICS_OVERSCAN_WIDTH - 8 * width) / 2;
  corner_y = (GRAPHICS_OVERSCAN_HEIGHT - 8 * 1) / 2;

  /* the offsets from the screen center are in 4x4 half-cells */
  corner_x += 4 * offset_x;
  corner_y += 4 * offset_y;

  /* set lighting and palette */
  lighting = 0;
  palette = VB_ALL_PALETTE_1;

  /* draw the divider */
  sprite_index =  GRAPHICS_WIDGETS_SPRITES_START_INDEX + 
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_WIDGETS];

  for (m = 0; m < width; m++)
  {
    /* determine center of this piece */
    pos_x = corner_x + (8 * m) + 4;
    pos_y = corner_y + (8 * 0) + 4;

    /* 1st piece */
    if (m % 3 == 0)
    {
      cell_x = 7;
      cell_y = 7;
    }
    /* 2nd piece */
    else if (m % 3 == 1)
    {
      cell_x = 8;
      cell_y = 7;
    }
    /* 3rd piece */
    else
    {
      cell_x = 9;
      cell_y = 7;
    }

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
  start_x = (GRAPHICS_OVERSCAN_WIDTH / 2);
  start_y = (GRAPHICS_OVERSCAN_HEIGHT / 2);

  /* left:    the center of the first character is at the screen center */
  /* center:  the center of the whole string is at the screen center    */
  /* right:   the center of the last character is at the screen center  */
  if (align == VB_ALL_ALIGN_LEFT)
    start_x += 0;
  else if (align == VB_ALL_ALIGN_CENTER)
    start_x -= 4 * (length - 1);
  else if (align == VB_ALL_ALIGN_RIGHT)
    start_x -= 8 * (length - 1);

  /* the offsets from the screen center are in 4x4 half-cells */
  start_x += 4 * offset_x;
  start_y += 4 * offset_y;

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
  pos_x = (GRAPHICS_OVERSCAN_WIDTH / 2);
  pos_y = (GRAPHICS_OVERSCAN_HEIGHT / 2);

  /* the offsets from the screen center are in 4x4 half-cells */
  pos_x += 4 * offset_x;
  pos_y += 4 * offset_y;

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
  if ((width < 2) || (width > GRAPHICS_OVERSCAN_WIDTH / 8))
    return 1;

  /* make sure the bounds are valid */
  if (upper_bound <= lower_bound)
    return 1;

  if ((value < lower_bound) || (value > upper_bound))
    return 1;

  /* determine coordinates of top left corner */
  corner_x = (GRAPHICS_OVERSCAN_WIDTH - 8 * width) / 2;
  corner_y = (GRAPHICS_OVERSCAN_HEIGHT - 8 * 1) / 2;

  /* the offsets from the screen center are in 4x4 half-cells */
  corner_x += 4 * offset_x;
  corner_y += 4 * offset_y;

  /* set lighting and palette */
  lighting = 0;
  palette = VB_ALL_PALETTE_1;

  /* draw the slider track */
  sprite_index =  GRAPHICS_WIDGETS_SPRITES_START_INDEX + 
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_WIDGETS];

  for (m = 0; m < width; m++)
  {
    /* determine center of this piece */
    pos_x = corner_x + (8 * m) + 4;
    pos_y = corner_y + (8 * 0) + 4;

    /* left piece */
    if (m == 0)
    {
      cell_x = 1;
      cell_y = 7;
    }
    /* right piece */
    else if (m == width - 1)
    {
      cell_x = 3;
      cell_y = 7;
    }
    /* middle piece */
    else
    {
      cell_x = 2;
      cell_y = 7;
    }

    VB_ALL_ADD_SLIDER_TRACK_PIECE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette)
  }

  /* update underlay sprite layer count */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_WIDGETS] = 
    sprite_index - GRAPHICS_WIDGETS_SPRITES_START_INDEX;

  /* draw the slider itself */
  sprite_index =  GRAPHICS_TEXT_SPRITES_START_INDEX + 
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_TEXT];

  pos_x = corner_x + (8 * 0) + 4;
  pos_y = corner_y + (8 * 0) + 4;

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
  vb_all_load_vertical_scrollbar_track( LAYOUT_SCROLLBAR_AREA_X, 
                                        LAYOUT_SCROLLBAR_AREA_Y, 
                                        LAYOUT_SCROLLBAR_AREA_HEIGHT);

  vb_all_load_vertical_scrollbar_moving_part( LAYOUT_SCROLLBAR_AREA_X, 
                                              LAYOUT_SCROLLBAR_AREA_Y, 
                                              LAYOUT_SCROLLBAR_AREA_HEIGHT, 
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
                      VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_GRAY, VB_ALL_PARAM_MAX_TEXT_SIZE, 
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

    /* determine vertical position for headers (can be scrolled up/down) */
    pos_y = b->center_y - G_current_scroll_amount;

    if (LAYOUT_CART_HEADER_OR_PARAM_IS_NOT_IN_MAIN_AREA(pos_y))
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

    /* determine vertical position for headers (can be scrolled up/down) */
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
    if ((pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_VELOCITY)    || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_MOD_WHEEL)   || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_AFTERTOUCH)  || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_PITCH_WHEEL))
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

    /* determine parameter value */
    if (pr->label == LAYOUT_CART_PARAM_LABEL_CART_NUMBER)
      value = G_patch_edit_cart_number;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PATCH_NUMBER)
      value = G_patch_edit_patch_number;
    else if ( (pr->label == LAYOUT_CART_PARAM_LABEL_ALGORITHM) || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_ALG_NAME))
    {
      value = p->algorithm;
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_WAVEFORM)
      value = p->osc_waveform[pr->num];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_FEEDBACK)
      value = p->osc_feedback[pr->num];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_PHI)
      value = p->osc_phi[pr->num];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_FREQ_MODE)
      value = p->osc_freq_mode[pr->num];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_MULTIPLE)
      value = p->osc_multiple[pr->num];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_DIVISOR)
      value = p->osc_divisor[pr->num];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_DETUNE)
      value = p->osc_detune[pr->num];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_ATTACK)
      value = p->env_attack[pr->num];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_DECAY_1)
      value = p->env_decay_1[pr->num];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_DECAY_2)
      value = p->env_decay_2[pr->num];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_RELEASE)
      value = p->env_release[pr->num];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_AMPLITUDE)
      value = p->env_amplitude[pr->num];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_SUSTAIN)
      value = p->env_sustain[pr->num];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_RATE_KS)
      value = p->env_rate_ks[pr->num];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_LEVEL_KS)
      value = p->env_level_ks[pr->num];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_BREAK_POINT)
      value = p->env_break_point[pr->num];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PORTAMENTO_MODE)
      value = p->portamento_mode;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PORTAMENTO_FOLLOW)
      value = p->portamento_follow;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PORTAMENTO_LEGATO)
      value = p->portamento_legato;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PORTAMENTO_SPEED)
      value = p->portamento_speed;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ARPEGGIO_MODE)
      value = p->arpeggio_mode;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ARPEGGIO_PATTERN)
      value = p->arpeggio_pattern;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ARPEGGIO_OCTAVES)
      value = p->arpeggio_octaves;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ARPEGGIO_SPEED)
      value = p->arpeggio_speed;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PEG_ATTACK)
      value = p->peg_attack;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PEG_DECAY)
      value = p->peg_decay;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PEG_RELEASE)
      value = p->peg_release;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PEG_LEGATO)
      value = p->peg_legato;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PEG_AMPLITUDE)
      value = p->peg_amplitude;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PEG_SUSTAIN)
      value = p->peg_sustain;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PEG_FINALE)
      value = p->peg_finale;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PEG_TRANSPOSE)
      value = p->peg_transpose;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_LFO_WAVEFORM)
      value = p->lfo_waveform;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_LFO_FREQUENCY)
      value = p->lfo_frequency;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_LFO_DELAY)
      value = p->lfo_delay;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_LFO_QUANTIZE)
      value = p->lfo_quantize;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_VIBRATO_MODE)
      value = p->vibrato_mode;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_VIBRATO_DEPTH)
      value = p->vibrato_depth;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_VIBRATO_BASE)
      value = p->vibrato_base;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_MODE)
      value = p->tremolo_mode;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_DEPTH)
      value = p->tremolo_depth;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_BASE)
      value = p->tremolo_base;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_MOD_WHEEL_EFFECT)
      value = p->mod_wheel_effect;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AFTERTOUCH_EFFECT)
      value = p->aftertouch_effect;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_EXP_PEDAL_EFFECT)
      value = p->exp_pedal_effect;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PEDAL_ADJUST)
      value = p->pedal_adjust;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_VELOCITY_MODE)
      value = p->velocity_mode;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_VELOCITY_SCALING)
      value = p->velocity_scaling;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_NOISE_MODE)
      value = p->noise_mode;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_NOISE_FREQUENCY)
      value = p->noise_frequency;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_BOOST_MODE)
      value = p->boost_mode;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_BOOST_DEPTH)
      value = p->boost_depth;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_WHEEL_MODE)
      value = p->pitch_wheel_mode;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_WHEEL_RANGE)
      value = p->pitch_wheel_range;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_LEGACY_MULTIPLE)
      value = p->legacy_multiple;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_LEGACY_KEYSCALE)
      value = p->legacy_keyscale;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_SYNC_OSC)
      value = p->sync_osc;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_SYNC_LFO)
      value = p->sync_lfo;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_HIGHPASS_CUTOFF)
      value = p->highpass_cutoff;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_LOWPASS_CUTOFF)
      value = p->lowpass_cutoff;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_VELOCITY)
      value = G_patch_edit_note_velocity;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_MOD_WHEEL)
      value = G_patch_edit_mod_wheel_pos;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_AFTERTOUCH)
      value = G_patch_edit_aftertouch_pos;
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_PITCH_WHEEL)
      value = G_patch_edit_pitch_wheel_pos;
    else
      value = 0;

    /* determine parameter string */
    if ((pr->label == LAYOUT_CART_PARAM_LABEL_CART_NUMBER)        || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_PATCH_NUMBER)       || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_MULTIPLE)       || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_DIVISOR)        || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_ATTACK)         || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_DECAY_1)        || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_DECAY_2)        || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_RELEASE)        || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_RATE_KS)        || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_LEVEL_KS)       || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_PORTAMENTO_SPEED)   || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_ARPEGGIO_OCTAVES)   || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_ARPEGGIO_SPEED)     || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_PEG_ATTACK)         || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_PEG_DECAY)          || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_PEG_RELEASE)        || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_LFO_FREQUENCY)      || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_LFO_QUANTIZE)       || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_VIBRATO_DEPTH)      || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_DEPTH)      || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_NOISE_FREQUENCY)    || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_BOOST_DEPTH)        || 
        (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_WHEEL_RANGE))
    {
      value_string = S_common_edit_1_to_64_values[value - pr->lower_bound];
    }
    else if ( (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_FEEDBACK)       || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_AMPLITUDE)      || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_SUSTAIN)        || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_LFO_DELAY)          || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_VIBRATO_BASE)       || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_BASE)       || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_VELOCITY_SCALING)   || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_MOD_WHEEL) || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_AFTERTOUCH))
    {
      value_string = S_common_edit_0_to_64_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_CART_NAME)
      value_string = &G_cart_names[cart_index][0];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PATCH_NAME)
      value_string = &G_patch_names[patch_index][0];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ALGORITHM)
      value_string = S_patch_edit_algorithm_values[value - pr->lower_bound];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ALG_NAME)
      value_string = S_patch_edit_alg_name_values[value - pr->lower_bound];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_WAVEFORM)
      value_string = S_patch_edit_osc_waveform_values[value - pr->lower_bound];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_PHI)
      value_string = S_patch_edit_osc_phi_values[value - pr->lower_bound];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_FREQ_MODE)
      value_string = S_patch_edit_osc_freq_mode_values[value - pr->lower_bound];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_OSC_DETUNE)
      value_string = S_patch_edit_osc_detune_values[value - pr->lower_bound];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ENV_BREAK_POINT)
      value_string = S_patch_edit_env_break_point_values[value - pr->lower_bound];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PORTAMENTO_MODE)
      value_string = S_patch_edit_portamento_mode_values[value - pr->lower_bound];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PORTAMENTO_FOLLOW)
      value_string = S_patch_edit_portamento_follow_values[value - pr->lower_bound];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PORTAMENTO_LEGATO)
      value_string = S_patch_edit_portamento_legato_values[value - pr->lower_bound];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ARPEGGIO_MODE)
      value_string = S_patch_edit_arpeggio_mode_values[value - pr->lower_bound];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_ARPEGGIO_PATTERN)
      value_string = S_patch_edit_arpeggio_pattern_values[value - pr->lower_bound];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PEG_LEGATO)
      value_string = S_patch_edit_peg_legato_values[value - pr->lower_bound];
    else if ( (pr->label == LAYOUT_CART_PARAM_LABEL_PEG_AMPLITUDE)  || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_PEG_SUSTAIN)    || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_PEG_FINALE))
    {
      value_string = S_patch_edit_peg_level_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PEG_TRANSPOSE)
      value_string = S_patch_edit_peg_transpose_values[value - pr->lower_bound];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_LFO_WAVEFORM)
      value_string = S_patch_edit_lfo_waveform_values[value - pr->lower_bound];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_VIBRATO_MODE)
      value_string = S_patch_edit_vibrato_mode_values[value - pr->lower_bound];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_TREMOLO_MODE)
      value_string = S_patch_edit_tremolo_mode_values[value - pr->lower_bound];
    else if ( (pr->label == LAYOUT_CART_PARAM_LABEL_MOD_WHEEL_EFFECT)   || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_AFTERTOUCH_EFFECT)  || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_EXP_PEDAL_EFFECT))
    {
      value_string = S_patch_edit_controller_effect_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PEDAL_ADJUST)
      value_string = S_patch_edit_pedal_adjust_values[value - pr->lower_bound];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_VELOCITY_MODE)
      value_string = S_patch_edit_velocity_mode_values[value - pr->lower_bound];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_NOISE_MODE)
      value_string = S_patch_edit_noise_mode_values[value - pr->lower_bound];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_BOOST_MODE)
      value_string = S_patch_edit_boost_mode_values[value - pr->lower_bound];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_PITCH_WHEEL_MODE)
      value_string = S_patch_edit_pitch_wheel_mode_values[value - pr->lower_bound];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_LEGACY_MULTIPLE)
      value_string = S_patch_edit_legacy_multiple_values[value - pr->lower_bound];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_LEGACY_KEYSCALE)
      value_string = S_patch_edit_legacy_keyscale_values[value - pr->lower_bound];
    else if ( (pr->label == LAYOUT_CART_PARAM_LABEL_SYNC_OSC) || 
              (pr->label == LAYOUT_CART_PARAM_LABEL_SYNC_LFO))
    {
      value_string = S_patch_edit_sync_values[value - pr->lower_bound];
    }
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_HIGHPASS_CUTOFF)
      value_string = S_patch_edit_highpass_cutoff_values[value - pr->lower_bound];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_LOWPASS_CUTOFF)
      value_string = S_patch_edit_lowpass_cutoff_values[value - pr->lower_bound];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_VELOCITY)
      value_string = S_patch_edit_audition_note_velocity_values[value - pr->lower_bound];
    else if (pr->label == LAYOUT_CART_PARAM_LABEL_AUDITION_PITCH_WHEEL)
      value_string = S_patch_edit_audition_pitch_wheel_values[value - pr->lower_bound];
    else
      value_string = NULL;

    /* load the parameter name, value, and slider, arrows, or radio button */
    if (pr->type == LAYOUT_PARAM_TYPE_SLIDER)
    {
      vb_all_load_text( pr->center_x + LAYOUT_CART_PARAM_NAME_X, pos_y, 
                        VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_2, VB_ALL_PARAM_MAX_TEXT_SIZE, 
                        S_cart_param_labels[pr->label]);

      vb_all_load_text( pr->center_x + LAYOUT_CART_PARAM_SLIDER_VALUE_X, pos_y, 
                        VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_GRAY, VB_ALL_PARAM_MAX_TEXT_SIZE, 
                        value_string);

      vb_all_load_slider( pr->center_x + LAYOUT_CART_PARAM_SLIDER_TRACK_X, pos_y, 
                          LAYOUT_CART_PARAM_SLIDER_WIDTH, 
                          value, pr->lower_bound, pr->upper_bound);
    }
    else if (pr->type == LAYOUT_PARAM_TYPE_ARROWS)
    {
      vb_all_load_text( pr->center_x + LAYOUT_CART_PARAM_NAME_X, pos_y, 
                        VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_2, VB_ALL_PARAM_MAX_TEXT_SIZE, 
                        S_cart_param_labels[pr->label]);

      vb_all_load_text( pr->center_x + LAYOUT_CART_PARAM_ARROWS_VALUE_X, pos_y, 
                        VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_GRAY, VB_ALL_PARAM_MAX_TEXT_SIZE, 
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
      vb_all_load_text( pr->center_x + LAYOUT_CART_PARAM_NAME_X, pos_y, 
                        VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_2, VB_ALL_PARAM_MAX_TEXT_SIZE, 
                        S_cart_param_labels[pr->label]);

      vb_all_load_text( pr->center_x + LAYOUT_CART_PARAM_RADIO_VALUE_X, pos_y, 
                        VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_GRAY, VB_ALL_PARAM_MAX_TEXT_SIZE, 
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
    else if (pr->type == LAYOUT_PARAM_TYPE_NAME)
    {
      vb_all_load_text( pr->center_x, pos_y, 
                        VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_GRAY, VB_ALL_PARAM_MAX_TEXT_SIZE, 
                        value_string);
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

