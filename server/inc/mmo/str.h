#ifndef MMO_STR_H
#define MMO_STR_H

#include <mmo/arr/char.h>
#include <mmo/arr/char_arr.h>
#include <mmo/list/char_span.h>

#define MMO_STR_LEN(literal) (sizeof(literal) - 1)

void mmo_str_get_next_token(const mmo_char_arr_t *text, const mmo_char_arr_t *prev,
                            mmo_char_arr_t *out);

/* Splits a string into a linked list of string "views". */
void mmo_str_split(const mmo_char_arr_t *text, char delimiter, mmo_char_span_list_t *out);

/* Returns column width of UTF-8 encoded string. */
int mmo_str_utf8_width(const mmo_char_arr_t *str);

/* Stylizes text to fit within a column of specified width.
   Returns an array of lines. */
void mmo_str_justify_and_hyphenate(const mmo_char_arr_t *in, int width, mmo_char_arr_arr_t *lines);

#endif
