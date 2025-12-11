/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "app_hooks.h"
#include "uart_stdio.h"

int main(void)
{
    uart_stdio_init();
    vHeapRegionsInt();
   
    printf("application hello world");
    while (1)
    {
        /* code */
    }
    
}
