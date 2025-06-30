#ifndef MMO_NET_H
#define MMO_NET_H

#include <arpa/inet.h>
#include <stdint.h>

#include <mmo/arr/char.h>
#include <mmo/arr/client.h>
#include <mmo/arr/client_handle.h>
#include <mmo/arr/client_input.h>
#include <mmo/arr/telopt.h>

#define MMO_ALLOWED_CLIENT_VERSION 1

typedef int mmo_socket_t;
typedef mmo_socket_t mmo_client_handle_t;

typedef enum mmo_client_state_e {
    MMO_CLIENT_STATE_TELNET_NEGOTIATION,
    MMO_CLIENT_STATE_ONLINE,
    MMO_CLIENT_STATE_TO_BE_REMOVED
} mmo_client_state_t;

typedef enum mmo_telnet_state_e {
    MMO_TELNET_STATE_DATA,
    MMO_TELNET_STATE_IAC,
    MMO_TELNET_STATE_COMMAND,
    MMO_TELNET_STATE_SB_IAC,
    MMO_TELNET_STATE_SB,
    MMO_TELNET_STATE_SB_DATA
} mmo_telnet_state_t;

typedef struct mmo_client_s {
    union {
        mmo_client_handle_t handle;
        mmo_socket_t socket;
    };

    char ip[INET_ADDRSTRLEN];

    /* Received data. */
    mmo_char_arr_t in;

    /* Outgoing data. */
    mmo_char_arr_t out;

    int terminal_width;
    int terminal_height;

    mmo_client_state_t state;

    /* Telnet state machine. */
    struct {
        mmo_telopt_arr_t opts;
        mmo_telnet_state_t state;
        uint8_t cmd;

        uint8_t sb_opt;
        mmo_char_arr_t sb_buf;
    } telnet;
} mmo_client_t;

typedef struct mmo_server_s {
    int num_max_clients;

    mmo_socket_t listener;
    mmo_client_arr_t clients;

    /* New events after calls to mmo_server_poll() are stored here.
       They are cleared automatically at the start of each call to
       mmo_server_poll(). */
    struct {
        mmo_client_handle_arr_t new_clients;
        mmo_client_handle_arr_t removed_clients;
        mmo_client_input_arr_t client_inputs;
    } events;
} mmo_server_t;

void mmo_server_new(mmo_server_t *server, int num_max_clients);
void mmo_server_free(mmo_server_t *server);
void mmo_server_listen(mmo_server_t *server, int port);
void mmo_server_poll(mmo_server_t *server, int timeout_millisecs);
void mmo_server_remove_client(mmo_server_t *server, mmo_client_handle_t handle);
void mmo_server_send(mmo_server_t *server, mmo_client_handle_t handle,
                     const mmo_char_arr_t *msg);

#endif
