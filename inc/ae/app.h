#ifndef AE_APP_H
#define AE_APP_H

#include <stdbool.h>

#include <ae/meta.h>
#include <ae/net.h>
#include <ae/string.h>

typedef enum ae_state_e {
    AE_STATE_GREET,
    AE_STATE_LOGIN_PASSWORD,
    AE_STATE_SIGNUP_USERNAME,
    AE_STATE_SIGNUP_PASSWORD,
    AE_STATE_MAIN
} ae_state_t;

typedef struct ae_user_s {
    ae_client_handle_t handle;
    ae_state_t state;

    struct {
        ae_string_t name;

        union {
            struct {
                int dummy;
            } auth;
        } state;
    } data;
} ae_user_t;

typedef ae_map(ae_client_handle_t, ae_user_t *) ae_handle_to_user_ptr_map_t;
typedef ae_map(ae_string_t, ae_user_t *) ae_string_to_user_ptr_map_t;

typedef struct ae_app_s {
    ae_server_t server;

    struct {
        ae_handle_to_user_ptr_map_t from_handle;
        ae_string_to_user_ptr_map_t from_username;
    } users;
} ae_app_t;

void ae_user_new(ae_user_t *user, ae_app_t *app, ae_client_handle_t handle);
void ae_user_free(ae_user_t *user);
void ae_user_update(ae_user_t *user, ae_app_t *app, ae_bytes_t *in);

void ae_app_new(ae_app_t *app);
void ae_app_free(ae_app_t *app);
void ae_app_update(ae_app_t *app);
void ae_app_serialize(const ae_app_t *app);
void ae_app_deserialize(ae_app_t *app);

void ae_state_new(ae_user_t *user, ae_app_t *app);
void ae_state_free(ae_user_t *user, ae_app_t *app);
void ae_state_update(ae_user_t *user, ae_app_t *app, ae_bytes_t *in);
void ae_state_switch(ae_user_t *user, ae_app_t *app, ae_state_t state);

#endif
