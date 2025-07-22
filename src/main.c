#include <stdlib.h>
#include <assert.h>

#include <mmo/map.h>
#include <mmo/app.h>
#include <mmo/net.h>
#include <mmo/log.h>

#define MMO_DEFAULT_PORT 2000

static uint64_t hash(char *key) {
    uint64_t hash = 14695981039346656037UL;

    for (char *b = key; *b; b++) {
        hash ^= (uint64_t)(uint8_t)*b;
        hash *= 1099511628211UL;
    }

    return hash;
}

static bool eq(char *a, char *b) { return strcmp(a, b) == 0; }

int main(int argc, char *argv[]) {
    int port = argc < 2 ? MMO_DEFAULT_PORT : atoi(argv[1]);

    mmo_app_t app;
    mmo_app_new(&app);
    mmo_server_new(&app.server);
    mmo_server_listen(&app.server, port);

    mmo_log_fmt(MMO_LOG_INFO, "Server listening on port %d.", port);

    while (true) {
        mmo_server_poll(&app.server);

        mmo_app_update(&app);
    }
}
