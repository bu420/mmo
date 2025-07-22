#include <mmo/game.h>

#include <mmo/io.h>

void mmo_state_login_new(mmo_player_t *player, mmo_server_t *server) {
    mmo_print_fmt(player, server, MMO_PRINT_AFTER_ENTER,
                  "Enter your username.");
}

void mmo_state_login_free(mmo_player_t *player, mmo_server_t *server) {
}

void mmo_state_login_update(mmo_player_t *player, mmo_game_t *game,
                            mmo_server_t *server, mmo_char_arr_t *in) {
    mmo_char_arr_t line;
    mmo_char_arr_new(&line);

    if (mmo_get_line(&line, in)) {
        mmo_prompt(player, server);
    }
}
