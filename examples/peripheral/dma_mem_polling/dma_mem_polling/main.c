#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "cli.h"
#include "log.h"
#include "hosal_dma.h"


uint8_t  dma_src_mem_buf[1024];
uint8_t  dma_dest_mem_buf[1024];

int main(void) {
    uint32_t address, i, temp, dest_offset, ret_status;

    hosal_dma_dev_t dma_dev;

    printf(" hosal dma polling exmaples %s, %s \r\n", __DATE__, __TIME__);

    hosal_dma_init();

    for (i = 0; i < 1024; i++) {
        dma_src_mem_buf[i] =  i;
        dma_dest_mem_buf[i] = 0xFF;
    }

    printf("using DMA polling mode: %c%c", '\r', '\n');

    dma_dev.channel = 0;
    dma_dev.src_address = (uint32_t)dma_src_mem_buf;
    dma_dev.dst_address = (uint32_t)dma_dest_mem_buf;
    dma_dev.callbackfn = NULL;
    dma_dev.size = 1024;

    hosal_dma_polling_mode(&dma_dev);

    for (i = 0; i < 1024; i++) {
        if (dma_src_mem_buf[i] != dma_dest_mem_buf[i]) {

            printf("polling Error %d\n", i);
            while (1);
        }

        dma_dest_mem_buf[i] = 0;    /*because next we want to use polling mode test*/
    }

    printf("DMA polling mode move successful!%c%c", '\r', '\n');

    while (1);

}
