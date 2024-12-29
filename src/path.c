/*******************************************************************************
** path.c (path variables)
*******************************************************************************/

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "path.h"

#define PATH_FREE_SDL_DIR(name)                                                \
  if (name != NULL)                                                            \
  {                                                                            \
    SDL_free(name);                                                            \
    name = NULL;                                                               \
  }

#define PATH_FREE_STRING(str)                                                  \
  if (str != NULL)                                                             \
  {                                                                            \
    free(str);                                                                 \
    str = NULL;                                                                \
  }

char* G_path_base_dir;
char* G_path_pref_dir;

char* G_path_graphics_dat;
char* G_path_shaders_dat;

char* G_path_carts_dir;
char* G_path_songs_dir;

char* G_path_cart_test_1;

/*******************************************************************************
** path_init()
*******************************************************************************/
short int path_init()
{
  /* initialize pointers */
  G_path_base_dir = NULL;
  G_path_pref_dir = NULL;

  G_path_graphics_dat = NULL;
  G_path_shaders_dat = NULL;

  G_path_carts_dir = NULL;
  G_path_songs_dir = NULL;

  G_path_cart_test_1 = NULL;

  /* executable & preferences directories */
  G_path_base_dir = SDL_GetBasePath();
  G_path_pref_dir = SDL_GetPrefPath("Michael Behrens", "Gersemi");

  if (G_path_base_dir == NULL)
    return 1;

  if (G_path_pref_dir == NULL)
    return 1;

  /* graphics file path */
  G_path_graphics_dat = 
    malloc(sizeof(char) * (strlen(G_path_base_dir) + strlen("gersgrph.dat") + 1));

  if (G_path_graphics_dat == NULL)
    return 1;

  strcpy(G_path_graphics_dat, G_path_base_dir);
  strcat(G_path_graphics_dat, "gersgrph.dat");

  /* shader file path */
  G_path_shaders_dat = 
    malloc(sizeof(char) * (strlen(G_path_base_dir) + strlen("shader2z.dat") + 1));

  if (G_path_shaders_dat == NULL)
    return 1;

  strcpy(G_path_shaders_dat, G_path_base_dir);
  strcat(G_path_shaders_dat, "shader2z.dat");

  /* document directories */

  /* testing */
  G_path_carts_dir = malloc(sizeof(char) * (255 + 1));

  if (G_path_carts_dir == NULL)
    return 1;

  strcpy(G_path_carts_dir, "/Users/mike/Documents/Gersemi/Carts/");

  G_path_songs_dir = malloc(sizeof(char) * (255 + 1));

  if (G_path_songs_dir == NULL)
    return 1;

  strcpy(G_path_songs_dir, "/Users/mike/Documents/Gersemi/Songs/");

  G_path_cart_test_1 = malloc(sizeof(char) * (255 + 1));

  if (G_path_cart_test_1 == NULL)
    return 1;

  strcpy(G_path_cart_test_1, G_path_carts_dir);
  strcat(G_path_cart_test_1, "cart01.crt");

  return 0;
}

/*******************************************************************************
** path_deinit()
*******************************************************************************/
short int path_deinit()
{
  PATH_FREE_SDL_DIR(G_path_base_dir)
  PATH_FREE_SDL_DIR(G_path_pref_dir)

  PATH_FREE_STRING(G_path_graphics_dat)
  PATH_FREE_STRING(G_path_shaders_dat)

  PATH_FREE_STRING(G_path_carts_dir)
  PATH_FREE_STRING(G_path_songs_dir)

  PATH_FREE_STRING(G_path_cart_test_1)

  return 0;
}

