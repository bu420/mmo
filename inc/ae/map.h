#ifndef AE_MAP_H
#define AE_MAP_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <ae/meta.h>

#define AE_MAP_CAP 16

typedef enum ae_map_entry_state_e {
    AE_MAP_ENTRY_STATE_EMPTY,
    AE_MAP_ENTRY_STATE_USED,
    AE_MAP_ENTRY_STATE_REMOVED
} ae_map_entry_state_t;

#define ae_map(K, V)                                                          \
    struct {                                                                  \
        K *keys;                                                              \
        K *old_keys;                                                          \
        V *vals;                                                              \
        V *old_vals;                                                          \
        ae_map_entry_state_t *states;                                         \
                                                                              \
        size_t len;                                                           \
        size_t cap;                                                           \
                                                                              \
        uint64_t (*hash)(K);                                                  \
        bool (*eq)(K, K);                                                     \
    }

typedef ae_map(char *, int) ae_string_to_int_map_t;
typedef ae_map(char *, float) ae_string_to_float_map_t;
typedef ae_map(char *, char *) ae_string_to_string_map_t;

#define ae_map_new_reserve(M, N, HASH, EQ)                                    \
    do {                                                                      \
        ae_generic_assign((M).keys, calloc((N), sizeof((M).keys[0])));        \
        ae_generic_assign((M).vals, calloc((N), sizeof((M).vals[0])));        \
        ae_generic_assign((M).states,                                         \
                          calloc((N), sizeof(ae_map_entry_state_t)));         \
                                                                              \
        (M).len = 0;                                                          \
        (M).cap = (N);                                                        \
                                                                              \
        (M).hash = (HASH);                                                    \
        (M).eq   = (EQ);                                                      \
    } while (0)

#define ae_map_new(M, HASH, EQ) ae_map_new_reserve(M, AE_MAP_CAP, HASH, EQ)

#define ae_map_free(M)                                                        \
    do {                                                                      \
        free((M).keys);                                                       \
        free((M).vals);                                                       \
        free((M).states);                                                     \
    } while (0)

#define ae_map_get(M, K, V)                                                   \
    do {                                                                      \
        (V)                 = NULL;                                           \
        size_t ae_unique(i) = (M).hash(K) % (M).cap;                          \
                                                                              \
        while ((M).states[ae_unique(i)] != AE_MAP_ENTRY_STATE_EMPTY) {        \
            if ((M).states[ae_unique(i)] == AE_MAP_ENTRY_STATE_USED &&        \
                (M).eq(K, (M).keys[ae_unique(i)])) {                          \
                (V) = &(M).vals[ae_unique(i)];                                \
                break;                                                        \
            }                                                                 \
            if (++ae_unique(i) == (M).cap) {                                  \
                ae_unique(i) = 0;                                             \
            }                                                                 \
        }                                                                     \
    } while (0)

#define _ae_map_set(M, K, V)                                                  \
    do {                                                                      \
        size_t ae_unique(i)        = (M).hash(K) % (M).cap;                   \
        ssize_t ae_unique(removed) = -1;                                      \
        bool ae_unique(set)        = false;                                   \
                                                                              \
        while ((M).states[ae_unique(i)] != AE_MAP_ENTRY_STATE_EMPTY) {        \
            if ((M).states[ae_unique(i)] == AE_MAP_ENTRY_STATE_USED &&        \
                (M).eq(K, (M).keys[ae_unique(i)])) {                          \
                (M).vals[ae_unique(i)] = V;                                   \
                ae_unique(set)         = true;                                \
                break;                                                        \
            }                                                                 \
            if ((M).states[ae_unique(i)] == AE_MAP_ENTRY_STATE_REMOVED &&     \
                ae_unique(removed) == -1) {                                   \
                ae_unique(removed) = (ssize_t)ae_unique(i);                   \
            }                                                                 \
            if (++ae_unique(i) == (M).cap) {                                  \
                ae_unique(i) = 0;                                             \
            }                                                                 \
        }                                                                     \
        if (!ae_unique(set)) {                                                \
            size_t ae_unique(j)      = (ae_unique(removed) != -1)             \
                                           ? (size_t)ae_unique(removed)       \
                                           : ae_unique(i);                    \
            (M).keys[ae_unique(j)]   = K;                                     \
            (M).vals[ae_unique(j)]   = V;                                     \
            (M).states[ae_unique(j)] = AE_MAP_ENTRY_STATE_USED;               \
            (M).len++;                                                        \
        }                                                                     \
    } while (0)

#define ae_map_set(M, K, V)                                                   \
    do {                                                                      \
        if ((M).len == (M).cap / 2) {                                         \
            (M).old_keys                                = (M).keys;           \
            (M).old_vals                                = (M).vals;           \
            ae_map_entry_state_t *ae_unique(old_states) = (M).states;         \
            size_t ae_unique(old_cap)                   = (M).cap;            \
                                                                              \
            (M).len = 0;                                                      \
            (M).cap *= 2;                                                     \
                                                                              \
            ae_generic_assign((M).keys,                                       \
                              calloc((M).cap, sizeof((M).keys[0])));          \
            ae_generic_assign((M).vals,                                       \
                              calloc((M).cap, sizeof((M).vals[0])));          \
            ae_generic_assign((M).states,                                     \
                              calloc((M).cap, sizeof(ae_map_entry_state_t))); \
                                                                              \
            for (size_t ae_unique(k) = 0; ae_unique(k) < ae_unique(old_cap);  \
                 ae_unique(k)++) {                                            \
                if (ae_unique(old_states)[ae_unique(k)] ==                    \
                    AE_MAP_ENTRY_STATE_USED) {                                \
                    _ae_map_set(M, (M).old_keys[ae_unique(k)],                \
                                (M).old_vals[ae_unique(k)]);                  \
                }                                                             \
            }                                                                 \
                                                                              \
            free((M).old_keys);                                               \
            free((M).old_vals);                                               \
            free(ae_unique(old_states));                                      \
        }                                                                     \
                                                                              \
        _ae_map_set(M, K, V);                                                 \
    } while (0)

#define ae_map_remove(M, K)                                                   \
    do {                                                                      \
        size_t ae_unique(i) = (M).hash(K) % (M).cap;                          \
                                                                              \
        while ((M).states[ae_unique(i)] != AE_MAP_ENTRY_STATE_EMPTY) {        \
            if ((M).states[ae_unique(i)] == AE_MAP_ENTRY_STATE_USED &&        \
                (M).eq(K, (M).keys[ae_unique(i)])) {                          \
                (M).states[ae_unique(i)] = AE_MAP_ENTRY_STATE_REMOVED;        \
                (M).len--;                                                    \
                break;                                                        \
            }                                                                 \
            if (++ae_unique(i) == (M).cap) {                                  \
                ae_unique(i) = 0;                                             \
            }                                                                 \
        }                                                                     \
    } while (0)

static inline uint64_t ae_hash_string(char *key) {
    uint64_t hash = 14695981039346656037UL;

    for (char *b = key; *b; b++) {
        hash ^= (uint64_t)(uint8_t)*b;
        hash *= 1099511628211UL;
    }

    return hash;
}

static inline bool ae_eq_string(char *a, char *b) {
    return strcmp(a, b) == 0;
}

#endif
