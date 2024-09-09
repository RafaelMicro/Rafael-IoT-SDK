#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mcu.h"
#include "FreeRTOS.h"
#include "task.h"
#include "hosal_uart.h"
#include "cli.h"
#include "log.h"

int main(void)
{
    cli_init();
    printf("Main hello world");
    while (1) {
        vTaskDelay(200);
    }    
    return 0;
}
