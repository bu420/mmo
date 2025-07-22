#ifndef MMO_IO_H
#define MMO_IO_H

#include <stdint.h>
#include <stdbool.h>

#include <mmo/app.h>
#include <mmo/arr.h>

/* Consume and sanitize a line of input.
   Return false if a complete line does not exist. */
bool mmo_get_line(mmo_byte_arr_t line, mmo_byte_arr_t in);

bool mmo_str_eq_ignore_case(mmo_byte_arr_t a, mmo_byte_arr_t b);

void mmo_prompt(mmo_user_t *user, mmo_app_t *app);

typedef enum mmo_print_type_e {
    MMO_PRINT_AFTER_ENTER,
    MMO_PRINT_INTERRUPT
} mmo_print_type_t;

void mmo_print_fmt(mmo_user_t *user, mmo_app_t *app, mmo_print_type_t action,
                   const char *fmt, ...);

#endif
