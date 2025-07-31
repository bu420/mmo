#ifndef AE_STRING_H
#define AE_STRING_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <ae/meta.h>

typedef ae_arr(char) ae_string_t;
typedef ae_map(ae_string_t, int) ae_string_to_int_map_t;
typedef ae_map(ae_string_t, ae_string_t) ae_string_to_string_map_t;

uint64_t ae_string_hash(const ae_string_t key);
bool ae_string_eq(const ae_string_t a, const ae_string_t b);
bool ae_string_eq_ignore_case(const ae_string_t a, const ae_string_t b);

#endif
