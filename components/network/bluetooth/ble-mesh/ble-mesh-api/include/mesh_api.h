// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
#ifndef __MESH_API_H__
#define __MESH_API_H__

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                Include (Better to prevent)
//=============================================================================
#include "stdint.h"
#include "stdbool.h"
#include "ble_api.h"

typedef ble_err_t    (pf_mesh_evt_indication)(void *p_param);
extern pf_mesh_evt_indication *pf_app_indication;
//=============================================================================
//                Private ENUM
//=============================================================================
typedef enum MESH_API_TYPE
{
    TYPE_MESH_API_START                                     = 1,

    TYPE_MESH_PROV_START_REQ = TYPE_MESH_API_START,
    TYPE_MESH_PROV_STOP_REQ,
    TYPE_MESH_PROV_SCAN_REQ,
    TYPE_MESH_PROV_SEL_REQ,

    TYPE_MESH_DEVICE_NAME_SET_REQ,

    TYPE_MESH_PROXY_ENABLE_REQ,
    TYPE_MESH_PROXY_DISABLE_REQ,

    TYPE_MESH_BLE_TRSPS_CONNECT_CREATE_REQ,
    TYPE_MESH_BLE_TRSPS_CONNECT_CANCEL_REQ,
    TYPE_MESH_BLE_TRSPS_DATA_SET_REQ,

    TYPE_MESH_START_SCAN_UNPROV_DEVICE_REQ,
    TYPE_MESH_STOP_SCAN_UNPROV_DEVICE_REQ,

    TYPE_MESH_LINK_OPEN_UNPROV_DEVICE_REQ,

    TYPE_MESH_CFG_MDL_EVT_MSG_REQ,

    TYPE_MESH_APP_MDL_REG_REQ,
    TYPE_MESH_APP_MDL_EVT_MSG_REQ,

    TYPE_MESH_CMD_REQ                                       = 0x099,

    TYPE_MESH_PROV_START_CFM                                = 0x100,
    TYPE_MESH_PROV_SCAN_CFM,
    TYPE_MESH_PROV_SEL_CFM,

    TYPE_MESH_PROV_SCAN_IDC                                 = 0x200,
    TYPE_MESH_PROV_COMPLETE_IDC,
    TYPE_MESH_NODE_RESET_IDC,
    TYPE_MESH_UNPROV_DEVICE_IDC,
    TYPE_MESH_PROVISION_COMPLETE_IDC,

    TYPE_MESH_PROV_ADV_ENABLED_IDC,
    TYPE_MESH_PROV_ADV_DISABLED_IDC,
    TYPE_MESH_PROXY_ENABLE_IDC,
    TYPE_MESH_PROXY_DISABLE_IDC,

    TYPE_MESH_BLE_TRSPS_CONNECT_CREATE_IDC,
    TYPE_MESH_BLE_TRSPS_CONNECT_CANCEL_IDC,
    TYPE_MESH_BLE_TRSPS_CONNECTED_IDC,
    TYPE_MESH_BLE_TRSPS_DISCONNECTED_IDC,
    TYPE_MESH_FRIEND_ESTABLISED_IDC,
    TYPE_MESH_FRIEND_TERMINATED_IDC,

    TYPE_MESH_APP_MDL_EVT_MSG_IDC                           = 0x300,
    TYPE_MESH_CFG_MDL_EVT_MSG_IDC,
    TYPE_MESH_CFG_MDL_APP_BIND_IDC,

    TYPE_MESH_BLE_SVC_TRSPS_WRITE_IDC                       = 0x400,
} mesh_api_type_t;


//=============================================================================
//                Private Definitions of const value
//=============================================================================

typedef struct
{
    uint8_t     bearer_level;       /* Bluetooth LE task PRIORITY : bearer level */
    uint8_t     mesh_level;     /* Bluetooth LE task PRIORITY : ble mesh level*/
} mesh_task_priority_t;

typedef enum MESH_CMD_TYPE
{
    MESH_CMD_MIB_COUNTER = 0,
    MESH_CMD_MIB_COUNTER_CLEAR,
    MESH_CMD_SYSLOG_INFO,
    MESH_CMD_MAX,
} mesh_cmd_t;


typedef enum
{
    MESH_ERR_ACK_TIMOUT              = -24,
    MESH_ERR_NOT_FOUND               = -23,
    MESH_ERR_DUPLICATE_ADDR          = -22,
    MESH_ERR_NO_MEM                  = -21,
    MESH_ERR_INVALID_PARAM           = -20,
    MESH_ERR_INVALID_ADDR            = -19,
    MESH_ERR_INVALID_LENGTH          = -18,
    MESH_ERR_NULL                    = -17,
    MESH_ERR_BUSY                    = -16,
    MESH_ERR_TIMER_OP                = -15,
    MESH_ERR_ALLOC_MEMORY_FAIL       = -14,
    MESH_ERR_RECVFROM_LEN_NOT_ENOUGH = -13,
    MESH_ERR_RECVFROM_FAIL           = -12,
    MESH_ERR_SENDTO_FAIL             = -11,
    MESH_ERR_SENDTO_POINTER_NULL     = -10,
    MESH_ERR_APP_KEY_NOT_FOUND       = -9,
    MESH_ERR_WRONG_CONFIG            = -8,
    MESH_ERR_SEMAPHORE_MALLOC_FAIL   = -7,
    MESH_ERR_QUEUE_MALLOC_FAIL       = -6,
    MESH_ERR_DATA_MALLOC_FAIL        = -5,
    MESH_ERR_DUPLICATE_INIT          = -4,
    MESH_ERR_NOT_INIT                = -3,
    MESH_ERR_UNKNOW_TYPE             = -2,
    MESH_ERR_RESULT_WAITING          = -1,   
    MESH_SUCCESS                     = 0,
} mesh_status_t;


typedef enum
{
    MESH_FRIEND_TERMINATED_REASON_POLL_TIMEOUT, /* No response from the LPN within the Poll Timeout. */
    MESH_FRIEND_TERMINATED_REASON_NEW_FRIEND    /* The LPN established a friendship with a another Friend. */
} mesh_friend_terminated_reason_t;

typedef struct MESH_TLV_FORMAT
{
    uint16_t    type;       // Payload Identifier : defined by each module
    uint16_t    length;     // Payload Length : the length of the payload data
    uint8_t     value[];    // Payload Data
} mesh_tlv_t;

typedef struct MESH_LOCAL_CONFIG
{
    uint32_t type;
} mesh_local_cfg_t;

typedef struct MESH_APP_CONFIG
{
    pf_mesh_evt_indication  *pf_evt_indication;
    mesh_local_cfg_t        *pt_local_cfg;
} mesh_app_cfg_t;

typedef struct __attribute((packed))
{
    uint8_t     num_elements;
    uint16_t    algorithms;
    uint8_t     pubkey_type;
    uint8_t     oob_static_types;
    uint8_t     oob_output_size;
    uint16_t    oob_output_actions;
    uint8_t     oob_input_size;
    uint16_t    oob_input_actions;
}
ble_mesh_prov_evt_oob_caps_t;


typedef struct MESH_NWK_DATA_REQ
{
    uint8_t    addr_type;
    uint16_t   addr;
    uint8_t    *p_virtual_uuid;
    uint16_t   src;
    uint8_t    ttl;
    bool       control_packet;
    uint8_t    mpdu_len;
    uint8_t    mpdu[16];
} mesh_nwk_data_req_t;

typedef struct MESH_PROVISION_START_REQ
{
    uint32_t    gatt_provision;
    uint32_t    beacon_interval;
    uint8_t     uuid[16];
} mesh_prov_start_req_t;

typedef struct MESH_PROVISION_DEVICE_REQ
{
    uint16_t    primary_address;
    uint8_t     uuid[16];
} mesh_prov_device_req_t;

typedef struct MESH_PROVISION_START_CFM
{
    uint32_t    status;
    uint16_t    element_address;
} mesh_prov_start_cfm_t;

typedef struct MESH_PROVISION_SCAN_REQ
{
    uint32_t    scan_window;
} mesh_prov_scan_req_t;

typedef struct MESH_PROVISION_SCAN_CFM
{
    uint32_t    status;
} mesh_prov_scan_cfm_t;

typedef struct MESH_PROVISION_SCAN_IDC
{
    uint8_t      uuid[16];
} mesh_prov_scan_idc_t;

typedef struct MESH_PROVISION_SELECT_REQ
{
    uint8_t      uuid[16];
} mesh_prov_select_req_t;

typedef struct MESH_PROVISION_SELECT_CFM
{
    uint32_t    status;
} mesh_prov_select_cfm_t;

typedef struct __attribute__((packed))
{
    uint8_t    status;
    uint8_t    device_uuid[16];
    uint16_t   element_address;
    uint8_t    element_count;
    uint8_t    device_key[16];
}
mesh_prov_complete_idc_t;

typedef struct MESH_UNPROVISION_DEVICE_IDC
{
    uint8_t      uuid[16];
    uint8_t      peer_mac_addr[6];
    int8_t       rssi;
} mesh_unprov_device_idc_t;

typedef struct __attribute__((packed))
{
    uint8_t    status;
}
mesh_prov_adv_enable_idc_t;

typedef struct __attribute__((packed))
{
    uint8_t    status;
}
mesh_proxy_adv_enable_idc_t;

typedef struct MESH_MDL_REGIST_REQ
{
    bool        is_sig;
    uint32_t    model_id;
    uint16_t    element_address;
} mesh_mdl_reg_req_t;

typedef struct MESH_APP_MDL_EVT_MSG_REQ
{
    uint32_t ttl             : 8;
    uint32_t opcode_invalid  : 1;
    uint32_t appkey_index    : 12;
    uint32_t                 : 11;
    uint32_t dst_addr        : 16;
    uint32_t src_addr        : 16;
    uint32_t opcode;
    uint32_t parameter_len;
    uint8_t parameter[];
} mesh_app_mdl_evt_msg_req_t;

typedef struct MESH_CFG_MDL_EVT_MSG_REQ
{
    uint32_t dst_addr      : 16;
    uint32_t src_addr      : 16;
    uint32_t opcode;
    uint16_t netkey_index;
    uint16_t ttl           : 8;
    uint16_t mic_size      : 8;
    uint8_t  config_key[16];
    uint32_t parameter_len;
    uint8_t  parameter[];
} mesh_cfg_mdl_evt_msg_req_t;

typedef struct MESH_APP_MDL_EVT_MSG_IDC
{
    uint32_t src_addr           : 16;
    uint32_t dst_addr           : 16;
    uint32_t opcode;
    uint32_t appkey_index: 16;
    uint32_t parameter_len: 16;

    uint8_t parameter[];
} mesh_app_mdl_evt_msg_idc_t;

typedef struct MESH_CFG_MDL_EVT_MSG_IDC
{
    uint32_t opcode;
    uint32_t src_addr       : 16;
    uint32_t parameter_len  : 16;
    uint8_t parameter[];
} mesh_cfg_mdl_evt_msg_idc_t;

typedef struct MESH_CFG_MDL_APP_BIND_IDC
{
    uint16_t element_address;
    uint16_t appkey_index;
    uint32_t model_id;
} mesh_cfg_mdl_app_bind_idc_t;

typedef struct MESH_FRIEND_ESTABLISHED_IDC
{
    uint16_t lpn_address;
    uint16_t element_cnt;
} mesh_friend_established_idc_t;

typedef struct MESH_FRIEND_TERMINATED_IDC
{
    uint16_t                        lpn_address;
    mesh_friend_terminated_reason_t terminate_reason;
} mesh_friend_terminated_idc_t;


typedef struct MESH_CMD_REQUEST_PARAMETER
{
    uint32_t cmd_type;
    uint32_t cmd_length;
    uint32_t cmd_value[];
} mesh_cmd_req_t;

//=============================================================================
//                Public Function Declaration
//=============================================================================
int mesh_stack_init(mesh_task_priority_t *p_task_priority_cfg, pf_mesh_evt_indication *pfn_callback);
void mesh_cfm_send(mesh_status_t err_code, uint16_t dst, uint16_t src);
int mesh_evt_msg_sendto(mesh_tlv_t *pt_tlv);


#ifdef __cplusplus
};
#endif
#endif /* __MESH_API_H__*/
