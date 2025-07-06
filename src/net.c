#include <mmo/net.h>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <errno.h>

#include <mmo/telnet.h>
#include <mmo/arr/pollfd.h>
#include <mmo/arr/char.h>
#include <mmo/arr/client_handle.h>
#include <mmo/log.h>

[[nodiscard]] static int mmo_socket_set_blocking(mmo_socket_t socket,
                                                 bool blocking) {
    int flags = fcntl(socket, F_GETFL, 0);

    if (flags == -1) {
        return EXIT_FAILURE;
    }

    if (blocking) {
        flags &= ~O_NONBLOCK;
    } else {
        flags |= O_NONBLOCK;
    }

    if (fcntl(socket, F_SETFL, flags) == -1) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void mmo_server_new(mmo_server_t *server, int num_max_clients) {
    assert(server);

    server->num_max_clients = num_max_clients;

    mmo_client_arr_new(&server->clients);
    mmo_client_handle_arr_new(&server->events.new_clients);
    mmo_client_handle_arr_new(&server->events.removed_clients);
}

void mmo_server_free(mmo_server_t *server) {
    assert(server);

    mmo_client_handle_arr_free(&server->events.removed_clients);
    mmo_client_handle_arr_free(&server->events.new_clients);
    mmo_client_arr_free(&server->clients);
}

void mmo_server_listen(mmo_server_t *server, int port) {
    assert(server);

    /* Create socket. */

    server->listener = socket(AF_INET, SOCK_STREAM, 0);

    if (server->listener == -1) {
        mmo_log_fmt(MMO_LOG_ERROR,
                    "socket(): failed to create listener socket.");
        exit(EXIT_FAILURE);
    }

    /* Set socket to non-blocking mode. */

    if (mmo_socket_set_blocking(server->listener, false) == -1) {
        mmo_log_fmt(MMO_LOG_ERROR, "mmo_socket_set_blocking(): failed to set "
                                   "listener socket to non-blocking.");
        exit(EXIT_FAILURE);
    }

    /* Allow socket to be reusable. Avoids address-in-use error. */
    int yes = 1;
    setsockopt(server->listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    /* Bind socket. */

    struct sockaddr_in addr;
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons((uint16_t)port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server->listener, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        mmo_log_fmt(MMO_LOG_ERROR, "bind(): failed to bind listener socket.");
        exit(EXIT_FAILURE);
    }

    /* Listen on socket. */

    if (listen(server->listener, server->num_max_clients) == -1) {
        mmo_log_fmt(MMO_LOG_ERROR, "listen(): failed to listen.");
        exit(EXIT_FAILURE);
    }
}

static void mmo_disconnect_client(mmo_server_t *server, mmo_client_t *client) {
    assert(server);
    assert(client);

    mmo_client_handle_arr_append(&server->events.removed_clients,
                                 &client->handle);

    mmo_log_fmt(MMO_LOG_INFO, "Client disconnected (%s).", client->ip);

    close(client->socket);

    mmo_char_arr_free(&client->in);
    mmo_char_arr_free(&client->out);
    mmo_telopt_arr_free(&client->telnet.opts);
    mmo_char_arr_free(&client->telnet.sb_buf);

    mmo_client_arr_remove_from_ptr(&server->clients, client);
}

static void mmo_handle_incoming_connection(mmo_server_t *server) {
    assert(server);

    /* Accept connection. */

    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);

    mmo_socket_t socket =
        accept(server->listener, (struct sockaddr *)&addr, &len);

    if (socket == -1) {
        return;
    }

    /* Set socket to non-blocking mode. */
    if (mmo_socket_set_blocking(socket, false) == -1) {
        mmo_log_fmt(MMO_LOG_WARNING, "mmo_socket_set_blocking(): failed to set "
                                     "client socket to non-blocking. "
                                     "Disconnecting client.");
        close(socket);
        return;
    }

    /* If too many clients, reject socket with message. */
    if (server->clients.num_elems == (size_t)server->num_max_clients) {
        const char response[] = "Connection refused. Server is full.";
        send(socket, response, strlen(response), 0);

        close(socket);

        return;
    }

    /* Create client and add to array. */

    mmo_client_t client;
    client.socket          = socket;
    client.terminal_width  = 50;
    client.terminal_height = 25;
    client.state           = MMO_CLIENT_STATE_TELNET_NEGOTIATION;
    client.telnet.state    = MMO_TELNET_STATE_DATA;

    mmo_char_arr_new(&client.in);
    mmo_char_arr_new(&client.out);
    mmo_telopt_arr_new(&client.telnet.opts);
    mmo_char_arr_new(&client.telnet.sb_buf);

    if (!inet_ntop(addr.ss_family, &(((struct sockaddr_in *)&addr)->sin_addr),
                   client.ip, INET_ADDRSTRLEN)) {
        mmo_log_fmt(MMO_LOG_ERROR, "inet_ntop(): fail.");
        return;
    }

    mmo_client_arr_append(&server->clients, &client);

    mmo_log_fmt(MMO_LOG_INFO, "Client connected (%s).", client.ip);

    mmo_client_t *new_client =
        &server->clients.elems[server->clients.num_elems - 1];
    mmo_telnet_negotiate_options(new_client, server);
}

static bool mmo_find_unsuccessful_telopt(mmo_telopt_t *opt, void *ctx) {
    (void)ctx;
    return !opt->done;
}

void mmo_server_poll(mmo_server_t *server, int timeout_millisecs) {
    assert(server);

    /* Reset. */
    mmo_client_handle_arr_clear(&server->events.new_clients);
    mmo_client_handle_arr_clear(&server->events.removed_clients);

    /* Array of file descriptors (sockets) to be polled.
       Set first element to listener socket and the rest to client sockets. */

    mmo_pollfd_arr_t polled_sockets;
    mmo_pollfd_arr_new(&polled_sockets);

    mmo_pollfd_arr_append(
        &polled_sockets,
        &(struct pollfd){.fd = server->listener, .events = POLLIN});

    MMO_FOREACH(server->clients, client) {
        if (client->state == MMO_CLIENT_STATE_TO_BE_REMOVED) {
            mmo_disconnect_client(server, client);

            continue;
        }

        if (client->state == MMO_CLIENT_STATE_TELNET_NEGOTIATION) {
            /* Check if all telnet options have been successfully negotiated. */
            if (!mmo_telopt_arr_find(&client->telnet.opts,
                                     mmo_find_unsuccessful_telopt, NULL)) {
                client->state = MMO_CLIENT_STATE_ONLINE;

                /* Notify server owner of a new user. */
                mmo_client_handle_arr_append(&server->events.new_clients,
                                             &client->handle);
            }
        }

        /* Add client socket to array of sockets to be polled for events. */
        mmo_pollfd_arr_append(
            &polled_sockets,
            &(struct pollfd){.fd = client->socket, .events = POLLIN});
    }

    /* poll(): While there are no connected clients, wait indefinitely (-1). */

    int timeout = server->clients.num_elems == 0 ? -1 : timeout_millisecs;

    switch (
        poll(polled_sockets.elems, (nfds_t)polled_sockets.num_elems, timeout)) {
        case -1:
            mmo_log_fmt(MMO_LOG_ERROR, "poll(): error.");
            exit(EXIT_FAILURE);

        /* Timeout. No events. */
        case 0:
            goto no_events;
    }

    /* Check listener socket for incoming connection. */
    if (polled_sockets.elems[0].revents & POLLIN) {
        mmo_handle_incoming_connection(server);
    }

    /* Check if client sockets have received data. */
    for (size_t i = 1; i < polled_sockets.num_elems; i += 1) {
        struct pollfd *polled_client_socket = &polled_sockets.elems[i];

        if (polled_client_socket->revents & POLLIN) {
            char bytes[512];
            int num_bytes =
                (int)recv(polled_client_socket->fd, bytes, sizeof(bytes), 0);

            mmo_client_t *client = &server->clients.elems[i - 1];

            /* No data to read. */
            if (num_bytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                mmo_log_fmt(MMO_LOG_WARNING,
                            "recv(): returned empty for client (%s).",
                            client->ip);
                continue;
            }

            /* Client disconnected gracefully (0) or error (-1). */
            if (num_bytes <= 0) {
                mmo_disconnect_client(server, client);
                continue;
            }

            mmo_telnet_parse((mmo_char_span_t){.elems     = bytes,
                                               .num_elems = (size_t)num_bytes},
                             client, server);
        }
    }

no_events:
    mmo_pollfd_arr_free(&polled_sockets);

    /* Send outgoing data to clients. */
    MMO_FOREACH(server->clients, client) {
        if (client->out.num_elems > 0) {
            ssize_t num_bytes_sent = send(client->socket, client->out.elems,
                                          client->out.num_elems, 0);

            if (num_bytes_sent != -1) {
                mmo_char_arr_remove_n(&client->out, 0, (size_t)num_bytes_sent);
            }
        }
    }
}

static bool mmo_find_client(mmo_client_t *client, void *ctx) {
    return client->handle == *(mmo_client_handle_t *)ctx;
}

void mmo_server_remove_client(mmo_server_t *server,
                              mmo_client_handle_t handle) {
    assert(server);

    mmo_client_t *client =
        mmo_client_arr_find(&server->clients, mmo_find_client, &handle);
    assert(client && "Client must exist. Something is wrong.");

    client->state = MMO_CLIENT_STATE_TO_BE_REMOVED;
}

void mmo_server_send(mmo_server_t *server, mmo_client_handle_t handle,
                     const mmo_char_arr_t *msg) {
    assert(server);
    assert(msg);

    mmo_client_t *client =
        mmo_client_arr_find(&server->clients, mmo_find_client, &handle);
    assert(client && "Client must exist. Something is wrong.");

    /* Append message to out stream. */
    mmo_char_arr_append_arr(&client->out, (mmo_char_arr_t *)msg);
}
