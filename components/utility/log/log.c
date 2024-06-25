
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "log.h"
#include <FreeRTOS.h>
#include <task.h>

extern void vprint(const char *fmt, va_list argp);

static struct {
  void *udata;
  int level;
} L;

static const char *LS[] = {"DEBUG", "INFO", "WARN",  "ERROR"};
static const char *LC[] = {"\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m"};

static void stdout_callback(log_Event *ev, ...) {
  printf("[%5u] %s%-5s\x1b[0m \x1b[90m%8s:%3d\x1b[0m ", 
          (xPortIsInsideInterrupt())?(xTaskGetTickCountFromISR()):(xTaskGetTickCount()),
          LC[ev->level], LS[ev->level], ev->file, ev->line);
  vprint(ev->fmt, ev->ap);
  printf("\n\r");
  fflush(ev->udata);
}

static void stdout_callback_hex(log_Event *ev, const char *tag, uint8_t *data, const size_t len) {
  for (size_t i = 0; i < len; i++) {
    if(i%8==0){
      if(i!=0) printf("\n\r"); 
      printf("[%5u] %s%-5s\x1b[0m \x1b[90m%8s:%3d\x1b[0m [%s] ", 
              (xPortIsInsideInterrupt())?(xTaskGetTickCountFromISR()):(xTaskGetTickCount()),
              LC[ev->level], LS[ev->level], ev->file, ev->line, tag);
    }
    printf("%02x ", data[i]);
  }
  printf("\n\r");
  fflush(ev->udata);
}

void log_set_level(int level) { L.level = level; }

void log_dump(int level, const char *file, int line, const char *tag, const void *data, const size_t len) {
  if (level >= L.level) { 
    file = (strrchr(file, '/') ? strrchr(file, '/') + 1 : (strrchr(file, '\\') ? strrchr(file, '\\') + 1 : file));
    log_Event ev = { .file = file, .line = line, .level = level, };
    stdout_callback_hex(&ev, tag, (uint8_t *)data, len); 
  }
}

void log_log(int level, const char *file, int line, const char *fmt, ...) {
  if (level >= L.level) {
    file = (strrchr(file, '/') ? strrchr(file, '/') + 1 : (strrchr(file, '\\') ? strrchr(file, '\\') + 1 : file));
    log_Event ev = { .fmt = fmt, .file = file, .line = line, .level = level, };
    va_start(ev.ap, fmt);
    stdout_callback(&ev);
    va_end(ev.ap);
  }
}