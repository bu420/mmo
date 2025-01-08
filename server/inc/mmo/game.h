#ifndef MMO_GAME_H
#define MMO_GAME_H

#include <mmo/net.h>
#include <mmo/player_arr.h>
#include <mmo/client_handle_arr.h>

typedef struct mmo_player_s {
    mmo_client_handle_t client_handle;

    char *name;
    char *gender;
} mmo_player_t;

typedef struct mmo_game_s {
    mmo_player_arr_t players;
} mmo_game_t;

void mmo_game_new(mmo_game_t *game);

void mmo_game_run(mmo_game_t *game, const mmo_client_handle_arr_t *new_clients,
                  const mmo_client_handle_arr_t *removed_clients);

#endif
