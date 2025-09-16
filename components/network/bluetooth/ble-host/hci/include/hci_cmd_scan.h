/** @file hci_cmd_scan.h
 *
 * @defgroup hci_cmd_scan HCI Scan Related Commands APIs
 * @{
 * @ingroup hci_cmd
 *
 * @brief Includes LE Controller commands (OGF = 0x08):
 *        - OCF = 0x000B : LE Set Scan Parameters command
 *        - OCF = 0x000C : LE Set Scan Enable command
 * @}
 */

#ifndef __HCI_CMD_SCAN_H__
#define __HCI_CMD_SCAN_H__

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <stdint.h>
#include "ble_hci.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**************************************************************************************************
 *    PUBLIC FUNCTIONS
 *************************************************************************************************/

/** LE Set Scan Parameters command.
 *
 * @ingroup hci_cmd_scan
 *
 * @param p_param : a pointer to the scan parameter.
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_set_scan_param_cmd(ble_hci_cmd_set_scan_param_param_t *p_param);


/** LE Set Scan Enable command.
 *
 * @ingroup hci_cmd_scan
 *
 * @param p_param : a pointer to the set scan enable parameter.
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_set_scan_enable_cmd(ble_hci_cmd_set_scan_enable_param_t *p_param);


/** LE Set Extended Scan Parameters command
 *
 * @ingroup hci_cmd_scan
 *
 * @param p_param : a pointer to the set extended scan parameter.
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_set_extended_scan_param_cmd(ble_hci_cmd_set_extended_scan_param_t *p_param);


/** LE Set Extended Scan Enable command
 *
 * @ingroup hci_cmd_scan
 *
 * @param p_param : a pointer to the set extended scan enable parameter.
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_set_extened_scan_enable_cmd(ble_hci_cmd_set_extended_scan_enable_t *p_param);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __HCI_CMD_SCAN_H__ */
