#ifndef MMO_IO_H
#define MMO_IO_H

#include <stdint.h>
#include <stdbool.h>

#include <mmo/arr/char.h>

#define MMO_USERNAME_MAX 16
#define MMO_USERNAME_MIN 2

/* Consume and sanitize a line of input.
   Returns false if a complete line does not exist. */
bool mmo_get_line(mmo_char_arr_t *line, mmo_char_arr_t *in);

bool mmo_str_eq_ignore_case(mmo_char_span_t a, mmo_char_span_t b);

typedef struct mmo_player_s mmo_player_t;
typedef struct mmo_server_s mmo_server_t;

void mmo_prompt(mmo_player_t *player, mmo_server_t *server);

typedef enum mmo_print_type_e {
    MMO_PRINT_AFTER_ENTER,
    MMO_PRINT_INTERRUPT
} mmo_print_type_t;

void mmo_print_fmt(mmo_player_t *player, mmo_server_t *server,
                   mmo_print_type_t action, const char *fmt, ...);

#endif
