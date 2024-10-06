#include <mmo/core.h>

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#define MMO_RESULT_MAX_LEN 512

mmo_result_t mmo_result_fmt(mmo_status_t status, const char *fmt, ...) {
    if (!fmt) {
        return (mmo_result_t){ .status = status, .msg = NULL };
    }

    mmo_result_t res;
    res.status = status;
    res.msg = malloc(MMO_RESULT_MAX_LEN);

    if (!res.msg) {
        perror("malloc() failed");
        exit(EXIT_FAILURE);
    }
    
    va_list args;
    va_start(args, fmt);
    vsnprintf(res.msg, MMO_RESULT_MAX_LEN, fmt, args);
    va_end(args);

    return res;
}

void mmo_result_free(mmo_result_t *res) {
    if (res->msg) {
        free(res->msg);
    }
}