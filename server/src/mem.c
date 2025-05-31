#include <mmo/mem.h>

#include <stdlib.h>
#include <stdio.h>

#include <mmo/log.h>

void *mmo_malloc(size_t num_bytes) {
    void *mem = malloc(num_bytes);

    if (!mem) {
        mmo_log_fmt(MMO_LOG_ERROR,
                    "malloc(): heap memory allocation failure. Out of memory. "
                    "Requested block "
                    "size: %zu bytes. Possible memory leak?",
                    num_bytes);
        exit(EXIT_FAILURE);
    }

    return mem;
}

void *mmo_realloc(void *mem, size_t num_bytes) {
    void *new_mem = realloc(mem, num_bytes);

    if (!new_mem) {
        mmo_log_fmt(MMO_LOG_ERROR,
                    "realloc(): heap memory allocation failure. Out of memory. "
                    "Requested block "
                    "size: %zu bytes. Possible memory leak?",
                    num_bytes);
        exit(EXIT_FAILURE);
    }

    return new_mem;
}
