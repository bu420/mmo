#ifndef MMO_STR_H
#define MMO_STR_H

typedef struct mmo_str_s {
    char *bytes;
    int size;
} mmo_str_t;

/* Returns terminal column width of UTF-8 encoded string. */
int mmo_str_utf8_width(const mmo_str_t str);

/* Stylizes text to fit within a column of specific width.
   Caller must free returned string. */
int mmo_justify_and_hyphenate(const mmo_str_t in, int width, mmo_str_t *out);

#endif