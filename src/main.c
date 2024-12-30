#include <stdio.h>
#include <stdlib.h>

#include <mmo/game.h>

#define TPS 10

int main(int argc, char *argv[]) {
    /* Read port number from command line arguments. */

    if (argc < 2) {
        printf("Too few arguments. Pass: <port>\nTerminating.\n");
        return -1;
    }

    int port = atoi(argv[1]);

    /* Initialize game. */

    mmo_game_t game;

    if (mmo_game_new(&game) == -1) {
        return -1;
    }

    /* Start server. */

    mmo_server_t server;

    if (mmo_server_listen(&server, port) == -1) {
        return -1;
    }

    printf("Server listening on port %d.\n", port);

    mmo_client_handle_arr_t new_clients;
    mmo_client_handle_arr_t removed_clients;

    if (mmo_client_handle_arr_new(&new_clients, 0) == -1 ||
        mmo_client_handle_arr_new(&removed_clients, 0) == -1) {
        return -1;
    }

    /* Main loop. */

    while (true) {
        /* Poll for server events. */

        if (mmo_server_poll(&server, 1000 / TPS, &new_clients, &removed_clients) == -1) {
            return -1;
        }

        /* Update game. */

        if (mmo_game_run(&game, &new_clients, &removed_clients) == -1) {
            return -1;
        }

        /* Cleanup. */

        mmo_client_handle_arr_free(&new_clients);
        mmo_client_handle_arr_free(&removed_clients);
    }
}
