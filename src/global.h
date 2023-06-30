/*******************************************************************************
** global.h (global variables)
*******************************************************************************/

#ifndef GLOBAL_H
#define GLOBAL_H

extern int          G_game_screen;
extern int          G_last_screen;

extern int          G_flag_window_minimized;
extern int          G_flag_quit_program;

extern unsigned int G_timer_count;
extern unsigned int G_saved_timer_count;

extern int          G_current_scroll_amount;

extern int          G_patch_edit_patch_index;
extern int          G_patch_edit_voice_index;
extern int          G_patch_edit_octave;
extern int          G_patch_edit_degree;
extern int          G_patch_edit_volume;
extern int          G_patch_edit_brightness;

/* function declarations */
short int globals_init_variables();

#endif
