#ifndef MMO_POLLFD_ARR_H
#define MMO_POLLFD_ARR_H

#include <sys/poll.h>

#include <mmo/arr.h>

MMO_ARR_DECL(struct pollfd, mmo_pollfd_arr)

#endif
