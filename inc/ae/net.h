#ifndef AE_NET_H
#define AE_NET_H

#include <stdint.h>
#include <stdbool.h>
#include <arpa/inet.h>

#include <ae/meta.h>

#define AE_CLIENTS_MAX 50

typedef int ae_socket_t;
typedef int ae_client_handle_t;

typedef enum ae_client_state_e {
    AE_CLIENT_STATE_NEW,
    AE_CLIENT_STATE_ONLINE,
    AE_CLIENT_STATE_TO_BE_REMOVED
} ae_client_state_t;

typedef struct ae_client_s {
    union {
        ae_client_handle_t handle;
        ae_socket_t socket;
    } conn;

    ae_client_state_t state;

    /* Received data. */
    ae_bytes_t in;

    /* Outgoing data. */
    ae_bytes_t out;

    char ip[INET_ADDRSTRLEN];
} ae_client_t;

typedef ae_map(ae_client_handle_t, ae_client_t) ae_handle_to_client_map_t;
uint64_t ae_handle_hash(const ae_client_handle_t *handle);
bool ae_handle_eq(const ae_client_handle_t *a, const ae_client_handle_t *b);

typedef struct ae_server_s {
    ae_socket_t listener;
    ae_handle_to_client_map_t clients;
} ae_server_t;

void ae_server_new(ae_server_t *server);
void ae_server_free(ae_server_t *server);
void ae_server_listen(ae_server_t *server, int port);
void ae_server_poll(ae_server_t *server);
void ae_server_remove_client(ae_server_t *server, ae_client_handle_t handle);
void ae_server_send(ae_server_t *server, ae_client_handle_t handle,
                    const ae_bytes_t data);

#endif
