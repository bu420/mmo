#include <mmo/game.h>

#include <stdio.h>

#include <mmo/char_arr.h>
#include <mmo/client_input_arr.h>
#include <mmo/net.h>

void mmo_game_new(mmo_game_t *game) { mmo_player_arr_new(&game->players); }

void mmo_game_free(mmo_game_t *game) { mmo_player_arr_free(&game->players); }

int mmo_game_update(mmo_game_t *game, mmo_server_t *server) {
    (void)game;

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

        /* Broadcast input to all players except sender. */
        for (size_t j = 0; j < server->clients.num_elems; j += 1) {
            mmo_client_handle_t receiver = server->clients.elems[j].handle;

            if (receiver != input.client) {
                if (mmo_server_send(server, receiver, mmo_char_arr_to_view(&msg)) == -1) {
                    return -1;
                }
            }
        }
    }

    return 0;
}
