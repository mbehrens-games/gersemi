/*******************************************************************************
** shaders.c (opengl shaders)
*******************************************************************************/

#include <glad/glad.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "path.h"
#include "shaders.h"

/* shader source texts */
static GLchar*  S_vert_source_string[SHADERS_NUM_PROGRAMS];
static GLint    S_vert_source_length[SHADERS_NUM_PROGRAMS];

static GLchar*  S_frag_source_string[SHADERS_NUM_PROGRAMS];
static GLint    S_frag_source_length[SHADERS_NUM_PROGRAMS];

/* shader program and uniform ids */
GLuint G_program_id_A;
GLuint G_program_id_B;
GLuint G_program_id_C;
GLuint G_program_id_D;

GLuint G_uniform_A_mvp_matrix_id;
GLuint G_uniform_A_texture_sampler_id;

GLuint G_uniform_B_mvp_matrix_id;
GLuint G_uniform_B_texture_sampler_id;

GLuint G_uniform_C_mvp_matrix_id;
GLuint G_uniform_C_texture_sampler_id;
GLuint G_uniform_C_palette_sampler_id;
GLuint G_uniform_C_levels_id;

GLuint G_uniform_D_mvp_matrix_id;
GLuint G_uniform_D_texture_sampler_id;

/*******************************************************************************
** shaders_compile_program()
*******************************************************************************/
short int shaders_compile_program(GLuint  program_id, 
                                  GLchar* S_vert_source_string, 
                                  GLint   S_vert_source_length, 
                                  GLchar* S_frag_source_string, 
                                  GLint   S_frag_source_length)
{
  GLuint  vertex_shader_id;
  GLuint  fragment_shader_id;

  const GLchar* string_array[1];
  GLint         length_array[1];

  GLint   compile_status;
  GLint   link_status;
  char    info_log[256];

  /* create and compile vertex shader */
  vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

  string_array[0] = S_vert_source_string;
  length_array[0] = S_vert_source_length;

  glShaderSource(vertex_shader_id, 1, string_array, length_array);
  glCompileShader(vertex_shader_id);

  /* make sure the shader compiled correctly */
  glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &compile_status);

  if (compile_status != GL_TRUE)
  {
    fprintf(stdout, "Failed to compile vertex shader.\n");

    glGetShaderInfoLog(vertex_shader_id, 256, NULL, info_log);
    fprintf(stdout, "Info Log: %s\n", info_log);

    glDeleteShader(vertex_shader_id);

    return 1;
  }

  /* create and compile fragment shader */
  fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

  string_array[0] = S_frag_source_string;
  length_array[0] = S_frag_source_length;

  glShaderSource(fragment_shader_id, 1, string_array, length_array);
  glCompileShader(fragment_shader_id);

  /* make sure the shader compiled correctly */
  glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &compile_status);

  if (compile_status != GL_TRUE)
  {
    fprintf(stdout, "Failed to compile fragment shader.\n");

    glGetShaderInfoLog(fragment_shader_id, 256, NULL, info_log);
    fprintf(stdout, "Info Log: %s\n", info_log);

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    return 1;
  }

  /* link program */
  glAttachShader(program_id, vertex_shader_id);
  glAttachShader(program_id, fragment_shader_id);
  glLinkProgram(program_id);

  /* make sure the program was linked correctly */
  glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);

  if (link_status != GL_TRUE)
  {
    fprintf(stdout, "Failed to link OpenGL program.\n");

    glGetProgramInfoLog(program_id, 256, NULL, info_log);
    fprintf(stdout, "Info Log: %s\n", info_log);

    glDetachShader(program_id, vertex_shader_id);
    glDetachShader(program_id, fragment_shader_id);

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    glDeleteProgram(program_id);
    return 1;
  }

  /* cleanup */
  glDetachShader(program_id, vertex_shader_id);
  glDetachShader(program_id, fragment_shader_id);

  glDeleteShader(vertex_shader_id);
  glDeleteShader(fragment_shader_id);

  return 0;
}

/*******************************************************************************
** shaders_load_shaders()
*******************************************************************************/
short int shaders_load_shaders()
{
  int m;

  FILE* fp;
  char  signature[8];

  /* open file */
  fp = fopen(G_path_shaders_dat, "rb");

  /* if file did not open, return error */
  if (fp == NULL)
    return 1;

  /* read signature */
  if (fread(signature, 1, 8, fp) < 8)
  {
    fclose(fp);
    return 1;
  }

  if (strncmp(signature, "SHADER2Z", 8))
  {
    fclose(fp);
    return 1;
  }

  /* read shader sources */
  for (m = 0; m < SHADERS_NUM_PROGRAMS; m++)
  {
    /* vertex shader */
    if (fread(&S_vert_source_length[m], 4, 1, fp) == 0)
    {
      fclose(fp);
      return 1;
    }

    S_vert_source_string[m] = malloc(sizeof(GLchar) * (unsigned int) S_vert_source_length[m]);

    if (S_vert_source_string[m] == NULL)
    {
      fclose(fp);
      return 1;
    }

    if (fread(S_vert_source_string[m], 1, S_vert_source_length[m], fp) < (unsigned int) S_vert_source_length[m])
    {
      fclose(fp);
      return 1;
    }

    /* fragment shader */
    if (fread(&S_frag_source_length[m], 4, 1, fp) == 0)
    {
      fclose(fp);
      return 1;
    }

    S_frag_source_string[m] = malloc(sizeof(GLchar) * (unsigned int) S_frag_source_length[m]);

    if (S_frag_source_string[m] == NULL)
    {
      fclose(fp);
      return 1;
    }

    if (fread(S_frag_source_string[m], 1, S_frag_source_length[m], fp) < (unsigned int) S_frag_source_length[m])
    {
      fclose(fp);
      return 1;
    }
  }

  /* close file */
  fclose(fp);

  /* create and compile programs */

  /* draw tiles program */
  G_program_id_A = glCreateProgram();

  if (shaders_compile_program(G_program_id_A, 
                              S_vert_source_string[SHADERS_PROGRAM_A],
                              S_vert_source_length[SHADERS_PROGRAM_A],
                              S_frag_source_string[SHADERS_PROGRAM_A],
                              S_frag_source_length[SHADERS_PROGRAM_A]))
  {
    fprintf(stdout, "Failed to compile OpenGL program (draw tiles).\n");
    glDeleteProgram(G_program_id_A);
    return 1;
  }

  /* draw sprites program */
  G_program_id_B = glCreateProgram();

  if (shaders_compile_program(G_program_id_B, 
                              S_vert_source_string[SHADERS_PROGRAM_B],
                              S_vert_source_length[SHADERS_PROGRAM_B],
                              S_frag_source_string[SHADERS_PROGRAM_B],
                              S_frag_source_length[SHADERS_PROGRAM_B]))
  {
    fprintf(stdout, "Failed to compile OpenGL program (draw sprites).\n");
    glDeleteProgram(G_program_id_A);
    glDeleteProgram(G_program_id_B);
    return 1;
  }

  /* convert to rgb program */
  G_program_id_C = glCreateProgram();

  if (shaders_compile_program(G_program_id_C, 
                              S_vert_source_string[SHADERS_PROGRAM_C],
                              S_vert_source_length[SHADERS_PROGRAM_C],
                              S_frag_source_string[SHADERS_PROGRAM_C],
                              S_frag_source_length[SHADERS_PROGRAM_C]))
  {
    fprintf(stdout, "Failed to compile OpenGL program (convert to rgb).\n");
    glDeleteProgram(G_program_id_A);
    glDeleteProgram(G_program_id_B);
    glDeleteProgram(G_program_id_C);
    return 1;
  }

  /* linear upscale program */
  G_program_id_D = glCreateProgram();

  if (shaders_compile_program(G_program_id_D, 
                              S_vert_source_string[SHADERS_PROGRAM_D],
                              S_vert_source_length[SHADERS_PROGRAM_D],
                              S_frag_source_string[SHADERS_PROGRAM_D],
                              S_frag_source_length[SHADERS_PROGRAM_D]))
  {
    fprintf(stdout, "Failed to compile OpenGL program (linear upscale).\n");
    glDeleteProgram(G_program_id_A);
    glDeleteProgram(G_program_id_B);
    glDeleteProgram(G_program_id_C);
    glDeleteProgram(G_program_id_D);
    return 1;
  }

  /* free shader source strings */
  for (m = 0; m < SHADERS_NUM_PROGRAMS; m++)
  {
    if (S_vert_source_string[m] != NULL)
    {
      free(S_vert_source_string[m]);
      S_vert_source_string[m] = NULL;
    }

    if (S_frag_source_string[m] != NULL)
    {
      free(S_frag_source_string[m]);
      S_frag_source_string[m] = NULL;
    }

    S_vert_source_length[m] = 0;
    S_frag_source_length[m] = 0;
  }

  return 0;
}

/*******************************************************************************
** shaders_create_programs()
*******************************************************************************/
short int shaders_create_programs()
{
  int m;

  /* initialize source texts */
  for (m = 0; m < SHADERS_NUM_PROGRAMS; m++)
  {
    S_vert_source_string[m] = NULL;
    S_vert_source_length[m] = 0;

    S_frag_source_string[m] = NULL;
    S_frag_source_length[m] = 0;
  }

  /* compile glsl shaders */
  if (shaders_load_shaders())
  {
    fprintf(stdout, "Failed to compile GLSL shaders.\n");
    return 1;
  }

  /* obtain ids for uniform variables in the shaders */
  G_uniform_A_mvp_matrix_id = glGetUniformLocation(G_program_id_A, "mvp_matrix");
  G_uniform_A_texture_sampler_id = glGetUniformLocation(G_program_id_A, "texture_sampler");

  G_uniform_B_mvp_matrix_id = glGetUniformLocation(G_program_id_B, "mvp_matrix");
  G_uniform_B_texture_sampler_id = glGetUniformLocation(G_program_id_B, "texture_sampler");

  G_uniform_C_mvp_matrix_id = glGetUniformLocation(G_program_id_C, "mvp_matrix");
  G_uniform_C_texture_sampler_id = glGetUniformLocation(G_program_id_C, "texture_sampler");
  G_uniform_C_palette_sampler_id = glGetUniformLocation(G_program_id_C, "palette_sampler");
  G_uniform_C_levels_id = glGetUniformLocation(G_program_id_C, "levels");

  G_uniform_D_mvp_matrix_id = glGetUniformLocation(G_program_id_D, "mvp_matrix");
  G_uniform_D_texture_sampler_id = glGetUniformLocation(G_program_id_D, "texture_sampler");

  return 0;
}

/*******************************************************************************
** shaders_destroy_programs()
*******************************************************************************/
short int shaders_destroy_programs()
{
  int m;

  /* free source texts */
  for (m = 0; m < SHADERS_NUM_PROGRAMS; m++)
  {
    if (S_vert_source_string[m] != NULL)
    {
      free(S_vert_source_string[m]);
      S_vert_source_string[m] = NULL;
    }

    if (S_frag_source_string[m] != NULL)
    {
      free(S_frag_source_string[m]);
      S_frag_source_string[m] = NULL;
    }

    S_vert_source_length[m] = 0;
    S_frag_source_length[m] = 0;
  }

  /* delete opengl shader programs */
  glDeleteProgram(G_program_id_A);
  glDeleteProgram(G_program_id_B);
  glDeleteProgram(G_program_id_C);
  glDeleteProgram(G_program_id_D);

  return 0;
}

