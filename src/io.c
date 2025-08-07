#include <ae/io.h>

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

static bool ae_has_complete_line(const ae_string_t in, size_t *len) {
    for (*len = 0; *len < ae_alen(in); *len += 1) {
        if (in[*len] == '\r' || in[*len] == '\n') {
            return true;
        }
    }
    return false;
}

bool ae_get_line(ae_string_t *line, ae_string_t *in) {
    /* Find line length. */

    size_t len;

    if (!ae_has_complete_line(*in, &len)) {
        return false;
    }

    /* Append printable characters to line buffer. */
    for (size_t i = 0; i < len; i++) {
        if ((*in)[i] >= 0x20 && (*in)[i] <= 0x7e) {
            ae_arr_append(*line, (*in)[i]);
        }
    }

    /* Consume line and newline characters. */

    for (; len < ae_alen(*in) && ((*in)[len] == '\r' || (*in)[len] == '\n');
         len += 1)
        ;

    ae_arr_remove_n((*in), 0, len);

    return true;
}

void ae_prompt(const ae_user_t *user, ae_app_t *app) {
    ae_bytes_t msg;
    ae_arr_from_string_literal(msg, ">", 1, 0);
    ae_server_send(&app->server, user->handle, msg);
}

void ae_print_fmt(const ae_user_t *user, ae_app_t *app, ae_print_type_t action,
                  const char *fmt, ...) {
    ae_bytes_t nl;
    ae_arr_from_string_literal(nl, "\r\n", 2, 0);

    if (action == AE_PRINT_INTERRUPT) {
        ae_server_send(&app->server, user->handle, nl);
    }

    ae_bytes_t out;
    ae_arr_new_reserve(out, 1024);

    va_list args;
    va_start(args, fmt);
    ae_alen(out) =
        (size_t)vsnprintf((char *)out, ae_acap(out), fmt, args);
    va_end(args);

    ae_server_send(&app->server, user->handle, out);
    ae_arr_free(out);

    ae_server_send(&app->server, user->handle, nl);
    ae_prompt(user, app);
}
