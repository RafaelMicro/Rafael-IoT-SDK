#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mcu.h"

#define APP_XADDR 0x8000

typedef void (*pfunction)(void);

static pfunction app_start;

int main(void) {

    puts("Bootloader app hello world\r\n");

    __disable_irq();

    app_start = (pfunction) * (__IO uint32_t*)(APP_XADDR + 4);

    // SCB->VTOR = (uint32_t)APP_XADDR;
    __set_MSP(*(__IO uint32_t*)APP_XADDR);

    app_start();

    while (1) {}
    return 0;
}
