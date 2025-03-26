#include <mmo/render.h>

#include <stdio.h>

#include <mmo/cell_arr.h>
#include <mmo/char_arr.h>

#define MMO_FOREGROUND 38
#define MMO_BACKGROUND 48

int mmo_screen_buf_new(mmo_screen_buf_t *buf, int width, int height) {
    mmo_cell_arr_new(&buf->cells);

    if (mmo_cell_arr_resize(&buf->cells, (size_t)(width * height)) == -1) {
        return -1;
    }

    buf->width  = width;
    buf->height = height;

    for (int i = 0; i < width * height; i += 1) {
        buf->cells.elems[i] = (mmo_cell_t){
            .char_code = ' ', .fg_color = {.is_set = false}, .bg_color = {.is_set = false}};
    }

    return 0;
}

void mmo_screen_buf_free(mmo_screen_buf_t *buf) {
    mmo_cell_arr_free(&buf->cells);
    buf->width  = 0;
    buf->height = 0;
}

int mmo_screen_buf_resize(mmo_screen_buf_t *buf, int width, int height) {
    if (mmo_cell_arr_resize(&buf->cells, (size_t)(width * height)) == -1) {
        return -1;
    }

    buf->width  = width;
    buf->height = height;

    return 0;
}

[[nodiscard]] static int mmo_cell_color_to_string(mmo_cell_color_t *color, int layer,
                                                  mmo_char_arr_t *out) {
    assert(color);
    assert(out);
    assert(layer == MMO_FOREGROUND || layer == MMO_BACKGROUND);

    if (mmo_char_arr_resize(out, 64) == -1) {
        return -1;
    }

    int len = snprintf(out->elems, out->num_elems, "\x1b[%d;2;%d;%d;%dm", layer, color->r, color->g,
                       color->b);

    out->num_elems = (size_t)len;

    return 0;
}

[[nodiscard]] static int mmo_cell_to_string(mmo_cell_t *cell, mmo_char_arr_t *out) {
    if (mmo_char_arr_append_arr(out, MMO_ANSI_RESET) == -1) {
        return -1;
    }

    if (cell->fg_color.is_set) {
        mmo_char_arr_t str;
        mmo_char_arr_new(&str);

        if (mmo_cell_color_to_string(&cell->fg_color, MMO_FOREGROUND, &str) == -1) {
            return -1;
        }

        if (mmo_char_arr_append_arr(out, mmo_char_arr_to_view(&str)) == -1) {
            return -1;
        }

        mmo_char_arr_free(&str);
    }

    if (cell->bg_color.is_set) {
        mmo_char_arr_t str;
        mmo_char_arr_new(&str);

        if (mmo_cell_color_to_string(&cell->bg_color, MMO_BACKGROUND, &str) == -1) {
            return -1;
        }

        if (mmo_char_arr_append_arr(out, mmo_char_arr_to_view(&str)) == -1) {
            return -1;
        }

        mmo_char_arr_free(&str);
    }

    if (mmo_char_arr_append_arr(
            out, (mmo_char_arr_view_t){.elems = (char *)&cell->char_code, .num_elems = 4}) == -1) {
        return -1;
    }

    return 0;
}

int mmo_screen_buf_to_string(mmo_screen_buf_t *buf, mmo_char_arr_t *out) {
    mmo_char_arr_t str;
    mmo_char_arr_new(&str);

    for (int y = 0; y < buf->width; y += 1) {
        for (int x = 0; x < buf->height; x += 1) {
            mmo_char_arr_clear(&str);

            if (mmo_cell_to_string(&buf->cells.elems[y * buf->width + x], &str) == -1) {
                return -1;
            }

            if (mmo_char_arr_append_arr(out, mmo_char_arr_to_view(&str)) == -1) {
                return -1;
            }
        }
    }

    mmo_char_arr_free(&str);

    return 0;
}
