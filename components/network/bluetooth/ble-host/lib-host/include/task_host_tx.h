
#ifndef __TASK_HOST_TX_H__
#define __TASK_HOST_TX_H__

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/

#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/

#define TX_TYPE_DATA 0x00
#define TX_TYPE_CMD  0x01

#define PHY_STATUS                          (0x05)     /**< Length of PHY status (HW generate). */
#define CRC                                 (0x03)     /**< Length of CRC (HW generate).*/
#define MIC                                 (0x04)     /**< Length of MIC (HW generate).*/
#define HCI_PKT_IND                         (0x01)     /**< Length of HCI packet indicator. */
#define HANDLE_PB_PC                        (0x02)     /**< Length of HCI ACL data header, handle + PB + PC. */
#define DATA_TOTAL_LEN                      (0x02)     /**< Length of HCI ACL data header, data total length. */

#define BLE_TRANSPORT_HCI_COMMAND           (0x01)     /**< HCI packet indicators for command packet. */
#define BLE_TRANSPORT_HCI_ACL_DATA          (0x02)     /**< HCI packet indicators for ACL data packet. */
#define BLE_TRANSPORT_HCI_EVENT             (0x04)     /**< HCI packet indicators for event packet. */

#define HCI_ACL_DATA_MAX_LENGTH             (251+MIC)  /**< Maximum ACL data length. */
#define HCI_ARRAY_MAX_LENGTH                (HCI_ACL_DATA_MAX_LENGTH + PHY_STATUS + CRC + HCI_PKT_IND + HANDLE_PB_PC + DATA_TOTAL_LEN)  /**< Maximum data length get from HW. */


/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/

typedef enum
{
    HCI_MSG_BASE = 0x0000,
    HCI_MSG_SEND_HCI_ACL_DATA,
    HCI_MSG_SEND_HCI_CMD,
    HCI_MSG_GET_NOCP_EVENT,
    HCI_MSG_GET_CMD_COMPLETE_EVENT,
    HCI_MSG_UPDATE_LOCAL_BUFFER_SIZE,
    HCI_MSG_DROP_UNFINISH_ACL_DATA,
    HCI_MSG_WAKEUP_HOST_TX_TASK
} hci_msg_def_t;

/**
 * @brief The parameter definition for host transmit HCI command.
 * @ingroup task_hci
 */

typedef struct __attribute__((packed))
{
    uint8_t transport_id; /**< Transport id. */
    uint16_t ocf: 10;     /**< Opcode command field. */
    uint16_t ogf: 6;      /**< Opcode group field. */
    uint8_t length;       /**< Length. */
    uint8_t parameter[];  /**< Parameters. */
}
ble_hci_tx_hci_command_t;


/**
 * @brief The parameter definition for host transmit HCI ACL data.
 * @ingroup task_host
 */

typedef struct __attribute__((packed))
{
    uint8_t transport_id;   /**< Transport id. */
    uint16_t sequence;      /**< Sequence number. */
    uint16_t handle: 12;    /**< Connection id. */
    uint16_t pb_flag: 2;    /**< Packet boundary flag. */
    uint16_t bc_flag: 2;    /**< Broadcast flag. */
    uint16_t length;        /**< Data total length. */
    void    *p_data;        /**< ACL data. */
}
ble_hci_tx_acl_data_t;


/**
 * @brief The parameter definition for host clear unfinish ACL data by conn handle.
 * @ingroup task_host
 */

typedef struct __attribute__((packed))
{
    uint16_t conn_handle;    /**< Connection handle. */
}
ble_hci_drop_unfinish_acl_data_t;


/**
 * @brief The parameter definition for BLE host to notify host tx task to update local buffer size
 * @ingroup task_host
 */

typedef struct __attribute__((packed))
{
    uint8_t buffer_size;  /**< buffer size. */
}
ble_hci_update_local_buffer_size_t;

/**
 * @brief The parameter definition for BLE host to notify host tx task how many transmitted HCI command completed.
 * @ingroup task_host
 */

typedef struct __attribute__((packed))
{
    uint8_t complete_num;  /**< number of complete command. */
}
ble_hci_get_command_complete_event_t;

/**
 * @brief The parameter definition for BLE host to notify host tx task how many transmitted ACL data completed.
 * @ingroup task_host
 */

typedef struct __attribute__((packed))
{
    uint8_t complete_num;  /**< number of complete command. */
}
ble_hci_get_nocp_event_t;


/**
 * @brief All messages that HCI task can handle.
 * @ingroup task_hci
 */

typedef struct
{
    union
    {
        uint8_t                               ble_hci_array[HCI_ARRAY_MAX_LENGTH];  /**< HCI array for read data from communication subsystem. */
        ble_hci_tx_hci_command_t              tx_hci_command;                       /**< HCI command format send to controller. */
        ble_hci_tx_acl_data_t                 tx_hci_acl_data;                      /**< HCI ACL data format send to controller. */
        ble_hci_get_command_complete_event_t  get_cmd_complete_event;               /**< HCI command complete or command status event get from controller. */
        ble_hci_get_nocp_event_t              get_nocp_event;                       /**< HCI NOCP event get from controller. */
        ble_hci_update_local_buffer_size_t    update_buffer_size;                   /**< ACL data complete number event get from BLE host. */
        ble_hci_drop_unfinish_acl_data_t      drop_acl_data;                        /**< Drop unfinish ACL data. */
    } msg_type;
} ble_hci_message_struct_t;

/**
 * @brief The message parameter of HCI task.
 * @ingroup task_hci
 */

typedef struct
{
    uint32_t hci_msg_tag;                          /**< hci message tag. */
    ble_hci_message_struct_t *p_hci_msg;        /**< this parameter saving pointer. */
} hci_task_common_queue_t;



/**************************************************************************
 * EXTERN DEFINITIONS
 **************************************************************************/
/**@brief The handle of HCI common queue.
 * @ingroup task_hci
 */


extern QueueHandle_t g_hci_common_handle;
extern QueueHandle_t g_hci_tx_acl_handle;
extern QueueHandle_t g_hci_tx_cmd_handle;
/**************************************************************************************************
 *    PUBLIC FUNCTIONS
 *************************************************************************************************/

/**@brief BLE HCI task initialization.
 *
 * @ingroup task_hci
 *
 * @return none
 */
void task_host_tx_init(uint8_t task_priority);

void task_host_tx_get_tx_done(uint8_t tx_type);

void task_host_tx_update_buffer_size(uint8_t buffer_size);

#ifdef __cplusplus
};
#endif

#endif /* __TASK_HOST_TX_H__*/
