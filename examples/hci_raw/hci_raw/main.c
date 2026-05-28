/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

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
#include "app_hooks.h"
#include "uart_stdio.h"


int main(void)
{
    uart_stdio_init();
    vHeapRegionsInt();
    printf("HCI\r\n");
    cli_init();
    
    hosal_rf_init(HOSAL_RF_MODE_MULTI_PROTOCOL);

    hci_bridge_init();
    hci_uart_init();

    return 0;
}
