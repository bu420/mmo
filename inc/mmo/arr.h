#ifndef MMO_ARR_H
#define MMO_ARR_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <mmo/meta.h>
#include <mmo/literal.h>

#define mmo_arr(T) T *

typedef mmo_arr(char) mmo_string_t;
typedef mmo_arr(uint8_t) mmo_byte_arr_t;

#define mmo_arr_len(A) ((size_t *)A)[-2]
#define mmo_arr_cap(A) ((size_t *)A)[-1]

#define mmo_arr_new(A)                                                         \
    mmo_generic_assign(A, (size_t *)(calloc(2, sizeof(size_t))) + 2)

#define mmo_arr_new_n(A, N)                                                    \
    do {                                                                       \
        mmo_generic_assign(                                                    \
            A, (size_t *)malloc(2 * sizeof(size_t) + (N) * sizeof(A[0])) + 2); \
        mmo_arr_len(A) = mmo_arr_cap(A) = (N);                                 \
    } while (0);

#define mmo_arr_new_n_zero(A, N)                                               \
    do {                                                                       \
        mmo_generic_assign(                                                    \
            A,                                                                 \
            (size_t *)calloc(1, 2 * sizeof(size_t) + (N) * sizeof(A[0])) + 2); \
        mmo_arr_len(A) = mmo_arr_cap(A) = (N);                                 \
    } while (0);

#define mmo_arr_new_reserve(A, N)                                              \
    do {                                                                       \
        mmo_generic_assign(                                                    \
            A, (size_t *)malloc(2 * sizeof(size_t) + (N) * sizeof(A[0])) + 2); \
        mmo_arr_len(A) = 0;                                                    \
        mmo_arr_cap(A) = (N);                                                  \
    } while (0);

#define mmo_arr_new_reserve_zero(A, N)                                         \
    do {                                                                       \
        mmo_generic_assign(                                                    \
            A,                                                                 \
            (size_t *)calloc(1, 2 * sizeof(size_t) + (N) * sizeof(A[0])) + 2); \
        mmo_arr_len(A) = 0;                                                    \
        mmo_arr_cap(A) = (N);                                                  \
    } while (0);

#define mmo_arr_from_string_literal(A, L, N)                                   \
    do {                                                                       \
        mmo_generic_assign(A,                                                  \
                           mmo_size_to_literal(N) mmo_size_to_literal(N) L);   \
        mmo_static_assert(sizeof(L) - 1 == N,                                  \
                          "Invalid literal length passed.");                   \
    } while (0)

#define mmo_arr_free(A) free(((size_t *)A) - 2);

#define mmo_arr_insert_n(A, I, N, B)                                           \
    do {                                                                       \
        assert((I) <= mmo_arr_len(A));                                         \
                                                                               \
        while (mmo_arr_len(A) + (N) > mmo_arr_cap(A)) {                        \
            mmo_arr_cap(A) = mmo_arr_cap(A) == 0 ? (N) : mmo_arr_cap(A) * 2;   \
        }                                                                      \
                                                                               \
        mmo_generic_assign(                                                    \
            A, (size_t *)(realloc((size_t *)A - 2,                             \
                                  2 * sizeof(size_t) +                         \
                                      mmo_arr_cap(A) * sizeof(A[0]))) +        \
                   2);                                                         \
        assert(A);                                                             \
                                                                               \
        if ((I) != mmo_arr_len(A)) {                                           \
            memmove(A + (I) + (N), B + (I),                                    \
                    (mmo_arr_len(A) - (I)) * sizeof(A[0]));                    \
        }                                                                      \
                                                                               \
        memcpy(A + (I), B, (N) * sizeof(A[0]));                                \
                                                                               \
        mmo_arr_len(A) += (N);                                                 \
    } while (0)

#define mmo_arr_insert(A, I, E) mmo_arr_insert_n(A, I, 1, &E)

#define mmo_arr_append_n(A, N, B) mmo_arr_insert_n(A, mmo_arr_len(A), N, B)

#define mmo_arr_append(A, E) mmo_arr_insert(A, mmo_arr_len(A), E)

#define mmo_arr_remove_n(A, I, N)                                              \
    do {                                                                       \
        assert((I) < mmo_arr_len(A));                                          \
        assert((I) + (N) <= mmo_arr_len(A));                                   \
                                                                               \
        if ((I) + (N) < mmo_arr_len(A)) {                                      \
            memmove(A + (I), A + (I) + (N),                                    \
                    (mmo_arr_len(A) - (I) - (N)) * sizeof(A[0]));              \
        }                                                                      \
                                                                               \
        mmo_arr_len(A) -= (N);                                                 \
    } while (0)

#define mmo_arr_remove(A, I) mmo_arr_remove_n(A, I, 1)

#define mmo_arr_remove_ptr(A, P) mmo_arr_remove(A, P - A)

#endif
