// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
#ifndef __PRV_TYPE_H__
#define __PRV_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mesh_api.h"

typedef struct authenticationParams_tag
{
    uint8_t     confirmationInputs[MESH_PRV_CONFIRMATION_INPUTS_SIZE];
    uint8_t     tempRandomAndAuthValue[MESH_PRV_PDU_RANDOM_RANDOM_SIZE
                                       + MESH_PRV_AUTH_VALUE_SIZE];
    uint8_t     confirmationSaltAndFinalRandoms[MESH_PRV_CONFIRMATION_SALT_SIZE + 2 * MESH_PRV_PDU_RANDOM_RANDOM_SIZE];
    uint8_t     provisioningSalt[MESH_PRV_PROVISIONING_SALT_SIZE];
    uint8_t     sessionKey[16];
    uint8_t     sessionNonce[MESH_PRV_SESSION_NONCE_SIZE];
    uint8_t     confirmationKey[16];
    uint8_t     confirmation[MESH_PRV_PDU_CONFIRM_CONFIRM_SIZE];
    uint8_t     peerConfirmation[MESH_PRV_PDU_CONFIRM_CONFIRM_SIZE];
} t_authParams;

typedef struct ProvSessionData_tag
{
    struct startParams_tag
    {
        uint8_t     alog;
        uint8_t     oobPublicKey;
        uint8_t     authMethod;
        uint8_t     authAction;
        uint8_t     authSize;
    } startParams;

    uint8_t PubKeyX[32];      /*!< Pointer to the ECC Public Key X component, 32-octet array. */
    uint8_t PubKeyY[32];      /*!< Pointer to the ECC Public Key Y component, 32-octet array. */
    uint8_t PrivateKey[32];   /*!< Pointer to the ECC Private Key, 32-octet array. */

    t_authParams authParams;
    uint8_t peerUUID[16];
    uint8_t ecdhSecret[32];
    uint8_t provisioningDataAndMic[MESH_PRV_PDU_DATA_PARAM_SIZE];
    uint8_t deviceKey[16];
    uint16_t devAddr;
    uint16_t element_cnt;
    uint8_t oob_authMethod;
    uint8_t static_pbkey;
} ProvSessionData_t;


typedef struct PROV_SESSION_INFO
{
    uint8_t *pTxPrvPdu;                         /*!< Transmitted Prv PDU buffer. */
    uint32_t txTranTimeoutMs;                   /*!< Tx transaction timeout in ms. */
    uint16_t txTotalLength;                     /*!< Tx transaction PDU total length. */
    uint8_t txNextSegmentIndex;                 /*!< Tx transaction next segment index. */
    uint8_t txSegN;                             /*!< Tx transaction SegN value. */

    uint8_t *pRxPrvPdu;                         /*!< Received Prv PDU buffer. */
    uint32_t rxSegMask[2];                      /*!< PB Control PDU received callback. */
    uint16_t rxTotalLength;                     /*!< Rx transaction PDU total length. */
    uint8_t rxSegN;                             /*!< Rx transaction SegN value. */
    uint8_t rxFcs;                              /*!< Rx transaction FCS value. */
    bool rxAckSent;                             /*!< Rx transaction has been acked. */
    uint8_t rxLastReceivedOpcode;               /*!< Last received Provisioning PDU opcode. */

    uint8_t receivedTranNum;                    /*!< Received transaction number on the PB-ADV link. */
    uint8_t localTranNum;                       /*!< Local transaction number */

    uint32_t linkId;                            /*!< PB-ADV link indentifier. */
    uint8_t *pDeviceUuid;                       /*!< PB-ADV Device UUID. */
    bool linkOpened;                            /*!< TRUE if PB-ADV link is opened, FALSE otherwise. */
    uint8_t retry_cnt;                            /*!< TRUE if PB-ADV link is opened, FALSE otherwise. */
} prov_session_info;


/**
 * Unprovisioned node beacon received event structure.
 */
typedef struct
{
    /** Provisioning over GATT supported by the unprovisioned node.  */
    bool gatt_supported;
    /** Whether the beacon has a URI hash or not. */
    bool uri_hash_present;
    /** Hash of the URI in the beacon, or 0 if no URI is present. */
    uint8_t uri_hash[4];
    /** Device UUID of the unprovisioned node. */
    uint8_t device_uuid[16];
} ble_mesh_prov_evt_unprov_t;

typedef struct
{
    uint32_t u32_linkid;
    uint8_t pu8_uuid[16];
} ble_mesh_prov_evt_linkopen_t;

typedef struct
{
    uint8_t   alog;
    uint8_t   useOobPublicKey;
    uint8_t   oobAuthMethod;
    uint8_t   oobSize;
    uint16_t  oobAction;
} ble_mesh_prov_evt_sel_auth_t;

typedef struct
{
    uint32_t status;
    uint16_t element_cnt;
    uint16_t addr;
    uint8_t  uuid[16];
    uint8_t  devKey[16];
} ble_mesh_prov_evt_complete_t;

typedef struct
{
    uint32_t enable;
    uint32_t interval;
} ble_mesh_prov_evt_beacon_t;

/** Provisioning event structure. */
typedef struct
{
    /* Event paramenters. */
    union
    {
        ble_mesh_prov_evt_unprov_t              unprov;
        ble_mesh_prov_evt_linkopen_t            linkopen;
        ble_mesh_prov_evt_oob_caps_t            oob_caps;
        ble_mesh_prov_evt_sel_auth_t            sel_auth;
        ble_mesh_prov_evt_complete_t            complete_info;
        ble_mesh_prov_evt_beacon_t              beacon_req;
    } params;
} ble_mesh_prov_evt_t;
#ifdef __cplusplus
};
#endif
#endif /* __PRV_TYPE_H__ */

