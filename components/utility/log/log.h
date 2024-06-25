
#ifndef LOG_H
#define LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_DEBUG_LEVEL LOG_LEVEL

typedef struct {
  va_list ap;
  const char *fmt;
  const char *file;
  void *udata;
  int line;
  int level;
} log_Event;

typedef void (*log_LogFn)(log_Event *ev);
enum { U_LOG_DEBUG, U_LOG_INFO, U_LOG_WARN, U_LOG_ERROR, };

#if MAX_DEBUG_LEVEL==0
#define log_debug(...)
// #define log_debug(...) log_log(U_LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...)  log_log(U_LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)  log_log(U_LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(U_LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

#define log_debug_hexdump(...)
#define log_info_hexdump(...)  log_dump(U_LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn_hexdump(...)  log_dump(U_LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error_hexdump(...) log_dump(U_LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

#elif MAX_DEBUG_LEVEL==1
#define log_debug(...)
#define log_info(...)  log_log(U_LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)  log_log(U_LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(U_LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

#define log_debug_hexdump(...)
#define log_info_hexdump(...)  log_dump(U_LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn_hexdump(...)  log_dump(U_LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error_hexdump(...) log_dump(U_LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

#elif MAX_DEBUG_LEVEL==2
#define log_debug(...)
#define log_info(...)
#define log_warn(...)  log_log(U_LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(U_LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

#define log_debug_hexdump(...)
#define log_info_hexdump(...)
#define log_warn_hexdump(...)  log_dump(U_LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error_hexdump(...) log_dump(U_LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

#elif MAX_DEBUG_LEVEL==3
#define log_debug(...)
#define log_info(...)
#define log_warn(...)
#define log_error(...) log_log(U_LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

#define log_debug_hexdump(...)
#define log_info_hexdump(...)
#define log_warn_hexdump(...)
#define log_error_hexdump(...) log_dump(U_LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#endif

void log_set_level(int level);
void log_log(int level, const char *file, int line, const char *fmt, ...);
void log_dump(int level, const char *file, int line, const char *tag, const void *data, const size_t len);

#ifdef __cplusplus
}
#endif

#endif