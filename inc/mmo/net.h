#ifndef MMO_NET_H
#define MMO_NET_H

#include <stdint.h>
#include <arpa/inet.h>

#include <mmo/arr.h>

#define MMO_ALLOWED_CLIENT_VERSION 1
#define MMO_MAX_CLIENTS 100

MMO_ARR_DECL(mmo_client_t, mmo_client);

typedef int mmo_socket_t;

typedef struct mmo_client_s {
    mmo_socket_t socket;

    char ip[INET_ADDRSTRLEN];

    /* Received data. */
    mmo_char_arr_t in;

    /* Outgoing data. */
    mmo_char_arr_t out;

    int terminal_width;
    int terminal_height;
} mmo_client_t;

typedef struct mmo_server_s {
    mmo_socket_t listener;
    mmo_client_arr_t clients;
} mmo_server_t;

/* Create server. Listens for incoming connections. Non-blocking. */
int mmo_server_listen(mmo_server_t *server, uint16_t port);

/* Poll for events. Receives data and accepts/closes connections.
   It's blocking for the duration of tick_remaining_millisecs. */
int mmo_server_poll(mmo_server_t *server, int tick_remaining_millisecs);

#endif