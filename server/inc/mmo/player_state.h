#ifndef MMO_PLAYER_STATE_H
#define MMO_PLAYER_STATE_H

#include <mmo/arr/char_arr.h>
#include <mmo/render.h>

typedef struct mmo_player_state_s {
    void *ctx;

    void (*free)(void *ctx);
    void (*update)(void *ctx, mmo_char_arr_span_t *client_keyboard_inputs);
    void (*render)(void *ctx, mmo_screen_buf_t *screen_buf);
} mmo_player_state_t;

#endif
