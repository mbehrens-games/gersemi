/*******************************************************************************
** render.c (opengl rendering)
*******************************************************************************/

#include <glad/glad.h>

#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "graphics.h"
#include "palette.h"
#include "render.h"
#include "shaders.h"
#include "texture.h"

/* set opengl settings */
#define RENDER_TILES_OPENGL_SETTINGS()                                         \
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);                                        \
  glClearDepth(1.0f);                                                          \
                                                                               \
  glDepthFunc(GL_LEQUAL);                                                      \
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);                           \
  glBlendEquation(GL_FUNC_ADD);                                                \
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);                             \
                                                                               \
  glFrontFace(GL_CCW);                                                         \
  glCullFace(GL_BACK);                                                         \
                                                                               \
  glEnable(GL_DEPTH_TEST);                                                     \
  glDisable(GL_BLEND);                                                         \
  glEnable(GL_CULL_FACE);

#define RENDER_SPRITES_OPENGL_SETTINGS()                                       \
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);                                        \
  glClearDepth(1.0f);                                                          \
                                                                               \
  glDepthFunc(GL_LEQUAL);                                                      \
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);                           \
  glBlendEquation(GL_FUNC_ADD);                                                \
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);                             \
                                                                               \
  glFrontFace(GL_CCW);                                                         \
  glCullFace(GL_BACK);                                                         \
                                                                               \
  glEnable(GL_DEPTH_TEST);                                                     \
  glEnable(GL_BLEND);                                                          \
  glDisable(GL_CULL_FACE);

#define RENDER_POSTPROCESSING_OPENGL_SETTINGS()                                \
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);                                        \
  glClearDepth(1.0f);                                                          \
                                                                               \
  glDepthFunc(GL_LEQUAL);                                                      \
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);                           \
  glBlendEquation(GL_FUNC_ADD);                                                \
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);                             \
                                                                               \
  glFrontFace(GL_CCW);                                                         \
  glCullFace(GL_BACK);                                                         \
                                                                               \
  glDisable(GL_DEPTH_TEST);                                                    \
  glDisable(GL_BLEND);                                                         \
  glEnable(GL_CULL_FACE);

/* framebuffer setup */
#define RENDER_SETUP_OVERSCAN_OUTPUT(set)                                         \
  glBindFramebuffer(GL_FRAMEBUFFER, G_framebuffer_id_intermediate[set]);          \
  glViewport( 0, GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT - GRAPHICS_OVERSCAN_HEIGHT, \
              (GLsizei) GRAPHICS_OVERSCAN_WIDTH,                                  \
              (GLsizei) GRAPHICS_OVERSCAN_HEIGHT);

#define RENDER_SETUP_AND_CLEAR_OVERSCAN_OUTPUT(set)                            \
  RENDER_SETUP_OVERSCAN_OUTPUT(set)                                            \
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#define RENDER_SETUP_AND_RESET_DEPTH_OVERSCAN_OUTPUT(set)                      \
  RENDER_SETUP_OVERSCAN_OUTPUT(set)                                            \
  glClear(GL_DEPTH_BUFFER_BIT);

#define RENDER_SETUP_INTERMEDIATE_TEXTURE_OUTPUT(set)                          \
  glBindFramebuffer(GL_FRAMEBUFFER, G_framebuffer_id_intermediate[set]);       \
  glViewport(0, 0,  (GLsizei) GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH,             \
                    (GLsizei) GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT);           \

#define RENDER_SETUP_AND_CLEAR_INTERMEDIATE_TEXTURE_OUTPUT(set)                \
  RENDER_SETUP_INTERMEDIATE_TEXTURE_OUTPUT(set)                                \
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#define RENDER_SETUP_AND_RESET_DEPTH_INTERMEDIATE_TEXTURE_OUTPUT(set)          \
  RENDER_SETUP_INTERMEDIATE_TEXTURE_OUTPUT(set)                                \
  glClear(GL_DEPTH_BUFFER_BIT);

#define RENDER_SETUP_AND_CLEAR_WINDOW_OUTPUT()                                 \
  glBindFramebuffer(GL_FRAMEBUFFER, 0);                                        \
  glViewport(0, 0, (GLsizei) G_viewport_w, (GLsizei) G_viewport_h);            \
  glClear(GL_COLOR_BUFFER_BIT);

/* tiles */
#define RENDER_BEGIN_TILE_RENDERING()                                                       \
  glEnableVertexAttribArray(0);                                                             \
  glBindBuffer(GL_ARRAY_BUFFER, G_overscan_vertex_buffer_id[GRAPHICS_BUFFER_SET_TILES]);    \
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);                                 \
                                                                                            \
  glEnableVertexAttribArray(1);                                                             \
  glBindBuffer(GL_ARRAY_BUFFER, G_overscan_tex_coord_buffer_id[GRAPHICS_BUFFER_SET_TILES]); \
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);                                 \
                                                                                            \
  glEnableVertexAttribArray(2);                                                             \
  glBindBuffer(GL_ARRAY_BUFFER, G_overscan_pal_coord_buffer_id[GRAPHICS_BUFFER_SET_TILES]); \
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);

#define RENDER_END_TILE_RENDERING()                                            \
  glDisableVertexAttribArray(0);                                               \
  glDisableVertexAttribArray(1);                                               \
  glDisableVertexAttribArray(2);

#define RENDER_DRAW_BACKGROUND()                                                                \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_overscan_index_buffer_id[GRAPHICS_BUFFER_SET_TILES]); \
  glDrawElements( GL_TRIANGLES,                                                                 \
                  6 * G_tile_layer_count[GRAPHICS_TILE_LAYER_BACKGROUND],                       \
                  GL_UNSIGNED_SHORT,                                                            \
                  (void *) (sizeof(unsigned short) * 6 * G_tile_layer_index[GRAPHICS_TILE_LAYER_BACKGROUND]));

/* sprites */
#define RENDER_BEGIN_SPRITE_RENDERING()                                                       \
  glEnableVertexAttribArray(0);                                                               \
  glBindBuffer(GL_ARRAY_BUFFER, G_overscan_vertex_buffer_id[GRAPHICS_BUFFER_SET_SPRITES]);    \
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);                                   \
                                                                                              \
  glEnableVertexAttribArray(1);                                                               \
  glBindBuffer(GL_ARRAY_BUFFER, G_overscan_tex_coord_buffer_id[GRAPHICS_BUFFER_SET_SPRITES]); \
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);                                   \
                                                                                              \
  glEnableVertexAttribArray(2);                                                               \
  glBindBuffer(GL_ARRAY_BUFFER, G_overscan_pal_coord_buffer_id[GRAPHICS_BUFFER_SET_SPRITES]); \
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);

#define RENDER_END_SPRITE_RENDERING()                                          \
  glDisableVertexAttribArray(0);                                               \
  glDisableVertexAttribArray(1);                                               \
  glDisableVertexAttribArray(2);

#define RENDER_DRAW_PANELS()                                                                      \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_overscan_index_buffer_id[GRAPHICS_BUFFER_SET_SPRITES]); \
  glDrawElements( GL_TRIANGLES,                                                                   \
                  6 * G_sprite_layer_count[GRAPHICS_SPRITE_LAYER_PANELS],                         \
                  GL_UNSIGNED_SHORT,                                                              \
                  (void *) (sizeof(unsigned short) * 6 * G_sprite_layer_index[GRAPHICS_SPRITE_LAYER_PANELS]));

#define RENDER_DRAW_WIDGETS()                                                                     \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_overscan_index_buffer_id[GRAPHICS_BUFFER_SET_SPRITES]); \
  glDrawElements( GL_TRIANGLES,                                                                   \
                  6 * G_sprite_layer_count[GRAPHICS_SPRITE_LAYER_WIDGETS],                        \
                  GL_UNSIGNED_SHORT,                                                              \
                  (void *) (sizeof(unsigned short) * 6 * G_sprite_layer_index[GRAPHICS_SPRITE_LAYER_WIDGETS]));

#define RENDER_DRAW_TEXT()                                                                        \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_overscan_index_buffer_id[GRAPHICS_BUFFER_SET_SPRITES]); \
  glDrawElements( GL_TRIANGLES,                                                                   \
                  6 * G_sprite_layer_count[GRAPHICS_SPRITE_LAYER_TEXT],                           \
                  GL_UNSIGNED_SHORT,                                                              \
                  (void *) (sizeof(unsigned short) * 6 * G_sprite_layer_index[GRAPHICS_SPRITE_LAYER_TEXT]));

/* postprocessing */
#define RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_OVERSCAN()                      \
  glEnableVertexAttribArray(0);                                                 \
  glBindBuffer(GL_ARRAY_BUFFER, G_postprocessing_vertex_buffer_id_overscan);    \
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);                     \
                                                                                \
  glEnableVertexAttribArray(1);                                                 \
  glBindBuffer(GL_ARRAY_BUFFER, G_postprocessing_tex_coord_buffer_id_overscan); \
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

#define RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_WINDOW()                        \
  glEnableVertexAttribArray(0);                                                 \
  glBindBuffer(GL_ARRAY_BUFFER, G_postprocessing_vertex_buffer_id_window);      \
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);                     \
                                                                                \
  glEnableVertexAttribArray(1);                                                 \
  glBindBuffer(GL_ARRAY_BUFFER, G_postprocessing_tex_coord_buffer_id_overscan); \
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

#define RENDER_END_POSTPROCESSING()                                            \
  glDisableVertexAttribArray(0);                                               \
  glDisableVertexAttribArray(1);

#define RENDER_POSTPROCESSING_DRAW_QUAD()                                      \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_postprocessing_index_buffer_id_all); \
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);

/* shader programs setup */
#define RENDER_SET_SHADER_A()                                                  \
  glUseProgram(G_program_id_A);                                                \
                                                                               \
  glUniformMatrix4fv( G_uniform_A_mvp_matrix_id, 1,                            \
                      GL_FALSE, G_mvp_matrix_overscan);                        \
                                                                               \
  glActiveTexture(GL_TEXTURE0);                                                \
  glBindTexture(GL_TEXTURE_2D, G_texture_id_rom_data);                         \
  glUniform1i(G_uniform_A_texture_sampler_id, 0);

#define RENDER_SET_SHADER_B()                                                  \
  glUseProgram(G_program_id_B);                                                \
                                                                               \
  glUniformMatrix4fv( G_uniform_B_mvp_matrix_id, 1,                            \
                      GL_FALSE, G_mvp_matrix_overscan);                        \
                                                                               \
  glActiveTexture(GL_TEXTURE0);                                                \
  glBindTexture(GL_TEXTURE_2D, G_texture_id_rom_data);                         \
  glUniform1i(G_uniform_B_texture_sampler_id, 0);

#define RENDER_SET_SHADER_C(set)                                               \
  glUseProgram(G_program_id_C);                                                \
                                                                               \
  glUniformMatrix4fv( G_uniform_C_mvp_matrix_id, 1,                            \
                      GL_FALSE, G_mvp_matrix_intermediate);                    \
                                                                               \
  glUniform1i(G_uniform_C_levels_id, G_palette_levels);                        \
                                                                               \
  glActiveTexture(GL_TEXTURE0);                                                \
  glBindTexture(GL_TEXTURE_2D, G_texture_id_intermediate[set]);                \
  glUniform1i(G_uniform_C_texture_sampler_id, 0);                              \
                                                                               \
  glActiveTexture(GL_TEXTURE1);                                                \
  glBindTexture(GL_TEXTURE_2D, G_texture_id_palette);                          \
  glUniform1i(G_uniform_C_palette_sampler_id, 1);

#define RENDER_SET_SHADER_D(set)                                                    \
  glUseProgram(G_program_id_D);                                                     \
                                                                                    \
  glUniformMatrix4fv(G_uniform_D_mvp_matrix_id, 1, GL_FALSE, G_mvp_matrix_window);  \
                                                                                    \
  glActiveTexture(GL_TEXTURE0);                                                     \
  glBindTexture(GL_TEXTURE_2D, G_texture_id_intermediate[set]);                     \
  glUniform1i(G_uniform_D_texture_sampler_id, 0);

/*******************************************************************************
** render_reset_vbos()
*******************************************************************************/
short int render_reset_vbos()
{
  int m;

  for (m = 0; m < GRAPHICS_NUM_TILE_LAYERS; m++)
    G_tile_layer_count[m] = 0;

  for (m = 0; m < GRAPHICS_NUM_SPRITE_LAYERS; m++)
    G_sprite_layer_count[m] = 0;

  return 0;
}

/*******************************************************************************
** render_all()
*******************************************************************************/
short int render_all()
{
  /* pass 1 - background tile rendering */
  RENDER_TILES_OPENGL_SETTINGS()
  RENDER_SETUP_AND_CLEAR_OVERSCAN_OUTPUT(GRAPHICS_INTERMEDIATE_SET_2)
  RENDER_SET_SHADER_A()
  RENDER_BEGIN_TILE_RENDERING()

  if (G_tile_layer_count[GRAPHICS_TILE_LAYER_BACKGROUND] > 0)
  {
    RENDER_DRAW_BACKGROUND()
  }

  RENDER_END_TILE_RENDERING()

  /* pass 2 - panels, widgets, & text sprite rendering */
  RENDER_SPRITES_OPENGL_SETTINGS()
  RENDER_SETUP_AND_RESET_DEPTH_OVERSCAN_OUTPUT(GRAPHICS_INTERMEDIATE_SET_2)
  RENDER_SET_SHADER_B()
  RENDER_BEGIN_SPRITE_RENDERING()

  if (G_sprite_layer_count[GRAPHICS_SPRITE_LAYER_PANELS] > 0)
  {
    RENDER_DRAW_PANELS()
  }

  if (G_sprite_layer_count[GRAPHICS_SPRITE_LAYER_WIDGETS] > 0)
  {
    RENDER_DRAW_WIDGETS()
  }

  if (G_sprite_layer_count[GRAPHICS_SPRITE_LAYER_TEXT] > 0)
  {
    RENDER_DRAW_TEXT()
  }

  RENDER_END_SPRITE_RENDERING()

  /* pass 3 - convert to rgb */
  RENDER_POSTPROCESSING_OPENGL_SETTINGS()
  RENDER_SETUP_AND_CLEAR_INTERMEDIATE_TEXTURE_OUTPUT(GRAPHICS_INTERMEDIATE_SET_1)
  RENDER_SET_SHADER_C(GRAPHICS_INTERMEDIATE_SET_2)
  RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_OVERSCAN()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  /* pass 4 - linear upscale to window */
  RENDER_POSTPROCESSING_OPENGL_SETTINGS()
  RENDER_SETUP_AND_CLEAR_WINDOW_OUTPUT()
  RENDER_SET_SHADER_D(GRAPHICS_INTERMEDIATE_SET_1)
  RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_WINDOW()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  return 0;
}

