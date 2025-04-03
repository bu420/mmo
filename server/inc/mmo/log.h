#ifndef MMO_LOG_H
#define MMO_LOG_H

#define MMO_LOG_INFO  "INFO"
#define MMO_LOG_WARN  "\x1b[38;2;255;204;0mWARN\x1b[0m"
#define MMO_LOG_ERROR "\x1b[48;2;204;51;0mERROR\x1b[0m"

void mmo_log_init();
void mmo_log_free();

void mmo_log_fmt(const char *level, const char *fmt, ...);

#endif
