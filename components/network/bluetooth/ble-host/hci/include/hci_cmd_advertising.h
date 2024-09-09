/** @file hci_cmd_advertising.h
 *
 * @defgroup hci_cmd_adv HCI Advertising Related Commands APIs
 * @{
 * @ingroup hci_cmd
 *
 * @brief Includes LE Controller commands (OGF = 0x08):
 *        - OCF = 0x0006 : LE Set Advertising Parameters command
 *        - OCF = 0x0008 : LE Set Advertising Data command
 *        - OCF = 0x0009 : LE Set Scan Response Data command
 *        - OCF = 0x000A : LE Set Advertising Enable command
 * @}
 */

#ifndef __HCI_CMD_ADVERTISING_H__
#define __HCI_CMD_ADVERTISING_H__

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "ble_hci.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**************************************************************************************************
 *    PUBLIC FUNCTIONS
 *************************************************************************************************/

/** LE Set Advertising Parameters command.
 *
 * @ingroup hci_cmd_adv
 *
 * @param p_param : a pointer to the advertising parameter.
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
int8_t hci_le_set_adv_param_cmd(ble_hci_cmd_set_adv_param_param_t *p_param);


/** LE Set Advertising Data command.
 *
 * @ingroup hci_cmd_adv
 *
 * @param p_param : a pointer to the advertising data.
 *
 * @return BLE_ERR_OK   : success.
 * @return ERR_MEM  : failed to send HCI command.
 */
int8_t hci_le_set_adv_data_cmd(ble_hci_cmd_set_adv_data_param_t *p_param);


/** LE Set Scan Response Data command.
 *
 * @ingroup hci_cmd_adv
 *
 * @param p_param : a pointer to the scan response data.
 *
 * @return BLE_ERR_OK   : success.
 * @return ERR_MEM  : failed to send HCI command.
 */
int8_t hci_le_set_scan_rsp_data_cmd(ble_hci_cmd_set_scan_rsp_param_t *p_param);


/** LE Set Advertising Enable command.
 *
 * @ingroup hci_cmd_adv
 *
 * @param p_param : a pointer to the set advertising enable parameter.
 *
 * @return BLE_ERR_OK   : success.
 * @return ERR_MEM  : failed to send HCI command.
 */
int8_t hci_le_set_adv_enable_cmd(ble_hci_cmd_set_adv_enable_param_t *p_param);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __HCI_CMD_ADVERTISING_H__ */
