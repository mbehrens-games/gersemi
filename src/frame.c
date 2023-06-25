/*******************************************************************************
** frame.c (sample frame generation functions)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "clock.h"
#include "frame.h"
#include "synth.h"

short int G_frame_sample_buffer[FRAME_BUFFER_MAX_SAMPLES];

int G_frame_num_samples;

/*******************************************************************************
** frame_reset_buffer()
*******************************************************************************/
short int frame_reset_buffer()
{
  int k;

  for (k = 0; k < FRAME_BUFFER_MAX_SAMPLES; k++)
    G_frame_sample_buffer[k] = 0;

  G_frame_num_samples = 0;

  return 0;
}

/*******************************************************************************
** frame_generate()
*******************************************************************************/
short int frame_generate(unsigned int ms)
{
  int k;

  int num_pairs;

  /* determine number of sample pairs to generate */
  if (ms >= FRAME_BUFFER_MAX_MS)
    num_pairs = FRAME_BUFFER_MAX_MS * FRAME_BUFFER_PAIRS_PER_MS;
  else
    num_pairs = ms * FRAME_BUFFER_PAIRS_PER_MS;

  /* generate samples */
  for (k = 0; k < num_pairs; k++)
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

  /* set number of samples */
  G_frame_num_samples = num_pairs * 2;

  return 0;
}

