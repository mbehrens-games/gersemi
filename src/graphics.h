/*******************************************************************************
** graphics.h (opengl objects)
*******************************************************************************/

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <glad/glad.h>

enum
{
  GRAPHICS_RESOLUTION_480P = 0, 
  GRAPHICS_RESOLUTION_600P, 
  GRAPHICS_RESOLUTION_720P, 
  GRAPHICS_RESOLUTION_768P, 
  GRAPHICS_RESOLUTION_1080P, 
  GRAPHICS_NUM_RESOLUTIONS 
};

enum
{
  GRAPHICS_INTERMEDIATE_SET_1 = 0, 
  GRAPHICS_INTERMEDIATE_SET_2, 
  GRAPHICS_NUM_INTERMEDIATE_SETS 
};

enum
{
  GRAPHICS_BUFFER_SET_TILES = 0, 
  GRAPHICS_BUFFER_SET_SPRITES, 
  GRAPHICS_NUM_BUFFER_SETS 
};

enum
{
  GRAPHICS_TILE_LAYER_BACKGROUND = 0, 
  GRAPHICS_NUM_TILE_LAYERS 
};

enum
{
  GRAPHICS_SPRITE_LAYER_PANELS = 0, 
  GRAPHICS_SPRITE_LAYER_WIDGETS, 
  GRAPHICS_SPRITE_LAYER_TEXT, 
  GRAPHICS_NUM_SPRITE_LAYERS 
};

#define GRAPHICS_OVERSCAN_WIDTH   400
#define GRAPHICS_OVERSCAN_HEIGHT  224

#define GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH   2048
#define GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT  1024

/* the maximum number of tiles is determined as follows:        */
/*   number of 8x8 background tiles (1 screen): 50 * 28 = 1400  */
/* overall, this is 1400                                        */
#define GRAPHICS_MAX_BACKGROUND_TILES 1400

/* the maximum number of sprites is determined as follows:  */
/*   number of 8x8  (1 screen): 50 * 28 = 1400              */
/* overall, this is 3 * 1400 = 4200                         */
#define GRAPHICS_MAX_PANELS_SPRITES   1400
#define GRAPHICS_MAX_WIDGETS_SPRITES  1400
#define GRAPHICS_MAX_TEXT_SPRITES     1400

/* z levels for each layer */
#define GRAPHICS_Z_LEVEL_FAR_PLANE  1.375f
#define GRAPHICS_Z_LEVEL_NEAR_PLANE 0.125f

#define GRAPHICS_Z_LEVEL_BACKGROUND 1.25f

/* the z levels for the sprites shouldn't matter, */
/* as the depth buffer should be reset and the    */
/* depth test should be off when we draw them.    */
#define GRAPHICS_Z_LEVEL_PANELS     0.75f
#define GRAPHICS_Z_LEVEL_WIDGETS    0.50f
#define GRAPHICS_Z_LEVEL_TEXT       0.25f

/* for the postprocessing, the z level of the fullscreen  */
/* quad is set to the midpoint of the near and far planes */
#define GRAPHICS_Z_LEVEL_FULL_SCREEN_QUAD 0.75f

/* sdl window, various sizes, etc */
extern SDL_Window* G_sdl_window;

extern int G_graphics_resolution;

extern int G_viewport_w;
extern int G_viewport_h;

/* intermediate set ids  */
extern GLuint G_texture_id_intermediate[GRAPHICS_NUM_INTERMEDIATE_SETS];
extern GLuint G_framebuffer_id_intermediate[GRAPHICS_NUM_INTERMEDIATE_SETS];
extern GLuint G_renderbuffer_id_intermediate[GRAPHICS_NUM_INTERMEDIATE_SETS];

/* tile & sprite vbo ids and data buffers */
extern GLuint G_vertex_array_id;

extern GLuint G_overscan_vertex_buffer_id[GRAPHICS_NUM_BUFFER_SETS];
extern GLuint G_overscan_tex_coord_buffer_id[GRAPHICS_NUM_BUFFER_SETS];
extern GLuint G_overscan_pal_coord_buffer_id[GRAPHICS_NUM_BUFFER_SETS];
extern GLuint G_overscan_index_buffer_id[GRAPHICS_NUM_BUFFER_SETS];

extern GLfloat*         G_overscan_vertex_buffer_data[GRAPHICS_NUM_BUFFER_SETS];
extern GLfloat*         G_overscan_tex_coord_buffer_data[GRAPHICS_NUM_BUFFER_SETS];
extern GLfloat*         G_overscan_pal_coord_buffer_data[GRAPHICS_NUM_BUFFER_SETS];
extern unsigned short*  G_overscan_index_buffer_data[GRAPHICS_NUM_BUFFER_SETS];

/* postprocessing vbo ids and data buffers */
extern GLuint G_postprocessing_vertex_buffer_id_overscan;
extern GLuint G_postprocessing_vertex_buffer_id_window;

extern GLuint G_postprocessing_tex_coord_buffer_id_overscan;

extern GLuint G_postprocessing_index_buffer_id_all;

extern GLfloat          G_postprocessing_vertex_buffer_data_overscan[12];
extern GLfloat          G_postprocessing_vertex_buffer_data_window[12];

extern GLfloat          G_postprocessing_tex_coord_buffer_data_overscan[8];

extern unsigned short   G_postprocessing_index_buffer_data_all[6];

/* mvp matrices */
extern GLfloat  G_mvp_matrix_overscan[16];
extern GLfloat  G_mvp_matrix_intermediate[16];
extern GLfloat  G_mvp_matrix_window[16];

/* tiles & sprites vbo index tables */
extern int G_tile_layer_index[GRAPHICS_NUM_TILE_LAYERS];
extern int G_tile_layer_count[GRAPHICS_NUM_TILE_LAYERS];
extern int G_tile_layer_max[GRAPHICS_NUM_TILE_LAYERS];

extern int G_sprite_layer_index[GRAPHICS_NUM_SPRITE_LAYERS];
extern int G_sprite_layer_count[GRAPHICS_NUM_SPRITE_LAYERS];
extern int G_sprite_layer_max[GRAPHICS_NUM_SPRITE_LAYERS];

/* function declarations */
short int graphics_create_opengl_objects();
short int graphics_destroy_opengl_objects();

short int graphics_set_graphics_resolution(int resolution);
short int graphics_read_desktop_dimensions();

short int graphics_set_window_size(int res);
short int graphics_increase_window_size();
short int graphics_decrease_window_size();

#endif

