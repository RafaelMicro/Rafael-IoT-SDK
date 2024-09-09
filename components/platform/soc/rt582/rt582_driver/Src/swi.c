/**
 * \file            swi.c
 * \brief           software interrupt driver file
 *
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
 *
 * Author:          ives.lee
 */


/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include "mcu.h"

/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/

/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/

/**************************************************************************************************
 *    GLOBAL VARIABLES
 *************************************************************************************************/
static swi_isr_handler_t   swi_reg_handler[MAX_NUMBER_OF_SWI];

/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/


/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/


/**
 * @ingroup swi_group
 * @brief Software interrupt handler
 * @details Clear the software interrupt status and handle the software interrupt routine
 * @param None
 * @return None
 */
void sw_handler(void) {
    swi_id_sel_t i;
    uint32_t swi_int_reg;

    swi_int_reg = SWI_INT_STATUS_GET();
    SWI_INT_CLEAR(swi_int_reg);

    if (SWI_INT_STATUS_GET()) {
        ASSERT();
    }

    if (swi_int_reg) {
        for (i = SWI_ID_0; i < MAX_NUMBER_OF_SWI; i++) {
            if ((swi_int_reg & (1 << i)) && (swi_reg_handler[i] != NULL)) {
                swi_reg_handler[i](i);
            }
        }
    }

    return;
}


/**
 * @ingroup swi_group
 * @brief SWI interrupt service routine callback for user application.
 * @param[in] swi_id Software interrupt ID
 * @param[in] swi_int_callback Software interrupt callback handler
 * @return None
 */
static void swi_int_callback_register(swi_id_sel_t swi_id, swi_isr_handler_t swi_int_callback) {
    swi_reg_handler[swi_id] = swi_int_callback;

    return;
}


/**
 * @ingroup swi_group
 * @brief Unregister SWI interrupt service routine callback.
 * @param[in] swi_id Software interrupt ID
             \arg SWI_ID_0 ~ SWI_ID_31
 * @return None
 */
static void swi_int_callback_unregister(swi_id_sel_t swi_id) {
    swi_reg_handler[swi_id] = NULL;

    return;
}


void swi_int_callback_clear(void) {
    swi_id_sel_t i;

    for (i = SWI_ID_0; i < MAX_NUMBER_OF_SWI; i++) {
        swi_int_callback_unregister(i);
    }

    return;
}


void swi_int_enable(swi_id_sel_t swi_id, swi_isr_handler_t swi_int_callback) {
    uint32_t swi_mask;

    if (swi_id < MAX_NUMBER_OF_SWI) {
        swi_int_callback_register(swi_id, swi_int_callback);

        swi_mask = (1 << swi_id);

        //if (SWI_INT_ENABLE_GET() == NULL) {
        NVIC_EnableIRQ((IRQn_Type)(Soft_IRQn));
        //}

        SWI_INT_CLEAR(swi_mask);
        SWI_INT_ENABLE(swi_mask);
    }

    return;
}


void swi_int_disable(swi_id_sel_t swi_id) {
    uint32_t swi_mask;

    if (swi_id < MAX_NUMBER_OF_SWI) {
        swi_int_callback_unregister(swi_id);

        swi_mask = (1 << swi_id);

        SWI_INT_DISABLE(swi_mask);
        SWI_INT_CLEAR(swi_mask);

        if (SWI_INT_ENABLE_GET() == NULL) {
            NVIC_DisableIRQ((IRQn_Type)(Soft_IRQn));
        }
    }

    return;
}


void swi_int_trigger(swi_id_sel_t swi_id) {
    uint32_t swi_mask;

    if (swi_id < MAX_NUMBER_OF_SWI) {
        swi_mask = (1 << swi_id);

        SWI_INT_STATUS_SET(swi_mask);
    }

    return;
}

