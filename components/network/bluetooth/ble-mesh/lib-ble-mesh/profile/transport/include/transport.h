// ---------------------------------------------------------------------------
// Copyright (c) 2021
// All rights reserved.
//
// ---------------------------------------------------------------------------
#ifndef __MESH_TRANSPORT_H__
#define __MESH_TRANSPORT_H__

#ifdef __cplusplus
extern "C" {
#endif
//=============================================================================
//                Include (Better to prevent)
//=============================================================================

#ifndef TRANSPORT_SAR_SESSIONS_MAX
#define TRANSPORT_SAR_SESSIONS_MAX (30)
#endif

/* Number of canceled SAR RX sessions to be cached. Must be power of two. */
#ifndef TRANSPORT_CANCELED_SAR_RX_SESSIONS_CACHE_LEN
#define TRANSPORT_CANCELED_SAR_RX_SESSIONS_CACHE_LEN (20)
#endif

/** Default TTL value for SAR segmentation acknowledgments */
#ifndef TRANSPORT_SAR_SEGACK_TTL_DEFAULT
#define TRANSPORT_SAR_SEGACK_TTL_DEFAULT (20)
#endif

/** Default RX timeout. */
#define TRANSPORT_SAR_RX_TIMEOUT_DEFAULT_MS                (10ul * 1000ul)

/** Default base RX acknowledgement timeout. */
#define TRANSPORT_SAR_RX_ACK_BASE_TIMEOUT_DEFAULT_MS        (150ul)

/** Default per hop RX acknowledgement timeout addition. */
#define TRANSPORT_SAR_RX_ACK_PER_HOP_ADDITION_DEFAULT_MS    (50ul)

/** Default base TX retry timeout. */
#define TRANSPORT_SAR_TX_RETRY_BASE_TIMEOUT_DEFAULT_MS      (200ul)

/** Default per hop TX retry timeout addition. */
#define TRANSPORT_SAR_TX_RETRY_PER_HOP_ADDITION_DEFAULT_MS  (50ul)

/** Default number of retries before cancelling SAR TX session. */
#define TRANSPORT_SAR_TX_RETRIES_DEFAULT (4)

/** Maximum number of control packet consumers. Heartbeat + LPN_SM + LPN FSM + Friend. */
#if MESH_FEATURE_LPN_ENABLED && MESH_FEATURE_FRIEND_ENABLED
#define TRANSPORT_CONTROL_PACKET_CONSUMERS_MAX   (4)
#elif MESH_FEATURE_LPN_ENABLED
#define TRANSPORT_CONTROL_PACKET_CONSUMERS_MAX   (3)
#elif MESH_FEATURE_FRIEND_ENABLED
#define TRANSPORT_CONTROL_PACKET_CONSUMERS_MAX   (2)
#else
#define TRANSPORT_CONTROL_PACKET_CONSUMERS_MAX   (1)
#endif




/** Maximum difference in sequence numbers between two SAR segments of the same session
 * (@tagMeshSp section 3.5.3.1) */
#define TRANSPORT_SAR_SEQNUM_DIFF_MAX                           (8191)

/** The highest control packet opcode that can fit in the packet field,  */
#define TRANSPORT_CONTROL_PACKET_OPCODE_MAX (PACKET_MESH_TRS_CONTROL_OPCODE_MASK)

#define TRANSPORT_SAR_SEQAUTH_IV_INDEX_OFFSET (NETWORK_SEQNUM_BITS)

/** Mask for SeqZero in SeqAuth. */
#define TRANSPORT_SAR_SEQZERO_MASK (TRANSPORT_SAR_SEQNUM_DIFF_MAX)


typedef enum
{
    TRANSPORT_PACKET_RECEIVER_NONE      = 0,        /**< No one is receiving this packet. */
    TRANSPORT_PACKET_RECEIVER_SELF      = (1 << 0), /**< This node is receiving this packet. */
    TRANSPORT_PACKET_RECEIVER_FRIEND    = (1 << 1), /**< A friendship device is receiving this packet. */
} transport_packet_receiver_t;


typedef enum
{
    /** Sent by a SAR packet receiver to a sender to report the status of the SAR packet receive process. */
    TRANSPORT_CONTROL_OPCODE_SEGACK,
    /** Sent by a Low Power node to its Friend node to request any messages that it has stored for the Low Power node. */
    TRANSPORT_CONTROL_OPCODE_FRIEND_POLL,
    /** Sent by a Friend node to a Low Power node to inform it about security updates. */
    TRANSPORT_CONTROL_OPCODE_FRIEND_UPDATE,
    /** Sent by a Low Power node the all-friends fixed group address to start to find a friend. */
    TRANSPORT_CONTROL_OPCODE_FRIEND_REQUEST,
    /** Sent by a Friend node to a Low Power node to offer to become its friend. */
    TRANSPORT_CONTROL_OPCODE_FRIEND_OFFER,
    /** Sent to a Friend node to inform a previous friend of a Low Power node about the removal of a friendship. */
    TRANSPORT_CONTROL_OPCODE_FRIEND_CLEAR,
    /** Sent from a previous friend to Friend node to confirm that a prior friend relationship has been removed. */
    TRANSPORT_CONTROL_OPCODE_FRIEND_CLEAR_CONFIRM,
    /** Sent to a Friend node to add one or more addresses to the Friend Subscription List. */
    TRANSPORT_CONTROL_OPCODE_FRIEND_SUBSCRIPTION_LIST_ADD,
    /** Sent to a Friend node to remove one or more addresses from the Friend Subscription List. */
    TRANSPORT_CONTROL_OPCODE_FRIEND_SUBSCRIPTION_LIST_REMOVE,
    /** Sent by a Friend node to confirm Friend Subscription List updates. */
    TRANSPORT_CONTROL_OPCODE_FRIEND_SUBSCRIPTION_LIST_CONFIRM,
    /** Sent by a node to let other nodes determine topology of a subnet. */
    TRANSPORT_CONTROL_OPCODE_HEARTBEAT
} transport_control_opcode_t;

/** Control packet structure. */
typedef struct
{
    transport_control_opcode_t opcode; /**< Opcode of the control packet. */
    packet_mesh_trs_control_packet_t *p_data;  /**< Control packet data. */
    uint32_t data_len; /**< Length of the control packet data. */
    bool reliable; /**< Whether or not the packet is an acknowledged message. */
    uint16_t src; /**< Source address (must be a unicast address). */
    mesh_address_t dst;
    uint8_t ttl; /**< TTL value for the control packet. This is a 7 bit value. */
    network_secmat_t *p_net_secmat;
} transport_control_packet_t;


typedef struct
{
    uint16_t seq_zero;
    uint8_t segment_offset;
    uint8_t last_segment;
} transport_segmentation_metadata_t;

typedef struct
{
    bool segmented;
    transport_segmentation_metadata_t segmentation;

    uint8_t mic_size;
    union
    {
        struct
        {
            bool using_app_key;
            uint8_t app_key_id;
            uint16_t key_index;
        } access;
        struct
        {
            transport_control_opcode_t opcode;
        } control;
    } type;

    network_meta_t p_net_meta;
    application_secmat_t *p_security_material;
    transport_packet_receiver_t receivers;
} transport_packet_metadata_t;

typedef struct
{
    mesh_address_t dst;
    uint16_t src;
    uint8_t ttl;
    bool force_segmented;
    uint32_t transmic_size;

    network_secmat_t *p_security_material;
    transport_packet_metadata_t *pt_metadata;

    uint8_t *p_data;
    uint16_t data_len;
} transport_tx_params_t;

typedef struct
{
    mesh_address_t dst;
    uint16_t src;
    uint8_t ttl;

    bool force_segmented;

    uint8_t transmic_size;//mesh_transmic_size_t transmic_size;
    mesh_secmat_t security_material;

    uint32_t data_len;
    uint8_t data[];
} transport_data_req_t;


typedef struct
{
    packet_mesh_trs_packet_t *p_packet;
    network_meta_t *pt_nwk_meta;
    uint8_t packet_len;

} transport_data_indication_t;


static inline uint32_t transport_sar_first_seq_num_get(uint32_t sequence_number, uint16_t seq_zero)
{
    if ((sequence_number & TRANSPORT_SAR_SEQZERO_MASK) < seq_zero)
    {
        return ((sequence_number - ((sequence_number & TRANSPORT_SAR_SEQZERO_MASK) - seq_zero) - (TRANSPORT_SAR_SEQZERO_MASK + 1)));
    }
    else
    {
        return ((sequence_number - ((sequence_number & TRANSPORT_SAR_SEQZERO_MASK) - seq_zero)));
    }
}

static inline uint64_t transport_sar_seqauth_get(uint32_t iv_index, uint32_t sequence_number, uint16_t seq_zero)
{
    return ((uint64_t) iv_index << TRANSPORT_SAR_SEQAUTH_IV_INDEX_OFFSET)
           + transport_sar_first_seq_num_get(sequence_number, seq_zero);
}

uint32_t transport_control_tx(transport_control_packet_t *p_params);
void transport_data_idc(transport_data_indication_t *p_trs_data_idc);
mesh_status_t transport_data_req(transport_data_req_t *p_data_req, uint8_t status_req);
void transport_cache_init(void);
void transport_init(void);
#ifdef __cplusplus
};
#endif
#endif /* __MESH_TRANSPORT_H__ */
