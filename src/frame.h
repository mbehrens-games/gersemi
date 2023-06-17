/*******************************************************************************
** frame.h (sample frame generation functions)
*******************************************************************************/

#ifndef FRAME_H
#define FRAME_H

#include "clock.h"

#define FRAME_NUM_SAMPLES   (CLOCK_SAMPLING_RATE / 60)   /* 36000 / 60 = 600 */

#define FRAME_BUFFER_LENGTH (FRAME_NUM_SAMPLES * 2)

extern short int G_frame_sample_buffer[FRAME_BUFFER_LENGTH];

/* function declarations */
short int frame_reset_buffer();

short int frame_prepare_for_playback();
short int frame_generate_one_frame();

#endif
