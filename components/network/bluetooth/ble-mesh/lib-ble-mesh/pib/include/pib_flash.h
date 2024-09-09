

/**
 * @file mmdl_light_lughtness_sr.h
 * @author Joshua Jean (joshua.jean@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2022-08-23
 *
 * @copyright Copyright (c) 2022
 *
 */


#ifndef __PIB_FLASH_H__
#define __PIB_FLASH_H__



#ifdef __cplusplus
extern "C"
{
#endif

/* define the flash data type */
enum
{
    FLASH_DATA_SYSINFO = 1,
    FLASH_DATA_STATE,
    FLASH_DATA_NET_KEY_ENTRY,
    FLASH_DATA_APP_KEY_ENTRY,
    FLASH_DATA_MODEL_PUBLISH,
    FLASH_DATA_MODEL_SUBSCRIBE,
    FLASH_DATA_MODEL_BINDING
};

enum
{
    FLASH_DATA_ADD = 0,
    FLASH_DATA_DELETE,
    FLASH_DATA_UPDATE
};


#define FLASH_DATA_START_ADDRESS        0x0000F1000
#define FLASH_BLOCK_SIZE                0x10
#define FLASH_PAGE_SIZE                 0x1000
#define FLASH_DATA_SIZE                 (FLASH_PAGE_SIZE * 3)
#define FLASH_BLOCKS_PER_PAGE           (FLASH_PAGE_SIZE/FLASH_BLOCK_SIZE)
#define FLASH_TOTAL_BLOCKS              (FLASH_DATA_SIZE/FLASH_BLOCK_SIZE)
#define FLASH_EMPTY_VALUE               0xFFFFFFFF
#define FLASH_DATA_TAG_VALID            0xFF
#define FLASH_DATA_INDEX_INVLID         0xFFFF

#define FLASH_ACCESS_ERROR              -1
#define FLASH_ACCESS_OUT_OF_RESOURCE    -2
#define FLASH_ACCESS_INDEX_INVALID      -3
#define FLASH_ACCESS_SUCCESS            0

#define FLASH_BLOCK_ADDRESS_GET(x)      (((uint8_t*)((void*)FLASH_DATA_START_ADDRESS)) + (x * FLASH_BLOCK_SIZE))


/* the following code is optimizing the flash storage for retention data */
#pragma pack(push)
#pragma pack(1)

typedef struct _flash_data_header_
{
    uint8_t data_type;
    uint8_t data_len;
    uint8_t data_checkssum;
    uint8_t data_status;
    uint8_t data[];
} flash_data_header_t;

typedef struct _flash_system_info_s
{
    uint16_t        cfg_PrimaryAddress;
    uint8_t         cfg_DeviceKey[16];
} flash_system_info_t;

typedef struct _flash_state_s
{
    uint32_t         cfg_TransportSequenceNumber;
    uint32_t         cfg_ivIndex;
    uint8_t          cfg_DefaultTTL;
    uint8_t          cfg_RelayState;
    uint8_t          cfg_BeaconState;
    uint8_t          cfg_ProxyState;
    uint8_t          cfg_FriendState;
    uint8_t          cfg_LowPowerState;
    uint8_t          cfg_RelayRetransCount;
    uint8_t          cfg_RelayRetransIntvlSteps;
    uint8_t          cfg_NodeIdentity;
} flash_state_t;

typedef struct _flash_net_key_entry_s
{
    uint16_t    net_key_index;
    uint8_t     key[16];
} flash_net_key_entry_t;

typedef struct _flash_app_key_entry_s
{
    uint16_t    app_key_index;
    uint16_t    net_key_index;
    uint8_t     key[16];
} flash_app_key_entry_t;

typedef struct _flash_model_binding_list_s
{
    uint8_t     element_index;
    uint8_t     model_index;
    uint16_t    binding_list[RAF_BLE_MESH_SUBSCRIPTION_LIST_SIZE];
} flash_model_binding_list_t;

typedef struct _flash_model_publish_info_s
{
    uint8_t     element_index;
    uint8_t     model_index;
    publication_info_t  publish_info;
} flash_model_publish_info_t;

typedef struct _flash_model_subscribe_list_s
{
    uint8_t     element_index;
    uint8_t     model_index;
    uint16_t    subscribe_list[RAF_BLE_MESH_SUBSCRIPTION_LIST_SIZE];
} flash_model_subscribe_list_t;

#pragma pack(pop)

void pib_flash_data_load(void);

#ifdef __cplusplus
};
#endif

#endif /* __PIB_FLASH_H__ */

