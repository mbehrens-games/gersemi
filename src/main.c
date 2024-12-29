/*******************************************************************************
** Gersemi - Michael Behrens 2023
*******************************************************************************/

/*******************************************************************************
** main.c
*******************************************************************************/

#include <SDL2/SDL.h>

#include <glad/glad.h>

#include <stdio.h>

#include "audio.h"
#include "controls.h"
#include "datafile.h"
#include "global.h"
#include "graphics.h"
#include "hola.h"
#include "instrument.h"
#include "layout.h"
#include "loop.h"
#include "palette.h"
#include "path.h"
#include "program.h"
#include "render.h"
#include "texture.h"

#include "frame.h"
#include "synth.h"

/*******************************************************************************
** main()
*******************************************************************************/
int main(int argc, char *argv[])
{
  SDL_Event event;
  Uint32    ticks_last_update;
  Uint32    ticks_current;

  /* initialize sdl */
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0)
  {
    fprintf(stdout, "Failed to initialize SDL: %s\n", SDL_GetError());
    return 0;
  }

  /* load opengl library */
  if (SDL_GL_LoadLibrary(NULL))
  {
    fprintf(stdout, "Failed to load OpenGL Library: %s\n", SDL_GetError());
    goto cleanup_sdl;
  }

  /* initialize global variables */
  program_reset();
  globals_init_variables();

  /* initialize paths */
  if (path_init())
  {
    fprintf(stdout, "Failed to initialize paths. Exiting...\n");
    goto cleanup_gl_library;
  }

  /* create window */
  if (hola_window())
  {
    fprintf(stdout, "Failed to create window. Exiting...\n");
    goto cleanup_paths;
  }

  /* initialize opengl */
  if (hola_opengl())
  {
    fprintf(stdout, "Failed to initialize OpenGL. Exiting...\n");
    goto cleanup_window;
  }

  /* initialize opengl objects */
  if (graphics_create_opengl_objects())
  {
    fprintf(stdout, "Failed to initialize OpenGL objects. Exiting...\n");
    goto cleanup_opengl;
  }

  /* initialize opengl textures */
  if (texture_init())
  {
    fprintf(stdout, "Failed to initialize textures. Exiting...\n");
    goto cleanup_opengl_objects;
  }

  if (palette_init())
  {
    fprintf(stdout, "Failed to initialize palette texture. Exiting...\n");
    goto cleanup_opengl_objects;
  }

  /* generate texture coordinate tables */
  texture_generate_coord_tables();
  palette_generate_coord_tables();

  /* generate palette texture */
  palette_create_opengl_texture();

  /* load all textures */
  if (texture_load_all())
  {
    fprintf(stdout, "Error loading gfx data. Exiting...\n");
    goto cleanup_textures;
  }

  /* initialize sample frame */
  frame_reset_buffer();

  /* initialize audio */
  if (audio_init())
  {
    fprintf(stdout, "Error initializing audio device.\n");
    goto cleanup_textures;
  }

  /* initialize tables */
  synth_generate_tables();

  /* generate screen layouts */
  layout_generate_tables();

  /* initialize controller input states */
  controls_setup();

  /* reset synth */
  synth_reset_banks();

  /* testing: load test cart file */
  datafile_cart_load(0, G_path_cart_test_1);

  /* testing */
  instrument_load_patch(G_patch_edit_instrument_index, 0, 0);

  /* initialize program screen */
  program_set_screen(PROGRAM_SCREEN_CART);

  /* initialize ticks */
  ticks_current = SDL_GetTicks();
  ticks_last_update = ticks_current;

  /* initialize minimization flag */
  G_program_flags &= ~PROGRAM_FLAG_WINDOW_MINIMIZED;

  /* main loop */
  while (1)
  {
    /* process sdl events */
    while (SDL_PollEvent(&event))
    {
      /* quit */
      if (event.type == SDL_QUIT)
      {
        goto cleanup_all;
      }

      /* window */
      if (event.type == SDL_WINDOWEVENT)
      {
        /* if window is closed, quit */
        if (event.window.event == SDL_WINDOWEVENT_CLOSE)
        {
          goto cleanup_all;
        }

        /* if focus is lost, pause */
        if ((event.window.event == SDL_WINDOWEVENT_MINIMIZED) ||
            (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST))
        {
          G_program_flags |= PROGRAM_FLAG_WINDOW_MINIMIZED;
          audio_pause();
        }

        /* if focus is gained, unpause */
        if ((event.window.event == SDL_WINDOWEVENT_RESTORED) ||
            (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED))
        {
          G_program_flags &= ~PROGRAM_FLAG_WINDOW_MINIMIZED;
          audio_unpause();
          ticks_last_update = SDL_GetTicks();
        }
      }

      /* keyboard (key down) */
      if (event.type == SDL_KEYDOWN)
      {
        if ((event.key.state == SDL_PRESSED) && (event.key.repeat == 0))
          controls_keyboard_key_pressed(event.key.keysym.scancode);
      }

      /* keyboard (key up) */
      if (event.type == SDL_KEYUP)
      {
        if ((event.key.state == SDL_RELEASED) && (event.key.repeat == 0))
          controls_keyboard_key_released(event.key.keysym.scancode);
      }

      /* mouse (button down) */
      if (event.type == SDL_MOUSEBUTTONDOWN)
      {
        if (event.button.state == SDL_PRESSED)
          controls_mouse_button_pressed(event.button.button, event.button.x, event.button.y);
      }

      /* mouse (button up) */
      if (event.type == SDL_MOUSEBUTTONUP)
      {
        if (event.button.state == SDL_RELEASED)
          controls_mouse_button_released(event.button.button);
      }

      /* mouse (cursor moved) */
      if (event.type == SDL_MOUSEMOTION)
      {
        if ((event.motion.xrel != 0) || (event.motion.yrel != 0))
          controls_mouse_cursor_moved(event.motion.x, event.motion.y);
      }

      /* mouse wheel (wheel up/down) */
      if (event.type == SDL_MOUSEWHEEL)
      {
        if (event.wheel.y != 0)
          controls_mouse_wheel_moved(event.wheel.y);
      }
    }

    /* make sure the window is not minimized */
    if (G_program_flags & PROGRAM_FLAG_WINDOW_MINIMIZED)
      continue;

    /* update ticks */
    ticks_current = SDL_GetTicks();

    /* check for tick wraparound (~49 days) */
    if (ticks_current < ticks_last_update)
      ticks_last_update = 0;

    /* check if a new frame has elapsed */
    if ((ticks_current - ticks_last_update) >= (1000 / 60))
    {
      /* advance frame */
      loop_advance_frame();

      /* generate samples for this frame */
      frame_generate(ticks_current - ticks_last_update);

      /* send samples to audio output */
      audio_queue_frame();

      /* quit */
      if (G_program_flags & PROGRAM_FLAG_QUIT)
      {
        goto cleanup_all;
      }

      /* update window */
      SDL_GL_SwapWindow(G_sdl_window);

      /* store this update time */
      ticks_last_update = ticks_current;
    }
  }

  /* cleanup window and quit */
cleanup_all:
  /* testing: save test patch set file */
  datafile_cart_save(0, G_path_cart_test_1);

  audio_deinit();
cleanup_textures:
  palette_deinit();
  texture_deinit();
cleanup_opengl_objects:
  graphics_destroy_opengl_objects();
cleanup_opengl:
  SDL_GL_DeleteContext(SDL_GL_GetCurrentContext());
cleanup_window:
  SDL_DestroyWindow(G_sdl_window);
cleanup_paths:
  path_deinit();
cleanup_gl_library:
  SDL_GL_UnloadLibrary();
cleanup_sdl:
  SDL_Quit();

  return 0;
}
