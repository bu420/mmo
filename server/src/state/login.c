#include <mmo/state/login.h>

#include <stdint.h>

#include <mmo/log.h>
#include <mmo/render.h>

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
    (void)ctx;

    // mmo_login_state_t *state = (mmo_login_state_t *)ctx;

    char *msg = "Hello and welcome to    !";

    for (int i = 0; i < (int)strlen(msg); i += 1) {
        mmo_cell_t cell;
        cell.c         = msg[i];
        cell.fg.is_set = false;
        cell.bg.is_set = false;

        if (false) {
            return;
        }

        mmo_screen_buf_set(screen_buf, i, 0, &cell);
    }

    mmo_cell_t cell;
    cell.c         = 'm';
    cell.fg.is_set = false;
    cell.fg.r      = 255;
    cell.fg.g      = 0;
    cell.fg.b      = 0;
    cell.bg.is_set = false;
    mmo_screen_buf_set(screen_buf, 21, 0, &cell);

    cell.fg.r = 0;
    cell.fg.g = 255;
    mmo_screen_buf_set(screen_buf, 22, 0, &cell);

    cell.c    = 'o';
    cell.fg.g = 0;
    cell.fg.b = 255;
    mmo_screen_buf_set(screen_buf, 23, 0, &cell);
}
