// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
#ifndef __BLE_MESH_LIB_API_H__
#define __BLE_MESH_LIB_API_H__

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                Include (Better to prevent)
//=============================================================================
#include "stdint.h"
#include "stdbool.h"

//=============================================================================
//                Private ENUM
//=============================================================================
//=============================================================================
//                Private Definitions of const value
//=============================================================================


//=============================================================================
//                Public Function Declaration
//=============================================================================
void ble_mesh_find_unprov_device_start(void);
void ble_mesh_find_unprov_device_stop(void);

void ble_mesh_provision_start(uint8_t from_gatt);
void ble_mesh_provision_stop(void);

void ble_mesh_proxy_enable(void);
void ble_mesh_proxy_disable(void);
void ble_mesh_select_unprovisioned_device(uint16_t primary_address, uint8_t *p_uuid);
bool ble_mesh_device_name_set(uint8_t len, uint8_t *p_device_name);
void ble_trsps_connection_create(void);
void ble_trsps_connection_cancel(void);
bool ble_trsps_data_set(uint16_t len, uint8_t *p_data);


#ifdef __cplusplus
};
#endif
#endif /* __BLE_MESH_LIB_API_H__*/
