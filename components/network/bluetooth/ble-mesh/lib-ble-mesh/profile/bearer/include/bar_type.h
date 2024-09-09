// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
#ifndef __BAR_TYPE_H__
#define __BAR_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                Include (Better to prevent)
//=============================================================================


#define AD_TYPE_MESH_NWK                        (0x2a)              /**< AD type for Bluetooth mesh. */
#define AD_TYPE_BEACON                          (0x2b)              /**< AD type for Bluetooth mesh beacons. */
#define AD_TYPE_MESH_PROV                       (0x29)              /**< AD type for PB-ADV messages. */
#define AD_TYPE_USR_TEST                        (0xF1)

#define GATT_MSG_TYPE_PROXY_CFG                 0x02

typedef struct _bar_indication_tag_
{
    int8_t  rssi;
    uint8_t interface;
    uint8_t type;
    uint8_t peer_addr[6];
    uint8_t len;
    uint8_t data[];
} bar_indication_t;

typedef struct __attribute__((packed)) _adv_hdr_tag_
{
    uint8_t AdLen;
    uint8_t Type;
    uint8_t AdData[];
} adv_hdr_t;


typedef struct __attribute__((packed)) _pb_gatt_prv_ad_tag_
{
    uint8_t AdLen;
    uint8_t Hdr[11];
    uint8_t ServiceData[22];
} pb_gatt_prv_ad_t;


typedef struct __attribute__((packed)) _pb_gatt_pxy_net_id_ad_tag_
{
    uint8_t AdLen;
    uint8_t Hdr[12];
    uint8_t NetworkID[8];
} pb_gatt_pxy_net_id_ad_t;

typedef struct __attribute__((packed)) _pb_gatt_pxy_node_id_ad_tag_
{
    uint8_t AdLen;
    uint8_t Hdr[12];
    uint8_t NodeIDHash[8];
    uint8_t NodeIDRandom[8];
} pb_gatt_pxy_node_id_ad_t;


typedef struct _inf_list_tag_
{
    uint16_t isConnected: 1,
             Rvd: 7,
             HostId: 8;
} inf_list_t;

typedef struct _inf_cb_tag_
{
    //inf_list_t  InfList[GATT_CONNECTION_NUMBER + 2];
    inf_list_t  InfList[1 + 2];
    uint32_t    InfCount;
} inf_cb_t;


#ifdef __cplusplus
};
#endif
#endif /* __BAR_TYPE_H__ */
