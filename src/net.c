#include "ae/meta.h"
#include <ae/net.h>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <errno.h>

#include <ae/log.h>

static int ae_socket_set_blocking(ae_socket_t socket, bool blocking) {
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

uint64_t ae_handle_hash(const ae_client_handle_t *handle) {
    return (uint64_t)*handle;
}

bool ae_handle_eq(const ae_client_handle_t *a, const ae_client_handle_t *b) {
    return a == b;
}

void ae_server_new(ae_server_t *server) {
    ae_map_new_reserve(server->clients, AE_CLIENTS_MAX);
}

void ae_server_free(ae_server_t *server) {
    ae_map_free(server->clients);
}

void ae_server_listen(ae_server_t *server, int port) {
    /* Create socket. */

    server->listener = socket(AF_INET, SOCK_STREAM, 0);

    if (server->listener == -1) {
        ae_log_fmt(AE_LOG_ERROR,
                   "socket(): failed to create listener socket.");
        exit(EXIT_FAILURE);
    }

    /* Set socket to non-blocking mode. */

    if (ae_socket_set_blocking(server->listener, false) == -1) {
        ae_log_fmt(AE_LOG_ERROR, "ae_socket_set_blocking(): failed to set "
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
        ae_log_fmt(AE_LOG_ERROR, "bind(): failed to bind listener socket.");
        exit(EXIT_FAILURE);
    }

    /* Listen on socket. */

    if (listen(server->listener, AE_CLIENTS_MAX) == -1) {
        ae_log_fmt(AE_LOG_ERROR, "listen(): failed to listen.");
        exit(EXIT_FAILURE);
    }
}

static void ae_disconnect_client(ae_server_t *server, ae_client_t *client) {
    ae_log_fmt(AE_LOG_INFO, "Client disconnected (%s).", client->ip);

    close(client->conn.socket);

    ae_arr_free(client->in);
    ae_arr_free(client->out);

    ae_map_remove(server->clients, ae_handle_hash, ae_handle_eq,
                  client->conn.handle);
}

static void ae_handle_conn(ae_server_t *server) {
    /* Accept connection. */

    struct sockaddr_storage addr;
    socklen_t len = sizeof addr;

    ae_socket_t socket =
        accept(server->listener, (struct sockaddr *)&addr, &len);

    if (socket == -1) {
        return;
    }

    /* Set socket to non-blocking mode. */
    if (ae_socket_set_blocking(socket, false) == -1) {
        ae_log_fmt(AE_LOG_WARNING, "ae_socket_set_blocking(): failed to set "
                                   "client socket to non-blocking. "
                                   "Disconnecting client.");
        close(socket);
        return;
    }

    /* If too many clients, reject socket with message. */
    if (server->clients.len == AE_CLIENTS_MAX) {
        const char response[] = "\r\nConnection refused. Server is full.\r\n";
        send(socket, response, strlen(response), 0);
        close(socket);
        return;
    }

    /* Create client and add to array. */

    ae_client_t client;
    client.conn.socket = socket;
    client.state       = AE_CLIENT_STATE_NEW;

    if (!inet_ntop(addr.ss_family, &(((struct sockaddr_in *)&addr)->sin_addr),
                   client.ip, INET_ADDRSTRLEN)) {
        close(socket);
        ae_log_fmt(AE_LOG_ERROR, "inet_ntop(): fail.");
        return;
    }

    ae_arr_new(client.in);
    ae_arr_new(client.out);

    ae_map_set(server->clients, ae_handle_hash, ae_handle_eq,
               client.conn.handle, client);

    ae_log_fmt(AE_LOG_INFO, "Client connected (%s).", client.ip);
}

void ae_server_poll(ae_server_t *server) {
    /* Array of file descriptors (sockets) to be polled.
       Set first element to listener socket and the rest to client sockets. */

    ae_arr(struct pollfd) polled_sockets;
    ae_arr_new_reserve(polled_sockets, 1 + server->clients.len);

    struct pollfd fd = {.fd = server->listener, .events = POLLIN};
    ae_arr_append(polled_sockets, fd);

    ae_client_handle_t *handle;
    ae_client_t *client;

    ae_map_foreach(server->clients, handle, client) {
        if (client->state == AE_CLIENT_STATE_NEW) {
            client->state = AE_CLIENT_STATE_ONLINE;
        }

        else if (client->state == AE_CLIENT_STATE_TO_BE_REMOVED) {
            ae_disconnect_client(server, client);
            continue;
        }

        /* Add client socket to array of sockets to be polled for events. */
        fd.fd = client->conn.socket;
        ae_arr_append(polled_sockets, fd);
    }

    /* poll(): while there are no connected clients, wait indefinitely (-1). */

    int timeout = server->clients.pop_bkts == 0 ? -1 : 0;

    switch (poll(polled_sockets, (nfds_t)ae_alen(polled_sockets), timeout)) {
        case -1:
            ae_log_fmt(AE_LOG_ERROR, "poll(): error.");
            exit(EXIT_FAILURE);

        /* Timeout. No events. */
        case 0:
            goto no_events;
    }

    /* Check listener socket for incoming connection. */
    if (polled_sockets[0].revents & POLLIN) {
        ae_handle_conn(server);
    }

    /* Check if client sockets have received data. */
    for (size_t i = 1; i < ae_alen(polled_sockets); i += 1) {
        struct pollfd polled_client_socket = polled_sockets[i];

        if (polled_client_socket.revents & POLLIN) {
            char buf[512];
            int len = (int)recv(polled_client_socket.fd, buf, sizeof buf, 0);

            ae_map_get(server->clients, ae_handle_hash, ae_handle_eq,
                       polled_client_socket.fd, client);
            assert(client);

            /* No data to read. */
            if (len == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                ae_log_fmt(AE_LOG_WARNING,
                           "recv(): returned empty for client (%s).",
                           client->ip);
                continue;
            }

            /* Client disconnected gracefully (0) or error (-1). */
            if (len <= 0) {
                client->state = AE_CLIENT_STATE_TO_BE_REMOVED;
                continue;
            }

            ae_arr_append_n(client->in, (size_t)len, buf);
        }
    }

no_events:
    ae_arr_free(polled_sockets);

    /* Send outgoing data to clients. */
    ae_map_foreach(server->clients, handle, client) {
        if (ae_alen(client->out) > 0) {
            ssize_t num_bytes_sent = send(client->conn.socket, client->out,
                                          ae_alen(client->out), 0);

            if (num_bytes_sent != -1) {
                ae_arr_remove_n(client->out, 0, (size_t)num_bytes_sent);
            }
        }
    }
}

void ae_server_remove_client(ae_server_t *server, ae_client_handle_t handle) {
    ae_client_t *client;
    ae_map_get(server->clients, ae_handle_hash, ae_handle_eq, handle, client);

    char *goodbye = "\r\nGoodbye.\r\n";
    send(client->conn.socket, goodbye, strlen(goodbye), 0);

    client->state = AE_CLIENT_STATE_TO_BE_REMOVED;
}

void ae_server_send(ae_server_t *server, ae_client_handle_t handle,
                    const ae_bytes_t data) {
    ae_client_t *client;
    ae_map_get(server->clients, ae_handle_hash, ae_handle_eq, handle, client);

    /* Append message to out stream. */
    ae_arr_append_n(client->out, ae_alen(data), data);
}
