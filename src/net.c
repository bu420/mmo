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

#include <mmo/packet.h>
#include <mmo/arr/pollfd.h>

static int mmo_socket_set_blocking(mmo_socket_t socket, bool blocking) {
    int flags = fcntl(socket, F_GETFL, 0);

    if (flags == -1) {
        return -1;
    }

    if (blocking) {
        flags &= ~O_NONBLOCK;
    } else {
        flags |= O_NONBLOCK;
    }

    if (fcntl(socket, F_SETFL, flags) == -1) {
        return -1;
    }

    return 0;
}

int mmo_server_listen(mmo_server_t *server, uint16_t port) {
    if (mmo_client_arr_new(&server->clients, 0) == -1) {
        return -1;
    }

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
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);
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
    /* Array of file descriptors (sockets) to be polled.
       Set first element to listener socket and rest to client sockets. */

    mmo_pollfd_arr_t polled_sockets;

    if (mmo_pollfd_arr_new(&polled_sockets, 0) == -1) {
        return -1;
    }

    if (mmo_pollfd_arr_append(&polled_sockets, (struct pollfd){.fd = server->listener, .events = POLLIN}) ==
        -1) {
        return -1;
    }

    for (size_t i = 0; i < server->clients.num_elems; i += 1) {
        if (mmo_pollfd_arr_append(&polled_sockets, (struct pollfd){.fd     = server->clients.elems[i].socket,
                                                                   .events = POLLIN}) == -1) {
            return -1;
        }
    }

    /* poll(): Block until there's an event on atleast one socket.
       While there are no connected clients, wait indefinitely (-1). */

    int timeout = server->clients.num_elems == 0 ? -1 : tick_remaining_millisecs;

    switch (poll(polled_sockets.elems, (nfds_t)polled_sockets.num_elems, timeout)) {
        case -1: return -1;

        /* Timeout. No events. */
        case 0: return 0;
    }

    /* Check listener socket for incoming connection. */

    if (polled_sockets.elems[0].revents & POLLIN) {
        /* Accept connection. */

        struct sockaddr_storage addr;
        socklen_t len = sizeof(addr);

        mmo_socket_t socket = accept(server->listener, (struct sockaddr *)&addr, &len);

        if (socket == -1) {
            goto skip;
        }

        /* Set socket to non-blocking mode. */

        if (mmo_socket_set_blocking(socket, false) == -1) {
            return -1;
        }

        /* If too many clients, reject socket with message. */

        if (server->clients.num_elems == MMO_MAX_CLIENTS) {
            const char response[] = "Connection refused. Server is full.";

            send(socket, response, strlen(response), 0);

            close(socket);

            goto skip;
        }

        /* Create client and add to array. */

        mmo_client_t client;
        client.socket = socket;
        client.has_shook_hands = false;

        if (mmo_char_arr_new(&client.in, 0) == -1) {
            return -1;
        }

        if (mmo_char_arr_new(&client.out, 0) == -1) {
            return -1;
        }

        if (!inet_ntop(addr.ss_family, &(((struct sockaddr_in *)&addr)->sin_addr), client.ip,
                       INET_ADDRSTRLEN)) {
            return -1;
        }

        mmo_client_arr_append(&server->clients, client);
    }

skip:
    /* Check if client sockets have received data. */

    for (size_t i = 1; i < polled_sockets.num_elems; i += 1) {
        if (polled_sockets.elems[i].revents & POLLIN) {
            struct pollfd polled_client_socket = polled_sockets.elems[i];

            char bytes[256];
            int num_bytes = (int)recv(polled_client_socket.fd, bytes, sizeof(bytes), 0);

            mmo_client_t *client = &server->clients.elems[i - 1];

            /* Client disconnected gracefully. */
            if (num_bytes == 0) {
                printf("Connection closed.\n");

                close(polled_client_socket.fd);

                /* Remove client. */

                mmo_pollfd_arr_remove(&polled_sockets, i);
                i -= 1;

                mmo_char_arr_free(&client->in);
                mmo_char_arr_free(&client->out);
                mmo_client_arr_remove_from_ptr(&server->clients, client);

                continue;
            }

            /* Error. */
            if (num_bytes == -1) {
                close(polled_client_socket.fd);

                return -1;
            }

            /* Append data to array. */
            for (int j = 0; j < num_bytes; j += 1) {
                if (mmo_char_arr_append(&client->in, bytes[j]) == -1) {
                    return -1;
                }
            }

            /* Parse data into packets. */

            if (mmo_has_received_complete_packet(&client->in)) {
                if (mmo_handle_packet(&client->in, client) == -1) {
                    /* On malformed packet remove client. */

                    mmo_pollfd_arr_remove(&polled_sockets, i);
                    i -= 1;

                    mmo_char_arr_free(&client->in);
                    mmo_char_arr_free(&client->out);
                    mmo_client_arr_remove_from_ptr(&server->clients, client);
                }
            }
        }
    }

    return 0;
}