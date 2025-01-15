#include <mmo/game.h>

void mmo_game_new(mmo_game_t *game) {
    mmo_player_arr_new(&game->players);
}

void mmo_game_free(mmo_game_t *game) {
    mmo_player_arr_free(&game->players);
}

int mmo_game_update(mmo_game_t *game) {
    (void)game;

    return 0;
}
