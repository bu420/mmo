#ifndef MMO_CORE_H
#define MMO_CORE_H

#include <string.h>
#include <errno.h>

/* Beware! Not thread safe. */
#define MMO_ERRNO strerror(errno)

typedef enum mmo_status_e {
    MMO_OK,
    MMO_ERR,
    MMO_ACCEPT_ERR
} mmo_status_t;

typedef struct mmo_result_s {
    mmo_status_t status;
    char *msg;
} mmo_result_t;

mmo_result_t mmo_result_new(mmo_status_t status, const char *fmt, ...);
void mmo_result_free(mmo_result_t *result);

#endif