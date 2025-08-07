
#include <ae/app.h>

#include <stdlib.h>
#include <string.h>

#include <ae/io.h>

void ae_state_new(ae_user_t *user, ae_app_t *app) {
    switch (user->state) {
        case AE_STATE_GREET: {
            ae_bytes_t msg;
            ae_arr_from_string_literal(
                msg,
                "\x1b[31;40mBlood of Draemothuul\x1b[0m\r\n"
                "Enter your name or NEW.\r\n",
                59, 0);

            ae_server_send(&app->server, user->handle, msg);
            ae_prompt(user, app);
            break;
        }

        case AE_STATE_LOGIN_PASSWORD:
            break;

        case AE_STATE_SIGNUP_USERNAME: {
            ae_bytes_t msg;
            ae_arr_from_string_literal(
                msg,
                "You are creating a new account.\r\n"
                "Enter a username between 2-16 characters long.\r\n"
                "It may include letters, numbers and underscores.\r\n",
                131, 0);

            ae_server_send(&app->server, user->handle, msg);
            ae_prompt(user, app);
            break;
        }

        case AE_STATE_SIGNUP_PASSWORD:
            break;

        case AE_STATE_MAIN:
            break;
    }
}

void ae_state_free(ae_user_t *user, ae_app_t *app) {
    switch (user->state) {
        case AE_STATE_GREET:
            break;

        case AE_STATE_SIGNUP_USERNAME:
            break;

        case AE_STATE_SIGNUP_PASSWORD:
            break;

        case AE_STATE_LOGIN_PASSWORD:
            break;

        case AE_STATE_MAIN:
            break;
    }
}

void ae_state_update(ae_user_t *user, ae_app_t *app, ae_bytes_t *in) {
    switch (user->state) {
        case AE_STATE_GREET: {
            ae_string_t line;
            ae_arr_new(line);

            if (ae_get_line(&line, (ae_string_t *)in)) {
                if (ae_alen(line) == 0) {
                    ae_prompt(user, app);
                } else {
                    ae_string_t signup_new;
                    ae_arr_from_string_literal(signup_new, "new", 3, 0);

                    if (ae_string_eq_ignore_case(line, signup_new)) {
                        ae_state_switch(user, app, AE_STATE_SIGNUP_USERNAME);
                    } else {
                        // ae_state_switch(user, app, AE_STATE_LOGIN);
                    }
                }
            }

            ae_arr_free(line);
            break;
        }

        case AE_STATE_SIGNUP_USERNAME: {
            ae_string_t line;
            ae_arr_new(line);

            if (ae_get_line(&line, (ae_string_t *)in)) {
                ae_prompt(user, app);
            }

            ae_arr_free(line);
            break;
        }

        case AE_STATE_SIGNUP_PASSWORD: {
            break;
        }

        case AE_STATE_LOGIN_PASSWORD: {
            ae_string_t line;
            ae_arr_new(line);

            if (ae_get_line(&line, (ae_string_t *)in)) {
                ae_prompt(user, app);
            }

            ae_arr_free(line);
            break;
        }

        case AE_STATE_MAIN:
            break;
    }
}

void ae_state_switch(ae_user_t *user, ae_app_t *app, ae_state_t state) {
    ae_state_free(user, app);
    user->state = state;
    ae_state_new(user, app);
}
