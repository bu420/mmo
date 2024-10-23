#ifndef MMO_NET_H
#define MMO_NET_H

#include <stdint.h>
#include <stddef.h>

#ifndef MMO_SERVER_TPS
#define MMO_SERVER_TPS 10
#endif

#ifndef MMO_MAX_CLIENTS
#define MMO_MAX_CLIENTS 100
#endif

typedef int mmo_socket_t;

typedef struct mmo_client_s {
    mmo_socket_t socket;

    /* Received data ready to be parsed. */
    char *in;
    size_t in_size;

    /* Outgoing data to be sent. */
    char *out;
    size_t out_size;
} mmo_client_t;

typedef struct mmo_server_s {
    mmo_socket_t listener;

    mmo_client_t clients[MMO_MAX_CLIENTS];
    int num_clients;
} mmo_server_t;

/* Create server with a non-blocking socket listening for incoming
   client connections. */
int mmo_server_listen(mmo_server_t *server, uint16_t port);

/* Poll for events. Receives data and accepts/closes connections. */
int mmo_server_poll(mmo_server_t *server, int tick_remaining_millisecs);

#endif