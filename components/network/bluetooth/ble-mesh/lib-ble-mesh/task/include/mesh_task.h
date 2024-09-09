// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
#ifndef __TASK_MAIN_H__
#define __TASK_MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                Include (Better to prevent)
//=============================================================================
#include "mesh_api.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "ble_api.h"

//=============================================================================
//                Private ENUM
//=============================================================================
#define MSG_TAG_BLE_BEARER_BASE             (0x0000)
#define MSG_TAG_PROVISION_BASE              (0x0100)
#define MSG_TAG_SECURITY_BASE               (0x0200)
#define MSG_TAG_NETWORKING_EVENT_BASE       (0x0400)
#define MSG_TAG_FOUNDATION_EVENT_BASE       (0x0800)
#define MSG_TAG_MESH_APP_BASE               (0x8000)


typedef enum MESH_LAYER_ID
{
    MESH_LAYER_START,
    MESH_LAYER_NETWORKING = MESH_LAYER_START,
    MESH_LAYER_BEARER,
    MESH_LAYER_MAX,
} e_mesh_layer_id;

typedef enum MESH_TASK_ERR
{
    MESH_TASK_ERR_SENDTO_POINTER_NULL       = -10,
    MESH_TASK_ERR_RECVFROM_FAIL             = -9,
    MESH_TASK_ERR_RECVFROM_POINTER_NULL     = -8,
    MESH_TASK_ERR_WRONG_CONFIG              = -7,
    MESH_TASK_ERR_SEMAPHORE_MALLOC_FAIL     = -6,
    MESH_TASK_ERR_TASK_MALLOC_FAIL          = -5,
    MESH_TASK_ERR_QUEUE_MALLOC_FAIL         = -4,
    MESH_TASK_ERR_DATA_MALLOC_FAIL          = -3,
    MESH_TASK_ERR_DUPLICATE_INIT            = -2,
    MESH_TASK_ERR_NOT_INIT                  = -1,
    MESH_TASK_ERR_OK                        = 0,
} e_mesh_task_err;


typedef enum MSG_TAG_DEF
{
    MSG_TAG_BEARER_HCI_EVENT = MSG_TAG_BLE_BEARER_BASE,
    MSG_TAG_BEARER_HCI_ACL_DATA,
    MSG_TAG_BEARER_ADV_TX_DATA,
    MSG_TAG_BEARER_ADV_TX_COMPLETE,
    MSG_TAG_BEARER_ADV_IDC_DATA,
    MSG_TAG_BEARER_CONNECTION_COMPLETE,
    MSG_TAG_BEARER_DISCONNECTION_COMPLETE,

    MSG_TAG_BEARER_GAPS_DEVICE_NAME_SET,

    MSG_TAG_BEARER_GATT_PROV_ENABLE,
    MSG_TAG_BEARER_GATT_PROV_DISABLE,
    MSG_TAG_BEARER_GATT_PROXY_ENABLE,
    MSG_TAG_BEARER_GATT_PROXY_DISABLE,

    MSG_TAG_BEARER_GATT_TX_DATA,
    MSG_TAG_BEARER_GATT_IDC_DATA,
    MSG_TAG_BEARER_TRSPS_TX_DATA,
    MSG_TAG_BEARER_SET_BEACON,
    MSG_TAG_BEARER_START_TX_SEC_BEACON,
    MSG_TAG_BEARER_STOP_TX_SEC_BEACON,

    MSG_TAG_BEARER_START_ADV_SCAN,
    MSG_TAG_BEARER_STOP_ADV_SCAN,

    MSG_TAG_BEARER_START_PROV,
    MSG_TAG_BEARER_STOP_PROV,
    MSG_TAG_BEARER_BLE_FOTA_TIMER_EXPIRY,

    MSG_TAG_BEARER_DBG_SET_ADDR_FILTER,

    MSG_TAG_BEARER_MAX = MSG_TAG_BEARER_DBG_SET_ADDR_FILTER,

    MSG_TAG_PROVISION_RX_PB_ADV_ATA = MSG_TAG_PROVISION_BASE,
    MSG_TAG_PROVISION_RX_MESH_UNPROVISIONED_BEACON,
    MSG_TAG_PROVISION_SELECT_DEVICE,
    MSG_TAG_PROVISION_SELECT_AUTH,
    MSG_TAG_PROVISION_START_BEACON,
    MSG_TAG_PROVISION_OOB_INPUT,
    MSG_TAG_PROVISION_OOB_OUTPUT,
    MSG_TAG_PROVISION_FAILED,
    MSG_TAG_PROVISION_MAX = MSG_TAG_PROVISION_FAILED,

    MSG_TAG_SECURITY_CCM_DECRYPT = MSG_TAG_SECURITY_BASE,
    MSG_TAG_SECURITY_CCM_ENCRYPT,
    MSG_TAG_SECURITY_PROV_PUBLIC_KEY_GEN,
    MSG_TAG_SECURITY_PROV_ECDH_CACULATE,
    MSG_TAG_SECURITY_PROV_CONFIRMATION_GEN,
    MSG_TAG_SECURITY_PROV_CONFIRMATION_CHK,
    MSG_TAG_SECURITY_PROV_SESSION_KEY_GEN,
    MSG_TAG_SECURITY_MAX = MSG_TAG_SECURITY_PROV_SESSION_KEY_GEN,

    MSG_TAG_NETWORKING_NWK_DATA_IDC = MSG_TAG_NETWORKING_EVENT_BASE,
    MSG_TAG_NETWORKING_RCV_CFG_MODEL_ACCESS_MSG_FROM_UPPER_LAYER,
    MSG_TAG_NETWORKING_BEACON_TIMER_START,
    MSG_TAG_NETWORKING_BEACON_TIMER_STOP,
    MSG_TAG_NETWORKING_RCV_MESH_CMD,
    MSG_TAG_NETWORKING_NWK_TX_DATA,
    MSG_TAG_NETWORKING_TRS_TX_DATA,
    MSG_TAG_NETWORKING_LPN_FD_REQ,
    MSG_TAG_NETWORKING_LPN_FD_TERMINATE,
    MSG_TAG_NETWORKING_LPN_SUB_ADD,
    MSG_TAG_NETWORKING_LPN_SUB_REMOVE,
    MSG_TAG_NETWORKING_MAX = MSG_TAG_NETWORKING_LPN_SUB_REMOVE,


    MSG_TAG_APP_UNPROVISIONED_RECEIVED = MSG_TAG_MESH_APP_BASE,
    MSG_TAG_APP_CAPABILITIES,
    MSG_TAG_APP_PROVISION_COMPLETE,
    MSG_TAG_APP_RF_RESET,
    MSG_TAG_APP_TEST = MSG_TAG_MESH_APP_BASE | 0xF1,
    MSG_TAG_APP_MAX = MSG_TAG_APP_TEST,

} msg_tag_def_t;

//=============================================================================
//                Private Definitions of const value
//=============================================================================
typedef struct MESH_LAYER_CONFIG
{
    uint32_t            u32_queue_size;
    const char          *p_thread_name;
    uint32_t            u32_thread_stacksize;
    uint32_t            u32_thread_priority;
    int                 (*pf_thread_init)(void);
    uint8_t       (*pf_thread_handle)(mesh_tlv_t *pt_tlv);
} mesh_layer_cfg_t;

typedef struct MESH_LAYER_HANDLE
{
    QueueHandle_t queue;
    TaskHandle_t  task;
} mesh_layer_handle_t;

typedef struct MESH_LAYER
{
    e_mesh_layer_id         e_layer_id;
    mesh_layer_handle_t     t_layer_handle;
    mesh_layer_cfg_t        *pt_cfg;
} mesh_layer_t;

typedef struct MESH_TASK_HANDLE
{
    uint32_t            ready;
    mesh_layer_t        t_layer[MESH_LAYER_MAX];
} mesh_task_handle_t;

//=============================================================================
//                Public Function Declaration
//=============================================================================
mesh_status_t mesh_queue_sendto(e_mesh_layer_id e_layer_id, mesh_tlv_t *pt_tlv);
uint32_t mesh_queue_remaining_size(e_mesh_layer_id e_layer_id);
ble_err_t mesh_callback_set(pf_mesh_evt_indication *pfn_callback);
int mesh_layer_task_init(e_mesh_layer_id e_layer_id, uint8_t task_priority);
int mesh_layer_queue_init(e_mesh_layer_id e_layer_id);

#ifdef __cplusplus
};
#endif
#endif /* __TASK_MAIN_H__*/
