#include <stdio.h>
#include <stdlib.h>

#include <mmo/game.h>
#include <mmo/net.h>
#include <mmo/log.h>

int main(int argc, char *argv[]) {
    /* Initialize logger. */
    mmo_log_init();

    /* Read port number from command line arguments. */

    if (argc < 2) {
        mmo_log_fmt(MMO_LOG_ERROR, "Invalid arguments. Pass port number.");
        return EXIT_FAILURE;
    }

    int port = atoi(argv[1]);

    /* Initialize game. */

    mmo_game_t game;
    mmo_game_new(&game);

    /* Start server. */

    mmo_server_t server;
    mmo_server_new(&server, 100);
    mmo_server_listen(&server, port);

    mmo_log_fmt(MMO_LOG_INFO, "Server listening on port %d.", port);

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
