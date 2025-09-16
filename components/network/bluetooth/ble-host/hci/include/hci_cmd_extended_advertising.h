/** @file hci_cmd_extended_advertising.h
 *
 * @defgroup hci_cmd_extend_adv HCI Extended Advertising Related Commands APIs
 * @{
 * @ingroup hci_cmd
 *
 * @brief Includes LE Controller commands (OGF = 0x08):
 *        - OCF = 0x0035 : LE Set Advertising set random address command
 *        - OCF = 0x0036 : LE Set extended advertising parameters command
 *        - OCF = 0x0037 : LE Set extended advertising data command
 *        - OCF = 0x0038 : LE Set extended scan response data command
 *        - OCF = 0x0039 : LE Set extended advertising enable command
 *        - OCF = 0x003A : LE Set read maximum advertising data length command
 *        - OCF = 0x003B : LE Set read number of supported advertising set command
 *        - OCF = 0x003C : LE Set remove advertising set command
 *        - OCF = 0x003D : LE Set clear advertising sets command
 *        - OCF = 0x003E : LE Set periodic advertising parameters command
 *        - OCF = 0x003F : LE Set periodic advertising data command
 *        - OCF = 0x0040 : LE Set periodic advertising enable command
 *        - OCF = 0x0044 : LE Set periodic advertising create sync command
 *        - OCF = 0x0045 : LE Set periodic advertising create sync cancel command
 *        - OCF = 0x0046 : LE Set periodic advertising terminate sync command
 *        - OCF = 0x0047 : LE Add device to periodic advertiser list command
 *        - OCF = 0x0048 : LE Remove device from periodic advertiser list command
 *        - OCF = 0x0049 : LE Clear periodic advertiser list command
 *        - OCF = 0x004A : LE Read periodic advertiser list size command
 *        - OCF = 0x0059 : LE Set periodic advertising receive enable command
 *        - OCF = 0x005A : LE periodic advertising sync transfer command
 *        - OCF = 0x005B : LE periodic advertising set info transfer command
 *        - OCF = 0x005C : LE Set periodic advertising sync transfer parameters command
 *        - OCF = 0x005D : LE Set default periodic advertising sync transfer parameters command
 *        - OCF = 0x0082 : LE Set periodic advertising subevent data command
 *        - OCF = 0x0083 : LE Set periodic advertising response data command
 *        - OCF = 0x0084 : LE Set periodic sync subevent command
 *        - OCF = 0x0086 : LE Set periodic advertising parameters v2 command
 * @}
 */

#ifndef __HCI_CMD_Extended_ADVERTISING_H__
#define __HCI_CMD_Extended_ADVERTISING_H__

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
/** LE Set Advertising set random address command
 *
 * @ingroup hci_cmd_adv
 *
 * @param p_param : a pointer to the set advertising set random address parameter.
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_set_adv_set_random_addr_cmd(ble_hci_cmd_set_adv_set_random_addr_t *p_param);


/** LE Set extended advertising parameters command
 *
 * @ingroup hci_cmd_adv
 *
 * @param p_param : a pointer to the set extended advertising parameters.
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_set_extended_adv_param_cmd(ble_hci_cmd_set_extended_adv_param_t *p_param);


/** LE Set extended advertising data command
 *
 * @ingroup hci_cmd_adv
 *
 * @param p_param : a pointer to the set extended advertising data parameters.
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_set_extended_adv_data_cmd(ble_hci_cmd_set_extended_adv_data_t *p_param);


/** LE Set extended advertising enable command
 *
 * @ingroup hci_cmd_adv
 *
 * @param p_param : a pointer to the set extended advertising enable parameters.
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_set_extended_adv_enable_cmd(ble_hci_cmd_set_extended_adv_enable_t *p_param);


/** LE read maximum advertising data length command
 *
 * @ingroup hci_cmd_adv
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_read_max_adv_data_len_cmd(void);


/** LE read number of supported advertising sets command
 *
 * @ingroup hci_cmd_adv
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_read_num_of_adv_sets_cmd(void);


/** LE remove advertising set command
 *
 * @ingroup hci_cmd_adv
 *
 * @param p_param : a pointer to the remove advertising set parameters.
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_remove_adv_set_cmd(ble_hci_cmd_remove_adv_set_t *p_param);


/** LE clear advertising sets command
 *
 * @ingroup hci_cmd_adv
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_clear_adv_sets_cmd(void);


/** LE set periodic advertising parameters command
 *
 * @ingroup hci_cmd_extend_adv
 *
 * @param p_param : a pointer to the periodic advertising parameters.
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_set_periodic_adv_param_cmd(ble_hci_set_periodic_adv_param_v2_t *p_param);


/** LE set periodic advertising data command
 *
 * @ingroup hci_cmd_extend_adv
 *
 * @param p_param : a pointer to the periodic advertising data parameters.
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_set_periodic_adv_data_cmd(ble_hci_cmd_set_periodic_adv_data_t *p_param);


/** LE set periodic advertising enable command
 *
 * @ingroup hci_cmd_extend_adv
 *
 * @param p_param : a pointer to the periodic advertising enable parameters.
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_set_periodic_adv_enable_cmd(ble_hci_cmd_set_periodic_adv_enable_t *p_param);


/** LE set periodic advertising create sync command
 *
 * @ingroup hci_cmd_extend_adv
 *
 * @param p_param : a pointer to the periodic advertising create sync parameters.
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_set_periodic_create_sync_cmd(ble_hci_cmd_periodic_adv_create_sync_param_t *p_param);


/** LE set periodic advertising create sync cancel command
 *
 * @ingroup hci_cmd_extend_adv
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_set_periodic_create_sync_cancel_cmd(void);


/** LE set periodic advertising terminate sync command
 *
 * @ingroup hci_cmd_extend_adv
 *
 * @param p_param : a pointer to the periodic advertising terminate sync parameters.
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_set_periodic_terminate_sync_cmd(ble_hci_cmd_periodic_adv_terminate_sync_param_t *p_param);


/** LE Add device to periodic advertiser list command
 *
 * @ingroup hci_cmd_extend_adv
 *
 * @param p_param : a pointer to add the periodic advertiser list parameters.
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_add_device_to_periodic_advertiser_list_cmd(ble_hci_add_device_to_periodic_adv_list_param_t *p_param);


/** LE Clear periodic advertiser list command
 *
 * @ingroup hci_cmd_extend_adv
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_clear_periodic_advertiser_list_cmd(void);


/** LE Read periodic advertiser list size command
 *
 * @ingroup hci_cmd_extend_adv
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_read_periodic_adv_list_size_cmd(void);


/** LE Set periodic advertising receive enable command
 *
 * @ingroup hci_cmd_extend_adv
 *
 * @param p_param : a pointer to the periodic advertising receive enable parameters.
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_set_periodic_adv_receive_enable_cmd(ble_hci_set_periodic_adv_receive_en_param_t *p_param);


/** LE periodic advertising sync transfer command
 *
 * @ingroup hci_cmd_extend_adv
 *
 * @param p_param : a pointer to the periodic advertising sync transfer parameters.
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_periodic_adv_sync_transfer_cmd(ble_hci_periodic_adv_sync_transfer_param_t *p_param);


/** LE periodic advertising set info transfer command
 *
 * @ingroup hci_cmd_extend_adv
 *
 * @param p_param : a pointer to the periodic advertising set info transfer parameters.
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_periodic_adv_set_info_transfer_cmd(ble_hci_periodic_adv_set_info_transfer_param_t *p_param);


/** LE set periodic advertising sync transfer parameters command
 *
 * @ingroup hci_cmd_extend_adv
 *
 * @param p_param : a pointer to set the periodic advertising sync transfer parameters.
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_set_periodic_adv_sync_transfer_param_cmd(ble_hci_set_periodic_adv_sync_transfer_param_t *p_param);


/** LE set default periodic advertising sync transfer parameters command
 *
 * @ingroup hci_cmd_extend_adv
 *
 * @param p_param : a pointer to the default periodic advertising sync transfer parameters.
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_default_periodic_adv_sync_transfer_param_cmd(ble_hci_default_periodic_adv_sync_transfer_t *p_param);


/** LE set periodic advertising subevent data command
 *
 * @ingroup hci_cmd_extend_adv
 *
 * @param p_param : a pointer to the periodic advertising subevent data parameters.
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_set_periodic_adv_subevent_data_cmd(ble_hci_set_periodic_adv_subevent_data_t *p_param);


/** LE set periodic advertising response data command
 *
 * @ingroup hci_cmd_extend_adv
 *
 * @param p_param : a pointer to the periodic advertising response data parameters.
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_set_periodic_adv_rsp_data_cmd(ble_hci_set_periodic_adv_rsp_data_t *p_param);


/** LE set periodic sync subevent command
 *
 * @ingroup hci_cmd_extend_adv
 *
 * @param p_param : a pointer to the set periodic advertising sync subevent parameters.
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_set_periodic_sync_subevent_cmd(ble_hci_set_periodic_sync_subevent_t *p_param);


/** LE set periodic advertising parameters v2 command
 *
 * @ingroup hci_cmd_extend_adv
 *
 * @param p_param : a pointer to the periodic advertising parameters v2.
 *
 * @return BLE_ERR_OK   : success.
 * @return BLE_ERR_DATA_MALLOC_FAIL  : failed to malloc memory.
 * @return BLE_ERR_HCI_TX_CMD_QUEUE_FULL : failed to tx data queue full.
 */
ble_err_t hci_le_set_periodic_adv_param_v2_cmd(ble_hci_set_periodic_adv_param_v2_t *p_param);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __HCI_CMD_EXTENDED_ADVERTISING_H__ */
