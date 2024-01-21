/*******************************************************************************
** text.h (text scanning macros)
*******************************************************************************/

#ifndef TEXT_H
#define TEXT_H

#define TEXT_CHARACTER_IS_SPACE_OR_TAB(c)                                      \
  ((c == ' ') || (c == '\t'))

#define TEXT_CHARACTER_IS_NEWLINE(c)                                           \
  ((c == '\n') || (c == '\r'))

#define TEXT_CHARACTER_IS_DIGIT(c)                                             \
  ((c >= '0') && (c <= '9'))

#define TEXT_CHARACTER_IS_LETTER(c)                                            \
  ( ((c >= 'A') && (c <= 'Z')) ||                                              \
    ((c >= 'a') && (c <= 'z')))

#define TEXT_CHARACTER_IS_VALID_IN_PATCH_NAME(c)                               \
  ( (TEXT_CHARACTER_IS_DIGIT(c))  ||                                           \
    (TEXT_CHARACTER_IS_LETTER(c)) ||                                           \
    (c == ' ') || (c == '\0'))

#define TEXT_CHARACTER_IS_VALID_IN_CART_NAME(c)                                \
  TEXT_CHARACTER_IS_VALID_IN_PATCH_NAME(c)

#define TEXT_CHARACTER_IS_VALID_IN_FILE_NAME(c)                                \
  ( (TEXT_CHARACTER_IS_DIGIT(c))  ||                                           \
    (TEXT_CHARACTER_IS_LETTER(c)) ||                                           \
    (c == '_') || (c == '.') || (c == '\0'))

#endif
