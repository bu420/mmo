#include <mmo/render.h>

#include <stdint.h>
#include <stdio.h>

#include <mmo/arr/cell.h>
#include <mmo/arr/char.h>
#include <mmo/str.h>
#include <mmo/log.h>
#include <mmo/arr/bool.h>

#define MMO_FOREGROUND 38
#define MMO_BACKGROUND 48

void mmo_ansi_move_cursor(int x, int y, mmo_char_arr_t *out) {
  mmo_char_arr_resize(out, 64);
  out->num_elems =
      (size_t)snprintf(out->elems, out->num_elems, "\x1b[%d;%dH", x, y);
}

void mmo_screen_buf_new(mmo_screen_buf_t *buf, int width, int height) {
  assert(buf);

  mmo_cell_arr_new(&buf->cells);
  mmo_bool_arr_new(&buf->cells_modified_flags);

  mmo_screen_buf_resize(buf, width, height);

  memset(buf->cells_modified_flags.elems, false,
         buf->cells_modified_flags.num_elems);
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

  buf->width  = width;
  buf->height = height;
}

static void mmo_cell_color_to_str(const mmo_cell_color_t *color, int layer,
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
}

static bool mmo_cell_color_equals(const mmo_cell_color_t *a,
                                  const mmo_cell_color_t *b) {
  if (!a->is_set && !b->is_set) {
    return true;
  }

  if (a->is_set != b->is_set) {
    return false;
  }

  return a->r == b->r && a->g == b->g && a->b == b->b;
}

/* prev_cell is nullble. */
static void mmo_cell_to_str(const mmo_cell_t *cell, const mmo_cell_t *prev_cell,
                            mmo_char_arr_t *out) {
  assert(cell);
  assert(out);

  bool update_fg = true;
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
  }

  mmo_char_arr_append(out, (char *)&cell->c);
}

void mmo_screen_buf_to_str(const mmo_screen_buf_t *buf, mmo_char_arr_t *out) {
  assert(buf);
  assert(out);

  mmo_char_arr_t str;
  mmo_char_arr_new(&str);

  bool prev_cell_modified = false;

  for (int y = 0; y < buf->height; y += 1) {
    for (int x = 0; x < buf->width; x += 1) {
      mmo_char_arr_clear(&str);

      mmo_cell_t *cell = &buf->cells.elems[y * buf->width + x];
      bool modified    = buf->cells_modified_flags.elems[y * buf->width + x];
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

  buf->cells.elems[y * buf->width + x]                = *cell;
  buf->cells_modified_flags.elems[y * buf->width + x] = true;
}
