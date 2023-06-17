/*******************************************************************************
** frame.c (sample frame generation functions)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "clock.h"
#include "frame.h"
#include "synth.h"

short int G_frame_sample_buffer[FRAME_BUFFER_LENGTH];

/*******************************************************************************
** frame_reset_buffer()
*******************************************************************************/
short int frame_reset_buffer()
{
  int k;

  for (k = 0; k < FRAME_BUFFER_LENGTH; k++)
    G_frame_sample_buffer[k] = 0;

  return 0;
}

/*******************************************************************************
** frame_prepare_for_playback()
*******************************************************************************/
short int frame_prepare_for_playback()
{

  return 0;
}

/*******************************************************************************
** frame_generate_one_frame()
*******************************************************************************/
short int frame_generate_one_frame()
{
  int k;

  /* generate one frame of samples */
  for (k = 0; k < FRAME_NUM_SAMPLES; k++)
  {
    /* update voices */
    synth_update();

    /* add sample to buffer */
    if (G_synth_level_left > 32767)
      G_frame_sample_buffer[2 * k + 0] = 32767;
    else if (G_synth_level_left < -32768)
      G_frame_sample_buffer[2 * k + 0] = -32768;
    else
      G_frame_sample_buffer[2 * k + 0] = (short int) G_synth_level_left;

    if (G_synth_level_right > 32767)
      G_frame_sample_buffer[2 * k + 1] = 32767;
    else if (G_synth_level_right < -32768)
      G_frame_sample_buffer[2 * k + 1] = -32768;
    else
      G_frame_sample_buffer[2 * k + 1] = (short int) G_synth_level_right;
  }

  return 0;
}

