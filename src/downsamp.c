/*******************************************************************************
** downsamp.c (downsampling functions)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clock.h"
#include "downsamp.h"
#include "frame.h"

short int G_downsamp_sample_buffer[DOWNSAMP_SAMPLE_BUFFER_LENGTH];

/*******************************************************************************
** downsamp_reset_buffer()
*******************************************************************************/
short int downsamp_reset_buffer()
{
  int m;

  for (m = 0; m < DOWNSAMP_SAMPLE_BUFFER_LENGTH; m++)
    G_downsamp_sample_buffer[m] = 0;

  return 0;
}

/*******************************************************************************
** downsamp_downsample_one_frame()
*******************************************************************************/
short int downsamp_downsample_one_frame(unsigned int frame_num)
{
  int m;

  int length;
  int time_step;

  unsigned int time_elapsed;

  int sample_index;

  /* initialize variables based on whether this is an even or odd frame */
  if (frame_num % 2 == 0)
  {
    length = DOWNSAMP_SAMPLES_PER_EVEN_FRAME;
    time_step = DOWNSAMP_EVEN_TIME_STEP;
  }
  else
  {
    length = DOWNSAMP_SAMPLES_PER_ODD_FRAME;
    time_step = DOWNSAMP_ODD_TIME_STEP;
  }

  time_elapsed = 0;
  sample_index = 0;

  /* begin downsample */
  G_downsamp_sample_buffer[0] = G_frame_sample_buffer[0];

  for (m = 1; m < length; m++)
  {
    time_elapsed += time_step;

    sample_index += time_elapsed / CLOCK_DELTA_T_NANOSECONDS;
    time_elapsed = time_elapsed % CLOCK_DELTA_T_NANOSECONDS;

    if (sample_index >= FRAME_SAMPLES_PER_FRAME - 1)
    {
      G_downsamp_sample_buffer[m] = G_frame_sample_buffer[FRAME_SAMPLES_PER_FRAME - 1];
      continue;
    }

    /* nearest neighbor */
    if (time_elapsed < CLOCK_DELTA_T_NANOSECONDS / 2)
      G_downsamp_sample_buffer[m] = G_frame_sample_buffer[sample_index];
    else
      G_downsamp_sample_buffer[m] = G_frame_sample_buffer[sample_index + 1];
  }

  return 0;
}

