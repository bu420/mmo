#include <stdlib.h>
#include <assert.h>

#include <ae/app.h>
#include <ae/net.h>
#include <ae/log.h>

#define AE_DEFAULT_PORT 2000

int main(int argc, char *argv[]) {
    int port = argc < 2 ? AE_DEFAULT_PORT : atoi(argv[1]);

    ae_app_t app;
    ae_app_new(&app);
    ae_server_new(&app.server);
    ae_server_listen(&app.server, port);

    ae_log_fmt(AE_LOG_INFO, "Server listening on port %d.", port);

    while (true) {
        ae_server_poll(&app.server);
        ae_app_update(&app);
    }
}
