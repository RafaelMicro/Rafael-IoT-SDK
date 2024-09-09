#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mcu.h"
#include "hosal_rf.h"
#include "hci_bridge.h"

#include "FreeRTOS.h"
#include "task.h"

#include "main.h"
#include "cli.h"
#include "log.h"


int main(void)
{
    printf("HCI\r\n");
    cli_init();
    
    hosal_rf_init(HOSAL_RF_MODE_MULTI_PROTOCOL);

    hci_bridge_init();
    hci_uart_init();

    return 0;
}
