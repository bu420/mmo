#ifndef MMO_GAME_H
#define MMO_GAME_H

#include <mmo/net.h>
#include <mmo/char_arr.h>
#include <mmo/player_arr.h>
#include <mmo/render.h>

typedef struct mmo_player_s {
    mmo_client_handle_t client_handle;

    mmo_screen_buf_t screen_buf;

    mmo_char_arr_t name;
} mmo_player_t;

[[nodiscard]] int mmo_player_new(mmo_player_t *player, mmo_client_handle_t handle, int terminal_width, int terminal_height);
void mmo_player_free(mmo_player_t *player);

typedef struct mmo_game_s {
    mmo_player_arr_t players;
} mmo_game_t;

void mmo_game_new(mmo_game_t *game);
void mmo_game_free(mmo_game_t *game);
[[nodiscard]] int mmo_game_update(mmo_game_t *game, mmo_server_t *server);

#endif
