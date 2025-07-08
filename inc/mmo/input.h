#ifndef MMO_INPUT_H
#define MMO_INPUT_H

#include <stdint.h>
#include <stdbool.h>

#include <mmo/arr/char.h>

#define MMO_USERNAME_MAX 16
#define MMO_USERNAME_MIN 2

/* Consume and sanitize a line of input.
   Returns false if a complete line does not exist. */
bool mmo_get_line(mmo_char_arr_t *line, mmo_char_arr_t *in);

#endif
