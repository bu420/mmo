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
    server->num_clients = 0;
    
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

    return 0;
}

int mmo_server_poll(mmo_server_t *server, int tick_remaining_millisecs) {
    /* Setup poll structure. Set first element to listener socket,
       fill rest with client sockets. */

    struct pollfd sockets[MMO_MAX_CLIENTS + 1];
    int num_sockets = server->num_clients + 1;

    sockets[0].fd = server->listener;
    sockets[0].events = POLLIN;

    for (int i = 0; i < server->num_clients; i += 1) {
        sockets[1 + i].fd = server->clients[i].socket;
        sockets[1 + i].events = POLLIN;
    }
    
    /* Poll for socket events. poll() blocks and becomes server's 
       sleep function. While no clients: wait indefinitely (-1), 
       otherwise wait for remaining tick time. */

    int timeout = server->num_clients == 0 ? -1 : tick_remaining_millisecs;

    switch (poll(sockets, (nfds_t)num_sockets, timeout)) {
    case -1:
        return -1;

    /* Timeout. No events. */
    case 0:
        return 0;
    }

    /* Check if listener socket has been notified of new connection.
       Check if client socket has received data. */
    
    for (int i = 0; i < num_sockets; i += 1) {
        if (sockets[i].revents & POLLIN) {
            /* New connection. */
            if (sockets[i].fd == server->listener) {
                /* Accept connection. */

                struct sockaddr_storage addr;
                socklen_t len = sizeof(addr);

                mmo_socket_t socket = accept(server->listener, (struct sockaddr *)&addr, &len);

                if (socket == -1) {
                    continue;
                }

                /* Set socket to non-blocking mode. */

                if (mmo_socket_set_blocking(socket, false) == -1) {
                    return -1;
                }

                /* If too many clients, reject socket with message. */

                if (server->num_clients == MMO_MAX_CLIENTS) {
                    const char response[] = "Connection refused. Server is full.";

                    send(socket, response, strlen(response), 0);

                    close(socket);

                    continue;
                }

                /* Add socket to array. */

                mmo_client_t client;
                client.socket = socket;
                mmo_char_arr_new(&client.in, 0);
                mmo_char_arr_new(&client.out, 0);

                server->clients[server->num_clients] = client;
                server->num_clients += 1;

                const char welcome[] = "Welcome to \x1b[38;2;255;0;100mBu's \x1b[38;2;0;100;200mTelnet server\x1b[0m\r\n>";
                send(socket, welcome, strlen(welcome), 0);

                char ip[INET_ADDRSTRLEN];
                printf("New connection from %s.\n", inet_ntop(addr.ss_family, &(((struct sockaddr_in *)&addr)->sin_addr), ip, INET_ADDRSTRLEN));
            }
            /* Existing connection. */
            else {
                char bytes[256];
                int num_bytes = (int)recv(sockets[i].fd, bytes, sizeof(bytes), 0);

                /* Client disconnected gracefully. */
                if (num_bytes == 0) {
                    printf("Connection closed.\n");

                    close(sockets[i].fd);

                    /* Simple remove. Replace element with last tracked element in array. */

                    sockets[i] = sockets[num_sockets - 1];
                    num_sockets -= 1;

                    int client_idx = i - 1;

                    mmo_char_arr_free(&server->clients[client_idx].in);
                    mmo_char_arr_free(&server->clients[client_idx].out);

                    server->clients[client_idx] = server->clients[server->num_clients - 1];
                    server->num_clients -= 1;

                    continue;
                }

                /* Error. */
                if (num_bytes == -1) {
                    close(sockets[i].fd);
                    
                    return -1;
                }
                
                /* Broadcast data to everyone else. */

                for (int j = 1; j < num_sockets; j += 1) {
                    mmo_socket_t receiver = sockets[j].fd;
                    mmo_socket_t sender = sockets[i].fd;

                    if (receiver != sender) {
                        send(receiver, bytes, (size_t)num_bytes, 0);
                    }
                }
            }
        }
    }

    return 0;
}