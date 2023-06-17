/*******************************************************************************
** shaders.h (opengl shaders)
*******************************************************************************/

#ifndef SHADERS_H
#define SHADERS_H

#include <glad/glad.h>

enum
{
  SHADERS_PROGRAM_A = 0,
  SHADERS_PROGRAM_B,
  SHADERS_PROGRAM_C,
  SHADERS_PROGRAM_D,
  SHADERS_NUM_PROGRAMS
};

/* shader program and uniform ids */
extern GLuint G_program_id_A;
extern GLuint G_program_id_B;
extern GLuint G_program_id_C;
extern GLuint G_program_id_D;

extern GLuint G_uniform_A_mvp_matrix_id;
extern GLuint G_uniform_A_texture_sampler_id;

extern GLuint G_uniform_B_mvp_matrix_id;
extern GLuint G_uniform_B_texture_sampler_id;

extern GLuint G_uniform_C_mvp_matrix_id;
extern GLuint G_uniform_C_texture_sampler_id;
extern GLuint G_uniform_C_palette_sampler_id;
extern GLuint G_uniform_C_levels_id;

extern GLuint G_uniform_D_mvp_matrix_id;
extern GLuint G_uniform_D_texture_sampler_id;

/* function declarations */
short int shaders_create_programs();
short int shaders_destroy_programs();

#endif
