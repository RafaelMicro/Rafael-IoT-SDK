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
#define DATA_RATE_TOOL_VER  "v0.0.1"

/** Marcro Return Control Command Error String. */
#define PRINT_CTRL_CMD_ERROR(err)     (printf("ERROR:%s\r\n", err))

/**************************************************************************
 * Data Rate Command Identification Definitions
 **************************************************************************/

/** Data Rate Command Identification Definition
*/
#define CTRL_CMD_TABLE    \
        X(CMD_HELP,              "help",            "Help") \
        X(CMD_GET_PARAM,         "param",           "Get all parameters.") \
        X(CMD_LADDR,             "lAddr",           "Set local device address. (=112233445566 means 66:55:44:33:22:11)") \
        X(CMD_TADDR,             "tAddr",           "Set target device address.(=112233445566 means 66:55:44:33:22:11)") \
        X(CMD_CREATE_CON,        "createCon",       "Set create connection.") \
        X(CMD_CANCEL_CON,        "cancelCon",       "Set cancel the On-going connection.") \
        X(CMD_DISCON,            "disconnect",      "Terminate the connection.") \
        X(CMD_SEC_REQ,           "conEnc",          "Enable encrypted link. Re-connect to reset to unencrypted link.") \
        X(CMD_CONN_INTERVAL,     "conInt",          "Set connection interval in unit(1.25ms). (=6 means 7.5ms)") \
        X(CMD_RF_PHY,            "phy",             "Set PHY. =1:BLE_PHY_1M; =2:BLE_PHY_2M. =3:BLE_CODED_PHY_S2. =4:BLE_CODED_PHY_S8.") \
        X(CMD_DATA_LENGTH,       "dataLen",         "Set data length per packet, the range = 20~244") \
        X(CMD_TEST_TOTAL_TX_LEN, "testTxLen",       "Set total TX test length.") \
        X(CMD_TEST_MODE_SET,     "testMode",        "Set test mode. =1:TX; =2:RX.") \
        X(CMD_TEST_START_SET,    "testStart",       "Set to start data rate test.") \
        X(CMD_TEST_STOP_SET,     "testStop",        "Set to stop data rate test.") \
        X(CMD_CH_CLASSIF_SET,    "channelmapSet",   "Set channel map. (=FFFFFFFF1F channel all open.)") \
        X(CMD_CH_MAP_READ,       "channelmapRead",   "Read channel map.") \
        X(CMD_NONE,              "NONE",            "")


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
 * Structures
 **************************************************************************/
/* Running mode definition. */
typedef enum runnung_mode
{
    DATARATE_MODE_IDLE,
    DATARATE_MODE_RX,
    DATARATE_MODE_TX,
} runnung_mode;

/* Running state definition. */
typedef enum runnung_state
{
    DATARATE_STATE_IDLE,
    DATARATE_STATE_RUN,
    DATARATE_STATE_SETTING,
    DATARATE_STATE_RUNNING,
    DATARATE_STATE_WAITING,
} runnung_state;

/* Data rate test application information definition. */
typedef struct app_dataRateTest_info
{
    uint32_t      total_Tx_test_len;
    runnung_mode  test_mode;
    runnung_state test_state;
} app_dataRateTest_info;

typedef struct link0_connect_info
{
    uint16_t             conn_interval;           /**< Current link default connection interval. */
    ble_phy_t            phy;                     /**< Current link default RF PHY. */
    uint16_t             data_length;             /**< Current link default data length. */
} link0_connect_info;

/**************************************************************************
 * External Definitions
 **************************************************************************/
// BLE target address
extern ble_gap_addr_t g_target_addr;
extern link0_connect_info connect_info;

// BLE data rate test information
extern app_dataRateTest_info app_DR_test_info;

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

