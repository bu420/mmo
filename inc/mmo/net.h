#ifndef MMO_NET_H
#define MMO_NET_H

#include <arpa/inet.h>
#include <stdint.h>

#include <mmo/arr.h>

#define MMO_CLIENTS_MAX 50

typedef int mmo_socket_t;
typedef mmo_socket_t mmo_client_handle_t;

typedef enum mmo_client_state_e {
    MMO_CLIENT_STATE_NEW,
    MMO_CLIENT_STATE_ONLINE,
    MMO_CLIENT_STATE_TO_BE_REMOVED
} mmo_client_state_t;

typedef struct mmo_client_s {
    union {
        mmo_client_handle_t handle;
        mmo_socket_t socket;
    } conn;

    mmo_client_state_t state;

    /* Received data. */
    mmo_byte_arr_t in;

    /* Outgoing data. */
    mmo_byte_arr_t out;

    char ip[INET_ADDRSTRLEN];
} mmo_client_t;

typedef mmo_arr(mmo_client_t) mmo_client_arr_t;

typedef struct mmo_server_s {
    mmo_socket_t listener;
    mmo_client_arr_t clients;
} mmo_server_t;

void mmo_server_new(mmo_server_t *server);
void mmo_server_free(mmo_server_t *server);
void mmo_server_listen(mmo_server_t *server, int port);
void mmo_server_poll(mmo_server_t *server);
void mmo_server_remove_client(mmo_server_t *server, mmo_client_handle_t handle);
void mmo_server_send(mmo_server_t *server, mmo_client_handle_t handle,
                     mmo_byte_arr_t data);

#endif
