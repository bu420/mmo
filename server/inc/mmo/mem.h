#ifndef MMO_MEM_H
#define MMO_MEM_H

#include <stddef.h>

void *mmo_malloc(size_t num_bytes);
void *mmo_realloc(void *mem, size_t num_bytes);

#endif
