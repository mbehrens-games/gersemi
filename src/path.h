/*******************************************************************************
** path.h (path variables)
*******************************************************************************/

#ifndef PATH_H
#define PATH_H

extern char*  G_path_base_dir;
extern char*  G_path_pref_dir;

extern char*  G_path_graphics_dat;
extern char*  G_path_shaders_dat;

extern char*  G_path_carts_dir;
extern char*  G_path_songs_dir;

extern char*  G_path_cart_test_1;

/* function declarations */
short int path_init();
short int path_deinit();

#endif
