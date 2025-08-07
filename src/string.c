#include <ae/string.h>

#include <string.h>
#include <ctype.h>

uint64_t ae_string_hash(const ae_string_t *key) {
    uint64_t hash = 14695981039346656037UL;

    for (size_t i = 0; i < ae_alen(key); i++) {
        hash ^= (uint64_t)(uint8_t)(*key)[i];
        hash *= 1099511628211UL;
    }
    return hash;
}

bool ae_string_eq(const ae_string_t *a, const ae_string_t *b) {
    return ae_alen(*a) == ae_alen(*b) && memcmp(*a, *b, ae_alen(*a));
}

bool ae_string_eq_ignore_case(const ae_string_t a, const ae_string_t b) {
    if (ae_alen(a) != ae_alen(b)) {
        return false;
    }

    for (size_t i = 0; i < ae_alen(a); i += 1) {
        if (tolower(a[i]) != tolower(b[i])) {
            return false;
        }
    }

    return true;
}
