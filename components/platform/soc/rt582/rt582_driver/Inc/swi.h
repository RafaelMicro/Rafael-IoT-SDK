/**
 * \file            swi.h
 * \brief           software interrupt driver header file
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
 *
 * Author:         ives.Lee
 */
#ifndef SWI_H
#define SWI_H

typedef enum {
    SWI_ID_0          = 0,    /**< ID 0. */
    SWI_ID_1          = 1,    /**< ID 1. */
    SWI_ID_2          = 2,    /**< ID 2. */
    SWI_ID_3          = 3,    /**< ID 3. */
    SWI_ID_4          = 4,    /**< ID 4. */
    SWI_ID_5          = 5,    /**< ID 5. */
    SWI_ID_6          = 6,    /**< ID 6. */
    SWI_ID_7          = 7,    /**< ID 7. */
    SWI_ID_8          = 8,    /**< ID 8. */
    SWI_ID_9          = 9,    /**< ID 9. */
    SWI_ID_10         = 10,   /**< ID 10. */
    SWI_ID_11         = 11,   /**< ID 11. */
    SWI_ID_12         = 12,   /**< ID 12. */
    SWI_ID_13         = 13,   /**< ID 13. */
    SWI_ID_14         = 14,   /**< ID 14. */
    SWI_ID_15         = 15,   /**< ID 15. */
    SWI_ID_16         = 16,   /**< ID 16. */
    SWI_ID_17         = 17,   /**< ID 17. */
    SWI_ID_18         = 18,   /**< ID 18. */
    SWI_ID_19         = 19,   /**< ID 19. */
    SWI_ID_20         = 20,   /**< ID 20. */
    SWI_ID_21         = 21,   /**< ID 21. */
    SWI_ID_22         = 22,   /**< ID 22. */
    SWI_ID_23         = 23,   /**< ID 23. */
    SWI_ID_24         = 24,   /**< ID 24. */
    SWI_ID_25         = 25,   /**< ID 25. */
    SWI_ID_26         = 26,   /**< ID 26. */
    SWI_ID_27         = 27,   /**< ID 27. */
    SWI_ID_28         = 28,   /**< ID 28. */
    SWI_ID_29         = 29,   /**< ID 29. */
    SWI_ID_30         = 30,   /**< ID 30. */
    SWI_ID_31         = 31,   /**< ID 31. */
    MAX_NUMBER_OF_SWI = 32,   /**< Max SWI Number 32. */
} swi_id_sel_t;

/**
 * @brief User cb handler prototype.
 *
 * This function is called when the requested number of samples has been processed.
 *
 * @param[in] swi_id Software Interrupt ID
 *              \arg SWI_ID_0 ~ SWI_ID_31
 */
typedef void (*swi_isr_handler_t)(uint32_t swi_id);

/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/
#define SWI_INT_ENABLE(para_set)                 (REMAP->SW_IRQ_EN_SET = para_set)      /**< Enable the software interrupt*/
#define SWI_INT_ENABLE_GET()                     (REMAP->SW_IRQ_EN_SET)                 /**< Return the software interrupt enable status*/
#define SWI_INT_DISABLE(para_set)                (REMAP->SW_IRQ_EN_CLR = para_set)      /**< Disable the software interrupt*/
#define SWI_INT_CLEAR(para_set)                  (REMAP->SW_IRQ_CLR = para_set)         /**< Clear the software interrupt status*/
#define SWI_INT_STATUS_SET(para_set)             (REMAP->SW_IRQ_SET = para_set)         /**< Set the software interrupt status*/
#define SWI_INT_STATUS_GET()                     (REMAP->SW_IRQ_SET)                    /**< Return the software interrupt status*/

/**************************************************************************************************
 *    GLOBAL PROTOTYPES
 *************************************************************************************************/
/**
 * \brief Clear all SWI interrupt service routine callback.
 * \return None
 */
void swi_int_callback_clear(void);

/**
 * \brief Enable the specified software interrupts
 * \param[in] swi_id Software Interrupt ID
 *              \arg SWI_ID_0 ~ SWI_ID_31
 * \param[in] swi_int_callback Software interrupt callback handler
 * \return None
 */
void swi_int_enable(swi_id_sel_t swi_id, swi_isr_handler_t swi_int_callback);

/**
 * \brief Disable software interrupt(s)
 * \param[in] swi_id Software interrupt ID
 *              \arg SWI_ID_0 ~ SWI_ID_31
 * \return None
 */
void swi_int_disable(swi_id_sel_t swi_id);

/**
 * \brief Trigger software interrupt(s)
 * \param[in] swi_id Software interrupt ID
 *            \arg SWI_ID_0 ~ SWI_ID_31
 * \return None
 */
void swi_int_trigger(swi_id_sel_t swi_id);


#endif /* End of _SWI_H_ */

/** @} */ /* End of Peripheral Group */

