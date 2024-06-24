/*******************************************************************************
** texture.h (opengl graphics rom data texture)
*******************************************************************************/

#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

#define TEXTURE_SIZE      128
#define TEXTURE_NUM_CELLS (TEXTURE_SIZE / 8)

#define TEXTURE_COORD_TABLE_SIZE (TEXTURE_NUM_CELLS + 1)

extern GLuint G_texture_id_rom_data;

extern GLfloat G_texture_coord_table[TEXTURE_COORD_TABLE_SIZE];

/* function declarations */
short int texture_generate_coord_tables();

short int texture_init();
short int texture_deinit();

short int texture_load_all();

#endif
