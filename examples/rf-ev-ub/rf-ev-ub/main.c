#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mcu.h"
#include "FreeRTOS.h"
#include "task.h"
#include "hosal_uart.h"
#include <stdio.h>
#include <string.h>
#include "gpio.h"
#include "sysctrl.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"

#include "rf_mcu_types.h"
#include "rf_mcu.h"
#include "rf_common_init.h"
#include "uart_bridge.h"


/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/
#if defined (__ARMCC_VERSION)
#define  _PROJ_NAME_             "ARM Project"
#else
#define  _PROJ_NAME_             "GCC Project"
#endif


/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
#define GPIO16                          (16)
#define GPIO17                          (17)
#define GPIO20                          (20)
#define GPIO21                          (21)
#define GPIO22                          (22)
#define GPIO28                          (28)
#define GPIO29                          (29)
#define GPIO30                          (30)
#define GPIO31                          (31)
#define PTA_DEMO                        (FALSE)
#define GPIO_28_29_AS_DIO               (TRUE && (CHOOSE_UART == UART_0))

/*
 * Remark: UART_BAUDRATE_115200 is not 115200...Please don't use 115200 directly
 * Please use macro define  UART_BAUDRATE_XXXXXX
 */
#define PRINTF_BAUDRATE                 (UART_BAUDRATE_115200)

/*
#define RF_FW_LOAD_SELECT_RUCI_CMD                ((RF_FW_LOAD_SELECT)0x01)
#define RF_FW_LOAD_SELECT_BLE_CONTROLLER          ((RF_FW_LOAD_SELECT)0x02)
#define RF_FW_LOAD_SELECT_MULTI_PROTCOL_2P4G      ((RF_FW_LOAD_SELECT)0x10)
#define RF_FW_LOAD_SELECT_MX_MAC_ACCELARATOR      ((RF_FW_LOAD_SELECT)0x20)
#define RF_FW_LOAD_SELECT_INTERNAL_TEST           ((RF_FW_LOAD_SELECT)0x40)
#define RF_FW_LOAD_SELECT_RFK                     ((RF_FW_LOAD_SELECT)0x80)
#define RF_FW_LOAD_SELECT_UNSUPPORTED_CMD         ((RF_FW_LOAD_SELECT)0xE0)
#define RF_FW_LOAD_SELECT_FAIL                    ((RF_FW_LOAD_SELECT)0xFF)
*/
#define RF_FW_SELECT                    (RF_FW_LOAD_SELECT_RUCI_CMD)



/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/
void set_priotity(void)
{
    //NVIC_SetPriority(Gpio_IRQn,             IRQ_PRIORITY_LOW);
    NVIC_SetPriority(CommSubsystem_IRQn,    IRQ_PRIORITY_HIGH);
}


void app_set_pta_grant_pin(uint8_t gpio)
{
    pin_set_mode(gpio, MODE_GPIO);

    //SYSCTRL->SYS_TEST.bit.CFG_WLAN_ACTIVE_EN        = 1;
    //SYSCTRL->SYS_TEST.bit.CFG_WLAN_ACTIVE_SEL       = gpio;
}


/*this is pin mux setting*/
void init_default_pin_mux(void)
{
    //the following settings are moved to hosal_uart_init
    //pin_set_mode(GPIO16, MODE_UART);        /* GPIO16 as UART0 RX */
    //pin_set_mode(GPIO17, MODE_UART);        /* GPIO17 as UART0 TX */

    //pin_set_mode(GPIO28, MODE_UART);        /* GPIO28 as UART1 TX */ 
    //pin_set_mode(GPIO29, MODE_UART);        /* GPIO29 as UART1 RX */ 

    pin_set_mode(GPIO30, MODE_GPIO);        /* GPIO30 as GPIO debug output */
    gpio_cfg_output(GPIO30);
    gpio_pin_clear(GPIO30);

    pin_set_mode(GPIO31, MODE_GPIO);        /* GPIO31 as GPIO debug output */
    gpio_cfg_output(GPIO31);
    gpio_pin_clear(GPIO31);

    for (uint32_t pin = 0 ; pin < 8 ; pin++)
    {
        pin_set_mode(pin, 7);
    }

#if (CONFIG_RT582)
    SYSCTRL->gpio_aio_ctrl = ((SYSCTRL->gpio_aio_ctrl & 0xF0FFFFFF )| 0x07000000);
#elif (CONFIG_RT584)
#if (RT584_SHUTTLE_IC)
    SYSCTRL->sys_test.bit.dbg_out_sel = 25;
#endif
#endif
}


int main(void)
{
    /*init gpio pin setting*/
    init_default_pin_mux();

    /* RF system priority set */
    set_priotity();

    //Console_Drv_Init(PRINTF_BAUDRATE);

    /* Never Sleep @ Initialization Stage */
    //Comm_Subsystem_Sram_Deep_Sleep_Init();
    
    //enter_critical_section();
    
    /* UART communication at
       high baudrate (true):  1M     or
       low baurdate (false):  115200    */
    uasb_init(true);

    uasb_init_cmd_interface(RF_FW_SELECT, true);
    
    //leave_critical_section();

    printf("[Init]:UART Bridge Init Done \r\n");

    /* Start the scheduler. */
    //vTaskStartScheduler();

    while (1)
    {
        portYIELD();
    }
}

