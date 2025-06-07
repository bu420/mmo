#ifndef MMO_ARR_BMP_PIXEL_H
#define MMO_ARR_BMP_PIXEL_H

#include <stdint.h>

#include <mmo/arr/template.h>

typedef struct mmo_bmp_pixel_s {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    bool transparent;
} mmo_bmp_pixel_t;

MMO_ARR_DECL(mmo_bmp_pixel_t, mmo_bmp_pixel)

#endif
