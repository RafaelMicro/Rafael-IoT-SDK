/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hosal_lpm.h"
#include "hosal_sysctrl.h"
#include "uart_stdio.h"

int main(void) {


    uart_stdio_init();
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : hosal Power Down demo\r\n");
    printf("HOST MCU Enter Power Down\r\n");
    printf("HOST MCU Control RF  MCU Enter Sleep\r\n");
    printf("----------------------------------------------------------------\r\n");

    hosal_lpm_ioctrl(HOSAL_LPM_SET_POWER_LEVEL, HOSAL_LPM_POWER_DOWN);
    hosal_lpm_ioctrl(HOSAL_LPM_SUBSYSTEM_ENTER_LOW_POWER, HOSAL_COMMUMICATION_SUBSYSTEM_PWR_STATE_DEEP_SLEEP);
    printf("Wiat 1 sec\r\n");
    hosal_delay_ms(1000);
    hosal_lpm_ioctrl(HOSAL_LPM_ENTER_LOW_POWER, HOSAL_LPM_PARAM_NONE);

    while (1) {;}
}


