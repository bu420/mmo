#include <ae/io.h>

#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>

#include <ae/arr.h>

static bool ae_has_complete_line(ae_byte_arr_t in, size_t *len) {
    for (*len = 0; *len < ae_arr_len(in); *len += 1) {
        if (in[*len] == '\r' || in[*len] == '\n') {
            return true;
        }
    }
    return false;
}

bool ae_get_line(ae_byte_arr_t line, ae_byte_arr_t in) {
    /* Find line length. */

    size_t len;

    if (!ae_has_complete_line(in, &len)) {
        return false;
    }

    /* Append printable characters to line buffer. */
    for (size_t i = 0; i < len; i += 1) {
        if (in[i] >= 0x20 && in[i] <= 0x7e) {
            ae_arr_append(line, in[i]);
        }
    }

    /* Consume line and newline characters. */

    for (; len < ae_arr_len(in) && (in[len] == '\r' || in[len] == '\n');
         len += 1)
        ;

    ae_arr_remove_n(in, 0, len);

    return true;
}

bool ae_str_eq_ignore_case(ae_byte_arr_t a, ae_byte_arr_t b) {
    if (ae_arr_len(a) != ae_arr_len(b)) {
        return false;
    }

    for (size_t i = 0; i < ae_arr_len(a); i += 1) {
        if (tolower(a[i]) != tolower(b[i])) {
            return false;
        }
    }

    return true;
}

void ae_prompt(ae_user_t *user, ae_app_t *app) {
    char *prompt = ">";

    ae_byte_arr_t msg;
    ae_arr_new(msg);
    ae_arr_append_n(msg, strlen(prompt), prompt);

    ae_server_send(&app->server, user->handle, msg);
}

void ae_print_fmt(ae_user_t *user, ae_app_t *app, ae_print_type_t action,
                  const char *fmt, ...) {
    ae_byte_arr_t out;
    ae_arr_from_string_literal(out, "\r\n", 2);

    if (action == AE_PRINT_INTERRUPT) {
        ae_server_send(&app->server, user->handle, out);
    }

    ae_arr_new_reserve(out, 1024);

    va_list args;
    va_start(args, fmt);

    ae_arr_len(out) =
        (size_t)vsnprintf((char *)out, ae_arr_cap(out), fmt, args);

    va_end(args);

    ae_server_send(&app->server, user->handle, out);

    ae_arr_free(out);

    ae_arr_from_string_literal(out, "\r\n", 2);
    ae_server_send(&app->server, user->handle, out);

    ae_prompt(user, app);
}
