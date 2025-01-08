#include <mmo/arr/template.h>
#include <mmo/arr/char.h>
#include <mmo/arr/client_handle.h>
#include <mmo/arr/client.h>
#include <mmo/net.h>
#include <mmo/arr/pollfd.h>
#include <mmo/game.h>
#include <mmo/arr/client_input.h>

MMO_ARR_DEF(char, mmo_char_arr);
MMO_ARR_DEF(int, mmo_client_handle_arr);
MMO_ARR_DEF(mmo_client_t, mmo_client_arr);
MMO_ARR_DEF(struct pollfd, mmo_pollfd_arr);
MMO_ARR_DEF(mmo_player_t, mmo_player_arr);
MMO_ARR_DEF(mmo_client_input_t, mmo_client_input_arr);
