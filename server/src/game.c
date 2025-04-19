#include <mmo/game.h>

#include <stdio.h>

#include <mmo/arr/char.h>
#include <mmo/arr/client_input.h>
#include <mmo/net.h>
#include <mmo/arr/client.h>
#include <mmo/arr/player.h>
#include <mmo/render.h>
#include <mmo/list/player_state.h>
#include <mmo/login_state.h>
#include "mmo/player_state.h"

void mmo_player_new(mmo_player_t *player, mmo_client_handle_t handle, int terminal_width,
                    int terminal_height) {
    player->client_handle = handle;

    /* Put new players in the "login" state. */
    {
        mmo_login_state_t login_state;
        mmo_login_state_new(&login_state);

        mmo_player_state_t state;
        state.ctx       = &login_state;
        state.on_free   = mmo_login_state_free;
        state.on_update = mmo_login_state_update;
        state.on_render = mmo_login_state_render;

        mmo_player_state_list_new(&player->state_stack);
        mmo_player_state_list_append(&player->state_stack, state);
    }

    mmo_screen_buf_new(&player->screen_buf, terminal_width, terminal_height);

    mmo_char_arr_new(&player->name);
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

static void mmo_update_player_array(mmo_game_t *game, mmo_server_t *server) {
    /* Create new players. */
    for (size_t i = 0; i < server->events.new_clients.num_elems; i += 1) {
        mmo_client_handle_t handle = server->events.new_clients.elems[i];

        mmo_client_t *client = mmo_client_arr_find(&server->clients, find_client, &handle);

        mmo_player_t new_player;
        mmo_player_new(&new_player, server->events.new_clients.elems[i], client->terminal_width,
                       client->terminal_height);

        mmo_player_arr_append(&game->players, new_player);
    }

    /* Remove players. */
    for (size_t i = 0; i < server->events.removed_clients.num_elems; i += 1) {
        mmo_player_t *player = mmo_player_arr_find(&game->players, find_player,
                                                   &server->events.removed_clients.elems[i]);

        if (player) {
            mmo_player_arr_remove_from_ptr(&game->players, player);
        }
    }
}

void mmo_game_update(mmo_game_t *game, mmo_server_t *server) {
    mmo_update_player_array(game, server);

    for (size_t i = 0; i < server->events.client_inputs.num_elems; i += 1) {
        mmo_client_input_t input = server->events.client_inputs.elems[i];

        /* Format message to broadcast to players. */

        mmo_char_arr_t msg;
        mmo_char_arr_new(&msg);
        mmo_char_arr_resize(&msg, 256);

        int len = snprintf(msg.elems, msg.num_elems, "Player {%d} says: %.*s\n", input.client,
                           (int)input.input.num_elems, input.input.elems);

        msg.num_elems = (size_t)len;

        /* Broadcast message to all players except sender. */
        for (size_t j = 0; j < server->clients.num_elems; j += 1) {
            mmo_client_handle_t receiver = server->clients.elems[j].handle;

            if (receiver != input.client) {
                mmo_server_send(server, receiver, mmo_char_arr_to_view(&msg));
            }
        }

        mmo_char_arr_free(&msg);
    }
}
