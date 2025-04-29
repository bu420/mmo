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
#include <string.h>
#include "mmo/arr/char_arr.h"
#include "mmo/player_state.h"
#include "mmo/str.h"

void mmo_player_new(mmo_player_t *player, mmo_server_t *server, mmo_client_handle_t handle,
                    int terminal_width, int terminal_height) {
    player->client_handle = handle;

    /* Put new players in the "login" state. */
    {
        mmo_login_state_t login_state;
        mmo_login_state_new(&login_state);

        mmo_player_state_t state;
        state.ctx    = &login_state;
        state.free   = mmo_login_state_free;
        state.update = mmo_login_state_update;
        state.render = mmo_login_state_render;

        mmo_player_state_list_new(&player->state_stack);
        mmo_player_state_list_append(&player->state_stack, &state);
    }

    mmo_screen_buf_new(&player->screen_buf, terminal_width, terminal_height);

    mmo_char_arr_new(&player->name);

    /* Clear client screen. */
    mmo_server_send(server, handle,
                    &(mmo_char_arr_t){.elems     = MMO_ANSI_CLEAR_SCREEN,
                                      .num_elems = MMO_STR_LEN(MMO_ANSI_CLEAR_SCREEN)});
}

void mmo_player_free(mmo_player_t *player) {
    mmo_char_arr_free(&player->name);

    mmo_screen_buf_free(&player->screen_buf);

    /* Free up state stack (list). */

    for (mmo_player_state_list_node_t *state = player->state_stack.head; state;
         state                               = state->next) {
        state->data.free(state->data.ctx);
    }

    mmo_player_state_list_free(&player->state_stack);
}

void mmo_game_new(mmo_game_t *game) { mmo_player_arr_new(&game->players); }

void mmo_game_free(mmo_game_t *game) { mmo_player_arr_free(&game->players); }

static bool find_client(const mmo_client_t *client, void *ctx) {
    return client->handle == *(mmo_client_handle_t *)ctx;
}

static bool find_player(const mmo_player_t *player, void *ctx) {
    return player->client_handle == *(mmo_client_handle_t *)ctx;
}

static void mmo_handle_new_and_old_clients(mmo_game_t *game, mmo_server_t *server) {
    /* Create new players. */
    for (size_t i = 0; i < server->events.new_clients.num_elems; i += 1) {
        mmo_client_handle_t handle = server->events.new_clients.elems[i];

        mmo_client_t *client = mmo_client_arr_find(&server->clients, find_client, &handle);

        mmo_player_t new_player;
        mmo_player_new(&new_player, server, server->events.new_clients.elems[i], client->terminal_width,
                       client->terminal_height);

        mmo_player_arr_append(&game->players, &new_player);
    }

    /* Remove players. */
    for (size_t i = 0; i < server->events.removed_clients.num_elems; i += 1) {
        mmo_player_t *player = mmo_player_arr_find(&game->players, find_player,
                                                   &server->events.removed_clients.elems[i]);

        assert(player);

        mmo_player_free(player);

        if (player) {
            mmo_player_arr_remove_from_ptr(&game->players, player);
        }
    }
}

void mmo_game_update(mmo_game_t *game, mmo_server_t *server) {
    mmo_handle_new_and_old_clients(game, server);

    /* Update and render player states. */
    for (size_t i = 0; i < game->players.num_elems; i += 1) {
        mmo_player_t *player = &game->players.elems[i];

        /* Bundle up all client keyboard input events for the current player. */

        mmo_char_arr_arr_t inputs;
        mmo_char_arr_arr_new(&inputs);

        for (size_t ii = 0; ii < server->events.client_inputs.num_elems; ii += 1) {
            mmo_client_input_t *input = &server->events.client_inputs.elems[ii];

            if (player->client_handle == input->client) {
                mmo_char_arr_arr_append(&inputs, &input->input);
            }
        }

        /* Get the current player state. */
        mmo_player_state_t *state = &player->state_stack.tail->data;

        /* Invoke update callback for player and send keyboard inputs. */
        state->update(state->ctx, (mmo_char_arr_span_t *)&inputs);

        /* Invoke render callback for player. */
        state->render(state->ctx, &player->screen_buf);

        /* Convert player screen buffer to byte array and send to client. */

        mmo_char_arr_t bytes;
        mmo_char_arr_new(&bytes);

        mmo_screen_buf_to_str(&player->screen_buf, &bytes);
        memset(player->screen_buf.cells_modified_flags.elems, false,
               player->screen_buf.cells_modified_flags.num_elems);

        mmo_server_send(server, player->client_handle, &bytes);
    }
}
