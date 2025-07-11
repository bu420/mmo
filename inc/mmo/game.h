#ifndef MMO_GAME_H
#define MMO_GAME_H

#include <mmo/net.h>
#include <mmo/state.h>
#include <mmo/arr/char.h>
#include <mmo/arr/player.h>
#include <mmo/arr/bool.h>

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

#endif
