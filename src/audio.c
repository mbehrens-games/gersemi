/*******************************************************************************
** audio.c (audio output functions)
*******************************************************************************/

#include <SDL2/SDL.h>

#include <stdio.h>

#include "audio.h"
#include "clock.h"
#include "frame.h"

static SDL_AudioDeviceID S_audio_device_id;

/*******************************************************************************
** audio_init()
*******************************************************************************/
short int audio_init()
{
  SDL_AudioSpec desired;
  SDL_AudioSpec obtained;

  /* set up desired parameters */
  desired.freq = CLOCK_SAMPLING_RATE;
  desired.format = AUDIO_S16SYS;
  desired.channels = 2;
  desired.samples = 1024;
  desired.callback = NULL;

  /* open audio device */
  S_audio_device_id = SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, 0);

  if (S_audio_device_id == 0)
  {
    printf("Error opening audio device!\n");
    return 1;
  }

#if 0
  /* check what device we obtained */
  printf("Obtained Freq: %d\n", obtained.freq);
  printf("Obtained Channels: %d\n", obtained.channels);
  printf("Obtained Samples: %d\n", obtained.samples);
  printf("Obtained Size: %d\n", obtained.size);
  printf("Obtained Silence: %d\n", obtained.silence);
#endif

  /* unpause audio device */
  SDL_PauseAudioDevice(S_audio_device_id, 0);

  return 0;
}

/*******************************************************************************
** audio_deinit()
*******************************************************************************/
short int audio_deinit()
{
  /* close audio device */
  if (S_audio_device_id != 0)
  {
    SDL_CloseAudioDevice(S_audio_device_id);
    S_audio_device_id = 0;
  }

  return 0;
}

/*******************************************************************************
** audio_queue_frame()
*******************************************************************************/
short int audio_queue_frame()
{
  /* queue 1 frame of audio */
  SDL_QueueAudio(S_audio_device_id, &G_frame_sample_buffer, FRAME_BUFFER_LENGTH * 2);

  return 0;
}

