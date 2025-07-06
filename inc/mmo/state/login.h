#ifndef MMO_STATE_LOGIN_H
#define MMO_STATE_LOGIN_H

#include <mmo/arr/char.h>
#include <mmo/state/state.h>
#include <mmo/render.h>

typedef struct mmo_login_state_s {
    bool logged_in;

    mmo_char_arr_t name_input_buf;
} mmo_login_state_t;

void mmo_login_state_new(mmo_login_state_t *state);
void mmo_login_state_free(void *ctx);
void mmo_login_state_update(void *ctx, mmo_client_handle_t handle,
                            mmo_server_t *server, mmo_char_arr_t *in);
void mmo_login_state_render(void *ctx, mmo_screen_buf_t *screen_buf);

#endif
