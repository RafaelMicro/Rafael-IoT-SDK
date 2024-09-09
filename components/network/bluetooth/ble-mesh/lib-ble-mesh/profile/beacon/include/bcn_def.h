// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
#ifndef __BCN_DEF_H__
#define __BCN_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                Include (Better to prevent)
//=============================================================================


#define PB_ADV_UNPROVISION_BEACON_SIZE  20
#define PB_ADV_SEC_BEACON_SIZE          23



#define BEACON_TYPE_UNPROV        (0x00)  /**< Beacon type for unprovisioned beacons. */
#define BEACON_TYPE_SEC_NET_BCAST (0x01)  /**< Beacon type for secure network broadcast beacons. */
#define BEACON_TYPE_INVALID       (0xFF)  /**< Invalid beacon type. */


#define GATT_SERVICE_DATA_HEADER_SIZE       11
#define GATT_PROVISION_ADV_DATA_SIZE        (GATT_SERVICE_DATA_HEADER_SIZE + MESH_LABEL_UUID_SIZE + MESH_OOB_INFO_SIZE)
#define GATT_PROXY_NWK_ID_ADV_DATA_SIZE     (GATT_SERVICE_DATA_HEADER_SIZE + 1 + 8)
#define GATT_PROXY_NODE_ID_ADV_DATA_SIZE    (GATT_SERVICE_DATA_HEADER_SIZE + 1 + 8 + 8)


#ifdef __cplusplus
};
#endif
#endif /* __BCN_DEF_H__ */
