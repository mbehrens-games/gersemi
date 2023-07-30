/*******************************************************************************
** vball.c (vbo population functions - all)
*******************************************************************************/

#include <glad/glad.h>

#include <stdio.h>
#include <stdlib.h>

#include "controls.h"
#include "global.h"
#include "graphics.h"
#include "layout.h"
#include "palette.h"
#include "patch.h"
#include "screen.h"
#include "texture.h"
#include "vball.h"

#define VB_ALL_BACKGROUND_TILE_SIZE       16
#define VB_ALL_BACKGROUND_TILE_SIZE_HALF  (VB_ALL_BACKGROUND_TILE_SIZE / 2)

#define VB_ALL_BACKGROUND_WIDTH   (GRAPHICS_OVERSCAN_WIDTH / VB_ALL_BACKGROUND_TILE_SIZE)
#define VB_ALL_BACKGROUND_HEIGHT  (GRAPHICS_OVERSCAN_HEIGHT / VB_ALL_BACKGROUND_TILE_SIZE)

#define VB_ALL_PALETTE_1  0
#define VB_ALL_PALETTE_2  1
#define VB_ALL_PALETTE_3  4

enum
{
  VB_ALL_ALIGN_LEFT = 0,
  VB_ALL_ALIGN_CENTER,
  VB_ALL_ALIGN_RIGHT
};

enum
{
  VB_ALL_SPRITE_NAME_ADJUST_PARAM_LEFT = 0,
  VB_ALL_SPRITE_NAME_ADJUST_PARAM_RIGHT
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

#define VB_ALL_ADD_PANEL_PIECE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette)  \
  if (tile_index < GRAPHICS_PANELS_TILES_END_INDEX)                                         \
  {                                                                                         \
    VB_ALL_ADD_TILE_TO_VERTEX_BUFFER(pos_x, pos_y, 1, 1, GRAPHICS_Z_LEVEL_PANELS)           \
    VB_ALL_ADD_TILE_TO_TEXTURE_COORD_BUFFER(cell_x, cell_y, 1, 1)                           \
    VB_ALL_ADD_TILE_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)                       \
    VB_ALL_ADD_TILE_TO_ELEMENT_BUFFER()                                                     \
                                                                                            \
    tile_index += 1;                                                                        \
  }

#define VB_ALL_ADD_SCROLLBAR_TRACK_PIECE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette)  \
  if (tile_index < GRAPHICS_PANELS_TILES_END_INDEX)                                                   \
  {                                                                                                   \
    VB_ALL_ADD_TILE_TO_VERTEX_BUFFER(pos_x, pos_y, 1, 1, GRAPHICS_Z_LEVEL_PANELS)                     \
    VB_ALL_ADD_TILE_TO_TEXTURE_COORD_BUFFER(cell_x, cell_y, 1, 1)                                     \
    VB_ALL_ADD_TILE_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)                                 \
    VB_ALL_ADD_TILE_TO_ELEMENT_BUFFER()                                                               \
                                                                                                      \
    tile_index += 1;                                                                                  \
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

#define VB_ALL_UPDATE_PANELS_TILES_IN_VBOS()                                                        \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_tiles);                                          \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                  \
                  GRAPHICS_PANELS_TILES_START_INDEX * 12 * sizeof(GLfloat),                         \
                  G_tile_layer_counts[GRAPHICS_TILE_LAYER_PANELS] * 12 * sizeof(GLfloat),           \
                  &G_vertex_buffer_tiles[GRAPHICS_PANELS_TILES_START_INDEX * 12]);                  \
                                                                                                    \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_tiles);                                   \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                  \
                  GRAPHICS_PANELS_TILES_START_INDEX * 8 * sizeof(GLfloat),                          \
                  G_tile_layer_counts[GRAPHICS_TILE_LAYER_PANELS] * 8 * sizeof(GLfloat),            \
                  &G_texture_coord_buffer_tiles[GRAPHICS_PANELS_TILES_START_INDEX * 8]);            \
                                                                                                    \
  glBindBuffer(GL_ARRAY_BUFFER, G_lighting_and_palette_buffer_id_tiles);                            \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                  \
                  GRAPHICS_PANELS_TILES_START_INDEX * 8 * sizeof(GLfloat),                          \
                  G_tile_layer_counts[GRAPHICS_TILE_LAYER_PANELS] * 8 * sizeof(GLfloat),            \
                  &G_lighting_and_palette_buffer_tiles[GRAPHICS_PANELS_TILES_START_INDEX * 8]);     \
                                                                                                    \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_tiles);                                   \
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,                                                          \
                  GRAPHICS_PANELS_TILES_START_INDEX * 6 * sizeof(unsigned short),                   \
                  G_tile_layer_counts[GRAPHICS_TILE_LAYER_PANELS] * 6 * sizeof(unsigned short),     \
                  &G_index_buffer_tiles[GRAPHICS_PANELS_TILES_START_INDEX * 6]);

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

#define VB_ALL_ADD_BUTTON_PIECE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette) \
  if (sprite_index < GRAPHICS_BUTTONS_SPRITES_END_INDEX)                                    \
  {                                                                                         \
    VB_ALL_ADD_SPRITE_TO_VERTEX_BUFFER(pos_x, pos_y, 1, 2, GRAPHICS_Z_LEVEL_BUTTONS)        \
    VB_ALL_ADD_SPRITE_TO_TEXTURE_COORD_BUFFER(cell_x, cell_y, 1, 2)                         \
    VB_ALL_ADD_SPRITE_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)                     \
    VB_ALL_ADD_SPRITE_TO_ELEMENT_BUFFER()                                                   \
                                                                                            \
    sprite_index += 1;                                                                      \
  }

#define VB_ALL_ADD_SLIDER_TRACK_PIECE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette) \
  if (sprite_index < GRAPHICS_OVERLAY_SPRITES_END_INDEX)                                          \
  {                                                                                               \
    VB_ALL_ADD_SPRITE_TO_VERTEX_BUFFER(pos_x, pos_y, 1, 1, GRAPHICS_Z_LEVEL_SLIDERS)              \
    VB_ALL_ADD_SPRITE_TO_TEXTURE_COORD_BUFFER(cell_x, cell_y, 1, 1)                               \
    VB_ALL_ADD_SPRITE_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)                           \
    VB_ALL_ADD_SPRITE_TO_ELEMENT_BUFFER()                                                         \
                                                                                                  \
    sprite_index += 1;                                                                            \
  }

#define VB_ALL_ADD_SCROLLBAR_MOVING_PART_TO_BUFFERS(pos_x, pos_y, lighting, palette)  \
  if (sprite_index < GRAPHICS_OVERLAY_SPRITES_END_INDEX)                              \
  {                                                                                   \
    VB_ALL_ADD_SPRITE_TO_VERTEX_BUFFER(pos_x, pos_y, 1, 2, GRAPHICS_Z_LEVEL_OVERLAY)  \
    VB_ALL_ADD_SPRITE_TO_TEXTURE_COORD_BUFFER(14, 8, 1, 2)                            \
    VB_ALL_ADD_SPRITE_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)               \
    VB_ALL_ADD_SPRITE_TO_ELEMENT_BUFFER()                                             \
                                                                                      \
    sprite_index += 1;                                                                \
  }

#define VB_ALL_ADD_FONT_CHARACTER_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette) \
  if (sprite_index < GRAPHICS_OVERLAY_SPRITES_END_INDEX)                                      \
  {                                                                                           \
    VB_ALL_ADD_SPRITE_TO_VERTEX_BUFFER(pos_x, pos_y, 1, 1, GRAPHICS_Z_LEVEL_OVERLAY)          \
    VB_ALL_ADD_SPRITE_TO_TEXTURE_COORD_BUFFER(cell_x, cell_y, 1, 1)                           \
    VB_ALL_ADD_SPRITE_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)                       \
    VB_ALL_ADD_SPRITE_TO_ELEMENT_BUFFER()                                                     \
                                                                                              \
    sprite_index += 1;                                                                        \
  }

#define VB_ALL_ADD_SLIDER_MOVING_PART_TO_BUFFERS(pos_x, pos_y, lighting, palette)     \
  if (sprite_index < GRAPHICS_OVERLAY_SPRITES_END_INDEX)                              \
  {                                                                                   \
    VB_ALL_ADD_SPRITE_TO_VERTEX_BUFFER(pos_x, pos_y, 1, 1, GRAPHICS_Z_LEVEL_OVERLAY)  \
    VB_ALL_ADD_SPRITE_TO_TEXTURE_COORD_BUFFER(7, 7, 1, 1)                             \
    VB_ALL_ADD_SPRITE_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)               \
    VB_ALL_ADD_SPRITE_TO_ELEMENT_BUFFER()                                             \
                                                                                      \
    sprite_index += 1;                                                                \
  }

#define VB_ALL_ADD_NAMED_SPRITE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, width, height, lighting, palette)  \
  if (sprite_index < GRAPHICS_OVERLAY_SPRITES_END_INDEX)                                                    \
  {                                                                                                         \
    VB_ALL_ADD_SPRITE_TO_VERTEX_BUFFER(pos_x, pos_y, width, height, GRAPHICS_Z_LEVEL_OVERLAY)               \
    VB_ALL_ADD_SPRITE_TO_TEXTURE_COORD_BUFFER(cell_x, cell_y, width, height)                                \
    VB_ALL_ADD_SPRITE_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)                                     \
    VB_ALL_ADD_SPRITE_TO_ELEMENT_BUFFER()                                                                   \
                                                                                                            \
    sprite_index += 1;                                                                                      \
  }

#define VB_ALL_UPDATE_BUTTONS_SPRITES_IN_VBOS()                                                       \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_sprites);                                          \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                    \
                  GRAPHICS_BUTTONS_SPRITES_START_INDEX * 12 * sizeof(GLfloat),                        \
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_BUTTONS] * 12 * sizeof(GLfloat),        \
                  &G_vertex_buffer_sprites[GRAPHICS_BUTTONS_SPRITES_START_INDEX * 12]);               \
                                                                                                      \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_sprites);                                   \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                    \
                  GRAPHICS_BUTTONS_SPRITES_START_INDEX * 8 * sizeof(GLfloat),                         \
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_BUTTONS] * 8 * sizeof(GLfloat),         \
                  &G_texture_coord_buffer_sprites[GRAPHICS_BUTTONS_SPRITES_START_INDEX * 8]);         \
                                                                                                      \
  glBindBuffer(GL_ARRAY_BUFFER, G_lighting_and_palette_buffer_id_sprites);                            \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                    \
                  GRAPHICS_BUTTONS_SPRITES_START_INDEX * 8 * sizeof(GLfloat),                         \
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_BUTTONS] * 8 * sizeof(GLfloat),         \
                  &G_lighting_and_palette_buffer_sprites[GRAPHICS_BUTTONS_SPRITES_START_INDEX * 8]);  \
                                                                                                      \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_sprites);                                   \
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,                                                            \
                  GRAPHICS_BUTTONS_SPRITES_START_INDEX * 6 * sizeof(unsigned short),                  \
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_BUTTONS] * 6 * sizeof(unsigned short),  \
                  &G_index_buffer_sprites[GRAPHICS_BUTTONS_SPRITES_START_INDEX * 6]);

#define VB_ALL_UPDATE_OVERLAY_SPRITES_IN_VBOS()                                                       \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_sprites);                                          \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                    \
                  GRAPHICS_OVERLAY_SPRITES_START_INDEX * 12 * sizeof(GLfloat),                        \
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_OVERLAY] * 12 * sizeof(GLfloat),        \
                  &G_vertex_buffer_sprites[GRAPHICS_OVERLAY_SPRITES_START_INDEX * 12]);               \
                                                                                                      \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_sprites);                                   \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                    \
                  GRAPHICS_OVERLAY_SPRITES_START_INDEX * 8 * sizeof(GLfloat),                         \
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_OVERLAY] * 8 * sizeof(GLfloat),         \
                  &G_texture_coord_buffer_sprites[GRAPHICS_OVERLAY_SPRITES_START_INDEX * 8]);         \
                                                                                                      \
  glBindBuffer(GL_ARRAY_BUFFER, G_lighting_and_palette_buffer_id_sprites);                            \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                    \
                  GRAPHICS_OVERLAY_SPRITES_START_INDEX * 8 * sizeof(GLfloat),                         \
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_OVERLAY] * 8 * sizeof(GLfloat),         \
                  &G_lighting_and_palette_buffer_sprites[GRAPHICS_OVERLAY_SPRITES_START_INDEX * 8]);  \
                                                                                                      \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_sprites);                                   \
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,                                                            \
                  GRAPHICS_OVERLAY_SPRITES_START_INDEX * 6 * sizeof(unsigned short),                  \
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_OVERLAY] * 6 * sizeof(unsigned short),  \
                  &G_index_buffer_sprites[GRAPHICS_OVERLAY_SPRITES_START_INDEX * 6]);

static char S_patch_edit_header_labels[LAYOUT_HEADER_PATCH_EDIT_NUM_LABELS][12] = 
  { "Osc 1", "Osc 2", "Osc 3", "Osc 4", 
    "Env 1", "Env 2", "Env 3", "Env 4", 
    "LFO", 
    "Portamento", "Filters", "Noise", 
    "Depths", "Mod Wheel", "Aftertouch" 
  };

static char S_patch_edit_parameter_labels[LAYOUT_PARAM_PATCH_EDIT_NUM_LABELS][4] = 
  { "Alg", 
    "Wav", "FBk", "Syn", "Frq", "Mul", "Div", "Oct", "Nte", "Det", 
    "Att", "D1", "D2", "Rel", "Lev", "Sus", "RKS", "LKS", "Rep", 
    "Vib", "Tre", "Bst", 
    "Wav", "Frq", "Syn", "Dly", "Vib", "Tre", 
    "Mde", "Spd", 
    "HP", "LP", 
    "Mix", "Frq", 
    "Vib", "Tre", "Bst", 
    "Vib", "Tre", "Bst", 
    "Vib", "Tre", "Bst" 
  };

static char S_patch_edit_algorithm_values[PATCH_ALGORITHM_NUM_VALUES][4] = 
  { "1", "2", "3", "4", "5", "6", "7", "8" };

/*
static char S_patch_edit_algorithm_values[PATCH_ALGORITHM_NUM_VALUES][12] = 
  { "1C Chain", 
    "1C The Y", 
    "1C Crab", 
    "2C Twin",
    "2C Stacked",
    "3C 1 to 3",
    "3C 1 to 1",
    "4C Pipes"
  };
*/

static char S_patch_edit_osc_waveform_values[PATCH_OSC_WAVEFORM_NUM_VALUES][8] = 
  { "Sine", "Half", "Full", "Quar", "Alt", "Cam", "Squa", "LSaw" };

static char S_patch_edit_osc_feedback_values[PATCH_OSC_FEEDBACK_NUM_VALUES][4] = 
  { "0", "1", "2", "3", "4", "5", "6", "7" };

static char S_patch_edit_osc_sync_values[PATCH_OSC_SYNC_NUM_VALUES][8] = 
  { "Off", "0", "90", "180", "270" };

static char S_patch_edit_osc_freq_mode_values[PATCH_OSC_FREQ_MODE_NUM_VALUES][8] = 
  { "Ratio", "Fixed" };

static char S_patch_edit_osc_octave_values[PATCH_OSC_OCTAVE_NUM_VALUES][4] = 
  { "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7", "8", "9" };

static char S_patch_edit_osc_note_values[PATCH_OSC_NOTE_NUM_VALUES][4] = 
  { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

static char S_patch_edit_osc_detune_values[PATCH_OSC_DETUNE_NUM_VALUES][4] = 
  { "-16", "-15", "-14", "-13", "-12", "-11", "-10",  "-9", 
     "-8",  "-7",  "-6",  "-5",  "-4",  "-3",  "-2",  "-1", 
      "0", 
      "1",   "2",   "3",   "4",   "5",   "6",   "7",   "8", 
      "9",  "10",  "11",  "12",  "13",  "14",  "15",  "16" 
  };

static char S_patch_edit_env_trigger_values[PATCH_ENV_TRIGGER_NUM_VALUES][8] = 
  { "F 1", "B 1", "F R", "B R", "F/B", "B/F", "D/U", "U/D" };

static char S_patch_edit_mod_enable_values[PATCH_MOD_ENABLE_NUM_VALUES][8] = 
  { "Off", "On" };

static char S_patch_edit_lfo_waveform_values[PATCH_LFO_WAVEFORM_NUM_VALUES][8] = 
  { "Tri", "Squa",  "Saw U",  "Saw D", "Noise" };

static char S_patch_edit_lfo_sync_values[PATCH_LFO_SYNC_NUM_VALUES][4] = 
  { "Off", "On" };

static char S_patch_edit_portamento_mode_values[PATCH_PORTAMENTO_MODE_NUM_VALUES][8] = 
  { "Port", "Glis" };

static char S_patch_edit_highpass_cutoff_values[PATCH_HIGHPASS_CUTOFF_NUM_VALUES][4] = 
  { "A0", "A1", "A2", "A3" };

static char S_patch_edit_lowpass_cutoff_values[PATCH_LOWPASS_CUTOFF_NUM_VALUES][4] = 
  { "E7", "G7", "A7", "C8" };

static char S_patch_edit_1_to_32_values[32][4] = 
  { "1",  "2",  "3",  "4",  "5",  "6",  "7", "8", 
    "9", "10", "11", "12", "13", "14", "15", "16", 
   "17", "18", "19", "20", "21", "22", "23", "24", 
   "25", "26", "27", "28", "29", "30", "31", "32" 
  };

static char S_patch_edit_0_to_32_values[33][4] = 
  { "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7", "8", 
          "9", "10", "11", "12", "13", "14", "15", "16", 
         "17", "18", "19", "20", "21", "22", "23", "24", 
         "25", "26", "27", "28", "29", "30", "31", "32" 
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

  int tile_index;

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
  tile_index =  GRAPHICS_PANELS_TILES_START_INDEX + 
                G_tile_layer_counts[GRAPHICS_TILE_LAYER_PANELS];

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

      /* select panel type */
      if (type == LAYOUT_PANEL_TYPE_NORMAL)
        cell_x += 0;
      else if (type == LAYOUT_PANEL_TYPE_THIN)
        cell_x += 4;
      else
        cell_x += 0;

      VB_ALL_ADD_PANEL_PIECE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette)
    }
  }

  /* update panels tile layer count */
  G_tile_layer_counts[GRAPHICS_TILE_LAYER_PANELS] = 
    tile_index - GRAPHICS_PANELS_TILES_START_INDEX;

  return 0;
}

/*******************************************************************************
** vb_all_load_vertical_scrollbar_track()
*******************************************************************************/
short int vb_all_load_vertical_scrollbar_track( int offset_x, int offset_y, 
                                                int height)
{
  int n;

  int tile_index;

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
  tile_index =  GRAPHICS_PANELS_TILES_START_INDEX + 
                G_tile_layer_counts[GRAPHICS_TILE_LAYER_PANELS];

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
  G_tile_layer_counts[GRAPHICS_TILE_LAYER_PANELS] = 
    tile_index - GRAPHICS_PANELS_TILES_START_INDEX;

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
  sprite_index =  GRAPHICS_BUTTONS_SPRITES_START_INDEX + 
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_BUTTONS];

  for (m = 0; m < width; m++)
  {
    /* determine center of this piece */
    pos_x = corner_x + (8 * m) + 4;
    pos_y = corner_y + (16 * 0) + 8;

    /* left piece */
    if (m == 0)
    {
      cell_x = 0;
      cell_y = 8;
    }
    /* right piece */
    else if (m == width - 1)
    {
      cell_x = 3;
      cell_y = 8;
    }
    /* middle piece */
    else
    {
      cell_x = 1;
      cell_y = 8;
    }

    VB_ALL_ADD_BUTTON_PIECE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette)
  }

  /* update panels sprite layer count */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_BUTTONS] = 
    sprite_index - GRAPHICS_BUTTONS_SPRITES_START_INDEX;

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
  sprite_index =  GRAPHICS_OVERLAY_SPRITES_START_INDEX + 
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_OVERLAY];

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
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_OVERLAY] = 
    sprite_index - GRAPHICS_OVERLAY_SPRITES_START_INDEX;

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
  sprite_index =  GRAPHICS_OVERLAY_SPRITES_START_INDEX + 
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_OVERLAY];

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
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_OVERLAY] = 
    sprite_index - GRAPHICS_OVERLAY_SPRITES_START_INDEX;

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
  if (name == VB_ALL_SPRITE_NAME_ADJUST_PARAM_LEFT)
  {
    cell_x = 4;
    cell_y = 6;
    width = 1;
    height = 1;
  }
  else if (name == VB_ALL_SPRITE_NAME_ADJUST_PARAM_RIGHT)
  {
    cell_x = 5;
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
  sprite_index =  GRAPHICS_OVERLAY_SPRITES_START_INDEX + 
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_OVERLAY];

  VB_ALL_ADD_NAMED_SPRITE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, width, height, lighting, palette)

  /* update overlay sprite layer count */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_OVERLAY] = 
    sprite_index - GRAPHICS_OVERLAY_SPRITES_START_INDEX;

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
  sprite_index =  GRAPHICS_OVERLAY_SPRITES_START_INDEX + 
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_OVERLAY];

  for (m = 0; m < width; m++)
  {
    /* determine center of this piece */
    pos_x = corner_x + (8 * m) + 4;
    pos_y = corner_y + (8 * 0) + 4;

    /* left piece */
    if (m == 0)
    {
      cell_x = 8;
      cell_y = 7;
    }
    /* right piece */
    else if (m == width - 1)
    {
      cell_x = 11;
      cell_y = 7;
    }
    /* middle piece */
    else
    {
      cell_x = 9;
      cell_y = 7;
    }

    VB_ALL_ADD_SLIDER_TRACK_PIECE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette)
  }

  /* draw the slider itself */
  pos_x = corner_x + (8 * 0) + 4;
  pos_y = corner_y + (8 * 0) + 4;

  pos_x += ((value - lower_bound) * 8 * (width - 1)) / (upper_bound - lower_bound);

  lighting = 0;
  palette = VB_ALL_PALETTE_1;

  VB_ALL_ADD_SLIDER_MOVING_PART_TO_BUFFERS(pos_x, pos_y, lighting, palette)

  /* update overlay sprite layer count */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_OVERLAY] = 
    sprite_index - GRAPHICS_OVERLAY_SPRITES_START_INDEX;

  return 0;
}

/*******************************************************************************
** vb_all_load_common_panels_and_buttons()
*******************************************************************************/
short int vb_all_load_common_panels_and_buttons()
{
  int k;

  button* b;

  /* reset panels tile vbo count */
  G_tile_layer_counts[GRAPHICS_TILE_LAYER_PANELS] = 0;

  /* reset sprite vbo counts */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_BUTTONS] = 0;
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_OVERLAY] = 0;

  /* panels */
  vb_all_load_panel(LAYOUT_PANEL_TOP_X, LAYOUT_PANEL_TOP_Y, 
                    LAYOUT_PANEL_TOP_WIDTH, LAYOUT_PANEL_TOP_HEIGHT, 
                    LAYOUT_PANEL_TYPE_NORMAL);

  vb_all_load_panel(LAYOUT_PANEL_BOTTOM_X, LAYOUT_PANEL_BOTTOM_Y, 
                    LAYOUT_PANEL_BOTTOM_WIDTH, LAYOUT_PANEL_BOTTOM_HEIGHT, 
                    LAYOUT_PANEL_TYPE_THIN);

  /* vertical scrollbar track */
  vb_all_load_vertical_scrollbar_track( LAYOUT_SCROLLBAR_X, 
                                        LAYOUT_SCROLLBAR_Y, 
                                        LAYOUT_SCROLLBAR_HEIGHT);

  /* top panel buttons */
  for ( k = LAYOUT_TOP_PANEL_BUTTONS_START_INDEX; 
        k < LAYOUT_TOP_PANEL_BUTTONS_END_INDEX; 
        k++)
  {
    b = &G_layout_buttons[k];

    vb_all_load_button( b->center_x, b->center_y, 
                        b->width, b->state);
  }

  /* update vbos */
  VB_ALL_UPDATE_PANELS_TILES_IN_VBOS()
  VB_ALL_UPDATE_BUTTONS_SPRITES_IN_VBOS()

  return 0;
}

/*******************************************************************************
** vb_all_load_common_overlay()
*******************************************************************************/
short int vb_all_load_common_overlay()
{
  int k;

  button* b;
  header* hd;

  /* reset overlay sprite vbo count */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_OVERLAY] = 0;

  /* vertical scrollbar slider */
  vb_all_load_vertical_scrollbar_moving_part( LAYOUT_SCROLLBAR_X, 
                                              LAYOUT_SCROLLBAR_Y, 
                                              LAYOUT_SCROLLBAR_HEIGHT, 
                                              G_current_scroll_amount, 
                                              G_max_scroll_amount);

  /* top panel headers */
  for ( k = LAYOUT_TOP_PANEL_HEADERS_START_INDEX; 
        k < LAYOUT_TOP_PANEL_HEADERS_END_INDEX; 
        k++)
  {
    hd = &G_layout_headers[k];

    if (hd->label == LAYOUT_HEADER_TOP_PANEL_LABEL_NAME)
    {
      vb_all_load_text( hd->center_x, hd->center_y, 
                        VB_ALL_ALIGN_CENTER, 0, 6, 32, "Gersemi");
    }
    else if (hd->label == LAYOUT_HEADER_TOP_PANEL_LABEL_VERSION)
    {
      vb_all_load_text( hd->center_x, hd->center_y, 
                        VB_ALL_ALIGN_CENTER, 0, 6, 32, "v0.9");
    }
  }

  /* bottom panel headers */
  for ( k = LAYOUT_BOTTOM_PANEL_HEADERS_START_INDEX; 
        k < LAYOUT_BOTTOM_PANEL_HEADERS_END_INDEX; 
        k++)
  {
    hd = &G_layout_headers[k];

    if (hd->label == LAYOUT_HEADER_BOTTOM_PANEL_LABEL_OCTAVE)
    {
      vb_all_load_text( hd->center_x, hd->center_y, 
                        VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_1, 32, "Octave");
    }
    else if (hd->label == LAYOUT_HEADER_BOTTOM_PANEL_LABEL_KEY)
    {
      vb_all_load_text( hd->center_x, hd->center_y, 
                        VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_1, 32, "Key");
    }
  }

  /* bottom panel text (extra) */
  /*vb_all_load_value(-31, 25, 0, 6, G_patch_edit_octave);*/

  vb_all_load_text(9, 25, VB_ALL_ALIGN_LEFT, 0, 6, 32, "C  D  E  F  G  A  Bb");

  /* top panel buttons */
  for ( k = LAYOUT_TOP_PANEL_BUTTONS_START_INDEX; 
        k < LAYOUT_TOP_PANEL_BUTTONS_END_INDEX; 
        k++)
  {
    b = &G_layout_buttons[k];

    if (b->label == LAYOUT_BUTTON_LABEL_PATCHES)
    {
      vb_all_load_text( b->center_x, b->center_y, 
                        VB_ALL_ALIGN_CENTER, 0, 6, 16, "Patches");
    }
    else if (b->label == LAYOUT_BUTTON_LABEL_PATTERNS)
    {
      vb_all_load_text( b->center_x, b->center_y, 
                        VB_ALL_ALIGN_CENTER, 0, 6, 16, "Patterns");
    }
  }

  /* update vbos */
  VB_ALL_UPDATE_OVERLAY_SPRITES_IN_VBOS()

  return 0;
}

/*******************************************************************************
** vb_all_load_patches_overlay()
*******************************************************************************/
short int vb_all_load_patches_overlay()
{
  int k;

  patch* p;

  header* hd;
  param*  pr;

  short int value;
  char*     value_string;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(0))
    return 1;

  /* obtain patch pointer */
  p = &G_patch_bank[0];

  /* patch settings text */
  if (G_current_scroll_amount < 3)
  {
    vb_all_load_text( LAYOUT_PATCH_EDIT_PATCH_NUMBER_X, 
                      LAYOUT_PATCH_EDIT_PATCH_NUMBER_Y - G_current_scroll_amount, 
                      VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Patch");
  }

  /* headers */
  for ( k = LAYOUT_PATCH_EDIT_HEADERS_START_INDEX; 
        k < LAYOUT_PATCH_EDIT_HEADERS_END_INDEX; 
        k++)
  {
    hd = &G_layout_headers[k];

    /* make sure this header is within the viewable area */
    if (LAYOUT_PATCH_HEADER_IS_NOT_IN_MAIN_AREA(hd))
      continue;

    /* make sure the label is valid */
    if ((hd->label < 0) || (hd->label >= LAYOUT_HEADER_PATCH_EDIT_NUM_LABELS))
      continue;

    /* load the header! */
    vb_all_load_text( hd->center_x, hd->center_y - G_current_scroll_amount, 
                      VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_3, 16, 
                      S_patch_edit_header_labels[hd->label]);
  }

  /* parameters */
  for ( k = LAYOUT_PATCH_EDIT_PARAMS_START_INDEX; 
        k < LAYOUT_PATCH_EDIT_PARAMS_END_INDEX; 
        k++)
  {
    pr = &G_layout_params[k];

    /* make sure this parameter is within the viewable area */
    if (LAYOUT_PATCH_PARAM_IS_NOT_IN_MAIN_AREA(pr))
      continue;

    /* make sure the label is valid */
    if ((pr->label < 0) || (pr->label >= LAYOUT_PARAM_PATCH_EDIT_NUM_LABELS))
      continue;

    /* skip multiple/divisor or octave/note depending on the frequency mode */
    if (p->osc_freq_mode[pr->num] == 0)
    {
      if ((pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_OCTAVE) || 
          (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_NOTE))
      {
        continue;
      }
    }
    else if (p->osc_freq_mode[pr->num] == 1)
    {
      if ((pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_MULTIPLE) || 
          (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_DIVISOR))
      {
        continue;
      }
    }

    /* determine parameter value and string */
    if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_ALGORITHM)
    {
      value = p->algorithm;
      value_string = S_patch_edit_algorithm_values[p->algorithm - PATCH_ALGORITHM_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_WAVEFORM)
    {
      value = p->osc_waveform[pr->num];
      value_string = S_patch_edit_osc_waveform_values[p->osc_waveform[pr->num] - PATCH_OSC_WAVEFORM_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_FEEDBACK)
    {
      value = p->osc_feedback[pr->num];
      value_string = S_patch_edit_osc_feedback_values[p->osc_feedback[pr->num] - PATCH_OSC_FEEDBACK_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_SYNC)
    {
      value = p->osc_sync[pr->num];
      value_string = S_patch_edit_osc_sync_values[p->osc_sync[pr->num] - PATCH_OSC_SYNC_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_FREQ_MODE)
    {
      value = p->osc_freq_mode[pr->num];
      value_string = S_patch_edit_osc_freq_mode_values[p->osc_freq_mode[pr->num] - PATCH_OSC_FREQ_MODE_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_MULTIPLE)
    {
      value = p->osc_multiple_or_octave[pr->num];
      value_string = S_patch_edit_1_to_32_values[p->osc_multiple_or_octave[pr->num] - PATCH_OSC_MULTIPLE_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_DIVISOR)
    {
      value = p->osc_divisor_or_note[pr->num];
      value_string = S_patch_edit_1_to_32_values[p->osc_divisor_or_note[pr->num] - PATCH_OSC_DIVISOR_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_OCTAVE)
    {
      value = p->osc_multiple_or_octave[pr->num];
      value_string = S_patch_edit_osc_octave_values[p->osc_multiple_or_octave[pr->num] - PATCH_OSC_OCTAVE_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_NOTE)
    {
      value = p->osc_divisor_or_note[pr->num];
      value_string = S_patch_edit_osc_note_values[p->osc_divisor_or_note[pr->num] - PATCH_OSC_NOTE_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_OSC_DETUNE)
    {
      value = p->osc_detune[pr->num];
      value_string = S_patch_edit_osc_detune_values[p->osc_detune[pr->num] - PATCH_OSC_DETUNE_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_ATTACK)
    {
      value = p->env_attack[pr->num];
      value_string = S_patch_edit_1_to_32_values[p->env_attack[pr->num] - PATCH_ENV_RATE_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_DECAY_1)
    {
      value = p->env_decay_1[pr->num];
      value_string = S_patch_edit_1_to_32_values[p->env_decay_1[pr->num] - PATCH_ENV_RATE_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_DECAY_2)
    {
      value = p->env_decay_2[pr->num];
      value_string = S_patch_edit_1_to_32_values[p->env_decay_2[pr->num] - PATCH_ENV_RATE_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_RELEASE)
    {
      value = p->env_release[pr->num];
      value_string = S_patch_edit_1_to_32_values[p->env_release[pr->num] - PATCH_ENV_RATE_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_AMPLITUDE)
    {
      value = p->env_amplitude[pr->num];
      value_string = S_patch_edit_0_to_32_values[p->env_amplitude[pr->num] - PATCH_ENV_AMPLITUDE_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_SUSTAIN)
    {
      value = p->env_sustain[pr->num];
      value_string = S_patch_edit_0_to_32_values[p->env_sustain[pr->num] - PATCH_ENV_SUSTAIN_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_RATE_KS)
    {
      value = p->env_rate_ks[pr->num];
      value_string = S_patch_edit_1_to_32_values[p->env_rate_ks[pr->num] - PATCH_ENV_KEYSCALE_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_LEVEL_KS)
    {
      value = p->env_level_ks[pr->num];
      value_string = S_patch_edit_1_to_32_values[p->env_level_ks[pr->num] - PATCH_ENV_KEYSCALE_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_ENV_TRIGGER)
    {
      value = p->env_trigger[pr->num];
      value_string = S_patch_edit_env_trigger_values[p->env_trigger[pr->num] - PATCH_ENV_TRIGGER_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_VIBRATO_ENABLE)
    {
      value = p->vibrato_enable[pr->num];
      value_string = S_patch_edit_mod_enable_values[p->vibrato_enable[pr->num] - PATCH_MOD_ENABLE_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_TREMOLO_ENABLE)
    {
      value = p->tremolo_enable[pr->num];
      value_string = S_patch_edit_mod_enable_values[p->tremolo_enable[pr->num] - PATCH_MOD_ENABLE_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_BOOST_ENABLE)
    {
      value = p->boost_enable[pr->num];
      value_string = S_patch_edit_mod_enable_values[p->boost_enable[pr->num] - PATCH_MOD_ENABLE_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_LFO_WAVEFORM)
    {
      value = p->lfo_waveform;
      value_string = S_patch_edit_lfo_waveform_values[p->lfo_waveform - PATCH_LFO_WAVEFORM_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_LFO_FREQUENCY)
    {
      value = p->lfo_frequency;
      value_string = S_patch_edit_1_to_32_values[p->lfo_frequency - PATCH_LFO_FREQUENCY_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_LFO_SYNC)
    {
      value = p->lfo_sync;
      value_string = S_patch_edit_lfo_sync_values[p->lfo_sync - PATCH_LFO_SYNC_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_LFO_DELAY)
    {
      value = p->lfo_delay;
      value_string = S_patch_edit_0_to_32_values[p->lfo_delay - PATCH_LFO_DELAY_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_LFO_BASE_VIBRATO)
    {
      value = p->lfo_base_vibrato;
      value_string = S_patch_edit_0_to_32_values[p->lfo_base_vibrato - PATCH_MOD_BASE_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_LFO_BASE_TREMOLO)
    {
      value = p->lfo_base_tremolo;
      value_string = S_patch_edit_0_to_32_values[p->lfo_base_tremolo - PATCH_MOD_BASE_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_PORTAMENTO_MODE)
    {
      value = p->portamento_mode;
      value_string = S_patch_edit_portamento_mode_values[p->portamento_mode - PATCH_PORTAMENTO_MODE_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_PORTAMENTO_SPEED)
    {
      value = p->portamento_speed;
      value_string = S_patch_edit_1_to_32_values[p->portamento_speed - PATCH_PORTAMENTO_SPEED_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_HIGHPASS_CUTOFF)
    {
      value = p->highpass_cutoff;
      value_string = S_patch_edit_highpass_cutoff_values[p->highpass_cutoff - PATCH_HIGHPASS_CUTOFF_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_LOWPASS_CUTOFF)
    {
      value = p->lowpass_cutoff;
      value_string = S_patch_edit_lowpass_cutoff_values[p->lowpass_cutoff - PATCH_LOWPASS_CUTOFF_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_NOISE_MIX)
    {
      value = p->noise_mix;
      value_string = S_patch_edit_0_to_32_values[p->noise_mix - PATCH_NOISE_MIX_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_NOISE_FREQUENCY)
    {
      value = p->noise_frequency;
      value_string = S_patch_edit_1_to_32_values[p->noise_frequency - PATCH_NOISE_FREQUENCY_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_VIBRATO_DEPTH)
    {
      value = p->vibrato_depth;
      value_string = S_patch_edit_1_to_32_values[p->vibrato_depth - PATCH_MOD_DEPTH_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_TREMOLO_DEPTH)
    {
      value = p->tremolo_depth;
      value_string = S_patch_edit_1_to_32_values[p->tremolo_depth - PATCH_MOD_DEPTH_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_BOOST_DEPTH)
    {
      value = p->boost_depth;
      value_string = S_patch_edit_1_to_32_values[p->boost_depth - PATCH_MOD_DEPTH_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_MOD_WHEEL_VIBRATO)
    {
      value = p->mod_wheel_vibrato;
      value_string = S_patch_edit_0_to_32_values[p->mod_wheel_vibrato - PATCH_MOD_CONTROLLER_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_MOD_WHEEL_TREMOLO)
    {
      value = p->mod_wheel_tremolo;
      value_string = S_patch_edit_0_to_32_values[p->mod_wheel_tremolo - PATCH_MOD_CONTROLLER_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_MOD_WHEEL_BOOST)
    {
      value = p->mod_wheel_boost;
      value_string = S_patch_edit_0_to_32_values[p->mod_wheel_boost - PATCH_MOD_CONTROLLER_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_AFTERTOUCH_VIBRATO)
    {
      value = p->aftertouch_vibrato;
      value_string = S_patch_edit_0_to_32_values[p->aftertouch_vibrato - PATCH_MOD_CONTROLLER_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_AFTERTOUCH_TREMOLO)
    {
      value = p->aftertouch_tremolo;
      value_string = S_patch_edit_0_to_32_values[p->aftertouch_tremolo - PATCH_MOD_CONTROLLER_LOWER_BOUND];
    }
    else if (pr->label == LAYOUT_PARAM_PATCH_EDIT_LABEL_AFTERTOUCH_BOOST)
    {
      value = p->aftertouch_boost;
      value_string = S_patch_edit_0_to_32_values[p->aftertouch_boost - PATCH_MOD_CONTROLLER_LOWER_BOUND];
    }
    else
    {
      value = 0;
      value_string = NULL;
    }

    /* load the parameter name, value, and slider or arrows */
    if (pr->adjust_type == LAYOUT_PARAM_PATCH_EDIT_ADJUST_TYPE_SLIDER)
    {
      vb_all_load_text( pr->center_x + LAYOUT_PATCH_EDIT_PARAM_NAME_X, 
                        pr->center_y - G_current_scroll_amount, 
                        VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_2, 16, 
                        S_patch_edit_parameter_labels[pr->label]);

      vb_all_load_text( pr->center_x + LAYOUT_PATCH_EDIT_PARAM_SLIDER_VALUE_X, 
                        pr->center_y - G_current_scroll_amount, 
                        VB_ALL_ALIGN_CENTER, 0, 6, 16, 
                        value_string);

      vb_all_load_slider( pr->center_x + LAYOUT_PATCH_EDIT_PARAM_SLIDER_TRACK_X, 
                          pr->center_y - G_current_scroll_amount, 
                          LAYOUT_PATCH_EDIT_PARAM_SLIDER_WIDTH, 
                          value, pr->lower_bound, pr->upper_bound);
    }
    else if (pr->adjust_type == LAYOUT_PARAM_PATCH_EDIT_ADJUST_TYPE_ARROWS)
    {
      vb_all_load_text( pr->center_x + LAYOUT_PATCH_EDIT_PARAM_NAME_X, 
                        pr->center_y - G_current_scroll_amount, 
                        VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_2, 16, 
                        S_patch_edit_parameter_labels[pr->label]);

      vb_all_load_text( pr->center_x + LAYOUT_PATCH_EDIT_PARAM_ADJUST_VALUE_X, 
                        pr->center_y - G_current_scroll_amount, 
                        VB_ALL_ALIGN_CENTER, 0, 6, 16, 
                        value_string);

      if (value > pr->lower_bound)
      {
        vb_all_load_named_sprite( VB_ALL_SPRITE_NAME_ADJUST_PARAM_LEFT, 
                                  pr->center_x + LAYOUT_PATCH_EDIT_PARAM_ADJUST_LEFT_X, 
                                  pr->center_y - G_current_scroll_amount, 
                                  0, VB_ALL_PALETTE_1);
      }

      if (value < pr->upper_bound)
      {
        vb_all_load_named_sprite( VB_ALL_SPRITE_NAME_ADJUST_PARAM_RIGHT, 
                                  pr->center_x + LAYOUT_PATCH_EDIT_PARAM_ADJUST_RIGHT_X, 
                                  pr->center_y - G_current_scroll_amount, 
                                  0, VB_ALL_PALETTE_1);
      }
    }
  }

  /* update vbos */
  VB_ALL_UPDATE_OVERLAY_SPRITES_IN_VBOS()

  return 0;
}

/*******************************************************************************
** vb_all_load_patterns_overlay()
*******************************************************************************/
short int vb_all_load_patterns_overlay()
{
  /* update vbos */
  VB_ALL_UPDATE_OVERLAY_SPRITES_IN_VBOS()

  return 0;
}

/*******************************************************************************
** vb_all_clear_panels_buttons_and_overlay()
*******************************************************************************/
short int vb_all_clear_panels_buttons_and_overlay()
{
  /* reset panels tile vbo count */
  G_tile_layer_counts[GRAPHICS_TILE_LAYER_PANELS] = 0;

  /* reset sprite vbo counts */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_BUTTONS] = 0;
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_OVERLAY] = 0;

  return 0;
}

