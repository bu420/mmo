#include <mmo/login_state.h>

#include <mmo/log.h>

void mmo_login_state_new(mmo_login_state_t *state) {
    state->logged_in = false;
}

void mmo_login_state_free(void *ctx) {
    (void)ctx;
}

void mmo_login_state_update(void *ctx, mmo_char_arr_span_t *client_keyboard_inputs) {
    (void)ctx;
    (void)client_keyboard_inputs;

    mmo_log_fmt(MMO_LOG_INFO, "Updating login state.");
}

void mmo_login_state_render(void *ctx) {
    (void)ctx;
}
