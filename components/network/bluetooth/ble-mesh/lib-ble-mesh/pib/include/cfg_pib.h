// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
#ifndef __CFG_PIB_H__
#define __CFG_PIB_H__

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                Include (Better to prevent)
//=============================================================================
#include <stdint.h>

//=============================================================================
//                Public Definitions of const value
//=============================================================================
/**************************************************************************** */


//=============================================================================
//                Public ENUM
//=============================================================================
typedef enum
{
    MESH_PRV_SERVER,  /*!< Provisioner Server */
    MESH_PRV_CLIENT   /*!< Provisioner Client */
} meshPrvType;

typedef enum
{
    ROLE_SERVER,
    ROLE_CLIENT
} mesh_device_type_t;

//=============================================================================
//                Public Struct
//=============================================================================
#pragma pack(push)
#pragma pack(1)

typedef struct NET_KEY_TABLE
{
    uint16_t    net_key_index;
    uint8_t     key[16];
} net_key_table_t;

typedef struct APP_KEY_TABLE
{
    uint16_t    app_key_index;
    uint8_t     key[16];
} app_key_table_t;

//    uint8_t  cfg_UUID[16];
//    uint8_t  cfg_MACAddr[6];


typedef struct CFG_PIB
{
    uint16_t                 cfg_isProvisioned;
    uint16_t                 cfg_PrimaryAddress;
    uint32_t                 cfg_ivIndex;
    uint32_t                 cfg_TransportSequenceNumber;

    uint8_t                  cfg_DefaultTTL;
    uint8_t                  cfg_RelayState;
    uint8_t                  cfg_BeaconState;
    uint8_t                  cfg_ProxyState;
    uint8_t                  cfg_FriendState;
    uint8_t                  cfg_LowPowerState;
    uint8_t                  cfg_NodeIdentity;
    uint8_t                  cfg_NwkTransCount;
    uint8_t                  cfg_NwkIntvlStep;

    uint8_t                  cfg_DeviceKey[16];
    net_key_table_t          cfg_NetKeyTable[RAF_BLE_MESH_NET_KEY_LIST_SIZE];
    app_key_table_t          cfg_AppKeyTable[RAF_BLE_MESH_APP_KEY_LIST_SIZE];

    uint32_t                 cfg_RelayRetransCount;
    uint32_t                 cfg_RelayRetransIntvlSteps;

    uint32_t                 cfg_ivUpdateState;

    uint32_t                 cfg_ElementCount;
    ble_mesh_element_param_t *cfg_ElementBase;

    uint16_t                 cfg_ProvStartAddress;
    uint32_t                 cfg_ProvDevType;
    uint32_t                 cfg_ProvOOB;
    uint8_t                  cfg_ProvPublicKey[64];
    uint8_t                  cfg_ProvPrivateKey[32];
    uint32_t                 cfg_ProvCapAlogrithms;
    uint32_t                 cfg_ProvCapPublicKeyType;
    uint32_t                 cfg_ProvCapOOBStaticTypes;
    uint32_t                 cfg_ProvCapOOBOutputSize;
    uint32_t                 cfg_ProvCapOOBOutputActions;
    uint32_t                 cfg_ProvCapOOBInputSize;
    uint32_t                 cfg_ProvCapOOBInputAactions;

    uint32_t                 cfg_ProxyInterval;

    uint32_t                 cfg_CPRL;

} cfg_pib_t;

#pragma pack(pop)
//=============================================================================
//                Public Function Declaration
//=============================================================================

#ifdef __cplusplus
};
#endif
#endif /* __ADP_PIB_H__ */
