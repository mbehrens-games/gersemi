/*******************************************************************************
** cart.c (carts & patches)
*******************************************************************************/

#include <stdio.h>
#include <string.h>

#include "bank.h"
#include "cart.h"

/* cart bank */
cart G_cart_bank[BANK_NUM_CARTS];

/* patch parameter bounds array */
unsigned char G_patch_param_bounds[PATCH_NUM_PARAMS] = 
  {  3,   1,                                    /* algorithm, legacy keyscale */
     1,   1,                                    /* noise enable, osc sync */
     3,   7,   3,   1,  15,   7,   7,           /* osc 1 */
     3,   7,   3,   1,  15,   7,   7,           /* osc 2 */
     3,   7,   3,   1,  15,   7,   7,           /* osc 3 */
     3,   7,   3,   1,  15,   7,   7,           /* osc 4 */
    31,  31,  31,  15, 127,   3,  15,   3,   3, /* env 1 */
    31,  31,  31,  15, 127,   3,  15,   3,   3, /* env 2 */
    31,  31,  31,  15, 127,   3,  15,   3,   3, /* env 3 */
    31,  31,  31,  15, 127,   3,  15,   3,   3, /* env 4 */
     1,   1,   1,   1,                          /* vibrato routing */
     1,   1,   1,   1,                          /* tremolo routing */
     1,   1,   1,   1,                          /* boost routing */
     1,   1,   1,   1,                          /* velocity routing */
     3,  47,  15, 127,   7,   1,                /* vibrato */
     3,  47,  15, 127,   3,   1,                /* tremolo */
     7,   7,                                    /* boost, velocity sensitivity */
    31,  31,  15,  95,  95,                     /* pitch envelope */
     3,   3,                                    /* filter cutoffs */
     1,  11,                                    /* pitch wheel */
     1,   3,   3,  11,                          /* arpeggio */
     1,   2,  15,                               /* portamento */
     1,   1,   1,                               /* mod wheel routing */
     1,   1,   1,                               /* aftertouch routing */
     1,   1,   1                                /* exp pedal routing */
  };

/*******************************************************************************
** cart_reset_all()
*******************************************************************************/
short int cart_reset_all()
{
  int m;

  /* reset carts */
  for (m = 0; m < BANK_NUM_CARTS; m++)
    cart_reset_cart(m);

  return 0;
}

/*******************************************************************************
** cart_reset_patch()
*******************************************************************************/
short int cart_reset_patch(int cart_index, int patch_index)
{
  int m;

  cart*  c;
  patch* p;

  /* make sure that the cart & patch indices are valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(cart_index))
    return 1;

  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain cart & patch pointers */
  c = &G_cart_bank[cart_index];
  p = &(c->patches[patch_index]);

  /* reset patch name */
  for (m = 0; m < PATCH_NAME_SIZE; m++)
    p->name[m] = '\0';

  /* reset patch data */
  for (m = 0; m < PATCH_NUM_PARAMS; m++)
    p->values[m] = 0;

  return 0;
}

/*******************************************************************************
** cart_validate_patch()
*******************************************************************************/
short int cart_validate_patch(int cart_index, int patch_index)
{
  int m;

  cart*  c;
  patch* p;

  /* make sure that the cart & patch indices are valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(cart_index))
    return 1;

  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain cart & patch pointers */
  c = &G_cart_bank[cart_index];
  p = &(c->patches[patch_index]);

  /* validate patch name */
  p->name[PATCH_NAME_SIZE - 1] = '\0';

  for (m = PATCH_NAME_SIZE - 2; m >= 0; m--)
  {
    if ((p->name[m] == ' ') || (p->name[m] == '\0'))
      p->name[m] = '\0';
    else
      break;
  }

  for (m = 0; m < PATCH_NAME_SIZE; m++)
  {
    if (!(CART_CHARACTER_IS_VALID_IN_CART_OR_PATCH_NAME(p->name[m])))
      p->name[m] = ' ';
  }

  /* validate patch data */
  for (m = 0; m < PATCH_NUM_PARAMS; m++)
  {
    if (p->values[m] < 0)
      p->values[m] = 0;
    else if (p->values[m] > G_patch_param_bounds[m])
      p->values[m] = G_patch_param_bounds[m];
  }

  return 0;
}

/*******************************************************************************
** cart_copy_patch()
*******************************************************************************/
short int cart_copy_patch(int dest_cart_index,  int dest_patch_index, 
                          int src_cart_index,   int src_patch_index)
{
  int m;

  cart*  dest_c;
  patch* dest_p;

  cart*  src_c;
  patch* src_p;

  /* make sure the destination and source indices are different */
  if ((dest_cart_index == src_cart_index) && 
      (dest_patch_index == src_patch_index))
  {
    return 1;
  }

  /* make sure that the cart & patch indices are valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(dest_cart_index))
    return 1;

  if (BANK_PATCH_INDEX_IS_NOT_VALID(dest_patch_index))
    return 1;

  if (BANK_CART_INDEX_IS_NOT_VALID(src_cart_index))
    return 1;

  if (BANK_PATCH_INDEX_IS_NOT_VALID(src_patch_index))
    return 1;

  /* obtain destination pointers */
  dest_c = &G_cart_bank[dest_cart_index];
  dest_p = &(dest_c->patches[dest_patch_index]);

  /* obtain source pointers */
  src_c = &G_cart_bank[src_cart_index];
  src_p = &(src_c->patches[src_patch_index]);

  /* copy patch name */
  strncpy(dest_p->name, src_p->name, PATCH_NAME_SIZE);

  /* copy patch data */
  for (m = 0; m < PATCH_NUM_PARAMS; m++)
    dest_p->values[m] = src_p->values[m];

  return 0;
}

/*******************************************************************************
** cart_reset_cart()
*******************************************************************************/
short int cart_reset_cart(int cart_index)
{
  int m;

  cart* c;

  /* make sure that the cart index is valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(cart_index))
    return 1;

  /* obtain cart pointer */
  c = &G_cart_bank[cart_index];

  /* reset cart name */
  for (m = 0; m < CART_NAME_SIZE; m++)
    c->name[m] = '\0';

  /* reset all patches in cart */
  for (m = 0; m < BANK_PATCHES_PER_CART; m++)
    cart_reset_patch(cart_index, m);

  return 0;
}

/*******************************************************************************
** cart_validate_cart()
*******************************************************************************/
short int cart_validate_cart(int cart_index)
{
  int m;

  cart* c;

  /* make sure that the cart index is valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(cart_index))
    return 1;

  /* obtain cart pointer */
  c = &G_cart_bank[cart_index];

  /* validate cart name */
  c->name[CART_NAME_SIZE - 1] = '\0';

  for (m = CART_NAME_SIZE - 2; m >= 0; m--)
  {
    if ((c->name[m] == ' ') || (c->name[m] == '\0'))
      c->name[m] = '\0';
    else
      break;
  }

  for (m = 0; m < CART_NAME_SIZE; m++)
  {
    if (!(CART_CHARACTER_IS_VALID_IN_CART_OR_PATCH_NAME(c->name[m])))
      c->name[m] = ' ';
  }

  /* validate all patches in cart */
  for (m = 0; m < BANK_PATCHES_PER_CART; m++)
    cart_validate_patch(cart_index, m);

  return 0;
}

/*******************************************************************************
** cart_copy_cart()
*******************************************************************************/
short int cart_copy_cart(int dest_cart_index, int src_cart_index)
{
  int m;

  cart* dest_c;
  cart* src_c;

  /* make sure the destination and source indices are different */
  if (dest_cart_index == src_cart_index)
    return 1;

  /* make sure that the cart indices are valid */
  if (BANK_CART_INDEX_IS_NOT_VALID(dest_cart_index))
    return 1;

  if (BANK_CART_INDEX_IS_NOT_VALID(src_cart_index))
    return 1;

  /* obtain source & destination pointers */
  dest_c = &G_cart_bank[dest_cart_index];
  src_c = &G_cart_bank[src_cart_index];

  /* copy cart name */
  strncpy(dest_c->name, src_c->name, CART_NAME_SIZE);

  /* copy all patches in cart */
  for (m = 0; m < BANK_PATCHES_PER_CART; m++)
    cart_copy_patch(dest_cart_index, m, src_cart_index, m);

  return 0;
}

