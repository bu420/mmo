#ifndef MMO_ARR_H
#define MMO_ARR_H

#include <stdlib.h>

/* Generate struct and function declarations for generic resizeable array. 
   Put in header. */
#define MMO_ARR_DECL(type)                                                      \
    typedef struct type ## _arr_s {                                             \
        type *elems;                                                            \
        int num_elems;                                                          \
        int max_elems;                                                          \
    } type ## _arr_t;                                                           \
                                                                                \
    int type ## _arr_new(type ## _arr_t *arr, int num_elems);                   \
    void type ## _arr_free(type ## _arr_t *arr);                                \
    int type ## _arr_append(type ## _arr_t *arr, type elem);                    \
    int type ## _arr_insert(type ## _arr_t *arr, type elem, int i);             \
    void type ## _arr_remove(type ## _arr_t *arr, int i);

/* Generate function definitions for generic resizeable array. 
   Put in source. */
#define MMO_ARR_DEF(type)                                                       \
    int type ## _arr_new(type ## _arr_t *arr, int num_elems) {                  \
        arr->elems = malloc(num_elems * sizeof(type));                          \
                                                                                \
        if (!arr->elems) {                                                      \
            return -1;                                                          \
        }                                                                       \
                                                                                \
        arr->num_elems = num_elems;                                             \
        arr->max_elems = num_elems;                                             \
                                                                                \
        return 0;                                                               \
    }                                                                           \
                                                                                \
    void type ## _arr_free(type ## _arr_t *arr) {                               \
        free(arr->elems);                                                       \
        arr->elems = NULL;                                                      \
                                                                                \
        arr->num_elems = 0;                                                     \
        arr->max_elems = 0;                                                     \
    }                                                                           \
                                                                                \
    int type ## _arr_append(type ## _arr_t *arr, type elem) {                   \
        if (arr->num_elems == arr->max_elems) {                                 \
            if (arr->max_elems == 0) {                                          \
                arr->max_elems = 1;                                             \
            }                                                                   \
            else {                                                              \
                arr->max_elems *= 2;                                            \
            }                                                                   \
                                                                                \
            arr->elems = realloc(arr->elems, arr->max_elems);                   \
                                                                                \
            if (!arr->elems) {                                                  \
                return -1;                                                      \
            }                                                                   \
        }                                                                       \
                                                                                \
        arr->elems[arr->num_elems] = elem;                                      \
        arr->num_elems += 1;                                                    \
                                                                                \
        return 0;                                                               \
    }                                                                           \
                                                                                \
    int type ## _arr_insert(type ## _arr_t *arr, type elem, int i) {            \
        return -1;                                                              \
    }                                                                           \
                                                                                \
    void type ## _arr_remove(type ## _arr_t *arr, int i) {                      \
                                                                                \
    }

#endif