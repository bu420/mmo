#ifndef MMO_GAME_H
#define MMO_GAME_H

#include <mmo/net.h>
#include <mmo/arr/char.h>
#include <mmo/arr/player.h>

typedef enum mmo_state_e {
    MMO_STATE_GREETING,
    MMO_STATE_SIGNUP,
    MMO_STATE_LOGIN,
    MMO_STATE_MAIN_GAME
} mmo_state_t;

typedef struct mmo_player_s {
    mmo_client_handle_t handle;
    mmo_state_t state;

    /* Global data accessible from all states. */
    mmo_char_arr_t name;

    /* State local data. */
    /*union {
        struct {
            bool sent_greeting;
        } greeting;
    };*/
} mmo_player_t;

typedef struct mmo_game_s {
    mmo_player_arr_t players;
} mmo_game_t;

void mmo_player_new(mmo_player_t *player, mmo_server_t *server,
                    mmo_client_handle_t handle);
void mmo_player_free(mmo_player_t *player);
void mmo_player_update(mmo_player_t *player, mmo_game_t *game,
                       mmo_server_t *server, mmo_char_arr_t *in);

void mmo_game_new(mmo_game_t *game);
void mmo_game_free(mmo_game_t *game);
void mmo_game_update(mmo_game_t *game, mmo_server_t *server);

void mmo_state_new(mmo_player_t *player, mmo_server_t *server,
                   mmo_state_t state);
void mmo_state_free(mmo_player_t *player, mmo_server_t *server,
                    mmo_state_t state);
void mmo_state_update(mmo_player_t *player, mmo_game_t *game,
                      mmo_server_t *server, mmo_char_arr_t *in,
                      mmo_state_t state);
void mmo_state_switch(mmo_player_t *player, mmo_server_t *server,
                      mmo_state_t state);

#define MMO_STATE(name)                                                        \
    void mmo_state_##name##_new(mmo_player_t *player, mmo_server_t *server);   \
    void mmo_state_##name##_free(mmo_player_t *player, mmo_server_t *server);  \
    void mmo_state_##name##_update(mmo_player_t *player, mmo_game_t *game,     \
                                   mmo_server_t *server, mmo_char_arr_t *in);

MMO_STATE(greeting)
MMO_STATE(signup)
MMO_STATE(login)
MMO_STATE(main_game)

#undef MMO_STATE

#endif
