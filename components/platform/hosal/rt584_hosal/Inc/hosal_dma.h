/**
 * \file            hosal_dam.h
 * \brief           hosal_dma include file
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
 * Author:  ives.lee 
 */

#ifndef HOSAL_DMA_H
#define HOSAL_DMA_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \brief HOSAL DMA LINK LIST Number
 */
#define HOSAL_DMA_LINK_LIST_ITEM            8

typedef void (*hosal_dma_cb_fn)(uint32_t channel_id, uint32_t status);


/**
 *  \brief DMA linklist mode for DMA setting
 *  This linklist mode is very special dma mode, most application will not use this special setting.
 *  Only if you need to move some "spare data" to become "continous" data, this mode will use.
 *
 */

typedef struct  __attribute__((packed)) {
    uint8_t    *src_ptr;                       /*!< linklist source pointer */
    uint8_t    *dest_ptr;                      /*!< linklist destination pointer */
    uint32_t   size;                           /*!< dma move bytes number */
} hosal_dma_linklist_request_t;

/**
 *   \brief DMA LinkListEntry.
 *   Please do NOT change the declare order  of dma_linklist_entry_t.. it is hardware used setting
 *   Please refer DMA document for the structure setting
 */
typedef struct {
    uint32_t   src_addr;                       /*!< dma linklist source pointer */                       
    uint32_t   dest_addr;                      /*!< dma linklist destination pointer */                       
    uint32_t   transfer_length;                /*!< dma linklist transfer length */
    uint32_t   control_set;                    /*!< dma linklist control set */
    uint32_t   next_ptr_addr;                  /*!< dma linklist next pointer address*/
    uint32_t   status;                         /*!< dma linklist status */
    uint32_t   repeat_set;                     /*!< dma linklist repeat set */
    uint32_t   resv;                           /*!< dma reserved fild */
} hosal_dma_linklist_entry_t;
/**
 * \brief DMA device control setting struct.
 * 
 */
typedef struct {
    uint8_t         channel;                    /*!< dma device channel */ 
    uint8_t         interrupt;                  /*!< dma interrupt enable*/ 
    uint32_t        src_address;                /*!< dma source address */ 
    uint32_t        dst_address;                /*!< dma destination address */ 
    uint32_t        size;                       /*!< dma size */ 
    hosal_dma_cb_fn callbackfn;                 /*!< dma callback function */ 
} hosal_dma_dev_t;

/**
 * \brief dma link lsit 
 * 
 */
typedef struct {
    uint8_t                        hosal_item_number;                                   /*!< dma linklist item number */ 
    hosal_dma_linklist_request_t   hosal_dma_link_request[HOSAL_DMA_LINK_LIST_ITEM];    /*!< dma linklist request  */ 
    hosal_dma_linklist_entry_t     hosal_dma_link_list_entry[HOSAL_DMA_LINK_LIST_ITEM]; /*!< dma linklist link list entry */ 
} hosal_dma_link_dev_t;

/**
 * \brief            dam uninit function
 * \param[in]        NONE
 * \return function status  
 */
int hosal_dma_unint(void);
/**
 * \brief            dam init function
 * \param[in]        NONE
 * \return function status            
 */
int hosal_dma_init(void);
/**
 * \brief   dma interrupt mode function
 * 
 * \param[in]   dma_dev  
 * \return function status 
 */
int hosal_dma_interrupt_mode(hosal_dma_dev_t *dma_dev);
/**
 * \brief    dma polling mode function
 * 
 * \param dma_dev 
 * \return function status 
 */
int hosal_dma_polling_mode(hosal_dma_dev_t *dma_dev);
/**
 * \brief    dma link list function
 * 
 * \param[in] dma_dev 
 * \param[in] dev_link_dev 
 * \return function status 
 */
int hosal_dma_linklist(hosal_dma_dev_t *dma_dev, hosal_dma_link_dev_t *dev_link_dev);



#ifdef __cplusplus
}
#endif

#endif
