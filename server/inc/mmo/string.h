#ifndef MMO_STRING_H
#define MMO_STRING_H

#include <mmo/arr/char.h>
#include <mmo/arr/char_arr.h>
#include <mmo/list/char_arr_view.h>

void mmo_string_get_next_token(mmo_char_arr_view_t text, mmo_char_arr_view_t prev,
                               mmo_char_arr_t *out);

/* Splits a string into a linked list of string views. */
void mmo_string_split(mmo_char_arr_view_t text, char delimiter, mmo_char_arr_view_list_t *out);

/* Returns column width of UTF-8 encoded string. */
int mmo_string_utf8_width(mmo_char_arr_view_t string);

/* Stylizes text to fit within a column of specified width.
   Returns an array of lines. */
void mmo_string_justify_and_hyphenate(mmo_char_arr_view_t in, int width, mmo_char_arr_arr_t *lines);

#endif
