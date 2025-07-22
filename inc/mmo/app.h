#ifndef MMO_app_H
#define MMO_app_H

#include <stdbool.h>

#include <mmo/net.h>
#include <mmo/arr.h>

typedef enum mmo_state_e {
    MMO_STATE_GREETING,
    MMO_STATE_SIGNUP,
    MMO_STATE_LOGIN,
    MMO_STATE_GAME
} mmo_state_t;

typedef struct mmo_user_s {
    mmo_client_handle_t handle;
    mmo_state_t state;

    struct {
        mmo_string_t name;
    } data;

    union {
        struct {
            bool sent_greeting;
        } greeting;
    } state_data;
} mmo_user_t;

typedef mmo_arr(mmo_user_t) mmo_user_arr_t;

typedef struct mmo_app_s {
    mmo_server_t server;
    mmo_user_arr_t users;
} mmo_app_t;

void mmo_player_new(mmo_user_t *user, mmo_client_handle_t handle,
                    mmo_app_t *app);
void mmo_player_free(mmo_user_t *user);
void mmo_player_update(mmo_user_t *user, mmo_app_t *app, mmo_byte_arr_t in);

void mmo_app_new(mmo_app_t *app);
void mmo_app_free(mmo_app_t *app);
void mmo_app_update(mmo_app_t *app);

void mmo_state_new(mmo_user_t *user, mmo_app_t *app, mmo_state_t state);
void mmo_state_free(mmo_user_t *user, mmo_app_t *app, mmo_state_t state);
void mmo_state_update(mmo_user_t *user, mmo_app_t *app, mmo_byte_arr_t in,
                      mmo_state_t state);
void mmo_state_switch(mmo_user_t *user, mmo_app_t *app, mmo_state_t state);

#define MMO_STATE(name)                                                        \
    void mmo_state_##name##_new(mmo_user_t *user, mmo_app_t *app);             \
    void mmo_state_##name##_free(mmo_user_t *user, mmo_app_t *app);            \
    void mmo_state_##name##_update(mmo_user_t *user, mmo_app_t *app,           \
                                   mmo_byte_arr_t in);

MMO_STATE(greeting)
MMO_STATE(signup)
MMO_STATE(login)
MMO_STATE(main_app)

#undef MMO_STATE

#endif
