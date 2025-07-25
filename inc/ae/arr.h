#ifndef AE_ARR_H
#define AE_ARR_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <ae/meta.h>
#include <ae/literal.h>

#define ae_arr(T) T *

typedef ae_arr(char) ae_string_t;
typedef ae_arr(uint8_t) ae_byte_arr_t;

#define ae_arr_len(A) ((size_t *)(A))[-2]
#define ae_arr_cap(A) ((size_t *)(A))[-1]

#define ae_arr_new(A)                                                         \
    ae_generic_assign((A), (size_t *)(calloc(2, sizeof(size_t))) + 2);

#define ae_arr_new_n(A, N)                                                    \
    do {                                                                      \
        ae_generic_assign(                                                    \
            (A),                                                              \
            (size_t *)malloc(2 * sizeof(size_t) + (N) * sizeof((A)[0])) + 2); \
        ae_arr_len(A) = ae_arr_cap(A) = (N);                                  \
    } while (0);

#define ae_arr_new_n_zero(A, N)                                               \
    do {                                                                      \
        ae_generic_assign(                                                    \
            (A),                                                              \
            (size_t *)calloc(1, 2 * sizeof(size_t) + (N) * sizeof((A)[0])) +  \
                2);                                                           \
        ae_arr_len(A) = ae_arr_cap(A) = (N);                                  \
    } while (0);

#define ae_arr_new_reserve(A, N)                                              \
    do {                                                                      \
        ae_generic_assign(                                                    \
            (A),                                                              \
            (size_t *)malloc(2 * sizeof(size_t) + (N) * sizeof((A)[0])) + 2); \
        ae_arr_len(A) = 0;                                                    \
        ae_arr_cap(A) = (N);                                                  \
    } while (0);

#define ae_arr_new_reserve_zero(A, N)                                         \
    do {                                                                      \
        ae_generic_assign(                                                    \
            (A),                                                              \
            (size_t *)calloc(1, 2 * sizeof(size_t) + (N) * sizeof((A)[0])) +  \
                2);                                                           \
        ae_arr_len(A) = 0;                                                    \
        ae_arr_cap(A) = (N);                                                  \
    } while (0);

#define ae_arr_from_string_literal(A, L, N)                                   \
    do {                                                                      \
        ae_static_assert(sizeof(L) - 1 == (N),                                \
                         "Invalid literal length passed.");                   \
        ae_generic_assign((A),                                                \
                          (ae_size_to_literal(N) ae_size_to_literal(N) L) +   \
                              2 * sizeof(size_t));                            \
    } while (0)

#define ae_arr_free(A) free(((size_t *)A) - 2);

#define ae_arr_insert_n(A, I, N, B)                                           \
    do {                                                                      \
        assert((I) <= ae_arr_len(A));                                         \
                                                                              \
        while (ae_arr_len(A) + (N) > ae_arr_cap(A)) {                         \
            ae_arr_cap(A) = ae_arr_cap(A) == 0 ? (N) : ae_arr_cap(A) * 2;     \
        }                                                                     \
                                                                              \
        ae_generic_assign(                                                    \
            (A), (size_t *)(realloc((size_t *)(A) - 2,                        \
                                    2 * sizeof(size_t) +                      \
                                        ae_arr_cap(A) * sizeof((A)[0]))) +    \
                     2);                                                      \
                                                                              \
        if ((I) != ae_arr_len(A)) {                                           \
            memmove((A) + (I) + (N), (A) + (I),                               \
                    (ae_arr_len(A) - (I)) * sizeof((A)[0]));                  \
        }                                                                     \
                                                                              \
        memcpy((A) + (I), (B), (N) * sizeof((A)[0]));                         \
                                                                              \
        ae_arr_len(A) += (N);                                                 \
    } while (0)

#define ae_arr_insert(A, I, E) ae_arr_insert_n(A, I, 1, &(E))

#define ae_arr_append_n(A, N, B) ae_arr_insert_n(A, ae_arr_len(A), N, B)

#define ae_arr_append(A, E) ae_arr_insert(A, ae_arr_len(A), E)

#define ae_arr_remove_n(A, I, N)                                              \
    do {                                                                      \
        assert((I) < ae_arr_len(A));                                          \
        assert((I) + (N) <= ae_arr_len(A));                                   \
                                                                              \
        if ((I) + (N) < ae_arr_len(A)) {                                      \
            memmove((A) + (I), (A) + (I) + (N),                               \
                    (ae_arr_len(A) - (I) - (N)) * sizeof((A)[0]));            \
        }                                                                     \
                                                                              \
        ae_arr_len(A) -= (N);                                                 \
    } while (0)

#define ae_arr_remove(A, I) ae_arr_remove_n(A, I, 1)

#define ae_arr_remove_ptr(A, P) ae_arr_remove(A, (size_t)((P) - (A)))

#endif
