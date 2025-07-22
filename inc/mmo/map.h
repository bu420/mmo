#ifndef MMO_MAP_H
#define MMO_MAP_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <mmo/meta.h>

#define MMO_MAP_CAP 16

#define mmo_map(K, V)                                                          \
    struct {                                                                   \
        K *keys;                                                               \
        K *old_keys;                                                           \
        V *vals;                                                               \
        V *old_vals;                                                           \
        bool *in_use;                                                          \
                                                                               \
        size_t len;                                                            \
        size_t cap;                                                            \
                                                                               \
        uint64_t (*hash)(K);                                                   \
        bool (*eq)(K, K);                                                      \
    }

typedef mmo_map(char *, int) mmo_string_to_int_map_t;
typedef mmo_map(char *, float) mmo_string_to_float_map_t;
typedef mmo_map(char *, char *) mmo_string_to_string_map_t;

#define mmo_map_new(M, HASH, EQ)                                               \
    do {                                                                       \
        mmo_generic_assign(M.keys, calloc(MMO_MAP_CAP, sizeof(M.keys[0])));    \
        mmo_generic_assign(M.vals, calloc(MMO_MAP_CAP, sizeof(M.vals[0])));    \
        mmo_generic_assign(M.in_use, calloc(MMO_MAP_CAP, sizeof(bool)));       \
                                                                               \
        M.len = 0;                                                             \
        M.cap = MMO_MAP_CAP;                                                   \
                                                                               \
        M.hash = HASH;                                                         \
        M.eq   = EQ;                                                           \
    } while (0)

#define mmo_map_free(M)                                                        \
    do {                                                                       \
        free(M.keys);                                                          \
        free(M.vals);                                                          \
        free(M.in_use);                                                        \
    } while (0)

#define mmo_map_get(M, K, V)                                                   \
    do {                                                                       \
        V                    = NULL;                                           \
        size_t mmo_unique(i) = M.hash(K) % M.cap;                              \
                                                                               \
        while (M.in_use[mmo_unique(i)]) {                                      \
            if (M.eq(K, M.keys[mmo_unique(i)])) {                              \
                V = &M.vals[mmo_unique(i)];                                    \
                break;                                                         \
            }                                                                  \
            if (++mmo_unique(i) == M.cap) {                                    \
                mmo_unique(i) = 0;                                             \
            }                                                                  \
        }                                                                      \
    } while (0)

#define _mmo_map_set(M, K, V)                                                  \
    do {                                                                       \
        size_t mmo_unique(i) = M.hash(K) % M.cap;                              \
        bool mmo_unique(set) = false;                                          \
                                                                               \
        while (M.in_use[mmo_unique(i)]) {                                      \
            if (M.eq(K, M.keys[mmo_unique(i)])) {                              \
                M.vals[mmo_unique(i)] = V;                                     \
                mmo_unique(set)       = true;                                  \
                break;                                                         \
            }                                                                  \
            if (++mmo_unique(i) == M.cap) {                                    \
                mmo_unique(i) = 0;                                             \
            }                                                                  \
        }                                                                      \
                                                                               \
        if (!mmo_unique(set)) {                                                \
            M.keys[mmo_unique(i)]   = K;                                       \
            M.vals[mmo_unique(i)]   = V;                                       \
            M.in_use[mmo_unique(i)] = true;                                    \
            M.len++;                                                           \
        }                                                                      \
    } while (0)

#define mmo_map_set(M, K, V)                                                   \
    do {                                                                       \
        if (M.len == M.cap / 2) {                                              \
            M.old_keys                   = M.keys;                             \
            M.old_vals                   = M.vals;                             \
            bool *mmo_unique(old_in_use) = M.in_use;                           \
            size_t mmo_unique(old_cap)   = M.cap;                              \
                                                                               \
            M.len = 0;                                                         \
            M.cap *= 2;                                                        \
                                                                               \
            mmo_generic_assign(M.keys, calloc(M.cap, sizeof(M.keys[0])));      \
            mmo_generic_assign(M.vals, calloc(M.cap, sizeof(M.vals[0])));      \
            mmo_generic_assign(M.in_use, calloc(M.cap, sizeof(bool)));         \
                                                                               \
            for (size_t mmo_unique(j) = 0;                                     \
                 mmo_unique(j) < mmo_unique(old_cap); mmo_unique(j)++) {       \
                if (mmo_unique(old_in_use)[mmo_unique(j)]) {                   \
                    _mmo_map_set(M, M.old_keys[mmo_unique(j)],                 \
                                 M.old_vals[mmo_unique(j)]);                   \
                }                                                              \
            }                                                                  \
                                                                               \
            free(M.old_keys);                                                  \
            free(M.old_vals);                                                  \
            free(mmo_unique(old_in_use));                                      \
        }                                                                      \
                                                                               \
        _mmo_map_set(M, K, V);                                                 \
    } while (0)

#endif
