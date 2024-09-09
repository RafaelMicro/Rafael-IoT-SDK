/******************************************************************************
*
* @File ble_hci.h
* @Version
* $Revision:
* $Date:
* @Brief
* @Note
*
******************************************************************************/
#ifndef _BLE_HCI_H_
#define _BLE_HCI_H_

/******************************************************************************
* INCLUDES
******************************************************************************/
#include <stdint.h>
#include "util_list.h"
/*******************************************************************************
*   DEFINES
*******************************************************************************/
#define BLE_TRANSPORT_HCI_COMMAND           (0x01)
#define BLE_TRANSPORT_HCI_ACL_DATA          (0x02)
#define BLE_TRANSPORT_HCI_EVENT             (0x04)

#define HCI_COMMAND_EVENT_MAX_LENGTH        (255)
#define HCI_ACL_DATA_MAX_LENGTH             (255)

typedef uint8_t HCI_ERROR_CODE_TYPE;
#define HCI_ERROR_CODE_SUCCESS                                              ((HCI_ERROR_CODE_TYPE)0x00)
#define HCI_ERROR_CODE_UNKNOWN_HCI_COMMAND                                  ((HCI_ERROR_CODE_TYPE)0x01)
#define HCI_ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                        ((HCI_ERROR_CODE_TYPE)0x02)
#define HCI_ERROR_CODE_HARDWARE_FAILURE                                     ((HCI_ERROR_CODE_TYPE)0x03)
#define HCI_ERROR_CODE_PAGE_TIMEOUT                                         ((HCI_ERROR_CODE_TYPE)0x04)
#define HCI_ERROR_CODE_AUTHENTICATION_FAILURE                               ((HCI_ERROR_CODE_TYPE)0x05)
#define HCI_ERROR_CODE_PIN_KEY_MISSING                                      ((HCI_ERROR_CODE_TYPE)0x06)
#define HCI_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED                             ((HCI_ERROR_CODE_TYPE)0x07)
#define HCI_ERROR_CODE_CONNECTION_TIMEOUT                                   ((HCI_ERROR_CODE_TYPE)0x08)
#define HCI_ERROR_CODE_CONNECTION_LIMIT_EXCEEDED                            ((HCI_ERROR_CODE_TYPE)0x09)
#define HCI_ERROR_CODE_SYNC_CONN_LIMIT_EXCEEDED                             ((HCI_ERROR_CODE_TYPE)0x0A)
#define HCI_ERROR_CODE_ACL_CONNECTION_EXISTS                                ((HCI_ERROR_CODE_TYPE)0x0B)
#define HCI_ERROR_CODE_COMMAND_DISALLOWED                                   ((HCI_ERROR_CODE_TYPE)0x0C)
#define HCI_ERROR_CODE_CONN_REJECT_LIMIT_RESOURCES                          ((HCI_ERROR_CODE_TYPE)0x0D)
#define HCI_ERROR_CODE_CONN_REJECT_SECURITY_REASON                          ((HCI_ERROR_CODE_TYPE)0x0E)
#define HCI_ERROR_CODE_CONN_REJECT_UNACCEPT_BDADDR                          ((HCI_ERROR_CODE_TYPE)0x0F)
#define HCI_ERROR_CODE_CONN_ACCEPT_TIMEOUT_EXCEEDED                         ((HCI_ERROR_CODE_TYPE)0x10)
#define HCI_ERROR_CODE_UNSUPPORTED_FEAT_OR_PARMS_VALUE                      ((HCI_ERROR_CODE_TYPE)0x11)
#define HCI_ERROR_CODE_INVALID_HCI_COMMAND_PARMS                            ((HCI_ERROR_CODE_TYPE)0x12)
#define HCI_ERROR_CODE_REMOTE_USER_TERMINATED_CONN                          ((HCI_ERROR_CODE_TYPE)0x13)
#define HCI_ERROR_CODE_REMOTE_DEVICE_TERM_CONN_LOW_RESS                     ((HCI_ERROR_CODE_TYPE)0x14)
#define HCI_ERROR_CODE_REMOTE_DEVICE_TERM_CONN_POWER_OFF                    ((HCI_ERROR_CODE_TYPE)0x15)
#define HCI_ERROR_CODE_CONNECTION_TERMINATED_LOCAL_HOST                     ((HCI_ERROR_CODE_TYPE)0x16)
#define HCI_ERROR_CODE_REPEATED_ATTEMPTS                                    ((HCI_ERROR_CODE_TYPE)0x17)
#define HCI_ERROR_CODE_PAIRING_NOT_ALLOWED                                  ((HCI_ERROR_CODE_TYPE)0x18)
#define HCI_ERROR_CODE_UNKNOWN_LMP_PDU                                      ((HCI_ERROR_CODE_TYPE)0x19)
#define HCI_ERROR_CODE_UNSUPPORTED_REMOTE_LMP_FEAT                          ((HCI_ERROR_CODE_TYPE)0x1A)
#define HCI_ERROR_CODE_SCO_OFFSET_REJECTED                                  ((HCI_ERROR_CODE_TYPE)0x1B)
#define HCI_ERROR_CODE_SCO_INTERVAL_REJECTED                                ((HCI_ERROR_CODE_TYPE)0x1C)
#define HCI_ERROR_CODE_SCO_AIR_MODE_REJECTED                                ((HCI_ERROR_CODE_TYPE)0x1D)
#define HCI_ERROR_CODE_INVALID_LMP_LL_PARMS                                 ((HCI_ERROR_CODE_TYPE)0x1E)
#define HCI_ERROR_CODE_UNSPECIFIED_ERROR                                    ((HCI_ERROR_CODE_TYPE)0x1F)
#define HCI_ERROR_CODE_UNSUPPORTED_LMP_LL_VALUE                             ((HCI_ERROR_CODE_TYPE)0x20)
#define HCI_ERROR_CODE_ROLE_CHANGE_NOT_ALLOWED                              ((HCI_ERROR_CODE_TYPE)0x21)
#define HCI_ERROR_CODE_LMP_LL_RSP_TIMEOUT                                   ((HCI_ERROR_CODE_TYPE)0x22)
#define HCI_ERROR_CODE_LL_PROCEDURE_COLLISION                               ((HCI_ERROR_CODE_TYPE)0x23)
#define HCI_ERROR_CODE_LMP_PDU_NOT_ALLOWED                                  ((HCI_ERROR_CODE_TYPE)0x24)
#define HCI_ERROR_CODE_ENCRYPTION_MODE_NOT_ACCEPTABLE                       ((HCI_ERROR_CODE_TYPE)0x25)
#define HCI_ERROR_CODE_LINK_KEY_CANNOT_BE_CHANGED                           ((HCI_ERROR_CODE_TYPE)0x26)
#define HCI_ERROR_CODE_REQUESTED_QOS_NOT_SUPPORTED                          ((HCI_ERROR_CODE_TYPE)0x27)
#define HCI_ERROR_CODE_INSTANT_PASSED                                       ((HCI_ERROR_CODE_TYPE)0x28)
#define HCI_ERROR_CODE_PAIRING_UNIT_KEY_NOT_SUPPORTED                       ((HCI_ERROR_CODE_TYPE)0x29)
#define HCI_ERROR_CODE_DIFFERENT_TRANSACTION_COLLISION                      ((HCI_ERROR_CODE_TYPE)0x2A)
#define HCI_ERROR_CODE_QOS_UNACCEPTABLE_PARMS                               ((HCI_ERROR_CODE_TYPE)0x2C)
#define HCI_ERROR_CODE_QOS_REJECTED                                         ((HCI_ERROR_CODE_TYPE)0x2D)
#define HCI_ERROR_CODE_CHANNEL_CLASSIFICATION_NOT_SUPPORTED                 ((HCI_ERROR_CODE_TYPE)0x2E)
#define HCI_ERROR_CODE_INSUFFICIENT_SECURITY                                ((HCI_ERROR_CODE_TYPE)0x2F)
#define HCI_ERROR_CODE_PARMS_OUT_OF_MANDATORY_RANGE                         ((HCI_ERROR_CODE_TYPE)0x30)
#define HCI_ERROR_CODE_ROLE_SWITCH_PENDING                                  ((HCI_ERROR_CODE_TYPE)0x32)
#define HCI_ERROR_CODE_RESERVED_SLOT_VIOLATION                              ((HCI_ERROR_CODE_TYPE)0x34)
#define HCI_ERROR_CODE_ROLE_SWITCH_FAILED                                   ((HCI_ERROR_CODE_TYPE)0x35)
#define HCI_ERROR_CODE_EXTENDED_INQUIRY_RSP_LARGE                           ((HCI_ERROR_CODE_TYPE)0x36)
#define HCI_ERROR_CODE_SECURE_SIMPLE_PAIRING_NOT_SUPP_HOST                  ((HCI_ERROR_CODE_TYPE)0x37)
#define HCI_ERROR_CODE_HOST_BUSY_PAIRING                                    ((HCI_ERROR_CODE_TYPE)0x38)
#define HCI_ERROR_CODE_CONN_REJECTED_SUITABLE_CHANNEL_FOUND                 ((HCI_ERROR_CODE_TYPE)0x39)
#define HCI_ERROR_CODE_CONTROLLER_BUSY                                      ((HCI_ERROR_CODE_TYPE)0x3A)
#define HCI_ERROR_CODE_UNACCEPTABLE_CONNECTION_PARMS                        ((HCI_ERROR_CODE_TYPE)0x3B)
#define HCI_ERROR_CODE_ADVERTISING_TIMEOUT                                  ((HCI_ERROR_CODE_TYPE)0x3C)
#define HCI_ERROR_CODE_CONNECTION_TERMINATED_MIC_FAILURE                    ((HCI_ERROR_CODE_TYPE)0x3D)
#define HCI_ERROR_CODE_CONNECTION_FAILED_ESTABLISHED                        ((HCI_ERROR_CODE_TYPE)0x3E)
#define HCI_ERROR_CODE_MAC_CONNECTION_FAILED                                ((HCI_ERROR_CODE_TYPE)0x3F)
#define HCI_ERROR_CODE_COARSE_CLK_ADJ_REJ_WILL_TRY_ADJ_USING_CLK_DRAGG      ((HCI_ERROR_CODE_TYPE)0x40)
#define HCI_ERROR_CODE_UNKNOWN_ADVERTISING_IDENTIFIER                       ((HCI_ERROR_CODE_TYPE)0x42)
#define HCI_ERROR_CODE_LIMIT_REACHED                                        ((HCI_ERROR_CODE_TYPE)0x43)
#define HCI_ERROR_CODE_OPERATION_CANCELLED_BY_HOST                          ((HCI_ERROR_CODE_TYPE)0x44)

/* HCI COMMAND LE GROUP OPCODE */
#define HCI_CMD_LE_SET_RANDOM_DEVICE_ADDRESS                    (0x05)
#define HCI_CMD_LE_SET_ADVERTISING_PARAMETERS                   (0x06)
#define HCI_CMD_LE_WRITE_ADVERTISING_DATA                       (0x08)
#define HCI_CMD_LE_SET_SCAN_RSP_DATA                            (0x09)
#define HCI_CMD_LE_SET_ADVERTISING_ENABLE                       (0x0A)
#define HCI_CMD_LE_SET_SCAN_PARAMETERS                          (0x0B)
#define HCI_CMD_LE_WRITE_SCAN_ENABLE                            (0x0C)
#define HCI_CMD_LE_CREATE_LL_CONNECTION                         (0x0D)
#define HCI_CMD_LE_ENCRYPTION                                   (0x17)
#define HCI_CMD_LE_RANDOM                                       (0x18)


/* HCI Event Code */

#define HCI_EVENT_DISCONNECTION_COMPLETE                    (0x05)
#define HCI_EVENT_COMMAND_COMPLETE                          (0x0E)
#define HCI_EVENT_COMMAND_STATUS                            (0x0F)
#define HCI_EVENT_NUM_OF_COMPLETED_PACKET                   (0x13)
#define HCI_EVENT_LE_META_EVENT                             (0x3E)



/* HCI LE Sub-Event Code */

#define HCI_LE_EVENT_CONNECTION_COMPLETE                      (0x01)
#define HCI_LE_EVENT_ADV_PACKET_REPORT                        (0x02)
#define HCI_LE_EVENT_CONNECTION_UPDATE_COMPLETE               (0x03)
#define HCI_LE_EVENT_READ_REMOTE_FEATURES_COMPLETE            (0x04)
#define HCI_LE_EVENT_LTK_REQUESTED                            (0x05)
#define HCI_LE_EVENT_REMOTE_CONNECTION_PARMS_REQUEST          (0x06)
#define HCI_LE_EVENT_DATA_LENGTH_CHANGE                       (0x07)
#define HCI_LE_EVENT_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE      (0x08)
#define HCI_LE_EVENT_GENERATE_DHKEY_COMPLETE                  (0x09)
#define HCI_LE_EVENT_ENHANCED_CONNECTION_COMPLETE             (0x0A)
#define HCI_LE_EVENT_PHY_UPDATE_COMPLETE                      (0x0C)

struct __attribute__((packed)) ble_hci_command_struct
{
    uint8_t transport_id;
    uint16_t ocf: 10;
    uint16_t ogf: 6;
    uint8_t length;
    uint8_t parameter[HCI_COMMAND_EVENT_MAX_LENGTH];
};
struct __attribute__((packed)) ble_hci_acl_data_struct
{
    uint8_t transport_id;
    uint16_t handle: 12;
    uint16_t pb_flag: 2;
    uint16_t bc_flag: 2;
    uint16_t length;
    uint8_t data[HCI_ACL_DATA_MAX_LENGTH];
};
struct __attribute__((packed)) ble_hci_acl_data_sn_struct
{
    uint8_t transport_id;
    uint16_t sequence;
    uint16_t handle: 12;
    uint16_t pb_flag: 2;
    uint16_t bc_flag: 2;
    uint16_t length;
    uint8_t data[HCI_ACL_DATA_MAX_LENGTH];
};
struct __attribute__((packed)) ble_hci_event_struct
{
    uint8_t transport_id;
    uint8_t event_code;
    uint8_t length;
    uint8_t parameter[HCI_COMMAND_EVENT_MAX_LENGTH];
};

typedef union ble_hci_message
{
    uint8_t ble_hci_array[HCI_ACL_DATA_MAX_LENGTH + 13];
    struct ble_hci_command_struct hci_command;
    struct ble_hci_acl_data_struct hci_acl_data;
    struct ble_hci_event_struct hci_event;
} UNION_BLE_HCI_MESSAGE;


typedef struct 
{
    UNION_BLE_HCI_MESSAGE hci_message;
} ble_hci_message_t;

/*******************************************************************************
*   GLOBAL FUNCTIONS
*******************************************************************************/
#endif /* _BLE_HCI_H_ */

