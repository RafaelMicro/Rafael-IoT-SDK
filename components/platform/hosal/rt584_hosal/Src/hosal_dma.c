/**
 * \file            hosal_dma.c
  * \brief          Hosal Dma driver file
 */

/*
 * Copyright (c) 2024 Rafael Micro
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of library_name.
 *
 * Author:   
 */

#include "stdio.h"
#include <stdint.h>
#include "mcu.h"
#include "dma.h"
#include "hosal_dma.h"

#if 0
int hosal_dma_unint(void) {
    dma_uninit();

    return STATUS_SUCCESS;
}


int hosal_dma_init(void) {
    uint32_t status = STATUS_SUCCESS;

    dma_init();

    return (int)status;
}


int hosal_dma_interrupt_mode(hosal_dma_dev_t *dma_dev) {
    uint32_t dma_status, status = STATUS_SUCCESS;
    dma_config_t  dma_mode_config;

    dma_mode_config.dma_control_setting = DMA_CNTL_SRC_INC |
                                          DMA_CNTL_SRC_WIDTH_32BIT |  DMA_CNTL_SRC_BURST4 | DMA_CNTL_SRC_PORT |
                                          DMA_CNTL_DEST_INC | DMA_CNTL_DEST_WIDTH_32BIT | DMA_CNTL_DEST_BURST4
                                          |  DMA_CNTL_DEST_PORT | DMA_CNTL_MODE_MEMTOMEM;

    dma_mode_config.dma_src_addr = dma_dev->src_address;
    dma_mode_config.dma_dest_addr = dma_dev->dst_address;
    dma_mode_config.dma_xfer_length = dma_dev->size;
    dma_mode_config.dma_repeat_setting = 0;           /*We don't use DMA repeat mode in memory to memory*/
    dma_mode_config.dma_int_enable  = DMA_INT_ALL;    /*We use interrupt to notify Finish*/

    dma_register_isr(dma_dev->channel, dma_dev->callbackfn);

    dma_config(dma_dev->channel, &dma_mode_config);

    dma_enable(dma_dev->channel);

    return STATUS_SUCCESS;
}


int hosal_dma_polling_mode(hosal_dma_dev_t *dma_dev) {

    uint32_t dma_status, status = STATUS_SUCCESS;
    dma_config_t     dma_mode_config;

    dma_mode_config.dma_control_setting = DMA_CNTL_SRC_INC |
                                          DMA_CNTL_SRC_WIDTH_32BIT |  DMA_CNTL_SRC_BURST4 | DMA_CNTL_SRC_PORT |
                                          DMA_CNTL_DEST_INC | DMA_CNTL_DEST_WIDTH_32BIT | DMA_CNTL_DEST_BURST4
                                          |  DMA_CNTL_DEST_PORT | DMA_CNTL_MODE_MEMTOMEM;

    dma_mode_config.dma_src_addr = (uint32_t) dma_dev->src_address;
    dma_mode_config.dma_dest_addr = (uint32_t) dma_dev->dst_address;
    dma_mode_config.dma_xfer_length = dma_dev->size;
    dma_mode_config.dma_repeat_setting = 0;           /*We don't use DMA repeat mode in memory to memory*/
    dma_mode_config.dma_int_enable  = 0;              /*polled mode.. not enable interrupt*/

    dma_config(dma_dev->channel, &dma_mode_config);

    dma_enable(dma_dev->channel);

    dma_status = 0;

    do {
        dma_get_status(dma_dev->channel, &dma_status);

    } while ((dma_status & DMA_INT_XFER_DONE) != DMA_INT_XFER_DONE);

    dma_finish(dma_dev->channel);

    return STATUS_SUCCESS;
}



int hosal_dma_linklist(hosal_dma_dev_t *dma_dev, hosal_dma_link_dev_t *dev_link_dev) {


    uint32_t  dma_control_mode;


    /*the following setting is linkListEnable, meory to memory. */
    dma_control_mode =  DMA_CNTL_SRC_WIDTH_32BIT | DMA_CNTL_DEST_WIDTH_32BIT |
                        DMA_CNTL_SRC_INC  | DMA_CNTL_SRC_BURST8 | DMA_CNTL_SRC_PORT |
                        DMA_CNTL_DEST_INC | DMA_CNTL_DEST_BURST8 |  DMA_CNTL_DEST_PORT |
                        DMA_CNTL_MODE_MEMTOMEM  | DMA_CNTL_LINKLIST_ENABLE;


    /*create a LinkList reqest for hardware..*/
    create_dma_link_list(
        (dma_linklist_request_t *) (dev_link_dev->hosal_dma_link_request), dev_link_dev->hosal_item_number,
        (dma_linklist_entry_t *) (dev_link_dev->hosal_dma_link_list_entry), dma_control_mode );



    /*set dma to link list mode*/
    dma_set_link_list(dma_dev->channel, (dma_linklist_entry_t *) (dev_link_dev->hosal_dma_link_list_entry),
                      (DMA_INT_LL_ELEMENT_Done | DMA_INT_XFER_DONE));

    /*start DMA */
    dma_enable(dma_dev->channel);


    return STATUS_SUCCESS;
}
#endif
