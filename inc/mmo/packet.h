#ifndef MMO_PACKET_H
#define MMO_PACKET_H

#include <stdbool.h>

#include <mmo/arr/char.h>

bool mmo_has_received_complete_packet(const mmo_char_arr_t *bytes);

#endif