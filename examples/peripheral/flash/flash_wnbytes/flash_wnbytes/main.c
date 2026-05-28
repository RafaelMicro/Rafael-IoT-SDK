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
    static uint8_t buf1[1024], buf2[1024];
    
    uart_stdio_init();
    vHeapRegionsInt();

	
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : flash read/write n bytes demo\r\n");
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


    len = 768;

    printf("  Erase Sector  \r\n");
    status = hosal_flash_erase(HOSAL_FLASH_ERASE_SECTOR, flash_operation_address);
    while(hosal_flash_ioctrl(HOSAL_FLASH_BUSY,NULL));
    for (i = 0; i < len; i++) {
        buf1[i] = (i + 1);
        buf2[i] = 0xFF;
    }
     
    printf("  Write %d Btyes \r\n",len);
    status = hosal_flash_write_n_bytes(flash_operation_address,buf1,len);
    while(hosal_flash_ioctrl(HOSAL_FLASH_BUSY,NULL));
   


    printf("  Read %d Btyes \r\n",len);
    status = hosal_flash_read_n_bytes(flash_operation_address, buf2,len);
    while(hosal_flash_ioctrl(HOSAL_FLASH_BUSY,NULL));
    
   
      for(i=0;i<len;i++)
      {
          if(buf1[i]!=buf2[i])
          {
            printf("error %d:%.2x",i,buf2[i]);
          }
      }
  
    printf(" \nhosal flash read/write %d bytes success \r\n",len);

    while (1) {;}

}

