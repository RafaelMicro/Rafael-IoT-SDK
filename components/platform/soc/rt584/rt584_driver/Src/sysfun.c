/**************************************************************************//**
 * @file     sysfun.c
 * @version
 * @brief    System function implement
 *
 * @copyright
 ******************************************************************************/

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include "mcu.h"
#include "sysfun.h"
#include "wdt.h"

/**
 * @brief Nest of critical section.
 */
static int critical_counter = 0;

void enter_critical_section(void)
{
    __disable_irq();
    critical_counter++;
}

void leave_critical_section(void)
{
    critical_counter--;

#ifdef  DEBUG
    if (critical_counter < 0)
    {
        /*Serious Error */
        while (1);
    }
#endif

    if (critical_counter == 0)
    {
        __enable_irq();
    }
}

uint32_t version_check(void)
{
#if 0
    /*not support yet*/
    uint32_t   version_info, ret = 1, chip_id, chip_rev;

    version_info =  SYSCTRL_S->CHIP_INFO ;

    chip_id =  (version_info >> 8) & 0xFF;
    chip_rev = (version_info >> 4) & 0x0F;

    if (chip_id != 0x84)
    {
        return 0;
    }

    return ret;

#else

    return 1;

#endif

}
/*
 *  Reset the system software by using the watchdog timer to reset the chip.
 *
 */
void sys_software_reset(void)
{
    wdt_ctrl_t controller;


    controller.reg = 0;
    if (WDT->control.bit.lockout)
    {
        while (1);
    }

    WDT->clear = 1;/*clear interrupt*/

    controller.bit.int_en = 0;
    WDT->prescale = 15;    /*clock is divided by 16*/
    controller.bit.reset_en = 1;
    controller.bit.wdt_en = 1;

    WDT->win_max = 0x2;
    WDT->control.reg = controller.reg;
    while (1);
}
/*
 *  Reset the system software by using the watchdog timer to reset the chip.
 *
 */
chip_model_t getotpversion()
{
    chip_model_t  chip_model;

    chip_model.type =0;
    chip_model.version = 0;
    
    return chip_model;
}
