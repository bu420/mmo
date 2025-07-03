#ifndef MMO_RENDER_H
#define MMO_RENDER_H

#include <stdint.h>

#include <mmo/arr/cell.h>
#include <mmo/arr/char.h>
#include <mmo/arr/bool.h>
#include <mmo/arr/bmp_pixel.h>

#define MMO_COLS 80
#define MMO_ROWS 24

#define MMO_ANSI_RESET                "\x1b[0m"
#define MMO_ANSI_CLEAR_SCREEN         "\x1b[2J"
#define MMO_ANSI_HIDE_CURSOR          "\x1b[?25l"
#define MMO_ANSI_SHOW_CURSOR          "\x1b[?25h"
#define MMO_ANSI_MOVE_CURSOR_TO_START "\x1b[H"

void mmo_ansi_move_cursor(int x, int y, mmo_char_arr_t *out);
void mmo_ansi_move_cursor_relative(int dx, int dy, mmo_char_arr_t *out);

typedef struct mmo_cell_color_s {
    bool is_set;
    uint8_t r;
    uint8_t g;
    uint8_t b;
} mmo_cell_color_t;

typedef struct mmo_cell_s {
    char codepoint[5];
    mmo_cell_color_t fg;
    mmo_cell_color_t bg;
} mmo_cell_t;

/* Number of cells = MMO_COLS x MMO_ROWS. */
typedef struct mmo_screen_buf_s {
    mmo_cell_arr_t cells;
    mmo_bool_arr_t cells_modified_flags;
    bool should_clear;
} mmo_screen_buf_t;

void mmo_screen_buf_new(mmo_screen_buf_t *buf);
void mmo_screen_buf_free(mmo_screen_buf_t *buf);
void mmo_screen_buf_to_str(mmo_screen_buf_t *buf, int term_cols, int term_rows,
                           mmo_char_arr_t *out);
void mmo_screen_buf_set(mmo_screen_buf_t *buf, int x, int y,
                        const mmo_cell_t *cell);

typedef struct mmo_cell_buf_s {
    int cols;
    int rows;
    mmo_cell_arr_t cells;
} mmo_cell_buf_t;

/* Parse ANSI escape sequences and UTF-8 unicode.  */
void mmo_cell_buf_parse(mmo_cell_buf_t *buf, char *utf8);
void mmo_cell_buf_free(mmo_cell_buf_t *buf);

#endif
