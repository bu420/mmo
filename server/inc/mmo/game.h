#ifndef MMO_GAME_H
#define MMO_GAME_H

#include <mmo/net.h>
#include <mmo/char_arr.h>
#include <mmo/player_arr.h>

typedef struct mmo_player_s {
    mmo_client_handle_t client_handle;

    mmo_char_arr_t name;
    mmo_char_arr_t gender;
} mmo_player_t;

typedef struct mmo_game_s {
    mmo_player_arr_t players;
} mmo_game_t;

void mmo_game_new(mmo_game_t *game);
void mmo_game_free(mmo_game_t *game);
[[nodiscard]] int mmo_game_update(mmo_game_t *game);

#endif
