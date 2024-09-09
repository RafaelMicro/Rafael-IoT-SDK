// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
#ifndef __BCN_TYPE_H__
#define __BCN_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                Include (Better to prevent)
//=============================================================================

typedef struct __attribute__((packed)) _sec_beacon_tag_
{
    uint8_t BcnType;
    uint8_t Flag_KeyRefresh: 1,
    Flag_IVUpdate: 1,
    Flag_RFU: 6;
    uint8_t NwkID[MESH_NWK_ID_NUM_BYTES];
    uint8_t IVIndex[MESH_IV_NUM_BYTES];
    uint8_t AuthValue[MESH_SEC_BEACON_AUTH_SIZE];
} secure_beacon_t;


typedef struct __attribute__((packed)) _pb_adv_unprv_beacon_tag_
{
    uint8_t Len;
    uint8_t Type;
    uint8_t BcnType;
    uint8_t UUID[MESH_LABEL_UUID_SIZE];
    uint8_t OOBInfo[MESH_OOB_INFO_SIZE];
} pb_adv_unprv_beacon_t;


typedef struct __attribute__((packed)) _pb_adv_sec_beacon_tag_
{
    uint8_t Len;
    uint8_t Type;
    uint8_t BcnType;
    uint8_t Flag_KeyRefresh: 1,
    Flag_IVUpdate: 1,
    Flag_RFU: 6;
    uint8_t NwkID[MESH_NWK_ID_NUM_BYTES];
    uint8_t IVIndex[MESH_IV_NUM_BYTES];
    uint8_t AuthValue[MESH_SEC_BEACON_AUTH_SIZE];
} pb_adv_sec_beacon_t;

typedef struct __attribute__((packed)) _pb_adv_rx_sec_beacon_tag_
{
    uint8_t BcnType;
    uint8_t Flag_KeyRefresh: 1,
    Flag_IVUpdate: 1,
    Flag_RFU: 6;
    uint8_t NwkID[MESH_NWK_ID_NUM_BYTES];
    uint8_t IVIndex[MESH_IV_NUM_BYTES];
    uint8_t AuthValue[MESH_SEC_BEACON_AUTH_SIZE];
} pb_adv_rx_sec_beacon_t;

typedef struct __attribute__((packed)) _unprvbcn_tag_
{
    uint8_t UUID[MESH_LABEL_UUID_SIZE];
    uint8_t OOB[MESH_OOB_INFO_SIZE];
} unprvbcn_t;

typedef struct __attribute__((packed)) _nwk_id_tag_
{
    uint8_t Type;
    uint8_t NwkID[8];
} nwk_id_t;


typedef struct __attribute__((packed)) _node_id_tag_
{
    uint8_t Type;
    uint8_t Hash[8];
    uint8_t Random[8];
} node_id_t;

typedef union __attribute__((packed)) _bcn_tag_
{
    unprvbcn_t  unprvbcn;
    nwk_id_t    nwk_id;
    node_id_t   node_id;
} bcn_t;



typedef struct __attribute__((packed)) _pb_gatt_adv_data_tag_
{
    uint8_t Header[GATT_SERVICE_DATA_HEADER_SIZE];
    bcn_t   Bcn;
} pb_gatt_adv_data_t;



typedef struct __attribute__((packed)) _pb_gatt_adv_nwk_id_tag_
{
    uint8_t Header[GATT_SERVICE_DATA_HEADER_SIZE];
    uint8_t Type;
    uint8_t NwkID[8];
} pb_gatt_adv_nwk_id_t;

typedef struct __attribute__((packed)) _pb_gatt_adv_nod_id_tag_
{
    uint8_t Header[GATT_SERVICE_DATA_HEADER_SIZE];
    uint8_t Type;
    uint8_t Hash[8];
    uint8_t Random[8];
} pb_gatt_adv_nod_id_t;


typedef struct _beacon_set_param_tag_
{
    bool     isEnabled;
    uint32_t Interval;
    uint32_t Duration;
    uint8_t  Interface;
} beacon_set_param_t;


#ifdef __cplusplus
};
#endif
#endif /* __BCN_TYPE_H__ */
