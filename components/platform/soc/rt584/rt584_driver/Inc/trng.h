/***********************************************************************************************************************
* @file     trng.h
* @version
* @brief    rt584 trng driver API
*
* @copyright
**********************************************************************************************************************/
/**
* @defgroup trng_group trng
* @ingroup peripheral_group
* @{
* @brief   trng definitions, structures, and functions
*/
#ifndef _RT584_TRNG_H_
#define _RT584_TRNG_H_

#ifdef __cplusplus
extern "C"
{
#endif


/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include "mcu.h"
#include "sysctrl.h"
#include "pufs_rt_regs.h"

/***********************************************************************************************************************
 *    GLOBAL PROTOTYPES
 **********************************************************************************************************************/
/**
* @brief get_random_numberk
* @details
* @param[in] p_buffer,
* @param[in] number,
* @retval    STATUS_SUCCESS           If uninitialization was successful.
*/
uint32_t Get_Random_Number(uint32_t *p_buffer, uint32_t number);

#ifdef __cplusplus
}
#endif

#endif


