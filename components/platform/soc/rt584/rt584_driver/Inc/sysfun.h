/**************************************************************************//**
 * @file     sysfun.h
 * @version
 * @brief    SYSTEM HELP FUNCTION API DEFINITION
 *
 * @copyright
 ******************************************************************************/
/** @defgroup SYSFUN_Driver SYSTEM FUNTION Driver
 *  @ingroup  peripheral_group
 *  @{
 *  @brief  System function header information
 */
#ifndef SYSFUN_H
#define SYSFUN_H

#ifdef __cplusplus
extern "C"
{
#endif
/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include "mcu.h"
/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
/**
 *  @brief To maintain compatibility with 58x, the modification will be done by using the macro (#define) in the pre-processing stage.
*/
/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/

typedef enum
{
    CHIP_TYPE_581 = 0x01,            /*!<ic type 581 */
    CHIP_TYPE_582 = 0x02,            /*!<ic type 582 */
    CHIP_TYPE_583 = 0x03,            /*!<ic type 583 */
    CHIP_TYPE_584 = 0x04,            /*!<ic type 584 */
    CHIP_TYPE_UNKNOW = 0xFF,
} chip_type_t;


typedef enum
{
    CHIP_VERSION_SHUTTLE = 0x00,                     /*!<ic type SHUTTLE */
    CHIP_VERSION_MPA = 0x01,                         /*!<ic type MPA */
    CHIP_VERSION_MPB = 0x02,                   /*!<ic type MPB */
    CHIP_VERSION_UNKNOW = 0xFF,
} chip_version_t;

typedef struct __attribute__((packed))
{
    chip_type_t     type;
    chip_version_t  version;
}
chip_model_t;
/**
 * @brief Irq priority definitions.
 */
typedef enum
{
    IRQ_PRIORITY_HIGHEST = 0,
    IRQ_PRIORITY_HIGH = 1,
    IRQ_PRIORITY_NORMAL = 3,
    IRQ_PRIORITY_LOW = 5,
    IRQ_PRIORITY_LOWEST = 7,
} irq_priority_t;

/**************************************************************************************************
 *    GLOBAL PROTOTYPES
 *************************************************************************************************/
/**
* @brief   enter critical sections
* @details This function is nest function, that is, system call this function several times.
*           This function will mask all interrupt , except non-mask interrupt.
*           So as short as possible for using this function.
*
*/
void enter_critical_section(void);

/**
 * @brief   leave critical sections
 * @details Because enter critical sections is nest allowed.
 *           So it will only unmask all inerrupt when the number "enter_critical_section"
 *           equals "leave_critical_section" times.
 *           Please be careful design your code when calling enter_critical_section/leave_critical_section.
 *           One Enter_Critical_Section must have one corresponding leave_critical_section!
 *
 */
void leave_critical_section(void);

/**
 * @brief   check hardware chip version and software defined version compared value.
 * @details
 *           version_check is help function to check
 *           software project setting is the same as hardware IC version.
 *           If software project define "CHIP_VERSION" is
 *           not matched with hardware IC version, this functio will return 0, otherwise 1.
 * @return
 * @retval    0 --- hardware and system defined matched.
 * @retval    1 --- hardware and system defined mis-matched.
 */
uint32_t version_check(void);

/**
 * @brief   System reset
 * @details Reset the system software by using the watchdog timer to reset the chip.
 */
void sys_software_reset(void);
/*@}*/ /* end of peripheral_group SYSFUN_Driver */

#ifdef __cplusplus
}
#endif

#endif      /* end of ___SYSFUN_H__ */

