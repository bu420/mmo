#ifndef MMO_NET_H
#define MMO_NET_H

#ifndef MMO_MAX_CLIENTS
#define MMO_MAX_CLIENTS 100
#endif

#include <stdio.h>
#include <sys/poll.h>

#include <mmo/core.h>

typedef int mmo_socket_t;

typedef struct mmo_server_s {
    mmo_socket_t listener;

    struct pollfd sockets[MMO_MAX_CLIENTS];
    int socket_count;
} mmo_server_t;

/* Create server with a non-blocking socket listening for incoming
   client connections. */
mmo_result_t mmo_server_listen(mmo_server_t *server, int port);

/* Updating server involves accepting and closing client connections.
   If no clients are connected and there's no activity this function
   will block indefinitely to save CPU resources. */
mmo_result_t mmo_server_update(mmo_server_t *server);

#endif