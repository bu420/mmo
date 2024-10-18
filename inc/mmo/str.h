#ifndef MMO_STR_H
#define MMO_STR_H

#include <stdint.h>

typedef struct mmo_str_s {
    const char *addr;
    size_t size;
} mmo_str_t;

/* Returns the number of cells the string occupies in the terminal. */
int mmo_str_width_utf8(const mmo_str_t str);

/* Stylizes text to fit within a column of specified width.
   Caller must free returned string. */
int mmo_justify_and_hyphenate(const mmo_str_t in, int width, mmo_str_t *out);

#endif