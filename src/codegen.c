#include <mmo/arr/template.h>
#include <mmo/arr/char.h>
#include <mmo/arr/char_arr.h>
#include <mmo/arr/client.h>
#include <mmo/net.h>
#include <mmo/arr/pollfd.h>
#include <mmo/arr/player.h>
#include <mmo/game.h>
#include <mmo/arr/bool.h>

#include <mmo/list/template.h>
#include <mmo/list/char_arr.h>
#include <mmo/list/char_span.h>

MMO_ARR_DEF(char, mmo_char)
MMO_ARR_DEF(mmo_char_arr_t, mmo_char_arr)
MMO_ARR_DEF(mmo_client_t, mmo_client)
MMO_ARR_DEF(struct pollfd, mmo_pollfd)
MMO_ARR_DEF(mmo_player_t, mmo_player)
MMO_ARR_DEF(bool, mmo_bool)

MMO_LIST_DEF(mmo_char_arr_t, mmo_char_arr)
MMO_LIST_DEF(mmo_char_span_t, mmo_char_span)
