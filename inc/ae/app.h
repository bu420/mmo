#ifndef AE_APP_H
#define AE_APP_H

#include <stdbool.h>

#include <ae/net.h>
#include <ae/arr.h>
#include <ae/map.h>

typedef enum ae_state_e {
    AE_STATE_GREETING,
    AE_STATE_SIGNUP,
    AE_STATE_LOGIN,
    AE_STATE_GAME
} ae_state_t;

typedef struct ae_user_s {
    ae_client_handle_t handle;
    ae_state_t state;

    struct {
        ae_string_t name;

        union {
            struct {
                bool sent_greeting;
            } greeting;
        } state;
    } data;
} ae_user_t;

typedef ae_map(ae_client_handle_t, ae_user_t) ae_handle_to_user_map_t;

typedef struct ae_app_s {
    ae_server_t server;
    ae_handle_to_user_map_t users;
} ae_app_t;

void ae_user_new(ae_user_t *user, ae_app_t *app, ae_client_handle_t handle);
void ae_user_free(ae_user_t *user);
void ae_user_update(ae_user_t *user, ae_app_t *app, ae_byte_arr_t in);

void ae_app_new(ae_app_t *app);
void ae_app_free(ae_app_t *app);
void ae_app_update(ae_app_t *app);

void ae_state_new(ae_user_t *user, ae_app_t *app);
void ae_state_free(ae_user_t *user, ae_app_t *app);
void ae_state_update(ae_user_t *user, ae_app_t *app, ae_byte_arr_t in);
void ae_state_switch(ae_user_t *user, ae_app_t *app, ae_state_t state);

#endif
