#include <mmo/net.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <sys/socket.h>

static int mmo_socket_set_blocking(mmo_socket_t socket, bool blocking) {
    int flags = fcntl(socket, F_GETFL, 0);
    
    if (flags == -1) {
        return -1;
    }

    if (blocking) {
        flags &= ~O_NONBLOCK;
    }
    else {
        flags |= O_NONBLOCK;
    }

    if (fcntl(socket, F_SETFL, flags) == -1) {
        return -1;
    }

    return 0;
}

int mmo_server_listen(mmo_server_t *server, uint16_t port) {
    /* Create socket. */

    server->listener = socket(AF_INET, SOCK_STREAM, 0);

    if (server->listener == -1) {
        return -1;
    }

    /* Allow socket to be reusable. Avoids address-in-use error. */

    char yes = 1;
    setsockopt(server->listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    /* Set socket to non-blocking mode. */

    if (mmo_socket_set_blocking(server->listener, false) == -1) {
        return -1;
    }

    /* Bind socket. */

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server->listener, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        close(server->listener);
        return -1;
    }

    /* Listen on socket. */

    if (listen(server->listener, MMO_MAX_CLIENTS) == -1) {
        close(server->listener);
        return -1;
    }

    /* Add listener socket to polls. */

    server->sockets[0].fd = server->listener;
    server->sockets[0].events = POLLIN;
    server->socket_count = 1;

    return 0;
}

int mmo_server_poll(mmo_server_t *server, int tick_remaining_millisecs) {
    /* Poll for socket events. poll() is blocking and becomes essentially the
       server's sleep function.
       While no client sockets are present (socket_count = 1) wait indefinitely (-1).
       Otherwise wait for the duration specified in MMO_SERVER_TPS. */

    int timeout = server->socket_count == 1 ? -1 : tick_remaining_millisecs;

    switch (poll(server->sockets, (nfds_t)server->socket_count, timeout)) {
    case -1:
        return -1;

    /* Timeout. No events. */
    case 0:
        return 0;
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

                const char welcome[] = "Welcome to \x1b[38;2;255;0;100mBu's \x1b[38;2;0;100;200mTelnet server\x1b[0m\r\n>";
                send(socket, welcome, strlen(welcome), 0);

                char ip[INET_ADDRSTRLEN];
                printf("New connection from %s\n", inet_ntop(addr.ss_family, &(((struct sockaddr_in *)&addr)->sin_addr), ip, INET_ADDRSTRLEN));
            }
            /* Existing connection. */
            else {
                char bytes_received[256];
                int num_bytes_received = (int)recv(server->sockets[i].fd, bytes_received, sizeof(bytes_received), 0);

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
                        /* Set socket to non-blocking before sending data and
                           after set it back to blocking. */

                        if (mmo_socket_set_blocking(receiver, false) == -1) {
                            return -1;
                        }

                        send(receiver, bytes_received, (size_t)num_bytes_received, 0);

                        if (mmo_socket_set_blocking(receiver, true) == -1) {
                            return -1;
                        }
                    }
                }
            }
        }
    }

    return 0;
}