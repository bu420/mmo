#include <ae/string.h>

#include <string.h>
#include <ctype.h>

uint64_t ae_string_hash(const ae_string_t key) {
    uint64_t hash = 14695981039346656037UL;

    for (size_t i = 0; i < ae_arr_len(key); i++) {
        hash ^= (uint64_t)(uint8_t)key[i];
        hash *= 1099511628211UL;
    }
    return hash;
}

bool ae_string_eq(const ae_string_t a, const ae_string_t b) {
    return ae_arr_len(a) == ae_arr_len(b) && memcmp(a, b, ae_arr_len(a));
}

bool ae_string_eq_ignore_case(const ae_string_t a, const ae_string_t b) {
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
