#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <mmo/net.h>

#define PORT 2000

int main() {
    mmo_server_t server;

    if (mmo_server_listen(&server, PORT) == -1) {
        printf("mmo_server_listen()%s\n", strerror(errno));
        return -1;
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        /* Update game. */

        

        /* Poll for socket events. */

        if (mmo_server_poll(&server, 1000 / MMO_SERVER_TPS) == -1) {
            printf("mmo_server_poll()%s\n", strerror(errno));
            return -1;
        }
    }
}