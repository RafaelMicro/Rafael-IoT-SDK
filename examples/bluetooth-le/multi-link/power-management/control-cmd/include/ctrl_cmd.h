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
#include "ble_gap.h"

/** Data Rate Tool Version Definition. */
#define POWER_MEASUREMENT_TOOL_VER  "v0.0.1"

/** AT Command "=" Sign Definition
 */
#define AT_CMD_ASSIGN_PUNC          '='

/** Marcro Return Control Command Error String. */
#define PRINT_CTRL_CMD_ERROR(err)     (printf("ERROR:%s\r\n", err))

/** Marcro Return AT Command OK String
 */
#define PRINT_AT_CMD_OK()           (printf("OK\r\n"))

/**************************************************************************
 * Data Rate Command Identification Definitions
 **************************************************************************/

/** Data Rate Command Identification Definition
*/
#define CTRL_CMD_TABLE    \
        X(AT_COMMAND_TEST,              "AT+TEST",        "Test command.") \
        X(AT_COMMAND_HELP,              "AT+HELP",        "Help") \
        X(AT_COMMAND_VERSION,           "AT+VER",         "Get power measurement tool version.") \
        X(AT_COMMAND_APPPARAM,          "AT+PARAM",       "Get all application BLE parameters.") \
        X(AT_COMMAND_TARGET_ADDR_SET,   "AT+TADDR",       "Set target device address. =112233445566 = \"66:55:44:33:22:11\"") \
        X(AT_COMMAND_ADV_INTERVAL,      "AT+ADVINT",      "Adv. interval in ms. 1:100; 2:500; 3:1000.") \
        X(AT_COMMAND_ADV_TYPE,          "AT+ADVTYPE",     "Adv. type. 1: ADV_IND; 2:ADV_TYPE_ADV_NONCONN_IND.") \
        X(AT_COMMAND_ADV_ENABLE,        "AT+ENADV",       "Enable adv. (adv data = scan rsp data = 31 bytes.)") \
        X(AT_COMMAND_ADV_DISABLE,       "AT+DISADV",      "Disable adv.") \
        X(AT_COMMAND_SCAN_TYPE,         "AT+SCANTYPE",    "Scan type. 1:SCAN_TYPE_PASSIVE; 2:SCAN_TYPE_ACTIVE.") \
        X(AT_COMMAND_SCAN_INT_WINDOW,   "AT+SCANINTWIN",  "Scan interval and window in ms. 1: int=win=20; 2: int=40, win=20.") \
        X(AT_COMMAND_SCAN_ENABLE,       "AT+ENSCAN",      "Enable scan.") \
        X(AT_COMMAND_SCAN_DISABLE,      "AT+DISSCAN",     "Disable scan.") \
        X(AT_COMMAND_CONN_INTERVAL,     "AT+CONINT",      "Connection interval in ms. 1:7.5; 2:30; 3:1000.") \
        X(AT_COMMAND_CREATE_CONN,       "AT+CRCON",       "Create connection.") \
        X(AT_COMMAND_CANCEL_CONN,       "AT+CCCON",       "Cancel create connection.") \
        X(AT_COMMAND_DISCONN,           "AT+DISCON",      "Terminate the connection.") \
        X(AT_COMMAND_RF_PHY,            "AT+PHY",         "PHY. 1:BLE_PHY_1M; 2:BLE_PHY_2M; 3:BLE_PHY_CODED_S2; 4:BLE_PHY_CODED_S8.") \
        X(AT_COMMAND_NONE,              "NONE",           "")


#define X(a, b, c) a,

/** Data Rate Command Identification in ENUM
*/
typedef enum CTRL_CMD_EValue
{
    CTRL_CMD_TABLE
} CTRL_CMD_EValue;
#undef X

#define X(a, b, c) b,
/** Data Rate Command Identification in String to a Table
*/
static char *ctrl_cmd_table[] = {CTRL_CMD_TABLE};
#undef X

#define X(a, b, c) c,
/** Data Rate Command Identification Description to a Table
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

