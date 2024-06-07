/*******************************************************************************
** fileio.h (loading and saving from native file formats)
*******************************************************************************/

#ifndef FILEIO_H
#define FILEIO_H

/* function declarations */
short int fileio_cart_load(int cart_index, char* filename);
short int fileio_cart_save(int cart_index, char* filename);

#endif
