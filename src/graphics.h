/*******************************************************************************
** graphics.h (opengl objects)
*******************************************************************************/

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <glad/glad.h>

enum
{
  GRAPHICS_RESOLUTION_640_480 = 0,
  GRAPHICS_RESOLUTION_800_600,
  GRAPHICS_RESOLUTION_960_720,
  GRAPHICS_RESOLUTION_1024_768,
  GRAPHICS_RESOLUTION_1280_960,
  GRAPHICS_RESOLUTION_1440_1080,
  GRAPHICS_NUM_RESOLUTIONS
};

enum
{
  GRAPHICS_TILE_LAYER_BACKGROUND = 0,
  GRAPHICS_NUM_TILE_LAYERS
};

enum
{
  GRAPHICS_SPRITE_LAYER_PANELS_AND_BUTTONS = 0,
  GRAPHICS_SPRITE_LAYER_OVERLAY,
  GRAPHICS_NUM_SPRITE_LAYERS
};

#define GRAPHICS_OVERSCAN_WIDTH   320
#define GRAPHICS_OVERSCAN_HEIGHT  240

#define GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH   2048
#define GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT  1024

/* the maximum number of tiles is determined as follows:        */
/*   number of 16x16 background tiles (1 screen): 20 * 15 = 300 */
/* overall, this is 300                                         */
#define GRAPHICS_MAX_BACKGROUND_TILES 300

#define GRAPHICS_MAX_TILES  GRAPHICS_MAX_BACKGROUND_TILES

#define GRAPHICS_BACKGROUND_TILES_START_INDEX 0
#define GRAPHICS_BACKGROUND_TILES_END_INDEX   ( GRAPHICS_BACKGROUND_TILES_START_INDEX + \
                                                GRAPHICS_MAX_BACKGROUND_TILES)

/* the maximum number of sprites is determined as follows:    */
/*   number of 16x16 panel pieces (1 screen): 20 * 15 = 300   */
/*   number of 16x16 button pieces (1 screen): 20 * 15 = 300  */
/*   number of 8x8 text characters (1 screen): 40 * 30 = 1200 */
/* overall, this is 300 + 300 + 1200 = 1800                   */
#define GRAPHICS_MAX_PANELS_AND_BUTTONS_SPRITES 600
#define GRAPHICS_MAX_OVERLAY_SPRITES            1200

#define GRAPHICS_MAX_SPRITES  ( GRAPHICS_MAX_PANELS_AND_BUTTONS_SPRITES +        \
                                GRAPHICS_MAX_OVERLAY_SPRITES)

#define GRAPHICS_PANELS_AND_BUTTONS_SPRITES_START_INDEX   0
#define GRAPHICS_PANELS_AND_BUTTONS_SPRITES_END_INDEX     ( GRAPHICS_PANELS_AND_BUTTONS_SPRITES_START_INDEX + \
                                                            GRAPHICS_MAX_PANELS_AND_BUTTONS_SPRITES)

#define GRAPHICS_OVERLAY_SPRITES_START_INDEX  GRAPHICS_PANELS_AND_BUTTONS_SPRITES_END_INDEX
#define GRAPHICS_OVERLAY_SPRITES_END_INDEX    ( GRAPHICS_OVERLAY_SPRITES_START_INDEX + \
                                                GRAPHICS_MAX_OVERLAY_SPRITES)

/* the near and far planes are from 0.125 (near) to 1.375 (far)   */
#define GRAPHICS_Z_LEVEL_BACKGROUND  1.25f

/* the z levels for the panels and overlay shouldn't  */
/* matter, as the depth buffer should be reset and    */
/* the depth test should be off when we draw them.    */
#define GRAPHICS_Z_LEVEL_PANELS   0.5f
#define GRAPHICS_Z_LEVEL_BUTTONS  0.375f
#define GRAPHICS_Z_LEVEL_OVERLAY  0.25f

/* for the postprocessing, the z level of the fullscreen  */
/* quad is set to the midpoint of the near and far planes */
#define GRAPHICS_Z_LEVEL_FULL_SCREEN_QUAD   0.75f

/* sdl window, various sizes, etc */
extern SDL_Window* G_sdl_window;

extern int G_graphics_resolution;

extern int G_viewport_w;
extern int G_viewport_h;

/* vbo size variables */
extern int G_tile_layer_counts[GRAPHICS_NUM_TILE_LAYERS];
extern int G_sprite_layer_counts[GRAPHICS_NUM_SPRITE_LAYERS];

/* opengl vbo ids */
extern GLuint G_vertex_array_id;

extern GLuint G_vertex_buffer_id_tiles;
extern GLuint G_texture_coord_buffer_id_tiles;
extern GLuint G_lighting_and_palette_buffer_id_tiles;
extern GLuint G_index_buffer_id_tiles;

extern GLuint G_vertex_buffer_id_sprites;
extern GLuint G_texture_coord_buffer_id_sprites;
extern GLuint G_lighting_and_palette_buffer_id_sprites;
extern GLuint G_index_buffer_id_sprites;

/* postprocessing vbo ids */
extern GLuint G_vertex_buffer_id_postprocessing_overscan;
extern GLuint G_vertex_buffer_id_postprocessing_window;

extern GLuint G_texture_coord_buffer_id_postprocessing_overscan;

extern GLuint G_index_buffer_id_postprocessing_all;

/* opengl intermediate textures */
extern GLuint G_texture_id_intermediate_1;
extern GLuint G_texture_id_intermediate_2;

/* opengl framebuffer ids */
extern GLuint G_framebuffer_id_intermediate_1;
extern GLuint G_renderbuffer_id_intermediate_1;

extern GLuint G_framebuffer_id_intermediate_2;
extern GLuint G_renderbuffer_id_intermediate_2;

/* vbos and matrices */
extern GLfloat*         G_vertex_buffer_tiles;
extern GLfloat*         G_texture_coord_buffer_tiles;
extern GLfloat*         G_lighting_and_palette_buffer_tiles;
extern unsigned short*  G_index_buffer_tiles;

extern GLfloat*         G_vertex_buffer_sprites;
extern GLfloat*         G_texture_coord_buffer_sprites;
extern GLfloat*         G_lighting_and_palette_buffer_sprites;
extern unsigned short*  G_index_buffer_sprites;

extern GLfloat          G_vertex_buffer_postprocessing_overscan[12];
extern GLfloat          G_vertex_buffer_postprocessing_window[12];

extern GLfloat          G_texture_coord_buffer_postprocessing_overscan[8];

extern unsigned short   G_index_buffer_postprocessing_all[6];

extern GLfloat          G_mvp_matrix_overscan[16];
extern GLfloat          G_mvp_matrix_intermediate[16];
extern GLfloat          G_mvp_matrix_window[16];

/* function declarations */
short int graphics_create_opengl_objects();
short int graphics_destroy_opengl_objects();

short int graphics_set_graphics_resolution(int resolution);
short int graphics_read_desktop_dimensions();

short int graphics_setup_viewport_mvp_matrix_and_buffers();

short int graphics_set_window_size(int res);
short int graphics_increase_window_size();
short int graphics_decrease_window_size();

#endif

