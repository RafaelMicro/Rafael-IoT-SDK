/** @file ble_host_cmd.h
 *
 * @addtogroup BLE
 * @{
 * @addtogroup ble_host BLE Host
 * @{
 * @defgroup ble_host_api BLE Host APIs
 * @}
 * @}
 */
#ifndef __BLE_BLE_HOST_CMD_H__
#define __BLE_BLE_HOST_CMD_H__


/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "ble_att_gatt.h"
#include "ble_hci.h"
#include "ble_security_manager.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/
/** @brief  Define BLE Host event callback.
 * @ingroup ble_host_api
 */
typedef ble_err_t (*ble_host_callback_t)(void* p_param);


/** @brief  Define BLE Application event type.
 * @ingroup ble_host_api
 */
typedef enum
{
    APP_GENERAL_EVENT,            /**< Queue type: BLE event. */
    APP_SERVICE_EVENT,            /**< Queue type: BLE service data. */
    APP_L2CAP_DATA_EVENT,         /**< Queue type: BLE L2CAP data. */
} ble_app_event_t;


/** @defgroup ble_host_timer BLE Host Timer Type Definition
 * @{
 * @ingroup ble_host_api
 * @brief  Define BLE Host timer event type.
 */
typedef uint8_t host_timer_evt;
#define TIMER_EVENT_NULL                                 0xFF                           /**< Null event.*/
#define TIMER_EVENT_CONN_PARAMETER_UPDATE_RSP            0x00                           /**< Host sw timer event for connection update parameter update response.*/
#define TIMER_EVENT_CONN_UPDATE_COMPLETE                 0x01                           /**< Host sw timer event for connection update complete.*/
#define TIMER_EVENT_AUTH_STATUS                          0x02                           /**< Host sw timer event for authentication status.*/
#define TIMER_EVENT_CLIENT_DB_PARSING                    0x03                           /**< Host sw timer event for database parsing.*/
#define TIMER_EVENT_GATT_COMMAND                         0x04                           /**< Host sw timer event for gatt command response*/
#define TOTAL_TIMER_EVENT                                (TIMER_EVENT_GATT_COMMAND+1)   /**< Total sw timer event number.*/
/** @} */

#define SYNC_ACTIVE                             0x00
#define PADV_ACTIVE                             0x01

#define BLE_MAX_SYNC_HANDLE                     0x0002
#define BLE_MAX_PADV_HANDLE                     0x02
/**************************************************************************
 * EXTERN DEFINITIONS
 **************************************************************************/
/** @brief  pointer of the BLE event callback.
 * @ingroup ble_host_api
 */
extern ble_host_callback_t g_ble_event_cb;


/**************************************************************************************************
 *    PUBLIC FUNCTIONS
 *************************************************************************************************/

/** @brief Bluetooth LE Host event callback parameter initialization.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  callback_type : Type of the callback function.
 * @param[in]  ble_host_callback_t : pointer of the type callback.
 *
 * @return @ref BLE_ERR_OK is success or an @ref ble_err_t "error".
 */
ble_err_t ble_host_callback_set(ble_app_event_t callback_type, ble_host_callback_t pfn_callback);


/** @brief Get the number of BLE connected links.
 *
 * @ingroup ble_host_api
 *
 * @return connected links.
 */
uint8_t bhc_host_connected_link_number_get(void);


/**@brief  Check host ID is valid or not.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  host_id : the link's host id.
 *
 * @retval  true : the host id is in the range.
 * @retval  false: the host id is invalid.
 */
bool bhc_host_id_is_valid_check(uint8_t host_id);


/** @brief Check host ID is connected to the device or not.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  host_id : the link's host id.
 * @param[out] conn_id : the link's connection id.
 *
 * @retval  true : the host id is connected.
 * @retval  false: the host id is free.
 */
bool bhc_host_id_is_connected_check(uint8_t host_id, uint16_t *conn_id);


/** @brief Check host ID is in the active or not.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  host_id : the link's host id.
 *
 * @retval  true : the host id is connected.
 * @retval  false: the host id is free.
 */
bool bhc_host_id_is_active_check(uint8_t host_id);


/** @brief The BLE host ID is set to active mode.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  host_id : the link's host id.
 * @param[in]  role : GAP role, @ref ble_gap_role_t.
 *
 * @return none.
 */
void bhc_host_id_state_active_set(uint8_t host_id, uint8_t role);


/** @brief The BLE host ID is set to idle mode.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  role : GAP role, @ref ble_gap_role_t.
 *
 * @return none.
 */
void bhc_host_id_state_active_release(uint8_t role);


/** @brief Check host DB parsing or encryption process is finished or not.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  host_id : the link's host id.
 *
 * @retval  true : the host is processing DB parsing or encryption.
 * @retval  false: the host is ready.
 */
bool bhc_host_parsing_process_is_finished_check(uint8_t host_id);


/** @brief Check the host is in encryption process or not.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  host_id : the link's host id.
 *
 * @retval  true : the host is busy.
 * @retval  false: the host is ready.
 */
bool bhc_host_is_in_encryption_check(uint8_t host_id);


/** @brief Check the GATT response is received or not.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  host_id : the link's host id.
 *
 * @retval  true : the host is waiting for the GATT response.
 * @retval  false: the host is ready.
 */
bool bhc_host_is_wating_gatt_rsp_check(uint8_t host_id);


/** @brief Check the BLE characteristic properties definition is defined or not.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  host_id    : the link's host id.
 * @param[in]  handle_num : attribute handle number.
 * @param[in]  property   : @ref bleCharPropertie.
 *
 * @retval  true : the definition is match.
 * @retval  false: the definition is not match.
 */
bool bhc_client_property_value_is_match_check(uint8_t host_id, uint16_t handle_num, uint8_t property);


/** @brief Check the BLE server characteristic properties definition is defined or not.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  host_id    : the link's host id.
 * @param[in]  handle_num : attribute handle number.
 * @param[in]  property   : @ref bleCharPropertie, NULL means only check if it is server role.
 *
 * @retval  true : the definition is match.
 * @retval  false: the definition is not match.
 */
bool bhc_server_property_value_is_match_check(uint8_t host_id, uint16_t handle_num, uint8_t property);


/** @brief  Set GATT preferred ATT_MTU size.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  host_id        : the link's host id.
 * @param[in]  preferred_mtu  : preferred ATT_MTU size.
 *
 * @retval  true : successful.
 * @retval  false: the host is in invalid state.
 */
bool bhc_gatt_preferred_mtu_set(uint8_t host_id, uint16_t preferred_mtu);


/** @brief Get ATT_MTU size.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  host_id : the link's host id.
 *
 * @return  ATT_MTU size.
 */
uint16_t bhc_gatt_att_mtu_get(uint8_t host_id);


/** @brief  Send ATT request.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  conn_id    : the link's connection id.
 * @param[in]  att_opcode : @ref ble_att_opcode_t "attribute opcode" .
 * @param[in]  handle_num : attribute handle number, set to "NULL" if att_opecode is @ref OPCODE_ATT_EXCHANGE_MTU_REQUEST.
 * @param[in]  p_data     : a point to the data.
 * @param[in]  length     : data length.
 * @retval  true : successful.
 * @retval  false: the queue is full or error handle number.
 */
int8_t bhc_att_req(uint16_t conn_id, uint8_t att_opcode, uint16_t handle_num, uint8_t *p_data, uint16_t length);


/** @brief  Send ATT error response request.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  conn_id    : the link's connection id.
 * @param[in]  att_opcode : @ref ble_att_opcode_t "attribute opcode" .
 * @param[in]  handle_num : attribute handle number, set to "NULL" if att_opecode is @ref OPCODE_ATT_EXCHANGE_MTU_REQUEST.
 * @param[in]  error_code : error code.
 *
 * @retval  true : successful.
 * @retval  false: the queue is full.
 */
int8_t bhc_att_error_rsp_req(uint16_t conn_id, uint8_t att_opcode, uint16_t handle_num, ble_att_error_code_t error_code);


/** @brief Get BLE hOST timer event status.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  host_id : the link's host id.
 * @param[in]  event : select the host timer event, refer to @ref ble_host_timer.
 *
 * @return  host timer status.
 */
uint8_t bhc_timer_evt_get(uint8_t host_id, host_timer_evt event);


/** @brief BLE Host transmit the connection parameter update request.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  conn_id : the link's host id.
 * @param[in]  p_param : a pointer to connection parameter.
 *
 * @retval  true : setting success.
 * @retval  false: event timer busy.
 */
ble_err_t bhc_gap_connection_update(uint16_t conn_id, ble_gap_conn_param_t *p_param);


/** @brief BLE Host send security request / pairing request.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  host_id : the link's host id.
 *
 * @return @ref BLE_ERR_OK is success or an @ref ble_err_t "error".
 */
ble_err_t bhc_sm_security_request(uint8_t host_id);


/** @brief Set security passkey.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  hex_passkey : the passkey in hex value.
 * @param[in]  conn_id : the link's connection id.
 *
 * @retval  true : always return true.
 */
int8_t bhc_sm_passkey_set(uint32_t hex_passkey, uint16_t conn_id);


/** @brief Set security numeric comparison result.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  same_numeric : numeric comparison result is same.
 * @param[in]  conn_id : the link's connection id.
 *
 * @retval  true : always return true.
 */
ble_err_t bhc_sm_numeric_comparison_set(uint8_t same_numeric, uint16_t conn_id);


/** @brief Set security parameters - IO capabilities.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  p_param : a pointer to the io capability parameter.
 *
 * @return none.
 */
void bhc_sm_io_caps_set(ble_sm_io_cap_param_t *p_param);


/** @brief Set security parameters - bonding flag.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  p_param : a pointer to the bonding flag parameter.
 *
 * @return none.
 */
void bhc_sm_bonding_flag_set(ble_sm_bonding_flag_param_t *p_param);


/** @brief BLE restore cccd value command.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  host_id : conn_id : the link's host id.
 *
 * @return @ref BLE_ERR_OK is success or an @ref ble_err_t "error".
 */
ble_err_t bhc_sm_restore_cccd_from_bond(uint8_t host_id);


/** @brief BLE Send Bonding space space init command.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  host_id : conn_id : the link's host id.
 *
 * @return @ref BLE_ERR_OK is success or an @ref ble_err_t "error".
 */
ble_err_t bhc_sm_bonding_space_init(void);


/** @brief The BLE set identity resolving key.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  p_param : a pointer to the identity reolving key parameter.
 *
 * @return none.
 */
ble_err_t bhc_sm_identity_resolving_key_set(ble_sm_irk_param_t *p_param);


/** @brief The BLE gen resolvable address.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  host_id : host id.
 *
 * @retval  0 : successful.
 * @retval  -1: the queue is full or memory not enough.
 */
int8_t bhc_host_gen_resolvable_address(uint8_t host_id);


/** @brief Get BLE GATT Attribute Handles Mapping Table.
 *
 * @ingroup ble_host_api
 *
 * @param[out] p_param : a point to handle mapping parameter.
 *
 * @return @ref BLE_ERR_OK is success or an @ref ble_err_t "error".
 */
ble_err_t ble_gatt_att_handle_mapping_get(ble_gatt_handle_table_param_t *p_param);


/** @brief Check irk key is exist or not.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  host_id : host id.
 *
 * @return @ref BLE_ERR_OK is success or an @ref ble_err_t "error".
 */
ble_err_t bhc_irk_key_exist_check(uint8_t host_id);


/** @brief Initial LESC related parameter.
 *
 * @ingroup ble_host_api
 *
 *
 * @return @ref BLE_ERR_OK is success or an @ref ble_err_t "error".
 */
ble_err_t ble_lesc_init(void);


/** @brief Initial Resolvable address related parameter.
 *
 * @ingroup ble_host_api
 *
 *
 * @return @ref BLE_ERR_OK is success or an @ref ble_err_t "error".
 */
ble_err_t bhc_resolvable_address_init(void);


/** @brief Regenerate Resolvable address related parameter.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  host_id : host id.
 * @param[in]  en_new_irk : enable new irk or not.
 * 
 * @return @ref BLE_ERR_OK is success or an @ref ble_err_t "error".
 */
ble_err_t bhc_regenerate_resolvable_address(uint8_t host_id, uint8_t en_new_irk);


/** @brief Send the le l2cap channel connect request.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  conn_id : connect id.
 * @param[in]  psm : Simplified Protocol/Service Multiplexer.
 * @param[in]  mtu : Maximum Transmission Unit.
 * 
 * @return @ref BLE_ERR_OK is success or an @ref ble_err_t "error".
 */
ble_err_t bhc_l2cap_chan_connect_reg(uint16_t conn_id, uint16_t psm, uint16_t mtu);


/** @brief Send the le l2cap data.
 *
 * @ingroup ble_host_api
 *
 * @param[in]  conn_id : connect id.
 * @param[in]  dest_id : destination channel id.
 * @param[in]  p_data : pointer to the l2cap data.
 * @param[in]  length : data length.
 * 
 * @return @ref BLE_ERR_OK is success or an @ref ble_err_t "error".
 */
ble_err_t bhc_l2cap_data_send(uint16_t conn_id, uint16_t dest_id, uint8_t* p_data, uint16_t length);


void bhc_host_id_state_active_enhanced_set(uint8_t host_id, uint16_t handle, uint8_t select);
void bhc_host_id_state_enhance_active_release(uint8_t select, uint16_t handle);
uint8_t bhc_host_id_state_enhance_active_get(uint8_t select, uint16_t handle);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __BLE_BLE_HOST_CMD_H__ */
