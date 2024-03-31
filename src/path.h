/*******************************************************************************
** path.h (path variables)
*******************************************************************************/

#ifndef PATH_H
#define PATH_H

extern char*  G_path_base;
extern char*  G_path_gfx_data;
extern char*  G_path_shader_data;

extern char*  G_path_pref;

extern char*  G_path_carts;
extern char*  G_path_cart_test_1;

extern char*  G_path_songs;

/* function declarations */
short int path_init_paths();
short int path_obtain_base_paths();
short int path_obtain_preferences_path();
short int path_set_documents_path();
short int path_free_paths();

#endif
