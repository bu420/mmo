#ifndef MMO_STRING_H
#define MMO_STRING_H

#include <mmo/char_arr.h>

/* Returns column width of UTF-8 encoded string. */
int mmo_string_utf8_width(mmo_char_arr_view_t string);

/* Stylizes text to fit within a column of specified width. */
int mmo_string_justify_and_hyphenate(mmo_char_arr_view_t in, int width, mmo_char_arr_t *out);

#endif
