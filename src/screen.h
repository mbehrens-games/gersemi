/*******************************************************************************
** screen.h (game screens)
*******************************************************************************/

#ifndef SCREEN_H
#define SCREEN_H

/* screen */
enum
{
  /* top panel choices */
  PROGRAM_SCREEN_CART = 0, 
  PROGRAM_SCREEN_INSTRUMENTS, 
  PROGRAM_SCREEN_SONG, 
  PROGRAM_SCREEN_MIXER, 
  PROGRAM_SCREEN_SOUND_FX, 
  PROGRAM_SCREEN_DPCM, 
  /* song subscreens */
  PROGRAM_SCREEN_MUSIC_STAFF, 
  PROGRAM_SCREEN_MUSIC_BAR, 
  /* sound fx subscreens */
  PROGRAM_SCREEN_FX_STAFF, 
  PROGRAM_SCREEN_FX_BAR, 
  /* quit */
  PROGRAM_SCREEN_QUIT, 
  PROGRAM_NUM_SCREENS
};

#endif
