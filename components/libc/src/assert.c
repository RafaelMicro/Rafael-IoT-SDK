/**
 * @file assert.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <assert.h>
#include <stdio.h>
#if !defined(CONFIG_BOOTLOADER)
#include <FreeRTOS.h>
#include <task.h>
#endif
/* for exit() and abort() */
void __attribute__((noreturn))
__assert(const char* file, int line, const char* cond) {
    printf("Assertion Failed: %s, in %s:%d\r\n", cond, file, line);
    while (1)
        ;
}

void __attribute__((noreturn))
__assert_func(const char* file, int line, const char* func, const char* cond) {
    printf("Assertion Failed: %s, in %s %s:%d\r\n", cond, func, file, line);
    while (1)
        ;
}

void __attribute__((noreturn)) abort(void) {
    while (1)
        ;
}
