#include <mmo/state/login.h>

#include <stdint.h>

#include <mmo/log.h>
#include <mmo/render.h>
#include <mmo/arr/client_input.h>

void mmo_login_state_new(mmo_login_state_t *state) {
    state->logged_in = false;

    mmo_load_bmp("perch.bmp", &state->fish);
}

void mmo_login_state_free(void *ctx) { (void)ctx; }

void mmo_login_state_update(void *ctx, mmo_client_handle_t handle,
                            mmo_server_t *server,
                            mmo_char_arr_span_t *client_keyboard_inputs) {
    (void)ctx;
    (void)handle;
    (void)server;
    (void)client_keyboard_inputs;
}

static void mmo_render_text_center(char *txt, int len, int y,
                                   mmo_screen_buf_t *screen_buf) {
    mmo_cell_t cell;
    cell.fg.is_set = false;
    cell.bg.is_set = false;

    for (int i = 0; i < len; i += 1) {
        cell.c = txt[i];

        mmo_screen_buf_set(screen_buf, screen_buf->width / 2 - len / 2 + i, y,
                           &cell);
    }
}

void mmo_login_state_render(void *ctx, mmo_screen_buf_t *screen_buf) {
    (void)ctx;

    /* Draw border. */

    mmo_cell_t cell;
    cell.c         = '~';
    cell.fg.is_set = false;
    cell.bg.is_set = false;

    for (int x = 0; x < screen_buf->width - 1; x += 2) {
        mmo_screen_buf_set(screen_buf, x, 0, &cell);
        mmo_screen_buf_set(screen_buf, x, screen_buf->height - 1, &cell);
    }

    for (int y = 0; y < screen_buf->height; y += 1) {
        mmo_screen_buf_set(screen_buf, 0, y, &cell);
        mmo_screen_buf_set(screen_buf, screen_buf->width - 1, y, &cell);
    }

    /* Draw text in center. */

    char *title    = "Welcome to Fisherman";
    char *subtitle = "A multiplayer fishing game";

    mmo_render_text_center(title, (int)strlen(title), screen_buf->height / 2,
                           screen_buf);
    mmo_render_text_center(subtitle, (int)strlen(subtitle),
                           screen_buf->height / 2 + 1, screen_buf);
}
