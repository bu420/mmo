#ifndef MMO_ARR_CLIENT_INPUT_H
#define MMO_ARR_CLIENT_INPUT_H

#include <mmo/arr/template.h>
#include <mmo/arr/char.h>

typedef int mmo_client_handle_t;

typedef struct mmo_client_input_s {
  mmo_client_handle_t client;
  mmo_char_arr_t input;
} mmo_client_input_t;

MMO_ARR_DECL(mmo_client_input_t, mmo_client_input)

#endif
