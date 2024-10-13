#ifndef MMO_NET_H
#define MMO_NET_H

#include <stdio.h>
#include <sys/poll.h>

#include <mmo/core.h>

/* Ticks per second. */
#ifndef MMO_SERVER_TPS
#define MMO_SERVER_TPS 10
#endif

#ifndef MMO_MAX_CLIENTS
#define MMO_MAX_CLIENTS 100
#endif

typedef int mmo_socket_t;

typedef struct mmo_server_s {
    mmo_socket_t listener;

    struct pollfd sockets[MMO_MAX_CLIENTS];
    int socket_count;
} mmo_server_t;

/* Create server with a non-blocking socket listening for incoming
   client connections. */
mmo_result_t mmo_server_listen(mmo_server_t *server, int port);

/* Poll for events. Receives data and accepts/closes connections. */
mmo_result_t mmo_server_poll(mmo_server_t *server, int tps_remaining_time);

#endif