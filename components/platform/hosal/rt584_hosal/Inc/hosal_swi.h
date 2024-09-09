/**
 * \file            hosal_swi.h
 * \brief           hosal_swi software interrupt include file
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
 * Author:         ives.lee
 */
#ifndef HOSAL_SWI_H
#define HOSAL_SWI_H


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief           software interrupt typedef callback function
 */
typedef void (*hosal_swi_callback_fn)(uint32_t p_arg);

/**
 * \brief           softwarer interrupt id number
 */
typedef enum
{
    HOSAL_SWI_0          = 0,    /**< software interrupt identifier  0. */
    HOSAL_SWI_1          = 1,    /**< software interrupt identifier  1. */
    HOSAL_SWI_2          = 2,    /**< software interrupt identifier  2. */
    HOSAL_SWI_3          = 3,    /**< software interrupt identifier  3. */
    HOSAL_SWI_4          = 4,    /**< software interrupt identifier  4. */
    HOSAL_SWI_5          = 5,    /**< software interrupt identifier  5. */
    HOSAL_SWI_6          = 6,    /**< software interrupt identifier  6. */
    HOSAL_SWI_7          = 7,    /**< software interrupt identifier  7. */
    HOSAL_SWI_8          = 8,    /**< software interrupt identifier  8. */
    HOSAL_SWI_9          = 9,    /**< software interrupt identifier  9. */
    HOSAL_SWI_10         = 10,   /**< software interrupt identifier  10. */
    HOSAL_SWI_11         = 11,   /**< software interrupt identifier  11. */
    HOSAL_SWI_12         = 12,   /**< software interrupt identifier  12. */
    HOSAL_SWI_13         = 13,   /**< software interrupt identifier  13. */
    HOSAL_SWI_14         = 14,   /**< software interrupt identifier  14. */
    HOSAL_SWI_15         = 15,   /**< software interrupt identifier  15. */
    HOSAL_SWI_16         = 16,   /**< software interrupt identifier  16. */
    HOSAL_SWI_17         = 17,   /**< software interrupt identifier  17. */
    HOSAL_SWI_18         = 18,   /**< software interrupt identifier  18. */
    HOSAL_SWI_19         = 19,   /**< software interrupt identifier  19. */
    HOSAL_SWI_20         = 20,   /**< software interrupt identifier  20. */
    HOSAL_SWI_21         = 21,   /**< software interrupt identifier  21. */
    HOSAL_SWI_22         = 22,   /**< software interrupt identifier  22. */
    HOSAL_SWI_23         = 23,   /**< software interrupt identifier  23. */
    HOSAL_SWI_24         = 24,   /**< software interrupt identifier  24. */
    HOSAL_SWI_25         = 25,   /**< software interrupt identifier  25. */
    HOSAL_SWI_26         = 26,   /**< software interrupt identifier  26. */
    HOSAL_SWI_27         = 27,   /**< software interrupt identifier  27. */
    HOSAL_SWI_28         = 28,   /**< software interrupt identifier  28. */
    HOSAL_SWI_29         = 29,   /**< software interrupt identifier  29. */
    HOSAL_SWI_30         = 30,   /**< software interrupt identifier  30. */
    HOSAL_SWI_31         = 31,   /**< software interrupt identifier  31. */
    HOSAL_MAX_NUMBER_OF_SWI = 32,  /**< Max SWI Number 32. */
} hosal_swi_id_sel_t;

/**
 * \brief           
 * \note            
 *                    
 */
typedef struct {
    hosal_swi_id_sel_t 		 id;            /*!software interrupt id struct*/
    hosal_swi_callback_fn    config;       /*!software callback function*/
} hosal_swi_dev_t;

            

/**
 * \brief           Software interrupt initialize
 * \note            
 * \param[in]       NONE 
 * \return          return function status
 */
int hosal_swi_uninit(void);
/**
 * \brief           Software interrupt uninitializ
 * \note            
 * \param[in]       NONE 
 * \return          return function status
 */
int hosal_swi_init(void);
/**
 * \brief           Trigger software interrupt
 * \note            
 * \param[in]       swi_id: software interrupt identification 
 * \return          return function status
 */
int hosal_swi_trigger(uint8_t swi_id);
/**
 * \brief           software interrupt register callback fucntion
 * \note            
 * \param[in]       swi_id:  software interrupt identification 
 * \param[in]       hosal_swi_callback_fn: call back function
 * \return          return function status
 */
int hosal_swi_callback_register(uint8_t swi_id, hosal_swi_callback_fn pfn_callback);
/**
 * \brief           software interrupt unregister callback fucntion
 * \note            
 * \param[in]       swi_id: First number
 * \return          return function status
 */
int hosal_swi_callback_unregister(uint8_t swi_id);


#ifdef __cplusplus
}
#endif

#endif
