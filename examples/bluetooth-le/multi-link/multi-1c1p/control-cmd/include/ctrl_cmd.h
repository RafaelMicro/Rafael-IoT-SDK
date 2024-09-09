/**************************************************************************//**
* @file       ctrl_cmd.h
* @brief      Provide the declarations that for BLE Data Rate Commands.
*
*****************************************************************************/

#ifndef _CTRL_CMD_H_
#define _CTRL_CMD_H_

#include <stdint.h>
#include <stdbool.h>
#include "print_common.h"

/** Control Command "=" Sign Definition
 */
#define CTRL_CMD_ASSIGN_PUNC          '='
#define CTRL_CMD_PARAM_SEP_PUNC       ','

/** Marcro Return Control Command Error String. */
#define PRINT_CTRL_CMD_ERROR(err)     (printf("%s\n", err))


/**************************************************************************
 * 1M1S Command Identification Definitions
 **************************************************************************/
/** UART Command Identification Definition
*/
#define CTRL_CMD_TABLE    \
        X(UART_CMD_HELP,        "Help",         "Help.") \
        X(UART_CMD_DATA,        "Send",         "Transmit data. ex: enter Send=0,5,12345 => Host_Id=0, Len=5, Data=12345.") \
        X(UART_CMD_ADV_START,   "EnAdv",        "Enable advertising.") \
        X(UART_CMD_ADV_STOP,    "DisAdv",       "Disable advertising.") \
        X(UART_CMD_SCAN_START,  "EnScan",       "Enable scan.") \
        X(UART_CMD_SCAN_STOP,   "DisScan",      "Disable scan.") \
        X(UART_CMD_DISCONN,     "Disconnect",   "Disable connection (command with host id.), ex: enter Disconnect=0.") \
        X(UART_CMD_NONE,        "",             "")


#define X(a, b, c) a,
/** UART Command Identification in ENUM
*/
typedef enum
{
    CTRL_CMD_TABLE
} CTRL_CMD_EValue;
#undef X


#define X(a, b, c) b,
/**UART Command Identification in String to a Table
*/
static char *ctrl_cmd_table[] = {CTRL_CMD_TABLE};
#undef X

#define X(a, b, c) c,
/** UART Command Identification Description to a Table
*/
static char *ctrl_cmd_description_table[] = {CTRL_CMD_TABLE};
#undef X

/**************************************************************************
 * Control Command Function
 **************************************************************************/
/** Handle Control Command Function.
 *
 * @param[in] data  : a pointer to received data from UART.
 * @param[in] length : data length.
 * @return none
 */
void handle_ctrl_cmd(uint8_t *data, int length);


#endif // _CTRL_CMD_H_

