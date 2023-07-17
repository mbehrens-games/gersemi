/*******************************************************************************
** dblinear.h (db to linear conversion)
*******************************************************************************/

#ifndef DB_LINEAR_H
#define DB_LINEAR_H

#define DB_STEP_10_BIT 0.046875
#define DB_STEP_12_BIT 0.01171875f

extern short int G_db_to_linear_table[4096];

/* function declarations */
short int db_linear_generate_tables();

#endif
