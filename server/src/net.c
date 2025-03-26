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
#include <mmo/pollfd_arr.h>
#include "mmo/char_arr.h"
#include "mmo/client_handle_arr.h"
#include "mmo/client_input_arr.h"

[[nodiscard]] static int mmo_socket_set_blocking(mmo_socket_t socket, bool blocking) {
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

void mmo_server_new(mmo_server_t *server, int num_max_clients) {
    server->num_max_clients = num_max_clients;

    mmo_client_arr_new(&server->clients);
    mmo_client_handle_arr_new(&server->events.new_clients);
    mmo_client_handle_arr_new(&server->events.removed_clients);
    mmo_client_input_arr_new(&server->events.client_inputs);
    mmo_client_handle_arr_new(&server->events.new_terminal_sizes);
}

void mmo_server_free(mmo_server_t *server) {
    mmo_client_handle_arr_free(&server->events.new_terminal_sizes);
    mmo_client_input_arr_free(&server->events.client_inputs);
    mmo_client_handle_arr_free(&server->events.removed_clients);
    mmo_client_handle_arr_free(&server->events.new_clients);
    mmo_client_arr_free(&server->clients);
}

int mmo_server_listen(mmo_server_t *server, int port) {
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
    addr.sin_port        = htons((uint16_t)port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server->listener, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        close(server->listener);
        return -1;
    }

    /* Listen on socket. */

    if (listen(server->listener, server->num_max_clients) == -1) {
        close(server->listener);
        return -1;
    }

    return 0;
}

[[nodiscard]] static int mmo_disconnect_client(mmo_server_t *server, mmo_client_t *client) {
    if (mmo_client_handle_arr_append(&server->events.removed_clients, client->handle) == -1) {
        return -1;
    }

    printf("Client left (%s).\n", client->ip);

    close(client->socket);

    mmo_char_arr_free(&client->in);
    mmo_char_arr_free(&client->out);

    mmo_client_arr_remove_from_ptr(&server->clients, client);

    return 0;
}

int mmo_server_poll(mmo_server_t *server, int timeout_millisecs) {
    /* Reset events. */
    mmo_client_handle_arr_clear(&server->events.new_clients);
    mmo_client_handle_arr_clear(&server->events.removed_clients);
    mmo_client_input_arr_clear(&server->events.client_inputs);
    mmo_client_handle_arr_clear(&server->events.new_terminal_sizes);

    /* Array of file descriptors (sockets) to be polled.
       Set first element to listener socket and rest to client sockets. */

    mmo_pollfd_arr_t polled_sockets;
    mmo_pollfd_arr_new(&polled_sockets);

    if (mmo_pollfd_arr_append(&polled_sockets,
                              (struct pollfd){.fd = server->listener, .events = POLLIN}) == -1) {
        return -1;
    }

    for (size_t i = 0; i < server->clients.num_elems; i += 1) {
        mmo_client_t *client = &server->clients.elems[i];

        /* Check if client is set to be removed. */
        if (client->state == MMO_CLIENT_STATE_TO_BE_REMOVED) {
            if (mmo_disconnect_client(server, client) == -1) {
                return -1;
            }

            continue;
        }

        /* Check If client has successfully sent a valid handshake packet.
           If so, notify server owner of a new user. */
        if (client->state == MMO_CLIENT_STATE_VERIFIED) {
            client->state = MMO_CLIENT_STATE_ONLINE;

            if (mmo_client_handle_arr_append(&server->events.new_clients, client->socket) == -1) {
                return -1;
            }
        }

        /* Otherwise add client socket to array of sockets to be polled for events. */
        if (mmo_pollfd_arr_append(
                &polled_sockets,
                (struct pollfd){.fd = server->clients.elems[i].socket, .events = POLLIN}) == -1) {
            return -1;
        }
    }

    /* poll(): Block until there's an event on atleast one socket.
       While there are no connected clients, wait indefinitely (-1). */

    int timeout = /*server->clients.num_elems == 0 ? -1 : */ timeout_millisecs;

    switch (poll(polled_sockets.elems, (nfds_t)polled_sockets.num_elems, timeout)) {
        case -1: return -1;

        /* Timeout. No events. */
        case 0: goto end;
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

        if (server->clients.num_elems == (size_t)server->num_max_clients) {
            const char response[] = "Connection refused. Server is full.";

            send(socket, response, strlen(response), 0);

            close(socket);

            goto skip;
        }

        /* Create client and add to array. */

        mmo_client_t client;
        client.socket = socket;
        client.state  = MMO_CLIENT_STATE_AWAITING_HANDSHAKE;

        mmo_char_arr_new(&client.in);
        mmo_char_arr_new(&client.out);

        if (!inet_ntop(addr.ss_family, &(((struct sockaddr_in *)&addr)->sin_addr), client.ip,
                       INET_ADDRSTRLEN)) {
            return -1;
        }

        if (mmo_client_arr_append(&server->clients, client) == -1) {
            return -1;
        }

        if (mmo_client_handle_arr_append(&server->events.new_clients, client.handle) == -1) {
            return -1;
        }

        printf("New client (%s).\n", client.ip);
    }

skip:
    /* Check if client sockets have received data. */

    for (size_t i = 1; i < polled_sockets.num_elems; i += 1) {
        if (polled_sockets.elems[i].revents & POLLIN) {
            struct pollfd polled_client_socket = polled_sockets.elems[i];

            char bytes[512];
            int num_bytes = (int)recv(polled_client_socket.fd, bytes, sizeof(bytes), 0);

            mmo_client_t *client = &server->clients.elems[i - 1];

            /* Client disconnected gracefully (0) or error (-1). */
            if (num_bytes <= 0) {
                if (mmo_client_handle_arr_append(&server->events.removed_clients, client->handle) ==
                    -1) {
                    return -1;
                }

                if (mmo_disconnect_client(server, client) == -1) {
                    return -1;
                }

                continue;
            }

            /* Append received data to array. */
            for (int j = 0; j < num_bytes; j += 1) {
                if (mmo_char_arr_append(&client->in, bytes[j]) == -1) {
                    return -1;
                }
            }

            /* Parse data into packets. */

            if (mmo_has_received_complete_packet(mmo_char_arr_to_view(&client->in))) {
                if (!mmo_handle_packet(&client->in, client, server)) {
                    /* On malformed packet remove client. */

                    if (mmo_client_handle_arr_append(&server->events.removed_clients,
                                                     client->socket) == -1) {
                        return -1;
                    }

                    if (mmo_disconnect_client(server, client) == -1) {
                        return -1;
                    }
                }
            }
        }
    }

end:
    mmo_pollfd_arr_free(&polled_sockets);

    /* Send outgoing data to clients. */

    for (size_t i = 0; i < server->clients.num_elems; i += 1) {
        mmo_client_t *client = &server->clients.elems[i];

        if (client->out.num_elems > 0) {
            ssize_t num_bytes_sent =
                send(client->socket, client->out.elems, client->out.num_elems, 0);

            if (num_bytes_sent != -1) {
                /* Pop sent data from stream. */
                for (ssize_t j = 0; j < num_bytes_sent; j += 1) {
                    mmo_char_arr_remove(&client->out, 0);
                }
            }
        }
    }

    return 0;
}

void mmo_server_remove_client(mmo_server_t *server, mmo_client_handle_t handle) {
    for (size_t i = 0; i < server->clients.num_elems; i += 1) {
        if (server->clients.elems[i].handle == handle) {
            server->clients.elems[i].state = MMO_CLIENT_STATE_TO_BE_REMOVED;
            break;
        }
    }
}

int mmo_server_send(mmo_server_t *server, mmo_client_handle_t handle, mmo_char_arr_view_t msg) {
    for (size_t i = 0; i < server->clients.num_elems; i += 1) {
        if (server->clients.elems[i].handle == handle) {
            /* Append message to outgoing stream. */
            for (size_t j = 0; j < msg.num_elems; j += 1) {
                if (mmo_char_arr_append(&server->clients.elems[i].out, msg.elems[j]) == -1) {
                    return -1;
                }
            }

            return 0;
        }
    }

    return -1;
}
