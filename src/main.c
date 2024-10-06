#include <stdio.h>
#include <stdlib.h>

#include <mmo/net.h>

#define PORT 23

int main() {
    mmo_server_t server;

    mmo_result_t res = mmo_server_listen(&server, PORT);

    if (res.status != MMO_OK) {
        printf("%s\n", res.msg);
        mmo_result_free(&res);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        
    }

    close(server_sock);
}