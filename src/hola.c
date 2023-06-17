/*******************************************************************************
** hola.c (startup functions)
*******************************************************************************/

#include <SDL2/SDL.h>

#include <glad/glad.h>

#include <stdio.h>

#include "global.h"
#include "graphics.h"

/*******************************************************************************
** hola_window()
*******************************************************************************/
short int hola_window()
{
  /* set opengl attributes */
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  /* set opengl version to 3.3 */
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  /* create window */
  G_sdl_window = SDL_CreateWindow("Gersemi",
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  640,
                                  480,
                                  SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

  if (G_sdl_window == NULL)
  {
    printf("Failed to create window: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  return 0;
}

/*******************************************************************************
** hola_opengl()
*******************************************************************************/
short int hola_opengl()
{
  SDL_GLContext context;

  /* create opengl context */
  context = SDL_GL_CreateContext(G_sdl_window);

  if (context == NULL)
  {
    fprintf(stdout, "Failed to create OpenGL context: %s\n", SDL_GetError());
    return 1;
  }

  SDL_GL_MakeCurrent(G_sdl_window, context);

  /* obtain opengl function pointers with glad */
  if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress))
  {
    fprintf(stdout, "Failed to obtain OpenGL function pointers.\n");
    return 1;
  }

  /* setup vsync */
  if (SDL_GL_SetSwapInterval(1) != 0)
    fprintf(stdout, "Failed to turn on vsync.\n");

  /* initialize 640x480 windowed mode */
  G_graphics_resolution = GRAPHICS_RESOLUTION_640_480;

  if (graphics_set_window_size(GRAPHICS_RESOLUTION_640_480) != 0)
  {
    fprintf(stdout, "Failed to set 640x480 window size.\n");
    return 1;
  }

  return 0;
}

