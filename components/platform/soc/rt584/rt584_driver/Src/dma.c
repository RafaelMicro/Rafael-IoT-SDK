/***********************************************************************************************************************
 * @file     dma.c
 * @version
 * @brief     dma dervier function
 *
 * @copyright
 **********************************************************************************************************************/
/***********************************************************************************************************************
*    INCLUDES
**********************************************************************************************************************/
#include "mcu.h"
#include "dma.h"

static dma_proc_cb_t   dma_usr_isr_cb[MAX_NUMBER_OF_DMA] = {NULL, NULL};
/***********************************************************************************************************************
 *    GLOBAL FUNCTIONS
 **********************************************************************************************************************/
/**
* @brief Dma_Register_Isr
*   register dam callback function and config dma interrupt
*/
uint32_t dma_register_isr(uint32_t  dma_channel_id, dma_proc_cb_t  app_dma_callback)
{
    DMA_T   *DMA;

    if ((dma_channel_id >= MAX_NUMBER_OF_DMA) || (app_dma_callback == NULL))
    {
        return STATUS_INVALID_PARAM;
    }

    if (dma_channel_id == 0)
    {
        DMA = DMA0;
    }
    else
    {
        DMA = DMA1;
    }

    if (dma_usr_isr_cb[dma_channel_id] != NULL)
    {
        return STATUS_EBUSY;
    }
    else
    {
        if (DMA->dma_control & DMA_BUSY)
        {
            STATUS_EBUSY;    /*almost impossible... some function using polling DMA?*/
        }

        DMA->dma_int = (DMA_CHL_INT_CLEAR_CLR | DMA_CHL_INT_WAKEUP_CLR);
        DMA->dma_int = DMA_CHL_INT_ENABLE | DMA_CHL_INT_WAKEUP;

        dma_usr_isr_cb[dma_channel_id] = app_dma_callback;      /*remember dma isr callback.*/

        NVIC_EnableIRQ( (IRQn_Type) (Dma_Ch0_IRQn + dma_channel_id));
    }

    return STATUS_SUCCESS;
}
/**
* @brief initinal dma and start dma function , interrupt mode
*
*/
uint32_t dma_config_and_enable(uint32_t  dma_channel_id, const dma_config_t *mode)
{
    DMA_T   *DMA;

    if ((dma_channel_id >= MAX_NUMBER_OF_DMA) || (mode == NULL))
    {
        return STATUS_INVALID_PARAM;
    }

    if (dma_channel_id == 0)
    {
        DMA = DMA0;
    }
    else
    {
        DMA = DMA1;
    }

    if (DMA->dma_control & DMA_BUSY)
    {
        return STATUS_EBUSY;    /*almost impossible... some function using polling DMA?*/
    }

    if (mode->dma_length == 0)
    {
        return STATUS_INVALID_PARAM;    /*length could not be zero*/
    }

    DMA->dma_src_adr  = mode->dma_src_addr;
    DMA->dma_dest_adr  = mode->dma_dest_addr;
    DMA->dma_bytes_num = mode->dma_length;

    DMA->dma_control = DMA_START_ENABLE;    /*start DMA*/

    return STATUS_SUCCESS;
}
/**
* @brief initinal dma and start dma function , no-interrupt mode
*
*/
uint32_t dma_config_and_enable_polling(uint32_t  dma_channel_id, const dma_config_t *mode)
{
    DMA_T   *DMA;

    if ((dma_channel_id >= MAX_NUMBER_OF_DMA) || (mode == NULL))
    {
        return STATUS_INVALID_PARAM;
    }

    NVIC_DisableIRQ( (IRQn_Type) (Dma_Ch0_IRQn + dma_channel_id));

    if (dma_channel_id == 0)
    {
        DMA = DMA0;
    }
    else
    {
        DMA = DMA1;
    }

    if (DMA->dma_control & DMA_BUSY)
    {
        return STATUS_EBUSY;    /*almost impossible... some function using polling DMA?*/
    }

    if (mode->dma_length == 0)
    {
        return STATUS_INVALID_PARAM;    /*length could not be zero*/
    }

    DMA->dma_src_adr  = mode->dma_src_addr;
    DMA->dma_dest_adr  = mode->dma_dest_addr;
    DMA->dma_bytes_num = mode->dma_length;

    DMA->dma_control = DMA_START_ENABLE;    /*start DMA*/

    while ((DMA->dma_control & DMA_BUSY)) {;}

    return STATUS_SUCCESS;
}
/**
* @brief check the dma status busy or idle mode
*
*/
uint32_t dma_get_status(uint32_t  dma_channel_id, uint32_t *status)
{
    DMA_T   *DMA;

    if ((dma_channel_id >= MAX_NUMBER_OF_DMA) || (status == NULL))
    {
        return STATUS_INVALID_PARAM;
    }

    if (dma_channel_id == 0)
    {
        if  (DMA0->dma_control & DMA_BUSY)
        {
            *status = DMA_STATUS_BUSY;
        }
        else
        {
            *status = DMA_STATUS_FREE;
        }
    }
    else
    {

        if  (DMA1->dma_control & DMA_BUSY)
        {
            *status = DMA_STATUS_BUSY;
        }
        else
        {
            *status = DMA_STATUS_FREE;
        }
    }

    return STATUS_SUCCESS;
}

/**
* @brief dma interrupt handler.
*
*/
void Dma0_Handler(void)
{

    DMA0->dma_int |= (DMA_CHL_INT_CLEAR_CLR | DMA_CHL_INT_WAKEUP_CLR);

    if (dma_usr_isr_cb[0] != NULL)
    {
        dma_usr_isr_cb[0](0);
    }

    return;
}

void Dma1_Handler(void)
{
    DMA1->dma_int |= (DMA_CHL_INT_CLEAR_CLR | DMA_CHL_INT_WAKEUP_CLR);

    if (dma_usr_isr_cb[1] != NULL)
    {
        dma_usr_isr_cb[1](1);
    }

    return;
}
