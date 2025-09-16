/** @file ble_hci_event.h
 *
 * @addtogroup BLE
 * @{
 * @addtogroup hci HCI
 * @{
 * @addtogroup hci_cmd HCI commands
 * @{
 * @defgroup hci_cmd_param HCI Command Parameters
 * @brief Defined HCI command definition and data structures.
 * @}
 * @}
 * @}
 */
#ifndef __BLE_HCI_H__
#define __BLE_HCI_H__


/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include "ble_gap.h"
#include "ble_hci_error.h"
#include "ble_hci_event.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**************************************************
   HCI opcode
 **************************************************/

/**
 * @defgroup hci_cmd_param_link_control Link Control Commands
 * @{
 * @ingroup hci_cmd_param
*/
#define HCI_CMD_LINK_CTRL_CMD                      (0x01)   /**< Link control commands, OGF=0x01. */
#define LINK_CTRL_CMD_DISCONNECT                   (0x06)   /**< Link control commands: disconnect command.*/
#define LINK_CTRL_CMD_READ_REMOTE_VERSION_INFO     (0x1D)   /**< Link control commands: read remote version information command.*/
/** @} */



/**
 * @defgroup hci_cmd_param_info_param Information Parameters Commands
 * @{
 * @ingroup hci_cmd_param
*/
#define HCI_CMD_INFO_PARAM                         (0x04)   /**< Information parameters commands, OGF=0x04. */
#define INFO_PARAM_READ_LOCAL_VERSION_INFO         (0x01)   /**< Information parameters commands: read local version information command. */
#define INFO_PARAM_READ_LOCAL_SUPPORTED_CMDS       (0x02)   /**< Information parameters commands: read local supported commands command. */
#define INFO_PARAM_READ_LOCAL_SUPPORTED_FETAUTES   (0x03)   /**< Information parameters commands: read local supported features command. */
#define INFO_PARAM_READ_BD_ADDR                    (0x09)   /**< Information parameters commands: read BD address command. */
/** @} */



/**
 * @defgroup hci_cmd_param_status_param Status Parameters Commands
 * @{
 * @ingroup hci_cmd_param
*/
#define HCI_CMD_STATUS_PARAM                       (0x05)   /**< Status parameters commands, OGF=0x05. */
#define STATUS_PARAM_READ_RSSI                     (0x05)   /**< Status parameters commands: read RSSI. */

/** @} */




/**
 * @defgroup hci_cmd_param_le_controller LE Controller Commands
 * @{
 * @ingroup hci_cmd_param
*/
#define HCI_CMD_LE_CONTROLLER_CMD                                       (0x08)   /**< LE controller commands, OGF=0x08. */
#define LE_SET_EVENT_MASK                                               (0x01)   /**< LE controller commands: LE set event mask command. */
#define LE_READ_BUFFER_SIZE                                             (0x02)   /**< LE controller commands: LE read buffer size command. */
#define LE_READ_LOCAL_SUPPORTED_FEATURES                                (0x03)   /**< LE controller commands: LE read local features command. */
#define LE_SET_RANDOM_DEVICE_ADDRESS                                    (0x05)   /**< LE controller commands: LE set random device address command. */
#define LE_SET_ADVERTISING_PARAMETERS                                   (0x06)   /**< LE controller commands: LE set advertising parameters command. */
#define LE_SET_ADVERTISING_DATA                                         (0x08)   /**< LE controller commands: LE set advertising data command. */
#define LE_SET_SCAN_RSP_DATA                                            (0x09)   /**< LE controller commands: LE set scan response data command. */
#define LE_SET_ADVERTISING_ENABLE                                       (0x0A)   /**< LE controller commands: LE set advertising enable command. */
#define LE_SET_SCAN_PARAMETERS                                          (0x0B)   /**< LE controller commands: LE set scan parameters command. */
#define LE_SET_SCAN_ENABLE                                              (0x0C)   /**< LE controller commands: LE set scan enable command. */
#define LE_CREATE_CONNECTION                                            (0x0D)   /**< LE controller commands: LE create connection command. */
#define LE_CREATE_CONNECTION_CANCEL                                     (0x0E)   /**< LE controller commands: LE create connection cancel command. */
#define LE_READ_FILTER_ACCEPT_LIST_SIZE                                 (0x0F)   /**< LE controller commands: LE read filter accept list size command. */
#define LE_CLEAR_FILTER_ACCEPT_LIST                                     (0x10)   /**< LE controller commands: LE clear filter accept list command. */
#define LE_ADD_DEVICE_TO_FILTER_ACCEPT_LIST                             (0x11)   /**< LE controller commands: LE add device to filter accept list command. */
#define LE_REMOVE_DEVICE_FROM_FILTER_ACCEPT_LIST                        (0x12)   /**< LE controller commands: LE remove device from filter accept list command. */
#define LE_CONNECTION_UPDATE                                            (0x13)   /**< LE controller commands: LE connection update command. */
#define LE_SET_HOST_CHANNEL_CLASSIFICATION                              (0x14)   /**< LE controller commands: LE set host channel classification command. */
#define LE_READ_CHANNEL_MAP                                             (0x15)   /**< LE controller commands: LE read channel map command. */
#define LE_READ_REMOTE_FEATURES                                         (0x16)   /**< LE controller commands: LE read remote features command. */
#define LE_ENCRYPT                                                      (0x17)   /**< LE controller commands: LE encrypt command. */
#define LE_RANDOM                                                       (0x18)   /**< LE controller commands: LE random command. */
#define LE_ENABLE_ENCRYPT                                               (0x19)   /**< LE controller commands: LE enable encrypt command. */
#define LE_LONG_TERM_KEY_REQ_REPLY                                      (0x1A)   /**< LE controller commands: LE long term key request reply command. */
#define LE_LONG_TERM_KEY_REQ_NEG_REPLY                                  (0x1B)   /**< LE controller commands: LE long tern key request negative reply command. */
#define LE_READ_SUPPORTED_STATES                                        (0x1C)   /**< LE controller commands: LE read supported state command. */
#define LE_REMOTE_CONNECTION_PARAMETER_REQ_REPLY                        (0x20)   /**< LE controller commands: LE remote connection parameter request reply command. */
#define LE_REMOTE_CONNECTION_PARAMETER_REQ_NEG_REPLY                    (0x21)   /**< LE controller commands: LE remote connection parameter request negative reply command. */
#define LE_SET_DATA_LENGTH                                              (0x22)   /**< LE controller commands: LE set data length command. */
#define LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH                           (0x23)   /**< LE controller commands: LE read suggested default data length command. */
#define LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH                          (0x24)   /**< LE controller commands: LE write suggested default data length command. */
#define LE_ADD_DEVICE_TO_RESOLVING_LIST                                 (0x27)   /**< LE controller commands: LE add device to resolving list command. */
#define LE_REMOVE_DEVICE_FROM_RESOLVING_LIST                            (0x28)   /**< LE controller commands: LE remove device from resolving list command. */
#define LE_CLEAR_RESOLVING_LIST                                         (0x29)   /**< LE controller commands: LE clear resolving list command. */
#define LE_READ_RESOLVING_LIST_SIZE                                     (0x2A)   /**< LE controller commands: LE read resolving list size command. */
#define LE_READ_PEER_RESOLVABLE_ADDRESS                                 (0x2B)   /**< LE controller commands: LE read peer resolvable address command. */
#define LE_READ_LOCAL_RESOLVABLE_ADDRESS                                (0x2C)   /**< LE controller commands: LE read local resolvable address command. */
#define LE_SET_ADDRESS_RESOLUTION_ENABLE                                (0x2D)   /**< LE controller commands: LE set address resolution enable command. */
#define LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT                       (0x2E)   /**< LE controller commands: LE set resolvable private address timeout command. */
#define LE_READ_MAXIMUM_DATA_LENGTH                                     (0x2F)   /**< LE controller commands: LE read maximum data length command. */
#define LE_READ_PHY                                                     (0x30)   /**< LE controller commands: LE read PHY command. */
#define LE_SET_DEFAULT_PHY                                              (0x31)   /**< LE controller commands: LE set default PHY command. */
#define LE_SET_PHY                                                      (0x32)   /**< LE controller commands: LE set PHY command. */
#define LE_SET_ADVERTISING_SET_RANDOM_ADDRESS                           (0x35)   /**< LE controller commands: LE set advertising set random address command. */
#define LE_SET_EXTENDED_ADVERTISING_PARAMETERS                          (0x36)   /**< LE controller commands: LE set extended advertising parameters command. */
#define LE_SET_EXTENDED_ADVERTISING_DATA                                (0x37)   /**< LE controller commands: LE set extended advertising data command. */
#define LE_SET_EXTENDED_SCAN_RESPONSE_DATA                              (0x38)   /**< LE controller commands: LE set extended scan response data command. */
#define LE_SET_EXTENDED_ADVERTISING_ENABLE                              (0x39)   /**< LE controller commands: LE set extended advertising enable command. */
#define LE_READ_MAXIMUM_ADVERTISING_DATA_LENGTH                         (0x3A)   /**< LE controller commands: LE read maximum advertising data length command. */
#define LE_READ_NUM_OF_SUPPORTED_ADVERTISING_SETS                       (0x3B)   /**< LE controller commands: LE read number of supported advertising data sets command. */
#define LE_REMOVE_ADVERTISING_SET                                       (0x3C)   /**< LE controller commands: LE remove advertising set command. */
#define LE_CLEAR_ADVERTISING_SETS                                       (0x3D)   /**< LE controller commands: LE clear advertising sets command. */
#define LE_SET_PERIODIC_ADVERTISING_PARAMETERS                          (0x3E)   /**< LE controller commands: LE set periodic advertising parameters command. */
#define LE_SET_PERIODIC_ADVERTISING_DATA                                (0x3F)   /**< LE controller commands: LE set periodic advertising data command. */
#define LE_SET_PERIODIC_ADVERTISING_ENABLE                              (0x40)   /**< LE controller commands: LE set periodic advertising enable command. */
#define LE_SET_EXTENDED_SCAN_PARAMETERS                                 (0x41)   /**< LE controller commands: LE set extended scan parameters command. */
#define LE_SET_EXTENDED_SCAN_ENABLE                                     (0x42)   /**< LE controller commands: LE set extended scan enable command. */
#define LE_EXTENDED_CREATE_CONNECTION                                   (0x43)   /**< LE controller commands: LE set extended create connection command. */
#define LE_PERIODIC_ADVERTISING_CREATE_SYNC                             (0x44)   /**< LE controller commands: LE periodic advertising create sync command. */
#define LE_PERIODIC_ADVERTISING_CREATE_SYNC_CANCEL                      (0x45)   /**< LE controller commands: LE periodic advertising create sync cancel command. */
#define LE_PERIODIC_ADVERTISING_TERMINATE_SYNC                          (0x46)   /**< LE controller commands: LE periodic advertising terminate sync command. */
#define LE_ADD_DEVICE_TO_PERIODIC_ADVERTISER_LIST                       (0x47)   /**< LE controller commands: LE add device to periodic advertiser sync command. */
#define LE_REMOVE_DEVICE_FROM_PERIODIC_ADVERTISER_LIST                  (0x48)   /**< LE controller commands: LE remove device from periodic advertising list command. */
#define LE_CLEAR_PERIODIC_ADVERTISER_LIST                               (0x49)   /**< LE controller commands: LE clear periodic advertising list command. */
#define LE_READ_PERIODIC_ADVERTISER_LIST_SIZE                           (0x4A)   /**< LE controller commands: LE read periodic advertising list size command. */
#define LE_SET_PRIVACY_MODE                                             (0x4E)   /**< LE controller commands: LE set Privacy mode command. */
#define LE_SET_CONNECTION_CTE_RECEIVE_PARAMETERS                        (0x54)   /**< LE controller commands: LE Set Connection CTE Receive Parameters command. */
#define LE_SET_CONNECTION_CTE_TRANSMIT_PARAMETERS                       (0x55)   /**< LE controller commands: LE Set Connection CTE Transmit Parameters command. */
#define LE_CONNECTION_CTE_REQUEST_ENABLE                                (0x56)   /**< LE controller commands: LE Connection CTE Request Enable command. */
#define LE_CONNECTION_CTE_RESPONSE_ENABLE                               (0x57)   /**< LE controller commands: LE Connection CTE Response Enable command. */
#define LE_READ_ANTENNA_INFORMATION                                     (0x58)   /**< LE controller commands: LE Read Antenna Information command. */
#define LE_SET_PERIODIC_ADVERTISING_RECEIVE_ENABLE                      (0x59)   /**< LE controller commands: LE set periodic advertising receive enable command. */
#define LE_PERIODIC_ADVERTISING_SYNC_TRANSFER                           (0x5A)   /**< LE controller commands: LE periodic advertising sync transfer command. */
#define LE_PERIODIC_ADVERTISING_SET_INFO_TRANSFER                       (0x5B)   /**< LE controller commands: LE periodic advertising set info transfer command. */
#define LE_SET_PERIODIC_ADVERTISING_SYNC_TRANSFER_PARAMETERS            (0x5C)   /**< LE controller commands: LE set periodic advertising sync transfer parameters command. */
#define LE_SET_DEFAULT_PERIODIC_ADVERTISING_SYNC_TRANSFER_PARAMETERS    (0x5D)   /**< LE controller commands: LE set default periodic advertising sync transfer parameters command. */
#define LE_SET_PERIODIC_ADVERTISING_SUBEVENT_DATA                       (0x82)   /**< LE controller commands: LE set periodic advertising subevent data command. */
#define LE_SET_PERIODIC_ADVERTISING_RESPONSE_DATA                       (0x83)   /**< LE controller commands: LE set periodic advertising responser data command. */
#define LE_SET_PERIODIC_SYNC_SUBEVENT                                   (0x84)   /**< LE controller commands: LE periodic sync subevent command. */
#define LE_SET_EXTENDED_CREATE_CONNECTION_V2                            (0x85)   /**< LE controller commands: LE extended create connection v2 command. */
#define LE_SET_PERIODIC_ADVERTISING_PARAMETERS_V2                       (0x86)   /**< LE controller commands: LE set periodic advertising parameters v2 command. */


/** @} */



/**
 * @defgroup hci_cmd_param_vendor Vendor Commands
 * @{
 * @ingroup hci_cmd_param
*/
#define HCI_CMD_LE_VENDOR_CMD                      (0x3F)   /**< Vendor commands, OGF=0x3F. */
#define LE_SET_CONTROLLER_INFO                     (0x01)   /**< Vendor commands: set public addr, BT spec. version, company ID. */
#define LE_SCAN_REQUEST_REPORT                     (0x82)   /**< Vendor commands: control the scan request report. */
/** @} */



/**
 * @defgroup hci_cmd_param_encrypt_enable HCI Encryption Enabled Parameter
 * @{
 * @ingroup hci_cmd_param
*/
#define HCI_ENCRPT_EN_OFF                                                           0x00    /**< Link level encryption is OFF. */
#define HCI_ENCRPT_EN_AES_CCM_LE                                                    0x01    /**< Link level encryption is ON with AES-CCM for LE. */
#define HCI_ENCRPT_EN_AES_CCM_BEDR                                                  0x02    /**< Link level encryption is ON with AES-CCM for BR/EDR. */
/** @} */


/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/


/* ==================================================
 *  Controller and Baseband Commands: OGF = 0x01
 * ================================================== */

/** @brief Disconnect parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0006
*/
typedef struct __attribute__((packed)) ble_hci_cmd_disconnect_param_s
{
    uint16_t  conn_handle;        /**< Connection handle. */
    uint8_t   reason;             /**< Disconnect reason. */
} ble_hci_cmd_disconnect_param_t;


/** @brief Read remote version information command parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x001D
*/
typedef struct __attribute__((packed)) ble_hci_cmd_read_remote_ver_info_param_s
{
    uint16_t  conn_handle;          /**< Connection handle. */
} ble_hci_cmd_read_remote_ver_info_param_t;


/* ==================================================
 *  Information Parameters: OGF = 0x04
 * ================================================== */


/* ==================================================
 *  Status Parameter: OGF = 0x05
 * ================================================== */

/** @brief Read RSSI command parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0005
*/
typedef struct __attribute__((packed)) ble_hci_cmd_read_rssi_param_s
{
    uint16_t  conn_handle;          /**< Connection handle. */
} ble_hci_cmd_read_rssi_param_t;


/* ==================================================
 *  LE Controller Command: OGF = 0x08
 * ================================================== */

/** @brief Set random address parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0005
*/
typedef struct __attribute__((packed)) ble_hci_cmd_set_random_addr_param_s
{
    uint8_t   addr[BLE_ADDR_LEN];   /**< Address. */
} ble_hci_cmd_set_random_addr_param_t;


/** @brief Set advertising parameter parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0006
*/
typedef struct __attribute__((packed)) ble_hci_cmd_set_adv_param_param_s
{
    uint16_t  adv_interval_min;         /**< Advertising minimum interval. */
    uint16_t  adv_interval_max;         /**< Advertising maximum interval. */
    uint8_t   adv_type;                 /**< Advertising type. */
    uint8_t   own_addr_type;            /**< Own address type. */
    uint8_t   peer_addr_type;           /**< Peer address type. */
    uint8_t   peer_addr[BLE_ADDR_LEN];  /**< Peer address. */
    uint8_t   adv_channel_map;          /**< Advertiing channel map. */
    uint8_t   adv_filter_policy;        /**< Advertiing filter policy. */
} ble_hci_cmd_set_adv_param_param_t;


/** @brief Set advertising data parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0008
*/
typedef struct __attribute__((packed)) ble_hci_cmd_set_adv_data_param_s
{
    uint8_t   adv_data_length;          /**< Advertising data length. */
    uint8_t   adv_data[31];             /**< Advertising data. */
} ble_hci_cmd_set_adv_data_param_t;


/** @brief Set scan response parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0009
*/
typedef struct __attribute__((packed)) ble_hci_cmd_set_scan_rsp_param_s
{
    uint8_t   scan_rsp_data_length;     /**< Scan response data length. */
    uint8_t   scan_rsp_data[31];        /**< Scan response data. */
} ble_hci_cmd_set_scan_rsp_param_t;


/** @brief Set advertising enable parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x000A
*/
typedef struct __attribute__((packed)) ble_hci_cmd_set_adv_enable_param_s
{
    uint8_t   adv_enable;               /**< Advertising enable flag.. */
} ble_hci_cmd_set_adv_enable_param_t;


/** @brief Set scan parameter parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x000B
*/
typedef struct __attribute__((packed)) ble_hci_cmd_set_scan_param_param_s
{
    uint8_t   scan_type;                /**< Scan type. */
    uint16_t  scan_interval;            /**< Scan interval. */
    uint16_t  scan_window;              /**< Scan window. */
    uint8_t   own_addr_type;            /**< Own address type. */
    uint8_t   scan_filter_policy;       /**< Scan filter policy. */
} ble_hci_cmd_set_scan_param_param_t;


/** @brief Set scan enable parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x000C
*/
typedef struct __attribute__((packed)) ble_hci_cmd_set_scan_enable_param_s
{
    uint8_t scan_enable;                /**< Scan enable flag. */
    uint8_t scan_filter_duplicates;     /**< Control duplicate advertising reports filter. */
} ble_hci_cmd_set_scan_enable_param_t;


/** @brief Create connection parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x000D
*/
typedef struct __attribute__((packed)) ble_hci_cmd_create_conn_param_s
{
    uint16_t  scan_interval;            /**< Scan interval. */
    uint16_t  scan_window;              /**< scan window. */
    uint8_t   init_filter_policy;       /**< Initiator filter policy. */
    uint8_t   peer_addr_type;           /**< Peer address type. */
    uint8_t   peer_addr[BLE_ADDR_LEN];  /**< Peer address. */
    uint8_t   own_addr_type;            /**< Own address type. */
    uint16_t  conn_interval_min;        /**< Connection minimum interval. */
    uint16_t  conn_interval_max;        /**< Connection maximum interval. */
    uint16_t  max_latency;              /**< Max latency value. */
    uint16_t  supv_timeout;             /**< Connection supervision timeout. */
    uint16_t  min_celength;             /**< provide the Controller with the expected minimum length of the connection events.  */
    uint16_t  max_celength;             /**< provide the Controller with the expected maximum length of the connection events.  */
} ble_hci_cmd_create_conn_param_t;


/** @brief Add Device to filter accept list
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0011
*/
typedef struct __attribute__((packed)) ble_hci_cmd_add_device_to_accept_list_s
{
    uint8_t   addr_type;                /**< address type. */
    uint8_t   address[BLE_ADDR_LEN];    /**< address. */
} ble_hci_cmd_add_device_to_accept_list_t;


/** @brief Remove device from filter accept list
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0012
*/
typedef struct __attribute__((packed)) ble_hci_cmd_remove_device_from_accept_list_s
{
    uint8_t   addr_type;                /**< address type. */
    uint8_t   address[BLE_ADDR_LEN];    /**< address. */
} ble_hci_cmd_remove_device_from_accept_list_t;


/** @brief Connection updated parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0013
*/
typedef struct __attribute__((packed)) ble_hci_cmd_conn_updated_param_s
{
    uint16_t  conn_handle;          /**< Connection handle. */
    uint16_t  conn_interval_min;    /**< Connection minimum interval. */
    uint16_t  conn_interval_max;    /**< Connection maximum interval. */
    uint16_t  periph_latency;       /**< Peripheral latency. */
    uint16_t  supv_timeout;         /**< Supervision timeout. */
    uint16_t  min_celength;         /**< provide the Controller with the expected minimum length of the connection events.  */
    uint16_t  max_celength;         /**< provide the Controller with the expected maximum length of the connection events.  */
} ble_hci_cmd_conn_updated_param_t;


/** @brief LE set host channel classification command
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0014
*/
typedef struct __attribute__((packed)) ble_hci_cmd_le_channel_classification_s
{
    uint8_t   channel_map[5];          /**< channel map value. */
} ble_hci_cmd_le_channel_classification_t;


/** @brief LE read channel map command
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0015
*/
typedef struct __attribute__((packed)) ble_hci_cmd_le_read_channel_map_s
{
    uint16_t  conn_handle;          /**< Connection handle. */
} ble_hci_cmd_le_read_channel_map_t;


/** @brief LE Read remote features command parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0016
*/
typedef struct __attribute__((packed)) ble_hci_cmd_read_remote_features_s
{
    uint16_t  conn_handle;          /**< Connection handle. */
} ble_hci_cmd_read_remote_features_t;


/** @brief LE encrypt parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0017
*/
typedef struct __attribute__((packed)) ble_hci_le_encrypt_param_s
{
    uint8_t key[16];              /**< 128 bit key for the encryption of the data given in the command. */
    uint8_t plaintext_data[16];   /**< 128 bit data block that is requested to be encrypted. */
} ble_hci_le_encrypt_param_t;


/** @brief LE enable encrypt parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0019
*/
typedef struct __attribute__((packed)) ble_hci_le_enable_encrypt_param_s
{
    uint16_t conn_handle;               /**< Connection handle. */
    uint8_t  random_number[8];          /**< 64 bit random number. */
    uint8_t  encrypted_diversifier[2];  /**< 16 bit encrypted diversifier. */
    uint8_t  long_term_key[16];         /**< 128 bit long term key. */
} ble_hci_le_enable_encrypt_param_t;


/** @brief LE long term key request reply parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x001A
*/
typedef struct __attribute__((packed)) ble_hci_le_long_term_key_req_reply_param_s
{
    uint16_t conn_handle;               /**< Connection handle. */
    uint8_t  long_term_key[16];         /**< 128 bit long term key for the given connection.*/
} ble_hci_le_long_term_key_req_reply_param_t;


/** @brief LE long term key negative reply parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x001B
*/
typedef struct __attribute__((packed)) ble_hci_le_long_term_key_neg_reply_param_s
{
    uint16_t conn_handle;                 /**< Connection handle. */
} ble_hci_le_long_term_key_neg_reply_param_t;


/** @brief LE Remote connection parameter request reply command parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0020
*/
typedef struct __attribute__((packed)) ble_hci_cmd_le_remote_conn_param_req_reply_param_s
{
    uint16_t  conn_handle;          /**< Connection handle. */
    uint16_t  conn_interval_min;    /**< Connection minimum interval. */
    uint16_t  conn_interval_max;    /**< Connection maximum interval. */
    uint16_t  max_latency;          /**< Max latency value. */
    uint16_t  supv_timeout;         /**< Connection supervision timeout. */
    uint16_t  min_celength;         /**< provide the Controller with the expected minimum length of the connection events.  */
    uint16_t  max_celength;         /**< provide the Controller with the expected maximum length of the connection events.  */
} ble_hci_cmd_le_remote_conn_param_req_reply_param_t;


/** @brief LE Remote connection parameter request reply command parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0021
*/
typedef struct __attribute__((packed)) ble_hci_cmd_le_remote_conn_param_req_neg_reply_param_s
{
    uint16_t  conn_handle;          /**< Connection handle. */
    uint8_t   reason;               /**< reason that the connection parameter request was rejected. */
} ble_hci_cmd_le_remote_conn_param_req_neg_reply_param_t;


/** @brief set data length parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0022
*/
typedef struct __attribute__((packed)) ble_hci_cmd_set_data_length_param_s
{
    uint16_t conn_handle;   /**< Connection handle. */
    uint16_t tx_octets;     /**< Preferred maximum number of payload octets that the local Controller should include in a single LL Data PDU on this connection.Range 0x001B to 0x00FB*/
    uint16_t tx_time;       /**< Preferred maximum number of microseconds that the local Controller should use to transmit \n
                                 a single Link Layer packet containing an LL Data PDU on this connection.*/
} ble_hci_cmd_set_data_length_param_t;


/** @brief Write default data length parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0024
*/
typedef struct __attribute__((packed)) ble_hci_cmd_write_default_data_length_param_s
{
    uint16_t tx_octets;     /**< Preferred maximum number of payload octets that the local Controller should include in a single LL Data PDU on this connection.Range 0x001B to 0x00FB*/
    uint16_t tx_time;       /**< Preferred maximum number of microseconds that the local Controller should use to transmit \n
                                 a single Link Layer packet containing an LL Data PDU on this connection.*/
} ble_hci_cmd_write_default_data_length_param_t;


typedef struct __attribute__((packed)) ble_hci_cmd_add_device_to_resolving_list_param_s
{
    uint8_t   peer_id_addr_type;           /**< Peer identity address type. */
    uint8_t   peer_id_addr[BLE_ADDR_LEN];  /**< Peer identity address. */
    uint8_t   peer_irk[16];                /**< Peer identity resloving key. */
    uint8_t   local_irk[16];               /**< Local identity resloving key. */
} ble_hci_cmd_add_device_to_resolving_list_param_t;


/** @brief Remove device from resloving list parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0028
*/
typedef struct __attribute__((packed)) ble_hci_cmd_remove_device_from_resolving_list_param_s
{
    uint8_t   peer_id_addr_type;           /**< Peer identity address type. */
    uint8_t   peer_id_addr[BLE_ADDR_LEN];  /**< Peer identity address. */
} ble_hci_cmd_remove_device_from_resolving_list_param_t;


/** @brief Read peer resolvable address parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x002B
*/
typedef struct __attribute__((packed)) ble_hci_cmd_read_peer_resolvable_addr_param_s
{
    uint8_t   peer_id_addr_type;           /**< Peer identity address type. */
    uint8_t   peer_id_addr[BLE_ADDR_LEN];  /**< Peer identity address. */
} ble_hci_cmd_read_peer_resolvable_addr_param_t;


/** @brief Read local resolvable address parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x002C
*/
typedef struct __attribute__((packed)) ble_hci_cmd_read_local_resolvable_addr_param_s
{
    uint8_t   peer_id_addr_type;           /**< Peer identity address type. */
    uint8_t   peer_id_addr[BLE_ADDR_LEN];  /**< Peer identity address. */
} ble_hci_cmd_read_local_resolvable_addr_param_t;


/** @brief Set address resolution enable parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x002D
*/
typedef struct __attribute__((packed)) ble_hci_cmd_set_addr_resolution_enable_param_s
{
    uint8_t   addr_resolution_enable;           /**< address resolution enable setting. */
} ble_hci_cmd_set_addr_resolution_enable_param_t;


/** @brief set resolvable private address timeout parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x002E
*/
typedef struct __attribute__((packed)) ble_hci_cmd_set_rpa_timeout_s
{
    uint16_t   rpa_timeout;           /**< Resolvable private address timeout measured in seconds. */
} ble_hci_cmd_set_rpa_timeout_t;


/** @brief Read PHY parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0030
*/
typedef struct __attribute__((packed)) ble_hci_cmd_read_phy_param_s
{
    uint16_t  conn_handle;      /**< Connection handle. */
} ble_hci_cmd_read_phy_param_t;


/** @brief set PHY parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0031
*/
typedef struct __attribute__((packed)) ble_hci_cmd_set_default_phy_param_s
{
    uint8_t   all_phys;         /**< Preference among the transmitter or receiver PHYs supported by the Controller. */
    uint8_t   tx_phys;          /**< TX PHY. */
    uint8_t   rx_phys;          /**< RX PHY. */
} ble_hci_cmd_set_default_phy_param_t;


/** @brief set PHY parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0032
*/
typedef struct __attribute__((packed)) ble_hci_cmd_set_phy_param_s
{
    uint16_t  conn_handle;      /**< Connection handle. */
    uint8_t   all_phys;         /**< Preference among the transmitter or receiver PHYs supported by the Controller. */
    uint8_t   tx_phys;          /**< TX PHY. */
    uint8_t   rx_phys;          /**< RX PHY. */
    uint16_t  phy_options;      /**< Coded PHY option. */
} ble_hci_cmd_set_phy_param_t;


/** @brief Set advertising set random address parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0035
*/
typedef struct __attribute__((packed)) ble_hci_cmd_set_adv_set_random_addr_s
{
    uint8_t   adv_handle;                   /**< Advertising handle set. */
    uint8_t   random_addr[BLE_ADDR_LEN];    /**< random address. */
} ble_hci_cmd_set_adv_set_random_addr_t;


/** @brief Set extended advertising parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0036
*/
typedef struct __attribute__((packed)) ble_hci_cmd_set_extended_adv_param_s
{
    uint8_t   adv_handle;                   /**< Advertising handle set. */
    uint16_t  adv_evt_properties;           /**< Advertising event properties. */
    uint16_t  primary_adv_interval_min_l;   /**< Primary advertising interval minimum (low bytes). */
    uint8_t   primary_adv_interval_min_h;   /**< Primary advertising interval minimum (high byte). */
    uint16_t  primary_adv_interval_max_l;   /**< Primary advertising interval maximum (low bytes). */
    uint8_t   primary_adv_interval_max_h;   /**< Primary advertising interval maximum (high byte). */
    uint8_t   primary_adv_ch_map;           /**< Primary advertising channel map. */
    uint8_t   own_addr_type;                /**< own address type. */
    uint8_t   peer_addr_type;               /**< peer address type. */
    uint8_t   peer_addr[BLE_ADDR_LEN];      /**< peer address. */
    uint8_t   adv_filter_policy;            /**< advertising filter policy. */
    uint8_t   adv_tx_power;                 /**< advertising TX power. */
    uint8_t   primary_adv_phy;              /**< primary advertising phy. */
    uint8_t   secondary_adv_max_skip;       /**< secondary advertising maximum skip. */
    uint8_t   secondary_adv_phy;            /**< secondary advertising phy. */
    uint8_t   adv_sid;                      /**< advertising sid. */
    uint8_t   scan_req_notification_enable; /**< scan request notification enable. */
} ble_hci_cmd_set_extended_adv_param_t;


/** @brief Set extended advertising data parameter
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0037
*/
typedef struct __attribute__((packed)) ble_hci_cmd_set_extended_adv_data_s
{
    uint8_t   adv_handle;               /**< Advertising handle set. */
    uint8_t   operation;                /**< operation. */
    uint8_t   frag_preference;          /**< fragment preference. */
    uint8_t   adv_data_len;             /**< Advertising data length. */
    uint8_t   adv_data[];            /**< Advertising data. */
} ble_hci_cmd_set_extended_adv_data_t;


/** @brief Set extended scan response data parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0038
*/
typedef struct __attribute__((packed)) ble_hci_cmd_set_extended_scan_rsp_data_s
{
    uint8_t   adv_handle;               /**< Advertising handle set. */
    uint8_t   operation;                /**< operation. */
    uint8_t   frag_preference;          /**< fragment preference. */
    uint8_t   scan_rsp_data_len;        /**< Scan response data length. */
    uint8_t   scan_rsp_data[];       /**< Scan response data. */
} ble_hci_cmd_set_extended_scan_rsp_data_t;


/** @brief extended advertising enable parameters
 * @ingroup hci_cmd_param
*/
typedef struct __attribute__((packed)) extended_adv_enable_param_s
{
    uint8_t   adv_handle;             /**< advertising handle. */
    uint16_t  duration;               /**< duration. */
    uint8_t   max_extended_adv_evt;   /**< Maximum extended advertising event. */
} extended_adv_enable_param_t;


/** @brief Set extended advertising enable parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0039
*/
typedef struct __attribute__((packed)) ble_hci_cmd_set_extended_adv_enable_s
{
    uint8_t   enable;                           /**< enable or not. */
    uint8_t   num_sets;                         /**< number of sets. */
    extended_adv_enable_param_t param_sets[];   /**< extended advertising enable parameters sets. */
} ble_hci_cmd_set_extended_adv_enable_t;


/** @brief Remove advertising set parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x003C
*/
typedef struct __attribute__((packed)) ble_hci_cmd_remove_adv_set_t
{
    uint8_t   adv_handle;                   /**< advertising handle. */
} ble_hci_cmd_remove_adv_set_t;


/** @brief Set periodic advertising parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x003E
*/
typedef struct __attribute__((packed)) ble_hci_set_periodic_adv_param_s
{
    uint8_t   adv_handle;                   /**< advertising handle. */
    uint16_t  periodic_adv_interval_min;    /**< periodic advertising interval minumum. */
    uint16_t  periodic_adv_interval_max;    /**< periodic advertising interval maximum. */
    uint16_t  periodic_adv_properties;      /**< periodic advertising properties. */
} ble_hci_set_periodic_adv_param_t;


/** @brief Set periodic advertising data parameter
 * @ingroup hci_cmd_param
 * @detail OCF = 0x003F
*/
typedef struct __attribute__((packed)) ble_hci_cmd_set_periodic_adv_data_s
{
    uint8_t   adv_handle;               /**< Advertising handle set. */
    uint8_t   operation;                /**< operation. */
    uint8_t   adv_data_len;             /**< Advertising data length. */
    uint8_t   adv_data[];               /**< Advertising data. */
} ble_hci_cmd_set_periodic_adv_data_t;


/** @brief Set periodic advertising enable parameter
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0040
*/
typedef struct __attribute__((packed)) ble_hci_cmd_set_periodic_adv_enable_s
{
    uint8_t   enable;                   /**< enable or not. */
    uint8_t   adv_handle;               /**< Advertising handle set. */
} ble_hci_cmd_set_periodic_adv_enable_t;


typedef struct __attribute__((packed)) extended_scan_param_s
{
    uint8_t   scan_type;                /**< Scan Type. */
    uint16_t  scan_interval;            /**< Scan interval. */
    uint16_t  scan_window;              /**< Scan window. */
} extended_scan_param_t;


/** @brief Set extened scan parameter
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0041
*/
typedef struct __attribute__((packed)) ble_hci_cmd_set_extended_scan_param_s
{
    uint8_t   own_addr_type;            /**< own address type. */
    uint8_t   scan_filter_policy;       /**< scan filter policy. */
    uint8_t   scan_phy;                 /**< scan phy. */
    extended_scan_param_t scan_param[];  /**< parameters of another phys. */
} ble_hci_cmd_set_extended_scan_param_t;


/** @brief Set extened scan parameter
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0042
*/
typedef struct __attribute__((packed)) ble_hci_cmd_set_extended_scan_enable_s
{
    uint8_t   scan_enable;              /**< scan enable. */
    uint8_t   filter_duplicates;        /**< filter duplicates. */
    uint16_t  duration;                 /**< scan duration. */
    uint16_t  period;                   /**< scan period. */
} ble_hci_cmd_set_extended_scan_enable_t;


/** @brief extened create connection parameter
 * @ingroup hci_cmd_param
*/
typedef struct __attribute__((packed)) extended_create_conn_param_s
{
    uint16_t scan_interval;         /**< scan interval. */
    uint16_t scan_window;           /**< scan window. */
    uint16_t conn_interval_min;     /**< connection interval minimum. */
    uint16_t conn_interval_max;     /**< connection interval maximum. */
    uint16_t max_latency;           /**< connection maximum latency. */
    uint16_t supervision_timeout;   /**< connection supervision timeout. */
    uint16_t min_ce_length;         /**< minimum ce length. */
    uint16_t max_ce_length;         /**< maximum ce length. */
} extended_create_conn_param_t;


/** @brief Set extened create connection parameter
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0043
*/
typedef struct __attribute__((packed)) ble_hci_cmd_extended_create_conn_param_s
{
    uint8_t   initiator_filter_policy;          /**< initiator filter policy. */
    uint8_t   own_addr_type;                    /**< Own address type. */
    uint8_t   peer_addr_type;                   /**< Peer address type. */
    uint8_t   peer_addr[BLE_ADDR_LEN];          /**< peer address. */
    uint8_t   initiating_phys;                  /**< initiating phys. */
    extended_create_conn_param_t conn_param[];  /**< connection parameters. */
} ble_hci_cmd_extended_create_conn_param_t;


/** @brief Set periodic advertising create sync parameter
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0044
*/
typedef struct __attribute__((packed)) ble_hci_cmd_periodic_adv_create_sync_param_s
{
    uint8_t   options;                          /**< options select. */
    uint8_t   adv_sid;                          /**< Advertising SID. */
    uint8_t   advertiser_addr_type;             /**< Advertiser address type. */
    uint8_t   advertiser_addr[BLE_ADDR_LEN];    /**< Advertiser address. */
    uint16_t  skip;                             /**< Skip. */
    uint16_t  sync_timeout;                     /**< synchronization timeout. */
    uint8_t   sync_cte_type;                    /**< sync CTE type. */
} ble_hci_cmd_periodic_adv_create_sync_param_t;


/** @brief Set periodic advertising create sync parameter
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0046
*/
typedef struct __attribute__((packed)) ble_hci_cmd_periodic_adv_terminate_sync_param_s
{
    uint16_t   sync_handle;                          /**< Sync handle. */
} ble_hci_cmd_periodic_adv_terminate_sync_param_t;


/** @brief Add device to periodic advertiser list parameter
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0047
*/
typedef struct __attribute__((packed)) ble_hci_add_device_to_periodic_adv_list_param_s
{
    uint8_t   advertiser_addr_type;             /**< Advertiser address type. */
    uint8_t   advertiser_addr[BLE_ADDR_LEN];    /**< Advertiser address. */
    uint8_t   adv_sid;                          /**< Advertising SID. */
} ble_hci_add_device_to_periodic_adv_list_param_t;


/** @brief Remove device from periodic advertiser list parameter
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0048
*/
typedef struct __attribute__((packed)) ble_hci_rm_device_to_periodic_adv_list_param_s
{
    uint8_t   advertiser_addr_type;             /**< Advertiser address type. */
    uint8_t   advertiser_addr[BLE_ADDR_LEN];    /**< Advertiser address. */
    uint8_t   adv_sid;                          /**< Advertising SID. */
} ble_hci_rm_device_to_periodic_adv_list_param_t;


/** @brief set privacy mode parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x004E
*/
typedef struct __attribute__((packed)) ble_hci_cmd_set_privacy_mode_param_s
{
    uint8_t   peer_id_addr_type;           /**< Peer identity address type. */
    uint8_t   peer_id_addr[BLE_ADDR_LEN];  /**< Peer identity address. */
    uint8_t   privacy_mode;                /**< privacy mode. */
} ble_hci_cmd_set_privacy_mode_param_t;


/** @brief set connection CTE receive parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0054
*/
typedef struct __attribute__((packed)) ble_hci_cmd_set_conn_cte_rx_param_s
{
    uint16_t  conn_handle;      /**< Connection handle. */
    uint8_t   sampling_enable;  /**< Connection IQ sampling is enable or not setting. */
    uint8_t   slot_durations;   /**< Switching and sampling slots. */
    uint8_t   sw_pattern_length;/**< The number of Antenna IDs in the pattern. */
    uint8_t   antenna_ids[];    /**< List of Antenna IDs in the pattern. */
} ble_hci_cmd_set_conn_cte_rx_param_t;


/** @brief set connection CTE transmit parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0055
*/
typedef struct __attribute__((packed)) ble_hci_cmd_set_conn_cte_tx_param_s
{
    uint16_t  conn_handle;      /**< Connection handle. */
    uint8_t   cte_types;        /**< type of Constant Tone Extension. */
    uint8_t   sw_pattern_length;/**< The number of Antenna IDs in the pattern. */
    uint8_t   antenna_ids[];    /**< List of Antenna IDs in the pattern. */
} ble_hci_cmd_set_conn_cte_tx_param_t;


/** @brief set connection CTE request enable parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0056
*/
typedef struct __attribute__((packed)) ble_hci_cmd_set_conn_cte_req_param_s
{
    uint16_t  conn_handle;      /**< Connection handle. */
    uint8_t   enable;           /**< CTE request setting. */
    uint16_t  cte_req_interval; /**< The Constant Tone Extension Request procedure is initiated only once or periodically.. */
    uint8_t   req_cte_length;   /**< The minimum length of the Constant Tone Extension. */
    uint8_t   req_cte_type;     /**< The type of Constant Tone Extension that the Controller shall request from the remote device. */
} ble_hci_cmd_set_conn_cte_req_param_t;


/** @brief set connection CTE response enable parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0057
*/
typedef struct __attribute__((packed)) ble_hci_cmd_set_conn_cte_rsp_param_s
{
    uint16_t  conn_handle;      /**< Connection handle. */
    uint8_t   enable;           /**< CTE response setting. */
} ble_hci_cmd_set_conn_cte_rsp_param_t;


/** @brief set periodic advertising receive enable parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0059
*/
typedef struct __attribute__((packed)) ble_hci_set_periodic_adv_receive_en_param_s
{
    uint16_t  sync_handle;      /**< sync handle. */
    uint8_t   enable;           /**< enable or not. */
} ble_hci_set_periodic_adv_receive_en_param_t;


/** @brief periodic advertising sync transfer parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x005A
*/
typedef struct __attribute__((packed)) ble_hci_periodic_adv_sync_transfer_param_s
{
    uint16_t  conn_handle;      /**< connection handle. */
    uint16_t  service_data;     /**< service data. */
    uint16_t  sync_handle;      /**< sync handle. */
} ble_hci_periodic_adv_sync_transfer_param_t;


/** @brief periodic advertising set info transfer parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x005B
*/
typedef struct __attribute__((packed)) ble_hci_periodic_adv_set_info_transfer_param_s
{
    uint16_t  conn_handle;      /**< connection handle. */
    uint16_t  service_data;     /**< service data. */
    uint8_t   adv_handle;        /**< advertising handle. */
} ble_hci_periodic_adv_set_info_transfer_param_t;


/** @brief Set periodic advertising sync info transfer parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x005C
*/
typedef struct __attribute__((packed)) ble_hci_set_periodic_adv_sync_transfer_param_s
{
    uint16_t  conn_handle;      /**< connection handle. */
    uint8_t   mode;             /**< mode select. */
    uint16_t  skip;             /**< skip. */
    uint16_t  sync_timeout;     /**< sync timeout. */
    uint8_t   cte_type;         /**< cte type. */
} ble_hci_set_periodic_adv_sync_transfer_param_t;


/** @brief Set default periodic advertising sync info transfer parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x005D
*/
typedef struct __attribute__((packed)) ble_hci_default_periodic_adv_sync_transfer_s
{
    uint8_t   mode;             /**< mode select. */
    uint16_t  skip;             /**< skip. */
    uint16_t  sync_timeout;     /**< sync timeout. */
    uint8_t   cte_type;         /**< cte type. */
} ble_hci_default_periodic_adv_sync_transfer_t;


typedef struct __attribute__((packed)) ble_hci_subevent_param_s
{
    uint8_t subevent;
    uint8_t rsp_slot_start;
    uint8_t rsp_slot_count;
    uint8_t subevent_data_len;
    uint8_t subevent_data[];
}ble_hci_subevent_param_t;

/** @brief Set periodic advertising subevent data parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0082
*/
typedef struct __attribute__((packed)) ble_hci_set_periodic_adv_subevent_data_s
{
    uint8_t  adv_handle;          /**< sync handle. */
    uint8_t  num_subevents;            /**< request event. */
    ble_hci_subevent_param_t subevent[];        /**< response data. */
} ble_hci_set_periodic_adv_subevent_data_t;


/** @brief Set periodic advertising response data parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0083
*/
typedef struct __attribute__((packed)) ble_hci_set_periodic_adv_rsp_data_s
{
    uint16_t  sync_handle;          /**< sync handle. */
    uint16_t  req_event;            /**< request event. */
    uint8_t   req_subevent;         /**< request subevent. */
    uint8_t   rsp_subevent;         /**< response subevent. */
    uint8_t   rsp_slot;             /**< response slot. */
    uint8_t   rsp_len;              /**< response length. */
    uint8_t   rsp_data[];        /**< response data. */
} ble_hci_set_periodic_adv_rsp_data_t;


/** @brief Set periodic advertising sync subevent parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0084
*/
typedef struct __attribute__((packed)) ble_hci_set_periodic_sync_subevent_s
{
    uint16_t  sync_handle;                  /**< sync handle. */
    uint16_t  periodic_adv_properties;      /**< periodic advertising properties. */
    uint8_t   num_subevent;                 /**< number of subevent. */
    uint8_t   subevent[];                   /**< subevent. */
} ble_hci_set_periodic_sync_subevent_t;


/** @brief Set extened create connection parameter
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0085
*/
typedef struct __attribute__((packed)) ble_hci_cmd_extended_create_conn_v2_s
{
    uint8_t   adv_handle;                       /**< advertising handle. */
    uint8_t   subevent;                         /**< subevent. */
    uint8_t   initiator_filter_policy;          /**< initiator filter policy. */
    uint8_t   own_addr_type;                    /**< Own address type. */
    uint8_t   peer_addr_type;                   /**< Peer address type. */
    uint8_t   peer_addr[BLE_ADDR_LEN];          /**< peer address. */
    uint8_t   initiating_phys;                  /**< initiating phys. */
    extended_create_conn_param_t conn_param[];  /**< connection parameters. */
} ble_hci_cmd_extended_create_conn_v2_t;


/** @brief Set periodic advertising parameters v2
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0086
*/
typedef struct __attribute__((packed)) ble_hci_set_periodic_adv_param_v2_s
{
    uint8_t   adv_handle;                   /**< advertising handle. */
    uint16_t  periodic_adv_interval_min;    /**< periodic advertising interval minumum. */
    uint16_t  periodic_adv_interval_max;    /**< periodic advertising interval maximum. */
    uint16_t  periodic_adv_properties;      /**< periodic advertising properties. */
    uint8_t   num_subevents;                /**< number of subevents. */
    uint8_t   subevent_interval;            /**< subevent interval. */
    uint8_t   rsp_slot_delay;               /**< response slot delay. */
    uint8_t   rsp_slot_spacing;             /**< response slot spacing. */
    uint8_t   num_rsp_slots;                /**< number of response slots. */
} ble_hci_set_periodic_adv_param_v2_t;


/* ==================================================
 *  Rafael Vendor Command: OGF = 0x3F
 * ================================================== */

/** @brief Vendor command parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0001
*/
typedef struct __attribute__((packed)) ble_hci_vcmd_cntlr_info_param_s
{
    uint8_t             ble_public_addr[BLE_ADDR_LEN];  /**< BLE public address. */
    uint8_t             ble_version;                    /**< BLE stack version see Assigned Numbers (https://www.bluetooth.com/specifications/assigned-numbers). */
    uint16_t            ble_company_id;                 /**< BLE company ID, see Assigned Numbers (https://www.bluetooth.com/specifications/assigned-numbers). */
} ble_hci_vcmd_cntlr_info_param_t;


/** @brief Vendor command : scan request report parameters
 * @ingroup hci_cmd_param
 * @detail OCF = 0x0082
*/
typedef struct __attribute__((packed)) ble_hci_vcmd_scan_req_rpt_param_s
{
    uint8_t scan_report_status;  /**< BLE Report Status. */
} ble_hci_vcmd_scan_req_rpt_param_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __BLE_HCI_H__ */
