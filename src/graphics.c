/*******************************************************************************
** graphics.c (opengl objects)
*******************************************************************************/

#include <SDL2/SDL.h>
#include <glad/glad.h>

#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "graphics.h"
#include "path.h"
#include "shaders.h"

/* sdl window, various sizes, etc */
SDL_Window* G_sdl_window;

int G_graphics_resolution;

int G_viewport_w;
int G_viewport_h;

static int S_window_w;
static int S_window_h;

/* intermediate set ids  */
GLuint  G_texture_id_intermediate[GRAPHICS_NUM_INTERMEDIATE_SETS];
GLuint  G_framebuffer_id_intermediate[GRAPHICS_NUM_INTERMEDIATE_SETS];
GLuint  G_renderbuffer_id_intermediate[GRAPHICS_NUM_INTERMEDIATE_SETS];

/* tile & sprite vbo ids and data buffers */
GLuint  G_vertex_array_id;

GLuint  G_overscan_vertex_buffer_id[GRAPHICS_NUM_BUFFER_SETS];
GLuint  G_overscan_tex_coord_buffer_id[GRAPHICS_NUM_BUFFER_SETS];
GLuint  G_overscan_pal_coord_buffer_id[GRAPHICS_NUM_BUFFER_SETS];
GLuint  G_overscan_index_buffer_id[GRAPHICS_NUM_BUFFER_SETS];

GLfloat*        G_overscan_vertex_buffer_data[GRAPHICS_NUM_BUFFER_SETS];
GLfloat*        G_overscan_tex_coord_buffer_data[GRAPHICS_NUM_BUFFER_SETS];
GLfloat*        G_overscan_pal_coord_buffer_data[GRAPHICS_NUM_BUFFER_SETS];
unsigned short* G_overscan_index_buffer_data[GRAPHICS_NUM_BUFFER_SETS];

/* postprocessing vbo ids and data buffers */
GLuint  G_postprocessing_vertex_buffer_id_overscan;
GLuint  G_postprocessing_vertex_buffer_id_window;

GLuint  G_postprocessing_tex_coord_buffer_id_overscan;

GLuint  G_postprocessing_index_buffer_id_all;

GLfloat         G_postprocessing_vertex_buffer_data_overscan[12];
GLfloat         G_postprocessing_vertex_buffer_data_window[12];

GLfloat         G_postprocessing_tex_coord_buffer_data_overscan[8];

unsigned short  G_postprocessing_index_buffer_data_all[6];

/* mvp matrices */
GLfloat  G_mvp_matrix_overscan[16];
GLfloat  G_mvp_matrix_intermediate[16];
GLfloat  G_mvp_matrix_window[16];

/* tiles & sprites vbo index tables */
int G_tile_layer_index[GRAPHICS_NUM_TILE_LAYERS];
int G_tile_layer_count[GRAPHICS_NUM_TILE_LAYERS];
int G_tile_layer_max[GRAPHICS_NUM_TILE_LAYERS];

int G_sprite_layer_index[GRAPHICS_NUM_SPRITE_LAYERS];
int G_sprite_layer_count[GRAPHICS_NUM_SPRITE_LAYERS];
int G_sprite_layer_max[GRAPHICS_NUM_SPRITE_LAYERS];

/* buffer max sizes */
static int S_graphics_buffer_set_size[GRAPHICS_NUM_BUFFER_SETS];

/*******************************************************************************
** graphics_setup_mvp_matrix_ortho()
*******************************************************************************/
short int graphics_setup_mvp_matrix_ortho(GLfloat* matrix, 
                                          int t_x, int t_y, 
                                          int width, int height, 
                                          float near, float far)
{
  matrix[0]  = 2.0f / width;
  matrix[1]  = 0.0f;
  matrix[2]  = 0.0f;
  matrix[3]  = 0.0f;

  matrix[4]  = 0.0f;
  matrix[5]  = -2.0f / height;
  matrix[6]  = 0.0f;
  matrix[7]  = 0.0f;

  matrix[8]  = 0.0f;
  matrix[9]  = 0.0f;
  matrix[10] = 2.0f / (far - near);
  matrix[11] = 0.0f;

  matrix[12] = ((2.0f * t_x) / width) - 1.0f;
  matrix[13] = ((-2.0f * t_y) / height) + 1.0f;
  matrix[14] = -((far + near) / (far - near));
  matrix[15] = 1.0f;

  return 0;
}

/*******************************************************************************
** graphics_create_opengl_objects()
*******************************************************************************/
short int graphics_create_opengl_objects()
{
  int m;
  int n;

  int index;

  GLenum usage;

  /* create vertex array object */
  glGenVertexArrays(1, &G_vertex_array_id);
  glBindVertexArray(G_vertex_array_id);

  /* create shader programs */
  if (shaders_create_programs())
  {
    fprintf(stdout, "Failed to create GLSL shader programs.\n");
    glDeleteVertexArrays(1, &G_vertex_array_id);
    return 1;
  }

  /* create intermediate set textures & framebuffer objects */
  for (m = 0; m < GRAPHICS_NUM_INTERMEDIATE_SETS; m++)
  {
    /* create texture */
    glGenTextures(1, &G_texture_id_intermediate[m]);
    glBindTexture(GL_TEXTURE_2D, G_texture_id_intermediate[m]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH, 
                  GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);

    /* create framebuffer and renderbuffer */
    glGenFramebuffers(1, &G_framebuffer_id_intermediate[m]);
    glBindFramebuffer(GL_FRAMEBUFFER, G_framebuffer_id_intermediate[m]);

    glGenRenderbuffers(1, &G_renderbuffer_id_intermediate[m]);
    glBindRenderbuffer(GL_RENDERBUFFER, G_renderbuffer_id_intermediate[m]);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 
                          GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH, GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
                              GL_RENDERBUFFER, G_renderbuffer_id_intermediate[m]);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, G_texture_id_intermediate[m], 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
      fprintf(stdout, "Error: Framebuffer Object is not complete!\n");
      return 1;
    }
  }

  /* set up orthographic projection matrices */
  graphics_setup_mvp_matrix_ortho(&G_mvp_matrix_overscan[0], 
                                  0, 0, 
                                  GRAPHICS_OVERSCAN_WIDTH, 
                                  GRAPHICS_OVERSCAN_HEIGHT, 
                                  GRAPHICS_Z_LEVEL_NEAR_PLANE, 
                                  GRAPHICS_Z_LEVEL_FAR_PLANE);

  graphics_setup_mvp_matrix_ortho(&G_mvp_matrix_intermediate[0], 
                                  0, 0, 
                                  GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH, 
                                  GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT, 
                                  GRAPHICS_Z_LEVEL_NEAR_PLANE, 
                                  GRAPHICS_Z_LEVEL_FAR_PLANE);

  graphics_setup_mvp_matrix_ortho(&G_mvp_matrix_window[0], 
                                  0, 0, 
                                  S_window_w, 
                                  S_window_h, 
                                  GRAPHICS_Z_LEVEL_NEAR_PLANE, 
                                  GRAPHICS_Z_LEVEL_FAR_PLANE);

  /* set up tile layer vbo index tables */
  index = 0;

  for (m = 0; m < GRAPHICS_NUM_TILE_LAYERS; m++)
  {
    if (m == GRAPHICS_TILE_LAYER_BACKGROUND)
      G_tile_layer_max[m] = GRAPHICS_MAX_BACKGROUND_TILES;
    else
      G_tile_layer_max[m] = 0;

    G_tile_layer_index[m] = index;
    G_tile_layer_count[m] = 0;

    index += G_tile_layer_max[m];
  }

  S_graphics_buffer_set_size[GRAPHICS_BUFFER_SET_TILES] = index;

  /* set up sprite layer vbo index tables */
  index = 0;

  for (m = 0; m < GRAPHICS_NUM_SPRITE_LAYERS; m++)
  {
    if (m == GRAPHICS_SPRITE_LAYER_PANELS)
      G_sprite_layer_max[m] = GRAPHICS_MAX_PANELS_SPRITES;
    else if (m == GRAPHICS_SPRITE_LAYER_WIDGETS)
      G_sprite_layer_max[m] = GRAPHICS_MAX_WIDGETS_SPRITES;
    else if (m == GRAPHICS_SPRITE_LAYER_TEXT)
      G_sprite_layer_max[m] = GRAPHICS_MAX_TEXT_SPRITES;
    else
      G_sprite_layer_max[m] = 0;

    G_sprite_layer_index[m] = index;
    G_sprite_layer_count[m] = 0;

    index += G_sprite_layer_max[m];
  }

  S_graphics_buffer_set_size[GRAPHICS_BUFFER_SET_SPRITES] = index;

  /* create tile & sprite buffer objects */

  /* initialize buffer pointers */
  for (m = 0; m < GRAPHICS_NUM_BUFFER_SETS; m++)
  {
    G_overscan_vertex_buffer_data[m] = NULL;
    G_overscan_tex_coord_buffer_data[m] = NULL;
    G_overscan_pal_coord_buffer_data[m] = NULL;
    G_overscan_index_buffer_data[m] = NULL;
  }

  /* generate buffers */
  for (m = 0; m < GRAPHICS_NUM_BUFFER_SETS; m++)
  {
    glGenBuffers(1, &G_overscan_vertex_buffer_id[m]);
    glGenBuffers(1, &G_overscan_tex_coord_buffer_id[m]);
    glGenBuffers(1, &G_overscan_pal_coord_buffer_id[m]);
    glGenBuffers(1, &G_overscan_index_buffer_id[m]);
  }

  /* allocate buffers */
  for (m = 0; m < GRAPHICS_NUM_BUFFER_SETS; m++)
  {
    G_overscan_vertex_buffer_data[m] = 
      malloc(sizeof(GLfloat) * 3 * 4 * S_graphics_buffer_set_size[m]);

    if (G_overscan_vertex_buffer_data[m] == NULL)
      return 1;

    G_overscan_tex_coord_buffer_data[m] = 
      malloc(sizeof(GLfloat) * 2 * 4 * S_graphics_buffer_set_size[m]);

    if (G_overscan_tex_coord_buffer_data[m] == NULL)
      return 1;

    G_overscan_pal_coord_buffer_data[m] = 
      malloc(sizeof(GLfloat) * 2 * 4 * S_graphics_buffer_set_size[m]);

    if (G_overscan_pal_coord_buffer_data[m] == NULL)
      return 1;

    G_overscan_index_buffer_data[m] = 
      malloc(sizeof(unsigned short) * 6 * S_graphics_buffer_set_size[m]);

    if (G_overscan_index_buffer_data[m] == NULL)
      return 1;
  }

  /* initialize buffers */
  for (m = 0; m < GRAPHICS_NUM_BUFFER_SETS; m++)
  {
    for (n = 0; n < 3 * 4 * S_graphics_buffer_set_size[m]; n++)
      G_overscan_vertex_buffer_data[m][n] = 0.0f;

    for (n = 0; n < 2 * 4 * S_graphics_buffer_set_size[m]; n++)
      G_overscan_tex_coord_buffer_data[m][n] = 0.0f;

    for (n = 0; n < 2 * 4 * S_graphics_buffer_set_size[m]; n++)
      G_overscan_pal_coord_buffer_data[m][n] = 0.0f;

    for (n = 0; n < 6 * S_graphics_buffer_set_size[m]; n++)
      G_overscan_index_buffer_data[m][n] = 0;
  }

  /* send buffer data to vbos */
  for (m = 0; m < GRAPHICS_NUM_BUFFER_SETS; m++)
  {
    if (m == GRAPHICS_BUFFER_SET_TILES)
      usage = GL_STATIC_DRAW;
    else if (m == GRAPHICS_BUFFER_SET_SPRITES)
      usage = GL_DYNAMIC_DRAW;
    else
      usage = GL_DYNAMIC_DRAW;

    glBindBuffer(GL_ARRAY_BUFFER, G_overscan_vertex_buffer_id[m]);
    glBufferData( GL_ARRAY_BUFFER, 
                  S_graphics_buffer_set_size[m] * 12 * sizeof(GLfloat),
                  G_overscan_vertex_buffer_data[m], usage);

    glBindBuffer(GL_ARRAY_BUFFER, G_overscan_tex_coord_buffer_id[m]);
    glBufferData( GL_ARRAY_BUFFER, 
                  S_graphics_buffer_set_size[m] * 8 * sizeof(GLfloat),
                  G_overscan_tex_coord_buffer_data[m], usage);

    glBindBuffer(GL_ARRAY_BUFFER, G_overscan_pal_coord_buffer_id[m]);
    glBufferData( GL_ARRAY_BUFFER, 
                  S_graphics_buffer_set_size[m] * 8 * sizeof(GLfloat),
                  G_overscan_pal_coord_buffer_data[m], usage);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_overscan_index_buffer_id[m]);
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 
                  S_graphics_buffer_set_size[m] * 6 * sizeof(unsigned short),
                  G_overscan_index_buffer_data[m], usage);
  }

  /* create postprocessing buffer objects */
  glGenBuffers(1, &G_postprocessing_vertex_buffer_id_overscan);
  glGenBuffers(1, &G_postprocessing_vertex_buffer_id_window);

  glGenBuffers(1, &G_postprocessing_tex_coord_buffer_id_overscan);

  glGenBuffers(1, &G_postprocessing_index_buffer_id_all);

  /* set up postprocessing overscan vertex & texture coordinate buffers */
  G_postprocessing_vertex_buffer_data_overscan[0] = 0.0f;
  G_postprocessing_vertex_buffer_data_overscan[1] = 0.0f;
  G_postprocessing_vertex_buffer_data_overscan[2] = GRAPHICS_Z_LEVEL_FULL_SCREEN_QUAD;

  G_postprocessing_vertex_buffer_data_overscan[3] = (GLfloat) GRAPHICS_OVERSCAN_WIDTH;
  G_postprocessing_vertex_buffer_data_overscan[4] = 0.0f;
  G_postprocessing_vertex_buffer_data_overscan[5] = GRAPHICS_Z_LEVEL_FULL_SCREEN_QUAD;

  G_postprocessing_vertex_buffer_data_overscan[6] = 0.0f;
  G_postprocessing_vertex_buffer_data_overscan[7] = (GLfloat) GRAPHICS_OVERSCAN_HEIGHT;
  G_postprocessing_vertex_buffer_data_overscan[8] = GRAPHICS_Z_LEVEL_FULL_SCREEN_QUAD;

  G_postprocessing_vertex_buffer_data_overscan[9] = (GLfloat) GRAPHICS_OVERSCAN_WIDTH;
  G_postprocessing_vertex_buffer_data_overscan[10] = (GLfloat) GRAPHICS_OVERSCAN_HEIGHT;
  G_postprocessing_vertex_buffer_data_overscan[11] = GRAPHICS_Z_LEVEL_FULL_SCREEN_QUAD;

  glBindBuffer(GL_ARRAY_BUFFER, G_postprocessing_vertex_buffer_id_overscan);
  glBufferData( GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), 
                G_postprocessing_vertex_buffer_data_overscan, GL_STATIC_DRAW);

  G_postprocessing_tex_coord_buffer_data_overscan[0] = 0.0f;
  G_postprocessing_tex_coord_buffer_data_overscan[1] = 1.0f;

  G_postprocessing_tex_coord_buffer_data_overscan[2] = (GLfloat) GRAPHICS_OVERSCAN_WIDTH / GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH;
  G_postprocessing_tex_coord_buffer_data_overscan[3] = 1.0f;

  G_postprocessing_tex_coord_buffer_data_overscan[4] = 0.0f;
  G_postprocessing_tex_coord_buffer_data_overscan[5] = 1.0f - (GLfloat) GRAPHICS_OVERSCAN_HEIGHT / GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT;

  G_postprocessing_tex_coord_buffer_data_overscan[6] = (GLfloat) GRAPHICS_OVERSCAN_WIDTH / GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH;
  G_postprocessing_tex_coord_buffer_data_overscan[7] = 1.0f - (GLfloat) GRAPHICS_OVERSCAN_HEIGHT / GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT;

  glBindBuffer(GL_ARRAY_BUFFER, G_postprocessing_tex_coord_buffer_id_overscan);
  glBufferData( GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), 
                G_postprocessing_tex_coord_buffer_data_overscan, GL_STATIC_DRAW);

  /* set up postprocessing window vertex buffer */
  G_postprocessing_vertex_buffer_data_window[0] = 0.0f;
  G_postprocessing_vertex_buffer_data_window[1] = 0.0f;
  G_postprocessing_vertex_buffer_data_window[2] = GRAPHICS_Z_LEVEL_FULL_SCREEN_QUAD;

  G_postprocessing_vertex_buffer_data_window[3] = (GLfloat) S_window_w;
  G_postprocessing_vertex_buffer_data_window[4] = 0.0f;
  G_postprocessing_vertex_buffer_data_window[5] = GRAPHICS_Z_LEVEL_FULL_SCREEN_QUAD;

  G_postprocessing_vertex_buffer_data_window[6] = 0.0f;
  G_postprocessing_vertex_buffer_data_window[7] = (GLfloat) S_window_h;
  G_postprocessing_vertex_buffer_data_window[8] = GRAPHICS_Z_LEVEL_FULL_SCREEN_QUAD;

  G_postprocessing_vertex_buffer_data_window[9] = (GLfloat) S_window_w;
  G_postprocessing_vertex_buffer_data_window[10] = (GLfloat) S_window_h;
  G_postprocessing_vertex_buffer_data_window[11] = GRAPHICS_Z_LEVEL_FULL_SCREEN_QUAD;

  glBindBuffer(GL_ARRAY_BUFFER, G_postprocessing_vertex_buffer_id_window);
  glBufferData( GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), 
                G_postprocessing_vertex_buffer_data_window, GL_STATIC_DRAW);

  /* set up postprocessing index buffer object */
  G_postprocessing_index_buffer_data_all[0] = 0;
  G_postprocessing_index_buffer_data_all[1] = 2;
  G_postprocessing_index_buffer_data_all[2] = 1;

  G_postprocessing_index_buffer_data_all[3] = 1;
  G_postprocessing_index_buffer_data_all[4] = 2;
  G_postprocessing_index_buffer_data_all[5] = 3;

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_postprocessing_index_buffer_id_all);
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned short), 
                G_postprocessing_index_buffer_data_all, GL_STATIC_DRAW);

  return 0;
}

/*******************************************************************************
** graphics_destroy_opengl_objects()
*******************************************************************************/
short int graphics_destroy_opengl_objects()
{
  int m;

  /* delete tile & sprite buffer objects */
  for (m = 0; m < GRAPHICS_NUM_BUFFER_SETS; m++)
  {
    glDeleteBuffers(1, &G_overscan_vertex_buffer_id[m]);
    glDeleteBuffers(1, &G_overscan_tex_coord_buffer_id[m]);
    glDeleteBuffers(1, &G_overscan_pal_coord_buffer_id[m]);
    glDeleteBuffers(1, &G_overscan_index_buffer_id[m]);
  }

  /* delete postprocessing buffer objects */
  glDeleteBuffers(1, &G_postprocessing_vertex_buffer_id_overscan);
  glDeleteBuffers(1, &G_postprocessing_vertex_buffer_id_window);

  glDeleteBuffers(1, &G_postprocessing_tex_coord_buffer_id_overscan);

  glDeleteBuffers(1, &G_postprocessing_index_buffer_id_all);

  /* delete intermediate set textures & framebuffer objects */
  for (m = 0; m < GRAPHICS_NUM_INTERMEDIATE_SETS; m++)
  {
    glDeleteFramebuffers(1, &G_framebuffer_id_intermediate[m]);
    glDeleteRenderbuffers(1, &G_renderbuffer_id_intermediate[m]);
    glDeleteTextures(1, &G_texture_id_intermediate[m]);
  }

  /* delete opengl shader programs */
  shaders_destroy_programs();

  /* delete vertex array object */
  glDeleteVertexArrays(1, &G_vertex_array_id);

  /* free tile & sprite buffers */
  for (m = 0; m < GRAPHICS_NUM_BUFFER_SETS; m++)
  {
    if (G_overscan_vertex_buffer_data[m] != NULL)
    {
      free(G_overscan_vertex_buffer_data[m]);
      G_overscan_vertex_buffer_data[m] = NULL;
    }

    if (G_overscan_tex_coord_buffer_data[m] != NULL)
    {
      free(G_overscan_tex_coord_buffer_data[m]);
      G_overscan_tex_coord_buffer_data[m] = NULL;
    }

    if (G_overscan_pal_coord_buffer_data[m] != NULL)
    {
      free(G_overscan_pal_coord_buffer_data[m]);
      G_overscan_pal_coord_buffer_data[m] = NULL;
    }

    if (G_overscan_index_buffer_data[m] != NULL)
    {
      free(G_overscan_index_buffer_data[m]);
      G_overscan_index_buffer_data[m] = NULL;
    }
  }

  return 0;
}

/*******************************************************************************
** graphics_set_graphics_resolution()
*******************************************************************************/
short int graphics_set_graphics_resolution(int resolution)
{
  /* set resolution */
  if (resolution == GRAPHICS_RESOLUTION_480P)
  {
    G_graphics_resolution = GRAPHICS_RESOLUTION_480P;

    S_window_w = 854;
    S_window_h = 480;
  }
  else if (resolution == GRAPHICS_RESOLUTION_600P)
  {
    G_graphics_resolution = GRAPHICS_RESOLUTION_600P;

    S_window_w = 1067;
    S_window_h = 600;
  }
  else if (resolution == GRAPHICS_RESOLUTION_720P)
  {
    G_graphics_resolution = GRAPHICS_RESOLUTION_720P;

    S_window_w = 1280;
    S_window_h = 720;
  }
  else if (resolution == GRAPHICS_RESOLUTION_768P)
  {
    G_graphics_resolution = GRAPHICS_RESOLUTION_768P;

    S_window_w = 1366;
    S_window_h = 768;
  }
  else if (resolution == GRAPHICS_RESOLUTION_1080P)
  {
    G_graphics_resolution = GRAPHICS_RESOLUTION_1080P;

    S_window_w = 1920;
    S_window_h = 1080;
  }

  return 0;
}

/*******************************************************************************
** graphics_read_desktop_dimensions()
*******************************************************************************/
short int graphics_read_desktop_dimensions()
{
  int             index;
  SDL_DisplayMode mode;

  /* determine display resolution */
  index = SDL_GetWindowDisplayIndex(G_sdl_window);
  SDL_GetDesktopDisplayMode(index, &mode);

  /* determine if the window will fit on-screen */
  if ((mode.w < S_window_w) || (mode.h < S_window_h))
    return 1;

  return 0;
}

/*******************************************************************************
** graphics_setup_viewport_windowed()
*******************************************************************************/
short int graphics_setup_viewport_windowed()
{
  /* orthographic projection matrix (window) */
  G_mvp_matrix_window[0]  = 2.0f / S_window_w;
  G_mvp_matrix_window[1]  = 0.0f;
  G_mvp_matrix_window[2]  = 0.0f;
  G_mvp_matrix_window[3]  = 0.0f;

  G_mvp_matrix_window[4]  = 0.0f;
  G_mvp_matrix_window[5]  = -2.0f / S_window_h;
  G_mvp_matrix_window[6]  = 0.0f;
  G_mvp_matrix_window[7]  = 0.0f;

  G_mvp_matrix_window[8]  = 0.0f;
  G_mvp_matrix_window[9]  = 0.0f;
  G_mvp_matrix_window[10] = 1.6f; /* 2.0f / (1.375f - 0.125f); */
  G_mvp_matrix_window[11] = 0.0f;

  G_mvp_matrix_window[12] = -1.0f;
  G_mvp_matrix_window[13] = 1.0f;
  G_mvp_matrix_window[14] = -1.2f; /* -((1.375f + 0.125f) / (1.375f - 0.125f)) */
  G_mvp_matrix_window[15] = 1.0f;

  /* set up postprocessing window vertex buffer object */
  G_postprocessing_vertex_buffer_data_window[0] = 0.0f;
  G_postprocessing_vertex_buffer_data_window[1] = 0.0f;
  G_postprocessing_vertex_buffer_data_window[2] = GRAPHICS_Z_LEVEL_FULL_SCREEN_QUAD;

  G_postprocessing_vertex_buffer_data_window[3] = (GLfloat) S_window_w;
  G_postprocessing_vertex_buffer_data_window[4] = 0.0f;
  G_postprocessing_vertex_buffer_data_window[5] = GRAPHICS_Z_LEVEL_FULL_SCREEN_QUAD;

  G_postprocessing_vertex_buffer_data_window[6] = 0.0f;
  G_postprocessing_vertex_buffer_data_window[7] = (GLfloat) S_window_h;
  G_postprocessing_vertex_buffer_data_window[8] = GRAPHICS_Z_LEVEL_FULL_SCREEN_QUAD;

  G_postprocessing_vertex_buffer_data_window[9] = (GLfloat) S_window_w;
  G_postprocessing_vertex_buffer_data_window[10] = (GLfloat) S_window_h;
  G_postprocessing_vertex_buffer_data_window[11] = GRAPHICS_Z_LEVEL_FULL_SCREEN_QUAD;

  glBindBuffer(GL_ARRAY_BUFFER, G_postprocessing_vertex_buffer_id_window);
  glBufferData( GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), 
                G_postprocessing_vertex_buffer_data_window, GL_STATIC_DRAW);

  /* set up viewport width and height */
  G_viewport_w = S_window_w;
  G_viewport_h = S_window_h;

  return 0;
}

/*******************************************************************************
** graphics_set_window_size()
*******************************************************************************/
short int graphics_set_window_size(int res)
{
  int old_res;

  /* make sure the resolution is valid */
  if ((res < 0) && (res >= GRAPHICS_NUM_RESOLUTIONS))
    return 0;

  /* note: we still want to run this function even if we are    */
  /* already in this resolution, because this function is also  */
  /* used at startup for initializing the default resolution    */

  /* store old resolution */
  old_res = G_graphics_resolution;

  /* try to set the new resolution */
  graphics_set_graphics_resolution(res);

  /* make sure the new window size will fit on the desktop */
  if (graphics_read_desktop_dimensions() != 0)
  {
    graphics_set_graphics_resolution(old_res);
    return 1;
  }

  /* resize the window if not in fullscreen */
  SDL_SetWindowSize(G_sdl_window, S_window_w, S_window_h);
  SDL_SetWindowPosition(G_sdl_window, 
                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

  /* setup viewport */
  graphics_setup_viewport_windowed();

  return 0;
}

/*******************************************************************************
** graphics_increase_window_size()
*******************************************************************************/
short int graphics_increase_window_size()
{
  if (G_graphics_resolution < GRAPHICS_NUM_RESOLUTIONS - 1)
  {
    graphics_set_window_size(G_graphics_resolution + 1);
  }

  return 0;
}

/*******************************************************************************
** graphics_decrease_window_size()
*******************************************************************************/
short int graphics_decrease_window_size()
{
  if (G_graphics_resolution > 0)
  {
    graphics_set_window_size(G_graphics_resolution - 1);
  }

  return 0;
}

