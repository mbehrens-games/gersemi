/*******************************************************************************
** frame.h (sample frame generation functions)
*******************************************************************************/

#ifndef FRAME_H
#define FRAME_H

#define FRAME_SAMPLE_BUFFER_LENGTH  600

#define FRAME_SAMPLES_PER_FRAME     600   /* 36000 / 60 */

extern short int  G_frame_sample_buffer[FRAME_SAMPLE_BUFFER_LENGTH];

/* function declarations */
short int frame_reset_buffer();

short int frame_prepare_for_playback();
short int frame_generate_one_frame();

#endif
