#ifndef MMO_INPUT_H
#define MMO_INPUT_H

#include <stdint.h>
#include <stdbool.h>

#include <mmo/arr/char.h>
#include <mmo/arr/cell.h>

#define MMO_USERNAME_MAX 16
#define MMO_USERNAME_MIN 2

/* Username is alphanumeric and may include underscore. */
void mmo_parse_username(mmo_char_arr_t *buf, bool *done, mmo_char_arr_t *in);

typedef struct mmo_cell_buf_s {
    int cols;
    int rows;
    mmo_cell_arr_t cells;
} mmo_cell_buf_t;

void mmo_cell_buf_parse_string(mmo_cell_buf_t *buf, mmo_char_span_t string);
void mmo_cell_buf_free(mmo_cell_buf_t *buf);

#endif
