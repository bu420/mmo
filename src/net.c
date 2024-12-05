#include <mmo/net.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <sys/socket.h>

MMO_ARR_DEF(mmo_client_t, mmo_client);

static int mmo_socket_set_blocking(mmo_socket_t socket, bool blocking) {
    int flags = fcntl(socket, F_GETFL, 0);
    
    if (flags == -1) {
        return -1;
    }

    if (blocking) {
        flags &= ~O_NONBLOCK;
    }
    else {
        flags |= O_NONBLOCK;
    }

    if (fcntl(socket, F_SETFL, flags) == -1) {
        return -1;
    }

    return 0;
}

static int32_t mmo_read_int32(const char bytes[4]) {
    assert(bytes);
    return (int32_t)ntohl(*(uint32_t *)bytes);
}

static int mmo_handle_packet_handshake(mmo_char_arr_t *bytes, mmo_client_arr_t *clients, size_t client_idx) {
    assert(bytes);
    assert(client);
    
    if (bytes->num_elems < 3 * sizeof(int32_t)) {
        return -1;
    }
    
    /* Read client version. */

    int32_t version = mmo_read_int32(bytes->elems);

    if (version != MMO_ALLOWED_CLIENT_VERSION) {
        const char response[256];
        snprintf(response, 256, "Connection refused. Client version must be %d. Your client is on version %d.\r\n", MMO_ALLOWED_CLIENT_VERSION, version);

        send(socket, response, strlen(response), 0);

        close(socket);

        

        return -1;
    }

    /* Read client terminal width. */
    mmo_read_int32(arr->elems + sizeof(int32_t));

    /* Read client terminal height. */
    mmo_read_int32(arr->elems + 2 * sizeof(int32_t));
}

static void mmo_handle_packet_text(const mmo_char_arr_t *arr) {
    
}

static void mmo_handle_packet_terminal_size(const mmo_char_arr_t *arr) {

}

int mmo_server_listen(mmo_server_t *server, uint16_t port) {
    server->num_clients = 0;
    
    /* Create socket. */

    server->listener = socket(AF_INET, SOCK_STREAM, 0);

    if (server->listener == -1) {
        return -1;
    }

    /* Allow socket to be reusable. Avoids address-in-use error. */

    char yes = 1;
    setsockopt(server->listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    /* Set socket to non-blocking mode. */

    if (mmo_socket_set_blocking(server->listener, false) == -1) {
        return -1;
    }

    /* Bind socket. */

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server->listener, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        close(server->listener);
        return -1;
    }

    /* Listen on socket. */

    if (listen(server->listener, MMO_MAX_CLIENTS) == -1) {
        close(server->listener);
        return -1;
    }

    return 0;
}

int mmo_server_poll(mmo_server_t *server, int tick_remaining_millisecs) {
    /* Setup poll structure. Set first element to listener socket,
       fill rest with client sockets. */

    struct pollfd sockets[MMO_MAX_CLIENTS + 1];
    int num_sockets = server->num_clients + 1;

    sockets[0].fd = server->listener;
    sockets[0].events = POLLIN;

    for (int i = 0; i < server->num_clients; i += 1) {
        sockets[1 + i].fd = server->clients[i].socket;
        sockets[1 + i].events = POLLIN;
    }
    
    /* Poll for socket events. poll() blocks and becomes server's 
       sleep function. While no clients: wait indefinitely (-1), 
       otherwise wait for remaining tick time. */

    int timeout = server->num_clients == 0 ? -1 : tick_remaining_millisecs;

    switch (poll(sockets, (nfds_t)num_sockets, timeout)) {
    case -1:
        return -1;

    /* Timeout. No events. */
    case 0:
        return 0;
    }

    /* Check if listener socket has been notified of new connection.
       Check if client socket has received data. */
    
    for (int i = 0; i < num_sockets; i += 1) {
        if (sockets[i].revents & POLLIN) {
            /* New connection. */
            if (sockets[i].fd == server->listener) {
                /* Accept connection. */

                struct sockaddr_storage addr;
                socklen_t len = sizeof(addr);

                mmo_socket_t socket = accept(server->listener, (struct sockaddr *)&addr, &len);

                if (socket == -1) {
                    continue;
                }

                /* Set socket to non-blocking mode. */

                if (mmo_socket_set_blocking(socket, false) == -1) {
                    return -1;
                }

                /* If too many clients, reject socket with message. */

                if (server->num_clients == MMO_MAX_CLIENTS) {
                    const char response[] = "Connection refused. Server is full.";

                    send(socket, response, strlen(response), 0);

                    close(socket);

                    continue;
                }

                /* Create client and add to array. */

                mmo_client_t client;
                client.socket = socket;

                if (mmo_char_arr_new(&client.in, 0) == -1) {
                    return -1;
                }

                if (mmo_char_arr_new(&client.out, 0) == -1) {
                    return -1;
                }

                if (!inet_ntop(addr.ss_family, &(((struct sockaddr_in *)&addr)->sin_addr), client.ip, INET_ADDRSTRLEN)) {
                    return -1;
                }

                server->clients[server->num_clients] = client;
                server->num_clients += 1;
            }
            /* Existing connection. */
            else {
                char bytes[256];
                int num_bytes = (int)recv(sockets[i].fd, bytes, sizeof(bytes), 0);

                int client_idx = i - 1;

                /* Client disconnected gracefully. */
                if (num_bytes == 0) {
                    printf("Connection closed.\n");

                    close(sockets[i].fd);

                    /* Simple remove. Replace element with last tracked element in array. */

                    sockets[i] = sockets[num_sockets - 1];
                    num_sockets -= 1;

                    mmo_char_arr_free(&server->clients[client_idx].in);
                    mmo_char_arr_free(&server->clients[client_idx].out);

                    server->clients[client_idx] = server->clients[server->num_clients - 1];
                    server->num_clients -= 1;

                    continue;
                }

                /* Error. */
                if (num_bytes == -1) {
                    close(sockets[i].fd);
                    
                    return -1;
                }
                
                /* Append data to array. */
                for (int j = 0; j < num_bytes; j += 1) {
                    if (mmo_char_arr_append(&server->clients[client_idx].in, bytes[j]) == -1) {
                        return -1;
                    }
                }
            }
        }
    }

    /* Parse received data to see if a whole packet has been received. */

    for (int i = 0; i < server->num_clients; i += 1) {
        if (server->clients[i].in.num_elems >= 4) {
            int net_packet_id = *(int *)server->clients[i].in.elems;
            int packet_id = (int)ntohl((uint32_t)net_packet_id);

            /* Pop first int. */
            for (int j = 0; j < sizeof(int32_t); j += 1) {
                mmo_char_arr_remove(&server->clients[i].in, 0);
            }

            switch (packet_id)
            {
            /* Handshake. */
            case 0:
                mmo_handle_packet_handshake(&server->clients[i].in);
                break;

            /* Text. */
            case 1:
                mmo_handle_packet_text(&server->clients[i].in);
                break;

            /* New terminal size. */
            case 2:
                mmo_handle_packet_terminal_size(&server->clients[i].in);
                break;
            }

            printf("Received packet (ID = %d) from client (%d)\n", packet_id, i);
            mmo_char_arr_clear(&server->clients[i].in);
        }
    }

    return 0;
}