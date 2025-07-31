#ifndef AE_LOG_H
#define AE_LOG_H

#define AE_LOG_INFO    "info"
#define AE_LOG_WARNING "\x1b[38;2;255;204;0mwarn\x1b[0m"
#define AE_LOG_ERROR   "\x1b[48;2;204;51;0merror\x1b[0m"
#define AE_LOG_DEBUG   "\x1b[38;2;0;255;255mdebug\x1b[0m"

void ae_log(const char *level, const char *msg);
void ae_log_fmt(const char *level, const char *fmt, ...);

#endif
