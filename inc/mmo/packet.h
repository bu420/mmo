#ifndef MMO_PACKET_H
#define MMO_PACKET_H

#include <stdbool.h>

#include <mmo/net.h>
#include <mmo/arr/char.h>

bool mmo_has_received_complete_packet(const mmo_char_arr_t *bytes);

int mmo_handle_packet(mmo_char_arr_t *bytes, mmo_client_t *client);

#endif