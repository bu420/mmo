#include <mmo/io.h>

#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>

#include <mmo/arr.h>

static bool mmo_has_complete_line(mmo_byte_arr_t in, size_t *len) {
    for (*len = 0; *len < mmo_arr_len(in); *len += 1) {
        if (in[*len] == '\r' || in[*len] == '\n') {
            return true;
        }
    }
    return false;
}

bool mmo_get_line(mmo_byte_arr_t line, mmo_byte_arr_t in) {
    /* Find line length. */

    size_t len;

    if (!mmo_has_complete_line(in, &len)) {
        return false;
    }

    /* Append printable characters to line buffer. */
    for (size_t i = 0; i < len; i += 1) {
        if (in[i] >= 0x20 && in[i] <= 0x7e) {
            mmo_arr_append(line, in[i]);
        }
    }

    /* Consume line and newline characters. */

    for (; len < mmo_arr_len(in) && (in[len] == '\r' || in[len] == '\n');
         len += 1)
        ;

    mmo_arr_remove_n(in, 0, len);

    return true;
}

bool mmo_str_eq_ignore_case(mmo_byte_arr_t a, mmo_byte_arr_t b) {
    if (mmo_arr_len(a) != mmo_arr_len(b)) {
        return false;
    }

    for (size_t i = 0; i < mmo_arr_len(a); i += 1) {
        if (tolower(a[i]) != tolower(b[i])) {
            return false;
        }
    }

    return true;
}

void mmo_prompt(mmo_user_t *user, mmo_app_t *app) {
    char *prompt = ">";

    mmo_byte_arr_t msg;
    mmo_arr_new(msg);
    mmo_arr_append_n(msg, strlen(prompt), prompt);

    mmo_server_send(&app->server, user->handle, msg);
}

void mmo_print_fmt(mmo_user_t *user, mmo_app_t *app,
                   mmo_print_type_t action, const char *fmt, ...) {
    mmo_byte_arr_t out;
    mmo_arr_from_string_literal(out, "\r\n", 2);

    if (action == MMO_PRINT_INTERRUPT) {
        mmo_server_send(&app->server, user->handle, out);
    }

    mmo_arr_new_reserve(out, 1024);

    va_list args;
    va_start(args, fmt);

    mmo_arr_len(out) = (size_t)vsnprintf((char *)out, mmo_arr_cap(out), fmt, args);

    va_end(args);

    mmo_server_send(&app->server, user->handle, out);

    mmo_arr_free(out);

    mmo_arr_from_string_literal(out, "\r\n", 2);
    mmo_server_send(&app->server, user->handle, out);

    mmo_prompt(user, app);
}
