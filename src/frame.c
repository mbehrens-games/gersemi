/*******************************************************************************
** frame.c (sample frame generation functions)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "clock.h"
#include "frame.h"
#include "sequence.h"
#include "synth.h"

short int   G_frame_sample_buffer[FRAME_SAMPLE_BUFFER_LENGTH];

/*******************************************************************************
** frame_reset_buffer()
*******************************************************************************/
short int frame_reset_buffer()
{
  int m;

  for (m = 0; m < FRAME_SAMPLE_BUFFER_LENGTH; m++)
    G_frame_sample_buffer[m] = 0;

  return 0;
}

/*******************************************************************************
** frame_prepare_for_playback()
*******************************************************************************/
short int frame_prepare_for_playback()
{
  sequencer_activate_step();

  return 0;
}

/*******************************************************************************
** frame_generate_one_frame()
*******************************************************************************/
short int frame_generate_one_frame()
{
  int m;

  /* generate one frame of samples */
  for (m = 0; m < FRAME_SAMPLES_PER_FRAME; m++)
  {
    /* update sequencer */
    sequencer_update();

    /* update voices */
    synth_update();

    /* add sample to buffer */
    if (G_synth_level > 32767)
      G_frame_sample_buffer[m] = 32767;
    else if (G_synth_level < -32768)
      G_frame_sample_buffer[m] = -32768;
    else
      G_frame_sample_buffer[m] = (short int) G_synth_level;
  }

  return 0;
}

