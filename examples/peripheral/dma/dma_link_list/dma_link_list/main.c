#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hosal_dma.h"
#include "uart_stdio.h"
#include "hosal_sysctrl.h"


uint8_t  dma_src_mem_buf[1024];
uint8_t  dma_dest_mem_buf[1024];
uint32_t dma_finish_state;

#define   DMA_LL_ITEM0_SIZE           6
#define   DMA_LL_ITEM1_SIZE           6
#define   DMA_LL_ITEM2_SIZE           2
#define   DMA_LL_ITEM3_SIZE         256


void dma_link_list_user_complete(uint32_t channel_id, uint32_t status) {
    /*this ISR will not set dma_finish_state bit, if DMA_INT_LL_ELEMENT_Done..*/
    if (status & DMA_INT_XFER_DONE) {
        if (channel_id == 0) {
            dma_finish_state = 1;
            /*printf("dma finish %d %c%c",g_dma_finish_state,'\r','\n');*/
        }
    }

    return;
}

uint32_t mem_cmp(uint8_t channel, uint8_t *test_src_addr, uint8_t *test_dest_addr, uint32_t length) {
    uint32_t  i;
    uint8_t   *src8_addr, *dest8_addr;

    src8_addr = (uint8_t *) test_src_addr;
    dest8_addr = (uint8_t *) test_dest_addr;

    for (i = 0; i < length ; i++) {
        if (src8_addr[i] != dest8_addr[i]) {
            printf("Error index: %d channel: %d \r\n", i, channel);
            printf("compare error  src: %x  dest: %x \r\n", (uint32_t)(src8_addr + i), (uint32_t)(dest8_addr + i));
            return STATUS_ERROR;
        }

        /*printf(" %d %c%c",src8_addr[i],'\r','\n');*/
    }

    /*create linklist for memory move.*/
    /*
     * In this example, we will move
     *
     *   dma_src_mem_buf[4] ~ dma_src_mem_buf[9]     6 bytes
     *   dma_src_mem_buf[24] ~ dma_src_mem_buf[29]   6 bytes
     *   dma_src_mem_buf[38] ~ dma_src_mem_buf[39]   2 bytes
     *   dma_src_mem_buf[62] ~ dma_src_mem_buf[317]   256 bytes
     *   to
     *   dma_dest_mem_buf[0] ~ dma_dest_mem_buf[269]
     *
     *   So after the dma linklist move finish, the dma_dest_mem_buf will become:
     *    4 5 6 7 8 9 24 25 26 27 28 29 38 39 62 63 64 .....  0xFE 0xFF 0 ... 61
     */

    return STATUS_SUCCESS;
}

int main(void) {

    uint32_t address, i, temp, dest_offset, ret_status;
    hosal_dma_dev_t dma_dev;
    hosal_dma_link_dev_t   dma_link_dev;


    uart_stdio_init();
    hosal_dma_init();
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : hosal dma link list demo\r\n");
    printf("----------------------------------------------------------------\r\n");

    
    for (i = 0; i < 1024; i++) {
        dma_src_mem_buf[i] =  0xFF;
        dma_dest_mem_buf[i] = 0xFF;
    }

    for (i = 0; i < 1024; i++) {
        dma_src_mem_buf[i] =  (i + 2);
        dma_dest_mem_buf[i] = 0xFF;
    }

    /*printf("using DMA interrupt mode: %c%c",'\r','\n');*/

    dma_dev.channel = 0;
    dma_dev.src_address = (uint32_t)dma_src_mem_buf;
    dma_dev.dst_address = (uint32_t)dma_dest_mem_buf;
    dma_dev.callbackfn = dma_link_list_user_complete;
    dma_dev.size = 1024;

    hosal_dma_interrupt_mode(&dma_dev);
    
    while (dma_finish_state != 0) {;}
    dma_finish_state = 0;

    for (i = 0; i < 1024; i++) {
        if (dma_src_mem_buf[i] != dma_dest_mem_buf[i]) {

            printf("Interrupt Error %d \r\n", i);
            while (1);
        }

        dma_dest_mem_buf[i] = 0;    /*because next we want to use polling mode test*/
    }

    dest_offset = 0;
    /*printf("using DMA interrupt mode: %c%c",'\r','\n');*/
    dma_link_dev.hosal_item_number = HOSAL_DMA_LINK_LIST_ITEM;
    dma_link_dev.hosal_dma_link_request[0].src_ptr  = dma_src_mem_buf + 4;                  /*src address*/
    dma_link_dev.hosal_dma_link_request[0].dest_ptr =  (dma_dest_mem_buf + dest_offset);    /*destinatio address*/
    dma_link_dev.hosal_dma_link_request[0].size = DMA_LL_ITEM0_SIZE;                        /*move 6 bytes*/
    hosal_delay_ms(1);
    dest_offset +=  DMA_LL_ITEM0_SIZE;

    dma_link_dev.hosal_dma_link_request[1].src_ptr  = dma_src_mem_buf + 24;
    dma_link_dev.hosal_dma_link_request[1].dest_ptr = dma_dest_mem_buf + dest_offset;
    dma_link_dev.hosal_dma_link_request[1].size = DMA_LL_ITEM1_SIZE;
    hosal_delay_ms(1);
    dest_offset +=  DMA_LL_ITEM1_SIZE;

    dma_link_dev.hosal_dma_link_request[2].src_ptr  = dma_src_mem_buf + 38;
    dma_link_dev.hosal_dma_link_request[2].dest_ptr = dma_dest_mem_buf + dest_offset;
    dma_link_dev.hosal_dma_link_request[2].size = DMA_LL_ITEM2_SIZE;
    hosal_delay_ms(1);
    dest_offset +=  DMA_LL_ITEM2_SIZE;

    dma_link_dev.hosal_dma_link_request[3].src_ptr  = dma_src_mem_buf + 62;
    dma_link_dev.hosal_dma_link_request[3].dest_ptr = dma_dest_mem_buf + dest_offset;
    dma_link_dev.hosal_dma_link_request[3].size = DMA_LL_ITEM3_SIZE;
    hosal_delay_ms(1);
    dest_offset +=  DMA_LL_ITEM3_SIZE;

    dma_finish_state = 0;
    hosal_dma_linklist(&dma_dev, &dma_link_dev);
    hosal_delay_ms(1);
    while (dma_finish_state != 0) {;}
    dma_finish_state = 0;

    i = 0;
    /*compare data here.*/
    for (i = 0; i < 4; i++) {

        ret_status = mem_cmp(0, dma_link_dev.hosal_dma_link_request[i].src_ptr,
                             dma_link_dev.hosal_dma_link_request[i].dest_ptr,  dma_link_dev.hosal_dma_link_request[i].size);

        if (ret_status == STATUS_ERROR) {
            while (1);
        }

    }

    printf("\r\n\r\n");
    printf("hosal dms memory link list finish\r\n");
    while (1);

}
