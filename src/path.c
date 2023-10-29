/*******************************************************************************
** path.c (path variables)
*******************************************************************************/

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "path.h"

#define PATH_FREE_PATH(name)                                                   \
  if (name != NULL)                                                            \
  {                                                                            \
    SDL_free(name);                                                            \
    name = NULL;                                                               \
  }

char* G_path_base;
char* G_path_gfx_data;
char* G_path_shader_data;

char* G_path_pref;

char* G_path_patch_sets;
char* G_path_patch_set_test_1;
char* G_path_songs;

/*******************************************************************************
** path_init_paths()
*******************************************************************************/
short int path_init_paths()
{
  G_path_base = NULL;
  G_path_gfx_data = NULL;
  G_path_shader_data = NULL;

  G_path_pref = NULL;

  G_path_patch_sets = NULL;
  G_path_patch_set_test_1 = NULL;
  G_path_songs = NULL;

  return 0;
}

/*******************************************************************************
** path_obtain_base_paths()
*******************************************************************************/
short int path_obtain_base_paths()
{
  G_path_base = SDL_GetBasePath();

  if (G_path_base == NULL)
  {
    return 1;
  }

  G_path_gfx_data = malloc(sizeof(char) * (strlen(G_path_base) + 12 + 1));
  strcpy(G_path_gfx_data, G_path_base);
  strcpy(G_path_gfx_data + strlen(G_path_base), "gersgrph.dat");

  G_path_shader_data = malloc(sizeof(char) * (strlen(G_path_base) + 12 + 1));
  strcpy(G_path_shader_data, G_path_base);
  strcpy(G_path_shader_data + strlen(G_path_base), "shader2z.dat");

  return 0;
}

/*******************************************************************************
** path_obtain_preferences_path()
*******************************************************************************/
short int path_obtain_preferences_path()
{
  G_path_pref = SDL_GetPrefPath("Michael Behrens", "Gersemi");

  if (G_path_pref == NULL)
  {
    return 1;
  }

  return 0;
}

/*******************************************************************************
** path_set_documents_path()
*******************************************************************************/
short int path_set_documents_path()
{
  G_path_patch_sets = malloc(sizeof(char) * (255 + 1));
  strcpy(G_path_patch_sets, "/Users/mike/Documents/Gersemi/Banks/");

  G_path_songs = malloc(sizeof(char) * (255 + 1));
  strcpy(G_path_songs, "/Users/mike/Documents/Gersemi/Songs/");

  G_path_patch_set_test_1 = malloc(sizeof(char) * (255 + 1));
  strcpy(G_path_patch_set_test_1, G_path_patch_sets);
  strcpy(G_path_patch_set_test_1 + strlen(G_path_patch_sets), "bank01.gpb");

  return 0;
}

/*******************************************************************************
** path_free_paths()
*******************************************************************************/
short int path_free_paths()
{
  PATH_FREE_PATH(G_path_base)
  PATH_FREE_PATH(G_path_gfx_data)
  PATH_FREE_PATH(G_path_shader_data)

  PATH_FREE_PATH(G_path_pref)

  PATH_FREE_PATH(G_path_patch_sets)
  PATH_FREE_PATH(G_path_patch_set_test_1)
  PATH_FREE_PATH(G_path_songs)

  return 0;
}

