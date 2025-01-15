#ifndef MMO_PACKET_H
#define MMO_PACKET_H

#include <stdbool.h>

#include <mmo/char_arr.h>
#include <mmo/net.h>

bool mmo_has_received_complete_packet(mmo_char_arr_view_t bytes);

/* Returns true if packet was ok and false if packet was malformed or sent
   when it shouldn't have been. */
bool mmo_handle_packet(mmo_char_arr_t *bytes, mmo_client_t *client);

#endif
