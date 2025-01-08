#include <stdio.h>
#include <stdlib.h>

#include <mmo/game.h>
#include <mmo/net.h>

#define TPS 10

void on_connect(mmo_client_handle_t client) {
    printf("New client.\n");
}

void on_disconnect(mmo_client_handle_t client) {
    printf("Client left.\n");
}

void on_input(mmo_client_handle_t client, mmo_string_view_t text) {
    printf("Client input: %.*s\n", text.num_elems, text.elems);
}

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

    /* Main loop. */

    while (true) {
        /* Poll for server events. */

        if (mmo_server_poll(&server, 1000 / TPS, on_connect, on_disconnect, on_input) == -1) {
            return -1;
        }

        /* Update game. */

        /*if (mmo_game_run(&game) == -1) {
            return -1;
        }*/
    }
}
