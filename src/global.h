/*******************************************************************************
** global.h (global variables)
*******************************************************************************/

#ifndef GLOBAL_H
#define GLOBAL_H

extern int G_current_scroll_amount;
extern int G_max_scroll_amount;

extern short int G_patch_edit_cart_number;
extern short int G_patch_edit_patch_number;

extern short int G_patch_edit_instrument_index;

extern short int G_patch_edit_octave;
extern short int G_patch_edit_note_velocity;
extern short int G_patch_edit_pitch_wheel_pos;

extern short int G_patch_edit_mod_wheel_pos;
extern short int G_patch_edit_aftertouch_pos;
extern short int G_patch_edit_exp_pedal_pos;

/* function declarations */
short int globals_init_variables();

#endif
