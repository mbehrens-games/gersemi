/*******************************************************************************
** path.c (path variables)
*******************************************************************************/

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "path.h"

char* G_path_base;
char* G_path_gfx_data;
char* G_path_shader_data;
char* G_path_pref;

/*******************************************************************************
** path_init_paths()
*******************************************************************************/
short int path_init_paths()
{
  G_path_base = NULL;
  G_path_gfx_data = NULL;
  G_path_shader_data = NULL;
  G_path_pref = NULL;

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
** path_free_paths()
*******************************************************************************/
short int path_free_paths()
{
  if (G_path_base != NULL)
  {
    SDL_free(G_path_base);
    G_path_base = NULL;
  }

  if (G_path_gfx_data != NULL)
  {
    free(G_path_gfx_data);
    G_path_gfx_data = NULL;
  }

  if (G_path_shader_data != NULL)
  {
    free(G_path_shader_data);
    G_path_shader_data = NULL;
  }

  if (G_path_pref != NULL)
  {
    SDL_free(G_path_pref);
    G_path_pref = NULL;
  }

  return 0;
}

