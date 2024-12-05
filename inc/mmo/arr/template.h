#ifndef MMO_ARR_TEMPLATE_H
#define MMO_ARR_TEMPLATE_H

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

/* Generate struct and function declarations for generic resizeable array.
   Put in header. */
#define MMO_ARR_DECL(type, name)                                                                            \
    typedef struct name##_arr_s {                                                                           \
        type *elems;                                                                                        \
        size_t num_elems;                                                                                   \
        size_t max_elems;                                                                                   \
    } name##_arr_t;                                                                                         \
                                                                                                            \
    int name##_arr_new(name##_arr_t *arr, size_t num_elems);                                                \
    void name##_arr_free(name##_arr_t *arr);                                                                \
    int name##_arr_append(name##_arr_t *arr, type elem);                                                    \
    int name##_arr_insert(name##_arr_t *arr, type elem, size_t i);                                          \
    void name##_arr_remove(name##_arr_t *arr, size_t i);                                                    \
    void name##_arr_remove_from_ptr(name##_arr_t *arr, type *elem);                                         \
    void name##_arr_clear(name##_arr_t *arr);

/* Generate function definitions for generic resizeable array.
   Put in source. */
#define MMO_ARR_DEF(type, name)                                                                             \
    int name##_arr_new(name##_arr_t *arr, size_t num_elems) {                                               \
        arr->elems = malloc(num_elems * sizeof(type));                                                      \
                                                                                                            \
        if (!arr->elems) {                                                                                  \
            return -1;                                                                                      \
        }                                                                                                   \
                                                                                                            \
        arr->num_elems = num_elems;                                                                         \
        arr->max_elems = num_elems;                                                                         \
                                                                                                            \
        return 0;                                                                                           \
    }                                                                                                       \
                                                                                                            \
    void name##_arr_free(name##_arr_t *arr) {                                                               \
        free(arr->elems);                                                                                   \
        arr->elems = NULL;                                                                                  \
                                                                                                            \
        arr->num_elems = 0;                                                                                 \
        arr->max_elems = 0;                                                                                 \
    }                                                                                                       \
                                                                                                            \
    int name##_arr_append(name##_arr_t *arr, type elem) {                                                   \
        assert(arr);                                                                                        \
                                                                                                            \
        if (name##_arr_insert(arr, elem, arr->num_elems) == -1) {                                           \
            return -1;                                                                                      \
        }                                                                                                   \
                                                                                                            \
        return 0;                                                                                           \
    }                                                                                                       \
                                                                                                            \
    int name##_arr_insert(name##_arr_t *arr, type elem, size_t i) {                                         \
        assert(arr);                                                                                        \
        assert(i >= 0 && i <= arr->num_elems);                                                              \
                                                                                                            \
        if (arr->num_elems == arr->max_elems) {                                                             \
            if (arr->max_elems == 0) {                                                                      \
                arr->max_elems = 1;                                                                         \
            } else {                                                                                        \
                arr->max_elems *= 2;                                                                        \
            }                                                                                               \
                                                                                                            \
            arr->elems = realloc(arr->elems, arr->max_elems * sizeof(type));                                \
                                                                                                            \
            if (!arr->elems) {                                                                              \
                return -1;                                                                                  \
            }                                                                                               \
        }                                                                                                   \
                                                                                                            \
        /* If new element is not last element, move elements after i one                                    \
           step back. */                                                                                    \
        if (i != arr->num_elems) {                                                                          \
            assert(memmove(arr->elems + i + 1, arr->elems + i, (arr->num_elems - i) * sizeof(type)) == 0);  \
        }                                                                                                   \
                                                                                                            \
        arr->elems[i]   = elem;                                                                             \
        arr->num_elems += 1;                                                                                \
                                                                                                            \
        return 0;                                                                                           \
    }                                                                                                       \
                                                                                                            \
    void name##_arr_remove(name##_arr_t *arr, size_t i) {                                                   \
        assert(arr);                                                                                        \
        assert(arr->num_elems > 0);                                                                         \
        assert(i >= 0 && i < arr->num_elems);                                                               \
                                                                                                            \
        if (i != arr->num_elems - 1) {                                                                      \
            assert(memmove(arr->elems + i, arr->elems + i + 1, (arr->num_elems - 1 - i) * sizeof(type)) ==  \
                   0);                                                                                      \
        }                                                                                                   \
                                                                                                            \
        arr->num_elems -= 1;                                                                                \
    }                                                                                                       \
                                                                                                            \
    void name##_arr_remove_from_ptr(name##_arr_t *arr, type *elem) {                                        \
        assert(arr);                                                                                        \
        assert(elem);                                                                                       \
                                                                                                            \
        ptrdiff_t idx = elem - arr->elems;                                                                  \
                                                                                                            \
        name##_arr_remove(arr, (size_t)idx);                                                                \
    }                                                                                                       \
                                                                                                            \
    void name##_arr_clear(name##_arr_t *arr) {                                                              \
        assert(arr);                                                                                        \
                                                                                                            \
        arr->num_elems = 0;                                                                                 \
    }

#endif