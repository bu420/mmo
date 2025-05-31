#ifndef MMO_LIST_TEMPLATE_H
#define MMO_LIST_TEMPLATE_H

#include <stdlib.h>
#include <stddef.h>
#include <assert.h>

#include <mmo/mem.h>

/* Generate struct and function declarations for generic doubly linked list.
   Put in header. */
#define MMO_LIST_DECL(type, name)                                              \
    typedef struct name##_list_node_s {                                        \
        struct name##_list_node_s *next;                                       \
        struct name##_list_node_s *prev;                                       \
        type data;                                                             \
    } name##_list_node_t;                                                      \
                                                                               \
    typedef struct name##_list_s {                                             \
        name##_list_node_t *head;                                              \
        name##_list_node_t *tail;                                              \
        size_t num_elems;                                                      \
    } name##_list_t;                                                           \
                                                                               \
    void name##_list_new(name##_list_t *list);                                 \
    void name##_list_free(name##_list_t *list);                                \
    name##_list_node_t *name##_list_at(name##_list_t *list, size_t i);         \
    void name##_list_append(name##_list_t *list, type *elem);                  \
    void name##_list_insert(name##_list_t *list, type *elem, size_t i);        \
    void name##_list_insert_after_node(name##_list_t *list, type *elem,        \
                                       name##_list_node_t *node);              \
    void name##_list_remove(name##_list_t *list, size_t i);

/* Generate function definitions for generic doubly linked list.
   Put in source. */
#define MMO_LIST_DEF(type, name)                                               \
    void name##_list_new(name##_list_t *list) {                                \
        assert(list);                                                          \
                                                                               \
        list->head      = NULL;                                                \
        list->tail      = NULL;                                                \
        list->num_elems = 0;                                                   \
    }                                                                          \
                                                                               \
    void name##_list_free(name##_list_t *list) {                               \
        assert(list);                                                          \
                                                                               \
        while (list->head) {                                                   \
            name##_list_node_t *temp = list->head;                             \
                                                                               \
            list->head = list->head->next;                                     \
                                                                               \
            free(temp);                                                        \
            temp = NULL;                                                       \
        }                                                                      \
                                                                               \
        list->head      = NULL;                                                \
        list->tail      = NULL;                                                \
        list->num_elems = 0;                                                   \
    }                                                                          \
                                                                               \
    name##_list_node_t *name##_list_at(name##_list_t *list, size_t i) {        \
        assert(list);                                                          \
        assert(i < list->num_elems);                                           \
                                                                               \
        bool traverse_from_start = i < list->num_elems / 2;                    \
                                                                               \
        size_t num_steps = traverse_from_start ? i : list->num_elems - 1 - i;  \
        name##_list_node_t *node =                                             \
            traverse_from_start ? list->head : list->tail;                     \
                                                                               \
        while (num_steps > 0) {                                                \
            node = traverse_from_start ? node->next : node->prev;              \
            num_steps -= 1;                                                    \
        }                                                                      \
                                                                               \
        return node;                                                           \
    }                                                                          \
                                                                               \
    void name##_list_append(name##_list_t *list, type *elem) {                 \
        assert(list);                                                          \
        assert(elem);                                                          \
                                                                               \
        name##_list_insert(list, elem, list->num_elems);                       \
    }                                                                          \
                                                                               \
    void name##_list_insert(name##_list_t *list, type *elem, size_t i) {       \
        assert(list);                                                          \
        assert(elem);                                                          \
        assert(i <= list->num_elems);                                          \
                                                                               \
        name##_list_node_t *node = mmo_malloc(sizeof(name##_list_node_t));     \
                                                                               \
        node->data = *elem;                                                    \
        node->prev = NULL;                                                     \
        node->next = NULL;                                                     \
                                                                               \
        /* Inserting at head (or if list is empty: head and tail). */          \
        if (i == 0) {                                                          \
            node->next = list->head;                                           \
                                                                               \
            if (list->num_elems > 0) {                                         \
                list->head->prev = node;                                       \
            } else {                                                           \
                list->tail = node;                                             \
            }                                                                  \
                                                                               \
            list->head = node;                                                 \
        } /* Inserting at tail. */                                             \
        else if (i == list->num_elems) {                                       \
            node->prev       = list->tail;                                     \
            list->tail->next = node;                                           \
            list->tail       = node;                                           \
        } else {                                                               \
            name##_list_node_t *prev = name##_list_at(list, i - 1);            \
            name##_list_node_t *next = prev->next;                             \
                                                                               \
            prev->next = node;                                                 \
            node->prev = prev;                                                 \
                                                                               \
            if (next) {                                                        \
                node->next = next;                                             \
                next->prev = node;                                             \
            }                                                                  \
        }                                                                      \
                                                                               \
        list->num_elems += 1;                                                  \
    }                                                                          \
                                                                               \
    void name##_list_insert_after_node(name##_list_t *list, type *elem,        \
                                       name##_list_node_t *node) {             \
        assert(list);                                                          \
        assert(elem);                                                          \
        assert(node);                                                          \
                                                                               \
        name##_list_node_t *new_node = mmo_malloc(sizeof(name##_list_node_t)); \
                                                                               \
        new_node->data = *elem;                                                \
        new_node->prev = node;                                                 \
        new_node->next = node->next;                                           \
                                                                               \
        node->next->prev = new_node;                                           \
        node->next       = new_node;                                           \
                                                                               \
        list->num_elems += 1;                                                  \
    }                                                                          \
                                                                               \
    void name##_list_remove(name##_list_t *list, size_t i) {                   \
        assert(list);                                                          \
        assert(i < list->num_elems);                                           \
                                                                               \
        name##_list_node_t *node = name##_list_at(list, i);                    \
        name##_list_node_t *prev = node->prev;                                 \
        name##_list_node_t *next = node->next;                                 \
                                                                               \
        if (node == list->head) {                                              \
            list->head = next;                                                 \
                                                                               \
            if (list->head) {                                                  \
                list->head->prev = NULL;                                       \
            } else {                                                           \
                list->tail = NULL;                                             \
            }                                                                  \
        } else if (node == list->tail) {                                       \
            list->tail       = prev;                                           \
            list->tail->next = NULL;                                           \
        } else {                                                               \
            prev->next = next;                                                 \
            next->prev = prev;                                                 \
        }                                                                      \
                                                                               \
        free(node);                                                            \
        node = NULL;                                                           \
                                                                               \
        list->num_elems -= 1;                                                  \
    }
#endif
