/*******************************************************************************
** global.h (global variables)
*******************************************************************************/

#ifndef GLOBAL_H
#define GLOBAL_H

extern int G_prog_screen;
extern int G_last_screen;

extern int G_flag_window_minimized;
extern int G_flag_quit_program;

extern unsigned int G_timer_count;
extern unsigned int G_saved_timer_count;

extern int G_current_scroll_amount;
extern int G_max_scroll_amount;

extern int G_patch_edit_cart_number;
extern int G_patch_edit_patch_number;

extern int G_patch_edit_instrument_index;

extern int G_patch_edit_octave;
extern int G_patch_edit_note_velocity;
extern int G_patch_edit_pitch_wheel_pos;

extern int G_patch_edit_mod_wheel_pos;
extern int G_patch_edit_aftertouch_pos;
extern int G_patch_edit_exp_pedal_pos;

/* function declarations */
short int globals_init_variables();

#endif
