/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hosal_otp.h"
#include "hosal_uart.h"
#include "hosal_trng.h"
#include "hosal_sysctrl.h"
#include "uart_stdio.h"



int main(void) {
    

    uint32_t  i,j,status;
    uint32_t randum[32];

    uart_stdio_init();
    printf("\r\n");
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : hosal rand number demo\r\n");
    printf("----------------------------------------------------------------\r\n");

    while (1)
    {
        hosal_trng_get_random_number(randum,8);

        for (i = 0; i < 4; i++)
        {
            printf("%.8X", randum[i]);
        }
        printf("\r\n");
        hosal_delay_ms(500);
    }

	printf("\r\n");    

    while (1) {;}
}

