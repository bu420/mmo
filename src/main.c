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
        int poll_result = poll(fds, 1, -1);

        if (poll_result == -1) {
            perror("Failed to poll");
            break;
        }

        if (fds[0].revents & POLLIN) {
            struct sockaddr_in client_addr;
            int addr_len = sizeof(client_addr);

            int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len);

            if (client_sock == -1) {
                perror("Failed to accept client connection");
                continue;
            }

            printf("New client connected: %d\n", client_sock);

            send(client_sock, "Hello World from Bu's Telnet server", 36, 0);
            close(client_sock);
        }
    }

    close(server_sock);
}