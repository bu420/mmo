#include <mmo/render.h>

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>

#include <mmo/arr/cell.h>
#include <mmo/arr/char.h>
#include <mmo/log.h>
#include <mmo/arr/bool.h>
#include <string.h>
#include <mmo/arr/bmp_pixel.h>

#define MMO_ANSI_FG 38
#define MMO_ANSI_BG 48

void mmo_ansi_move_cursor(int x, int y, mmo_char_arr_t *out) {
    mmo_char_arr_resize(out, 64);
    out->num_elems = (size_t)snprintf(out->elems, out->num_elems, "\x1b[%d;%dH",
                                      y + 1, x + 1);
}

void mmo_ansi_move_cursor_relative(int dx, int dy, mmo_char_arr_t *out) {
    mmo_char_arr_resize(out, 64);

    size_t len = 0;

    if (dx != 0) {
        len = (size_t)snprintf(out->elems, out->num_elems, "\x1b[%d%c", abs(dx),
                               dx > 0 ? 'C' : 'D');
    }
    if (dy != 0) {
        len += (size_t)snprintf(out->elems + len, out->num_elems - len,
                                "\x1b[%d%c", abs(dy), dy > 0 ? 'B' : 'A');
    }

    out->num_elems = len;
}

void mmo_screen_buf_new(mmo_screen_buf_t *buf) {
    assert(buf);

    mmo_cell_arr_new(&buf->cells);
    mmo_bool_arr_new(&buf->cells_modified_flags);

    mmo_cell_arr_resize(&buf->cells, MMO_COLS * MMO_ROWS);
    mmo_bool_arr_resize(&buf->cells_modified_flags, MMO_COLS * MMO_ROWS);

    MMO_FOREACH(buf->cells, cell) { memcpy(cell->codepoint, " ", 2); }
    MMO_FOREACH(buf->cells_modified_flags, modified) { *modified = false; }

    buf->should_clear = true;
}

void mmo_screen_buf_free(mmo_screen_buf_t *buf) {
    assert(buf);

    mmo_bool_arr_free(&buf->cells_modified_flags);
    mmo_cell_arr_free(&buf->cells);
}

/*static void mmo_cell_color_to_str(const mmo_cell_color_t *color, int layer,
                                  mmo_char_arr_t *out) {
    assert(color);
    assert(out);
    assert(layer == MMO_FOREGROUND || layer == MMO_BACKGROUND);

    if (!color->is_set) {
        mmo_char_span_t reset = {.elems     = MMO_ANSI_RESET,
                                 .num_elems = MMO_STR_LEN(MMO_ANSI_RESET)};
        mmo_char_arr_append_arr(out, (mmo_char_arr_t *)&reset);
        return;
    }

    mmo_char_arr_resize(out, 64);

    int len = snprintf(out->elems, out->num_elems, "\x1b[%d;2;%d;%d;%dm", layer,
                       color->r, color->g, color->b);

    out->num_elems = (size_t)len;
}*/

/*static bool mmo_cell_color_equals(const mmo_cell_color_t *a,
                                  const mmo_cell_color_t *b) {
    if (!a->is_set && !b->is_set) {
        return true;
    }

    if (a->is_set != b->is_set) {
        return false;
    }

    return a->r == b->r && a->g == b->g && a->b == b->b;
}*/

/* prev_cell is nullble. */
static void mmo_cell_to_str(const mmo_cell_t *cell, const mmo_cell_t *prev_cell,
                            mmo_char_arr_t *out) {
    assert(cell);
    assert(out);

    (void)prev_cell;

    /*bool update_fg = true;
    bool update_bg = true;

    if (prev_cell) {
        update_fg = !mmo_cell_color_equals(&cell->fg, &prev_cell->fg);
        update_bg = !mmo_cell_color_equals(&cell->bg, &prev_cell->bg);
    }

    mmo_char_arr_t str;
    mmo_char_arr_new(&str);

    if (update_fg) {
        mmo_cell_color_to_str(&cell->fg, MMO_FOREGROUND, &str);
        mmo_char_arr_append_arr(out, &str);
        mmo_char_arr_free(&str);
    }

    if (update_bg) {
        mmo_cell_color_to_str(&cell->bg, MMO_BACKGROUND, &str);
        mmo_char_arr_append_arr(out, &str);
        mmo_char_arr_free(&str);
    }*/

    mmo_char_arr_append_arr(
        out, &(mmo_char_arr_t){.elems     = (char *)cell->codepoint,
                               .num_elems = strlen(cell->codepoint)});
}

void mmo_screen_buf_to_str(mmo_screen_buf_t *buf, int term_cols, int term_rows,
                           mmo_char_arr_t *out) {
    assert(buf);
    assert(out);

    mmo_char_arr_t str;
    mmo_char_arr_new(&str);

    /* Check if client screen needs to be cleared. */
    if (buf->should_clear) {
        mmo_char_arr_append_arr(
            out, &(mmo_char_arr_t){.elems     = MMO_ANSI_CLEAR_SCREEN,
                                   .num_elems = strlen(MMO_ANSI_CLEAR_SCREEN)});
        buf->should_clear = false;
    }

    for (int y = 0; y < MMO_ROWS; y += 1) {
        /* Flag whether the cell to the left of this was modified. */
        bool prev_cell_modified = false;

        for (int x = 0; x < MMO_COLS; x += 1) {
            mmo_char_arr_clear(&str);

            mmo_cell_t *cell = &buf->cells.elems[y * MMO_COLS + x];
            bool modified = buf->cells_modified_flags.elems[y * MMO_COLS + x];
            mmo_cell_t *prev = (y == 0 && x == 0) ? NULL : cell - 1;

            if (modified) {
                if (!prev_cell_modified) {
                    mmo_ansi_move_cursor(term_cols / 2 - MMO_COLS / 2 + x,
                                         term_rows / 2 - MMO_ROWS / 2 + y,
                                         &str);
                    mmo_char_arr_append_arr(out, &str);
                    mmo_char_arr_clear(&str);
                }

                mmo_cell_to_str(cell, prev, &str);
                mmo_char_arr_append_arr(out, &str);
            }

            prev_cell_modified = modified;
        }
    }

    mmo_char_arr_free(&str);
}

void mmo_screen_buf_set(mmo_screen_buf_t *buf, int x, int y,
                        const mmo_cell_t *cell) {
    assert(buf);
    assert(cell);

    if (strcmp(cell->codepoint, buf->cells.elems[y * MMO_COLS + x].codepoint) !=
        0) {
        buf->cells.elems[y * MMO_COLS + x]                = *cell;
        buf->cells_modified_flags.elems[y * MMO_COLS + x] = true;
    }
}

static void mmo_handle_ansi(uint8_t *seq, uint8_t cmd) {
    (void)cmd;

    assert(seq);

    mmo_log_fmt(MMO_LOG_DEBUG, "ANSI escape sequence.");
}

static int mmo_codepoint_len(uint8_t b) {
    /* 0xxxxxxx, 1 byte (ASCII). */
    if ((b & 0x80) == 0x00) {
        return 1;
    }
    /* 110xxxxx, 2 bytes. */
    if ((b & 0xe0) == 0xc0) {
        return 2;
    }
    /* 1110xxxx, 3 bytes. */
    if ((b & 0xf0) == 0xe0) {
        return 3;
    }
    /* 11110xxx, 4 bytes. */
    if ((b & 0xf8) == 0xf0) {
        return 4;
    }

    assert(false && "Invalid UTF-8 codepoint.");
}

void mmo_cell_buf_parse(mmo_cell_buf_t *buf, char *utf8) {
    assert(buf);
    assert(utf8);

    mmo_cell_arr_new(&buf->cells);

    /* Allocate memory upfront and resize to the actual size at the end. */
    int max_cols = MMO_COLS * 4;
    int max_rows = MMO_ROWS * 4;
    mmo_cell_arr_resize(&buf->cells, (size_t)(max_cols * max_rows));

    buf->cols = 0;
    buf->rows = 0;

    int x = 0;
    int y = 0;

    for (uint8_t *b = (uint8_t *)utf8; *b;) {
        /* Parse ANSI escape sequence. */
        if (b[0] == '\x1b' && b[1] == '[') {
            b += 2;

            uint8_t seq[128];
            uint8_t *i = seq;

            while (*b && !isalpha(*b)) {
                *i++ = *b++;
            }

            *i = '\0';

            uint8_t cmd = *b++;

            mmo_handle_ansi(seq, cmd);

            goto update_dimensions;
        }

        /* Parse UTF-8 codepoint. */

        int len = mmo_codepoint_len(*b);

        /* Validate codepoint continuation bytes.
           Continuation bytes must look like 10xxxxxx. */
        for (int i = 1; i < len; i += 1) {
            assert((b[i] & 0xc0) == 0x80 &&
                   "Invalid UTF-8 codepoint continuation byte.");
        }

        memcpy(buf->cells.elems[y * max_cols + x].codepoint, b, (size_t)len);
        buf->cells.elems[y * max_cols + x].codepoint[len] = '\0';

        if (len == 1 && *b == '\n') {
            x = 0;
            y += 1;
        } else if (len == 1 && *b == '\r') {
            x = 0;
        } else {
            x += 1;
        }

        assert(x >= 0 && x < max_cols && y >= 0 && y < max_rows &&
               "Outside bounds.");

        b += len;

    update_dimensions:
        if (x >= buf->cols) {
            buf->cols = x + 1;
        }
        if (y >= buf->rows) {
            buf->rows = y + 1;
        }
    }

    /* Shrink cell buffer to content. */

    mmo_cell_t *shrunken =
        calloc((size_t)(buf->cols * buf->rows), sizeof(mmo_cell_t));

    for (int row = 0; row < buf->rows; row += 1) {
        memcpy(&shrunken[row * buf->cols], &buf->cells.elems[row * max_cols],
               (size_t)buf->cols * sizeof(mmo_cell_t));
    }

    free(buf->cells.elems);
    buf->cells.elems = shrunken;
}

void mmo_cell_buf_free(mmo_cell_buf_t *buf) { mmo_cell_arr_free(&buf->cells); }
