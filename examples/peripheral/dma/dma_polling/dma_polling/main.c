#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hosal_dma.h"
#include "uart_stdio.h"

uint8_t  dma_src_mem_buf[1024];
uint8_t  dma_dest_mem_buf[1024];

int main(void) {
   

    uint32_t address, i, temp, dest_offset, ret_status;

    hosal_dma_dev_t dma_dev;
    
	uart_stdio_init();
	hosal_dma_init();
	
    printf("\r\n");
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : dma polling demo\r\n");
    printf("----------------------------------------------------------------\r\n");

    

    for (i = 0; i < 1024; i++) {
        dma_src_mem_buf[i] =  i;
        dma_dest_mem_buf[i] = 0xFF;
    }

    printf("using DMA polling mode: %c%c", '\r', '\n');

    dma_dev.channel = HOSAL_DMA_ID_0;
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

    printf("\r\n\r\n");
    printf("hosal dma polling mode finish\r\n");

    while (1);

}
