#ifndef MMO_STATE_STATE_H
#define MMO_STATE_STATE_H

#include <mmo/arr/char_arr.h>
#include <mmo/net.h>
#include <mmo/render.h>

typedef struct mmo_player_state_s {
    void *ctx;

    void (*free)(void *ctx);
    void (*update)(void *ctx, mmo_client_handle_t handle, mmo_server_t *server,
                   mmo_char_arr_t *in);
    void (*render)(void *ctx, mmo_screen_buf_t *screen_buf);
} mmo_player_state_t;

#endif
