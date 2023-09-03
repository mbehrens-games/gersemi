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
#define RENDER_SETUP_OVERSCAN_OUTPUT(num)                                         \
  glBindFramebuffer(GL_FRAMEBUFFER, G_framebuffer_id_intermediate_##num);         \
  glViewport( 0, GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT - GRAPHICS_OVERSCAN_HEIGHT, \
              (GLsizei) GRAPHICS_OVERSCAN_WIDTH,                                  \
              (GLsizei) GRAPHICS_OVERSCAN_HEIGHT);

#define RENDER_SETUP_AND_CLEAR_OVERSCAN_OUTPUT(num)                            \
  RENDER_SETUP_OVERSCAN_OUTPUT(num)                                            \
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#define RENDER_SETUP_AND_RESET_DEPTH_OVERSCAN_OUTPUT(num)                      \
  RENDER_SETUP_OVERSCAN_OUTPUT(num)                                            \
  glClear(GL_DEPTH_BUFFER_BIT);

#define RENDER_SETUP_INTERMEDIATE_TEXTURE_OUTPUT(num)                          \
  glBindFramebuffer(GL_FRAMEBUFFER, G_framebuffer_id_intermediate_##num);      \
  glViewport(0, 0,  (GLsizei) GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH,             \
                    (GLsizei) GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT);           \

#define RENDER_SETUP_AND_CLEAR_INTERMEDIATE_TEXTURE_OUTPUT(num)                \
  RENDER_SETUP_INTERMEDIATE_TEXTURE_OUTPUT(num)                                \
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#define RENDER_SETUP_AND_RESET_DEPTH_INTERMEDIATE_TEXTURE_OUTPUT(num)          \
  RENDER_SETUP_INTERMEDIATE_TEXTURE_OUTPUT(num)                                \
  glClear(GL_DEPTH_BUFFER_BIT);

#define RENDER_SETUP_AND_CLEAR_WINDOW_OUTPUT()                                 \
  glBindFramebuffer(GL_FRAMEBUFFER, 0);                                        \
  glViewport(0, 0, (GLsizei) G_viewport_w, (GLsizei) G_viewport_h);            \
  glClear(GL_COLOR_BUFFER_BIT);

/* tiles */
#define RENDER_BEGIN_TILE_RENDERING()                                          \
  glEnableVertexAttribArray(0);                                                \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_tiles);                     \
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);                    \
                                                                               \
  glEnableVertexAttribArray(1);                                                \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_tiles);              \
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);                    \
                                                                               \
  glEnableVertexAttribArray(2);                                                \
  glBindBuffer(GL_ARRAY_BUFFER, G_lighting_and_palette_buffer_id_tiles);       \
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);

#define RENDER_END_TILE_RENDERING()                                            \
  glDisableVertexAttribArray(0);                                               \
  glDisableVertexAttribArray(1);                                               \
  glDisableVertexAttribArray(2);

#define RENDER_DRAW_BACKGROUND()                                               \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_tiles);              \
  glDrawElements( GL_TRIANGLES,                                                \
                  6 * G_tile_layer_counts[GRAPHICS_TILE_LAYER_BACKGROUND],     \
                  GL_UNSIGNED_SHORT,                                           \
                  (void *) (sizeof(unsigned short) * 6 * GRAPHICS_BACKGROUND_TILES_START_INDEX));

#define RENDER_DRAW_TOP_PANEL()                                                \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_tiles);              \
  glDrawElements( GL_TRIANGLES,                                                \
                  6 * G_tile_layer_counts[GRAPHICS_TILE_LAYER_TOP_PANEL],      \
                  GL_UNSIGNED_SHORT,                                           \
                  (void *) (sizeof(unsigned short) * 6 * GRAPHICS_TOP_PANEL_TILES_START_INDEX));

/* sprites */
#define RENDER_BEGIN_SPRITE_RENDERING()                                        \
  glEnableVertexAttribArray(0);                                                \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_sprites);                   \
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);                    \
                                                                               \
  glEnableVertexAttribArray(1);                                                \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_sprites);            \
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);                    \
                                                                               \
  glEnableVertexAttribArray(2);                                                \
  glBindBuffer(GL_ARRAY_BUFFER, G_lighting_and_palette_buffer_id_sprites);     \
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);

#define RENDER_END_SPRITE_RENDERING()                                          \
  glDisableVertexAttribArray(0);                                               \
  glDisableVertexAttribArray(1);                                               \
  glDisableVertexAttribArray(2);

#define RENDER_DRAW_UNDERLAY()                                                 \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_sprites);            \
  glDrawElements( GL_TRIANGLES,                                                \
                  6 * G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_UNDERLAY],   \
                  GL_UNSIGNED_SHORT,                                           \
                  (void *) (sizeof(unsigned short) * 6 * GRAPHICS_UNDERLAY_SPRITES_START_INDEX));

#define RENDER_DRAW_TEXT()                                                     \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_sprites);            \
  glDrawElements( GL_TRIANGLES,                                                \
                  6 * G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_TEXT],       \
                  GL_UNSIGNED_SHORT,                                           \
                  (void *) (sizeof(unsigned short) * 6 * GRAPHICS_TEXT_SPRITES_START_INDEX));

#define RENDER_DRAW_POPUP()                                                    \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_sprites);            \
  glDrawElements( GL_TRIANGLES,                                                \
                  6 * G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_POPUP],      \
                  GL_UNSIGNED_SHORT,                                           \
                  (void *) (sizeof(unsigned short) * 6 * GRAPHICS_POPUP_SPRITES_START_INDEX));

#define RENDER_DRAW_OVERLAY()                                                  \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_sprites);            \
  glDrawElements( GL_TRIANGLES,                                                \
                  6 * G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_OVERLAY],    \
                  GL_UNSIGNED_SHORT,                                           \
                  (void *) (sizeof(unsigned short) * 6 * GRAPHICS_OVERLAY_SPRITES_START_INDEX));

/* postprocessing */
#define RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_OVERSCAN()                          \
  glEnableVertexAttribArray(0);                                                     \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_postprocessing_overscan);        \
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);                         \
                                                                                    \
  glEnableVertexAttribArray(1);                                                     \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_postprocessing_overscan); \
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

#define RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_WINDOW()                            \
  glEnableVertexAttribArray(0);                                                     \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_postprocessing_window);          \
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);                         \
                                                                                    \
  glEnableVertexAttribArray(1);                                                     \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_postprocessing_overscan); \
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

#define RENDER_END_POSTPROCESSING()                                            \
  glDisableVertexAttribArray(0);                                               \
  glDisableVertexAttribArray(1);

#define RENDER_POSTPROCESSING_DRAW_QUAD()                                      \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_postprocessing_all); \
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

#define RENDER_SET_SHADER_C(num)                                               \
  glUseProgram(G_program_id_C);                                                \
                                                                               \
  glUniformMatrix4fv( G_uniform_C_mvp_matrix_id, 1,                            \
                      GL_FALSE, G_mvp_matrix_intermediate);                    \
                                                                               \
  glUniform1i(G_uniform_C_levels_id, G_palette_levels);                        \
                                                                               \
  glActiveTexture(GL_TEXTURE0);                                                \
  glBindTexture(GL_TEXTURE_2D, G_texture_id_intermediate_##num);               \
  glUniform1i(G_uniform_C_texture_sampler_id, 0);                              \
                                                                               \
  glActiveTexture(GL_TEXTURE1);                                                \
  glBindTexture(GL_TEXTURE_2D, G_texture_id_palette);                          \
  glUniform1i(G_uniform_C_palette_sampler_id, 1);

#define RENDER_SET_SHADER_D(num)                                                    \
  glUseProgram(G_program_id_D);                                                     \
                                                                                    \
  glUniformMatrix4fv(G_uniform_D_mvp_matrix_id, 1, GL_FALSE, G_mvp_matrix_window);  \
                                                                                    \
  glActiveTexture(GL_TEXTURE0);                                                     \
  glBindTexture(GL_TEXTURE_2D, G_texture_id_intermediate_##num);                    \
  glUniform1i(G_uniform_D_texture_sampler_id, 0);

/*******************************************************************************
** render_reset_vbos()
*******************************************************************************/
short int render_reset_vbos()
{
  int k;

  for (k = 0; k < GRAPHICS_NUM_TILE_LAYERS; k++)
    G_tile_layer_counts[k] = 0;

  for (k = 0; k < GRAPHICS_NUM_SPRITE_LAYERS; k++)
    G_sprite_layer_counts[k] = 0;

  return 0;
}

/*******************************************************************************
** render_all()
*******************************************************************************/
short int render_all()
{
  /* pass 1 - background tile rendering */
  RENDER_TILES_OPENGL_SETTINGS()
  RENDER_SETUP_AND_CLEAR_OVERSCAN_OUTPUT(2)
  RENDER_SET_SHADER_A()
  RENDER_BEGIN_TILE_RENDERING()

  if (G_tile_layer_counts[GRAPHICS_TILE_LAYER_BACKGROUND] > 0)
  {
    RENDER_DRAW_BACKGROUND()
  }

  if (G_tile_layer_counts[GRAPHICS_TILE_LAYER_TOP_PANEL] > 0)
  {
    RENDER_DRAW_TOP_PANEL()
  }

  RENDER_END_TILE_RENDERING()

  /* pass 2 - panels, buttons, & overlay sprite rendering */
  RENDER_SPRITES_OPENGL_SETTINGS()
  RENDER_SETUP_AND_RESET_DEPTH_OVERSCAN_OUTPUT(2)
  RENDER_SET_SHADER_B()
  RENDER_BEGIN_SPRITE_RENDERING()

  if (G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_UNDERLAY] > 0)
  {
    RENDER_DRAW_UNDERLAY()
  }

  if (G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_TEXT] > 0)
  {
    RENDER_DRAW_TEXT()
  }

  if (G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_POPUP] > 0)
  {
    RENDER_DRAW_POPUP()
  }

  if (G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_OVERLAY] > 0)
  {
    RENDER_DRAW_OVERLAY()
  }

  RENDER_END_SPRITE_RENDERING()

  /* pass 3 - convert to rgb */
  RENDER_POSTPROCESSING_OPENGL_SETTINGS()
  RENDER_SETUP_AND_CLEAR_INTERMEDIATE_TEXTURE_OUTPUT(1)
  RENDER_SET_SHADER_C(2)
  RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_OVERSCAN()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  /* pass 4 - linear upscale to window */
  RENDER_POSTPROCESSING_OPENGL_SETTINGS()
  RENDER_SETUP_AND_CLEAR_WINDOW_OUTPUT()
  RENDER_SET_SHADER_D(1)
  RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_WINDOW()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  return 0;
}

