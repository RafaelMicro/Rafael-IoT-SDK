// ---------------------------------------------------------------------------
// Copyright (c) 2021
// All rights reserved.
//
// ---------------------------------------------------------------------------
#ifndef __MESH_NETWORK_H__
#define __MESH_NETWORK_H__

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                Include (Better to prevent)
//=============================================================================

#define NETWORK_SEQNUM_BITS         24
#define NETWORK_SEQNUM_MAX          ((1 << NETWORK_SEQNUM_BITS) - 1)
#define NETWORK_RELAY_RETRANSMITS_MAX   ((1 << 3) - 1)
#define NETWORK_RELAY_INTERVAL_STEPS_MAX ((1 << 5) - 1)
#define NETWORK_RELAY_INTERVAL_MAX_MS ((NETWORK_RELAY_INTERVAL_STEPS_MAX + 1) * 10)
#define NETWORK_MIN_IV_UPDATE_INTERVAL_MINUTES (96 * 60)
#define NETWORK_IV_RECOVERY_LIMIT 42


typedef enum
{
    NET_TX_SUCCESS,
    NET_TX_ERROR_INVALID_PARAMS,
    NET_TX_ERROR_NO_MEMORY,
} network_tx_status_t;

typedef enum
{
    NET_SUCCESS,
    NET_ERROR_INVALID_LENGTH,
    NET_ERROR_NOT_FOUND,
} network_status_t;
/** State of IV update procedure */
typedef enum
{
    /** In normal operation. */
    NET_STATE_IV_UPDATE_NORMAL,
    /** IV update procedure in progress. */
    NET_STATE_IV_UPDATE_IN_PROGRESS,
} network_state_iv_update_t;

typedef enum
{
    ENC_NONCE_NET   = 0x00,              /**< Nonce for network data. */
    ENC_NONCE_APP   = 0x01,              /**< Nonce for application data. */
    ENC_NONCE_DEV   = 0x02,              /**< Nonce for device key data. */
    ENC_NONCE_PROXY = 0x03               /**< Nonce for proxy configuration data. */
} enc_nonce_t;

typedef enum
{
    TX_ROLE_ORIGINATOR,            /**< The packet originated in this device. */
    TX_ROLE_RELAY,                 /**< The packet came from a different device and is being relayed. */
    TX_ROLE_COUNT                  /**< Number of roles available, not a valid role itself. */
} tx_role_t;

typedef enum
{
    TX_BEARER_TYPE_INVALID     = 0x00,
    TX_BEARER_TYPE_ADV         = 0x01,
    TX_BEARER_TYPE_GATT_SERVER = 0x02,
    TX_BEARER_TYPE_GATT_CLIENT = 0x04,
    TX_BEARER_TYPE_FRIEND      = 0x08,
    TX_BEARER_TYPE_LOW_POWER   = 0x10,
    TX_BEARER_TYPE_LOCAL       = 0x20,
    TX_BEARER_TYPE_ALLOW_ALL   = 0xFF,
} tx_bearer_selector_t;

typedef struct
{
    /** Network identifier. */
    uint8_t nid;
    /** Encryption key. */
    uint8_t encryption_key[MESH_KEY_SIZE_128];
    /** Privacy key. */
    uint8_t privacy_key[MESH_KEY_SIZE_128];

    uint8_t beacon_key[MESH_KEY_SIZE_128];

    uint8_t identify_key[MESH_KEY_SIZE_128];

    uint8_t network_id[MESH_NETID_SIZE];

} network_secmat_t;


typedef struct
{
    int8_t rssi;

    /** Network security material. */
    network_secmat_t *p_security_material;

    /** Packet destination address. */
    mesh_address_t dst;
    /** Address of the element the packet originates from (must be a unicast address). */
    uint16_t src;
    /** Time to live value for the packet, this is a 7 bit value. */
    uint8_t ttl;
    /** Flag indicating whether the packet is a control packet. */
    bool control_packet;

    /** Parameters generated internally. */
    struct
    {
        /** Sequence number of the message. */
        uint32_t sequence_number;
        /** IV index of the message. */
        uint32_t iv_index;
    } internal;
} network_meta_t;

typedef struct
{
    /** Data set by the user prior to allocation. */
    struct
    {
        /** Network metadata, used to fill header fields. */
        network_meta_t *p_metadata;
        /** Length of the payload. */
        uint16_t payload_len;
        uint16_t packet_len;
        /** The bearers on which the outgoing packets are to be sent on. Alternatively, use CORE_TX_BEARER_TYPE_ALLOW_ALL to allow allocation to all bearers. */
        tx_bearer_selector_t bearer_selector;
        /** Role this device has for the packet. */
        tx_role_t role;
        uint16_t transCnt;
        uint16_t transStep;
    } user_data;
    /** Pointer to the network data, set in the allocation. */
    uint8_t p_buffer[];
} network_tx_packet_buffer_t;

typedef struct
{
    network_meta_t *nwk_meta;
    uint8_t *pu8_data;
    uint8_t u8_datalen;
} nwk_data_req_t;

typedef struct
{
    uint8_t u8_interface;
    uint8_t u8_datalen;
    int8_t i8_rssi;
    uint8_t pu8_data[];
} nwk_data_idc_t;

void nwk_init(void);
void nwk_tx_complete(void);

void nwk_data_idc_handler(nwk_data_idc_t *p_nwk_dat_idc);
network_tx_status_t nwk_data_req_handler(nwk_data_req_t *p_nwk_data_req, uint8_t trans_cnt);
mesh_address_type_t mesh_address_type_get(uint16_t address);
bool network_packet_send(network_tx_packet_buffer_t *p_tx_buffer);
#ifdef __cplusplus
};
#endif
#endif /* __MESH_NETWORK_H__ */
