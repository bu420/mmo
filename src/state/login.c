#include <mmo/state/login.h>

#include <stdint.h>

#include <mmo/log.h>
#include <mmo/render.h>
#include <mmo/arr/client_input.h>

void mmo_login_state_new(mmo_login_state_t *state) { state->logged_in = false; }

void mmo_login_state_free(void *ctx) { (void)ctx; }

void mmo_login_state_update(void *ctx, mmo_client_handle_t handle,
                            mmo_server_t *server,
                            mmo_char_arr_span_t *client_keyboard_inputs) {
    (void)ctx;
    (void)handle;
    (void)server;
    (void)client_keyboard_inputs;
}

void mmo_login_state_render(void *ctx, mmo_screen_buf_t *screen_buf) {
    // mmo_login_state_t *state = (mmo_login_state_t *)ctx;
    (void)ctx;

    char *splash = "╭─────────────────────────────╮\n"
                   "│                             │\n"
                   "│          Fisherman          │\n"
                   "│     Multiplayer fishing     │\n"
                   "│          EST. 2025          │\n"
                   "│                             │\n"
                   "╰─────────────────────────────╯";

    mmo_cell_buf_t cell_buf;
    mmo_cell_buf_parse(&cell_buf, splash);

    for (int x = 0; x < cell_buf.cols; x += 1) {
        for (int y = 0; y < cell_buf.rows; y += 1) {
            mmo_screen_buf_set(screen_buf, x, y,
                               &cell_buf.cells.elems[y * cell_buf.cols + x]);
        }
    }

    /* Draw border. */

    /*mmo_cell_t cell;
    cell.c         = '~';
    cell.fg.is_set = false;
    cell.bg.is_set = false;

    for (int x = 0; x < MMO_COLS - 1; x += 2) {
        mmo_screen_buf_set(screen_buf, x, 0, &cell);
        mmo_screen_buf_set(screen_buf, x, MMO_ROWS - 1, &cell);
    }

    for (int y = 0; y < MMO_ROWS; y += 1) {
        mmo_screen_buf_set(screen_buf, 0, y, &cell);
        mmo_screen_buf_set(screen_buf, MMO_COLS - 1, y, &cell);
    }*/
}
