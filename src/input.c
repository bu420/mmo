#include <mmo/input.h>

#include <string.h>
#include <assert.h>
#include <stdint.h>

#include <mmo/log.h>
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

    for (; len < in->num_elems && (in->elems[len] == '\r' || in->elems[len] == '\n'); len += 1);

    mmo_char_arr_remove_n(in, 0, len);

    return true;
}
