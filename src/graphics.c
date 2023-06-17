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

/* vbo size variables */
int G_tile_layer_counts[GRAPHICS_NUM_TILE_LAYERS];
int G_sprite_layer_counts[GRAPHICS_NUM_SPRITE_LAYERS];

/* opengl vbo ids */
GLuint G_vertex_array_id;

GLuint G_vertex_buffer_id_tiles;
GLuint G_texture_coord_buffer_id_tiles;
GLuint G_lighting_and_palette_buffer_id_tiles;
GLuint G_index_buffer_id_tiles;

GLuint G_vertex_buffer_id_sprites;
GLuint G_texture_coord_buffer_id_sprites;
GLuint G_lighting_and_palette_buffer_id_sprites;
GLuint G_index_buffer_id_sprites;

/* postprocessing vbo ids */
GLuint G_vertex_buffer_id_postprocessing_overscan;
GLuint G_vertex_buffer_id_postprocessing_window;

GLuint G_texture_coord_buffer_id_postprocessing_overscan;

GLuint G_index_buffer_id_postprocessing_all;

/* opengl intermediate textures */
GLuint G_texture_id_intermediate_1;
GLuint G_texture_id_intermediate_2;

/* opengl framebuffer ids */
GLuint G_framebuffer_id_intermediate_1;
GLuint G_renderbuffer_id_intermediate_1;

GLuint G_framebuffer_id_intermediate_2;
GLuint G_renderbuffer_id_intermediate_2;

/* vbos and matrices */
GLfloat*          G_vertex_buffer_tiles;
GLfloat*          G_texture_coord_buffer_tiles;
GLfloat*          G_lighting_and_palette_buffer_tiles;
unsigned short*   G_index_buffer_tiles;

GLfloat*          G_vertex_buffer_sprites;
GLfloat*          G_texture_coord_buffer_sprites;
GLfloat*          G_lighting_and_palette_buffer_sprites;
unsigned short*   G_index_buffer_sprites;

GLfloat           G_vertex_buffer_postprocessing_overscan[12];
GLfloat           G_vertex_buffer_postprocessing_window[12];

GLfloat           G_texture_coord_buffer_postprocessing_overscan[8];

unsigned short    G_index_buffer_postprocessing_all[6];

GLfloat           G_mvp_matrix_overscan[16];
GLfloat           G_mvp_matrix_intermediate[16];
GLfloat           G_mvp_matrix_window[16];

/*******************************************************************************
** graphics_create_opengl_objects()
*******************************************************************************/
short int graphics_create_opengl_objects()
{
  int i;
  int j;

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

  /* create intermediate texture 1 */
  glGenTextures(1, &G_texture_id_intermediate_1);
  glBindTexture(GL_TEXTURE_2D, G_texture_id_intermediate_1);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH, 
                GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);

  /* create framebuffer object 1 */
  glGenFramebuffers(1, &G_framebuffer_id_intermediate_1);
  glBindFramebuffer(GL_FRAMEBUFFER, G_framebuffer_id_intermediate_1);

  glGenRenderbuffers(1, &G_renderbuffer_id_intermediate_1);
  glBindRenderbuffer(GL_RENDERBUFFER, G_renderbuffer_id_intermediate_1);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 
                        GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH, GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
                            GL_RENDERBUFFER, G_renderbuffer_id_intermediate_1);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, G_texture_id_intermediate_1, 0);
  glDrawBuffer(GL_COLOR_ATTACHMENT0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    fprintf(stdout, "Error: Framebuffer Object 1 is not complete!\n");
    return 1;
  }

  /* create intermediate texture 2 */
  glGenTextures(1, &G_texture_id_intermediate_2);
  glBindTexture(GL_TEXTURE_2D, G_texture_id_intermediate_2);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH, 
                GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);

  /* create framebuffer object 2 */
  glGenFramebuffers(1, &G_framebuffer_id_intermediate_2);
  glBindFramebuffer(GL_FRAMEBUFFER, G_framebuffer_id_intermediate_2);

  glGenRenderbuffers(1, &G_renderbuffer_id_intermediate_2);
  glBindRenderbuffer(GL_RENDERBUFFER, G_renderbuffer_id_intermediate_2);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 
                        GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH, GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
                            GL_RENDERBUFFER, G_renderbuffer_id_intermediate_2);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, G_texture_id_intermediate_2, 0);
  glDrawBuffer(GL_COLOR_ATTACHMENT0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    fprintf(stdout, "Error: Framebuffer Object 1 is not complete!\n");
    return 1;
  }

  /* orthographic projection matrix (overscan) */
  G_mvp_matrix_overscan[0]  = 2.0f / GRAPHICS_OVERSCAN_WIDTH;
  G_mvp_matrix_overscan[1]  = 0.0f;
  G_mvp_matrix_overscan[2]  = 0.0f;
  G_mvp_matrix_overscan[3]  = 0.0f;

  G_mvp_matrix_overscan[4]  = 0.0f;
  G_mvp_matrix_overscan[5]  = -2.0f / GRAPHICS_OVERSCAN_HEIGHT;
  G_mvp_matrix_overscan[6]  = 0.0f;
  G_mvp_matrix_overscan[7]  = 0.0f;

  G_mvp_matrix_overscan[8]  = 0.0f;
  G_mvp_matrix_overscan[9]  = 0.0f;
  G_mvp_matrix_overscan[10] = 1.6f; /* 2.0f / (1.375f - 0.125f); */
  G_mvp_matrix_overscan[11] = 0.0f;

  G_mvp_matrix_overscan[12] = -1.0f;
  G_mvp_matrix_overscan[13] = 1.0f;
  G_mvp_matrix_overscan[14] = -1.2f; /* -((1.375f + 0.125f) / (1.375f - 0.125f)) */
  G_mvp_matrix_overscan[15] = 1.0f;

  /* orthographic projection matrix (intermediate) */
  G_mvp_matrix_intermediate[0]  = 2.0f / GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH;
  G_mvp_matrix_intermediate[1]  = 0.0f;
  G_mvp_matrix_intermediate[2]  = 0.0f;
  G_mvp_matrix_intermediate[3]  = 0.0f;

  G_mvp_matrix_intermediate[4]  = 0.0f;
  G_mvp_matrix_intermediate[5]  = -2.0f / GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT;
  G_mvp_matrix_intermediate[6]  = 0.0f;
  G_mvp_matrix_intermediate[7]  = 0.0f;

  G_mvp_matrix_intermediate[8]  = 0.0f;
  G_mvp_matrix_intermediate[9]  = 0.0f;
  G_mvp_matrix_intermediate[10] = 1.6f; /* 2.0f / (1.375f - 0.125f); */
  G_mvp_matrix_intermediate[11] = 0.0f;

  G_mvp_matrix_intermediate[12] = -1.0f;
  G_mvp_matrix_intermediate[13] = 1.0f;
  G_mvp_matrix_intermediate[14] = -1.2f; /* -((1.375f + 0.125f) / (1.375f - 0.125f)) */
  G_mvp_matrix_intermediate[15] = 1.0f;

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

  /* create buffer objects */
  glGenBuffers(1, &G_vertex_buffer_id_tiles);
  glGenBuffers(1, &G_texture_coord_buffer_id_tiles);
  glGenBuffers(1, &G_lighting_and_palette_buffer_id_tiles);
  glGenBuffers(1, &G_index_buffer_id_tiles);

  glGenBuffers(1, &G_vertex_buffer_id_sprites);
  glGenBuffers(1, &G_texture_coord_buffer_id_sprites);
  glGenBuffers(1, &G_lighting_and_palette_buffer_id_sprites);
  glGenBuffers(1, &G_index_buffer_id_sprites);

  glGenBuffers(1, &G_vertex_buffer_id_postprocessing_overscan);
  glGenBuffers(1, &G_vertex_buffer_id_postprocessing_window);

  glGenBuffers(1, &G_texture_coord_buffer_id_postprocessing_overscan);

  glGenBuffers(1, &G_index_buffer_id_postprocessing_all);

  /* allocate buffers */
  G_vertex_buffer_tiles = malloc(sizeof(GLfloat) * 3 * 4 * GRAPHICS_MAX_TILES);
  G_texture_coord_buffer_tiles = malloc(sizeof(GLfloat) * 2 * 4 * GRAPHICS_MAX_TILES);
  G_lighting_and_palette_buffer_tiles = malloc(sizeof(GLfloat) * 2 * 4 * GRAPHICS_MAX_TILES);
  G_index_buffer_tiles = malloc(sizeof(unsigned short) * 6 * GRAPHICS_MAX_TILES);

  G_vertex_buffer_sprites = malloc(sizeof(GLfloat) * 3 * 4 * GRAPHICS_MAX_SPRITES);
  G_texture_coord_buffer_sprites = malloc(sizeof(GLfloat) * 2 * 4 * GRAPHICS_MAX_SPRITES);
  G_lighting_and_palette_buffer_sprites = malloc(sizeof(GLfloat) * 2 * 4 * GRAPHICS_MAX_SPRITES);
  G_index_buffer_sprites = malloc(sizeof(unsigned short) * 6 * GRAPHICS_MAX_SPRITES);

  /* initialize tile buffers */
  for (i = 0; i < GRAPHICS_MAX_TILES; i++)
  {
    for (j = 0; j < 12; j++)
      G_vertex_buffer_tiles[(i * 12) + j] = 0.0f;

    for (j = 0; j < 8; j++)
      G_texture_coord_buffer_tiles[(i * 8) + j] = 0.0f;

    for (j = 0; j < 8; j++)
      G_lighting_and_palette_buffer_tiles[(i * 8) + j] = 0.0f;

    for (j = 0; j < 6; j++)
      G_index_buffer_tiles[(i * 6) + j] = 0;
  }

  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_tiles);
  glBufferData( GL_ARRAY_BUFFER, GRAPHICS_MAX_TILES * 12 * sizeof(GLfloat),
                G_vertex_buffer_tiles, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_tiles);
  glBufferData( GL_ARRAY_BUFFER, GRAPHICS_MAX_TILES * 8 * sizeof(GLfloat),
                G_texture_coord_buffer_tiles, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, G_lighting_and_palette_buffer_id_tiles);
  glBufferData( GL_ARRAY_BUFFER, GRAPHICS_MAX_TILES * 8 * sizeof(GLfloat),
                G_lighting_and_palette_buffer_tiles, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_tiles);
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, GRAPHICS_MAX_TILES * 6 * sizeof(unsigned short),
                G_index_buffer_tiles, GL_STATIC_DRAW);

  /* initialize sprite buffers */
  for (i = 0; i < GRAPHICS_MAX_SPRITES; i++)
  {
    for (j = 0; j < 12; j++)
      G_vertex_buffer_sprites[(i * 12) + j] = 0.0f;

    for (j = 0; j < 8; j++)
      G_texture_coord_buffer_sprites[(i * 8) + j] = 0.0f;

    for (j = 0; j < 8; j++)
      G_lighting_and_palette_buffer_sprites[(i * 8) + j] = 0.0f;

    for (j = 0; j < 6; j++)
      G_index_buffer_sprites[(i * 6) + j] = 0;
  }

  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_sprites);
  glBufferData( GL_ARRAY_BUFFER, GRAPHICS_MAX_SPRITES * 12 * sizeof(GLfloat),
                G_vertex_buffer_sprites, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_sprites);
  glBufferData( GL_ARRAY_BUFFER, GRAPHICS_MAX_SPRITES * 8 * sizeof(GLfloat),
                G_texture_coord_buffer_sprites, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, G_lighting_and_palette_buffer_id_sprites);
  glBufferData( GL_ARRAY_BUFFER, GRAPHICS_MAX_SPRITES * 8 * sizeof(GLfloat),
                G_lighting_and_palette_buffer_sprites, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_sprites);
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, GRAPHICS_MAX_SPRITES * 6 * sizeof(unsigned short),
                G_index_buffer_sprites, GL_DYNAMIC_DRAW);

  /* initialize vbo counts */
  for (i = 0; i < GRAPHICS_NUM_TILE_LAYERS; i++)
    G_tile_layer_counts[i] = 0;

  for (i = 0; i < GRAPHICS_NUM_SPRITE_LAYERS; i++)
    G_sprite_layer_counts[i] = 0;

  /* set up postprocessing overscan vertex & texture coordinate buffers */
  G_vertex_buffer_postprocessing_overscan[0] = 0.0f;
  G_vertex_buffer_postprocessing_overscan[1] = 0.0f;
  G_vertex_buffer_postprocessing_overscan[2] = GRAPHICS_Z_LEVEL_FULL_SCREEN_QUAD;

  G_vertex_buffer_postprocessing_overscan[3] = (GLfloat) GRAPHICS_OVERSCAN_WIDTH;
  G_vertex_buffer_postprocessing_overscan[4] = 0.0f;
  G_vertex_buffer_postprocessing_overscan[5] = GRAPHICS_Z_LEVEL_FULL_SCREEN_QUAD;

  G_vertex_buffer_postprocessing_overscan[6] = 0.0f;
  G_vertex_buffer_postprocessing_overscan[7] = (GLfloat) GRAPHICS_OVERSCAN_HEIGHT;
  G_vertex_buffer_postprocessing_overscan[8] = GRAPHICS_Z_LEVEL_FULL_SCREEN_QUAD;

  G_vertex_buffer_postprocessing_overscan[9] = (GLfloat) GRAPHICS_OVERSCAN_WIDTH;
  G_vertex_buffer_postprocessing_overscan[10] = (GLfloat) GRAPHICS_OVERSCAN_HEIGHT;
  G_vertex_buffer_postprocessing_overscan[11] = GRAPHICS_Z_LEVEL_FULL_SCREEN_QUAD;

  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_postprocessing_overscan);
  glBufferData( GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), 
                G_vertex_buffer_postprocessing_overscan, GL_STATIC_DRAW);

  G_texture_coord_buffer_postprocessing_overscan[0] = 0.0f;
  G_texture_coord_buffer_postprocessing_overscan[1] = 1.0f;

  G_texture_coord_buffer_postprocessing_overscan[2] = (GLfloat) GRAPHICS_OVERSCAN_WIDTH / GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH;
  G_texture_coord_buffer_postprocessing_overscan[3] = 1.0f;

  G_texture_coord_buffer_postprocessing_overscan[4] = 0.0f;
  G_texture_coord_buffer_postprocessing_overscan[5] = 1.0f - (GLfloat) GRAPHICS_OVERSCAN_HEIGHT / GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT;

  G_texture_coord_buffer_postprocessing_overscan[6] = (GLfloat) GRAPHICS_OVERSCAN_WIDTH / GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH;
  G_texture_coord_buffer_postprocessing_overscan[7] = 1.0f - (GLfloat) GRAPHICS_OVERSCAN_HEIGHT / GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT;

  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_postprocessing_overscan);
  glBufferData( GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), 
                G_texture_coord_buffer_postprocessing_overscan, GL_STATIC_DRAW);

  /* set up postprocessing window vertex buffer */
  G_vertex_buffer_postprocessing_window[0] = 0.0f;
  G_vertex_buffer_postprocessing_window[1] = 0.0f;
  G_vertex_buffer_postprocessing_window[2] = GRAPHICS_Z_LEVEL_FULL_SCREEN_QUAD;

  G_vertex_buffer_postprocessing_window[3] = (GLfloat) S_window_w;
  G_vertex_buffer_postprocessing_window[4] = 0.0f;
  G_vertex_buffer_postprocessing_window[5] = GRAPHICS_Z_LEVEL_FULL_SCREEN_QUAD;

  G_vertex_buffer_postprocessing_window[6] = 0.0f;
  G_vertex_buffer_postprocessing_window[7] = (GLfloat) S_window_h;
  G_vertex_buffer_postprocessing_window[8] = GRAPHICS_Z_LEVEL_FULL_SCREEN_QUAD;

  G_vertex_buffer_postprocessing_window[9] = (GLfloat) S_window_w;
  G_vertex_buffer_postprocessing_window[10] = (GLfloat) S_window_h;
  G_vertex_buffer_postprocessing_window[11] = GRAPHICS_Z_LEVEL_FULL_SCREEN_QUAD;

  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_postprocessing_window);
  glBufferData( GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), 
                G_vertex_buffer_postprocessing_window, GL_STATIC_DRAW);

  /* set up postprocessing index buffer object */
  G_index_buffer_postprocessing_all[0] = 0;
  G_index_buffer_postprocessing_all[1] = 2;
  G_index_buffer_postprocessing_all[2] = 1;

  G_index_buffer_postprocessing_all[3] = 1;
  G_index_buffer_postprocessing_all[4] = 2;
  G_index_buffer_postprocessing_all[5] = 3;

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_postprocessing_all);
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned short), 
                G_index_buffer_postprocessing_all, GL_STATIC_DRAW);

  return 0;
}

/*******************************************************************************
** graphics_destroy_opengl_objects()
*******************************************************************************/
short int graphics_destroy_opengl_objects()
{
  /* delete buffer objects */
  glDeleteBuffers(1, &G_vertex_buffer_id_tiles);
  glDeleteBuffers(1, &G_texture_coord_buffer_id_tiles);
  glDeleteBuffers(1, &G_lighting_and_palette_buffer_id_tiles);
  glDeleteBuffers(1, &G_index_buffer_id_tiles);

  glDeleteBuffers(1, &G_vertex_buffer_id_sprites);
  glDeleteBuffers(1, &G_texture_coord_buffer_id_sprites);
  glDeleteBuffers(1, &G_lighting_and_palette_buffer_id_sprites);
  glDeleteBuffers(1, &G_index_buffer_id_sprites);

  glDeleteBuffers(1, &G_vertex_buffer_id_postprocessing_overscan);
  glDeleteBuffers(1, &G_vertex_buffer_id_postprocessing_window);

  glDeleteBuffers(1, &G_texture_coord_buffer_id_postprocessing_overscan);

  glDeleteBuffers(1, &G_index_buffer_id_postprocessing_all);

  /* delete framebuffer objects */
  glDeleteFramebuffers(1, &G_framebuffer_id_intermediate_1);
  glDeleteRenderbuffers(1, &G_renderbuffer_id_intermediate_1);

  glDeleteFramebuffers(1, &G_framebuffer_id_intermediate_2);
  glDeleteRenderbuffers(1, &G_renderbuffer_id_intermediate_2);

  /* delete intermediate textures */
  glDeleteTextures(1, &G_texture_id_intermediate_1);
  glDeleteTextures(1, &G_texture_id_intermediate_2);

  /* delete opengl shader programs */
  shaders_destroy_programs();

  /* delete vertex array object */
  glDeleteVertexArrays(1, &G_vertex_array_id);

  /* free tile buffers */
  if (G_vertex_buffer_tiles != NULL)
  {
    free(G_vertex_buffer_tiles);
    G_vertex_buffer_tiles = NULL;
  }

  if (G_texture_coord_buffer_tiles != NULL)
  {
    free(G_texture_coord_buffer_tiles);
    G_texture_coord_buffer_tiles = NULL;
  }

  if (G_lighting_and_palette_buffer_tiles != NULL)
  {
    free(G_lighting_and_palette_buffer_tiles);
    G_lighting_and_palette_buffer_tiles = NULL;
  }

  if (G_index_buffer_tiles != NULL)
  {
    free(G_index_buffer_tiles);
    G_index_buffer_tiles = NULL;
  }

  /* free sprite buffers */
  if (G_vertex_buffer_sprites != NULL)
  {
    free(G_vertex_buffer_sprites);
    G_vertex_buffer_sprites = NULL;
  }

  if (G_texture_coord_buffer_sprites != NULL)
  {
    free(G_texture_coord_buffer_sprites);
    G_texture_coord_buffer_sprites = NULL;
  }

  if (G_lighting_and_palette_buffer_sprites != NULL)
  {
    free(G_lighting_and_palette_buffer_sprites);
    G_lighting_and_palette_buffer_sprites = NULL;
  }

  if (G_index_buffer_sprites != NULL)
  {
    free(G_index_buffer_sprites);
    G_index_buffer_sprites = NULL;
  }

  return 0;
}

#if 0
/*******************************************************************************
** graphics_setup_overscan_mvp_matrix()
*******************************************************************************/
short int graphics_setup_overscan_mvp_matrix()
{
  /* orthographic projection matrix (overscan) */
  G_mvp_matrix_overscan[0]  = 2.0f / GRAPHICS_OVERSCAN_WIDTH;
  G_mvp_matrix_overscan[1]  = 0.0f;
  G_mvp_matrix_overscan[2]  = 0.0f;
  G_mvp_matrix_overscan[3]  = 0.0f;

  G_mvp_matrix_overscan[4]  = 0.0f;
  G_mvp_matrix_overscan[5]  = -2.0f / GRAPHICS_OVERSCAN_HEIGHT;
  G_mvp_matrix_overscan[6]  = 0.0f;
  G_mvp_matrix_overscan[7]  = 0.0f;

  G_mvp_matrix_overscan[8]  = 0.0f;
  G_mvp_matrix_overscan[9]  = 0.0f;
  G_mvp_matrix_overscan[10] = 1.6f; /* 2.0f / (1.375f - 0.125f); */
  G_mvp_matrix_overscan[11] = 0.0f;

  G_mvp_matrix_overscan[12] = -1.0f;
  G_mvp_matrix_overscan[13] = 1.0f;
  G_mvp_matrix_overscan[14] = -1.2f; /* -((1.375f + 0.125f) / (1.375f - 0.125f)) */
  G_mvp_matrix_overscan[15] = 1.0f;

  return 0;
}
#endif

/*******************************************************************************
** graphics_set_graphics_resolution()
*******************************************************************************/
short int graphics_set_graphics_resolution(int resolution)
{
  /* set resolution */
  if (resolution == GRAPHICS_RESOLUTION_640_480)
  {
    G_graphics_resolution = GRAPHICS_RESOLUTION_640_480;

    S_window_w = 640;
    S_window_h = 480;
  }
  else if (resolution == GRAPHICS_RESOLUTION_800_600)
  {
    G_graphics_resolution = GRAPHICS_RESOLUTION_800_600;

    S_window_w = 800;
    S_window_h = 600;
  }
  else if (resolution == GRAPHICS_RESOLUTION_960_720)
  {
    G_graphics_resolution = GRAPHICS_RESOLUTION_960_720;

    S_window_w = 960;
    S_window_h = 720;
  }
  else if (resolution == GRAPHICS_RESOLUTION_1024_768)
  {
    G_graphics_resolution = GRAPHICS_RESOLUTION_1024_768;

    S_window_w = 1024;
    S_window_h = 768;
  }
  else if (resolution == GRAPHICS_RESOLUTION_1280_960)
  {
    G_graphics_resolution = GRAPHICS_RESOLUTION_1280_960;

    S_window_w = 1280;
    S_window_h = 960;
  }
  else if (resolution == GRAPHICS_RESOLUTION_1440_1080)
  {
    G_graphics_resolution = GRAPHICS_RESOLUTION_1440_1080;

    S_window_w = 1440;
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
  G_vertex_buffer_postprocessing_window[0] = 0.0f;
  G_vertex_buffer_postprocessing_window[1] = 0.0f;
  G_vertex_buffer_postprocessing_window[2] = GRAPHICS_Z_LEVEL_FULL_SCREEN_QUAD;

  G_vertex_buffer_postprocessing_window[3] = (GLfloat) S_window_w;
  G_vertex_buffer_postprocessing_window[4] = 0.0f;
  G_vertex_buffer_postprocessing_window[5] = GRAPHICS_Z_LEVEL_FULL_SCREEN_QUAD;

  G_vertex_buffer_postprocessing_window[6] = 0.0f;
  G_vertex_buffer_postprocessing_window[7] = (GLfloat) S_window_h;
  G_vertex_buffer_postprocessing_window[8] = GRAPHICS_Z_LEVEL_FULL_SCREEN_QUAD;

  G_vertex_buffer_postprocessing_window[9] = (GLfloat) S_window_w;
  G_vertex_buffer_postprocessing_window[10] = (GLfloat) S_window_h;
  G_vertex_buffer_postprocessing_window[11] = GRAPHICS_Z_LEVEL_FULL_SCREEN_QUAD;

  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_postprocessing_window);
  glBufferData( GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), 
                G_vertex_buffer_postprocessing_window, GL_STATIC_DRAW);

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

