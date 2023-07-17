/*******************************************************************************
** dblinear.c (db to linear conversion)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "dblinear.h"

/* tables */
short int G_db_to_linear_table[4096];

/*******************************************************************************
** db_linear_generate_tables()
*******************************************************************************/
short int db_linear_generate_tables()
{
  int i;

  /* ym2612 - 10 bit envelope (shifted to 12 bit), 12 bit sine, 13 bit sum    */
  /* 10 bit db: 24, 12, 6, 3, 1.5, 0.75, 0.375, 0.1875, 0.09375, 0.046875     */
  /* 12 bit db: adds on 0.0234375, 0.01171875 in back                         */
  /* 13 bit db: adds on 48 in front                                           */

  /* db to linear scale conversion */
  G_db_to_linear_table[0] = 32767;

  for (i = 1; i < 4095; i++)
  {
    G_db_to_linear_table[i] = 
      (short int) ((32767.0f * exp(-log(10) * (DB_STEP_12_BIT / 10) * i)) + 0.5f);
  }

  G_db_to_linear_table[4095] = 0;

#if 0
  /* print out db to linear table values */
  for (i = 0; i < 4096; i += 4)
  {
    printf("DB to Linear Table Index %d: %d\n", i, G_db_to_linear_table[i]);
  }
#endif

  return 0;
}

