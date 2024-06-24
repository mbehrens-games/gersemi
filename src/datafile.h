/*******************************************************************************
** datafile.h (native file format loading & saving)
*******************************************************************************/

#ifndef DATAFILE_H
#define DATAFILE_H

/* function declarations */
short int datafile_cart_load(int cart_index, char* filename);
short int datafile_cart_save(int cart_index, char* filename);

#endif
