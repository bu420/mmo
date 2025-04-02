#include <mmo/mem.h>

#include <stdlib.h>
#include <stdio.h>

void *mmo_malloc(size_t num_bytes) {
    void *mem = malloc(num_bytes);

    if (!mem) {
        printf("malloc(): Heap memory allocation failure. Out of memory.\n");
        exit(EXIT_FAILURE);
    }

    return mem;
}

void *mmo_realloc(void *mem, size_t num_bytes) {
    void *new_mem = realloc(mem, num_bytes);

    if (!new_mem) {
        printf("realloc(): Heap memory allocation failure. Out of memory.\n");
        exit(EXIT_FAILURE);
    }

    return new_mem;
}