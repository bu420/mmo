#ifndef MMO_TELNET_H
#define MMO_TELNET_H

#include <mmo/arr/char.h>
#include <mmo/net.h>

/* Parse receieved bytes. It may contain both plain data aswell as
   telnet commands and snegotiations. */
void mmo_telnet_parse(mmo_char_span_t bytes, mmo_client_t *client,
                      mmo_server_t *server);

#endif
