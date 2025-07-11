#include <mmo/state.h>

#include <mmo/game.h>
#include <mmo/io.h>

void mmo_state_greeting_new(mmo_player_t *player, mmo_server_t *server) {
    assert(player);
    assert(server);

    static char greeting[] = {
#embed "../greeting.ans"
    };

    mmo_server_send(
        server, player->handle,
        (mmo_char_span_t){.elems = greeting, .num_elems = sizeof greeting});

    mmo_prompt(player, server);
}

void mmo_state_greeting_free(mmo_player_t *player, mmo_server_t *server) {
    assert(player);
    assert(server);
}

void mmo_state_greeting_update(mmo_player_t *player, mmo_game_t *game,
                               mmo_server_t *server, mmo_char_arr_t *in) {
    assert(player);
    assert(game);
    assert(server);
    assert(in);

    mmo_char_arr_t line;
    mmo_char_arr_new(&line);

    if (mmo_get_line(&line, in)) {
        if (line.num_elems == 0) {
            mmo_prompt(player, server);
        }

        else {
            if (mmo_str_eq_ignore_case(MMO_ARR_TO_SPAN(line),
                                       MMO_LITERAL_TO_SPAN("new"))) {
                mmo_state_switch(player, server, MMO_STATE_SIGNUP);

            } else {
                mmo_state_switch(player, server, MMO_STATE_LOGIN);
            }
        }
    }

    mmo_char_arr_free(&line);
}
