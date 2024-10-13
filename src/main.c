#include <stdio.h>
#include <stdlib.h>

#include <mmo/net.h>

#define PORT 2000

int main() {
    mmo_server_t server;

    mmo_result_t res = mmo_server_listen(&server, PORT);

    if (res.status != MMO_OK) {
        printf("%s\n", res.msg);
        return -1;
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        /* Update game. */

        

        /* Poll for socket events. */
        mmo_result_t res = mmo_server_poll(&server);

        if (res.status == MMO_ERR) {
            printf("Failed to update server: %s\n", res.msg);
            return -1;
        }
    }
}