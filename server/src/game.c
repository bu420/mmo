#include <mmo/game.h>

int mmo_game_new(mmo_game_t *game) {
    if (mmo_player_arr_new(&game->players, 0) == -1) {
	return -1;
    }
}

int mmo_game_run(mmo_game_t *game, const mmo_client_handle_arr_t *new_clients,
                 const mmo_client_handle_arr_t *removed_clients) {
    
}
