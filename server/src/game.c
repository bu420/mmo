#include <mmo/game.h>

#include <stdio.h>

#include <mmo/char_arr.h>
#include <mmo/client_input_arr.h>
#include <mmo/net.h>
#include "mmo/client_arr.h"
#include "mmo/player_arr.h"
#include "mmo/render.h"

int mmo_player_new(mmo_player_t *player, mmo_client_handle_t handle, int terminal_width,
                   int terminal_height) {
    player->client_handle = handle;

    if (mmo_screen_buf_new(&player->screen_buf, terminal_width, terminal_height) == -1) {
        return -1;
    }

    mmo_char_arr_new(&player->name);

    return 0;
}

void mmo_player_free(mmo_player_t *player) {
    mmo_char_arr_free(&player->name);

    mmo_screen_buf_free(&player->screen_buf);
}

void mmo_game_new(mmo_game_t *game) { mmo_player_arr_new(&game->players); }

void mmo_game_free(mmo_game_t *game) { mmo_player_arr_free(&game->players); }

static bool find_client(const mmo_client_t *client, void *ctx) {
    return client->handle == *(mmo_client_handle_t *)ctx;
}

static bool find_player(const mmo_player_t *player, void *ctx) {
    return player->client_handle == *(mmo_client_handle_t *)ctx;
}

[[nodiscard]] static int mmo_handle_new_and_old_clients(mmo_game_t *game, mmo_server_t *server) {
    /* Create new players for new clients. */
    for (size_t i = 0; i < server->events.new_clients.num_elems; i += 1) {
        mmo_client_handle_t handle = server->events.new_clients.elems[i];

        mmo_client_t *client = mmo_client_arr_find(&server->clients, find_client, &handle);

        mmo_player_t new_player;
        if (mmo_player_new(&new_player, server->events.new_clients.elems[i], client->terminal_width,
                           client->terminal_height) == -1) {
            return -1;
        }

        if (mmo_player_arr_append(&game->players, new_player) == -1) {
            return -1;
        }
    }

    /* Remove players for disconnected clients. */
    for (size_t i = 0; i < server->events.removed_clients.num_elems; i += 1) {
        mmo_player_arr_remove_from_ptr(
            &game->players,
            mmo_player_arr_find(&game->players, find_player, &server->events.new_clients.elems[i]));
    }

    return 0;
}

int mmo_game_update(mmo_game_t *game, mmo_server_t *server) {
    if (mmo_handle_new_and_old_clients(game, server) == -1) {
        return -1;
    }

    for (size_t i = 0; i < server->events.client_inputs.num_elems; i += 1) {
        mmo_client_input_t input = server->events.client_inputs.elems[i];

        /* Format message to broadcast to players. */

        mmo_char_arr_t msg;
        mmo_char_arr_new(&msg);

        if (mmo_char_arr_resize(&msg, 256) == -1) {
            return -1;
        }

        int len = snprintf(msg.elems, msg.num_elems, "Player {%d} says: %.*s\n", input.client,
                           (int)input.input.num_elems, input.input.elems);

        msg.num_elems = (size_t)len;

        /* Broadcast message to all players except sender. */
        for (size_t j = 0; j < server->clients.num_elems; j += 1) {
            mmo_client_handle_t receiver = server->clients.elems[j].handle;

            if (receiver != input.client) {
                if (mmo_server_send(server, receiver, mmo_char_arr_to_view(&msg)) == -1) {
                    return -1;
                }
            }
        }

        mmo_char_arr_free(&msg);
    }

    return 0;
}
