/** @file ble_api.h
 *
 * @brief BLE API header file.
 *
 */

/**
 * @defgroup ble_api BLE API Definition
 * @ingroup BLE_group
 * @{
 * @brief Define BLE API definitions, structures, and functions.
 * @}
 */
#ifndef __BLE_API_H__
#define __BLE_API_H__

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <stdint.h>
#include "ble_hci_error.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/



/** @brief BLE api return parameter definition.
 * @ingroup ble_api
*/
typedef enum
{
    BLE_ERR_L2CAP_DATA_DEST_ID_UNMATCH    = -29,  /** @brief BLE l2cap data destination id is not match. */
    BLE_ERR_L2CAP_DATA_CHANNEL_NOT_EXIST  = -28,  /** @brief BLE l2cap data channel is not exist. */
    BLE_ERR_RESOURCE_NOT_ENOUGH           = -27,  /** @brief BLE resource is not enough. */
    BLE_ERR_HCI_TX_DATA_QUEUE_FULL        = -26,  /** @brief BLE tx data queue full. */
    BLE_ERR_HCI_TX_CMD_QUEUE_FULL         = -25,  /** @brief BLE tx command queue full. */
    BLE_ERR_SEQUENTIAL_PROTOCOL_VIOLATION = -24,  /** @brief Detecting a sequential protocol violation. Usually happens in there is an another GATT request already in progress please wait and retry.*/
    BLE_ERR_DB_PARSING_IN_PROGRESS        = -23,  /** @brief Host peripheral database parsing is still in progress. */
    BLE_ERR_INVALID_HANDLE                = -22,  /** @brief Invalid BLE handle. */
    BLE_ERR_INVALID_HOST_ID               = -21,  /** @brief Invalid Host ID */
    BLE_ERR_CMD_NOT_SUPPORTED             = -20,  /** @brief Command not supported */
    BLE_ERR_INVALID_PARAMETER             = -19,  /** @brief Invalid parameter. */
    BLE_ERR_INVALID_STATE                 = -18,  /** @brief Invalid state. */
    BLE_ERR_TIMER_OP                      = -17,  /** @brief TIMER OP. */
    BLE_ERR_ALLOC_MEMORY_FAIL             = -16,  /** @brief RECVFROM memory fail. */
    BLE_ERR_RECVFROM_LEN_NOT_ENOUGH       = -15,  /** @brief RECVFROM length error. */
    BLE_ERR_RECVFROM_FAIL                 = -14,  /** @brief RECVFROM fail. */
    BLE_ERR_RECVFROM_NO_DATA              = -13,  /** @brief RECVFROM no data. */
    BLE_ERR_RECVFROM_POINTER_NULL         = -12,  /** @brief RECVFROM Pointer NULL. */
    BLE_ERR_SENDTO_FAIL                   = -11,  /** @brief SENDTO fail. */
    BLE_ERR_SENDTO_POINTER_NULL           = -10,  /** @brief SENDTO Pointer NULL. */
    BLE_BUSY                              = -9,   /** @brief Busy. */
    BLE_ERR_WRONG_CONFIG                  = -8,   /** @brief Wrong configuration. */
    BLE_ERR_SEMAPHORE_MALLOC_FAIL         = -7,   /** @brief Semaphore malloc fail. */
    BLE_ERR_THREAD_MALLOC_FAIL            = -6,   /** @brief Thread malloc fail. */
    BLE_ERR_QUEUE_MALLOC_FAIL             = -5,   /** @brief Queue malloc fail. */
    BLE_ERR_DATA_MALLOC_FAIL              = -4,   /** @brief Data malloc fail. */
    BLE_ERR_DUPLICATE_INIT                = -3,   /** @brief Duplicate init. */
    BLE_ERR_NOT_INIT                      = -2,   /** @brief Not initial. */
    BLE_ERR_UNKNOW_TYPE                   = -1,   /** @brief Unknown type. */
    BLE_ERR_OK                            = 0,    /** @brief Success. */
} ble_err_t;


typedef enum
{
    //====================================================================
    //     BLE GATT MAPPING
    //====================================================================
    TYPE_BLE_GATT_START,  //0

    /** @brief BLE GATT Read Response.
     *  @par   none
     *
     *  @attention Only supported if GATT role is @ref BLE_GATT_ROLE_SERVER.
     *  @note The read response is sent in reply to a received Read Request and contains the value of the attribute that has been read.
     *  @param @ref ble_gatt_data_param_t
     */
    TYPE_BLE_GATT_READ_RSP = TYPE_BLE_GATT_START,

    /** @brief BLE GATT Read By Type Response.
     *  @par   none
     *
     *  @attention Only supported if GATT role is @ref BLE_GATT_ROLE_SERVER.
     *  @note The read by type response is sent in reply to a received Read By Type Request and contains the handle number and value of the attribute that has been read.
     *  @param @ref ble_gatt_data_param_t
     */
    TYPE_BLE_GATT_READ_BY_TYPE_RSP,

    /** @brief BLE GATT Read Blob Response.
     *  @par   none
     *
     *  @attention Only supported if GATT role is @ref BLE_GATT_ROLE_SERVER.
     *  @note The read blob response is sent in reply to a received Read Blob Request and contains the value of the attribute that has been read.
     *  @param @ref ble_gatt_data_param_t
     */
    TYPE_BLE_GATT_READ_BLOB_RSP,

    /** @brief BLE GATT notification.
     *  @par   none
     *
     *  @attention Only supported if GATT role is @ref BLE_GATT_ROLE_SERVER.
     *  @note When a server is configured to notify a Characteristic Value to a client without the acknowledgment that the notification was successfully received.
     *  @param @ref ble_gatt_data_param_t
     */
    TYPE_BLE_GATT_NOTIFICATION,

    /** @brief BLE GATT indication.
     *  @par   none
     *
     *  @attention Only supported if GATT role is @ref BLE_GATT_ROLE_SERVER.
     *  @note When a server is configured to indicate a Characteristic Value to a client and expects the indication was successfully received.
     *  @param @ref ble_gatt_data_param_t
     */
    TYPE_BLE_GATT_INDICATION,

    /** @brief BLE GATT Write Request.
     *  @par   none
     *
     *  @attention Only supported if GATT role is @ref BLE_GATT_ROLE_CLIENT.
     *  @note The Write Request is used to request the server to write the value of an attribute and acknowledge that this has been achieved in a Write Response. \n
     *        A Write Response shall be sent by the server if the write of the Characteristic Value succeeded.
     *  @param @ref ble_gatt_data_param_t
     */
    TYPE_BLE_GATT_WRITE_REQ,

    /** @brief BLE GATT Write Command.
     *  @par   none
     *
     *  @attention Only supported if GATT role is @ref BLE_GATT_ROLE_CLIENT.
     *  @note Write a Characteristic Value to a server when the client knows the Characteristic Value Handle \n
     *        and the client does not need an acknowledgment that the write was successfully performed.
     *  @param @ref ble_gatt_data_param_t
     */
    TYPE_BLE_GATT_WRITE_CMD,

    //====================================================================
    //     END
    //====================================================================
    TYPE_BLE_API_MAX,

} ble_api_type_t;


typedef enum
{
    BLE_APP_RETURN_PARAMETER_EVENT,   /**< Queue type: BLE command request. */
    BLE_APP_GENERAL_EVENT,            /**< Queue type: BLE event. */
    BLE_APP_SERVICE_EVENT,            /**< Queue type: BLE service data. */
    BLE_APP_L2CAP_EVENT,              /**< Queue type: BLE L2CAP data. */
} ble_event_t;


typedef struct
{
    uint16_t    type;       /**< Payload Identifier : defined by each module */
    uint16_t    length;     /**< Payload Length : the length of the payload data */
    uint8_t     value[];    /**< Payload Data */
} ble_tlv_t;


typedef struct
{
    uint8_t     hci_tx_level;       /* Bluetooth LE task PRIORITY : HCI TX level */
    uint8_t     ble_host_level;     /* Bluetooth LE task PRIORITY : host level */
} ble_task_priority_t;





/**************************************************************************************************
 *    GLOBAL PROTOTYPES
 *************************************************************************************************/

/** @brief Set BLE host stack initialization.
 *
 * @ingroup ble_api
 *
 * @param[in] pt_cfg : a pointer to the event indication callback function.
 *
 * @return @ref BLE_ERR_OK is success or an @ref ble_err_t "error".
 */
int ble_host_stack_init(ble_task_priority_t* p_priority);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __BLE_API_H__*/
