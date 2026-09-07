/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

#ifndef __BLE_APP_H__
#define __BLE_APP_H__

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <stdint.h>
#include "ble_api.h"


/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/
#define ESL_AP_ADV_HANDLE   0x00
#define MAX_ESL_CTRL_POINT_GROUP_NUM   0x01
#define MAX_RSP_SLOT 70
#define MAX_SUBEVENT_NUM 15
#define APP_ESL_AP_C_HOST_ID             0 // ESL_AP_C: Central

/**
 * @brief Defines the types of parameters carried in the application queue.
 * @details This enumeration identifies which member of the @ref app_queue_t::param
 *          union is valid for a given queue entry.
 */
typedef enum
{
    QUEUE_TYPE_APP_REQ,             /**< Application queue type: application request.*/
    QUEUE_TYPE_SUBEVENT_DATA_SET,   /**< Application queue type: set subevent data.*/
    QUEUE_TYPE_OTHERS,              /**< Application queue type: others including BLE events and BLE service events.*/
    QUEUE_TYPE_UART_MSG,            /**< Application queue type: uart message.*/
} app_queue_param_type_t;

/**
 * @brief Enumeration of application request events.
 * @details These are the requests posted onto the application queue (via @ref app_request_set)
 *          to be handled by the central (ESL AP) link's request handler.
 */
typedef enum
{
    APP_REQUEST_IDLE,               /**< Application request event: idle.*/
    APP_REQUEST_SCAN_START,         /**< Application request event: scanning start.*/
    APP_REQUEST_SCAN_STOP,          /**< Application request event: scanning stop.*/
    APP_REQUEST_CREATE_CONN,        /**< Application request event: create connection.*/
    APP_REQUEST_ESL_AP_MULTI_CMD,   /**< Application request event: run/continue the post-connection GATT configuration sequence (MTU/data length exchange, CCCD setup, characteristic reads, L2CAP channel connect).*/
    APP_REQUEST_SEND_L2CAP_DATA,    /**< Application request event: send/continue the pending L2CAP data transfer.*/
} app_request_t;

/**
 * @brief Enumeration of BLE link states.
 * @details Tracks the state of the central (ESL AP) link stored in ble_app_link_info[].state.
 */
typedef enum
{
    STATE_STANDBY,                  /**< Application state: standby.*/
    STATE_ADVERTISING,              /**< Application state: advertising.*/
    STATE_SCANNING,                 /**< Application state: scanning.*/
    STATE_INITIATING,               /**< Application state: initialing.*/
    STATE_CONNECTED,                /**< Application state: connected.*/
} ble_state_t;

/**
 * @brief Structure for application request parameters.
 * @details This structure holds the parameters for application requests.
 */
typedef struct
{
    uint8_t   host_id;    /**< Application request parameter: host id.*/
    uint16_t  app_req;    /**< Application request parameter: @ref app_request_t "application request event".*/
} app_req_param_t;

/**
 * @brief Structure representing one entry of the application queue.
 * @details This structure holds the details for events and requests in the application queue.
 */
typedef struct
{
    uint8_t   event;        /**< Application queue parameter: event.*/
    uint8_t   from_isr;     /**< Application queue parameter: Dose the Request come from interruption? */
    uint16_t  param_type;   /**< Application queue parameter: @ref app_queue_param_type_t "application queue type".*/
    union
    {
        app_req_param_t  app_req;   /**< Application queue parameter: application request event. */
        ble_tlv_t        *pt_tlv;   /**< Application queue parameter: parameters (type: @ref ble_app_event_t, length, and value). */
    } param;
} app_queue_t;

/**
 * @brief Structure describing a set of ESL Control Point commands to broadcast over encrypted periodic advertising.
 * @details Passed to @ref esl_control_point_cmd_send_over_encrypt_adv. Each entry in group_cmd[] is encrypted
 *          and carried as the encrypted-advertising-data payload of one PAwR subevent.
 */
typedef struct
{
    uint8_t group_num;   /**< Number of groups (subevents) filled in group_cmd[], must not exceed MAX_ESL_CTRL_POINT_GROUP_NUM.*/
    struct
    {
        uint8_t group_id;   /**< Subevent (ESL group) index the command is broadcast to.*/
        uint8_t cmd_num;    /**< Number of response slots reserved for this subevent.*/
        uint8_t cmd_len;    /**< Length in bytes of the command pointed to by p_cmd.*/
        uint8_t *p_cmd;     /**< Pointer to the raw ESL Control Point command payload to encrypt and send.*/
    } group_cmd[MAX_ESL_CTRL_POINT_GROUP_NUM];   /**< Per-group (per-subevent) command entries.*/
} esl_control_point_cmd_send_t;

/**
 * @brief Structure holding the provisioning data written to a newly connected ESL device.
 * @details Passed to @ref ap_config_esl_device to configure the ESL's address, key material,
 *          and current absolute time right after connection.
 */
typedef struct
{
    uint16_t esl_id;             /**< ESL ID (address) to assign to the connected device.*/
    uint8_t  *p_ap_sync_key;     /**< Pointer to the 24-byte AP synchronization key material (16-byte key + 8-byte nonce) to provision.*/
    uint8_t  *p_esl_rsp_key;     /**< Pointer to the 24-byte ESL response key material (16-byte key + 8-byte nonce) to provision.*/
    uint32_t current_time;       /**< Current absolute time value to write to the ESL's absolute time characteristic.*/
} ap_config_esl_device_t;

/**
 * @brief Structure holding the Periodic Advertising with Responses (PAwR) parameters used to start synchronization.
 * @details Passed to @ref ap_sync_start to configure and enable periodic/extended advertising for the ESL AP role.
 */
typedef struct
{
    uint16_t padv_interval;      /**< periodic ADV interval(unit: 1.25ms) */
    uint8_t  num_subevents;      /**< number of subevents. */
    uint8_t  subevent_interval;  /**< interval between subevents (unit: 1.25ms). */
    uint8_t  rsp_slot_delay;     /**< time between adv packet and the first response slot (unit: 1.25ms). */
    uint8_t  rsp_slot_spacing;   /**< time between response slots (unit: 0.125ms). */
    uint8_t  num_rsp_slot;       /**< number of subevent response slots. */
} ap_sync_start_t;

/**
 * @brief Structure tracking the state of an in-progress outbound L2CAP data transfer.
 * @details A single global instance (g_l2cap_data) is used to fragment and send one buffer
 *          across successive L2CAP CoC packets as credits become available.
 */
typedef struct __attribute__((packed)) l2cap_data_tx_param_s
{
    uint8_t *p_data_buffer;   /**< Pointer to the allocated buffer holding the data being transmitted.*/
    uint8_t curr_credit;      /**< Number of L2CAP CoC credits currently available for transmission.*/
    uint16_t dest_id;         /**< Destination L2CAP channel/CID negotiated for the connection-oriented channel.*/
    uint16_t total_tx_len;    /**< Total length in bytes of the data to be transmitted.*/
    uint16_t curr_tx_len;     /**< Number of bytes already transmitted out of total_tx_len.*/
    uint16_t mtu_size;        /**< Negotiated L2CAP MTU/MPS size, used to size each fragment.*/
} l2cap_data_tx_param_t;

/**
 * @brief Structure representing one entry of the FOTA command queue.
 * @details Used to remember, while waiting for the FOTA server's command indication, which OTA
 *          command opcode (e.g. OTA_CMD_QUERY/START/ERASE/APPLY) is currently pending a response.
 */
typedef struct
{
    uint8_t   cmd_id;    /**< Pending OTA command opcode awaiting an indication response from the FOTA server.*/
} app_fota_cmd_queue_t;

extern uint8_t esl_ap_sync_key[16];
extern uint8_t esl_ap_nonce[8];
extern uint8_t esl_esl_rsp_key[16];
extern uint8_t esl_rsp_nonce[8];
extern const ble_gap_addr_t  DEVICE_ADDR;
extern l2cap_data_tx_param_t g_l2cap_data;

/**************************************************************************************************
 *    PUBLIC FUNCTIONS
 *************************************************************************************************/
/**
 * @brief Queues an application-level request for the given link.
 *
 * Builds a @ref app_queue_t entry of type QUEUE_TYPE_APP_REQ and posts it to the application
 * message queue so it will be handled by app_main_task() / ble_app_main(). A counting semaphore
 * (semaphore_app for normal context, semaphore_isr for ISR context) is used to bound the number
 * of outstanding requests; the semaphore is released again if the queue send fails.
 *
 * @param host_id Link (host) identifier the request applies to.
 * @param request Application request event to queue (see @ref app_request_t).
 * @param from_isr Set to true when called from interrupt context (uses the ISR-safe FreeRTOS APIs
 *                 and the ISR request semaphore); false for normal task context.
 * @return true if the request was successfully queued.
 * @retval false No free request semaphore slot was available, or the application queue was full.
 */
bool app_request_set(uint8_t host_id, app_request_t request, bool from_isr);

/**
 * @brief Copies data into the L2CAP transmit buffer and starts a new outbound transfer.
 *
 * Allocates a buffer of data_len bytes, copies p_l2cap_data into it, and resets the transfer's
 * length bookkeeping (g_l2cap_data.total_tx_len/curr_tx_len) so svcs_esl_send_l2cap_data_handler()
 * can begin sending it in MTU-sized fragments.
 *
 * @param p_l2cap_data Pointer to the source data to be transmitted over the L2CAP channel.
 * @param data_len Length in bytes of the data to transmit.
 * @return ble_err_t Result of the operation.
 * @retval BLE_ERR_OK Data buffer allocated and copied successfully.
 * @retval BLE_ERR_INVALID_PARAMETER data_len exceeds APP_L2CAP_BUFFER_SIZE.
 * @retval BLE_BUSY A previous transfer's buffer has not finished sending yet.
 * @retval BLE_ERR_DATA_MALLOC_FAIL Failed to allocate the transmit buffer.
 */
ble_err_t esl_update_l2cap_data(uint8_t *p_l2cap_data, uint16_t data_len);

/**
 * @brief Sends a raw command to the peer's ESL Control Point characteristic.
 *
 * Issues a GATT write request carrying p_control_point_cmd to the ESL Service's Control Point
 * handle on the connected central (ESL AP) link.
 *
 * @param p_control_point_cmd Pointer to the ESL Control Point command payload.
 * @param cmd_len Length in bytes of the command payload.
 * @return ble_err_t Result returned by ble_svcs_data_send() (BLE_ERR_OK on success, otherwise
 *                    the underlying GATT write error).
 */
ble_err_t esl_control_point_cmd_send(uint8_t *p_control_point_cmd, uint8_t cmd_len);

/**
 * @brief Sends a raw command to the peer's Object Action Control Point (OACP) characteristic.
 *
 * Issues a GATT write request carrying p_control_point_cmd to the Object Transfer Service's
 * Object Action Control Point handle on the connected central (ESL AP) link.
 *
 * @param p_control_point_cmd Pointer to the OACP command payload.
 * @param cmd_len Length in bytes of the command payload.
 * @return ble_err_t Result returned by ble_svcs_data_send() (BLE_ERR_OK on success, otherwise
 *                    the underlying GATT write error).
 */
ble_err_t object_action_control_point_cmd_send(uint8_t *p_control_point_cmd, uint8_t cmd_len);

/**
 * @brief Sends a raw command to the peer's Object List Control Point (OLCP) characteristic.
 *
 * Issues a GATT write request carrying p_control_point_cmd to the Object Transfer Service's
 * Object List Control Point handle on the connected central (ESL AP) link.
 *
 * @param p_control_point_cmd Pointer to the OLCP command payload.
 * @param cmd_len Length in bytes of the command payload.
 * @return ble_err_t Result returned by ble_svcs_data_send() (BLE_ERR_OK on success, otherwise
 *                    the underlying GATT write error).
 */
ble_err_t object_list_control_point_cmd_send(uint8_t *p_control_point_cmd, uint8_t cmd_len);

/**
 * @brief Broadcasts one or more ESL Control Point commands as encrypted periodic advertising data.
 *
 * Builds an AES-CCM encrypted "Encrypted Advertising Data" AD structure (using esl_ap_sync_key /
 * esl_ap_nonce) for each group in p_esl_cmd->group_cmd[], packs them into a periodic advertising
 * subevent data set, and queues it (QUEUE_TYPE_SUBEVENT_DATA_SET) so app_main_task() can hand it
 * to the controller via ble_cmd_periodic_adv_subevent_data_set().
 *
 * @param p_esl_cmd Pointer to the group/command list to broadcast (see @ref esl_control_point_cmd_send_t).
 * @return ble_err_t Result of the operation.
 * @retval BLE_ERR_OK Subevent data built and queued successfully.
 * @retval BLE_ERR_INVALID_PARAMETER p_esl_cmd->group_num exceeds MAX_ESL_CTRL_POINT_GROUP_NUM.
 * @retval BLE_ERR_ALLOC_MEMORY_FAIL Failed to allocate the subevent data buffer or the encryption scratch buffer.
 * @retval BLE_ERR_SEMAPHORE_MALLOC_FAIL No free subevent queue slot was available.
 * @retval BLE_BUSY Failed to post the built subevent data onto the application queue.
 */
ble_err_t esl_control_point_cmd_send_over_encrypt_adv(esl_control_point_cmd_send_t *p_esl_cmd);

/**
 * @brief Stops periodic advertising used for ESL synchronization (PAwR).
 *
 * Issues ble_cmd_periodic_adv_disable_set() for the ESL AP advertising set (ESL_AP_ADV_HANDLE).
 *
 * @return ble_err_t Result of the operation.
 * @retval BLE_ERR_ALLOC_MEMORY_FAIL Failed to allocate the HCI command parameter buffer.
 * @retval BLE_ERR_OK Periodic advertising disabled successfully.
 * @retval (other) ble_err_t code returned by ble_cmd_periodic_adv_disable_set() on failure.
 */
ble_err_t ap_sync_stop(void);

/**
 * @brief Configures and starts extended + periodic advertising (PAwR) for the ESL AP role.
 *
 * Sets the extended advertising parameters and random address for ESL_AP_ADV_HANDLE, applies the
 * periodic advertising (subevent/response-slot) parameters from p_config_device, enables periodic
 * advertising, then enables extended advertising so PAwR trains start being broadcast.
 *
 * @param p_config_device Pointer to the PAwR timing/response-slot configuration to apply (see @ref ap_sync_start_t).
 * @return ble_err_t Result of the operation.
 * @retval BLE_ERR_OK All advertising parameters were set and advertising was enabled successfully.
 * @retval BLE_ERR_ALLOC_MEMORY_FAIL Failed to allocate one of the HCI command parameter buffers.
 * @retval (other) ble_err_t code returned by the failing HCI command (parameter set / address set / enable).
 */
ble_err_t ap_sync_start(ap_sync_start_t *p_config_device);

/**
 * @brief Provisions a newly connected ESL device with its ID, key material, and current time.
 *
 * Writes, in order, the ESL ID, the AP synchronization key material, the ESL response key
 * material (both byte-reversed via _memcpy_inv before the trailing nonce is appended), and the
 * current absolute time to the corresponding ESL Service characteristics on the connected link.
 * Key-material and time writes are retried (busy-wait, up to ~6000 attempts with a 2-tick delay)
 * while the peer responds busy.
 *
 * @param p_config_device Pointer to the ESL ID, key material, and current time to provision (see @ref ap_config_esl_device_t).
 * @return ble_err_t Result of the operation.
 * @retval BLE_ERR_OK All characteristics were written successfully.
 * @retval BLE_ERR_ALLOC_MEMORY_FAIL Failed to allocate a write-data buffer.
 * @retval BLE_BUSY A write kept returning busy past the retry limit.
 * @retval (other) ble_err_t code returned by ble_svcs_data_send() for the ESL ID write.
 */
ble_err_t ap_config_esl_device(ap_config_esl_device_t *p_config_device);

/**
 * @brief Records the list of ESL IDs assigned to the response slots of a queued subevent.
 *
 * Pushes a new entry onto the internal subevent_data_list ring buffer, storing group_id and a
 * copy of the esl_cmd_num ESL IDs in p_esl_id, so that response slot indices reported later in a
 * PAwR response report can be translated back to ESL IDs.
 *
 * @param group_id Subevent (ESL group) index the ID list applies to.
 * @param esl_cmd_num Number of ESL IDs pointed to by p_esl_id (must be less than MAX_RSP_SLOT).
 * @param p_esl_id Pointer to the array of ESL IDs, indexed by response slot.
 * @return ble_err_t Result of the operation.
 * @retval BLE_ERR_OK Entry recorded successfully.
 * @retval BLE_ERR_INVALID_PARAMETER esl_cmd_num is not less than MAX_RSP_SLOT.
 * @retval BLE_BUSY The ring buffer is full (no free entry available).
 * @retval BLE_ERR_ALLOC_MEMORY_FAIL Failed to allocate storage for the ESL ID array.
 */
ble_err_t esl_add_control_point_esl_id_list(uint8_t group_id, uint8_t esl_cmd_num, uint8_t *p_esl_id);

/**
 * @brief Retrieves the ESL ID list previously recorded for a subevent.
 *
 * Reads (without advancing) the current entry at the head of the subevent_data_list ring buffer,
 * verifying it matches group_id, and copies its ESL ID array and count out to the caller. Used
 * while processing a PAwR response report to map response slots back to ESL IDs.
 *
 * @param group_id Subevent (ESL group) index expected to match the recorded entry.
 * @param p_esl_cmd_num Output: number of ESL IDs copied into p_esl_id.
 * @param p_esl_id Output buffer that receives the ESL ID array, indexed by response slot.
 * @return ble_err_t Result of the operation.
 * @retval BLE_ERR_OK ESL ID list retrieved successfully.
 * @retval BLE_ERR_INVALID_PARAMETER The recorded entry's group_id does not match group_id.
 */
ble_err_t esl_get_control_point_esl_id_list(uint8_t group_id, uint8_t *p_esl_cmd_num, uint8_t *p_esl_id);

/**
 * @brief Retires the subevent ID-list entry once its data has been transmitted.
 *
 * Called on BLE_PADV_EVT_PADV_SUBEVENT_DATA_REQ (controller ready for the next subevent data) to
 * advance the read index of the subevent_data_list ring buffer past the entry matching group_id.
 *
 * @param group_id Subevent (ESL group) index expected to match the entry being retired.
 * @return ble_err_t Result of the operation.
 * @retval BLE_ERR_OK Entry retired successfully.
 * @retval BLE_ERR_INVALID_PARAMETER The head entry's group_id does not match group_id.
 * @retval BLE_ERR_UNKNOW_TYPE The ring buffer is empty (no entry to retire).
 */
ble_err_t esl_update_control_point_esl_id_list(uint8_t group_id);

/**
 * @brief Records the destination ESL of an in-flight Control Point command.
 *
 * Packs group_id and esl_id into g_esl_control_point_cmd_dest_id so that when the corresponding
 * ESL Control Point notification arrives, esl_get_control_point_esl_id() can identify its source.
 *
 * @param group_id Subevent (ESL group) index of the command's destination.
 * @param esl_id ESL ID (response slot) of the command's destination.
 */
void esl_record_control_point_esl_id(uint8_t group_id, uint8_t esl_id);

/**
 * @brief Retrieves and clears the destination ESL recorded for the in-flight Control Point command.
 *
 * Unpacks g_esl_control_point_cmd_dest_id into *p_group_id and *p_esl_id, then resets it to 0xFFFF
 * (idle). Called when the ESL Control Point notification is received so the response can be
 * attributed to the correct ESL, and to clear the pending state.
 *
 * @param p_group_id Output: subevent (ESL group) index of the command's destination.
 * @param p_esl_id Output: ESL ID (response slot) of the command's destination.
 */
void esl_get_control_point_esl_id(uint8_t *p_group_id, uint8_t *p_esl_id);

/**
 * @brief Checks whether a Control Point command is still awaiting a response.
 *
 * @return uint8_t TRUE if g_esl_control_point_cmd_dest_id is not 0xFFFF (a command is in flight),
 *                 FALSE if it is 0xFFFF (idle, no command pending).
 */
uint8_t esl_control_point_cmd_processing(void);

#ifdef __cplusplus
};
#endif

#endif /* __BLE_APP_H__*/
