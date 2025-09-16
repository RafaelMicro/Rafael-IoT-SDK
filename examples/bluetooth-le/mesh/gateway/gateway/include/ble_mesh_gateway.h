/**
 * @file uart_handler.h
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2022-03-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef __BLE_MESH_GATEWAY_H__
#define __BLE_MESH_GATEWAY_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "mesh_api.h"
#include "uart_handler.h"
//=============================================================================
//                Include (Better to prevent)
//=============================================================================

//=============================================================================
//                Public Definitions of const value
//=============================================================================


#define MESH_NWK_SVC_CMD                               0x10000000
#define MESH_NWK_OPCODE_UNPROV_DEVICE_SCAN_STATUS_GET  0x00
#define MESH_NWK_OPCODE_UNPROV_DEVICE_SCAN_STATUS_SET  0x01
#define MESH_NWK_OPCODE_UNPROV_DEVICE_SCAN_STATUS      0x02
#define MESH_NWK_OPCODE_UNPROV_DEVICE_LIST             0x03
#define MESH_NWK_OPCODE_DEVICE_PROV_SET                0x04
#define MESH_NWK_OPCODE_DEVICE_PROV_STAUS              0x05
#define MESH_NWK_OPCODE_NWK_KEY_GET                    0x10
#define MESH_NWK_OPCODE_NWK_KEY_SET                    0x11
#define MESH_NWK_OPCODE_NWK_KEY_STAUS                  0x12
#define MESH_NWK_OPCODE_APP_KEY_GET                    0x13
#define MESH_NWK_OPCODE_APP_KEY_SET                    0x14
#define MESH_NWK_OPCODE_APP_KEY_STAUS                  0x15
#define MESH_NWK_OPCODE_DEV_KEY_GET                    0x16
#define MESH_NWK_OPCODE_DEV_KEY_SET                    0x17
#define MESH_NWK_OPCODE_DEV_KEY_STAUS                  0x18


#define DEVICE_CONFIGURATION_SVC_CMD                   0x10001000
#define DEVICE_APPLICATION_SVC_CMD                     0x10002000

//=============================================================================
//                Public ENUM
//=============================================================================
//=============================================================================
//                Public Struct
//=============================================================================

typedef struct __attribute__((packed))
{
    uint8_t status;
    uint8_t nwk_key[16];
}
mesh_nwk_cmd_nwk_key_status;

typedef struct __attribute__((packed))
{
    uint8_t status;
    uint8_t app_key_idx;
    uint8_t app_key[16];
}
mesh_nwk_cmd_app_key_status;

typedef struct __attribute__((packed))
{
    uint8_t status;
    uint8_t dev_key[16];
}
mesh_nwk_cmd_dev_key_status;

//=============================================================================
//                Public Function Declaration
//=============================================================================

uint8_t ble_mesh_gateway_cmd_proc(uint32_t data, mesh_tlv_t *p_mesh_tlv);
uart_handler_data_sts_t ble_mesh_gateway_cmd_parser(uint8_t *p_buf, uint16_t len, uint16_t *p_data_len, uint16_t *p_offset);
void ble_mesh_gateway_cmd_send(uint32_t cmd_id, uint16_t addr, uint16_t key_index, uint32_t opcode, uint8_t *p_param, uint32_t len);

#ifdef __cplusplus
};
#endif
#endif /* __BLE_MESH_GATEWAY_H__ */
