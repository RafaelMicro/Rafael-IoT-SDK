/**************************************************************************//**
 * @file     i2s.c
 * @version
 * @brief    I2S driver
 *
 * @copyright
 ******************************************************************************/


/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include "i2s.h"

static i2s_isr_handler_t  i2s_reg_handler = NULL;

void i2s_Register_Int_Callback(i2s_isr_handler_t i2s_int_callback)
{
    i2s_reg_handler = i2s_int_callback;

    return;
}
uint32_t I2s_Init(i2s_para_set_t *i2s_para)
{
    I2S_T *i2s = I2S_MASTER;
    i2s_xdma_ctrl_ptr_t *i2s_xdma_config;
    uint8_t blk_osr = 0;

    if ((i2s_para->fmt >= I2S_FMT_MAX) |
            (i2s_para->sr >= I2S_SR_MAX))
    {
        //Fail Cases
        return STATUS_INVALID_PARAM;
    }

    // Make I2S reset
    I2S_RESET();

    i2s->I2S_MCLK_SET0.bit.CFG_MCK_ISEL = i2s_para->imck_rate;

    i2s->I2S_MCLK_SET1.bit.CFG_MCK_DIV = i2s_para->mck_div;

    i2s->I2S_MS_SET0.bit.CFG_BCK_OSR = i2s_para->bck_osr;
    i2s->I2S_MS_SET0.bit.CFG_I2S_MOD = i2s_para->trx_mode;
    i2s->I2S_MS_SET0.bit.CFG_I2S_FMT = i2s_para->fmt;
    i2s->I2S_MS_SET0.bit.CFG_BCK_LEN = i2s_para->bck_ratio;
    i2s->I2S_MS_SET0.bit.CFG_TXD_WID = i2s_para->width;
    i2s->I2S_MS_SET0.bit.CFG_RXD_WID = i2s_para->width;
    i2s->I2S_MS_SET0.bit.CFG_TXD_CHN = i2s_para->ch;
    i2s->I2S_MS_SET0.bit.CFG_RXD_CHN = i2s_para->ch;

    // Reset XDMA
    i2s->I2S_RDMA_CTL1.reg = I2S_RDMA_RESET;
    i2s->I2S_WDMA_CTL1.reg = I2S_WDMA_RESET;
    // Clear XDMA IRQ
    i2s->I2S_INT_CLEAR = I2S_RDMA_IRQ_CLR | I2S_RDMA_ERR_IRQ_CLR | I2S_WDMA_IRQ_CLR | I2S_WDMA_ERR_IRQ_CLR;

    if (i2s_para->trx_mode == I2S_TRX_MODE_TXRX)
    {
        // Enable XDMA irq
        i2s->I2S_INT_MASK = ~(I2S_RDMA_IRQ_MASK_ENABLE | I2S_RDMA_ERR_IRQ_MASK_ENABLE | I2S_WDMA_IRQ_MASK_ENABLE | I2S_WDMA_ERR_IRQ_MASK_ENABLE);
        NVIC_EnableIRQ(I2s0_IRQn);

        i2s_xdma_config = i2s_para->rdma_config;
        i2s->I2S_RDMA_SET0 = (((uint32_t)i2s_xdma_config->i2s_xdma_seg_size << I2S_RDMA_SEG_SIZE_SHFT) & I2S_RDMA_SEG_SIZE_MASK) |
                             (((uint32_t)i2s_xdma_config->i2s_xdma_blk_size << I2S_RDMA_BLK_SIZE_SHFT) & I2S_RDMA_BLK_SIZE_MASK);
        i2s->I2S_RDMA_SET1 = i2s_xdma_config->i2s_xdma_start_addr;

        i2s_xdma_config = i2s_para->wdma_config;
        i2s->I2S_WDMA_SET0 = (((uint32_t)i2s_xdma_config->i2s_xdma_seg_size << I2S_RDMA_SEG_SIZE_SHFT) & I2S_RDMA_SEG_SIZE_MASK) |
                             (((uint32_t)i2s_xdma_config->i2s_xdma_blk_size << I2S_RDMA_BLK_SIZE_SHFT) & I2S_RDMA_BLK_SIZE_MASK);
        i2s->I2S_WDMA_SET1 = i2s_xdma_config->i2s_xdma_start_addr;
    }
    else if (i2s_para->trx_mode == I2S_TRX_MODE_TX)
    {
        // Enable XDMA irq
        i2s->I2S_INT_MASK = ~(I2S_RDMA_IRQ_MASK_ENABLE | I2S_RDMA_ERR_IRQ_MASK_ENABLE);
        NVIC_EnableIRQ(I2s0_IRQn);

        i2s_xdma_config = i2s_para->rdma_config;
        i2s->I2S_RDMA_SET0 = (((uint32_t)i2s_xdma_config->i2s_xdma_seg_size << I2S_RDMA_SEG_SIZE_SHFT) & I2S_RDMA_SEG_SIZE_MASK) |
                             (((uint32_t)i2s_xdma_config->i2s_xdma_blk_size << I2S_RDMA_BLK_SIZE_SHFT) & I2S_RDMA_BLK_SIZE_MASK);
        i2s->I2S_RDMA_SET1 = i2s_xdma_config->i2s_xdma_start_addr;
    }
    else if (i2s_para->trx_mode == I2S_TRX_MODE_RX)
    {
        // Enable XDMA irq
        i2s->I2S_INT_MASK = ~(I2S_WDMA_IRQ_MASK_ENABLE | I2S_WDMA_ERR_IRQ_MASK_ENABLE);
        NVIC_EnableIRQ(I2s0_IRQn);

        i2s_xdma_config = i2s_para->wdma_config;
        i2s->I2S_WDMA_SET0 = (((uint32_t)i2s_xdma_config->i2s_xdma_seg_size << I2S_RDMA_SEG_SIZE_SHFT) & I2S_RDMA_SEG_SIZE_MASK) |
                             (((uint32_t)i2s_xdma_config->i2s_xdma_blk_size << I2S_RDMA_BLK_SIZE_SHFT) & I2S_RDMA_BLK_SIZE_MASK);
        i2s->I2S_WDMA_SET1 = i2s_xdma_config->i2s_xdma_start_addr;
    }
    else
    {
        return STATUS_INVALID_PARAM;
    }

    return STATUS_SUCCESS;
}

uint32_t I2s_Uninit(void)
{
    I2S_T *i2s = I2S_MASTER;

    // Make I2S reset
    I2S_RESET();

    // Reset XDMA
    i2s->I2S_RDMA_CTL1.reg = I2S_RDMA_RESET;
    i2s->I2S_WDMA_CTL1.reg = I2S_WDMA_RESET;

    // Clear XDMA IRQ
    i2s->I2S_INT_CLEAR = I2S_RDMA_IRQ_CLR | I2S_RDMA_ERR_IRQ_CLR | I2S_WDMA_IRQ_CLR | I2S_WDMA_ERR_IRQ_CLR;

    // Disable XDMA irq
    i2s->I2S_INT_MASK = (I2S_RDMA_IRQ_MASK_ENABLE | I2S_RDMA_ERR_IRQ_MASK_ENABLE | I2S_WDMA_IRQ_MASK_ENABLE | I2S_WDMA_ERR_IRQ_MASK_ENABLE);
    NVIC_DisableIRQ(I2s0_IRQn);

    return STATUS_SUCCESS;
}

uint32_t I2s_Start(i2s_para_set_t *i2s_para)
{
    I2S_T *i2s = I2S_MASTER;
    if (i2s_para->trx_mode == I2S_TRX_MODE_TXRX)
    {
        i2s->I2S_RDMA_CTL0.reg |= I2S_RDMA_ENABLE;
        i2s->I2S_WDMA_CTL0.reg |= I2S_WDMA_ENABLE;
    }
    else if (i2s_para->trx_mode == I2S_TRX_MODE_TX)
    {
        i2s->I2S_RDMA_CTL0.reg = I2S_RDMA_ENABLE;
    }
    else if (i2s_para->trx_mode == I2S_TRX_MODE_RX)
    {
        i2s->I2S_WDMA_CTL0.reg = I2S_WDMA_ENABLE;
    }
    else
    {
        return STATUS_INVALID_PARAM;
    }
    // Enable I2S
    i2s->I2S_MS_CTL0.bit.CFG_MCK_ENA = 1;
    i2s->I2S_MS_CTL0.bit.CFG_I2S_ENA = 1;

    return STATUS_SUCCESS;
}

uint32_t I2s_Stop(void)
{
    I2S_T *i2s = I2S_MASTER;

    // Disable I2S
    i2s->I2S_MS_CTL0.bit.CFG_I2S_ENA = 0;
    i2s->I2S_MS_CTL0.bit.CFG_MCK_ENA = 0;

    return STATUS_SUCCESS;
}


#if 0
/**
 * @ingroup I2S_Driver
 * @brief I2S interrupt
 * @details
 * @return
 */
void I2s0_Handler(void)
{
    I2S_T *i2s = I2S_MASTER;
    i2s_cb_t cb;

    if (i2s->I2S_INT_STATUS & I2S_RDMA_IRQ_MASK_MASK)
    {
        i2s->I2S_INT_CLEAR |= I2S_RDMA_IRQ_CLR;

        if (((i2s->I2S_RDMA_SET0 & 0xFFFF0000) >> 16) == 0)
        {
            //If one-shot mode happened, this condition  fuction is valid.
            i2s->I2S_RDMA_CTL0.reg = I2S_RDMA_ENABLE;
        }
        cb.type = I2S_CB_RDMA;
        cb.blk_size = I2S_GET_RDMA_BLK_SIZE;
        cb.seg_size = I2S_GET_RDMA_SEG_SIZE;
        if( i2s_reg_handler != NULL )
        {
            i2s_reg_handler(&cb);
        }
    }
    else if (i2s->I2S_INT_STATUS & I2S_RDMA_ERR_IRQ_MASK_MASK)
    {
        i2s->I2S_INT_CLEAR |= I2S_RDMA_ERR_IRQ_CLR;
    }
    else if (i2s->I2S_INT_STATUS & I2S_WDMA_IRQ_MASK_MASK)
    {
        i2s->I2S_INT_CLEAR |= I2S_WDMA_IRQ_CLR;

        if (((i2s->I2S_WDMA_SET0 & 0xFFFF0000) >> 16) == 0)
        {
            //If one-shot mode happened, this condition  fuction is valid.
            i2s->I2S_WDMA_CTL0.reg = I2S_WDMA_ENABLE;
        }
        cb.type = I2S_CB_WDMA;
        cb.blk_size = I2S_GET_WDMA_BLK_SIZE;
        cb.seg_size = I2S_GET_WDMA_SEG_SIZE;

        if( i2s_reg_handler != NULL )
        {
            i2s_reg_handler(&cb);
        }
    }
    else if (i2s->I2S_INT_STATUS & I2S_WDMA_ERR_IRQ_MASK_MASK)
    {
        i2s->I2S_INT_CLEAR |= I2S_WDMA_ERR_IRQ_CLR;
    }

    return;
}
#endif
