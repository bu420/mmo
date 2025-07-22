#include <mmo/state.h>

#include <mmo/game.h>

void mmo_state_new(mmo_player_t *player, mmo_server_t *server,
                   mmo_state_t state) {
    switch (state) {
        case MMO_STATE_GREETING:
            mmo_state_greeting_new(player, server);
            break;

        case MMO_STATE_SIGNUP:
            mmo_state_signup_new(player, server);
            break;

        case MMO_STATE_LOGIN:
            mmo_state_login_new(player, server);
            break;

        case MMO_STATE_GAME:
            mmo_state_main_game_new(player, server);
            break;
    }
}

void mmo_state_free(mmo_player_t *player, mmo_server_t *server,
                    mmo_state_t state) {
    switch (state) {
        case MMO_STATE_GREETING:
            mmo_state_greeting_free(player, server);
            break;

        case MMO_STATE_SIGNUP:
            mmo_state_signup_free(player, server);
            break;

        case MMO_STATE_LOGIN:
            mmo_state_login_free(player, server);
            break;

        case MMO_STATE_MAIN_GAME:
            mmo_state_main_game_free(player, server);
            break;
    }
}

void mmo_state_update(mmo_player_t *player, mmo_game_t *game,
                      mmo_server_t *server, mmo_char_arr_t *in,
                      mmo_state_t state) {
    switch (state) {
        case MMO_STATE_GREETING:
            mmo_state_greeting_update(player, game, server, in);
            break;

        case MMO_STATE_SIGNUP:
            mmo_state_signup_update(player, game, server, in);
            break;

        case MMO_STATE_LOGIN:
            mmo_state_login_update(player, game, server, in);
            break;

        case MMO_STATE_MAIN_GAME:
            mmo_state_main_game_update(player, game, server, in);
            break;
    }
}

void mmo_state_switch(mmo_player_t *player, mmo_server_t *server,
                      mmo_state_t state) {
    mmo_state_free(player, server, player->state);
    mmo_state_new(player, server, state);
    player->state = state;
}
