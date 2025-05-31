#ifndef MMO_UI_INPUT_H
#define MMO_UI_INPUT_H

#include <mmo/arr/char.h>
#include <mmo/ui/elem.h>

typedef struct mmo_ui_input_s {
    mmo_char_arr_t buf;
} mmo_ui_input_t;

void mmo_ui_input_new(mmo_ui_input_t *input);
void mmo_ui_input_free(void *ctx);
void mmo_ui_input_update(mmo_ui_elem_t *elem);
void mmo_ui_input_render(mmo_ui_elem_t *elem, mmo_screen_buf_t *screen_buf);

#endif
