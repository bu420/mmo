#ifndef MMO_STRING_H
#define MMO_STRING_H

#include <mmo/arr/template.h>

MMO_ARR_DECL(char, mmo_string);

/* Returns width of UTF-8 encoded string. */
int mmo_string_utf8_width(mmo_string_view_t string);

/* Stylizes text to fit within a column of specified width.
   Caller must free returned string. */
int mmo_justify_and_hyphenate(mmo_string_view_t in, int width, mmo_string_t *out);

#endif
