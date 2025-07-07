#include <mmo/input.h>

#include <assert.h>
#include <stdint.h>

#include <mmo/render.h>
#include <mmo/log.h>
#include <mmo/arr/char.h>

static bool mmo_is_ansi_seq(mmo_char_span_t in, bool *complete, size_t *len) {
    assert(complete);
    assert(len);

    if (in.num_elems == 0) {
        return false;
    }

    if (in.elems[0] != '\x1b') {
        return false;
    }

    if (in.num_elems == 1) {
        *complete = false;
        return true;
    }

    /* Iterate until ANSI final byte (in range 0x40 to 0x7e). */
    for (*len = 2; in.elems[*len] < 0x40 || in.elems[*len] > 0x7e; *len += 1) {
        if (*len == in.num_elems) {
            *complete = false;
            return true;
        }
    }

    *len += 1;
    *complete = true;
    return true;
}

static size_t mmo_codepoint_len(uint8_t b) {
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

/* Consume a single-byte ASCII character from client input stream.
   Function skips and consumes multi-byte codepoints and ANSI escape sequences
   until a single-byte ASCII character appears. Returns false if there are no
   more ASCII characters in stream. */
static bool mmo_next_ascii(mmo_char_arr_t *in, char *c) {
    assert(in);
    assert(c);

    while (in->num_elems > 0) {
        bool complete;
        size_t len;

        if (mmo_is_ansi_seq(*(mmo_char_span_t *)in, &complete, &len)) {
            if (!complete) {
                return false;
            }

            mmo_char_arr_remove_n(in, 0, len);

            continue;
        }

        len = mmo_codepoint_len((uint8_t)in->elems[0]);

        if (len == 1) {
            *c = in->elems[0];

            mmo_char_arr_remove(in, 0);

            return true;
        } else if (len > in->num_elems) {
            return false;
        }

        mmo_char_arr_remove_n(in, 0, len);
    }

    return false;
}

void mmo_parse_username(mmo_char_arr_t *buf, bool *done, mmo_char_arr_t *in) {
    assert(buf);
    assert(done);
    assert(in);

    *done = false;

    char c;

    while (mmo_next_ascii(in, &c)) {
        if (buf->num_elems < MMO_USERNAME_MAX &&
            ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
             (c >= '0' && c <= '9') || c == '_')) {
            mmo_char_arr_append(buf, &c);
        } else if (buf->num_elems > 0 && (c == 0x08 || c == 0x7f)) {
            mmo_char_arr_remove(buf, buf->num_elems - 1);
        } else if (buf->num_elems >= MMO_USERNAME_MIN &&
                   buf->num_elems <= MMO_USERNAME_MAX &&
                   (c == '\n' || c == '\r')) {
            *done = true;
        }
    }
}

static void mmo_handle_ansi(mmo_char_span_t seq) {
    uint8_t cmd = (uint8_t)seq.elems[seq.num_elems - 1];
    (void)cmd;
}

void mmo_cell_buf_parse_string(mmo_cell_buf_t *buf, mmo_char_span_t string) {
    assert(buf);

    mmo_cell_arr_new(&buf->cells);

    /* Allocate memory upfront and resize to the actual size at the end. */
    int max_cols = MMO_COLS * 4;
    int max_rows = MMO_ROWS * 4;
    mmo_cell_arr_resize(&buf->cells, (size_t)(max_cols * max_rows));
    MMO_FOREACH(buf->cells, cell) { memcpy(cell->codepoint, " ", 2); }

    buf->cols = 0;
    buf->rows = 0;

    int col = 0;
    int row = 0;

    for (uint8_t *b = (uint8_t *)string.elems; *b;) {
        /* Parse ANSI escape sequence. */

        size_t len;

        mmo_char_span_t remaining;
        remaining.elems     = (char *)b;
        remaining.num_elems = (size_t)((char *)b - string.elems);

        if (mmo_is_ansi_seq(remaining, &(bool){}, &len)) {
            mmo_handle_ansi(remaining);
            goto update_dimensions;
        }

        /* Parse UTF-8 codepoint. */

        len = mmo_codepoint_len(*b);

        /* Validate codepoint continuation bytes.
           Continuation bytes must look like 10xxxxxx. */
        for (size_t i = 1; i < len; i += 1) {
            assert((b[i] & 0xc0) == 0x80 &&
                   "Invalid UTF-8 codepoint continuation byte.");
        }

        memcpy(buf->cells.elems[row * max_cols + col].codepoint, b,
               (size_t)len);
        buf->cells.elems[row * max_cols + col].codepoint[len] = '\0';

        if (len == 1 && *b == '\n') {
            col = 0;
            row += 1;
        } else if (len == 1 && *b == '\r') {
            col = 0;
        } else {
            col += 1;
        }

        assert(col >= 0 && col < max_cols && row >= 0 && row < max_rows &&
               "Outside bounds.");

    update_dimensions:
        if (col >= buf->cols) {
            buf->cols = col + 1;
        }
        if (row >= buf->rows) {
            buf->rows = row + 1;
        }

        b += len;
    }

    /* Shrink cell buffer to content. */

    mmo_cell_t *shrunken =
        calloc((size_t)(buf->cols * buf->rows), sizeof(mmo_cell_t));

    for (int y = 0; y < buf->rows; y += 1) {
        memcpy(&shrunken[y * buf->cols], &buf->cells.elems[y * max_cols],
               (size_t)buf->cols * sizeof(mmo_cell_t));
    }

    free(buf->cells.elems);
    buf->cells.elems = shrunken;
}

void mmo_cell_buf_free(mmo_cell_buf_t *buf) { mmo_cell_arr_free(&buf->cells); }
