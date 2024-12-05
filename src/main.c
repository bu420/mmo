#include <stdio.h>

#include <mmo/net.h>

#define PORT 2000
#define TPS 10

int main() {
    mmo_server_t server;

    if (mmo_server_listen(&server, PORT) == -1) {
        return -1;
    }

    printf("Server listening on port %d.\n", PORT);

    while (true) {
        /* Update game. */

        

        /* Poll for socket events. */

        if (mmo_server_poll(&server, 1000 / TPS) == -1) {
            return -1;
        }
    }
}