#ifndef MMO_ARR_H
#define MMO_ARR_H

#include <stdlib.h>
#include <string.h>

#include <mmo/meta.h>

#define mmo_arr(T) T *

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

#define mmo_arr_free(A) free(((size_t *)A) - 2);

#endif
