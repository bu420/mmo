#include <mmo/net.h>

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>

static mmo_result_t _set_nonblocking(int sock) {
    int flags = fcntl(sock, F_GETFL, 0);
    
    if (flags == -1) {
        return mmo_result_new(MMO_ERR, "Failed to set socket in non-blocking mode: fcntl(): %s", MMO_ERRNO);
    }

    if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1) {
        return mmo_result_new(MMO_ERR, "Failed to set socket in non-blocking mode: fcntl(): %s", MMO_ERRNO);
    }

    return mmo_result_new(MMO_OK, NULL);
}

mmo_result_t mmo_server_listen(mmo_server_t *server, int port) {
    server->sock = socket(AF_INET, SOCK_STREAM, 0);

    if (server->sock == -1) {
        return mmo_result_new(MMO_ERR, "Failed to create socket: %s", MMO_ERRNO);
    }

    mmo_result_t res = _set_nonblocking(server->sock);
    if (res.status != MMO_OK) {
        return res;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server->sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        close(server->sock);
        return mmo_result_new(MMO_ERR, "Failed to bind socket: %s", MMO_ERRNO);
    }

    if (listen(server->sock, MMO_MAX_CLIENTS) == -1) {
        close(server->sock);
        return mmo_result_new(MMO_ERR, "Failed to listen on socket: %s", MMO_ERRNO);
    }

    return mmo_result_new(MMO_OK, NULL);
}

mmo_result_t mmo_server_accept(mmo_server_t *server, int *client_sock) {
    struct pollfd fds[MMO_MAX_CLIENTS];

    server->fds[0].fd = server->sock;
    server->fds[0].events = POLLIN;

    if (poll(server->fds, 1, -1) == -1) {
        return mmo_result_new(MMO_ERR, "Failed to poll: %s", MMO_ERRNO);
    }

    if (server->fds[0].revents & POLLIN) {
        struct sockaddr_in client_addr;
        int addr_len = sizeof(client_addr);

        *client_sock = accept(server->sock, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len);

        if (*client_sock == -1) {
            return mmo_result_new(MMO_NO_INCOMING_CLIENT, NULL);
        }

        printf("New client connected: %d\n", *client_sock);

        send(*client_sock, "Hello World from Bu's Telnet server", 36, 0);
        close(*client_sock);
    }

    return mmo_result_new(MMO_OK, NULL);
}