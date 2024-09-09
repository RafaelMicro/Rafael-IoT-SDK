// ---------------------------------------------------------------------------
// Copyright (c) 2021
// All rights reserved.
//
// ---------------------------------------------------------------------------
#ifndef __UPPER_LAYER_H__
#define __UPPER_LAYER_H__

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                Include (Better to prevent)
//=============================================================================

typedef enum
{
    /** Selects 4 byte MIC size for the transport PDU */
    MESH_TRANSMIC_SIZE_SMALL,
    /** Selects 8 byte MIC size for the transport PDU */
    MESH_TRANSMIC_SIZE_LARGE,
    /** Selects default stack configured MIC size for the transport PDU */
    MESH_TRANSMIC_SIZE_DEFAULT,
    /** Invalid size */
    MESH_TRANSMIC_SIZE_INVALID
} mesh_transmic_size_t;


typedef struct
{
    /** Packet destination address. */
    mesh_address_t dst;
    uint16_t src;
    uint8_t ttl;

    bool force_segmented;

    mesh_transmic_size_t transmic_size;
    mesh_secmat_t security_material;

    uint32_t data_len;
    uint8_t data[];
} mesh_tx_params_t;

typedef struct
{
    /** Packet destination address. */
    packet_mesh_trs_packet_t t_packet;
    network_meta_t t_net_metadata;
    uint32_t trs_packet_len;
} trs_rx_params_t;


uint8_t upper_layer_dispatch_handle(mesh_tlv_t *pt_tlv, uint32_t u32_tlv_wait_tick);
ble_err_t mesh_upper_init(void *pt_param);
#ifdef __cplusplus
};
#endif
#endif /* __LOWER_LAYER_H__ */
