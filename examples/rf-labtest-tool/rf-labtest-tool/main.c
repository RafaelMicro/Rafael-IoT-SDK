/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */


#include <stdio.h>
#include <string.h>
#include "mcu.h"
//#include "cm3_mcu.h"
//#include "project_config.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"

#include "uart_drv.h"
//#include "retarget.h"
#include "sysctrl.h"
#include "lpm.h"
#include "gpio.h"

#include "rf_mcu.h"
#include "rf_common_init.h"


//#include "bsp.h"
//#include "util_printf.h"
//#include "util_log.h"

#include "dtm_mode.h"
#include "rfet.h"
#include "uart_bridge.h"
#include "app_hooks.h"
#include "uart_stdio.h"
/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
#define RX_BUF_SIZE         128
#define TX_BUF_SIZE         128
#define GPIO_LED            22
#define GPIO_SWITCH_0       17
#define GPIO_SWITCH_1       21

#ifndef UART_BRIDGE_EN
#define UART_BRIDGE_EN      (false) // debug only
#endif


/*
#define RF_FW_LOAD_SELECT_RUCI_CMD                ((RF_FW_LOAD_SELECT)0x01)
#define RF_FW_LOAD_SELECT_BLE_CONTROLLER          ((RF_FW_LOAD_SELECT)0x02)
#define RF_FW_LOAD_SELECT_MULTI_PROTCOL_2P4G      ((RF_FW_LOAD_SELECT)0x10)
#define RF_FW_LOAD_SELECT_UNSUPPORTED_CMD         ((RF_FW_LOAD_SELECT)0xE0)
#define RF_FW_LOAD_SELECT_FAIL                    ((RF_FW_LOAD_SELECT)0xFF)
*/
#define RF_FW_SELECT         (RF_FW_LOAD_SELECT_RUCI_CMD)
#define LPM_SRAM0_RETAIN                (0x1E)
/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/

/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/

/**************************************************************************************************
 *    GLOBAL VARIABLES
 *************************************************************************************************/
extern bool              g_gui_dtm_task;
extern RFET_MODE_SWITCH  g_rfet_mode_control;
/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/

void set_priotity(void)
{
    NVIC_SetPriority(Uart0_IRQn, 0x01);
    NVIC_SetPriority(Uart1_IRQn, 0x01);
    NVIC_SetPriority(CommSubsystem_IRQn, 0x00);
}

/*this is pin mux setting*/
void init_default_pin_mux(void)
{
    /*uart0 pinmux, This is default setting,
      we set it for safety. */
    //pin_set_mode(16, MODE_UART);     /*GPIO16 as UART0 RX*/
    //pin_set_mode(17, MODE_UART);     /*GPIO17 as UART0 TX*/

    /*uart1 pinmux*/
#if (defined(CONFIG_RT581) || defined(CONFIG_RT582) || defined(CONFIG_RT583))
    pin_set_mode(28, MODE_UART);     /*GPIO28 as UART1 TX,  J10.13*/
    pin_set_mode(29, MODE_UART);     /*GPIO29 as UART1 RX,  J10.14*/
#endif

    gpio_cfg_input(GPIO_TEST, GPIO_PIN_NOINT);
    pin_set_mode(GPIO_TEST, MODE_GPIO); /* GPIO 2/GPIO 1 as Labtest/DTM mode switch */
    pin_set_pullopt(GPIO_TEST, PULLUP_100K);
    return;
}

void dtm_gui_mode_init(void)
{
#if (UART_BRIDGE_EN == true)
    uasb_init(false);
#else
    RfMcu_DmaInit();
#endif
    /* DTM initialization */
    g_gui_dtm_task = true;
    dtm_sys_common_init();
    dtm_mode_init();
    g_rfet_mode_control = RFET_MODE_CONTROL_GUI;
}

void cli_mode_init(void)
{
    g_rfet_mode_control = RFET_MODE_CONTROL_CLI;
    rfet_init();
    printf("[Init]:RFET Init Done\r\n");
}

int rfet_main(void)
{
    /* Delay function init*/
#if (defined(CONFIG_RT581) || defined(CONFIG_RT582) || defined(CONFIG_RT583))
    //delay_init();
#endif

    /* RF system priority set */
    set_priotity();

    /* Uart Pin Mux Init*/
    init_default_pin_mux();

    /* DMA init*/
#if (defined(CONFIG_RT581) || defined(CONFIG_RT582) || defined(CONFIG_RT583))
    dma_init();
#endif

    rfet_init_cb_registration(rfet_init);

    /* Never sleep set*/
    lpm_low_power_mask(LOW_POWER_MASK_BIT_RESERVED31);

    /* Retaintion set*/
    lpm_set_sram_sleep_deepsleep_shutdown(LPM_SRAM0_RETAIN);

    /* Power level set: Normal */
    lpm_set_low_power_level(LOW_POWER_LEVEL_NORMAL);

    /*
        Default is DTM and GUI mode init
        If CLI mode is enabled, the CLI and GUI can be switched by pull GPIO
    */
#if (CLI_EN)
    if (gpio_pin_get(GPIO_TEST) == 0)
    {
        dtm_gui_mode_init();
    }
    else
    {
        cli_mode_init();
    }
#else
    dtm_gui_mode_init();
#endif
    return 0;
}

#if !(defined(RFET_LIB))
int main(void)
{
    uart_stdio_init();
    vHeapRegionsInt();
    rfet_main();
    /* Start the scheduler. */
    vTaskStartScheduler();
}
#endif



