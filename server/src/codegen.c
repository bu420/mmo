#include <mmo/char_arr.h>
#include <mmo/char_arr_arr.h>
#include <mmo/client_handle_arr.h>
#include <mmo/client_arr.h>
#include <mmo/net.h>
#include <mmo/pollfd_arr.h>
#include <mmo/player_arr.h>
#include <mmo/game.h>
#include <mmo/client_input_arr.h>
#include <mmo/char_arr_list.h>
#include <mmo/char_arr_view_list.h>

MMO_ARR_DEF(char, mmo_char_arr)
MMO_ARR_DEF(mmo_char_arr_t, mmo_char_arr_arr)
MMO_ARR_DEF(int, mmo_client_handle_arr)
MMO_ARR_DEF(mmo_client_t, mmo_client_arr)
MMO_ARR_DEF(struct pollfd, mmo_pollfd_arr)
MMO_ARR_DEF(mmo_player_t, mmo_player_arr)
MMO_ARR_DEF(mmo_client_input_t, mmo_client_input_arr)

MMO_LIST_DEF(mmo_char_arr_t, mmo_char_arr_list)
MMO_LIST_DEF(mmo_char_arr_view_t, mmo_char_arr_view_list)
