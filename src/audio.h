/*******************************************************************************
** audio.h (audio output functions)
*******************************************************************************/

#ifndef AUDIO_H
#define AUDIO_H

/* function declarations */
short int audio_init();
short int audio_deinit();

short int audio_queue_frame();

#endif
