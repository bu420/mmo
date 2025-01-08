#ifndef MMO_LIST_H
#define MMO_LIST_H

#include <stdlib.h>
#include <stddef.h>
#include <assert.h>

/* Generate struct and function declarations for generic doubly linked list.
   Put in header. */
#define MMO_LIST_DECL(type, name)                                                                  \
    typedef struct name##_node_s {                                                                 \
        struct name##_node_s *next;                                                                \
        struct name##_node_s *prev;                                                                \
        type data;                                                                                 \
    } name##_node_t;                                                                               \
                                                                                                   \
    typedef struct name##_s {                                                                      \
        name##_node_t *head;                                                                       \
        name##_node_t *tail;                                                                       \
        size_t num_elems;                                                                          \
    } name##_t;                                                                                    \
                                                                                                   \
    void name##_new(name##_t *list);                                                               \
    void name##_free(name##_t *list);                                                              \
    name##_node_t *name##_at(name##_t *list, size_t i);                                            \
    int name##_append(name##_t *list, type elem);                                                  \
    int name##_insert(name##_t *list, type elem, size_t i);                                        \
    void name##_remove(name##_t *list, size_t i);

/* Generate function definitions for generic doubly linked list.
   Put in source. */
#define MMO_LIST_DEF(type, name)                                                                   \
    void name##_new(name##_t *list) {                                                              \
        assert(list);                                                                              \
                                                                                                   \
        list->head      = NULL;                                                                    \
        list->tail      = NULL;                                                                    \
        list->num_elems = 0;                                                                       \
    }                                                                                              \
                                                                                                   \
    void name##_free(name##_t *list) {                                                             \
        assert(list);                                                                              \
                                                                                                   \
        while (list->head) {                                                                       \
            name##_node_t *temp = list->head;                                                      \
                                                                                                   \
            list->head = list->head->next;                                                         \
                                                                                                   \
            free(temp);                                                                            \
            temp = NULL;                                                                           \
        }                                                                                          \
                                                                                                   \
        list->head      = NULL;                                                                    \
        list->tail      = NULL;                                                                    \
        list->num_elems = 0;                                                                       \
    }                                                                                              \
                                                                                                   \
    name##_node_t *name##_at(name##_t *list, size_t i) {                                           \
        assert(list);                                                                              \
        assert(i < list->num_elems);                                                               \
                                                                                                   \
        bool traverse_from_start = i < list->num_elems / 2;                                        \
                                                                                                   \
        size_t num_steps    = traverse_from_start ? i : list->num_elems - 1 - i;                   \
        name##_node_t *node = traverse_from_start ? list->head : list->tail;                       \
                                                                                                   \
        while (num_steps > 0) {                                                                    \
            node       = traverse_from_start ? node->next : node->prev;                            \
            num_steps -= 1;                                                                        \
        }                                                                                          \
                                                                                                   \
        return node;                                                                               \
    }                                                                                              \
                                                                                                   \
    int name##_append(name##_t *list, type elem) {                                                 \
        assert(list);                                                                              \
                                                                                                   \
        if (name##_insert(list, elem, list->num_elems) == -1) {                                    \
            return -1;                                                                             \
        }                                                                                          \
                                                                                                   \
        return 0;                                                                                  \
    }                                                                                              \
                                                                                                   \
    int name##_insert(name##_t *list, type elem, size_t i) {                                       \
        assert(list);                                                                              \
        assert(i <= list->num_elems);                                                              \
                                                                                                   \
        name##_node_t *node = malloc(sizeof(name##_node_t));                                       \
                                                                                                   \
        if (!node) {                                                                               \
            return -1;                                                                             \
        }                                                                                          \
                                                                                                   \
        node->data = elem;                                                                         \
        node->prev = NULL;                                                                         \
        node->next = NULL;                                                                         \
                                                                                                   \
        /* Inserting at head (or if list is empty: head and tail). */                              \
        if (i == 0) {                                                                              \
            node->next = list->head;                                                               \
                                                                                                   \
            if (list->num_elems > 0) {                                                             \
                list->head->prev = node;                                                           \
            } else {                                                                               \
                list->tail = node;                                                                 \
            }                                                                                      \
                                                                                                   \
            list->head = node;                                                                     \
        } /* Inserting at tail. */                                                                 \
        else if (i == list->num_elems) {                                                           \
            node->prev       = list->tail;                                                         \
            list->tail->next = node;                                                               \
            list->tail       = node;                                                               \
        } else {                                                                                   \
            name##_node_t *prev = name##_at(list, i - 1);                                          \
            name##_node_t *next = prev->next;                                                      \
                                                                                                   \
            prev->next = node;                                                                     \
            node->prev = prev;                                                                     \
                                                                                                   \
            if (next) {                                                                            \
                node->next = next;                                                                 \
                next->prev = node;                                                                 \
            }                                                                                      \
        }                                                                                          \
                                                                                                   \
        list->num_elems += 1;                                                                      \
                                                                                                   \
        return 0;                                                                                  \
    }                                                                                              \
                                                                                                   \
    void name##_remove(name##_t *list, size_t i) {                                                 \
        assert(list);                                                                              \
        assert(i < list->num_elems);                                                               \
                                                                                                   \
        name##_node_t *node = name##_at(list, i);                                                  \
        name##_node_t *prev = node->prev;                                                          \
        name##_node_t *next = node->next;                                                          \
                                                                                                   \
        if (node == list->head) {                                                                  \
            list->head = next;                                                                     \
                                                                                                   \
            if (list->head) {                                                                      \
                list->head->prev = NULL;                                                           \
            } else {                                                                               \
                list->tail = NULL;                                                                 \
            }                                                                                      \
        } else if (node == list->tail) {                                                           \
            list->tail       = prev;                                                               \
            list->tail->next = NULL;                                                               \
        } else {                                                                                   \
            prev->next = next;                                                                     \
            next->prev = prev;                                                                     \
        }                                                                                          \
                                                                                                   \
        free(node);                                                                                \
        node = NULL;                                                                               \
                                                                                                   \
        list->num_elems -= 1;                                                                      \
    }
#endif
