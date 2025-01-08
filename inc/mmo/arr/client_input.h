#ifndef MMO_ARR_CLIENT_INPUT_H
#define MMO_ARR_CLIENT_INPUT_H

#include <mmo/string.h>
#include <mmo/arr/template.h>

typedef int mmo_client_handle_t;

typedef struct mmo_client_input_s {
    mmo_client_handle_t client;
    mmo_string_t input;
} mmo_client_input_t;

MMO_ARR_DECL(mmo_client_input_t, mmo_client_input_arr);

#endif
