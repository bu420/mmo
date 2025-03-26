#include <stdio.h>
#include <stdlib.h>

#include <mmo/game.h>
#include <mmo/net.h>

int main(int argc, char *argv[]) {
    /* Read port number from command line arguments. */

    if (argc < 2) {
        printf("Too few arguments. Pass: <port>\nTerminating.\n");
        return -1;
    }

    int port = atoi(argv[1]);

    /* Initialize game. */

    mmo_game_t game;
    mmo_game_new(&game);

    /* Start server. */

    mmo_server_t server;
    mmo_server_new(&server, 100);

    if (mmo_server_listen(&server, port) == -1) {
        return -1;
    }

    printf("Server listening on port %d.\n", port);

    /* Main loop. */

    while (true) {
        /* Update game. */

        if (mmo_game_update(&game, &server) == -1) {
            return -1;
        }

        /* Poll for server events. */

        if (mmo_server_poll(&server, 100) == -1) {
            return -1;
        }
    }
}
