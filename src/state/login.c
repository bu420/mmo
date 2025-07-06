#include <mmo/state/login.h>

#include <stdint.h>

#include <mmo/log.h>
#include <mmo/render.h>
#include <mmo/arr/char.h>
#include <mmo/input.h>

void mmo_login_state_new(mmo_login_state_t *state) {
    state->logged_in = false;

    mmo_char_arr_new(&state->name_input_buf);
}

void mmo_login_state_free(void *ctx) { (void)ctx; }

void mmo_login_state_update(void *ctx, mmo_client_handle_t handle,
                            mmo_server_t *server, mmo_char_arr_t *in) {
    (void)handle;
    (void)server;

    mmo_login_state_t *state = (mmo_login_state_t *)ctx;

    if (!state->logged_in && in->num_elems > 0) {
        mmo_parse_username(&state->name_input_buf, &state->logged_in, in);
    }
}

void mmo_login_state_render(void *ctx, mmo_screen_buf_t *screen_buf) {
    mmo_login_state_t *state = (mmo_login_state_t *)ctx;

    static char splash[] = {
#embed "../../res/login.ans"
        , '\0'};

    mmo_cell_buf_t cell_buf;
    mmo_cell_buf_parse_string(
        &cell_buf,
        (mmo_char_span_t){.elems = splash, .num_elems = strlen(splash)});

    for (int x = 0; x < cell_buf.cols; x += 1) {
        for (int y = 0; y < cell_buf.rows; y += 1) {
            mmo_screen_buf_set(screen_buf, x, y,
                               &cell_buf.cells.elems[y * cell_buf.cols + x]);
        }
    }

    mmo_cell_buf_free(&cell_buf);

    mmo_cell_t cell;
    cell.codepoint[0] = ' ';
    cell.codepoint[1] = '\0';
    cell.fg.is_set    = false;
    cell.bg.is_set    = false;

    for (int i = 0; i < MMO_USERNAME_MAX; i += 1) {
        mmo_screen_buf_set(screen_buf, 1 + i, 11, &cell);
    }

    for (int i = 0; i < (int)state->name_input_buf.num_elems; i += 1) {
        cell.codepoint[0] = state->name_input_buf.elems[i];
        mmo_screen_buf_set(screen_buf, 1 + i, 11, &cell);
    }

    if (state->logged_in) {
        char *welcome = "Logged in.";

        for (int i = 0; i < (int)strlen(welcome); i += 1) {
            cell.codepoint[0] = welcome[i];
            mmo_screen_buf_set(screen_buf, i, 13, &cell);
        }
    }
}
