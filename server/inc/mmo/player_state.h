#ifndef MMO_PLAYER_STATE_H
#define MMO_PLAYER_STATE_H

typedef struct mmo_player_state_s {
    void *ctx;

    void (*on_free)(void *ctx);
    void (*on_update)(void *ctx);
    void (*on_render)(void *ctx);
} mmo_player_state_t;

#endif
