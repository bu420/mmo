#ifndef MMO_PACKET_H
#define MMO_PACKET_H

#include <stdbool.h>

#include <mmo/arr/char.h>
#include <mmo/net.h>

bool mmo_has_received_complete_packet(const mmo_char_arr_t *bytes);

/* Returns true if packet was ok and false if packet was malformed or sent
   when it shouldn't have been. */
bool mmo_handle_packet(mmo_char_arr_t *bytes, mmo_client_t *client, mmo_server_t *server);

#endif
