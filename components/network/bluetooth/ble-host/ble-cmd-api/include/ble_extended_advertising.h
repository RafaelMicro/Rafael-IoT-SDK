/** @file
 *
 * @brief BLE extended advertising header file.
 *
 */

/**
 * @defgroup ble_extended_advertising extended Advertising
 * @ingroup BLE_group
 * @{
 * @brief Define BLE extended Advertising definitions, structures, and functions.
 * @}
 */

#ifndef __BLE_EXTENDED_ADVERTISING_H__
#define __BLE_EXTENDED_ADVERTISING_H__

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "ble_gap.h"
#include "ble_api.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/



/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/
/**
 * @defgroup ble_adv_type BLE Advertising Type Definition
 * @{
 * @ingroup ble_extended_advertising
 * @brief  Define advertising type.
 */
typedef uint8_t ble_periodic_adv_mode_type_t;
#define DISABLE_ALL_PERIODIC_ADV_EVENT                   0x00  /**<. */
#define SYNC_TRANSFER_EVENT_ONLY                         0x01  /**<  */
#define PERIODIC_ADV_EVENT_WITHOUT_DUPLICATE_FILTER      0x02  /**< . */
#define PERIODIC_ADV_EVENT_WITH_DUPLICATE_FILTER         0x03  /**< . */

#define BLE_PERIODIC_ADV_PKT_SKIP_MAX                    0x01F3  /**< . */
#define BLE_PERIODIC_ADV_SYNC_TIMEOUT_MIN                0x000A  /**< . */
#define BLE_PERIODIC_ADV_SYNC_TIMEOUT_MAX                0x4000  /**< . */

#define BLE_PERIODIC_ADV_RSP_DATA_LEN_MAX                251  /**< . */

#define BLE_PERIODIC_ADV_DATA_LEN_MAX                    252  /**< . */
#define BLE_PERIODIC_ADV_INTERVAL_MIN                    0x0006  /**< . */

#define BLE_PERIODIC_ADV_SUBEVENT_INTERVAL_MIN           0x06  /**< N*1.25ms. */
#define BLE_PERIODIC_ADV_SUBEVENT_NUM_MAX                0x80  /**< . */


#define BLE_EXTENDED_ADV_PRIMARY_ADV_INTERVAL_MIN        0x000020  /**< . */

#define BLE_EXTENDED_ADV_PRIMARY_ADV_PHY_1M              0x01  /**< . */
#define BLE_EXTENDED_ADV_PRIMARY_ADV_PHY_CODED           0x03  /**< . */

#define BLE_EXTENDED_ADV_SECONDARY_ADV_PHY_1M            0x01  /**< . */
#define BLE_EXTENDED_ADV_SECONDARY_ADV_PHY_2M            0x02  /**< . */
#define BLE_EXTENDED_ADV_SECONDARY_ADV_PHY_CODED         0x03  /**< . */

#define BLE_EXTENDED_ADV_SID_MAX                         0x0F  /**< . */

/** @} */

typedef uint16_t adv_event_properties_t;
#define ADV_EVENT_PROPERITES_CONN_ADV                    (0x0001)  /*connectable advertising*/
#define ADV_EVENT_PROPERITES_SCAN_ADV                    (0x0002)  /*scannable advertising*/
#define ADV_EVENT_PROPERITES_DIRECT_ADV                  (0x0004)  /*directed advertising*/
#define ADV_EVENT_PROPERITES_HIGH_DUTY_DIRECT_CONN_ADV   (0x0008)  /*High duty cycle directed connectable advertising (<= 3.75ms)*/
#define ADV_EVENT_PROPERITES_LEGACY_ADV                  (0x0010)  /*Use legacy advertising PDU*/
#define ADV_EVENT_PROPERITES_OMIT_ADDR                   (0x0020)  /*Omit advertiser's address from all PDUs*/
#define ADV_EVENT_PROPERITES_INCLUDE_TX_POWER            (0x0040)  /*Include TxPower in the extended header of at least one adverising PDU*/

typedef uint8_t scan_req_notify_enable_t;
#define SCAN_REQ_NOTIFY_DISABLE                          (0x00)  /*scan request notifications disabled*/
#define SCAN_REQ_NOTIFY_ENABLE                           (0x01)  /*scan request notifications enabled*/



/** @brief BLE advertising parameters.
 * @ingroup ble_extended_advertising
*/
typedef struct  __attribute__((packed)) 
{
    /**
     * @ref ble_adv_type "BLE advertising type"
     */
    ble_periodic_adv_mode_type_t      mode;

    /**
     * @ref ble_addr_type_t "BLE GAP Address Type"
     */
    uint16_t                          skip;

    /**
     * @ref ble_addr_type_t "BLE GAP Address Type"
     */
    uint16_t                          sync_timeout; //unit: 10ms

} ble_default_periodic_adv_sync_transfer_param_t;


typedef struct  __attribute__((packed)) 
{
    /**
     * @ref ble_adv_type "BLE advertising type"
     */
    uint8_t                           host_id;
    /**
     * @ref ble_adv_type "BLE advertising type"
     */
    ble_periodic_adv_mode_type_t      mode;

    /**
     * @ref ble_addr_type_t "BLE GAP Address Type"
     */
    uint16_t                          skip;

    /**
     * @ref ble_addr_type_t "BLE GAP Address Type"
     */
    uint16_t                          sync_timeout;

} ble_periodic_adv_sync_transfer_param_t;

typedef struct  __attribute__((packed)) 
{
    /**
     * @ref ble_adv_type "BLE advertising type"
     */
    uint16_t                         sync_handle;
    /**
     * @ref ble_adv_type "BLE advertising type"
     */
    //uint8_t                          num_subevent;
    /**
     * @ref ble_addr_type_t "BLE GAP Address Type"
     */
    uint8_t                          subevent;

} ble_periodic_sync_subevent_param_t;

typedef struct  __attribute__((packed)) 
{
    uint16_t                         sync_handle;

} ble_periodic_advertising_sync_terminate_param_t;

typedef struct __attribute__((packed)) ble_periodic_adv_rsp_data_set_param_s
{
    uint16_t                          sync_handle;
    uint16_t                          req_event;
    uint8_t                           req_subevent;
    uint8_t                           rsp_subevent;
    uint8_t                           rsp_slot;
    uint8_t                           rsp_data_len;
    uint8_t                           rsp_data[];
} ble_periodic_adv_rsp_data_set_param_t;

typedef struct __attribute__((packed)) ble_periodic_adv_param_set_param_s
{
    uint8_t                           adv_handle;
    adv_event_properties_t            adv_event_properties;
    uint16_t                          padv_interval_min;
    uint16_t                          padv_interval_max;
    uint16_t                          padv_properties;
    uint8_t                           num_subevents;                /**< number of subevents. */
    uint8_t                           subevent_interval;            /**< interval between subevents. */
    uint8_t                           rsp_slot_delay;          /**< time between adv packet and the first response slot. */
    uint8_t                           rsp_slot_spacing;        /**< time between response slots. */
    uint8_t                           num_rsp_slot;            /**< number of subevent response slots. */
} ble_periodic_adv_param_set_param_t;

typedef struct __attribute__((packed)) ble_periodic_adv_data_set_param_s
{
    uint8_t                          adv_handle;
    uint8_t                          adv_data_len;
    uint8_t                          adv_data[];
} ble_periodic_adv_data_set_param_t;

typedef struct __attribute__((packed)) ble_periodic_adv_set_info_transfer_set_param_s
{
    uint8_t                          host_id;
    uint16_t                         service_data;
    uint8_t                          adv_handle;
} ble_periodic_adv_set_info_transfer_set_param_t;

typedef struct __attribute__((packed)) ble_periodic_adv_enable_set_param_s
{
    uint8_t                          adv_handle;
} ble_periodic_adv_enable_set_param_t;

typedef struct __attribute__((packed)) ble_periodic_adv_disable_set_param_s
{
    uint8_t                          adv_handle;
} ble_periodic_adv_disable_set_param_t;


typedef struct __attribute__((packed)) periodic_adv_subevent_param_
{
    uint8_t                          subevent;
    uint8_t                          rsp_slot_start;
    uint8_t                          rsp_slot_count;
    uint8_t                          subevent_data_len;
    uint8_t                          subevent_data[];
} periodic_adv_subevent_param_t;

typedef struct __attribute__((packed)) ble_periodic_adv_subevent_data_set_param_s
{
    uint8_t                          adv_handle;
    uint8_t                          num_subevents;
    uint8_t                          subevents[];
} ble_periodic_adv_subevent_data_set_param_t;

/**
 * @brief BLE Advertising Event Parameters.
 * @ingroup ble_advertising
*/
typedef struct __attribute__((packed)) ble_extended_adv_param_set_s
{
    uint8_t                           adv_handle;
    adv_event_properties_t            adv_event_properties;
    uint32_t                          primary_adv_interval_min;
    uint32_t                          primary_adv_interval_max;
    uint8_t                           primary_adv_ch_map;
    uint8_t                           own_address_type; 
    uint8_t                           peer_address_type;
    uint8_t                           peer_address[6];
    uint8_t                           adv_filter_policy;
    uint8_t                           primary_adv_phy;
    uint8_t                           secondary_adv_max_skip;
    uint8_t                           secondary_adv_phy;
    uint8_t                           adv_sid; 
    scan_req_notify_enable_t          scan_req_notification_enable;
} ble_extended_adv_param_set_t;


typedef struct __attribute__((packed)) ble_extended_adv_enable_set_s
{
    uint8_t                  enable;
    uint8_t                  adv_handle;
    uint16_t                 duration;
    uint8_t                  max_extended_adv_events;
} ble_extended_adv_enable_set_t;

typedef struct __attribute__((packed)) ble_extended_adv_data_set_s
{
    uint8_t                  adv_handle;
    uint8_t                  adv_data_len;
    uint8_t                  adv_data[];
} ble_extended_adv_data_set_t;

typedef struct __attribute__((packed)) ble_adv_set_random_address_set_s
{
    uint8_t                  adv_handle;
    uint8_t                  addr[BLE_ADDR_LEN];
} ble_adv_set_random_address_set_t;

typedef struct __attribute__((packed)) ble_evt_padv_set_padv_receive_enable_s
{
    /** @ref hci_cmd_param_error_code "ble_hci_error_code_t". */
    uint8_t     status;

} ble_evt_padv_set_padv_receive_enable_t;

/**
 * @brief Default Periodic Advertising Sync Transfer Parameter Event Parameters.
 * @ingroup ble_extended_advertising
*/
typedef struct __attribute__((packed)) ble_evt_padv_set_default_padv_sync_transfer_param_s
{
    /** @ref hci_cmd_param_error_code "ble_hci_error_code_t". */
    uint8_t     status;

} ble_evt_padv_set_default_padv_sync_transfer_param_t;

/**
 * @brief Periodic Advertising Sync Transfer Parameter Event Parameters.
 * @ingroup ble_extended_advertising
*/
typedef struct __attribute__((packed)) ble_evt_padv_set_padv_sync_transfer_param_s
{
    /** @ref hci_cmd_param_error_code "ble_hci_error_code_t". */
    uint8_t     status;

} ble_evt_padv_set_padv_sync_transfer_param_t;

typedef struct __attribute__((packed)) ble_evt_padv_set_padv_subevent_data_s
{
    uint8_t     status;
    uint8_t     adv_handle;
} ble_evt_padv_set_padv_subevent_data_t;

typedef struct __attribute__((packed)) ble_evt_padv_set_padv_response_data_s
{
    /** @ref hci_cmd_param_error_code "ble_hci_error_code_t". */
    uint8_t     status;
    uint16_t    sync_handle;
} ble_evt_padv_set_padv_response_data_t;

/**
 * @brief Periodic Advertising Sync Transfer Received Event Parameters.
 * @ingroup ble_extended_advertising
*/
typedef struct __attribute__((packed)) ble_evt_padv_padv_sync_transfer_received_s
{
    /** @ref hci_cmd_param_error_code "ble_hci_error_code_t". */
    uint8_t     status;
    uint8_t     host_id;
    uint16_t    service_data;              /**< Service data. */
    uint16_t    sync_handle;               /**< Sync handle. */
    uint8_t     advertising_sid;            /**< Advertising SID. */
    uint8_t     advertiser_address_type;    /**< Advertiser address type. */
    uint8_t     advertiser_address[6];      /**< Advertiser address. */
    uint8_t     advertiser_phy;             /**< Advertiser PHY. */
    uint16_t    periodic_adv_interval;     /**< Periodic advertising interval. */
    uint8_t     advertiser_clock_accuracy;  /**< Advertiser clock accuracy. */
} ble_evt_padv_padv_sync_transfer_received_t;

/**
 * @brief Periodic Advertising Sync Transfer Received Event Parameters.
 * @ingroup ble_extended_advertising
*/
typedef struct __attribute__((packed)) ble_evt_padv_padv_sync_transfer_received_v2_s
{
    /** @ref hci_cmd_param_error_code "ble_hci_error_code_t". */
    uint8_t     status;
    uint8_t     host_id;
    uint16_t    service_data;              /**< Service data. */
    uint16_t    sync_handle;               /**< Sync handle. */
    uint8_t     advertising_sid;            /**< Advertising SID. */
    uint8_t     advertiser_address_type;    /**< Advertiser address type. */
    uint8_t     advertiser_address[6];      /**< Advertiser address. */
    uint8_t     advertiser_phy;             /**< Advertiser PHY. */
    uint16_t    periodic_adv_interval;     /**< Periodic advertising interval. */
    uint8_t     advertiser_clock_accuracy;  /**< Advertiser clock accuracy. */
    uint8_t     num_subevents;              /**< Number of subevents. */
    uint8_t     subevent_interval;          /**< Subevent interval. */
    uint8_t     response_slot_delay;        /**< Response slot delay. */
    uint8_t     response_slot_spacing;      /**< Response slot spacing. */
} ble_evt_padv_padv_sync_transfer_received_v2_t;

/**
 * @brief Periodic Advertising Sync Established Event Parameters.
 * @ingroup ble_extended_advertising
*/
typedef struct __attribute__((packed)) ble_evt_padv_padv_sync_established_s
{
    /** @ref hci_cmd_param_error_code "ble_hci_error_code_t". */
    uint8_t     status;
    uint16_t    sync_handle;               /**< Sync handle. */
    uint8_t     advertising_sid;            /**< Advertising SID. */
    uint8_t     advertiser_address_type;    /**< Advertiser address type. */
    uint8_t     advertiser_address[6];      /**< Advertiser address. */
    uint8_t     advertiser_phy;             /**< Advertiser PHY. */
    uint16_t    periodic_adv_interval;     /**< Periodic advertising interval. */
    uint8_t     advertiser_clock_accuracy;  /**< Advertiser clock accuracy. */
} ble_evt_padv_padv_sync_established_t;

/**
 * @brief Periodic Advertising Sync Established v2 Event Parameters.
 * @ingroup ble_extended_advertising
*/
typedef struct __attribute__((packed)) ble_evt_padv_padv_sync_established_v2_s
{
    /** @ref hci_cmd_param_error_code "ble_hci_error_code_t". */
    uint8_t     status;
    uint16_t    sync_handle;               /**< Sync handle. */
    uint8_t     advertising_sid;            /**< Advertising SID. */
    uint8_t     advertiser_address_type;    /**< Advertiser address type. */
    uint8_t     advertiser_address[6];      /**< Advertiser address. */
    uint8_t     advertiser_phy;             /**< Advertiser PHY. */
    uint16_t    periodic_adv_interval;     /**< Periodic advertising interval. */
    uint8_t     advertiser_clock_accuracy;  /**< Advertiser clock accuracy. */
    uint8_t     num_subevents;              /**< Number of subevents. */
    uint8_t     subevent_interval;          /**< Subevent interval. */
    uint8_t     response_slot_delay;        /**< Response slot delay. */
    uint8_t     response_slot_spacing;      /**< Response slot spacing. */
} ble_evt_padv_padv_sync_established_v2_t;


/**
 * @brief Periodic Advertising Subevent Data Request Event Parameters.
 * @ingroup ble_extended_advertising
*/
typedef struct __attribute__((packed)) ble_evt_padv_padv_subevent_data_req_s
{
    uint8_t   adv_handle;                 /**< Advertising handle. */
    uint8_t   subevent_start;             /**< Subevent start. */
    uint8_t   subevent_data_cnt;          /**< Subevent data counter. */
} ble_evt_padv_padv_subevent_data_req_t;

typedef struct padv_rsp_report_s
{
    uint8_t tx_power;
    uint8_t rssi;
    uint8_t cte_type;
    uint8_t response_slot;
    uint8_t data_status;
    uint8_t data_length;
    uint8_t data[];
} padv_rsp_report_t;

/**
 * @brief Periodic Advertising Response Report Event Parameters.
 * @ingroup ble_extended_advertising
*/
typedef struct __attribute__((packed)) ble_evt_padv_padv_rsp_report_s
{
    uint8_t   adv_handle;                 /**< Advertising handle. */
    uint8_t   subevent;                   /**< Subevent. */
    uint8_t   tx_status;                  /**< Tx status. */
    uint8_t   num_responses;              /**< Number of responses. */
    padv_rsp_report_t rsp_info[];            /**< Response information */
} ble_evt_padv_padv_rsp_report_t;


/**
 * @brief Periodic Advertising Response Report Event Parameters.
 * @ingroup ble_extended_advertising
*/
typedef struct __attribute__((packed)) ble_evt_padv_set_psync_subevent_s
{
    uint8_t     status;
    uint16_t    sync_handle;               /**< Sync handle. */
} ble_evt_padv_set_psync_subevent_t;

/**
 * @brief Periodic Advertising Terminate Sync Event Parameters.
 * @ingroup ble_extended_advertising
*/
typedef struct __attribute__((packed)) ble_evt_padv_terminate_sync_s
{
    uint8_t     status;
} ble_evt_padv_terminate_sync_t;
/**
 * @brief Advertising Set Terminated Event Parameters.
 * @ingroup ble_extended_advertising
*/
typedef struct __attribute__((packed)) ble_evt_adv_set_terminated_s
{
    uint8_t     status;
    uint8_t     adv_handle;
    uint16_t    connect_handle;
    uint8_t     num_completed_ext_adv_events;
} ble_evt_adv_set_terminated_t;

/**
 * @brief Periodic Advertising Report Event Parameters.
 * @ingroup ble_extended_advertising
*/
typedef struct __attribute__((packed)) ble_evt_padv_padv_report_s
{
    uint16_t  sync_handle;                /**< Sync handle. */
    int8_t    tx_power;                   /**< tx power level. */
    int8_t    rssi;                       /**< RSSI value. */
    uint8_t   cte_type;                   /**< CTE Type. */
    uint8_t   data_status;                /**< data status. */
    uint8_t   data_length;                /**< data length. */
    uint8_t   data[];                     /**< data. */
} ble_evt_padv_padv_report_t;


/**
 * @brief Periodic Advertising Report v2 Event Parameters.
 * @ingroup ble_extended_advertising
*/
typedef struct __attribute__((packed)) ble_evt_padv_padv_report_v2_s
{
    uint16_t  sync_handle;                 /**< Sync handle. */
    int8_t    tx_power;                   /**< tx power level. */
    int8_t    rssi;                       /**< RSSI value. */
    uint8_t   cte_type;                   /**< CTE Type. */
    uint16_t  periodic_evt_cnt;           /**< Periodic Event Counter. */
    uint8_t   subevent;                   /**< Subevent. */
    uint8_t   data_status;                /**< data status. */
    uint8_t   data_length;                /**< data length. */
    uint8_t   data[];                     /**< data. */
} ble_evt_padv_padv_report_v2_t;


/**
 * @brief Periodic Advertising Sync Lost Event Parameters.
 * @ingroup ble_extended_advertising
*/
typedef struct __attribute__((packed)) ble_evt_padv_padv_sync_lost_s
{
    uint16_t  sync_handle;                 /**< Sync handle. */
} ble_evt_padv_padv_sync_lost_t;

/**
 * @brief Periodic Advertising Parameters Set Event Parameters.
 * @ingroup ble_extended_advertising
*/
typedef struct __attribute__((packed)) ble_evt_padv_padv_params_set_s
{
    uint8_t     status;
    uint8_t     adv_handle;               /**< advertising handle. */
} ble_evt_padv_padv_params_set_t;

typedef struct __attribute__((packed)) ble_evt_padv_padv_data_set_s
{
    uint8_t     status;
} ble_evt_padv_padv_data_set_t;

typedef struct __attribute__((packed)) ble_evt_padv_padv_enable_set_s
{
    uint8_t     status;
} ble_evt_padv_padv_enable_set_t;

typedef struct __attribute__((packed)) ble_evt_padv_padv_info_transfer_set_s
{
    uint8_t     status;
    uint8_t     host_id;
} ble_evt_padv_padv_info_transfer_set_t;

typedef struct __attribute__((packed)) ble_evt_extended_adv_params_s
{
    /** @ref hci_cmd_param_error_code "ble_hci_error_code_t". */
    uint8_t     status;
    uint8_t     selected_tx_power;

} ble_evt_extended_adv_params_t;

typedef struct __attribute__((packed)) ble_evt_extended_adv_enable_s
{
    /** @ref hci_cmd_param_error_code "ble_hci_error_code_t". */
    uint8_t     status;

} ble_evt_extended_adv_enable_t;

typedef struct __attribute__((packed)) ble_evt_extended_adv_data_s
{
    /** @ref hci_cmd_param_error_code "ble_hci_error_code_t". */
    uint8_t     status;

} ble_evt_extended_adv_data_t;

typedef struct __attribute__((packed)) ble_evt_adv_set_random_addr_s
{
    /** @ref hci_cmd_param_error_code "ble_hci_error_code_t". */
    uint8_t     status;

} ble_evt_adv_set_random_addr_t;

/**
 * @brief BLE Advertising Event Parameters.
 * @ingroup ble_extended_advertising
*/
typedef struct
{
    union
    {
        /** Set advertising parameter event parameters. */
        ble_evt_adv_set_random_addr_t                         evt_set_random_addr;
        ble_evt_extended_adv_params_t                         evt_set_extended_adv_params;
        ble_evt_extended_adv_enable_t                         evt_set_extended_adv_enable;
        ble_evt_extended_adv_data_t                           evt_set_extended_adv_data;
        ble_evt_padv_set_padv_receive_enable_t                evt_set_padv_receive_enable;
        ble_evt_padv_set_padv_sync_transfer_param_t           evt_set_padv_sync_transfer_param;
        ble_evt_padv_set_default_padv_sync_transfer_param_t   evt_set_default_padv_sync_transfer_param;
        ble_evt_padv_set_padv_subevent_data_t                 evt_set_padv_subevent_data;
        ble_evt_padv_set_padv_response_data_t                 evt_set_padv_rsp_data;
        ble_evt_padv_padv_sync_transfer_received_t            evt_padv_sync_transfer_received;
        ble_evt_padv_padv_sync_transfer_received_v2_t         evt_padv_sync_transfer_received_v2;
        ble_evt_padv_padv_sync_established_t                  evt_padv_sync_established;
        ble_evt_padv_padv_sync_established_v2_t               evt_padv_sync_established_v2;
        ble_evt_padv_padv_subevent_data_req_t                 evt_padv_subevent_data_req;
        ble_evt_padv_padv_rsp_report_t                        evt_padv_rsp_report;
        ble_evt_padv_set_psync_subevent_t                     evt_set_psync_subevent;            
        ble_evt_padv_padv_report_t                            evt_padv_report;
        ble_evt_padv_padv_report_v2_t                         evt_padv_report_v2;
        ble_evt_padv_padv_sync_lost_t                         evt_padv_sync_lost;
        ble_evt_padv_padv_params_set_t                        evt_padv_params_set;
        ble_evt_padv_padv_data_set_t                          evt_padv_data_set;
        ble_evt_padv_padv_enable_set_t                        evt_padv_enable_set;
        ble_evt_padv_padv_info_transfer_set_t                 evt_padv_set_info_transfer_set;
        ble_evt_padv_terminate_sync_t                         evt_padv_sync_terminate;
        ble_evt_adv_set_terminated_t                          evt_adv_set_terminated;

    } param;/** Event parameters. */

} ble_evt_periodic_adv_t;


/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/


/**************************************************************************************************
 *    GLOBAL PROTOTYPES
 *************************************************************************************************/
ble_err_t ble_cmd_extended_adv_parameter_set(ble_extended_adv_param_set_t *p_param);

ble_err_t ble_cmd_extended_adv_enable_set(uint8_t host_id, ble_extended_adv_enable_set_t *p_param);

ble_err_t ble_cmd_extended_adv_data_set(ble_extended_adv_data_set_t *p_param);

ble_err_t ble_cmd_adv_set_random_addr_set(ble_adv_set_random_address_set_t *p_param);

/** @brief Set BLE advertising parameters.
 *
 * @ingroup ble_extended_advertising
 *
 * @note    Advertising interval Min. and Max. : @ref ADV_INTERVAL_MIN to @ref ADV_INTERVAL_MAX \n
 *          Advertising interval Min. shall be less than or equal to advertising interval Max.
 *
 * @param[in] p_param : a pointer to advertising parameter.
 *
 * @return @ref BLE_ERR_OK is success or an @ref ble_err_t "error".
 */
ble_err_t ble_cmd_default_periodic_adv_sync_transfer_param_set(ble_default_periodic_adv_sync_transfer_param_t *p_param);

/** @brief Set BLE advertising parameters.
 *
 * @ingroup ble_extended_advertising
 *
 * @note    Advertising interval Min. and Max. : @ref ADV_INTERVAL_MIN to @ref ADV_INTERVAL_MAX \n
 *          Advertising interval Min. shall be less than or equal to advertising interval Max.
 *
 * @param[in] p_param : a pointer to advertising parameter.
 *
 * @return @ref BLE_ERR_OK is success or an @ref ble_err_t "error".
 */
ble_err_t ble_cmd_periodic_adv_sync_transfer_param_set(ble_periodic_adv_sync_transfer_param_t *p_param);

/** @brief Set BLE Periodic sync subevent.
 *
 * @ingroup ble_advertising
 *
 * @note    Advertising interval Min. and Max. : @ref ADV_INTERVAL_MIN to @ref ADV_INTERVAL_MAX \n
 *          Advertising interval Min. shall be less than or equal to advertising interval Max.
 *
 * @param[in] p_param : a pointer to advertising parameter.
 *
 * @return @ref BLE_ERR_OK is success or an @ref ble_err_t "error".
 */
ble_err_t ble_cmd_periodic_sync_subevent_set(ble_periodic_sync_subevent_param_t *p_param);

ble_err_t ble_cmd_periodic_advertising_sync_terminate(ble_periodic_advertising_sync_terminate_param_t *p_param);

ble_err_t ble_cmd_periodic_adv_receive_enable(uint16_t sync_handle);

ble_err_t ble_cmd_periodic_adv_receive_disable(uint16_t sync_handle);

ble_err_t ble_cmd_periodic_adv_rsp_data_set(ble_periodic_adv_rsp_data_set_param_t *p_param);

ble_err_t ble_cmd_periodic_adv_parameter_set(ble_periodic_adv_param_set_param_t *p_param);

ble_err_t ble_cmd_periodic_adv_enable_set(ble_periodic_adv_enable_set_param_t *p_param);

ble_err_t ble_cmd_periodic_adv_disable_set(ble_periodic_adv_disable_set_param_t *p_param);

ble_err_t ble_cmd_periodic_adv_data_set(ble_periodic_adv_data_set_param_t *p_param);

ble_err_t ble_cmd_periodic_adv_set_info_transfer_set(ble_periodic_adv_set_info_transfer_set_param_t *p_param);

ble_err_t ble_cmd_periodic_adv_subevent_data_set(ble_periodic_adv_subevent_data_set_param_t *p_param);

/** @brief BLE advertising events handler.
 *
 * @ingroup ble_extended_advertising
 *
 * @param p_param :  a pointer to the event parameter.
 *
 * @return @ref BLE_ERR_OK is success or an @ref ble_err_t "error".
 */
ble_err_t ble_evt_periodic_adv_handler(void *p_param);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __BLE_EXTENDED_ADVERTISING_H__*/
