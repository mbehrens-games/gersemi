/*******************************************************************************
** cart.h (patch cart)
*******************************************************************************/

#ifndef CART_H
#define CART_H

#define CART_NUM_EDITOR_CARTS   8
#define CART_NUM_PLAYBACK_CARTS 2

#define CART_NUM_CARTS  ( CART_NUM_EDITOR_CARTS +                              \
                          CART_NUM_PLAYBACK_CARTS)

#define CART_EDITOR_CART_NO_DEFAULT     1
#define CART_EDITOR_CART_NO_LOWER_BOUND 1
#define CART_EDITOR_CART_NO_UPPER_BOUND CART_NUM_EDITOR_CARTS
#define CART_EDITOR_CART_NO_NUM_VALUES  (CART_EDITOR_CART_NO_UPPER_BOUND - CART_EDITOR_CART_NO_LOWER_BOUND + 1)

#define CART_EDITOR_CART_NO_IS_VALID(num)                                       \
  ((num >= CART_EDITOR_CART_NO_LOWER_BOUND) && (num <= CART_EDITOR_CART_NO_UPPER_BOUND))

#define CART_EDITOR_CART_NO_IS_NOT_VALID(num)                                   \
  (!(CART_EDITOR_CART_NO_IS_VALID(num)))

#define CART_PLAYBACK_CART_NO_INSTRUMENTS (CART_EDITOR_CART_NO_UPPER_BOUND + 1)
#define CART_PLAYBACK_CART_NO_SOUND_FX    (CART_EDITOR_CART_NO_UPPER_BOUND + 2)

#define CART_TOTAL_CART_NO_DEFAULT      1
#define CART_TOTAL_CART_NO_LOWER_BOUND  1
#define CART_TOTAL_CART_NO_UPPER_BOUND  CART_NUM_CARTS
#define CART_TOTAL_CART_NO_NUM_VALUES   (CART_TOTAL_CART_NO_UPPER_BOUND - CART_TOTAL_CART_NO_LOWER_BOUND + 1)

#define CART_TOTAL_CART_NO_IS_VALID(num)                                       \
  ((num >= CART_TOTAL_CART_NO_LOWER_BOUND) && (num <= CART_TOTAL_CART_NO_UPPER_BOUND))

#define CART_TOTAL_CART_NO_IS_NOT_VALID(num)                                   \
  (!(CART_TOTAL_CART_NO_IS_VALID(num)))

#define CART_PATCHES_PER_CART     32

#define CART_PATCH_NO_DEFAULT     1
#define CART_PATCH_NO_LOWER_BOUND 1
#define CART_PATCH_NO_UPPER_BOUND CART_PATCHES_PER_CART
#define CART_PATCH_NO_NUM_VALUES  (CART_PATCH_NO_UPPER_BOUND - CART_PATCH_NO_LOWER_BOUND + 1)

#define CART_PATCH_NO_IS_VALID(num)                                            \
  ((num >= CART_PATCH_NO_LOWER_BOUND) && (num <= CART_PATCH_NO_UPPER_BOUND))

#define CART_PATCH_NO_IS_NOT_VALID(num)                                        \
  (!(CART_PATCH_NO_IS_VALID(num)))

#define CART_COMPUTE_PATCH_INDEX(cart_num, patch_num)                          \
  patch_index = ((cart_num - CART_TOTAL_CART_NO_LOWER_BOUND) * CART_PATCHES_PER_CART) + (patch_num - CART_PATCH_NO_LOWER_BOUND);

#endif
