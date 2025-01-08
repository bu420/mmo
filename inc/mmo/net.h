#ifndef MMO_NET_H
#define MMO_NET_H

#include <arpa/inet.h>

#include <mmo/string.h>
#include <mmo/arr/char.h>
#include <mmo/arr/client_handle.h>
#include <mmo/arr/client.h>

#define MMO_ALLOWED_CLIENT_VERSION 1
#define MMO_MAX_CLIENTS            100
#define MMO_MAX_TERMINAL_WIDTH     1024
#define MMO_MAX_TERMINAL_HEIGHT    1024

typedef int mmo_socket_t;
typedef mmo_socket_t mmo_client_handle_t;

typedef enum mmo_client_state_e {
    MMO_CLIENT_STATE_AWAITING_HANDSHAKE,
    MMO_CLIENT_STATE_VERIFIED,
    MMO_CLIENT_STATE_ONLINE,
    MMO_CLIENT_STATE_TO_BE_REMOVED
} mmo_client_state_t;

typedef struct mmo_client_s {
    mmo_socket_t socket;

    mmo_client_state_t state;

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

    mmo_client_handle_arr_t new_clients;
    mmo_client_handle_arr_t removed_clients;
    mmo_client_input_arr_t client_inputs;
} mmo_server_t;

int mmo_server_new();
void mmo_server_free();
int mmo_server_listen(mmo_server_t *server, int port);

/* Poll for events. Receives data and accepts/closes connections.
   It's blocking for the duration of tick_remaining_millisecs. */
int mmo_server_poll(mmo_server_t *server, int tick_remaining_millisecs);

void mmo_server_remove_client(mmo_server_t *server, mmo_client_handle_t handle);

#endif
