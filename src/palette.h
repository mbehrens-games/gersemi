/*******************************************************************************
** palette.h (opengl palette texture)
*******************************************************************************/

#ifndef PALETTE_H
#define PALETTE_H

#include <glad/glad.h>

#define PALETTE_SIZE 256

#define PALETTE_NUM_PALETTES        16
#define PALETTE_LEVELS_PER_PALETTE  (PALETTE_SIZE / PALETTE_NUM_PALETTES)

#define PALETTE_BASE_LEVEL  (PALETTE_LEVELS_PER_PALETTE / 2)

extern GLuint G_texture_id_palette;

extern GLint G_palette_levels;

extern GLfloat G_lighting_coord_table[PALETTE_LEVELS_PER_PALETTE];
extern GLfloat G_palette_coord_table[PALETTE_NUM_PALETTES];

/* function declarations */
short int palette_generate_coord_tables();

short int palette_init();
short int palette_deinit();

short int palette_create_opengl_texture();

#endif
