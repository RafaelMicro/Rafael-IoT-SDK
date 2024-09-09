#ifndef __BAR_PB_GATT_H__
#define __BAR_PB_GATT_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint8_t host_id;
    uint8_t Type;
    uint8_t Len;
    uint8_t *pBuf;
} bar_pb_gatt_indication_t;

typedef struct __attribute((packed))
{
    uint8_t type; /**< See @ref proxy_adv_type_t for legal values. */
    union __attribute((packed))
    {
        struct __attribute((packed))
        {
            uint8_t network_id[MESH_NETID_SIZE];
        }
        network_id;
        struct __attribute((packed))
        {
            uint8_t hash[8];
            uint8_t random[8];
        }
        node_id;
    }
    params;
}
proxy_adv_service_data_t;

void bar_pb_gatt_adv_rsp_init(void);
void bar_pb_gatt_tx_secure_beacon(void);
void bar_pb_gatt_tx_data(uint8_t *p_buf, uint16_t buf_len);
void bar_pb_gatt_start_adv(uint32_t interval, uint8_t *p_buf, uint16_t buf_len);
void bar_pb_gatt_stop_adv(void);

void bar_pb_gatt_provision_enable(void);
void bar_pb_gatt_pkt_in(uint8_t *p_pdu, uint16_t pdu_len, uint8_t host_id);
void bar_pb_gatt_proxy_enable(void);
void bar_pb_gatt_proxy_enable_node_id(uint8_t *p_net_key);
void bar_pb_gatt_set_default_device_name(void);
void bar_pb_gatt_change_device_name(uint8_t len, uint8_t *p_name);

#ifdef __cplusplus
};
#endif
#endif /* __BAR_PB_GATT_H__ */
