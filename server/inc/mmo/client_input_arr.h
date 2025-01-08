#ifndef MMO_CLIENT_INPUT_ARR_H
#define MMO_CLIENT_INPUT_ARR_H

#include <mmo/arr.h>
#include <mmo/char_arr.h>

typedef int mmo_client_handle_t;

typedef struct mmo_client_input_s {
    mmo_client_handle_t client;
    mmo_char_arr_t input;
} mmo_client_input_t;

MMO_ARR_DECL(mmo_client_input_t, mmo_client_input_arr)

#endif
