#include <mmo/io.h>

#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>

#include <mmo/log.h>
#include <mmo/game.h>
#include <mmo/arr/char.h>

static bool mmo_has_complete_line(mmo_char_arr_t *in, size_t *len) {
    assert(in);
    assert(len);

    for (*len = 0; *len < in->num_elems; *len += 1) {
        if (in->elems[*len] == '\r' || in->elems[*len] == '\n') {
            return true;
        }
    }

    return false;
}

bool mmo_get_line(mmo_char_arr_t *line, mmo_char_arr_t *in) {
    assert(line);
    assert(in);

    /* Find line length. */

    size_t len;

    if (!mmo_has_complete_line(in, &len)) {
        return false;
    }

    /* Append printable characters to line buffer. */
    for (size_t i = 0; i < len; i += 1) {
        if (in->elems[i] >= 0x20 && in->elems[i] <= 0x7e) {
            mmo_char_arr_append(line, &in->elems[i]);
        }
    }

    /* Consume line and newline characters. */

    for (; len < in->num_elems &&
           (in->elems[len] == '\r' || in->elems[len] == '\n');
         len += 1)
        ;

    mmo_char_arr_remove_n(in, 0, len);

    return true;
}

bool mmo_str_eq_ignore_case(mmo_char_span_t a, mmo_char_span_t b) {
    if (a.num_elems != b.num_elems) {
        return false;
    }

    for (size_t i = 0; i < a.num_elems; i += 1) {
        if (tolower(a.elems[i]) != tolower(b.elems[i])) {
            return false;
        }
    }

    return true;
}

void mmo_prompt(mmo_player_t *player, mmo_server_t *server) {
    char *prompt = ">";

    mmo_server_send(
        server, player->handle,
        (mmo_char_span_t){.elems = prompt, .num_elems = strlen(prompt)});
}

void mmo_print_fmt(mmo_player_t *player, mmo_server_t *server,
                   mmo_print_type_t action, const char *fmt, ...) {
    mmo_char_span_t out = {.elems = "\r\n", .num_elems = 2};

    if (action == MMO_PRINT_INTERRUPT) {
        mmo_server_send(server, player->handle, out);
    }

    char buf[1024];

    va_list args;
    va_start(args, fmt);

    size_t len = (size_t)vsnprintf(buf, sizeof buf, fmt, args);

    va_end(args);

    out.elems     = buf;
    out.num_elems = len;
    mmo_server_send(server, player->handle, out);

    out.elems     = "\r\n";
    out.num_elems = 2;
    mmo_server_send(server, player->handle, out);

    mmo_prompt(player, server);
}
