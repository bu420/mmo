#include <ae/app.h>

#include <assert.h>
#include <stdarg.h>

#include <ae/net.h>
#include <ae/io.h>

void ae_user_new(ae_user_t *user, ae_app_t *app, ae_client_handle_t handle) {
    user->handle = handle;

    ae_arr_new(user->data.name);

    user->state = AE_STATE_GREETING;
    ae_state_new(user, app);
}

void ae_user_free(ae_user_t *user) {
    ae_arr_free(user->data.name);
}

void ae_user_update(ae_user_t *user, ae_app_t *app, ae_byte_arr_t *in) {
    ae_state_update(user, app, in);
}

void ae_app_new(ae_app_t *app) {
    ae_map_new_reserve(app->users, AE_CLIENTS_MAX, ae_hash_client_handle,
                       ae_eq_client_handle);
}

void ae_app_free(ae_app_t *app) {
    ae_map_free(app->users);
}

void ae_app_update(ae_app_t *app) {
    for (size_t i = 0; i < app->server.clients.cap; i++) {
        if (app->server.clients.states[i] != AE_MAP_ENTRY_STATE_USED) {
            continue;
        }

        ae_client_t *client = &app->server.clients.vals[i];

        if (client->state == AE_CLIENT_STATE_NEW) {
            ae_user_t new_user;
            ae_user_new(&new_user, app, client->conn.handle);
            ae_map_set(app->users, new_user.handle, new_user);
        }

        else if (client->state == AE_CLIENT_STATE_TO_BE_REMOVED) {
            ae_user_t *user;
            ae_map_get(app->users, client->conn.handle, user);
            assert(user);

            ae_user_free(user);
            ae_map_remove(app->users, client->conn.handle);
        }
    }

    /* Update users. */
    for (size_t i = 0; i < app->users.cap; i++) {
        if (app->users.states[i] != AE_MAP_ENTRY_STATE_USED) {
            continue;
        }

        ae_user_t *user = &app->users.vals[i];

        ae_client_t *client;
        ae_map_get(app->server.clients, user->handle, client);
        assert(client);

        ae_user_update(user, app, &client->in);
    }
}
