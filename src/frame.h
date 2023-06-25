/*******************************************************************************
** frame.h (sample frame generation functions)
*******************************************************************************/

#ifndef FRAME_H
#define FRAME_H

#include "clock.h"

#define FRAME_BUFFER_PAIRS_PER_MS (CLOCK_SAMPLING_RATE / 1000)  /* 32000 / 1000 = 32 */
#define FRAME_BUFFER_MAX_MS       40

/* 2 samples per pair (left/right) */
#define FRAME_BUFFER_MAX_PAIRS    (FRAME_BUFFER_MAX_MS * FRAME_BUFFER_PAIRS_PER_MS)
#define FRAME_BUFFER_MAX_SAMPLES  (FRAME_BUFFER_MAX_PAIRS * 2)

extern short int G_frame_sample_buffer[FRAME_BUFFER_MAX_SAMPLES];

extern int G_frame_num_samples;

/* function declarations */
short int frame_reset_buffer();

short int frame_generate(unsigned int ms);

#endif
