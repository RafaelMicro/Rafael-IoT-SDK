#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "cli.h"
#include "log.h"
#include "hosal_flash.h"


int main(void) {

    uint32_t flash_operation_address, len, i;
    int status;
    uint8_t  buf1[256], buf2[256];

    printf("hosal flash examples %s %s\r\n", __DATE__, __TIME__);

    flash_operation_address = 0x70000;

    status = hosal_flash_erase(HOSAL_FLASH_ERASE_SECTOR, flash_operation_address);

    for (i = 0; i < 256; i++) {
        buf1[i] = i + 1;
        buf2[i] = 0xFF;
    }

    status = hosal_flash_write(HOSAL_FLASH_WRITE_PAGE, flash_operation_address, buf1);

    status = hosal_flash_read(HOSAL_FLASH_READ_PAGE, flash_operation_address, buf2);

    for (i = 0; i < 256; i++) {

        if (buf1[i] != buf2[i]) {

            printf(" %x ", buf2[i]);
        }

    }

    printf("hosal flash erase/read/write success \r\n");

    while (1) {;}

}

