
#include <ae/app.h>

#include <ae/io.h>
#include <ae/arr.h>

void ae_state_new(ae_user_t *user, ae_app_t *app) {
    switch (user->state) {
        case AE_STATE_GREETING: {
            ae_byte_arr_t greeting;
            ae_arr_from_string_literal(
                greeting,
                "\x1b[31;40mBlood of Draemothuul\x1b[0m\r\n"
                "Enter your name or NEW.\r\n",
                59);

            ae_server_send(&app->server, user->handle, greeting);
            ae_prompt(user, app);
            break;
        }

        case AE_STATE_SIGNUP:
            break;

        case AE_STATE_LOGIN:
            break;

        case AE_STATE_MAIN:
            break;
    }
}

void ae_state_free(ae_user_t *user, ae_app_t *app) {
    switch (user->state) {
        case AE_STATE_GREETING:
            break;

        case AE_STATE_SIGNUP:
            break;

        case AE_STATE_LOGIN:
            break;

        case AE_STATE_MAIN:
            break;
    }
}

void ae_state_update(ae_user_t *user, ae_app_t *app, ae_byte_arr_t *in) {
    switch (user->state) {
        case AE_STATE_GREETING: {
            ae_byte_arr_t line;
            ae_arr_new(line);

            if (ae_get_line(&line, in)) {
                /*if (ae_arr_len(line) == 0) {
                    ae_prompt(user, app);
                }
                else {
                    ae_byte_arr_t signup_new;
                    ae_arr_from_string_literal(signup_new, "new", 3);

                    if (ae_str_eq_ignore_case(line, signup_new)) {
                        ae_state_switch(user, app, AE_STATE_SIGNUP);
                    } else {
                        ae_state_switch(user, app, AE_STATE_LOGIN);
                    }
                }*/
                ae_prompt(user, app);
            }

            ae_arr_free(line);
            break;
        }

        case AE_STATE_SIGNUP: {
            ae_byte_arr_t line;
            ae_arr_new(line);

            if (ae_get_line(&line, in)) {
                ae_prompt(user, app);
            }

            ae_arr_free(line);
            break;
        }

        case AE_STATE_LOGIN: {
            ae_byte_arr_t line;
            ae_arr_new(line);

            if (ae_get_line(&line, in)) {
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
