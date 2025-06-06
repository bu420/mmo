#include <mmo/arr/template.h>
#include <mmo/arr/char.h>
#include <mmo/arr/char_arr.h>
#include <mmo/arr/client_handle.h>
#include <mmo/arr/client.h>
#include <mmo/net.h>
#include <mmo/arr/pollfd.h>
#include <mmo/arr/player.h>
#include <mmo/game.h>
#include <mmo/arr/client_input.h>
#include <mmo/render.h>
#include <mmo/arr/cell.h>
#include <mmo/arr/bool.h>
#include <mmo/arr/telopt.h>
#include <mmo/arr/bmp_pixel.h>

#include <mmo/list/template.h>
#include <mmo/list/char_arr.h>
#include <mmo/list/char_span.h>
#include <mmo/list/player_state.h>

MMO_ARR_DEF(char, mmo_char)
MMO_ARR_DEF(mmo_char_arr_t, mmo_char_arr)
MMO_ARR_DEF(int, mmo_client_handle)
MMO_ARR_DEF(mmo_client_t, mmo_client)
MMO_ARR_DEF(struct pollfd, mmo_pollfd)
MMO_ARR_DEF(mmo_player_t, mmo_player)
MMO_ARR_DEF(mmo_client_input_t, mmo_client_input)
MMO_ARR_DEF(mmo_cell_t, mmo_cell)
MMO_ARR_DEF(bool, mmo_bool)
MMO_ARR_DEF(mmo_telopt_t, mmo_telopt)
MMO_ARR_DEF(mmo_bmp_pixel_t, mmo_bmp_pixel)

MMO_LIST_DEF(mmo_char_arr_t, mmo_char_arr)
MMO_LIST_DEF(mmo_char_span_t, mmo_char_span)
MMO_LIST_DEF(mmo_player_state_t, mmo_player_state)
