#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "mcu.h"
#if CONFIG_BUILD_COMPONENT_UART_STDIO
#include "uart_stdio.h"
#endif
static void pin_mux_init(void) {
    int i;
    for (i = 0; i < 32; i++) {
        pin_set_mode(i, MODE_GPIO);
    }
    return;
}

int rt582_main(void) {
    pin_mux_init();
#if CONFIG_BUILD_COMPONENT_UART_STDIO
    uart_stdio_init();
#endif
    puts("Main hello world\r\n");

    extern int main();
    main();

    while (1) {}

    return 0;
}