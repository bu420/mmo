#include <mmo/arr/template.h>
#include <mmo/arr/char.h>
#include <mmo/arr/client.h>
#include <mmo/net.h>
#include <mmo/arr/pollfd.h>

MMO_ARR_DEF(char, mmo_char);
MMO_ARR_DEF(mmo_client_t, mmo_client);
MMO_ARR_DEF(struct pollfd, mmo_pollfd);