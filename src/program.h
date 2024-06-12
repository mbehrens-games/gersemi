/*******************************************************************************
** program.h (program state functions)
*******************************************************************************/

#ifndef PROGRAM_H
#define PROGRAM_H

/* screens */
enum
{
  /* top panel choices */
  PROGRAM_SCREEN_CART = 0, 
  PROGRAM_SCREEN_INSTRUMENTS, 
  PROGRAM_SCREEN_SONG, 
  PROGRAM_SCREEN_MIXER, 
  PROGRAM_SCREEN_SOUND_FX, 
  PROGRAM_SCREEN_DPCM, 
  /* cart screen */
  PROGRAM_SCREEN_TEXT_ENTRY_CART_NAME, 
  PROGRAM_SCREEN_TEXT_ENTRY_PATCH_NAME, 
  PROGRAM_NUM_SCREENS
};

#define PROGRAM_SCREEN_IS_CART_RELATED()                                     \
  ( (G_program_screen == PROGRAM_SCREEN_CART)                 ||             \
    (G_program_screen == PROGRAM_SCREEN_TEXT_ENTRY_CART_NAME) ||             \
    (G_program_screen == PROGRAM_SCREEN_TEXT_ENTRY_PATCH_NAME))

#define PROGRAM_FLAGS_CLEAR 0x00
#define PROGRAM_FLAGS_MASK  0x03

#define PROGRAM_FLAG_WINDOW_MINIMIZED 0x01
#define PROGRAM_FLAG_QUIT             0x02

extern int G_program_screen;

extern int G_program_flags;

/* function declarations */
short int program_reset();

short int program_set_screen(int screen);

#endif
