#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "hosal_dma.h"
#include "uart_stdio.h"

uint8_t  dma_src_mem_buf[1024];
uint8_t  dma_dest_mem_buf[1024];
uint32_t dma_finish_state;

void dma_user_complete(uint32_t channel_id, uint32_t status) {
    
    dma_finish_state = 1;

    return;
}

int main(void) {


    uint32_t address, i;

    hosal_dma_dev_t dma_dev;

    dma_finish_state = 0;

    uart_stdio_init();
    hosal_dma_init();

    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : hosal dma interrupt demo\r\n");
    printf("----------------------------------------------------------------\r\n");


    for (i = 0; i < 1024; i++) {

        dma_src_mem_buf[i] =  i;
        dma_dest_mem_buf[i] = 0xFF;
    }



    printf("using DMA interrupt mode: %c%c", '\r', '\n');

    dma_dev.channel = HOSAL_DMA_ID_0;
    dma_dev.src_address = (uint32_t)dma_src_mem_buf;
    dma_dev.dst_address = (uint32_t)dma_dest_mem_buf;
    dma_dev.callbackfn = dma_user_complete;
    dma_dev.size = 1024;

    
    hosal_dma_interrupt_mode(&dma_dev);

    while (dma_finish_state != 0) {;}
    dma_finish_state = 0;

    for (i = 0; i < 1024; i++) {
        if (dma_src_mem_buf[i] != dma_dest_mem_buf[i]) {

            printf("Interrupt Error %d%c%c", i, '\r', '\n');
            while (1);
        }

        dma_dest_mem_buf[i] = 0;    /*because next we want to use polling mode test*/
    }
    
    printf("\r\n\r\n");
    printf("hosal dma interrupt mode finish!\r\n");

    while (1);

}
