/*******************************************************************************
** vball.c (vbo population functions - all)
*******************************************************************************/

#include <glad/glad.h>

#include <stdio.h>
#include <stdlib.h>

#include "controls.h"
#include "global.h"
#include "graphics.h"
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
  VB_ALL_BUTTON_OFF = 0,
  VB_ALL_BUTTON_ON
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
  if (sprite_index < GRAPHICS_PANELS_AND_BUTTONS_SPRITES_END_INDEX)                         \
  {                                                                                         \
    VB_ALL_ADD_SPRITE_TO_VERTEX_BUFFER(pos_x, pos_y, 2, 2, GRAPHICS_Z_LEVEL_PANELS)         \
    VB_ALL_ADD_SPRITE_TO_TEXTURE_COORD_BUFFER(cell_x, cell_y, 2, 2)                         \
    VB_ALL_ADD_SPRITE_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)                     \
    VB_ALL_ADD_SPRITE_TO_ELEMENT_BUFFER()                                                   \
                                                                                            \
    sprite_index += 1;                                                                      \
  }

#define VB_ALL_ADD_BUTTON_PIECE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette) \
  if (sprite_index < GRAPHICS_PANELS_AND_BUTTONS_SPRITES_END_INDEX)                         \
  {                                                                                         \
    VB_ALL_ADD_SPRITE_TO_VERTEX_BUFFER(pos_x, pos_y, 2, 2, GRAPHICS_Z_LEVEL_BUTTONS)        \
    VB_ALL_ADD_SPRITE_TO_TEXTURE_COORD_BUFFER(cell_x, cell_y, 2, 2)                         \
    VB_ALL_ADD_SPRITE_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)                     \
    VB_ALL_ADD_SPRITE_TO_ELEMENT_BUFFER()                                                   \
                                                                                            \
    sprite_index += 1;                                                                      \
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

#define VB_ALL_UPDATE_PANELS_SPRITES_IN_VBOS()                                                                  \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_sprites);                                                    \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                              \
                  GRAPHICS_PANELS_AND_BUTTONS_SPRITES_START_INDEX * 12 * sizeof(GLfloat),                       \
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_PANELS_AND_BUTTONS] * 12 * sizeof(GLfloat),       \
                  &G_vertex_buffer_sprites[GRAPHICS_PANELS_AND_BUTTONS_SPRITES_START_INDEX * 12]);              \
                                                                                                                \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_sprites);                                             \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                              \
                  GRAPHICS_PANELS_AND_BUTTONS_SPRITES_START_INDEX * 8 * sizeof(GLfloat),                        \
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_PANELS_AND_BUTTONS] * 8 * sizeof(GLfloat),        \
                  &G_texture_coord_buffer_sprites[GRAPHICS_PANELS_AND_BUTTONS_SPRITES_START_INDEX * 8]);        \
                                                                                                                \
  glBindBuffer(GL_ARRAY_BUFFER, G_lighting_and_palette_buffer_id_sprites);                                      \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                              \
                  GRAPHICS_PANELS_AND_BUTTONS_SPRITES_START_INDEX * 8 * sizeof(GLfloat),                        \
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_PANELS_AND_BUTTONS] * 8 * sizeof(GLfloat),        \
                  &G_lighting_and_palette_buffer_sprites[GRAPHICS_PANELS_AND_BUTTONS_SPRITES_START_INDEX * 8]); \
                                                                                                                \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_sprites);                                             \
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,                                                                      \
                  GRAPHICS_PANELS_AND_BUTTONS_SPRITES_START_INDEX * 6 * sizeof(unsigned short),                 \
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_PANELS_AND_BUTTONS] * 6 * sizeof(unsigned short), \
                  &G_index_buffer_sprites[GRAPHICS_PANELS_AND_BUTTONS_SPRITES_START_INDEX * 6]);

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

/*******************************************************************************
** vb_all_load_panel()
*******************************************************************************/
short int vb_all_load_panel(int offset_x, int offset_y, int width, int height)
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

  /* make sure the width and height are valid       */
  /* the width & height are in terms of 16x16 tiles */
  if ((width < 2) || (width > GRAPHICS_OVERSCAN_WIDTH / 16))
    return 1;

  if ((height < 2) || (height > GRAPHICS_OVERSCAN_HEIGHT / 16))
    return 1;

  /* determine coordinates of top left corner */
  corner_x = (GRAPHICS_OVERSCAN_WIDTH - 16 * width) / 2;
  corner_y = (GRAPHICS_OVERSCAN_HEIGHT - 16 * height) / 2;

  /* the offsets from the screen center are in 4x4 half-cells */
  corner_x += 4 * offset_x;
  corner_y += 4 * offset_y;

  /* make sure the panel is on the screen */
  if (corner_x + (16 * width) < 0)
    return 1;

  if (corner_x > GRAPHICS_OVERSCAN_WIDTH)
    return 1;

  if (corner_y + (16 * height) < 0)
    return 1;

  if (corner_y > GRAPHICS_OVERSCAN_HEIGHT)
    return 1;

  /* set lighting and palette */
  lighting = 0;
  palette = VB_ALL_PALETTE_1;

  /* draw the panel */
  sprite_index =  GRAPHICS_PANELS_AND_BUTTONS_SPRITES_START_INDEX + 
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_PANELS_AND_BUTTONS];

  for (n = 0; n < height; n++)
  {
    for (m = 0; m < width; m++)
    {
      /* determine center of this piece */
      pos_x = corner_x + (16 * m) + 8;
      pos_y = corner_y + (16 * n) + 8;

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
        cell_y = 14;
      }
      /* top right corner piece */
      else if ((m == width - 1) && (n == 0))
      {
        cell_x = 2;
        cell_y = 12;
      }
      /* bottom right corner piece */
      else if ((m == width - 1) && (n == height - 1))
      {
        cell_x = 2;
        cell_y = 14;
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
        cell_y = 14;
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
        cell_x = 2;
        cell_y = 13;
      }
      /* middle piece */
      else
      {
        cell_x = 1;
        cell_y = 13;
      }

      VB_ALL_ADD_PANEL_PIECE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette)
    }
  }

  /* update panels sprite layer count */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_PANELS_AND_BUTTONS] = 
    sprite_index - GRAPHICS_PANELS_AND_BUTTONS_SPRITES_START_INDEX;

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

  /* make sure the width is valid         */
  /* the width is in terms of 16x16 tiles */
  if ((width < 2) || (width > GRAPHICS_OVERSCAN_WIDTH / 16))
    return 1;

  /* determine coordinates of top left corner */
  corner_x = (GRAPHICS_OVERSCAN_WIDTH - 16 * width) / 2;
  corner_y = (GRAPHICS_OVERSCAN_HEIGHT - 16 * 1) / 2;

  /* the offsets from the screen center are in 4x4 half-cells */
  corner_x += 4 * offset_x;
  corner_y += 4 * offset_y;

  /* make sure the button is on the screen */
  if (corner_x + (16 * width) < 0)
    return 1;

  if (corner_x > GRAPHICS_OVERSCAN_WIDTH)
    return 1;

  if (corner_y + (16 * 1) < 0)
    return 1;

  if (corner_y > GRAPHICS_OVERSCAN_HEIGHT)
    return 1;

  /* set lighting and palette */
  if (state == VB_ALL_BUTTON_OFF)
  {
    lighting = 0;
    palette = 6;
  }
  else if (state == VB_ALL_BUTTON_ON)
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
  sprite_index =  GRAPHICS_PANELS_AND_BUTTONS_SPRITES_START_INDEX + 
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_PANELS_AND_BUTTONS];

  for (m = 0; m < width; m++)
  {
    /* determine center of this piece */
    pos_x = corner_x + (16 * m) + 8;
    pos_y = corner_y + (16 * 0) + 8;

    /* left piece */
    if (m == 0)
    {
      cell_x = 0;
      cell_y = 10;
    }
    /* right piece */
    else if (m == width - 1)
    {
      cell_x = 2;
      cell_y = 10;
    }
    /* middle piece */
    else
    {
      cell_x = 1;
      cell_y = 10;
    }

    VB_ALL_ADD_BUTTON_PIECE_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette)
  }

  /* update panels sprite layer count */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_PANELS_AND_BUTTONS] = 
    sprite_index - GRAPHICS_PANELS_AND_BUTTONS_SPRITES_START_INDEX;

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

  /* make sure the text string is on the screen */
  if (start_x + (8 * (length - 1)) + 4 < 0)
    return 1;

  if (start_x - 4 > GRAPHICS_OVERSCAN_WIDTH)
    return 1;

  if (start_y + 4 < 0)
    return 1;

  if (start_y - 4 > GRAPHICS_OVERSCAN_HEIGHT)
    return 1;

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
    cell_x = 6;
    cell_y = 6;
    width = 1;
    height = 1;
  }
  else if (name == VB_ALL_SPRITE_NAME_ADJUST_PARAM_RIGHT)
  {
    cell_x = 7;
    cell_y = 6;
    width = 1;
    height = 1;
  }
  else
    return 1;

  /* make sure the sprite is on the screen */
  if (pos_x + 4 < 0)
    return 1;

  if (pos_x - 4 > GRAPHICS_OVERSCAN_WIDTH)
    return 1;

  if (pos_y + 4 < 0)
    return 1;

  if (pos_y - 4 > GRAPHICS_OVERSCAN_HEIGHT)
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
** vb_all_load_parameter()
*******************************************************************************/
short int vb_all_load_parameter(int offset_x, int offset_y, 
                                int lighting, int palette, 
                                short int param, short int low, short int high)
{
  int length;

  int sprite_index;

  int start_x;
  int start_y;

  int pos_x;
  int pos_y;

  int cell_x;
  int cell_y;

  /* make sure the bounds are valid */
  if ((low < 0) || (high < 0) || (low > high))
    return 1;

  /* make sure the parameter is valid */
  if ((param < low) || (param > high))
    return 1;

  /* determine length of parameter string */
  if ((param >= 0) && (param <= 9))
    length = 1;
  else if ((param >= 10) && (param <= 99))
    length = 2;
  else
    return 1;

  /* determine coordinates of center of first character */
  /* note that each character is 8x8                    */
  start_x = (GRAPHICS_OVERSCAN_WIDTH / 2);
  start_y = (GRAPHICS_OVERSCAN_HEIGHT / 2);

  /* the alignment is set to "center" here */
  start_x -= 4 * (length - 1);

  /* the offsets from the screen center are in 4x4 half-cells */
  start_x += 4 * offset_x;
  start_y += 4 * offset_y;

  /* make sure the parameter string is on the screen */
  if (start_x + (8 * (length - 1)) + 4 < 0)
    return 1;

  if (start_x - 4 > GRAPHICS_OVERSCAN_WIDTH)
    return 1;

  if (start_y + 4 < 0)
    return 1;

  if (start_y - 4 > GRAPHICS_OVERSCAN_HEIGHT)
    return 1;

  /* bound lighting and palette */
  if ((lighting < -4) || (lighting > 3))
    lighting = 0;

  if ((palette < 0) || (palette > 15))
    palette = 0;

  /* draw the parameter string */
  sprite_index =  GRAPHICS_OVERLAY_SPRITES_START_INDEX + 
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_OVERLAY];

  if (length == 1)
  {
    /* ones place */
    pos_x = start_x;
    pos_y = start_y;

    cell_x = param % 10;
    cell_y = 1;

    VB_ALL_ADD_FONT_CHARACTER_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette)
  }
  else if (length == 2)
  {
    /* tens place */
    pos_x = start_x;
    pos_y = start_y;

    cell_x = param / 10;
    cell_y = 1;

    VB_ALL_ADD_FONT_CHARACTER_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette)

    /* ones place */
    pos_x = start_x + 8;
    pos_y = start_y;

    cell_x = param % 10;
    cell_y = 1;

    VB_ALL_ADD_FONT_CHARACTER_TO_BUFFERS(pos_x, pos_y, cell_x, cell_y, lighting, palette)
  }

  /* update overlay sprite layer count */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_OVERLAY] = 
    sprite_index - GRAPHICS_OVERLAY_SPRITES_START_INDEX;

  /* draw sprites for adjustment arrows if necessary */
  if (param > low)
  {
    vb_all_load_named_sprite(VB_ALL_SPRITE_NAME_ADJUST_PARAM_LEFT, offset_x - 4, offset_y, 0, VB_ALL_PALETTE_1);
  }

  if (param < high)
  {
    vb_all_load_named_sprite(VB_ALL_SPRITE_NAME_ADJUST_PARAM_RIGHT, offset_x + 4, offset_y, 0, VB_ALL_PALETTE_1);
  }

  return 0;
}

/*******************************************************************************
** vb_all_load_menu_background()
*******************************************************************************/
short int vb_all_load_menu_background()
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
** vb_all_load_patches_panels_and_buttons()
*******************************************************************************/
short int vb_all_load_patches_panels_and_buttons()
{
  /* reset sprite vbo counts */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_PANELS_AND_BUTTONS] = 0;
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_OVERLAY] = 0;

  /* top panel */
  vb_all_load_panel(0, -24, 20, 3);

  /* top buttons */
  vb_all_load_button(-30, -22, 4, VB_ALL_BUTTON_ON);
  vb_all_load_button(-12, -22, 5, VB_ALL_BUTTON_OFF);
  vb_all_load_button(8, -22, 5, VB_ALL_BUTTON_OFF);
  vb_all_load_button(24, -22, 3, VB_ALL_BUTTON_OFF);

  /* settings buttons */

  /* bottom panel */
  vb_all_load_panel(0, 26, 20, 2);

  /* update vbos */
  VB_ALL_UPDATE_PANELS_SPRITES_IN_VBOS()

  return 0;
}

/*******************************************************************************
** vb_all_load_patches_overlay()
*******************************************************************************/
short int vb_all_load_patches_overlay()
{
  patch* p;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(0))
    return 1;

  /* obtain patch pointer */
  p = &G_patch_bank[0];

  /* reset overlay sprite vbo count */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_OVERLAY] = 0;

  /* top panel text */
  vb_all_load_text(0, -26, VB_ALL_ALIGN_CENTER, 0, 6, 32, "Gersemi v0.9");

  /* top button text */
  vb_all_load_text(-30, -22, VB_ALL_ALIGN_CENTER, 0, 6, 16, "Patches");
  vb_all_load_text(-12, -22, VB_ALL_ALIGN_CENTER, 0, 6, 16, "Patterns");
  vb_all_load_text(8, -22, VB_ALL_ALIGN_CENTER, 0, 6, 16, "Drums/SFX");
  vb_all_load_text(24, -22, VB_ALL_ALIGN_CENTER, 0, 6, 16, "Song");

  /* patch settings text */
  vb_all_load_text(-38, -16, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Patch");

  vb_all_load_named_sprite(VB_ALL_SPRITE_NAME_ADJUST_PARAM_LEFT, -27, -16, 0, VB_ALL_PALETTE_1);
  vb_all_load_text(-23, -16, VB_ALL_ALIGN_CENTER, 0, 6, 16, "01");
  vb_all_load_named_sprite(VB_ALL_SPRITE_NAME_ADJUST_PARAM_RIGHT, -19, -16, 0, VB_ALL_PALETTE_1);

  vb_all_load_text(-15, -16, VB_ALL_ALIGN_LEFT, 0, 6, 16, "Name of Patch");

  /* program, algorithm, etc settings text */
  vb_all_load_text(-38, -13, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Program");

  vb_all_load_text(2, -13, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Wave");

  /* oscillator 1 settings text */
  vb_all_load_text(-30, -10, VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_3, 16, "Osc 1");
  vb_all_load_text(-38, -7, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Num");
  vb_all_load_text(-38, -4, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Den");
  vb_all_load_text(-38, -1, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Det");
  vb_all_load_text(-38,  2, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Amp");

  vb_all_load_named_sprite(VB_ALL_SPRITE_NAME_ADJUST_PARAM_LEFT, -32, -7, 0, VB_ALL_PALETTE_1);
  vb_all_load_text(-28, -7, VB_ALL_ALIGN_CENTER, 0, 6, 16, "01");
  vb_all_load_named_sprite(VB_ALL_SPRITE_NAME_ADJUST_PARAM_RIGHT, -24, -7, 0, VB_ALL_PALETTE_1);

  vb_all_load_named_sprite(VB_ALL_SPRITE_NAME_ADJUST_PARAM_LEFT, -32, -4, 0, VB_ALL_PALETTE_1);
  vb_all_load_text(-28, -4, VB_ALL_ALIGN_CENTER, 0, 6, 16, "01");
  vb_all_load_named_sprite(VB_ALL_SPRITE_NAME_ADJUST_PARAM_RIGHT, -24, -4, 0, VB_ALL_PALETTE_1);

  vb_all_load_named_sprite(VB_ALL_SPRITE_NAME_ADJUST_PARAM_LEFT, -32, -1, 0, VB_ALL_PALETTE_1);
  vb_all_load_text(-28, -1, VB_ALL_ALIGN_CENTER, 0, 6, 16, "01");
  vb_all_load_named_sprite(VB_ALL_SPRITE_NAME_ADJUST_PARAM_RIGHT, -24, -1, 0, VB_ALL_PALETTE_1);

  vb_all_load_named_sprite(VB_ALL_SPRITE_NAME_ADJUST_PARAM_LEFT, -32, 2, 0, VB_ALL_PALETTE_1);
  vb_all_load_text(-28,  2, VB_ALL_ALIGN_CENTER, 0, 6, 16, "01");
  vb_all_load_named_sprite(VB_ALL_SPRITE_NAME_ADJUST_PARAM_RIGHT, -24, 2, 0, VB_ALL_PALETTE_1);

  /* oscillator 2 settings text */
  vb_all_load_text(-10, -10, VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_3, 16, "Osc 2");
  vb_all_load_text(-18, -7, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Num");
  vb_all_load_text(-18, -4, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Den");
  vb_all_load_text(-18, -1, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Det");
  vb_all_load_text(-18,  2, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Amp");

  /* oscillator 3 settings text */
  vb_all_load_text(10, -10, VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_3, 16, "Osc 3");
  vb_all_load_text(2, -7, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Num");
  vb_all_load_text(2, -4, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Den");
  vb_all_load_text(2, -1, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Det");
  vb_all_load_text(2,  2, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Amp");

  /* oscillator 4 settings text */
  vb_all_load_text(30, -10, VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_3, 16, "Osc 4");
  vb_all_load_text(22, -7, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Num");
  vb_all_load_text(22, -4, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Den");
  vb_all_load_text(22, -1, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Det");
  vb_all_load_text(22,  2, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Amp");

  /* carrier envelope settings */
  vb_all_load_text(-30,  5, VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_3, 16, "Car Env");

  vb_all_load_text(-38,  8, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Att");
  vb_all_load_text(-37, 11, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "D1");
  vb_all_load_text(-37, 14, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "D2");
  vb_all_load_text(-38, 17, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Rel");
  vb_all_load_text(-38, 20, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Sus");

  vb_all_load_parameter(-28,  8, 0, 6, p->carr_attack, 0, 32);
  vb_all_load_parameter(-28, 11, 0, 6, p->carr_decay_1, 0, 32);
  vb_all_load_parameter(-28, 14, 0, 6, p->carr_decay_2, 0, 32);
  vb_all_load_parameter(-28, 17, 0, 6, p->carr_release, 0, 32);
  vb_all_load_parameter(-28, 20, 0, 6, p->carr_sustain, 0, 16);

  /* modulator envelope settings */
  vb_all_load_text(-10,  5, VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_3, 16, "Mod Env");

  vb_all_load_text(-18,  8, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Att");
  vb_all_load_text(-17, 11, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "D1");
  vb_all_load_text(-17, 14, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "D2");
  vb_all_load_text(-18, 17, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Rel");
  vb_all_load_text(-18, 20, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Sus");

  /* keyscaling settings */
  vb_all_load_text(10,  5, VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_3, 16, "Car KS");

  vb_all_load_text(2,  8, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Rate");
  vb_all_load_text(2, 11, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Lev");

  vb_all_load_text(10, 14, VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_3, 16, "Mod KS");

  vb_all_load_text(2, 17, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Rate");
  vb_all_load_text(2, 20, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Lev");

  /* noise and filter settings */
  vb_all_load_text(30,  5, VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_3, 16, "Noise");

  vb_all_load_text(22,  8, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Per");
  vb_all_load_text(22, 11, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "Mix");

  vb_all_load_text(30, 14, VB_ALL_ALIGN_CENTER, 0, VB_ALL_PALETTE_3, 16, "Filters");

  vb_all_load_text(22, 17, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "LP");
  vb_all_load_text(22, 20, VB_ALL_ALIGN_LEFT, 0, VB_ALL_PALETTE_2, 16, "HP");

  /* update vbos */
  VB_ALL_UPDATE_OVERLAY_SPRITES_IN_VBOS()

  return 0;
}

/*******************************************************************************
** vb_all_load_patterns_panels_and_buttons()
*******************************************************************************/
short int vb_all_load_patterns_panels_and_buttons()
{
  /* reset sprite vbo counts */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_PANELS_AND_BUTTONS] = 0;
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_OVERLAY] = 0;

  /* top panel */
  vb_all_load_panel(0, -24, 20, 3);

  /* top buttons */
  vb_all_load_button(-30, -22, 4, VB_ALL_BUTTON_OFF);
  vb_all_load_button(-12, -22, 5, VB_ALL_BUTTON_ON);
  vb_all_load_button(8, -22, 5, VB_ALL_BUTTON_OFF);
  vb_all_load_button(24, -22, 3, VB_ALL_BUTTON_OFF);

  /* settings buttons */

  /* update vbos */
  VB_ALL_UPDATE_PANELS_SPRITES_IN_VBOS()

  return 0;
}

/*******************************************************************************
** vb_all_load_patterns_overlay()
*******************************************************************************/
short int vb_all_load_patterns_overlay()
{
  /* reset overlay sprite vbo count */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_OVERLAY] = 0;

  /* top panel text */
  vb_all_load_text(0, -26, VB_ALL_ALIGN_CENTER, 0, 6, 32, "Gersemi v0.9");

  /* top button text */
  vb_all_load_text(-30, -22, VB_ALL_ALIGN_CENTER, 0, 6, 16, "Patches");
  vb_all_load_text(-12, -22, VB_ALL_ALIGN_CENTER, 0, 6, 16, "Patterns");
  vb_all_load_text(8, -22, VB_ALL_ALIGN_CENTER, 0, 6, 16, "Drums/SFX");
  vb_all_load_text(24, -22, VB_ALL_ALIGN_CENTER, 0, 6, 16, "Song");

  /* settings text */

  /* update vbos */
  VB_ALL_UPDATE_OVERLAY_SPRITES_IN_VBOS()

  return 0;
}

/*******************************************************************************
** vb_all_clear_panels_buttons_and_overlay()
*******************************************************************************/
short int vb_all_clear_panels_buttons_and_overlay()
{
  /* reset sprite vbo counts */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_PANELS_AND_BUTTONS] = 0;
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_OVERLAY] = 0;

  return 0;
}

