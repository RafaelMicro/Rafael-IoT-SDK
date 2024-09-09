// ---------------------------------------------------------------------------
// Copyright (c) 2021
// All rights reserved.
//
// ---------------------------------------------------------------------------
#ifndef __MESH_DSM_H__
#define __MESH_DSM_H__

#ifdef __cplusplus
extern "C" {
#endif
//=============================================================================
//                Include (Better to prevent)
//=============================================================================

#ifndef DSM_SUBNET_MAX
#define DSM_SUBNET_MAX          RAF_BLE_MESH_NET_KEY_LIST_SIZE
#endif
#ifndef DSM_APP_MAX
#define DSM_APP_MAX             RAF_BLE_MESH_APP_KEY_LIST_SIZE
#endif
#ifndef DSM_DEVICE_MAX
#define DSM_DEVICE_MAX          1
#endif
#ifndef DSM_VIRTUAL_ADDR_MAX
#define DSM_VIRTUAL_ADDR_MAX 4
#endif
#ifndef DSM_NONVIRTUAL_ADDR_MAX
#define DSM_NONVIRTUAL_ADDR_MAX 8
#endif

#ifndef DSM_MODEL_BINDING_MAX
#define DSM_MODEL_BINDING_MAX   20//RAF_BLE_MESH_MODEL_BIND_LIST_SIZE
#endif

#define DSM_PUBLISH_MAX  4
#define DSM_PUBLISH_VIRTUAL_MAX 4

/** Maximum number of addresses in total. */
#define DSM_ADDR_MAX (DSM_VIRTUAL_ADDR_MAX + DSM_NONVIRTUAL_ADDR_MAX)
/** Invalid handle index */
#define DSM_HANDLE_INVALID  (0xFFFF)

/** Maximum key index allowed. */
#define DSM_KEY_INDEX_MAX   (MESH_GLOBAL_KEY_INDEX_MAX)

/** The highest number of appkeys allowed on the device. */
#define DSM_APP_MAX_LIMIT   (247)
/** The highest number of subnets allowed on the device. */
#define DSM_SUBNET_MAX_LIMIT (252)
/** The highest number of addresses (virtual and group) allowed on the device. */
#define DSM_ADDR_MAX_LIMIT  (185)

#define ARRAY_SIZE(a)   (sizeof(a)/sizeof(a[0]))

/** Key index type, used for network key index and application key index. */
typedef uint16_t mesh_key_index_t;
/** DSM handle type, used for the handles returned for the each set of data added. */
typedef uint16_t dsm_handle_t;


typedef enum
{
    DSM_SUCCESS,
    DSM_ERROR_NULL,
    DSM_ERROR_NOT_FOUND,
    DSM_ERROR_INVALID_STATE,
    DSM_ERROR_INVALID_ADDR,
    DSM_ERROR_NO_MEM,
    DSM_ERROR_FORBIDDEN,
    DSM_ERROR_INVALID_DATA,
    DSM_ERROR_INVALID_PARAM,
    DSM_ERROR_INVALID_KEY_INDEX,
    DSM_ERROR_INVALID_NET_KEY_INDEX,
    DSM_ERROR_KEY_EXISTED,
    DSM_ERROR_INTERNAL,
    DSM_ERROR_INVALID_LENGTH,
} device_state_manager_status_t;



/*lint -e415 -e416 Lint fails to understand the boundary checking used for handles in this module (MBTLE-1831). */

/*****************************************************************************
* Local defines
*****************************************************************************/

/** Key index of the primary subnet. */
#define PRIMARY_SUBNET_INDEX        (0)


/** In order for the two to share address space, the first devkey handle starts
 * after the last appkey handle */
#define DSM_DEVKEY_HANDLE_START     DSM_APP_MAX

/** In order for the two to share address space, the first virtual address
 * handle starts after the last nonvirtual handle */
#define DSM_VIRTUAL_HANDLE_START     DSM_NONVIRTUAL_ADDR_MAX

#if MESH_FEATURE_LPN_ENABLED && MESH_FEATURE_FRIEND_ENABLED
#define MESH_FRIENDSHIP_CREDENTIALS (MESH_FRIEND_FRIENDSHIP_COUNT + 1)
#elif MESH_FEATURE_LPN_ENABLED
#define MESH_FRIENDSHIP_CREDENTIALS  1
#elif MESH_FEATURE_FRIEND_ENABLED
#define MESH_FRIENDSHIP_CREDENTIALS (MESH_FRIEND_FRIENDSHIP_COUNT)
#endif

/* Compares 2 application key identifiers. */
#define IS_AIDS_EQUAL(aid1, aid2) (((aid1) & PACKET_MESH_TRS_ACCESS_AID_MASK) == ((aid2) & PACKET_MESH_TRS_ACCESS_AID_MASK))

typedef enum
{
    DSM_ADDRESS_ROLE_SUBSCRIBE,
    DSM_ADDRESS_ROLE_PUBLISH
} dsm_address_role_t;


/*****************************************************************************
* Local typedefs
*****************************************************************************/
typedef struct _netkey_entry_tag_
{
    uint8_t             key[MESH_KEY_SIZE];
    network_secmat_t    secmat;
} netkey_entry_t;

typedef struct
{
    uint16_t                 NetKeyIndex: 12,
                             RFU: 1,
                             is_key_refreshing: 1,
                             NewKeyAvailable: 1,
                             isLocated: 1;

    netkey_entry_t           NetKey[2];

    uint8_t                  NetKeyCurrentHandel;

    mesh_key_refresh_phase_t key_refresh_phase;

    //    mesh_key_index_t net_key_index;

    //    uint8_t root_key[MESH_KEY_SIZE];
    //    uint8_t root_key_updated[MESH_KEY_SIZE];

    //    network_secmat_t secmat;
    //    network_secmat_t secmat_updated;


    uint8_t                  NodeIdentity;

    pb_adv_sec_beacon_t      *pPrvSecureBeaconPtr;

} subnet_t;

#if (MESH_FEATURE_LPN_ENABLED || MESH_FEATURE_FRIEND_ENABLED)
typedef struct
{
    dsm_handle_t subnet_handle;
    keygen_friendship_secmat_params_t secmat_params;

    network_secmat_t secmat;
    network_secmat_t secmat_updated;
} friendship_secmat_t;
#endif
typedef struct
{
    uint16_t                 app_key_index: 12,
                             RFU: 1,
                             is_key_refreshing: 1,
                             newKeyAvailable: 1,
                             isLocated: 1;

    dsm_handle_t subnet_handle; /**< Subnetwork this application key is bound to. */

    application_secmat_t secmat;

    /* store the new key when rx app key update command */
    uint8_t              UpdateKey[MESH_KEY_SIZE];

    //    bool key_updated;
    //    application_secmat_t secmat_updated;
} appkey_t;

/** Device key instance. */
typedef struct
{
    uint16_t key_owner; /**< Unicast address of the device that owns the devkey. */
    dsm_handle_t subnet_handle; /**< Subnetwork this device key is bound to. */
    application_secmat_t secmat; /**< Security material for packet encryption and decryption. */
} devkey_t;

typedef struct
{
    uint16_t address;
    uint8_t publish_count;
    uint8_t subscription_count;
} regular_address_t;

typedef struct
{
    uint16_t address;
    uint8_t publish_count;
    uint8_t subscription_count;
    /** 128-bit virtual label UUID. */
    uint8_t uuid[MESH_UUID_SIZE];
} virtual_address_t;

typedef struct
{
    uint8_t is_metadata_stored : 1;
    uint8_t is_load_failed : 1;
    uint8_t is_legacy_found : 1;
} local_dsm_status_t;


/**
 * Structure representing the unicast addresses assigned to this device.
 */
typedef struct
{
    uint16_t address_start; /**< First address in the range of unicast addresses. */
    uint16_t count;         /**< Number of addresses in the range of unicast addresses. */
} dsm_local_unicast_address_t;

typedef struct
{
    network_secmat_t *p_net;
    application_secmat_t *p_app;
} mesh_secmat_t;


typedef struct _model_binding_entry_tag_
{
    uint16_t Address;
    uint16_t AppKeyIndex;
    bool     isSIGModel;
    uint32_t ModleID;
} model_binding_entry_t;



/**
 * Initialize the device state manager.
 */

bool get_subnet_by_handle(uint16_t handle, subnet_t **ppSubNetInfo);
bool get_subnet_by_keyindex(uint16_t Keyindex, subnet_t **ppSubNetInfo);
bool check_subnet_by_keyindex(uint16_t Keyindex);

uint16_t dsm_netkey_get_list(uint8_t *pList);

uint16_t dsm_appkey_get_list(uint16_t NetkeyIndex, uint8_t *pList);

void net_secmat_local_key_index_set(uint16_t NetKeyIndex);

/**
 * Clear all stored state in the Device State Manager, including flash state.
 */
void dsm_clear(void);
void dsm_friendship_clear(uint16_t lpn_addr);

uint8_t appkey_check(void);

void appkey_update(uint8_t remove);
void dsm_app_key_update(uint8_t remove);

uint8_t dsm_init(void);
void dsm_app_key_init(void);

uint32_t dsm_node_id_set(uint16_t NetKeyIndex, uint8_t Identity);
uint32_t dsm_node_id_get(uint16_t NetKeyIndex, uint8_t *pIdentity);


uint32_t dsm_model_app_key_bind(bool isSIG, uint16_t Address, uint16_t KeyIndex, uint32_t Model);
uint32_t dsm_model_app_key_unbind(bool isSIG, uint16_t Address, uint16_t KeyIndex, uint32_t Model);
bool dsm_model_appkey_binding_status(uint16_t Address,  uint32_t Model, uint16_t Keyindex);

uint32_t dsm_local_unicast_addresses_set(const dsm_local_unicast_address_t *p_address);
uint16_t dsm_model_app_keyindex_list_get(bool isSIG, uint16_t Address, uint32_t Model, uint8_t *pListPtr);
uint16_t dsm_model_app_keyindex_get(uint16_t address, uint32_t model, uint16_t *p_key);


uint32_t dsm_address_get(dsm_handle_t address_handle, mesh_address_t *p_address);

dsm_handle_t dsm_appkey_index_to_appkey_handle(mesh_key_index_t appkey_index);

uint32_t dsm_subnet_add(mesh_key_index_t net_key_index, const uint8_t *p_key, dsm_handle_t *pKeyHandel);
uint32_t dsm_subnet_updt(uint16_t NetkeyIndex, const uint8_t *p_key);
uint32_t dsm_subnet_del(uint16_t NetKeyIndex);

uint32_t dsm_devkey_add(uint16_t raw_unicast_addr, dsm_handle_t subnet_handle, const uint8_t *p_key, dsm_handle_t *p_devkey_handle);
uint32_t dsm_appkey_add(mesh_key_index_t net_key_index, mesh_key_index_t app_key_index,  const uint8_t *p_key, dsm_handle_t *p_app_handle);

uint32_t dsm_appkey_update(uint16_t AppkKeyIndex, const uint8_t *p_key);
uint32_t dsm_appkey_delete(uint16_t AppKeyIndex);

uint32_t dsm_get_appkey_by_keyindex(uint16_t Keyindex, application_secmat_t *pSecmatPtr);

uint32_t dsm_sub_net_from_keyindex_get(mesh_key_index_t net_key_index, subnet_t **pp_sub_net);

uint16_t mesh_app_secmat_next_get(network_secmat_t *p_network_secmat, uint8_t aid,
                                  application_secmat_t **pp_app_secmat,
                                  application_secmat_t **pp_app_secmat_secondary);
void mesh_devkey_secmat_get(uint16_t owner_addr, application_secmat_t **pp_app_secmat);
bool mesh_rx_address_get(uint16_t raw_address, mesh_address_t *p_address);
void mesh_unicast_address_get(uint16_t *p_addr_start, uint16_t *p_addr_count);



mesh_key_refresh_phase_t mesh_key_refresh_phase_get(const network_secmat_t *p_secmat);



uint32_t dsm_key_phase_get(uint16_t keyindex, uint8_t *pKeyPhase);
uint32_t dsm_key_phase_set(uint16_t keyindex, uint8_t KeyPhase);





bool mesh_is_address_rx(mesh_address_t *p_addr);


bool dsm_network_id_search(uint8_t *pNwkId, uint16_t *pKeyIndx, uint8_t **ppBcnkey, dsm_handle_t *pSubnetHandle);

uint32_t dsm_get_client_device_key(uint16_t owner_address, uint8_t *dev_key);
void dsm_remote_dev_key_config(uint16_t dev_addr, uint8_t *dev_key);
void dsm_subnet_check_new_key(dsm_handle_t SubnetHandle, uint8_t *pNwkId);


#if (MESH_FEATURE_LPN_ENABLED || MESH_FEATURE_FRIEND_ENABLED)
uint32_t mesh_friendship_secmat_params_set(network_secmat_t *p_net, keygen_friendship_secmat_params_t *p_secmat_params);
void mesh_friendship_secmat_get(uint16_t lpn_addr, network_secmat_t **pp_secmat);

#endif
#if MESH_FEATURE_FRIEND_ENABLED
network_secmat_t *mesh_net_master_secmat_get(network_secmat_t *p_secmat);
#endif
#ifdef __cplusplus
};
#endif
#endif /* __MESH_DSM_H__ */
