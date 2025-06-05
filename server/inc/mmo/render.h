#ifndef MMO_RENDER_H
#define MMO_RENDER_H

#include <stdint.h>

#include <mmo/arr/cell.h>
#include <mmo/arr/char.h>
#include "mmo/arr/bool.h"

#define MMO_ANSI_RESET                "\x1b[0m"
#define MMO_ANSI_CLEAR_SCREEN         "\x1b[2J"
#define MMO_ANSI_HIDE_CURSOR          "\x1b[?25l"
#define MMO_ANSI_SHOW_CURSOR          "\x1b[?25h"
#define MMO_ANSI_MOVE_CURSOR_TO_START "\x1b[H"

void mmo_ansi_move_cursor(int x, int y, mmo_char_arr_t *out);

typedef struct mmo_cell_color_s {
    /* If color is not set the default color will be used.
       If the client terminal supports transparency, the default "color" will be
       transparency. */
    bool is_set;

    uint8_t r;
    uint8_t g;
    uint8_t b;
} mmo_cell_color_t;

typedef struct mmo_cell_s {
    char c;
    mmo_cell_color_t fg;
    mmo_cell_color_t bg;
} mmo_cell_t;

typedef struct mmo_screen_buf_s {
    int width;
    int height;
    mmo_cell_arr_t cells;
    mmo_bool_arr_t cells_modified_flags;
    bool should_clear;
} mmo_screen_buf_t;

void mmo_screen_buf_new(mmo_screen_buf_t *buf, int width, int height);
void mmo_screen_buf_free(mmo_screen_buf_t *buf);
void mmo_screen_buf_resize(mmo_screen_buf_t *buf, int width, int height);
void mmo_screen_buf_to_str(mmo_screen_buf_t *buf, mmo_char_arr_t *out);
void mmo_screen_buf_set(mmo_screen_buf_t *buf, int x, int y,
                        const mmo_cell_t *cell);

#endif
