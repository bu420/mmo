#ifndef AE_META_H
#define AE_META_H

/* -------------------------------------------------------------------------
   Macro metaprogramming.
   ------------------------------------------------------------------------- */

#define ae_concat2(A, B) A##B
#define ae_concat(A, B)  ae_concat2(A, B)

/* Generate unique name for internal macro identifiers. */
#define ae_uniq(NAME) ae_concat(NAME, __LINE__)

#ifdef __GNUC__
#define ae_static_assert(COND, MSG)                                           \
    __attribute__((unused)) typedef char ae_uniq(                             \
        ae_static_assert)[(COND) ? 1 : -1]
#else
#define ae_static_assert(COND, MSG)
#endif

/* Assign pointer to pointer without type cast. */
#define ae_generic_assign(A, B)                                               \
    do {                                                                      \
        void *ae_uniq(m) = B;                                                 \
        memcpy(&A, &ae_uniq(m), sizeof(void *));                              \
    } while (0)

#define ae_foreach(N, I) for (size_t I = 0; I < (N); I++)

/* -------------------------------------------------------------------------
   Generic type-safe dynamic array.
   Metadata about array length and capacity is stored at
   indices -2 and -1 and array elements begin like normal at index 0.
   ------------------------------------------------------------------------- */

#define ae_arr(T) T *

typedef ae_arr(unsigned char) ae_bytes_t;
typedef ae_arr(int) ae_ints_t;
typedef ae_arr(double) ae_doubles_t;

#define ae_alen(A) ((size_t *)(A))[-2]
#define ae_acap(A) ((size_t *)(A))[-1]

#define ae_arr_foreach(A, I) for (size_t I = 0; I < ae_alen(A); I++)

#define ae_arr_new(A)                                                         \
    ae_generic_assign((A), (size_t *)(calloc(2, sizeof(size_t))) + 2);

#define ae_arr_new_n(A, N)                                                    \
    do {                                                                      \
        ae_generic_assign(                                                    \
            (A),                                                              \
            (size_t *)malloc(2 * sizeof(size_t) + (N) * sizeof((A)[0])) + 2); \
        ae_alen(A) = ae_acap(A) = (N);                                        \
    } while (0);

#define ae_arr_new_n_zero(A, N)                                               \
    do {                                                                      \
        ae_generic_assign(                                                    \
            (A),                                                              \
            (size_t *)calloc(1, 2 * sizeof(size_t) + (N) * sizeof((A)[0])) +  \
                2);                                                           \
        ae_alen(A) = ae_acap(A) = (N);                                        \
    } while (0);

#define ae_arr_new_reserve(A, N)                                              \
    do {                                                                      \
        ae_generic_assign(                                                    \
            (A),                                                              \
            (size_t *)malloc(2 * sizeof(size_t) + (N) * sizeof((A)[0])) + 2); \
        ae_alen(A) = 0;                                                       \
        ae_acap(A) = (N);                                                     \
    } while (0);

#define ae_arr_new_reserve_zero(A, N)                                         \
    do {                                                                      \
        ae_generic_assign(                                                    \
            (A),                                                              \
            (size_t *)calloc(1, 2 * sizeof(size_t) + (N) * sizeof((A)[0])) +  \
                2);                                                           \
        ae_alen(A) = 0;                                                       \
        ae_acap(A) = (N);                                                     \
    } while (0);

#if !defined(__SIZEOF_SIZE_T__) || __SIZEOF_SIZE_T__ == 8
#define AE_ZEROES "\x00\x00\x00\x00\x00\x00"
#elif __SIZEOF_SIZE_T__ == 16
#define AE_ZEROES "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#elif __SIZEOF_SIZE_T__ == 4
#define AE_ZEROES "\x00\x00"
#else
#define AE_ZEROES
#endif

#include <ae/hex.h>

#if !defined(__BYTE_ORDER__) || __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define ae_literal(LO, HI)                                                    \
    ae_concat(AE_HEX_, LO) ae_concat(AE_HEX_, HI) AE_ZEROES
#else
#define ae_literal(LO, HI)                                                    \
    ae_concat(AE_HEX_, HI) ae_concat(AE_HEX_, LO) AE_ZEROES
#endif

/* Embed string literal and metadata in array without using heap memory. */
#define ae_arr_from_string_literal(A, L, LO, HI)                              \
    do {                                                                      \
        ae_static_assert(sizeof(L) - 1 == (((HI) << 8) | (LO)),               \
                         "Invalid length.");                                  \
        ae_generic_assign((A), (ae_literal(LO, HI) ae_literal(LO, HI) L) +    \
                                   2 * sizeof(size_t));                       \
    } while (0)

#define ae_arr_free(A) free(((size_t *)A) - 2);

#define ae_arr_resize(A, N)                                                   \
    do {                                                                      \
        ae_generic_assign(                                                    \
            (A),                                                              \
            (size_t *)(realloc((size_t *)(A) - 2,                             \
                               2 * sizeof(size_t) + (N) * sizeof((A)[0]))) +  \
                2);                                                           \
        ae_alen(A) = ae_acap(A) = (N);                                        \
    } while (0)

#define ae_arr_insert_raw_n(A, I, N)                                          \
    do {                                                                      \
        assert((I) <= ae_alen(A));                                            \
                                                                              \
        while (ae_alen(A) + (N) > ae_acap(A)) {                               \
            ae_acap(A) = ae_acap(A) == 0 ? (N) : ae_acap(A) * 2;              \
        }                                                                     \
                                                                              \
        ae_generic_assign(                                                    \
            (A), (size_t *)(realloc((size_t *)(A) - 2,                        \
                                    2 * sizeof(size_t) +                      \
                                        ae_acap(A) * sizeof((A)[0]))) +       \
                     2);                                                      \
                                                                              \
        if ((I) != ae_alen(A)) {                                              \
            memmove((A) + (I) + (N), (A) + (I),                               \
                    (ae_alen(A) - (I)) * sizeof((A)[0]));                     \
        }                                                                     \
                                                                              \
        ae_alen(A) += (N);                                                    \
    } while (0)

#define ae_arr_insert_raw(A, I) ae_arr_insert_raw_n(A, I, 1)

#define ae_arr_append_n_raw(A, N) ae_arr_insert_raw_n(A, ae_alen(A), N)

#define ae_arr_append_raw(A) ae_arr_append_n_raw(A, 1)

#define ae_arr_insert_n(A, I, N, B)                                           \
    do {                                                                      \
        ae_arr_insert_raw_n(A, I, N);                                         \
        memcpy((A) + (I), (B), (N) * sizeof((A)[0]));                         \
    } while (0)

#define ae_arr_insert(A, I, E) ae_arr_insert_n(A, I, 1, &(E))

#define ae_arr_append_n(A, N, B) ae_arr_insert_n(A, ae_alen(A), N, B)

#define ae_arr_append(A, E) ae_arr_append_n(A, 1, &(E))

#define ae_arr_remove_n(A, I, N)                                              \
    do {                                                                      \
        assert((I) < ae_alen(A));                                             \
        assert((I) + (N) <= ae_alen(A));                                      \
                                                                              \
        if ((I) + (N) < ae_alen(A)) {                                         \
            memmove((A) + (I), (A) + (I) + (N),                               \
                    (ae_alen(A) - (I) - (N)) * sizeof((A)[0]));               \
        }                                                                     \
                                                                              \
        ae_alen(A) -= (N);                                                    \
    } while (0)

#define ae_arr_remove(A, I) ae_arr_remove_n(A, I, 1)

#define ae_arr_remove_ptr(A, P) ae_arr_remove(A, (size_t)((P) - (A)))

/* -------------------------------------------------------------------------
   Generic type-safe hash map.
   ------------------------------------------------------------------------- */

#define ae_map(K, V)                                                          \
    struct {                                                                  \
        ae_arr(ae_arr(struct {                                                \
            K key;                                                            \
            V val;                                                            \
        })) bkts;                                                             \
                                                                              \
        /* Number of entries. */                                              \
        size_t len;                                                           \
        /* Number of populated buckets. */                                    \
        size_t pop_bkts;                                                      \
    }

/* Warning: the map is read-only while iterating. */
#define ae_map_foreach(M, K, V)                                               \
    ae_arr_foreach((M).bkts, ae_uniq(i)) ae_arr_foreach(                      \
        (M).bkts[ae_uniq(i)],                                                 \
        ae_uniq(                                                              \
            j)) for (bool ae_uniq(once) = true;                               \
                     ae_uniq(                                                 \
                         once);) for ((K) =                                   \
                                          &((M).bkts[ae_uniq(i)][ae_uniq(j)]  \
                                                .key),                        \
                                      (V) =                                   \
                                          &((M).bkts[ae_uniq(i)][ae_uniq(j)]  \
                                                .val);                        \
                                      ae_uniq(once); ae_uniq(once) = false)

#define ae_map_new_reserve(M, N)                                              \
    do {                                                                      \
        ae_arr_new_n((M).bkts, N);                                            \
        (M).len = (M).pop_bkts = 0;                                           \
                                                                              \
        ae_arr_foreach((M).bkts, ae_uniq(i)) {                                \
            ae_arr_new_reserve((M).bkts[ae_uniq(i)], 2);                      \
        }                                                                     \
    } while (0)

#define ae_map_new(M) ae_map_new_reserve(M, 16)

#define ae_map_free(M)                                                        \
    do {                                                                      \
        ae_arr_foreach((M).bkts, ae_uniq(i)) {                                \
            ae_arr_free((M).bkts[ae_uniq(i)]);                                \
        }                                                                     \
        ae_arr_free((M).bkts);                                                \
    } while (0)

#define ae_map_get_maybe_null(M, HASH, EQ, K, V)                              \
    do {                                                                      \
        (V)                  = NULL;                                          \
        size_t ae_uniq(hash) = HASH(&(K)) % ae_acap((M).bkts);                \
                                                                              \
        ae_arr_foreach((M).bkts[ae_uniq(hash)], ae_uniq(i)) {                 \
            if (EQ(&(M).bkts[ae_uniq(hash)][ae_uniq(i)].key, &(K))) {         \
                (V) = &(M).bkts[ae_uniq(hash)][ae_uniq(i)].val;               \
                break;                                                        \
            }                                                                 \
        }                                                                     \
    } while (0)

#define ae_map_get(M, HASH, EQ, K, V)                                         \
    do {                                                                      \
        ae_map_get_maybe_null(M, HASH, EQ, K, V);                             \
        assert(V);                                                            \
    } while (0)

#define ae_map_contains(M, HASH, EQ, K, BOOL)                                 \
    do {                                                                      \
        void *ae_uniq(v);                                                     \
        ae_map_get_maybe_null(M, HASH, EQ, K, ae_uniq(v));                    \
        (BOOL) = ae_uniq(v) != NULL;                                          \
    } while (0)

#define ae_map_set_no_rehash(M, CAP, HASH, EQ, K, V)                          \
    do {                                                                      \
        size_t ae_uniq(hash) = HASH(&(K)) % (CAP);                            \
        /* If two keys within a bucket match the value will be replaced.      \
           Otherwise append new entry to bucket. */                           \
        bool ae_uniq(replaced) = false;                                       \
                                                                              \
        ae_foreach(ae_alen((M).bkts[ae_uniq(hash)]), ae_uniq(_i)) {           \
            if (EQ(&(M).bkts[ae_uniq(hash)][ae_uniq(_i)].key, &(K))) {        \
                (M).bkts[ae_uniq(hash)][ae_uniq(_i)].val = V;                 \
                ae_uniq(replaced)                        = true;              \
                break;                                                        \
            }                                                                 \
        }                                                                     \
                                                                              \
        if (!ae_uniq(replaced)) {                                             \
            (M).len++;                                                        \
                                                                              \
            if (ae_alen((M).bkts[ae_uniq(hash)]) == 0) {                      \
                (M).pop_bkts++;                                               \
            }                                                                 \
                                                                              \
            ae_arr_append_raw((M).bkts[ae_uniq(hash)]);                       \
            ((M).bkts[ae_uniq(hash)])[ae_alen((M).bkts[ae_uniq(hash)]) - 1]   \
                .key = K;                                                     \
            ((M).bkts[ae_uniq(hash)])[ae_alen((M).bkts[ae_uniq(hash)]) - 1]   \
                .val = V;                                                     \
        }                                                                     \
    } while (0)

#define ae_map_set(M, HASH, EQ, K, V)                                         \
    do {                                                                      \
        /* Grow array if needed. */                                           \
        if ((M).pop_bkts == ae_acap((M).bkts) / 2) {                          \
            (M).len = (M).pop_bkts = 0;                                       \
                                                                              \
            size_t ae_uniq(old_cap) = ae_acap((M).bkts);                      \
            size_t ae_uniq(new_cap) = ae_acap((M).bkts) * 2;                  \
                                                                              \
            /* Double number of buckets. Also allocate a 3rd region to        \
               temporarily store the old data while re-hashing. */            \
            ae_arr_resize((M).bkts, ae_uniq(new_cap) + ae_uniq(old_cap));     \
                                                                              \
            /* Move old data to the 3rd region. */                            \
            memcpy((M).bkts + ae_uniq(new_cap), (M).bkts,                     \
                   (ae_uniq(old_cap)) * sizeof((M).bkts[0]));                 \
            memset((M).bkts, 0x0, ae_uniq(old_cap) * sizeof((M).bkts[0]));    \
                                                                              \
            /* Initialize new buckets. */                                     \
            ae_foreach(ae_uniq(new_cap), ae_uniq(bkt)) {                      \
                ae_arr_new_reserve((M).bkts[ae_uniq(bkt)], 2);                \
            }                                                                 \
                                                                              \
            /* Re-hash. */                                                    \
            ae_foreach(ae_uniq(old_cap), ae_uniq(bkt)) {                      \
                ae_arr_foreach(((M).bkts + ae_uniq(new_cap))[ae_uniq(bkt)],   \
                               ae_uniq(i)) {                                  \
                    ae_map_set_no_rehash(                                     \
                        M, ae_uniq(new_cap), HASH, EQ,                        \
                        ((M).bkts +                                           \
                         ae_uniq(new_cap))[ae_uniq(bkt)][ae_uniq(i)]          \
                            .key,                                             \
                        ((M).bkts +                                           \
                         ae_uniq(new_cap))[ae_uniq(bkt)][ae_uniq(i)]          \
                            .val);                                            \
                }                                                             \
                                                                              \
                /* Free old bucket after re-hash. */                          \
                ae_arr_free(((M).bkts + ae_uniq(new_cap))[ae_uniq(bkt)]);     \
            }                                                                 \
                                                                              \
            /* Shrink array to new capacity. */                               \
            ae_acap((M).bkts) = ae_uniq(new_cap);                             \
        }                                                                     \
                                                                              \
        /* Insert new value. */                                               \
        ae_map_set_no_rehash(M, ae_acap((M).bkts), HASH, EQ, K, V);           \
    } while (0)

#define ae_map_remove(M, HASH, EQ, K)                                         \
    do {                                                                      \
        size_t ae_uniq(hash) = HASH(&(K)) % ae_acap((M).bkts);                \
                                                                              \
        ae_arr_foreach((M).bkts[ae_uniq(hash)], ae_uniq(i)) {                 \
            if (EQ(&(M).bkts[ae_uniq(hash)][ae_uniq(i)].key, &(K))) {         \
                ae_arr_remove((M).bkts[ae_uniq(hash)], ae_uniq(i));           \
                                                                              \
                (M).len--;                                                    \
                                                                              \
                if (ae_alen((M).bkts[ae_uniq(hash)]) == 0) {                  \
                    (M).pop_bkts--;                                           \
                }                                                             \
                break;                                                        \
            }                                                                 \
        }                                                                     \
    } while (0)

#endif
