#include <mmo/net.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <sys/socket.h>

static mmo_result_t _set_nonblocking(int socket) {
    int flags = fcntl(socket, F_GETFL, 0);
    
    if (flags == -1) {
        return mmo_result_new(MMO_ERR, "Failed to set socket in non-blocking mode: fcntl(): %s", MMO_ERRNO);
    }

    if (fcntl(socket, F_SETFL, flags | O_NONBLOCK) == -1) {
        return mmo_result_new(MMO_ERR, "Failed to set socket in non-blocking mode: fcntl(): %s", MMO_ERRNO);
    }

    return mmo_result_new(MMO_OK, NULL);
}

mmo_result_t mmo_server_listen(mmo_server_t *server, int port) {
    /* Create socket. */

    server->listener = socket(AF_INET, SOCK_STREAM, 0);

    if (server->listener == -1) {
        return mmo_result_new(MMO_ERR, "Failed to create socket: %s", MMO_ERRNO);
    }

    /* Allow socket to be reusable. Avoids address-in-use error. */

    char yes = 1;
    setsockopt(server->listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    /* Set socket to non-blocking mode. */

    mmo_result_t res = _set_nonblocking(server->listener);

    if (res.status != MMO_OK) {
        return res;
    }

    /* Bind socket. */

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server->listener, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        close(server->listener);
        return mmo_result_new(MMO_ERR, "Failed to bind socket: %s", MMO_ERRNO);
    }

    /* Listen on socket. */

    if (listen(server->listener, MMO_MAX_CLIENTS) == -1) {
        close(server->listener);
        return mmo_result_new(MMO_ERR, "Failed to listen on socket: %s", MMO_ERRNO);
    }

    /* Add listener socket to polls. */

    server->sockets[0].fd = server->listener;
    server->sockets[0].events = POLLIN;
    server->socket_count = 1;

    return mmo_result_new(MMO_OK, NULL);
}

mmo_result_t mmo_server_poll(mmo_server_t *server) {
    /* Poll for socket events (blocking).
       While no client sockets are present (socket_count = 1) wait indefinitely (-1).
       Otherwise wait for the duration specified in MMO_SERVER_TPS. */

    int timeout = server->socket_count == 1 ? -1 : 1000 / MMO_SERVER_TPS;

    switch (poll(server->sockets, server->socket_count, timeout)) {
    case -1:
        return mmo_result_new(MMO_ERR, "Failed to poll: %s", MMO_ERRNO);

    /* Timeout. No events. */
    case 0:
        return mmo_result_new(MMO_OK, NULL);
    }

    /* Check if a socket is ready to read. */
    
    for (int i = 0; i < server->socket_count; i += 1) {
        if (server->sockets[i].revents & POLLIN) {
            /* New connection. */
            if (server->sockets[i].fd == server->listener) {
                /* Accept connection. */

                struct sockaddr_storage addr;
                socklen_t len = sizeof(addr);

                mmo_socket_t socket = accept(server->listener, (struct sockaddr *)&addr, &len);

                if (socket == -1) {
                    continue;
                }

                /* Add socket to array. */

                server->sockets[server->socket_count].fd = socket;
                server->sockets[server->socket_count].events = POLLIN;
                server->socket_count += 1;

                const char welcome[] = "Welcome to \x1b[38;2;255;0;100mBu's \x1b[38;2;0;100;200mTelnet server\r\n>";
                send(socket, welcome, strlen(welcome), 0);

                char ip[INET_ADDRSTRLEN];
                printf("New connection from %s\n", inet_ntop(addr.ss_family, &(((struct sockaddr_in *)&addr)->sin_addr), ip, INET_ADDRSTRLEN));
            }
            /* Existing connection. */
            else {
                char bytes_received[256];
                int num_bytes_received = recv(server->sockets[i].fd, bytes_received, sizeof(bytes_received), 0);

                /* Close and delete client socket on disconnect or error. */
                if (num_bytes_received <= 0) {
                    if (num_bytes_received == 0) {
                        printf("Connection closed.\n");
                    }
                    
                    close(server->sockets[i].fd);

                    /* Replace this socket with last socket. */

                    server->sockets[i] = server->sockets[server->socket_count - 1];
                    server->socket_count -= 1;

                    continue;
                }
                
                /* Broadcast data to everyone else. */

                for (int j = 0; j < server->socket_count; j += 1) {
                    mmo_socket_t receiver = server->sockets[j].fd;
                    mmo_socket_t sender = server->sockets[i].fd;

                    if (receiver != server->listener && receiver != sender) {
                        send(receiver, bytes_received, num_bytes_received, 0);
                    }
                }
            }
        }
    }

    return mmo_result_new(MMO_OK, NULL);
}