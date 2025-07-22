#include <mmo/app.h>

#include <assert.h>

#include <mmo/meta.h>
#include <mmo/io.h>

void mmo_state_greeting_new(mmo_user_t *user, mmo_app_t *app) {
    static char *greeting =
        "-_____                  ,  ,,               ,, ,,            "
        "  ' | -,               ||  ||           _   || ||            "
        " /| |  |`  _-_  -_-_  =||= ||/\\\\  _-_  < \\, || ||  _-_  ,._-_"
        " || |==|| || \\\\ || \\\\  ||  || || ||    /-|| || || || \\\\  ||  "
        "~|| |  |, ||/   || ||  ||  || || ||   (( || || || ||/    ||  "
        " ~-____,  \\\\,/  ||-'   \\\\, \\\\ |/ \\\\,/  \\/\\\\ \\\\ \\\\ "
        "\\\\,/   \\\\, "
        "(               |/           _/                              "
        "                '                     Depthcaller v0.1.0"
        "~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~\r\n"
        "The fog parts as you arrive.\r\n"
        "You have come to the edge of all charted seas.\r\n"
        "The creatures of the deep lurk below.\r\n"
        "\r\n"
        "Enter NEW to signup or enter your username to login.\r\n";

    mmo_server_send(
        server, user->handle,
        (mmo_char_span_t){.elems = greeting, .num_elems = sizeof greeting});

    mmo_prompt(user, server);
}

void mmo_state_greeting_free(mmo_user_t *user, mmo_server_t *server) {
}

void mmo_state_greeting_update(mmo_user_t *user, mmo_game_t *game,
                               mmo_server_t *server, mmo_char_arr_t *in) {
    mmo_char_arr_t line;
    mmo_char_arr_new(&line);

    if (mmo_get_line(&line, in)) {
        if (line.num_elems == 0) {
            mmo_prompt(user, server);
        }

        else {
            if (mmo_str_eq_ignore_case(MMO_ARR_TO_SPAN(line),
                                       MMO_LITERAL_TO_SPAN("new"))) {
                mmo_state_switch(user, server, MMO_STATE_SIGNUP);

            } else {
                mmo_state_switch(user, server, MMO_STATE_LOGIN);
            }
        }
    }

    mmo_char_arr_free(&line);
}
