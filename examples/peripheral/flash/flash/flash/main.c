/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "hosal_status.h"
#include "hosal_flash.h"
#include "hosal_sysctrl.h"
#include "uart_stdio.h"
#include "hosal_dma.h"
#include "app_hooks.h"


int main(void) {

    uint32_t flash_operation_address, len, i;
    int status;
    static uint8_t buf1[256], buf2[256];

    uart_stdio_init();
    vHeapRegionsInt();

	
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : flash demo\r\n");
    printf("----------------------------------------------------------------\r\n");

    #if defined(CONFIG_RT581) || defined(CONFIG_RT582)
    flash_operation_address = 0x00099000;
    #elif defined(CONFIG_RT583)
    flash_operation_address = 0x0012F000;
    #elif defined(CONFIG_RF1301)
    flash_operation_address = 0x1009C000;
    #elif defined(CONFIG_RT584H) || defined(CONFIG_RT584L)
    flash_operation_address = 0x10132000;
    #else
    flash_operation_address = 0x10272000;
    #endif


    status = hosal_flash_erase(HOSAL_FLASH_ERASE_SECTOR, flash_operation_address);
    while(hosal_flash_ioctrl(HOSAL_FLASH_BUSY,NULL));

    for (i = 0; i < 256; i++) {
        buf1[i] = i + 1;
        buf2[i] = 0xFF;
    }

    status = hosal_flash_write(HOSAL_FLASH_WRITE_PAGE, flash_operation_address, buf1);
    while(hosal_flash_ioctrl(HOSAL_FLASH_BUSY,NULL));

    status = hosal_flash_read(HOSAL_FLASH_READ_PAGE, flash_operation_address, buf2);
    while(hosal_flash_ioctrl(HOSAL_FLASH_BUSY,NULL));

    for (i = 0; i < 256; i++) {

        if (buf1[i] != buf2[i]) {

            printf(" %x ", buf2[i]);
        }
    }
    
    printf("hosal flash erase/read/write address %.8X success \r\n", flash_operation_address);

    while (1) {;}

}