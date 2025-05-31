#ifndef MMO_UI_ELEM_H
#define MMO_UI_ELEM_H

#include <mmo/render.h>

typedef struct mmo_ui_elem_s {
    int x;
    int y;
    int width;
    int height;

    bool in_focus;

    struct {
        struct mmo_ui_elem_s *left;
        struct mmo_ui_elem_s *right;
        struct mmo_ui_elem_s *up;
        struct mmo_ui_elem_s *down;
    } neighbors;

    void (*free)(void *ctx);
    void (*update)(struct mmo_ui_elem_s *elem);
    void (*render)(struct mmo_ui_elem_s *elem, mmo_screen_buf_t *screen_buf);

    void *ctx;
} mmo_ui_elem_t;

#endif
