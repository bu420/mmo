#include <mmo/log.h>

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <assert.h>

static FILE *log = NULL;

void mmo_log_init() {
    /* Performance optimization: make stdout fully buffered instead of line buffered. */
    setvbuf(stdout, NULL, _IOFBF, 4096);

    log = fopen("log.txt", "w");

    if (!log) {
        puts("fopen(): failed to open log.txt.\n");
        exit(EXIT_FAILURE);
    }
}

void mmo_log_free() { fclose(log); }

void mmo_log_fmt(const char *level, const char *fmt, ...) {
    assert(level);
    assert(fmt);
    assert(log && "Call mmo_log_init() first!");

    char buf[1024];
    char *pos = buf;

    /* Append date to buffer (UTC). */

    time_t now = time(NULL);
    struct tm tm = *gmtime(&now);
    char date[100];
    strftime(date, 100, "%FT%TZ", &tm);

    pos += snprintf(pos, 256, "[%s] [%s]: ", date, level);

    /* Append arguments supplied by caller to buffer. */

    va_list args;
    va_start(args, fmt);

    pos += snprintf(pos, 766, fmt, args);

    memcpy(pos, "\n\0", 2);
    pos += 2;

    size_t len = (size_t)(pos - buf);

    /* Write buffer to file. */
    fwrite(buf, len, 1, log);

    /* Write buffer to terminal. */
    fwrite(buf, len, 1, stdout);
}
