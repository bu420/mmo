#include <ae/app.h>

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include <ae/net.h>
#include <ae/io.h>

void ae_user_new(ae_user_t *user, ae_app_t *app, ae_client_handle_t handle) {
    user->handle = handle;

    ae_arr_new(user->data.name);

    user->state = AE_STATE_GREET;
    ae_state_new(user, app);
}

void ae_user_free(ae_user_t *user) {
    ae_arr_free(user->data.name);
}

void ae_user_update(ae_user_t *user, ae_app_t *app, ae_bytes_t *in) {
    ae_state_update(user, app, in);
}

void ae_app_new(ae_app_t *app) {
    ae_map_new_reserve(app->users.from_handle, AE_CLIENTS_MAX, ae_handle_hash,
                       ae_handle_eq);
    ae_map_new_reserve(app->users.from_username, AE_CLIENTS_MAX,
                       ae_string_hash, ae_string_eq);
}

void ae_app_free(ae_app_t *app) {
    ae_map_free(app->users.from_handle);
    ae_map_free(app->users.from_username);
}

void ae_app_update(ae_app_t *app) {
    ae_map_foreach(app->server.clients, i) {
        ae_client_t *client = &app->server.clients.vals[i];

        if (client->state == AE_CLIENT_STATE_NEW) {
            ae_user_t *user = malloc(sizeof(ae_user_t));
            assert(user);
            ae_user_new(user, app, client->conn.handle);

            ae_map_set(app->users.from_handle, user->handle, user);
        } else if (client->state == AE_CLIENT_STATE_TO_BE_REMOVED) {
            ae_user_t **user;
            ae_map_get(app->users.from_handle, client->conn.handle, user);

            ae_map_remove(app->users.from_handle, (*user)->handle);

            bool username_in_map;
            ae_map_contains(app->users.from_username, (*user)->data.name,
                            username_in_map);

            if (username_in_map) {
                ae_map_remove(app->users.from_username, (*user)->data.name);
            }

            ae_user_free(*user);
        }
    }

    /* Update users. */
    ae_map_foreach(app->users.from_handle, i) {
        ae_user_t *user = app->users.from_handle.vals[i];

        ae_client_t *client;
        ae_map_get(app->server.clients, user->handle, client);
        assert(client);

        ae_user_update(user, app, &client->in);
    }
}

void ae_app_serialize(const ae_app_t *app) {
}

void ae_app_deserialize(ae_app_t *app) {
}
