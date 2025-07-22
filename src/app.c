#include <mmo/game.h>

#include <assert.h>
#include <stdarg.h>

#include <mmo/arr/char.h>
#include <mmo/net.h>
#include <mmo/arr/client.h>
#include <mmo/arr/player.h>
#include <mmo/arr/char_arr.h>
#include <mmo/io.h>

void mmo_user_new(mmo_player_t *player, mmo_server_t *server,
                    mmo_client_handle_t handle) {
    player->handle = handle;

    mmo_char_arr_new(&player->name);

    player->state = MMO_STATE_GREETING;
    mmo_state_greeting_new(player, server);
}

void mmo_player_free(mmo_player_t *player) { mmo_char_arr_free(&player->name); }

void mmo_player_update(mmo_player_t *player, mmo_game_t *game,
                       mmo_server_t *server, mmo_char_arr_t *in) {
    mmo_state_update(player, game, server, in, player->state);
}

void mmo_game_new(mmo_game_t *game) { mmo_player_arr_new(&game->players); }

void mmo_game_free(mmo_game_t *game) { mmo_player_arr_free(&game->players); }

static bool mmo_find_player(mmo_player_t *player, void *ctx) {
    return player->handle == *(mmo_client_handle_t *)ctx;
}

static bool mmo_find_client(mmo_client_t *client, void *ctx) {
    return client->handle == *(mmo_client_handle_t *)ctx;
}

void mmo_game_update(mmo_game_t *game, mmo_server_t *server) {
    MMO_FOREACH(server->clients, client) {
        if (client->state == MMO_CLIENT_STATE_NEW) {
            mmo_player_t new_player;
            mmo_player_new(&new_player, server, client->handle);

            mmo_player_arr_append(&game->players, &new_player);
        }

        else if (client->state == MMO_CLIENT_STATE_TO_BE_REMOVED) {
            mmo_player_t *player = mmo_player_arr_find(
                &game->players, mmo_find_player, &client->handle);
            assert(player);

            mmo_player_free(player);
            mmo_player_arr_remove_from_ptr(&game->players, player);
        }
    }

    /* Update players. */
    MMO_FOREACH(game->players, player) {
        mmo_client_t *client = mmo_client_arr_find(
            &server->clients, mmo_find_client, &player->handle);
        assert(client);

        mmo_player_update(player, game, server, &client->in);
    }
}
