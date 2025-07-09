#include <stdlib.h>

#include <mmo/game.h>
#include <mmo/net.h>
#include <mmo/log.h>
#include <unistd.h>

#define MMO_DEFAULT_PORT 2000

int main(int argc, char *argv[]) {
    /* Initialize logger. */
    mmo_log_init();

    /* Initialize game. */

    mmo_game_t game;
    mmo_game_new(&game);

    /* Start server. */

    int port = argc < 2 ? MMO_DEFAULT_PORT : atoi(argv[1]);

    mmo_server_t server;
    mmo_server_new(&server);
    mmo_server_listen(&server, port);

    mmo_log_fmt(MMO_LOG_INFO, "Server listening on port %d.", port);

    /* Main loop. */

    while (true) {
        /* Poll for server events. */
        mmo_server_poll(&server);

        /* Update game. */
        mmo_game_update(&game, &server);
    }
}
