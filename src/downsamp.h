/*******************************************************************************
** downsamp.h (downsampling functions)
*******************************************************************************/

#ifndef DOWNSAMP_H
#define DOWNSAMP_H

#define DOWNSAMP_SAMPLE_BUFFER_LENGTH 400

#define DOWNSAMP_SAMPLES_PER_FRAME  367   /* 22050 / 60 = 367.5       */
#define DOWNSAMP_TIME_STEP          45351 /* 1 / 22050 seconds, in ns */

#define DOWNSAMP_SAMPLES_PER_EVEN_FRAME 367
#define DOWNSAMP_SAMPLES_PER_ODD_FRAME  368

#define DOWNSAMP_SAMPLES_PER_TWO_FRAMES 735

#define DOWNSAMP_EVEN_TIME_STEP 45413 /* (1 / 60) / 367 seconds, in ns */
#define DOWNSAMP_ODD_TIME_STEP  45290 /* (1 / 60) / 368 seconds, in ns */

extern short int  G_downsamp_sample_buffer[DOWNSAMP_SAMPLE_BUFFER_LENGTH];

/* function declarations */
short int downsamp_reset_buffer();

short int downsamp_downsample_one_frame(unsigned int frame_num);

#endif
