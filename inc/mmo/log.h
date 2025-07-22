#ifndef MMO_LOG_H
#define MMO_LOG_H

#define MMO_LOG_INFO    "info"
#define MMO_LOG_WARNING "\x1b[38;2;255;204;0mwarning\x1b[0m"
#define MMO_LOG_ERROR   "\x1b[48;2;204;51;0merror\x1b[0m"
#define MMO_LOG_DEBUG   "\x1b[38;2;0;255;255mdebug\x1b[0m"

void mmo_log(const char *level, const char *msg);
void mmo_log_fmt(const char *level, const char *fmt, ...);

#endif
