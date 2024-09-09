/**************************************************************************//**
 * @file     project_config.h
 * @version
 * @brief   define project configuration
 *
 * @copyright
 ******************************************************************************/

#include "chip_define.h"

#ifndef ___PROJECT_CONFIG_H__
#define ___PROJECT_CONFIG_H__

// <<< Use Configuration Wizard in Context Menu >>>\n

#define MODULE_ENABLE(module)     (module > 0)

/* If system support multi-tasking, some hardware need mutex to protect*/
//<q> Multitasking
#define SUPPORT_MULTITASKING               1

//==========================================================
// <h> Driver
//==========================================================

//==========================================================
// <h> UART0
//==========================================================
/*System use UART0 */
//<q> SUPPORT_UART0
#define SUPPORT_UART0                      1

//<q> UART0_USER_HANDLE_RECV_RX
#define UART0_USER_HANDLE_RECV_RX          1
// </h>

//==========================================================
// <h> UART1
//==========================================================
/*System use UART1 */
//<q> SUPPORT_UART1
#define SUPPORT_UART1                      1

//<q> SUPPORT_UART1_TX_DMA
#define SUPPORT_UART1_TX_DMA               1

//<q> SUPPORT_UART1_RX_DMA
#define SUPPORT_UART1_RX_DMA               1

/*enable this option, UART1 will support CTS/RTS pin for flow control*/
//<q> SUPPORT_UART1_FLOWCNTL
#define SUPPORT_UART1_FLOWCNTL             0
// </h>

//==========================================================
// <h> UART2
//==========================================================
/*System use UART2 */
//<q> SUPPORT_UART2
#define SUPPORT_UART2                      1

//<q> SUPPORT_UART2_TX_DMA
#define SUPPORT_UART2_TX_DMA               1

//<q> SUPPORT_UART2_RX_DMA
#define SUPPORT_UART2_RX_DMA               1
// </h>

//<q> SUPPORT_QSPI_DMA
#define SUPPORT_QSPI_DMA                   1

//==========================================================
// <h> Crypto
//==========================================================
/*Support AES  */
//<q> CRYPTO_AES_ENABLE
#define CRYPTO_AES_ENABLE                  1

/*Support ECC SECP192R1/P192 curve */
//<q> CRYPTO_SECP192R1_ENABLE
#define CRYPTO_SECP192R1_ENABLE            1

/*Support ECC SECP256R1/P256 curve */
//<q> CRYPTO_SECP256R1_ENABLE
#define CRYPTO_SECP256R1_ENABLE            1

/*Support ECC SECT163R2/B163 curve */
//<q> CRYPTO_SECT163R2_ENABLE
#define CRYPTO_SECT163R2_ENABLE            1
// </h>
// </h>

//==========================================================
// <h> FreeRTOS
//==========================================================
//<q> SUPPORT_FREERTOS_PORT
#define SUPPORT_FREERTOS_PORT              1
// </h>

//==========================================================
// <h> Log Setting
//==========================================================
//<q> CONFIG_ENABLE_LOG
#define CONFIG_ENABLE_LOG_TEE               1

// </h>


//==========================================================
// <h> CLI Console
//==========================================================
//<q> DEBUG_CONSOLE_UART_ID
#define DEBUG_CONSOLE_UART_ID               0

// <o> DEBUG_CONSOLE_UART_BAUDRATE
// <1667=>  UART_BAUDRATE_Baud2400
// <833=>   UART_BAUDRATE_Baud4800
// <417=>   UART_BAUDRATE_Baud9600
// <278=>   UART_BAUDRATE_Baud14400
// <208=>   UART_BAUDRATE_Baud19200
// <139=>   UART_BAUDRATE_Baud28800
// <104=>   UART_BAUDRATE_Baud38400
// <69=>    UART_BAUDRATE_Baud57600
// <52=>    UART_BAUDRATE_Baud76800
// <35=>    UART_BAUDRATE_Baud115200
// <8=>     UART_BAUDRATE_Baud500000
// <4=>     UART_BAUDRATE_Baud1000000
// <2=>     UART_BAUDRATE_Baud2000000
#define DEBUG_CONSOLE_UART_BAUDRATE         35


//<q> SUPPORT_DEBUG_CONSOLE_CLI
#define SUPPORT_DEBUG_CONSOLE_CLI           0

//<q> SUPPORT_SHELL_CMD_HISTORY
#define SUPPORT_SHELL_CMD_HISTORY           0
// </h>


//==========================================================
// <h> RF TX Power compensation
//==========================================================
//<q> RF_TX_POWER_COMP
#define RF_TX_POWER_COMP                    0
// </h>

// <<< end of configuration section >>>

#endif
