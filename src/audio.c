/*******************************************************************************
** audio.c (audio output functions)
*******************************************************************************/

#include <SDL2/SDL.h>

#include <stdio.h>

#include "audio.h"
#include "clock.h"
#include "frame.h"

#define AUDIO_SDL_BUFFER_NUM_PAIRS    1024
#define AUDIO_SDL_BUFFER_NUM_SAMPLES  (AUDIO_SDL_BUFFER_NUM_PAIRS * 2)

#define AUDIO_RING_BUFFER_NUM_PAIRS   4096
#define AUDIO_RING_BUFFER_NUM_SAMPLES (AUDIO_RING_BUFFER_NUM_PAIRS * 2)

static SDL_AudioDeviceID S_audio_device_id;

static short int  S_audio_ring_buffer[AUDIO_RING_BUFFER_NUM_SAMPLES];
static int        S_audio_ring_start;
static int        S_audio_ring_end;
static int        S_audio_ring_size;

/*******************************************************************************
** audio_callback()
*******************************************************************************/
void audio_callback(void* userdata, Uint8* stream, int len)
{
  int num_samples;

  int samples_wrap_1;
  int samples_wrap_2;

#if 0
  /* cast pointer to passed in user data */
  ring_buffer = (short int *) userdata;
#endif

  /* determine number of samples to write (each sample is 2 bytes) */
  num_samples = len / 2;

  /* handle wraparound if necessary */
  if (S_audio_ring_start + num_samples >= AUDIO_RING_BUFFER_NUM_SAMPLES)
  {
    samples_wrap_1 = AUDIO_RING_BUFFER_NUM_SAMPLES - S_audio_ring_start;
    samples_wrap_2 = num_samples - samples_wrap_1;
  }
  else
  {
    samples_wrap_1 = num_samples;
    samples_wrap_2 = 0;
  }

  /* copy samples to sdl's audio buffer */
  memcpy( stream, 
          ((Uint8 *) &S_audio_ring_buffer[S_audio_ring_start]), 
          samples_wrap_1 * 2);

  memcpy( &stream[samples_wrap_1 * 2], 
          ((Uint8 *) &S_audio_ring_buffer[0]), 
          samples_wrap_2 * 2);

  /* update ring buffer indices */
  if (S_audio_ring_start + num_samples >= AUDIO_RING_BUFFER_NUM_SAMPLES)
    S_audio_ring_start = samples_wrap_2;
  else
    S_audio_ring_start += num_samples;

  /* update ring buffer size */
  S_audio_ring_size -= num_samples;

  if (S_audio_ring_size < 0)
  {
    S_audio_ring_end = S_audio_ring_start;
    S_audio_ring_size = 0;
  }

  return;
}

/*******************************************************************************
** audio_init()
*******************************************************************************/
short int audio_init()
{
  int k;

  SDL_AudioSpec desired;
  SDL_AudioSpec obtained;

  /* initialize ring buffer */
  for (k = 0; k < AUDIO_RING_BUFFER_NUM_SAMPLES; k++)
    S_audio_ring_buffer[k] = 0;

  S_audio_ring_start = 0;
  S_audio_ring_end = 0;
  S_audio_ring_size = 0;

  /* set up desired parameters */
  desired.freq = CLOCK_SAMPLING_RATE;
  desired.format = AUDIO_S16SYS;
  desired.channels = 2;
  desired.samples = AUDIO_SDL_BUFFER_NUM_PAIRS;
  desired.callback = audio_callback;
  desired.userdata = NULL;

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
** audio_pause()
*******************************************************************************/
short int audio_pause()
{
  /* pause audio device */
  if (S_audio_device_id != 0)
    SDL_PauseAudioDevice(S_audio_device_id, 1);

  return 0;
}

/*******************************************************************************
** audio_unpause()
*******************************************************************************/
short int audio_unpause()
{
  /* unpause audio device */
  if (S_audio_device_id != 0)
    SDL_PauseAudioDevice(S_audio_device_id, 0);

  return 0;
}

/*******************************************************************************
** audio_queue_frame()
*******************************************************************************/
short int audio_queue_frame()
{
  int samples_wrap_1;
  int samples_wrap_2;

  /* lock device */
  SDL_LockAudioDevice(S_audio_device_id);

  /* handle wraparound if necessary */
  if (S_audio_ring_end + G_frame_num_samples > AUDIO_RING_BUFFER_NUM_SAMPLES)
  {
    samples_wrap_1 = AUDIO_RING_BUFFER_NUM_SAMPLES - S_audio_ring_end;
    samples_wrap_2 = G_frame_num_samples - samples_wrap_1;
  }
  else
  {
    samples_wrap_1 = G_frame_num_samples;
    samples_wrap_2 = 0;
  }

  /* write samples to the ring buffer */
  memcpy( &S_audio_ring_buffer[S_audio_ring_end], 
          &G_frame_sample_buffer[0], 
          samples_wrap_1 * 2);

  memcpy( &S_audio_ring_buffer[0], 
          &G_frame_sample_buffer[samples_wrap_1], 
          samples_wrap_2 * 2);

  /* update ring buffer indices */
  if (S_audio_ring_end + G_frame_num_samples >= AUDIO_RING_BUFFER_NUM_SAMPLES)
    S_audio_ring_end = samples_wrap_2;
  else
    S_audio_ring_end += G_frame_num_samples;

  /* update ring buffer size */
  S_audio_ring_size += G_frame_num_samples;

  if (S_audio_ring_size > AUDIO_RING_BUFFER_NUM_SAMPLES)
  {
    S_audio_ring_start = S_audio_ring_end + 1;
    S_audio_ring_start %= AUDIO_RING_BUFFER_NUM_SAMPLES;

    S_audio_ring_size = AUDIO_RING_BUFFER_NUM_SAMPLES;
  }

  /* unlock device */
  SDL_UnlockAudioDevice(S_audio_device_id);

  return 0;
}

