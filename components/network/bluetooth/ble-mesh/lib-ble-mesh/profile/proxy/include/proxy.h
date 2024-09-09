// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
#ifndef __MESH_PROXY_H__
#define __MESH_PROXY_H__

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                Include (Better to prevent)
//=============================================================================
#include "proxy_filter.h"

//=============================================================================
//                Public Function Declaration
//=============================================================================
#define PROXY_CONFIG_PARAM_OVERHEAD (offsetof(proxy_config_msg_t, params))

typedef enum
{
    PROXY_CONFIG_OPCODE_FILTER_TYPE_SET = 0x00,
    PROXY_CONFIG_OPCODE_FILTER_ADDR_ADD = 0x01,
    PROXY_CONFIG_OPCODE_FILTER_ADDR_REMOVE = 0x02,
    PROXY_CONFIG_OPCODE_FILTER_STATUS = 0x03,
} proxy_config_opcode_t;


typedef struct __attribute((packed))
{
    uint8_t filter_type;
}
proxy_config_params_filter_type_set_t;

typedef struct __attribute((packed))
{
    uint16_t addrs[MESH_GATT_PROXY_FILTER_ADDR_COUNT];
}
proxy_config_params_filter_addr_add_t;

typedef struct __attribute((packed))
{
    uint16_t addrs[MESH_GATT_PROXY_FILTER_ADDR_COUNT];
}
proxy_config_params_filter_addr_remove_t;

typedef struct __attribute((packed))
{
    uint8_t filter_type;
    uint16_t list_size;
}
proxy_config_params_filter_status_t;

typedef struct __attribute((packed))
{
    uint8_t opcode;
    union __attribute((packed))
    {
        proxy_config_params_filter_type_set_t filter_type_set;
        proxy_config_params_filter_addr_add_t filter_addr_add;
        proxy_config_params_filter_addr_remove_t filter_addr_remove;
        proxy_config_params_filter_status_t filter_status;
    }
    params;
}
proxy_config_msg_t;

void proxy_cfg_data_indication(uint8_t *p_data, uint32_t length);
void proxy_net_packet_processed(network_meta_t *p_net_metadata, nwk_data_idc_t *p_nwk_dat_idc);
uint32_t proxy_net_packet_send(uint16_t u16_addr, uint8_t *pData, uint32_t u32_DataLen);

#ifdef __cplusplus
};
#endif
#endif /* __MESH_PROXY_H__ */
