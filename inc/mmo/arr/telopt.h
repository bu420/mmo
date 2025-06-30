#ifndef MMO_ARR_TELOPT_H
#define MMO_ARR_TELOPT_H

#include <stdint.h>

#include <mmo/arr/template.h>

typedef struct mmo_telopt_s {
    bool done;
    uint8_t cmd;
    uint8_t opt;
} mmo_telopt_t;

MMO_ARR_DECL(mmo_telopt_t, mmo_telopt)

#endif
