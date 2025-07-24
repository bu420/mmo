#include <ae/log.h>

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

void ae_log(const char *level, const char *msg) {
    static FILE *log = NULL;

    if (!log) {
        log = fopen("log.txt", "w");

        if (!log) {
            puts("fopen(): failed to open log.txt.\n");
            exit(EXIT_FAILURE);
        }
    }

    time_t now   = time(NULL);
    struct tm tm = *gmtime(&now);
    char date[100];
    strftime(date, 100, "%F %T", &tm);

    char buf[1024];
    size_t len = (size_t)snprintf(buf, sizeof buf, "[%s %s]: %s\r\n", date, level, msg);

    /* Write buffer to file. */
    fwrite(buf, len, 1, log);
    fflush(log);

    /* Write buffer to terminal. */
    fwrite(buf, len, 1, stdout);
}

void ae_log_fmt(const char *level, const char *fmt, ...) {
    char buf[924];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof buf, fmt, args);
    va_end(args);

    ae_log(level, buf);
}
