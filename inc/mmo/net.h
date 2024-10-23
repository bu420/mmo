#ifndef MMO_NET_H
#define MMO_NET_H

#include <stdint.h>
#include <stddef.h>

#define MMO_MAX_CLIENTS 2

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

/* Create server. Listens for incoming connections. Non-blocking. */
int mmo_server_listen(mmo_server_t *server, uint16_t port);

/* Poll for events. Receives data and accepts/closes connections.
   Blocking for the duration of tick_remaining_millisecs. */
int mmo_server_poll(mmo_server_t *server, int tick_remaining_millisecs);

#endif