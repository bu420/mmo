#include <mmo/render.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <mmo/arr/cell.h>
#include <mmo/arr/char.h>
#include <mmo/str.h>
#include <mmo/log.h>
#include <mmo/arr/bool.h>
#include <string.h>
#include <mmo/arr/bmp_pixel.h>
#include <mmo/mem.h>

#define MMO_ASCII " .:-=+*#%@"

#define MMO_FOREGROUND 38
#define MMO_BACKGROUND 48

void mmo_ansi_move_cursor(int x, int y, mmo_char_arr_t *out) {
    mmo_char_arr_resize(out, 64);
    out->num_elems = (size_t)snprintf(out->elems, out->num_elems, "\x1b[%d;%dH",
                                      y + 1, x + 1);
}

void mmo_screen_buf_new(mmo_screen_buf_t *buf, int width, int height) {
    assert(buf);

    mmo_cell_arr_new(&buf->cells);
    mmo_bool_arr_new(&buf->cells_modified_flags);

    mmo_screen_buf_resize(buf, width, height);
}

void mmo_screen_buf_free(mmo_screen_buf_t *buf) {
    assert(buf);

    mmo_bool_arr_free(&buf->cells_modified_flags);
    mmo_cell_arr_free(&buf->cells);

    buf->width  = 0;
    buf->height = 0;
}

void mmo_screen_buf_resize(mmo_screen_buf_t *buf, int width, int height) {
    assert(buf);

    mmo_cell_arr_resize(&buf->cells, (size_t)(width * height));
    mmo_bool_arr_resize(&buf->cells_modified_flags, (size_t)(width * height));

    /* Reset all cells. */
    MMO_FOREACH(buf->cells, cell) { cell->c = ' '; }

    /* Flag all cells as not modified. */
    MMO_FOREACH(buf->cells_modified_flags, modified) { *modified = false; }

    buf->width  = width;
    buf->height = height;

    buf->should_clear = true;
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

    mmo_char_arr_append(out, (char *)&cell->c);
}

void mmo_screen_buf_to_str(mmo_screen_buf_t *buf, mmo_char_arr_t *out) {
    assert(buf);
    assert(out);

    mmo_char_arr_t str;
    mmo_char_arr_new(&str);

    /* Check if client screen needs to be cleared. */
    if (buf->should_clear) {
        mmo_char_arr_append_arr(
            out,
            &(mmo_char_arr_t){.elems     = MMO_ANSI_CLEAR_SCREEN,
                              .num_elems = MMO_STR_LEN(MMO_ANSI_CLEAR_SCREEN)});
        buf->should_clear = false;
    }

    for (int y = 0; y < buf->height; y += 1) {
        /* Flag whether the cell to the left of this was modified. */
        bool prev_cell_modified = false;

        for (int x = 0; x < buf->width; x += 1) {
            mmo_char_arr_clear(&str);

            mmo_cell_t *cell = &buf->cells.elems[y * buf->width + x];
            bool modified = buf->cells_modified_flags.elems[y * buf->width + x];
            mmo_cell_t *prev = (y == 0 && x == 0) ? NULL : cell - 1;

            if (modified) {
                if (!prev_cell_modified) {
                    mmo_ansi_move_cursor(x, y, &str);
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

    if (cell->c != buf->cells.elems[y * buf->width + x].c) {
        buf->cells.elems[y * buf->width + x]                = *cell;
        buf->cells_modified_flags.elems[y * buf->width + x] = true;
    }
}

void mmo_cell_buf_new(mmo_cell_buf_t *buf) {
    mmo_cell_arr_new(&buf->cells);
    buf->width  = 0;
    buf->height = 0;
}

void mmo_cell_buf_free(mmo_cell_buf_t *buf) { mmo_cell_arr_free(&buf->cells); }

static int mmo_to_int(const uint8_t *header, int offset) {
    return header[offset + 0] | (header[offset + 1] << 8) |
           (header[offset + 2] << 16) | (header[offset + 3] << 24);
}

void mmo_bmp_load(const char *path, mmo_bmp_t *bmp) {
    assert(path);
    assert(bmp);

    FILE *file = fopen(path, "rb");

    if (!file) {
        mmo_log_fmt(MMO_LOG_ERROR,
                    "mmo_load_bmp(): file %s could not be opened.", path);
        exit(EXIT_FAILURE);
    }

    uint8_t header[54];
    fread(header, 1, 54, file);

    int file_size   = mmo_to_int(header, 2);
    int header_size = mmo_to_int(header, 10);
    int width       = mmo_to_int(header, 18);
    int height      = mmo_to_int(header, 22);
    int pixel_size  = header[28] / 8;
    int padding     = (4 - width * pixel_size % 4) % 4;

    if (pixel_size != 4) {
        mmo_log_fmt(MMO_LOG_ERROR,
                    "mmo_load_bmp(): file %s has incompatible pixel size, must "
                    "be 4 bytes per pixel, was %d.",
                    path, pixel_size);
        exit(EXIT_FAILURE);
    }

    mmo_bmp_pixel_arr_new(&bmp->pixels);
    mmo_bmp_pixel_arr_resize(&bmp->pixels, (size_t)(width * height));
    bmp->width  = width;
    bmp->height = height;

    uint8_t *data = mmo_malloc((size_t)(file_size - header_size));
    fseek(file, header_size, SEEK_SET);
    fread(data, 1, (size_t)(file_size - header_size), file);
    fclose(file);

    for (int y = 0; y < height; y += 1) {
        for (int x = 0; x < width; x += 1) {
            const uint8_t *src =
                &data[((height - 1 - y) * (width + padding) + x) * pixel_size];
            mmo_bmp_pixel_t *dst = &bmp->pixels.elems[y * width + x];

            dst->transparent = src[3] < 128;

            if (!dst->transparent) {
                dst->r = src[2];
                dst->g = src[1];
                dst->b = src[0];
            }
        }
    }

    free(data);
}

void mmo_bmp_render(mmo_bmp_t *bmp, mmo_cell_buf_t *buf, int width,
                    int height) {
    assert(bmp);
    assert(buf);

    mmo_cell_arr_resize(&buf->cells, (size_t)(width * height));
    buf->width  = width;
    buf->height = height;

    for (int x = 0; x < width; x += 1) {
        for (int y = 0; y < height; y += 1) {
            int src_x = (int)((float)x / (float)width * (float)bmp->width);
            int src_y = (int)((float)y / (float)height * (float)bmp->height);

            const mmo_bmp_pixel_t *src =
                &bmp->pixels.elems[src_y * bmp->width + src_x];
            mmo_cell_t *dst = &buf->cells.elems[y * width + x];

            if (src->transparent) {
                dst->c = ' ';
                continue;
            }

            float brightness =
                ((float)src->r + (float)src->g + (float)src->b) / 3.f;
            int index =
                (int)(brightness / 256.0f * (float)MMO_STR_LEN(MMO_ASCII));

            dst->c = MMO_ASCII[index];
        }
    }
}
