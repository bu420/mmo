#ifndef AE_IO_H
#define AE_IO_H

#include <stdint.h>
#include <stdbool.h>

#include <ae/app.h>
#include <ae/meta.h>

/* Consume and sanitize a line of input.
   Return false if a complete line does not exist. */
bool ae_get_line(ae_string_t *line, ae_string_t *in);

void ae_prompt(const ae_user_t *user, ae_app_t *app);

typedef enum ae_print_type_e {
    AE_PRINT_AFTER_ENTER,
    AE_PRINT_INTERRUPT
} ae_print_type_t;

void ae_print_fmt(const ae_user_t *user, ae_app_t *app, ae_print_type_t action,
                  const char *fmt, ...);

#endif
