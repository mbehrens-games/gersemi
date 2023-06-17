/*******************************************************************************
** palette.c (opengl palette texture)
*******************************************************************************/

#include <glad/glad.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "palette.h"

#define PI      3.14159265358979323846f
#define TWO_PI  6.28318530717958647693f

GLuint G_texture_id_palette;

GLint G_palette_levels;

GLfloat G_lighting_coord_table[PALETTE_LEVELS_PER_PALETTE];
GLfloat G_palette_coord_table[PALETTE_NUM_PALETTES];

/* the standard table step is 1 / (n + 2),  */
/* where n is the number of colors per hue  */
#define COMPOSITE_16_TABLE_STEP 0.055555555555556f  /* 1/18 */

/* the luma is the average of the low and high voltages */
/* for the 1st half of each table, the low value is 0   */
/* for the 2nd half of each table, the high value is 1  */
/* the saturation is half of the peak-to-peak voltage   */
static float S_composite_16_lum[16];
static float S_composite_16_sat[16];

static float* S_luma_table;
static float* S_saturation_table;
static int    S_table_length;

/*******************************************************************************
** palette_generate_coord_tables()
*******************************************************************************/
short int palette_generate_coord_tables()
{
  int k;

  /* lighting coordinate table */
  /* finding shift amounts, each the width of a pixel on the texture  */
  /* on a 256x256 texture, the pixel size is 1/256, so the shift      */
  /* amounts are multiples of 1/256 = 0.00390625                      */
  for (k = 0; k < PALETTE_LEVELS_PER_PALETTE; k++)
    G_lighting_coord_table[k] = k * 0.00390625f;

  /* palette coordinate table */
  /* finding coordinates at the center of a pixel on the texture      */
  /* on a 256x256 texture, the pixel size is 1/256, so the centers    */
  /* are at odd multiples of 1/512 = 0.001953125                      */
  /* the value in the table is the 1st (darkest) pixel in the palette */
  for (k = 0; k < PALETTE_NUM_PALETTES; k++)
    G_palette_coord_table[k] = (2 * (PALETTE_LEVELS_PER_PALETTE * k) + 1) * 0.001953125f;

  return 0;
}

/*******************************************************************************
** palette_init()
*******************************************************************************/
short int palette_init()
{
  G_texture_id_palette = 0;

  /* set number of levels per palette (for the convert_to_rgb shader) */
  G_palette_levels = PALETTE_LEVELS_PER_PALETTE;

  return 0;
}

/*******************************************************************************
** palette_deinit()
*******************************************************************************/
short int palette_deinit()
{
  /* clear palette id from opengl */
  if (G_texture_id_palette != 0)
  {
    glDeleteTextures(1, &(G_texture_id_palette));
    G_texture_id_palette = 0;
  }

  return 0;
}

/*******************************************************************************
** palette_generate_voltage_tables()
*******************************************************************************/
short int palette_generate_voltage_tables()
{
  int k;

  /* composite 16 tables */
  for (k = 0; k < 8; k++)
  {
    S_composite_16_lum[k] = (k + 1) * COMPOSITE_16_TABLE_STEP;
    S_composite_16_lum[15 - k] = 1.0f - S_composite_16_lum[k];

    S_composite_16_sat[k] = S_composite_16_lum[k];
    S_composite_16_sat[15 - k] = S_composite_16_sat[k];
  }

  return 0;
}

/*******************************************************************************
** palette_create_opengl_texture()
*******************************************************************************/
short int palette_create_opengl_texture()
{
  int   num_hues;
  int   num_gradients;
  float phi;

  int   num_shades;
  int   shade_step;
  int   rotation_step;

  int   fixed_hues_left;
  int   fixed_hues_right;

  int   tint_hue_red;
  int   tint_hue_blue;

  int   m;
  int   n;
  int   k;

  int   p;

  float y;
  float i;
  float q;

  int   r;
  int   g;
  int   b;

  int   index;

  int   source_base_index;
  int   dest_base_index;

  GLubyte*  palette_data;

  /* generate voltage tables */
  palette_generate_voltage_tables();

  /* set voltage table pointers */
  S_luma_table = S_composite_16_lum;
  S_saturation_table = S_composite_16_sat;
  S_table_length = 16;

  /* initialize number of hues and shades */
  num_hues = 12;
  num_shades = 16;

  phi = 0.0f;

  fixed_hues_left = 1;
  fixed_hues_right = 1;

  tint_hue_red = 2;
  tint_hue_blue = 6;

  /* compute secondary variables */
  num_gradients = num_hues + 1;

  shade_step = num_shades / PALETTE_BASE_LEVEL;
  rotation_step = num_hues / 6;

  /* allocate palette data */
  palette_data = malloc(sizeof(GLubyte) * 3 * PALETTE_SIZE * PALETTE_SIZE);

  /* initialize palette data */
  for (k = 0; k < PALETTE_SIZE * PALETTE_SIZE; k++)
  {
    palette_data[3 * k + 0] = 0;
    palette_data[3 * k + 1] = 0;
    palette_data[3 * k + 2] = 0;
  }

  /* initialize palette 0 */
  for (m = PALETTE_BASE_LEVEL; m < PALETTE_LEVELS_PER_PALETTE; m++)
  {
    for (n = 0; n < num_gradients; n++)
    {
      for (k = 0; k < num_shades; k++)
      {
        index = m * PALETTE_SIZE + n * num_shades + k;

        palette_data[3 * index + 0] = 255;
        palette_data[3 * index + 1] = 255;
        palette_data[3 * index + 2] = 255;
      }
    }
  }

  /* initialize palette 7 (inverted greyscale) */
  for (m = 0; m < PALETTE_BASE_LEVEL; m++)
  {
    for (n = 0; n < num_gradients; n++)
    {
      for (k = 0; k < num_shades; k++)
      {
        index = ((7 * PALETTE_LEVELS_PER_PALETTE) + m) * PALETTE_SIZE + n * num_shades + k;

        palette_data[3 * index + 0] = 255;
        palette_data[3 * index + 1] = 255;
        palette_data[3 * index + 2] = 255;
      }
    }
  }

  /* generate palette 0 */
  for (n = 0; n < num_gradients; n++)
  {
    for (k = 0; k < num_shades; k++)
    {
      index = PALETTE_BASE_LEVEL * PALETTE_SIZE + n * num_shades + k;

      /* compute color in yiq */
      y = S_luma_table[k];

      if (n == 0)
      {
        i = 0.0f;
        q = 0.0f;
      }
      else
      {
        i = S_saturation_table[k] * cos(((TWO_PI * (n - 1)) / num_hues) + phi);
        q = S_saturation_table[k] * sin(((TWO_PI * (n - 1)) / num_hues) + phi);
      }

      /* convert from yiq to rgb */
      r = (int) (((y + (i * 0.956f) + (q * 0.619f)) * 255) + 0.5f);
      g = (int) (((y - (i * 0.272f) - (q * 0.647f)) * 255) + 0.5f);
      b = (int) (((y - (i * 1.106f) + (q * 1.703f)) * 255) + 0.5f);

      /* hard clipping at the bottom */
      if (r < 0)
        r = 0;
      if (g < 0)
        g = 0;
      if (b < 0)
        b = 0;

      /* hard clipping at the top */
      if (r > 255)
        r = 255;
      if (g > 255)
        g = 255;
      if (b > 255)
        b = 255;

      /* insert this color into the palette */
      palette_data[3 * index + 0] = r;
      palette_data[3 * index + 1] = g;
      palette_data[3 * index + 2] = b;
    }
  }

  /* shadows for palette 0 */
  for (m = 1; m < PALETTE_BASE_LEVEL; m++)
  {
    source_base_index = PALETTE_BASE_LEVEL * PALETTE_SIZE;
    dest_base_index = m * PALETTE_SIZE;

    for (n = 0; n < num_gradients; n++)
    {
      memcpy( &palette_data[3 * (dest_base_index + num_shades * n + (PALETTE_BASE_LEVEL - m) * shade_step)], 
              &palette_data[3 * (source_base_index + num_shades * n)], 
              3 * m * shade_step);
    }
  }

  /* highlights for palette 0 */
  for (m = PALETTE_BASE_LEVEL + 1; m < PALETTE_LEVELS_PER_PALETTE; m++)
  {
    source_base_index = PALETTE_BASE_LEVEL * PALETTE_SIZE;
    dest_base_index = m * PALETTE_SIZE;

    for (n = 0; n < num_gradients; n++)
    {
      memcpy( &palette_data[3 * (dest_base_index + num_shades * n)], 
              &palette_data[3 * (source_base_index + num_shades * n + (m - PALETTE_BASE_LEVEL) * shade_step)], 
              3 * (PALETTE_LEVELS_PER_PALETTE - m) * shade_step);
    }
  }

  /* compute palette 7 (inverted greyscale) */
  source_base_index = PALETTE_BASE_LEVEL * PALETTE_SIZE;
  dest_base_index = ((7 * PALETTE_LEVELS_PER_PALETTE) + PALETTE_BASE_LEVEL) * PALETTE_SIZE;

  for (k = 0; k < num_shades; k++)
  {
    memcpy( &palette_data[3 * (dest_base_index + k)], 
            &palette_data[3 * (source_base_index + (num_shades - 1 - k))], 
            3);
  }

  for (n = 1; n < num_gradients; n++)
  {
    memcpy( &palette_data[3 * (dest_base_index + num_shades * n)], 
            &palette_data[3 * (dest_base_index)], 
            3 * num_shades);
  }

  /* shadows for palette 7 */
  for (m = 1; m < PALETTE_BASE_LEVEL; m++)
  {
    source_base_index = ((7 * PALETTE_LEVELS_PER_PALETTE) + PALETTE_BASE_LEVEL) * PALETTE_SIZE;
    dest_base_index = ((7 * PALETTE_LEVELS_PER_PALETTE) + m) * PALETTE_SIZE;

    for (n = 0; n < num_gradients; n++)
    {
      memcpy( &palette_data[3 * (dest_base_index + num_shades * n + (PALETTE_BASE_LEVEL - m) * shade_step)], 
              &palette_data[3 * (source_base_index + num_shades * n)], 
              3 * m * shade_step);
    }
  }

  /* highlights for palette 7 */
  for (m = PALETTE_BASE_LEVEL + 1; m < PALETTE_LEVELS_PER_PALETTE; m++)
  {
    source_base_index = ((7 * PALETTE_LEVELS_PER_PALETTE) + PALETTE_BASE_LEVEL) * PALETTE_SIZE;
    dest_base_index = ((7 * PALETTE_LEVELS_PER_PALETTE) + m) * PALETTE_SIZE;

    for (n = 0; n < num_gradients; n++)
    {
      memcpy( &palette_data[3 * (dest_base_index + num_shades * n)], 
              &palette_data[3 * (source_base_index + num_shades * n + (m - PALETTE_BASE_LEVEL) * shade_step)], 
              3 * (PALETTE_LEVELS_PER_PALETTE - m) * shade_step);
    }
  }

  /* palettes 1-5: rotations */
  for (p = 1; p < 6; p++)
  {
    for (m = 0; m < PALETTE_LEVELS_PER_PALETTE; m++)
    {
      source_base_index = m * PALETTE_SIZE;
      dest_base_index = ((p * PALETTE_LEVELS_PER_PALETTE) + m) * PALETTE_SIZE;

      /* greys */
      memcpy( &palette_data[3 * dest_base_index], 
              &palette_data[3 * source_base_index], 
              3 * num_shades);

      /* rotated hues */
      memcpy( &palette_data[3 * (dest_base_index + 1 * num_shades)], 
              &palette_data[3 * (source_base_index + (1 + p * rotation_step) * num_shades)], 
              3 * (6 - p) * rotation_step * num_shades);

      memcpy( &palette_data[3 * (dest_base_index + (1 + (6 - p) * rotation_step) * num_shades)], 
              &palette_data[3 * (source_base_index + num_shades)], 
              3 * p * rotation_step * num_shades);
    }
  }

  /* palette 6: greyscale */
  for (m = 0; m < PALETTE_LEVELS_PER_PALETTE; m++)
  {
    for (n = 0; n < num_gradients; n++)
    {
      source_base_index = m * PALETTE_SIZE;
      dest_base_index = ((6 * PALETTE_LEVELS_PER_PALETTE) + m) * PALETTE_SIZE;

      memcpy( &palette_data[3 * (dest_base_index + (n * num_shades))], 
              &palette_data[3 * (source_base_index + (0 * num_shades))], 
              3 * num_shades);
    }
  }

  /* palettes 8-12: alternate rotations (preserving flesh tones) */
  for (p = 1; p < 6; p++)
  {
    for (m = 0; m < PALETTE_LEVELS_PER_PALETTE; m++)
    {
      /* copy non-rotated hues from palette 0 */
      source_base_index = m * PALETTE_SIZE;
      dest_base_index = (((7 + p) * PALETTE_LEVELS_PER_PALETTE) + m) * PALETTE_SIZE;

      /* copying grey and the fixed hues on the left side */
      memcpy( &palette_data[3 * dest_base_index], 
              &palette_data[3 * source_base_index], 
              3 * num_shades * (1 + fixed_hues_left));

      /* copying the fixed hues on the right side */
      memcpy( &palette_data[3 * (dest_base_index + (1 + num_hues - fixed_hues_right) * num_shades)], 
              &palette_data[3 * (source_base_index + (1 + num_hues - fixed_hues_right) * num_shades)], 
              3 * num_shades * fixed_hues_right);

      /* copy rotated hues from the original rotated palette */
      source_base_index = ((p * PALETTE_LEVELS_PER_PALETTE) + m) * PALETTE_SIZE;
      dest_base_index = (((7 + p) * PALETTE_LEVELS_PER_PALETTE) + m) * PALETTE_SIZE;

      memcpy( &palette_data[3 * (dest_base_index + (1 + fixed_hues_left) * num_shades)], 
              &palette_data[3 * (source_base_index + (1 + fixed_hues_left) * num_shades)], 
              3 * num_shades * (num_hues - fixed_hues_left - fixed_hues_right));
    }
  }

  /* palette 13: alternate greyscale (preserving flesh tones) */
  for (m = 0; m < PALETTE_LEVELS_PER_PALETTE; m++)
  {
    /* copy non-rotated hues from palette 0 */
    source_base_index = m * PALETTE_SIZE;
    dest_base_index = ((13 * PALETTE_LEVELS_PER_PALETTE) + m) * PALETTE_SIZE;

    /* copying grey and the fixed hues on the left side */
    memcpy( &palette_data[3 * dest_base_index], 
            &palette_data[3 * source_base_index], 
            3 * num_shades * (1 + fixed_hues_left));

    /* copying the fixed hues on the right side */
    memcpy( &palette_data[3 * (dest_base_index + (1 + num_hues - fixed_hues_right) * num_shades)], 
            &palette_data[3 * (source_base_index + (1 + num_hues - fixed_hues_right) * num_shades)], 
            3 * num_shades * fixed_hues_right);

    /* copy greyscale hues from the original greyscale palette */
    source_base_index = ((6 * PALETTE_LEVELS_PER_PALETTE) + m) * PALETTE_SIZE;
    dest_base_index = ((13 * PALETTE_LEVELS_PER_PALETTE) + m) * PALETTE_SIZE;

    memcpy( &palette_data[3 * (dest_base_index + (1 + fixed_hues_left) * num_shades)], 
            &palette_data[3 * (source_base_index + (1 + fixed_hues_left) * num_shades)], 
            3 * num_shades * (num_hues - fixed_hues_left - fixed_hues_right));
  }

  /* palettes 14: red tint */
  for (m = 0; m < PALETTE_LEVELS_PER_PALETTE; m++)
  {
    for (n = 0; n < num_gradients; n++)
    {
      source_base_index = m * PALETTE_SIZE;
      dest_base_index = ((14 * PALETTE_LEVELS_PER_PALETTE) + m) * PALETTE_SIZE;

      memcpy( &palette_data[3 * (dest_base_index + n * num_shades)], 
              &palette_data[3 * (source_base_index + tint_hue_red * num_shades)], 
              3 * num_shades);
    }
  }

  /* palettes 15: blue tint */
  for (m = 0; m < PALETTE_LEVELS_PER_PALETTE; m++)
  {
    for (n = 0; n < num_gradients; n++)
    {
      source_base_index = m * PALETTE_SIZE;
      dest_base_index = ((15 * PALETTE_LEVELS_PER_PALETTE) + m) * PALETTE_SIZE;

      memcpy( &palette_data[3 * (dest_base_index + n * num_shades)], 
              &palette_data[3 * (source_base_index + tint_hue_blue * num_shades)], 
              3 * num_shades);
    }
  }

  /* assign gl texture id to the palette, set it to the current gl texture */
  glGenTextures(1, &G_texture_id_palette);
  glBindTexture(GL_TEXTURE_2D, G_texture_id_palette);

  /* set up texture in opengl */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  /* load the texture to opengl from the palette data */
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, PALETTE_SIZE, PALETTE_SIZE, 
                0, GL_RGB, GL_UNSIGNED_BYTE, palette_data);

  /* clear palette data */
  if (palette_data != NULL)
  {
    free(palette_data);
    palette_data = NULL;
  }

  return 0;
}

