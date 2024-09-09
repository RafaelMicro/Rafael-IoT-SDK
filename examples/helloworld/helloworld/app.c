#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mcu.h"
#include "FreeRTOS.h"
#include "task.h"
#include "hosal_uart.h"
#include "cli.h"

int app(void)
{
    cli_init();
    printf("APP Hello World\r\n");
    while (1) {
        vTaskDelay(200);
    }    
    return 0;
}
