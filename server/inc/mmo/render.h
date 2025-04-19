#ifndef MMO_RENDER_H
#define MMO_RENDER_H

#include <stdint.h>

#include <mmo/arr/cell.h>
#include <mmo/arr/char.h>

#define MMO_ANSI_RESET                (mmo_char_arr_view_t){.elems = "\x1b[0m", .num_elems = 7}
#define MMO_ANSI_CLEAR_SCREEN         (mmo_char_arr_view_t){.elems = "\x1b[2J", .num_elems = 7}
#define MMO_ANSI_HIDE_CURSOR          (mmo_char_arr_view_t){.elems = "\x1b[?25l", .num_elems = 9}
#define MMO_ANSI_SHOW_CURSOR          (mmo_char_arr_view_t){.elems = "\x1b[?25h", .num_elems = 9}
#define MMO_ANSI_MOVE_CURSOR_TO_START (mmo_char_arr_view_t){.elems = "\x1b[H", .num_elems = 6}

typedef struct mmo_cell_color_s {
    /* If color is not set the default color will be used.
       If the client's terminal supports transparency, the default color will be transparency. */
    bool is_set;

    uint8_t r;
    uint8_t g;
    uint8_t b;
} mmo_cell_color_t;

typedef struct mmo_cell_s {
    uint32_t char_code;
    mmo_cell_color_t fg_color;
    mmo_cell_color_t bg_color;
} mmo_cell_t;

typedef struct mmo_screen_buf_s {
    int width;
    int height;
    mmo_cell_arr_t cells;
} mmo_screen_buf_t;

void mmo_screen_buf_new(mmo_screen_buf_t *buf, int width, int height);
void mmo_screen_buf_free(mmo_screen_buf_t *buf);
void mmo_screen_buf_resize(mmo_screen_buf_t *buf, int width, int height);
void mmo_screen_buf_to_string(mmo_screen_buf_t *buf, mmo_char_arr_t *out);

#endif
