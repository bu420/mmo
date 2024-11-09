#ifndef MMO_NET_H
#define MMO_NET_H

#include <stdint.h>

#include <mmo/arr.h>

#define MMO_MAX_CLIENTS 2

typedef int mmo_socket_t;

typedef struct mmo_client_s {
    mmo_socket_t socket;

    /* Received data. */
    mmo_char_arr_t in;

    /* Outgoing data. */
    mmo_char_arr_t out;
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