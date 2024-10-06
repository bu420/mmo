#ifndef MMO_NET_H
#define MMO_NET_H

#ifndef MMO_MAX_CLIENTS
#define MMO_MAX_CLIENTS 100
#endif

#include <stdio.h>
#include <poll.h>

#include <mmo/core.h>

typedef struct mmo_server_s {
    int sock;
} mmo_server_t;

mmo_result_t mmo_server_listen(mmo_server_t *server, int port);
mmo_result_t mmo_server_accept(mmo_server_t *server, int *client_sock);

#endif