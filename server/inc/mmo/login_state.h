#ifndef MMO_LOGIN_STATE_H
#define MMO_LOGIN_STATE_H

#include <mmo/player_state.h>

typedef struct mmo_login_state_s {
    bool logged_in;
} mmo_login_state_t;

void mmo_login_state_new(mmo_login_state_t *state);
void mmo_login_state_free(void *ctx);
void mmo_login_state_update(void *ctx, mmo_char_arr_span_t *client_keyboard_inputs);
void mmo_login_state_render(void *ctx, mmo_screen_buf_t *screen_buf);

#endif
