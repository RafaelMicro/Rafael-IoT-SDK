/***********************************************************************************************************************
 * @file     SWI.c
 * @version
 * @brief    SWI driver file
 *
 * @copyright
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *    INCLUDES
 **********************************************************************************************************************/
#include "swi.h"
/***********************************************************************************************************************
 *    CONSTANTS AND DEFINES
 **********************************************************************************************************************/
#define MAX_NUMBER_OF_SOFT_INT                    2
#define SWI_MAX_NUMBER                            32

static swi_cb_t swi_cb[MAX_NUMBER_OF_SOFT_INT] = {NULL, NULL};
/***********************************************************************************************************************
 *    GLOBAL FUNCTIONS
 **********************************************************************************************************************/
/**
* @brief Register_Soft_Intr
* @details config swi callback function and interrupt
* @param[in] id,soft_cb_fun
* @return None
*/
uint32_t register_soft_intr(uint32_t id, swi_cb_t soft_cb_fun)
{
    if ((id >= MAX_NUMBER_OF_SOFT_INT) || (soft_cb_fun == NULL))
    {
        return STATUS_INVALID_PARAM;
    }

    /*NOTICE: soft_cb_fun must be in non-secure.*/

    if (swi_cb[id] == NULL)
    {
        swi_cb[id] = soft_cb_fun;
        NVIC_EnableIRQ((IRQn_Type)(Soft0_IRQn + id));
        /*TODO: priority setting*/
    }
    else
    {
        return STATUS_INVALID_REQUEST;      /*device already opened*/
    }

    return STATUS_SUCCESS;
}
/**
* @brief Enable_Soft_Intr
* @details Trigger swi
* @param[in] id
* @return None
*/
uint32_t enable_soft_intr(uint32_t id, uint32_t bit)
{
    if (id == 0)
    {
        SW_INT0->enable_irq |= bit;
    }
    else if (id == 1)
    {
        SW_INT1->enable_irq |= bit;
    }
    else
    {
        return STATUS_INVALID_PARAM;
    }

    return STATUS_SUCCESS;
}
/**
* @brief clear software interrupt
* @details config pwm paramas, sequence order,number, play count, clcok div,trigger source,nterrupt
* @param[in] pwm_para_config
* @return None
*/
uint32_t clear_soft_intr(uint32_t id, uint32_t bit)
{

    if (id == 0)
    {
        SW_INT0->clear_irq |= bit;
    }
    else if (id == 1)
    {
        SW_INT1->clear_irq |= bit;
    }
    else
    {
        return STATUS_INVALID_PARAM;
    }

    return STATUS_SUCCESS;
}
/**
* @brief Get software interrupt data
*/
uint32_t get_soft_intr_data(uint32_t id, uint32_t *data)
{
    if (id == 0)
    {
        *data = SW_INT0->data;
    }
    else if (id == 1)
    {
        *data = SW_INT1->data;
    }
    else
    {
        return STATUS_INVALID_PARAM;
    }

    return STATUS_SUCCESS;
}
/**
* @brief Set software interrupt data
*/
uint32_t set_soft_intr_data(uint32_t id, uint32_t data)
{
    if (id == 0)
    {
        SW_INT0->data = data;
    }
    else if (id == 1)
    {
        SW_INT1->data = data;
    }
    else
    {
        return STATUS_INVALID_PARAM;
    }

    return STATUS_SUCCESS;
}
/**
* @brief SWI Interrupt Hander initinal function
*/
void SWI0_Handler(void)
{
    swi_cb_t  cb_isr_fun;

    uint32_t i = 0, get_status = 0;

    for (i = 0; i < SWI_MAX_NUMBER; i++)
    {
        get_status = (SW_INT0->irq_state) & (1 << i);

        if (get_status != 0)
        {
            SW_INT0->clear_irq = (SW_INT0->clear_irq | (1 << i));

            cb_isr_fun = swi_cb[0];

            if (cb_isr_fun != NULL)
            {
                /*
                     * call register Software Interrupt callback
                     * Please Notice: the CB is ISR context switch too!
                     */
                cb_isr_fun(i, SW_INT0->data);
            }
        }
    }
}
/**
* @brief SWI Interrupt Hander initinal function
*/
void SWI1_Handler(void)
{
    swi_cb_t  cb_isr_fun;

    uint32_t i = 0, get_status = 0;

    for (i = 0; i < SWI_MAX_NUMBER; i++)
    {
        get_status = SW_INT1->irq_state & (1 << i);

        if (get_status != 0)
        {
            SW_INT1->clear_irq = (SW_INT1->clear_irq | (1 << i));

            cb_isr_fun = swi_cb[1];

            if (cb_isr_fun != NULL)
            {
                /*
                     * call register Software Interrupt callback
                     * Please Notice: the CB is ISR context switch too!
                     */
                cb_isr_fun(i, SW_INT1->data);
            }
        }
    }
}

