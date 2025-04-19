#ifndef MMO_PLAYER_STATE_H
#define MMO_PLAYER_STATE_H

typedef struct mmo_player_state_s {
    void (*on_new)();
    void (*on_free)();
    void (*on_update)();
    void (*on_render)();
} mmo_player_state_t;

#endif
