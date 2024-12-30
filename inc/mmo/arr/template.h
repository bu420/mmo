#ifndef MMO_ARR_TEMPLATE_H
#define MMO_ARR_TEMPLATE_H

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

/* Generate struct and function declarations for generic resizeable array.
   Put in header. */
#define MMO_ARR_DECL(type, name)                                                                   \
    typedef struct name##_s {                                                                      \
        type *elems;                                                                               \
        size_t num_elems;                                                                          \
        size_t max_elems;                                                                          \
    } name##_arr_t;                                                                                \
                                                                                                   \
    typedef struct name##_view_s {                                                                 \
        type *elems;                                                                               \
        size_t num_elems;                                                                          \
    } name##_view_t;                                                                               \
                                                                                                   \
    name##_view_t name##_to_view(name##_t *arr);                                                   \
    void name##_new(name##_t *arr);                                                                \
    void name##_free(name##_t *arr);                                                               \
    int name##_append(name##_t *arr, type elem);                                                   \
    int name##_insert(name##_t *arr, type elem, size_t i);                                         \
    void name##_remove(name##_t *arr, size_t i);                                                   \
    void name##_remove_from_ptr(name##_t *arr, type *elem);                                        \
    /* This is an optimization over free().                                                        \
       Call this to reset element count without freeing up memory. */                              \
    void name##_clear(name##_t *arr);

/* Generate function definitions for generic resizeable array.
   Put in source. */
#define MMO_ARR_DEF(type, name)                                                                    \
    name##_view_t name##_to_view(name##_t *arr) {                                                  \
        assert(arr);                                                                               \
                                                                                                   \
        return (name##_view_t){.elems = arr->elems, .num_elems = arr->num_elems};                  \
    }                                                                                              \
                                                                                                   \
    void name##_new(name##_t *arr) {                                                               \
        assert(arr);                                                                               \
                                                                                                   \
        arr->elems     = NULL;                                                                     \
        arr->num_elems = 0;                                                                        \
        arr->max_elems = 0;                                                                        \
    }                                                                                              \
                                                                                                   \
    void name##_free(name##_t *arr) {                                                              \
        assert(arr);                                                                               \
                                                                                                   \
        free(arr->elems);                                                                          \
        arr->elems = NULL;                                                                         \
                                                                                                   \
        arr->num_elems = 0;                                                                        \
        arr->max_elems = 0;                                                                        \
    }                                                                                              \
                                                                                                   \
    int name##_append(name##_t *arr, type elem) {                                                  \
        assert(arr);                                                                               \
                                                                                                   \
        if (name##_insert(arr, elem, arr->num_elems) == -1) {                                      \
            return -1;                                                                             \
        }                                                                                          \
                                                                                                   \
        return 0;                                                                                  \
    }                                                                                              \
                                                                                                   \
    int name##_insert(name##_t *arr, type elem, size_t i) {                                        \
        assert(arr);                                                                               \
        assert(i <= arr->num_elems);                                                               \
                                                                                                   \
        if (arr->num_elems == arr->max_elems) {                                                    \
            if (arr->max_elems == 0) {                                                             \
                arr->max_elems = 1;                                                                \
            } else {                                                                               \
                arr->max_elems *= 2;                                                               \
            }                                                                                      \
                                                                                                   \
            arr->elems = realloc(arr->elems, arr->max_elems * sizeof(type));                       \
                                                                                                   \
            if (!arr->elems) {                                                                     \
                return -1;                                                                         \
            }                                                                                      \
        }                                                                                          \
                                                                                                   \
        /* If new element is not last element, move elements after i one                           \
           step back. */                                                                           \
        if (i != arr->num_elems) {                                                                 \
            memmove(arr->elems + i + 1, arr->elems + i, (arr->num_elems - i) * sizeof(type));      \
        }                                                                                          \
                                                                                                   \
        arr->elems[i]   = elem;                                                                    \
        arr->num_elems += 1;                                                                       \
                                                                                                   \
        return 0;                                                                                  \
    }                                                                                              \
                                                                                                   \
    void name##_remove(name##_t *arr, size_t i) {                                                  \
        assert(arr);                                                                               \
        assert(i < arr->num_elems);                                                                \
                                                                                                   \
        if (i != arr->num_elems - 1) {                                                             \
            memmove(arr->elems + i, arr->elems + i + 1, (arr->num_elems - 1 - i) * sizeof(type));  \
        }                                                                                          \
                                                                                                   \
        arr->num_elems -= 1;                                                                       \
    }                                                                                              \
                                                                                                   \
    void name##_remove_from_ptr(name##_t *arr, type *elem) {                                       \
        assert(arr);                                                                               \
        assert(elem);                                                                              \
                                                                                                   \
        ptrdiff_t i = elem - arr->elems;                                                           \
                                                                                                   \
        name##_remove(arr, (size_t)i);                                                             \
    }                                                                                              \
                                                                                                   \
    void name##_clear(name##_t *arr) {                                                             \
        assert(arr);                                                                               \
                                                                                                   \
        arr->num_elems = 0;                                                                        \
        arr->max_elems = 0;                                                                        \
    }

#endif
