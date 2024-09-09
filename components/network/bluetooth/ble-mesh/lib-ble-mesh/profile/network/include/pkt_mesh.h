// ---------------------------------------------------------------------------
// Copyright (c) 2021
// All rights reserved.
//
// ---------------------------------------------------------------------------
#ifndef __MESH_PKT_MESH_H__
#define __MESH_PKT_MESH_H__

#ifdef __cplusplus
extern "C" {
#endif
//=============================================================================
//                Include (Better to prevent)
//=============================================================================


#ifndef MESH_FEATURE_LPN_ACT_AS_REGULAR_NODE_OUT_OF_FRIENDSHIP
#define MESH_FEATURE_LPN_ACT_AS_REGULAR_NODE_OUT_OF_FRIENDSHIP 0
#endif

/** Friend feature. */
#ifndef MESH_FEATURE_FRIEND_ENABLED
#define MESH_FEATURE_FRIEND_ENABLED 1
#endif

/** Number of friendships supported simultaneously. */
#ifndef MESH_FRIEND_FRIENDSHIP_COUNT
#define MESH_FRIEND_FRIENDSHIP_COUNT 4
#endif

/** Size of the Friend Subscription List (per friendship). */
#ifndef MESH_FRIEND_SUBLIST_SIZE
#define MESH_FRIEND_SUBLIST_SIZE 5
#endif

//Size of the Friend Queue (per friendship).
#ifndef MESH_FRIEND_QUEUE_SIZE
#define MESH_FRIEND_QUEUE_SIZE 16
#endif

#define PACKET_MESH_TRS_CONTROL_FRIEND_CLEAR_LPN_ADDRESS0_OFFSET   (0)          /**< Offset to the trs_control_friend_clear_lpn_address field (0).*/
#define PACKET_MESH_TRS_CONTROL_FRIEND_CLEAR_LPN_ADDRESS1_OFFSET   (1)          /**< Offset to the trs_control_friend_clear_lpn_address field (1).*/

#define PACKET_MESH_TRS_CONTROL_FRIEND_CLEAR_LPN_COUNTER0_OFFSET   (2)          /**< Offset to the trs_control_friend_clear_lpn_counter field (0).*/
#define PACKET_MESH_TRS_CONTROL_FRIEND_CLEAR_LPN_COUNTER1_OFFSET   (3)          /**< Offset to the trs_control_friend_clear_lpn_counter field (1).*/

#define PACKET_MESH_TRS_CONTROL_FRIEND_CLEAR_CONFIRM_LPN_ADDRESS0_OFFSET   (0)          /**< Offset to the trs_control_friend_clear_confirm_lpn_address field (0).*/
#define PACKET_MESH_TRS_CONTROL_FRIEND_CLEAR_CONFIRM_LPN_ADDRESS1_OFFSET   (1)          /**< Offset to the trs_control_friend_clear_confirm_lpn_address field (1).*/

#define PACKET_MESH_TRS_CONTROL_FRIEND_CLEAR_CONFIRM_LPN_COUNTER0_OFFSET   (2)          /**< Offset to the trs_control_friend_clear_confirm_lpn_counter field (0).*/
#define PACKET_MESH_TRS_CONTROL_FRIEND_CLEAR_CONFIRM_LPN_COUNTER1_OFFSET   (3)          /**< Offset to the trs_control_friend_clear_confirm_lpn_counter field (1).*/

#define PACKET_MESH_TRS_CONTROL_FRIEND_OFFER_RECEIVE_WINDOW_OFFSET   (0)            /**< Offset to the trs_control_friend_offer_receive_window field.*/

#define PACKET_MESH_TRS_CONTROL_FRIEND_OFFER_QUEUE_SIZE_OFFSET   (1)            /**< Offset to the trs_control_friend_offer_queue_size field.*/

#define PACKET_MESH_TRS_CONTROL_FRIEND_OFFER_SUBSCRIPTION_LIST_SIZE_OFFSET   (2)            /**< Offset to the trs_control_friend_offer_subscription_list_size field.*/

#define PACKET_MESH_TRS_CONTROL_FRIEND_OFFER_RSSI_OFFSET   (3)          /**< Offset to the trs_control_friend_offer_rssi field.*/

#define PACKET_MESH_TRS_CONTROL_FRIEND_OFFER_FRIEND_COUNTER0_OFFSET   (4)           /**< Offset to the trs_control_friend_offer_friend_counter field (0).*/
#define PACKET_MESH_TRS_CONTROL_FRIEND_OFFER_FRIEND_COUNTER1_OFFSET   (5)           /**< Offset to the trs_control_friend_offer_friend_counter field (1).*/

#define PACKET_MESH_TRS_CONTROL_FRIEND_POLL_FSN_OFFSET   (0)            /**< Offset to the trs_control_friend_poll_fsn field.*/
#define PACKET_MESH_TRS_CONTROL_FRIEND_POLL_FSN_MASK     (0x01)         /**< Mask for trs_control_friend_poll_fsn field. */
#define PACKET_MESH_TRS_CONTROL_FRIEND_POLL_FSN_MASK_INV (0xFE)         /**< Inverse mask for trs_control_friend_poll_fsn field. */

#define PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_RSSI_FACTOR_OFFSET   (0)             /**< Offset to the trs_control_friend_request_rssi_factor field.*/
#define PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_RSSI_FACTOR_MASK     (0x60)          /**< Mask for trs_control_friend_request_rssi_factor field. */
#define PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_RSSI_FACTOR_MASK_INV (0x9F)          /**< Inverse mask for trs_control_friend_request_rssi_factor field. */

#define PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_RECEIVE_WINDOW_FACTOR_OFFSET   (0)       /**< Offset to the trs_control_friend_request_receive_window_factor field.*/
#define PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_RECEIVE_WINDOW_FACTOR_MASK     (0x18)    /**< Mask for trs_control_friend_request_receive_window_factor field. */
#define PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_RECEIVE_WINDOW_FACTOR_MASK_INV (0xE7)    /**< Inverse mask for trs_control_friend_request_receive_window_factor field. */

#define PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_MIN_QUEUE_SIZE_LOG_OFFSET   (0)       /**< Offset to the trs_control_friend_request_min_queue_size_log field.*/
#define PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_MIN_QUEUE_SIZE_LOG_MASK     (0x07)    /**< Mask for trs_control_friend_request_min_queue_size_log field. */
#define PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_MIN_QUEUE_SIZE_LOG_MASK_INV (0xF8)    /**< Inverse mask for trs_control_friend_request_min_queue_size_log field. */

#define PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_RECEIVE_DELAY_OFFSET   (1)       /**< Offset to the trs_control_friend_request_receive_delay field.*/

#define PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_POLL_TIMEOUT0_OFFSET   (2)       /**< Offset to the trs_control_friend_request_poll_timeout field (0).*/
#define PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_POLL_TIMEOUT1_OFFSET   (3)       /**< Offset to the trs_control_friend_request_poll_timeout field (1).*/
#define PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_POLL_TIMEOUT2_OFFSET   (4)       /**< Offset to the trs_control_friend_request_poll_timeout field (2).*/

#define PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_PREVIOUS_ADDRESS0_OFFSET   (5)       /**< Offset to the trs_control_friend_request_previous_address field (0).*/
#define PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_PREVIOUS_ADDRESS1_OFFSET   (6)       /**< Offset to the trs_control_friend_request_previous_address field (1).*/

#define PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_NUM_ELEMENTS_OFFSET   (7)       /**< Offset to the trs_control_friend_request_num_elements field.*/

#define PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_LPN_COUNTER0_OFFSET   (8)       /**< Offset to the trs_control_friend_request_lpn_counter field (0).*/
#define PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_LPN_COUNTER1_OFFSET   (9)       /**< Offset to the trs_control_friend_request_lpn_counter field (1).*/

#define PACKET_MESH_TRS_CONTROL_FRIEND_SUBLIST_ADD_REMOVE_TRANSACTION_NUMBER_OFFSET   (0)       /**< Offset to the trs_control_friend_sublist_add_remove_transaction_number field.*/

#define PACKET_MESH_TRS_CONTROL_FRIEND_SUBLIST_ADD_REMOVE_ADDRESS_LIST0_OFFSET   (1)       /**< Offset to the trs_control_friend_sublist_add_remove_address_list field (0).*/
#define PACKET_MESH_TRS_CONTROL_FRIEND_SUBLIST_ADD_REMOVE_ADDRESS_LIST1_OFFSET   (2)       /**< Offset to the trs_control_friend_sublist_add_remove_address_list field (1).*/
#define PACKET_MESH_TRS_CONTROL_FRIEND_SUBLIST_ADD_REMOVE_ADDRESS_LIST_MAX_COUNT (5)       /**< Highest number of elements allowed in the trs_control_friend_sublist_add_remove_address_list field. */

#define PACKET_MESH_TRS_CONTROL_FRIEND_SUBLIST_CONFIRM_TRANSACTION_NUMBER_OFFSET   (0)       /**< Offset to the trs_control_friend_sublist_confirm_transaction_number field.*/

#define PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_IV_UPDATE_FLAG_OFFSET   (0)       /**< Offset to the trs_control_friend_update_iv_update_flag field.*/
#define PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_IV_UPDATE_FLAG_MASK     (0x02)    /**< Mask for trs_control_friend_update_iv_update_flag field. */
#define PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_IV_UPDATE_FLAG_MASK_INV (0xFD)    /**< Inverse mask for trs_control_friend_update_iv_update_flag field. */

#define PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_KEY_REFRESH_FLAG_OFFSET   (0)       /**< Offset to the trs_control_friend_update_key_refresh_flag field.*/
#define PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_KEY_REFRESH_FLAG_MASK     (0x01)    /**< Mask for trs_control_friend_update_key_refresh_flag field. */
#define PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_KEY_REFRESH_FLAG_MASK_INV (0xFE)    /**< Inverse mask for trs_control_friend_update_key_refresh_flag field. */

#define PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_IV_INDEX0_OFFSET   (1)       /**< Offset to the trs_control_friend_update_iv_index field (0).*/
#define PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_IV_INDEX1_OFFSET   (2)       /**< Offset to the trs_control_friend_update_iv_index field (1).*/
#define PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_IV_INDEX2_OFFSET   (3)       /**< Offset to the trs_control_friend_update_iv_index field (2).*/
#define PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_IV_INDEX3_OFFSET   (4)       /**< Offset to the trs_control_friend_update_iv_index field (3).*/

#define PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_MD_OFFSET   (5)       /**< Offset to the trs_control_friend_update_md field.*/

#define PACKET_MESH_TRS_CONTROL_HEARTBEAT_INIT_TTL_OFFSET   (0)       /**< Offset to the trs_control_heartbeat_init_ttl field.*/
#define PACKET_MESH_TRS_CONTROL_HEARTBEAT_INIT_TTL_MASK     (0x7F)    /**< Mask for trs_control_heartbeat_init_ttl field. */
#define PACKET_MESH_TRS_CONTROL_HEARTBEAT_INIT_TTL_MASK_INV (0x80)    /**< Inverse mask for trs_control_heartbeat_init_ttl field. */

#define PACKET_MESH_TRS_CONTROL_HEARTBEAT_FEATURES0_OFFSET   (1)       /**< Offset to the trs_control_heartbeat_features field (0).*/
#define PACKET_MESH_TRS_CONTROL_HEARTBEAT_FEATURES1_OFFSET   (2)       /**< Offset to the trs_control_heartbeat_features field (1).*/

#define PACKET_MESH_NET_IVI_OFFSET   (0)       /**< Offset to the net_ivi field.*/
#define PACKET_MESH_NET_IVI_MASK     (0x80)    /**< Mask for net_ivi field. */
#define PACKET_MESH_NET_IVI_MASK_INV (0x7F)    /**< Inverse mask for net_ivi field. */

#define PACKET_MESH_NET_NID_OFFSET   (0)       /**< Offset to the net_nid field.*/
#define PACKET_MESH_NET_NID_MASK     (0x7F)    /**< Mask for net_nid field. */
#define PACKET_MESH_NET_NID_MASK_INV (0x80)    /**< Inverse mask for net_nid field. */

#define PACKET_MESH_NET_CTL_OFFSET   (1)       /**< Offset to the net_ctl field.*/
#define PACKET_MESH_NET_CTL_MASK     (0x80)    /**< Mask for net_ctl field. */
#define PACKET_MESH_NET_CTL_MASK_INV (0x7F)    /**< Inverse mask for net_ctl field. */

#define PACKET_MESH_NET_TTL_OFFSET   (1)       /**< Offset to the net_ttl field.*/
#define PACKET_MESH_NET_TTL_MASK     (0x7F)    /**< Mask for net_ttl field. */
#define PACKET_MESH_NET_TTL_MASK_INV (0x80)    /**< Inverse mask for net_ttl field. */

#define PACKET_MESH_NET_SEQ0_OFFSET   (2)       /**< Offset to the net_seq field (0).*/
#define PACKET_MESH_NET_SEQ1_OFFSET   (3)       /**< Offset to the net_seq field (1).*/
#define PACKET_MESH_NET_SEQ2_OFFSET   (4)       /**< Offset to the net_seq field (2).*/

#define PACKET_MESH_NET_SRC0_OFFSET   (5)       /**< Offset to the net_src field (0).*/
#define PACKET_MESH_NET_SRC1_OFFSET   (6)       /**< Offset to the net_src field (1).*/

#define PACKET_MESH_NET_DST0_OFFSET   (7)       /**< Offset to the net_dst field (0).*/
#define PACKET_MESH_NET_DST1_OFFSET   (8)       /**< Offset to the net_dst field (1).*/

#define PACKET_MESH_TRS_CONTROL_SEGACK_OBO_OFFSET   (0)       /**< Offset to the trs_control_segack_obo field.*/
#define PACKET_MESH_TRS_CONTROL_SEGACK_OBO_MASK     (0x80)    /**< Mask for trs_control_segack_obo field. */
#define PACKET_MESH_TRS_CONTROL_SEGACK_OBO_MASK_INV (0x7F)    /**< Inverse mask for trs_control_segack_obo field. */

#define PACKET_MESH_TRS_CONTROL_SEGACK_SEQZERO0_OFFSET   (0)       /**< Offset to the trs_control_segack_seqzero field (0).*/
#define PACKET_MESH_TRS_CONTROL_SEGACK_SEQZERO1_OFFSET   (1)       /**< Offset to the trs_control_segack_seqzero field (1).*/
#define PACKET_MESH_TRS_CONTROL_SEGACK_SEQZERO0_MASK     (0x7F)    /**< Mask for trs_control_segack_seqzero field (0). */
#define PACKET_MESH_TRS_CONTROL_SEGACK_SEQZERO1_MASK     (0xFC)    /**< Mask for trs_control_segack_seqzero field (1). */
#define PACKET_MESH_TRS_CONTROL_SEGACK_SEQZERO0_MASK_INV (0x80)    /**< Inverse mask for trs_control_segack_seqzero field (0). */
#define PACKET_MESH_TRS_CONTROL_SEGACK_SEQZERO1_MASK_INV (0x03)    /**< Inverse mask for trs_control_segack_seqzero field (1). */

#define PACKET_MESH_TRS_CONTROL_SEGACK_BLOCK_ACK0_OFFSET   (2)       /**< Offset to the trs_control_segack_block_ack field (0).*/
#define PACKET_MESH_TRS_CONTROL_SEGACK_BLOCK_ACK1_OFFSET   (3)       /**< Offset to the trs_control_segack_block_ack field (1).*/
#define PACKET_MESH_TRS_CONTROL_SEGACK_BLOCK_ACK2_OFFSET   (4)       /**< Offset to the trs_control_segack_block_ack field (2).*/
#define PACKET_MESH_TRS_CONTROL_SEGACK_BLOCK_ACK3_OFFSET   (5)       /**< Offset to the trs_control_segack_block_ack field (3).*/

#define PACKET_MESH_TRS_ACCESS_AKF_OFFSET   (0)       /**< Offset to the trs_access_akf field.*/
#define PACKET_MESH_TRS_ACCESS_AKF_MASK     (0x40)    /**< Mask for trs_access_akf field. */
#define PACKET_MESH_TRS_ACCESS_AKF_MASK_INV (0xBF)    /**< Inverse mask for trs_access_akf field. */

#define PACKET_MESH_TRS_ACCESS_AID_OFFSET   (0)       /**< Offset to the trs_access_aid field.*/
#define PACKET_MESH_TRS_ACCESS_AID_MASK     (0x3F)    /**< Mask for trs_access_aid field. */
#define PACKET_MESH_TRS_ACCESS_AID_MASK_INV (0xC0)    /**< Inverse mask for trs_access_aid field. */

#define PACKET_MESH_TRS_COMMON_SEG_OFFSET   (0)       /**< Offset to the trs_common_seg field.*/
#define PACKET_MESH_TRS_COMMON_SEG_MASK     (0x80)    /**< Mask for trs_common_seg field. */
#define PACKET_MESH_TRS_COMMON_SEG_MASK_INV (0x7F)    /**< Inverse mask for trs_common_seg field. */

#define PACKET_MESH_TRS_CONTROL_OPCODE_OFFSET   (0)       /**< Offset to the trs_control_opcode field.*/
#define PACKET_MESH_TRS_CONTROL_OPCODE_MASK     (0x7F)    /**< Mask for trs_control_opcode field. */
#define PACKET_MESH_TRS_CONTROL_OPCODE_MASK_INV (0x80)    /**< Inverse mask for trs_control_opcode field. */

#define PACKET_MESH_TRS_SEG_SZMIC_OFFSET   (1)       /**< Offset to the trs_seg_szmic field.*/
#define PACKET_MESH_TRS_SEG_SZMIC_MASK     (0x80)    /**< Mask for trs_seg_szmic field. */
#define PACKET_MESH_TRS_SEG_SZMIC_MASK_INV (0x7F)    /**< Inverse mask for trs_seg_szmic field. */

#define PACKET_MESH_TRS_SEG_SEQZERO0_OFFSET   (1)       /**< Offset to the trs_seg_seqzero field (0).*/
#define PACKET_MESH_TRS_SEG_SEQZERO1_OFFSET   (2)       /**< Offset to the trs_seg_seqzero field (1).*/
#define PACKET_MESH_TRS_SEG_SEQZERO0_MASK     (0x7F)    /**< Mask for trs_seg_seqzero field (0). */
#define PACKET_MESH_TRS_SEG_SEQZERO1_MASK     (0xFC)    /**< Mask for trs_seg_seqzero field (1). */
#define PACKET_MESH_TRS_SEG_SEQZERO0_MASK_INV (0x80)    /**< Inverse mask for trs_seg_seqzero field (0). */
#define PACKET_MESH_TRS_SEG_SEQZERO1_MASK_INV (0x03)    /**< Inverse mask for trs_seg_seqzero field (1). */

#define PACKET_MESH_TRS_SEG_SEGO0_OFFSET   (2)       /**< Offset to the trs_seg_sego field (0).*/
#define PACKET_MESH_TRS_SEG_SEGO1_OFFSET   (3)       /**< Offset to the trs_seg_sego field (1).*/
#define PACKET_MESH_TRS_SEG_SEGO0_MASK     (0x03)    /**< Mask for trs_seg_sego field (0). */
#define PACKET_MESH_TRS_SEG_SEGO1_MASK     (0xE0)    /**< Mask for trs_seg_sego field (1). */
#define PACKET_MESH_TRS_SEG_SEGO0_MASK_INV (0xFC)    /**< Inverse mask for trs_seg_sego field (0). */
#define PACKET_MESH_TRS_SEG_SEGO1_MASK_INV (0x1F)    /**< Inverse mask for trs_seg_sego field (1). */

#define PACKET_MESH_TRS_SEG_SEGN_OFFSET   (3)       /**< Offset to the trs_seg_segn field.*/
#define PACKET_MESH_TRS_SEG_SEGN_MASK     (0x1F)    /**< Mask for trs_seg_segn field. */
#define PACKET_MESH_TRS_SEG_SEGN_MASK_INV (0xE0)    /**< Inverse mask for trs_seg_segn field. */

#define PACKET_MESH_TRS_CONTROL_FRIEND_CLEAR_SIZE (4) /**< Size of trs control friend clear packet. */

#define PACKET_MESH_TRS_CONTROL_FRIEND_CLEAR_CONFIRM_SIZE (4) /**< Size of trs control friend clear confirm packet. */

#define PACKET_MESH_TRS_CONTROL_FRIEND_OFFER_SIZE (6) /**< Size of trs control friend offer packet. */

#define PACKET_MESH_TRS_CONTROL_FRIEND_POLL_SIZE (1) /**< Size of trs control friend poll packet. */

#define PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_SIZE (10) /**< Size of trs control friend request packet. */

#define PACKET_MESH_TRS_CONTROL_FRIEND_SUBLIST_ADD_REMOVE_SIZE (11) /**< Size of trs control friend sublist add remove packet. */

#define PACKET_MESH_TRS_CONTROL_FRIEND_SUBLIST_CONFIRM_SIZE (1) /**< Size of trs control friend sublist confirm packet. */

#define PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_SIZE (6) /**< Size of trs control friend update packet. */

#define PACKET_MESH_TRS_CONTROL_HEARTBEAT_SIZE (3) /**< Size of trs control heartbeat packet. */

#define PACKET_MESH_NET_PDU_OFFSET (9) /**< Offset of net packet PDU. */
#define PACKET_MESH_NET_MAX_SIZE (29) /**< Max size of net packet. */
#define PACKET_MESH_NET_PDU_MAX_SIZE (20) /**< Max size of net packet PDU. */

#define PACKET_MESH_TRS_CONTROL_SEGACK_SIZE (6) /**< Size of trs control segack packet. */

#define PACKET_MESH_TRS_TRANSMIC_SMALL_SIZE (4) /**< Size (in bytes) of the 32bit transport MIC. */
#define PACKET_MESH_TRS_TRANSMIC_LARGE_SIZE (8) /**< Size (in bytes) of the 64bit transport MIC. */
#define PACKET_MESH_TRS_TRANSMIC_CONTROL_SIZE (0) /**< Control packets do not have transport MIC.  */
#define PACKET_MESH_TRS_SIZE (16) /**< Size of trs packet. */

#define PACKET_MESH_TRS_SEG_ACCESS_PDU_MAX_SIZE (12) /**< Max PDU size of a segmented transport access message. */
#define PACKET_MESH_TRS_SEG_ACCESS_MAX_SIZE (16) /**< Max size of a segmented transport access message. */
#define PACKET_MESH_TRS_SEG_CONTROL_PDU_MAX_SIZE (8) /**< Max PDU size of a segmented transport control message. */
#define PACKET_MESH_TRS_SEG_CONTROL_MAX_SIZE (12) /**< Max size of a segmented transport control message. */
#define PACKET_MESH_TRS_SEG_PDU_OFFSET (4) /**< Offset of trs seg packet PDU. */

#define PACKET_MESH_TRS_UNSEG_ACCESS_PDU_MAX_SIZE (15) /**< Max PDU size of an unsegmented transport access message. */
#define PACKET_MESH_TRS_UNSEG_ACCESS_MAX_SIZE (16) /**< Max size of an unsegmented transport access message. */
#define PACKET_MESH_TRS_UNSEG_CONTROL_PDU_MAX_SIZE (11) /**< Max PDU size of an unsegmented transport control message. */
#define PACKET_MESH_TRS_UNSEG_CONTROL_MAX_SIZE (12) /**< Max size of an unsegmented transport control message. */
#define PACKET_MESH_TRS_UNSEG_PDU_OFFSET (1) /**< Offset of trs unseg packet PDU. */

/**
 * Packet type for the trs_control packet.
 */
typedef struct
{
    uint8_t pdu[11];
} packet_mesh_trs_control_packet_t;

/**
 * Packet type for the net packet.
 */
typedef struct
{
    uint8_t pdu[29];
} packet_mesh_net_packet_t;

/**
 * Packet type for the trs packet.
 */
typedef struct
{
    uint8_t pdu[16];
} packet_mesh_trs_packet_t;


static inline uint8_t packet_mesh_trs_common_seg_get( packet_mesh_trs_packet_t *p_pkt)
{
    return ((p_pkt->pdu[PACKET_MESH_TRS_COMMON_SEG_OFFSET] & PACKET_MESH_TRS_COMMON_SEG_MASK) > 0);
}


static inline uint8_t packet_mesh_trs_control_opcode_get( packet_mesh_trs_packet_t *p_pkt)
{
    return (p_pkt->pdu[PACKET_MESH_TRS_CONTROL_OPCODE_OFFSET] & PACKET_MESH_TRS_CONTROL_OPCODE_MASK);
}

static inline uint16_t packet_mesh_trs_control_segack_seqzero_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return (((p_pkt->pdu[PACKET_MESH_TRS_CONTROL_SEGACK_SEQZERO0_OFFSET] & PACKET_MESH_TRS_CONTROL_SEGACK_SEQZERO0_MASK) << 6) |
            ((p_pkt->pdu[PACKET_MESH_TRS_CONTROL_SEGACK_SEQZERO1_OFFSET] & PACKET_MESH_TRS_CONTROL_SEGACK_SEQZERO1_MASK) >> 2));
}

static inline uint16_t packet_mesh_trs_seg_seqzero_get( packet_mesh_trs_packet_t *p_pkt)
{
    return (((p_pkt->pdu[PACKET_MESH_TRS_SEG_SEQZERO0_OFFSET] & PACKET_MESH_TRS_SEG_SEQZERO0_MASK) << 6) |
            ((p_pkt->pdu[PACKET_MESH_TRS_SEG_SEQZERO1_OFFSET] & PACKET_MESH_TRS_SEG_SEQZERO1_MASK) >> 2));
}

static inline  uint8_t *packet_mesh_trs_unseg_payload_get( packet_mesh_trs_packet_t *p_pkt)
{
    return &p_pkt->pdu[PACKET_MESH_TRS_UNSEG_PDU_OFFSET];
}

static inline  uint8_t *packet_mesh_trs_seg_payload_get( packet_mesh_trs_packet_t *p_pkt)
{
    return &p_pkt->pdu[PACKET_MESH_TRS_SEG_PDU_OFFSET];
}


static inline  uint8_t *packet_mesh_net_payload_get( packet_mesh_net_packet_t *p_pkt)
{
    return &p_pkt->pdu[PACKET_MESH_NET_PDU_OFFSET];
}



static inline uint16_t packet_mesh_trs_control_friend_clear_lpn_address_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return ((p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_CLEAR_LPN_ADDRESS0_OFFSET] << 8) |
            p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_CLEAR_LPN_ADDRESS1_OFFSET]);
}

static inline void packet_mesh_trs_control_friend_clear_lpn_address_set(packet_mesh_trs_control_packet_t *p_pkt, uint16_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_CLEAR_LPN_ADDRESS0_OFFSET] = (val >> 8) & 0xFF;
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_CLEAR_LPN_ADDRESS1_OFFSET] = val & 0xFF;
}

static inline uint16_t packet_mesh_trs_control_friend_clear_lpn_counter_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return ((p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_CLEAR_LPN_COUNTER0_OFFSET] << 8) |
            p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_CLEAR_LPN_COUNTER1_OFFSET]);
}

static inline void packet_mesh_trs_control_friend_clear_lpn_counter_set(packet_mesh_trs_control_packet_t *p_pkt, uint16_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_CLEAR_LPN_COUNTER0_OFFSET] = (val >> 8) & 0xFF;
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_CLEAR_LPN_COUNTER1_OFFSET] = val & 0xFF;
}

static inline uint16_t packet_mesh_trs_control_friend_clear_confirm_lpn_address_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return ((p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_CLEAR_CONFIRM_LPN_ADDRESS0_OFFSET] << 8) |
            p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_CLEAR_CONFIRM_LPN_ADDRESS1_OFFSET]);
}

static inline void packet_mesh_trs_control_friend_clear_confirm_lpn_address_set(packet_mesh_trs_control_packet_t *p_pkt, uint16_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_CLEAR_CONFIRM_LPN_ADDRESS0_OFFSET] = (val >> 8) & 0xFF;
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_CLEAR_CONFIRM_LPN_ADDRESS1_OFFSET] = val & 0xFF;
}

static inline uint16_t packet_mesh_trs_control_friend_clear_confirm_lpn_counter_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return ((p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_CLEAR_CONFIRM_LPN_COUNTER0_OFFSET] << 8) |
            p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_CLEAR_CONFIRM_LPN_COUNTER1_OFFSET]);
}

static inline void packet_mesh_trs_control_friend_clear_confirm_lpn_counter_set(packet_mesh_trs_control_packet_t *p_pkt, uint16_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_CLEAR_CONFIRM_LPN_COUNTER0_OFFSET] = (val >> 8) & 0xFF;
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_CLEAR_CONFIRM_LPN_COUNTER1_OFFSET] = val & 0xFF;
}

static inline uint8_t packet_mesh_trs_control_friend_offer_receive_window_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_OFFER_RECEIVE_WINDOW_OFFSET];
}

static inline void packet_mesh_trs_control_friend_offer_receive_window_set(packet_mesh_trs_control_packet_t *p_pkt, uint8_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_OFFER_RECEIVE_WINDOW_OFFSET] = val & 0xFF;
}

static inline uint8_t packet_mesh_trs_control_friend_offer_queue_size_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_OFFER_QUEUE_SIZE_OFFSET];
}

static inline void packet_mesh_trs_control_friend_offer_queue_size_set(packet_mesh_trs_control_packet_t *p_pkt, uint8_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_OFFER_QUEUE_SIZE_OFFSET] = val & 0xFF;
}

static inline uint8_t packet_mesh_trs_control_friend_offer_subscription_list_size_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_OFFER_SUBSCRIPTION_LIST_SIZE_OFFSET];
}

static inline void packet_mesh_trs_control_friend_offer_subscription_list_size_set(packet_mesh_trs_control_packet_t *p_pkt, uint8_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_OFFER_SUBSCRIPTION_LIST_SIZE_OFFSET] = val & 0xFF;
}

static inline uint8_t packet_mesh_trs_control_friend_offer_rssi_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_OFFER_RSSI_OFFSET];
}

static inline void packet_mesh_trs_control_friend_offer_rssi_set(packet_mesh_trs_control_packet_t *p_pkt, uint8_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_OFFER_RSSI_OFFSET] = val & 0xFF;
}

static inline uint16_t packet_mesh_trs_control_friend_offer_friend_counter_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return ((p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_OFFER_FRIEND_COUNTER0_OFFSET] << 8) |
            p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_OFFER_FRIEND_COUNTER1_OFFSET]);
}

static inline void packet_mesh_trs_control_friend_offer_friend_counter_set(packet_mesh_trs_control_packet_t *p_pkt, uint16_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_OFFER_FRIEND_COUNTER0_OFFSET] = (val >> 8) & 0xFF;
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_OFFER_FRIEND_COUNTER1_OFFSET] = val & 0xFF;
}

static inline uint8_t packet_mesh_trs_control_friend_poll_fsn_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return ((p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_POLL_FSN_OFFSET] & PACKET_MESH_TRS_CONTROL_FRIEND_POLL_FSN_MASK) > 0);
}

static inline void packet_mesh_trs_control_friend_poll_fsn_set(packet_mesh_trs_control_packet_t *p_pkt, uint8_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_POLL_FSN_OFFSET] &= PACKET_MESH_TRS_CONTROL_FRIEND_POLL_FSN_MASK_INV;
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_POLL_FSN_OFFSET] |= (val & PACKET_MESH_TRS_CONTROL_FRIEND_POLL_FSN_MASK);
}

static inline uint8_t packet_mesh_trs_control_friend_request_rssi_factor_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return ((p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_RSSI_FACTOR_OFFSET] & PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_RSSI_FACTOR_MASK) >> 5 );
}

static inline void packet_mesh_trs_control_friend_request_rssi_factor_set(packet_mesh_trs_control_packet_t *p_pkt, uint8_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_RSSI_FACTOR_OFFSET] &= PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_RSSI_FACTOR_MASK_INV;
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_RSSI_FACTOR_OFFSET] |= (val << 5) & PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_RSSI_FACTOR_MASK;
}


static inline uint8_t packet_mesh_trs_control_friend_request_receive_window_factor_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return ((p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_RECEIVE_WINDOW_FACTOR_OFFSET] & PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_RECEIVE_WINDOW_FACTOR_MASK) >> 3 );
}

static inline void packet_mesh_trs_control_friend_request_receive_window_factor_set(packet_mesh_trs_control_packet_t *p_pkt, uint8_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_RECEIVE_WINDOW_FACTOR_OFFSET] &= PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_RECEIVE_WINDOW_FACTOR_MASK_INV;
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_RECEIVE_WINDOW_FACTOR_OFFSET] |= (val << 3) & PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_RECEIVE_WINDOW_FACTOR_MASK;
}

static inline uint8_t packet_mesh_trs_control_friend_request_min_queue_size_log_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return (p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_MIN_QUEUE_SIZE_LOG_OFFSET] & PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_MIN_QUEUE_SIZE_LOG_MASK);
}

static inline void packet_mesh_trs_control_friend_request_min_queue_size_log_set(packet_mesh_trs_control_packet_t *p_pkt, uint8_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_MIN_QUEUE_SIZE_LOG_OFFSET] &= PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_MIN_QUEUE_SIZE_LOG_MASK_INV;
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_MIN_QUEUE_SIZE_LOG_OFFSET] |= (val & PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_MIN_QUEUE_SIZE_LOG_MASK);
}

static inline uint8_t packet_mesh_trs_control_friend_request_receive_delay_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_RECEIVE_DELAY_OFFSET];
}

static inline void packet_mesh_trs_control_friend_request_receive_delay_set(packet_mesh_trs_control_packet_t *p_pkt, uint8_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_RECEIVE_DELAY_OFFSET] = val & 0xFF;
}

static inline uint32_t packet_mesh_trs_control_friend_request_poll_timeout_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return ((p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_POLL_TIMEOUT0_OFFSET] << 16) |
            (p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_POLL_TIMEOUT1_OFFSET] << 8) |
            p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_POLL_TIMEOUT2_OFFSET]);
}

static inline void packet_mesh_trs_control_friend_request_poll_timeout_set(packet_mesh_trs_control_packet_t *p_pkt, uint32_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_POLL_TIMEOUT0_OFFSET] = (val >> 16) & 0xFF;
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_POLL_TIMEOUT1_OFFSET] = (val >> 8) & 0xFF;
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_POLL_TIMEOUT2_OFFSET] = val & 0xFF;
}

static inline uint16_t packet_mesh_trs_control_friend_request_previous_address_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return ((p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_PREVIOUS_ADDRESS0_OFFSET] << 8) |
            p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_PREVIOUS_ADDRESS1_OFFSET]);
}

static inline void packet_mesh_trs_control_friend_request_previous_address_set(packet_mesh_trs_control_packet_t *p_pkt, uint16_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_PREVIOUS_ADDRESS0_OFFSET] = (val >> 8) & 0xFF;
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_PREVIOUS_ADDRESS1_OFFSET] = val & 0xFF;
}

static inline uint8_t packet_mesh_trs_control_friend_request_num_elements_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_NUM_ELEMENTS_OFFSET];
}

static inline void packet_mesh_trs_control_friend_request_num_elements_set(packet_mesh_trs_control_packet_t *p_pkt, uint8_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_NUM_ELEMENTS_OFFSET] = val & 0xFF;
}

static inline uint16_t packet_mesh_trs_control_friend_request_lpn_counter_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return ((p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_LPN_COUNTER0_OFFSET] << 8) |
            p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_LPN_COUNTER1_OFFSET]);
}

static inline void packet_mesh_trs_control_friend_request_lpn_counter_set(packet_mesh_trs_control_packet_t *p_pkt, uint16_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_LPN_COUNTER0_OFFSET] = (val >> 8) & 0xFF;
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_REQUEST_LPN_COUNTER1_OFFSET] = val & 0xFF;
}

static inline uint8_t packet_mesh_trs_control_friend_sublist_add_remove_transaction_number_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_SUBLIST_ADD_REMOVE_TRANSACTION_NUMBER_OFFSET];
}

static inline void packet_mesh_trs_control_friend_sublist_add_remove_transaction_number_set(packet_mesh_trs_control_packet_t *p_pkt, uint8_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_SUBLIST_ADD_REMOVE_TRANSACTION_NUMBER_OFFSET] = val & 0xFF;
}

static inline uint16_t packet_mesh_trs_control_friend_sublist_add_remove_address_list_get( packet_mesh_trs_control_packet_t *p_pkt, uint32_t index)
{
    return ((p_pkt->pdu[(index * 2) + PACKET_MESH_TRS_CONTROL_FRIEND_SUBLIST_ADD_REMOVE_ADDRESS_LIST0_OFFSET] << 8) |
            p_pkt->pdu[(index * 2) + PACKET_MESH_TRS_CONTROL_FRIEND_SUBLIST_ADD_REMOVE_ADDRESS_LIST1_OFFSET]);
}

static inline void packet_mesh_trs_control_friend_sublist_add_remove_address_list_set(packet_mesh_trs_control_packet_t *p_pkt, uint32_t index, uint16_t val)
{
    p_pkt->pdu[(index * 2) + PACKET_MESH_TRS_CONTROL_FRIEND_SUBLIST_ADD_REMOVE_ADDRESS_LIST0_OFFSET] = (val >> 8) & 0xFF;
    p_pkt->pdu[(index * 2) + PACKET_MESH_TRS_CONTROL_FRIEND_SUBLIST_ADD_REMOVE_ADDRESS_LIST1_OFFSET] = val & 0xFF;
}

static inline uint8_t packet_mesh_trs_control_friend_sublist_confirm_transaction_number_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_SUBLIST_CONFIRM_TRANSACTION_NUMBER_OFFSET];
}

static inline void packet_mesh_trs_control_friend_sublist_confirm_transaction_number_set(packet_mesh_trs_control_packet_t *p_pkt, uint8_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_SUBLIST_CONFIRM_TRANSACTION_NUMBER_OFFSET] = val & 0xFF;
}

static inline uint8_t packet_mesh_trs_control_friend_update_iv_update_flag_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return ((p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_IV_UPDATE_FLAG_OFFSET] & PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_IV_UPDATE_FLAG_MASK) > 0);
}

static inline void packet_mesh_trs_control_friend_update_iv_update_flag_set(packet_mesh_trs_control_packet_t *p_pkt, uint8_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_IV_UPDATE_FLAG_OFFSET] &= PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_IV_UPDATE_FLAG_MASK_INV;
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_IV_UPDATE_FLAG_OFFSET] |= (val << 1) & PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_IV_UPDATE_FLAG_MASK;
}

static inline uint8_t packet_mesh_trs_control_friend_update_key_refresh_flag_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return ((p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_KEY_REFRESH_FLAG_OFFSET] & PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_KEY_REFRESH_FLAG_MASK) > 0);
}

static inline void packet_mesh_trs_control_friend_update_key_refresh_flag_set(packet_mesh_trs_control_packet_t *p_pkt, uint8_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_KEY_REFRESH_FLAG_OFFSET] &= PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_KEY_REFRESH_FLAG_MASK_INV;
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_KEY_REFRESH_FLAG_OFFSET] |= (val & PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_KEY_REFRESH_FLAG_MASK);
}

static inline uint32_t packet_mesh_trs_control_friend_update_iv_index_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return ((p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_IV_INDEX0_OFFSET] << 24) |
            (p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_IV_INDEX1_OFFSET] << 16) |
            (p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_IV_INDEX2_OFFSET] << 8) |
            p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_IV_INDEX3_OFFSET]);
}

static inline void packet_mesh_trs_control_friend_update_iv_index_set(packet_mesh_trs_control_packet_t *p_pkt, uint32_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_IV_INDEX0_OFFSET] = (val >> 24) & 0xFF;
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_IV_INDEX1_OFFSET] = (val >> 16) & 0xFF;
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_IV_INDEX2_OFFSET] = (val >> 8) & 0xFF;
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_IV_INDEX3_OFFSET] = val & 0xFF;
}

static inline uint8_t packet_mesh_trs_control_friend_update_md_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_MD_OFFSET];
}

static inline void packet_mesh_trs_control_friend_update_md_set(packet_mesh_trs_control_packet_t *p_pkt, uint8_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_FRIEND_UPDATE_MD_OFFSET] = val & 0xFF;
}

static inline uint8_t packet_mesh_trs_control_heartbeat_init_ttl_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return (p_pkt->pdu[PACKET_MESH_TRS_CONTROL_HEARTBEAT_INIT_TTL_OFFSET] & PACKET_MESH_TRS_CONTROL_HEARTBEAT_INIT_TTL_MASK);
}

static inline void packet_mesh_trs_control_heartbeat_init_ttl_set(packet_mesh_trs_control_packet_t *p_pkt, uint8_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_HEARTBEAT_INIT_TTL_OFFSET] &= PACKET_MESH_TRS_CONTROL_HEARTBEAT_INIT_TTL_MASK_INV;
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_HEARTBEAT_INIT_TTL_OFFSET] |= (val & PACKET_MESH_TRS_CONTROL_HEARTBEAT_INIT_TTL_MASK);
}

static inline uint16_t packet_mesh_trs_control_heartbeat_features_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return ((p_pkt->pdu[PACKET_MESH_TRS_CONTROL_HEARTBEAT_FEATURES0_OFFSET] << 8) |
            p_pkt->pdu[PACKET_MESH_TRS_CONTROL_HEARTBEAT_FEATURES1_OFFSET]);
}

static inline void packet_mesh_trs_control_heartbeat_features_set(packet_mesh_trs_control_packet_t *p_pkt, uint16_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_HEARTBEAT_FEATURES0_OFFSET] = (val >> 8) & 0xFF;
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_HEARTBEAT_FEATURES1_OFFSET] = val & 0xFF;
}


static inline uint8_t packet_mesh_trs_control_segack_obo_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return ((p_pkt->pdu[PACKET_MESH_TRS_CONTROL_SEGACK_OBO_OFFSET] & PACKET_MESH_TRS_CONTROL_SEGACK_OBO_MASK) > 0);
}

static inline void packet_mesh_trs_control_segack_obo_set(packet_mesh_trs_control_packet_t *p_pkt, uint8_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_SEGACK_OBO_OFFSET] &= PACKET_MESH_TRS_CONTROL_SEGACK_OBO_MASK_INV;
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_SEGACK_OBO_OFFSET] |= (val << 7) & PACKET_MESH_TRS_CONTROL_SEGACK_OBO_MASK;
}


static inline void packet_mesh_trs_control_segack_seqzero_set(packet_mesh_trs_control_packet_t *p_pkt, uint16_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_SEGACK_SEQZERO0_OFFSET] &= PACKET_MESH_TRS_CONTROL_SEGACK_SEQZERO0_MASK_INV;
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_SEGACK_SEQZERO0_OFFSET] |= (val >> 6) & PACKET_MESH_TRS_CONTROL_SEGACK_SEQZERO0_MASK;
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_SEGACK_SEQZERO1_OFFSET] &= PACKET_MESH_TRS_CONTROL_SEGACK_SEQZERO1_MASK_INV;
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_SEGACK_SEQZERO1_OFFSET] |= (val << 2) & PACKET_MESH_TRS_CONTROL_SEGACK_SEQZERO1_MASK;
}

static inline uint32_t packet_mesh_trs_control_segack_block_ack_get( packet_mesh_trs_control_packet_t *p_pkt)
{
    return ((p_pkt->pdu[PACKET_MESH_TRS_CONTROL_SEGACK_BLOCK_ACK0_OFFSET] << 24) |
            (p_pkt->pdu[PACKET_MESH_TRS_CONTROL_SEGACK_BLOCK_ACK1_OFFSET] << 16) |
            (p_pkt->pdu[PACKET_MESH_TRS_CONTROL_SEGACK_BLOCK_ACK2_OFFSET] << 8) |
            p_pkt->pdu[PACKET_MESH_TRS_CONTROL_SEGACK_BLOCK_ACK3_OFFSET]);
}

static inline void packet_mesh_trs_control_segack_block_ack_set(packet_mesh_trs_control_packet_t *p_pkt, uint32_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_SEGACK_BLOCK_ACK0_OFFSET] = (val >> 24) & 0xFF;
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_SEGACK_BLOCK_ACK1_OFFSET] = (val >> 16) & 0xFF;
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_SEGACK_BLOCK_ACK2_OFFSET] = (val >> 8) & 0xFF;
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_SEGACK_BLOCK_ACK3_OFFSET] = val & 0xFF;
}

static inline uint8_t packet_mesh_trs_access_akf_get( packet_mesh_trs_packet_t *p_pkt)
{
    return ((p_pkt->pdu[PACKET_MESH_TRS_ACCESS_AKF_OFFSET] & PACKET_MESH_TRS_ACCESS_AKF_MASK) > 0);
}

static inline void packet_mesh_trs_access_akf_set(packet_mesh_trs_packet_t *p_pkt, uint8_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_ACCESS_AKF_OFFSET] &= PACKET_MESH_TRS_ACCESS_AKF_MASK_INV;
    p_pkt->pdu[PACKET_MESH_TRS_ACCESS_AKF_OFFSET] |= (val << 6) & PACKET_MESH_TRS_ACCESS_AKF_MASK;
}

static inline uint8_t packet_mesh_trs_access_aid_get( packet_mesh_trs_packet_t *p_pkt)
{
    return (p_pkt->pdu[PACKET_MESH_TRS_ACCESS_AID_OFFSET] & PACKET_MESH_TRS_ACCESS_AID_MASK);
}

static inline void packet_mesh_trs_access_aid_set(packet_mesh_trs_packet_t *p_pkt, uint8_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_ACCESS_AID_OFFSET] &= PACKET_MESH_TRS_ACCESS_AID_MASK_INV;
    p_pkt->pdu[PACKET_MESH_TRS_ACCESS_AID_OFFSET] |= (val & PACKET_MESH_TRS_ACCESS_AID_MASK);
}


static inline void packet_mesh_trs_common_seg_set(packet_mesh_trs_packet_t *p_pkt, uint8_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_COMMON_SEG_OFFSET] &= PACKET_MESH_TRS_COMMON_SEG_MASK_INV;
    p_pkt->pdu[PACKET_MESH_TRS_COMMON_SEG_OFFSET] |= (val << 7) & PACKET_MESH_TRS_COMMON_SEG_MASK;
}


static inline void packet_mesh_trs_control_opcode_set(packet_mesh_trs_packet_t *p_pkt, uint8_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_OPCODE_OFFSET] &= PACKET_MESH_TRS_CONTROL_OPCODE_MASK_INV;
    p_pkt->pdu[PACKET_MESH_TRS_CONTROL_OPCODE_OFFSET] |= (val & PACKET_MESH_TRS_CONTROL_OPCODE_MASK);
}

static inline uint8_t packet_mesh_trs_seg_szmic_get( packet_mesh_trs_packet_t *p_pkt)
{
    return ((p_pkt->pdu[PACKET_MESH_TRS_SEG_SZMIC_OFFSET] & PACKET_MESH_TRS_SEG_SZMIC_MASK) > 0);
}

static inline void packet_mesh_trs_seg_szmic_set(packet_mesh_trs_packet_t *p_pkt, uint8_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_SEG_SZMIC_OFFSET] &= PACKET_MESH_TRS_SEG_SZMIC_MASK_INV;
    p_pkt->pdu[PACKET_MESH_TRS_SEG_SZMIC_OFFSET] |= (val << 7) & PACKET_MESH_TRS_SEG_SZMIC_MASK;
}


static inline void packet_mesh_trs_seg_seqzero_set(packet_mesh_trs_packet_t *p_pkt, uint16_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_SEG_SEQZERO0_OFFSET] &= PACKET_MESH_TRS_SEG_SEQZERO0_MASK_INV;
    p_pkt->pdu[PACKET_MESH_TRS_SEG_SEQZERO0_OFFSET] |= (val >> 6) & PACKET_MESH_TRS_SEG_SEQZERO0_MASK;
    p_pkt->pdu[PACKET_MESH_TRS_SEG_SEQZERO1_OFFSET] &= PACKET_MESH_TRS_SEG_SEQZERO1_MASK_INV;
    p_pkt->pdu[PACKET_MESH_TRS_SEG_SEQZERO1_OFFSET] |= (val << 2) & PACKET_MESH_TRS_SEG_SEQZERO1_MASK;
}

static inline uint8_t packet_mesh_trs_seg_sego_get( packet_mesh_trs_packet_t *p_pkt)
{
    return (((p_pkt->pdu[PACKET_MESH_TRS_SEG_SEGO0_OFFSET] & PACKET_MESH_TRS_SEG_SEGO0_MASK) << 3) |
            (p_pkt->pdu[PACKET_MESH_TRS_SEG_SEGO1_OFFSET] & PACKET_MESH_TRS_SEG_SEGO1_MASK) >> 5);
}

static inline void packet_mesh_trs_seg_sego_set(packet_mesh_trs_packet_t *p_pkt, uint8_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_SEG_SEGO0_OFFSET] &= PACKET_MESH_TRS_SEG_SEGO0_MASK_INV;
    p_pkt->pdu[PACKET_MESH_TRS_SEG_SEGO0_OFFSET] |= (val >> 3) & PACKET_MESH_TRS_SEG_SEGO0_MASK;
    p_pkt->pdu[PACKET_MESH_TRS_SEG_SEGO1_OFFSET] &= PACKET_MESH_TRS_SEG_SEGO1_MASK_INV;
    p_pkt->pdu[PACKET_MESH_TRS_SEG_SEGO1_OFFSET] |= (val << 5) & PACKET_MESH_TRS_SEG_SEGO1_MASK;
}

static inline uint8_t packet_mesh_trs_seg_segn_get( packet_mesh_trs_packet_t *p_pkt)
{
    return (p_pkt->pdu[PACKET_MESH_TRS_SEG_SEGN_OFFSET] & PACKET_MESH_TRS_SEG_SEGN_MASK);
}

static inline void packet_mesh_trs_seg_segn_set(packet_mesh_trs_packet_t *p_pkt, uint8_t val)
{
    p_pkt->pdu[PACKET_MESH_TRS_SEG_SEGN_OFFSET] &= PACKET_MESH_TRS_SEG_SEGN_MASK_INV;
    p_pkt->pdu[PACKET_MESH_TRS_SEG_SEGN_OFFSET] |= (val & PACKET_MESH_TRS_SEG_SEGN_MASK);
}

#if 0

static inline uint8_t packet_mesh_net_ctl_get( packet_mesh_net_packet_t *p_pkt)
{
    return ((p_pkt->pdu[PACKET_MESH_NET_CTL_OFFSET] & PACKET_MESH_NET_CTL_MASK) > 0);
}

static inline uint8_t packet_mesh_net_ttl_get( packet_mesh_net_packet_t *p_pkt)
{
    return (p_pkt->pdu[PACKET_MESH_NET_TTL_OFFSET] & PACKET_MESH_NET_TTL_MASK);
}

static inline uint32_t packet_mesh_net_seq_get( packet_mesh_net_packet_t *p_pkt)
{
    return ((p_pkt->pdu[PACKET_MESH_NET_SEQ0_OFFSET] << 16) |
            (p_pkt->pdu[PACKET_MESH_NET_SEQ1_OFFSET] << 8) |
            p_pkt->pdu[PACKET_MESH_NET_SEQ2_OFFSET]);
}

static inline uint16_t packet_mesh_net_src_get( packet_mesh_net_packet_t *p_pkt)
{
    return ((p_pkt->pdu[PACKET_MESH_NET_SRC0_OFFSET] << 8) |
            p_pkt->pdu[PACKET_MESH_NET_SRC1_OFFSET]);
}

static inline void packet_mesh_net_ivi_set(packet_mesh_net_packet_t *p_pkt, uint8_t val)
{
    p_pkt->pdu[PACKET_MESH_NET_IVI_OFFSET] &= PACKET_MESH_NET_IVI_MASK_INV;
    p_pkt->pdu[PACKET_MESH_NET_IVI_OFFSET] |= (val << 7) & PACKET_MESH_NET_IVI_MASK;
}

static inline void packet_mesh_net_nid_set(packet_mesh_net_packet_t *p_pkt, uint8_t val)
{
    p_pkt->pdu[PACKET_MESH_NET_NID_OFFSET] &= PACKET_MESH_NET_NID_MASK_INV;
    p_pkt->pdu[PACKET_MESH_NET_NID_OFFSET] |= (val & PACKET_MESH_NET_NID_MASK);
}



static inline void packet_mesh_net_ctl_set(packet_mesh_net_packet_t *p_pkt, uint8_t val)
{
    p_pkt->pdu[PACKET_MESH_NET_CTL_OFFSET] &= PACKET_MESH_NET_CTL_MASK_INV;
    p_pkt->pdu[PACKET_MESH_NET_CTL_OFFSET] |= (val << 7) & PACKET_MESH_NET_CTL_MASK;
}


static inline void packet_mesh_net_ttl_set(packet_mesh_net_packet_t *p_pkt, uint8_t val)
{
    p_pkt->pdu[PACKET_MESH_NET_TTL_OFFSET] &= PACKET_MESH_NET_TTL_MASK_INV;
    p_pkt->pdu[PACKET_MESH_NET_TTL_OFFSET] |= (val & PACKET_MESH_NET_TTL_MASK);
}


static inline void packet_mesh_net_seq_set(packet_mesh_net_packet_t *p_pkt, uint32_t val)
{
    p_pkt->pdu[PACKET_MESH_NET_SEQ0_OFFSET] = (val >> 16) & 0xFF;
    p_pkt->pdu[PACKET_MESH_NET_SEQ1_OFFSET] = (val >> 8) & 0xFF;
    p_pkt->pdu[PACKET_MESH_NET_SEQ2_OFFSET] = val & 0xFF;
}


static inline void packet_mesh_net_src_set(packet_mesh_net_packet_t *p_pkt, uint16_t val)
{
    p_pkt->pdu[PACKET_MESH_NET_SRC0_OFFSET] = (val >> 8) & 0xFF;
    p_pkt->pdu[PACKET_MESH_NET_SRC1_OFFSET] = val & 0xFF;
}

static inline void packet_mesh_net_dst_set(packet_mesh_net_packet_t *p_pkt, uint16_t val)
{
    p_pkt->pdu[PACKET_MESH_NET_DST0_OFFSET] = (val >> 8) & 0xFF;
    p_pkt->pdu[PACKET_MESH_NET_DST1_OFFSET] = val & 0xFF;
}

static inline uint16_t packet_mesh_net_dst_get( packet_mesh_net_packet_t *p_pkt)
{
    return ((p_pkt->pdu[PACKET_MESH_NET_DST0_OFFSET] << 8) |
            p_pkt->pdu[PACKET_MESH_NET_DST1_OFFSET]);
}

static inline uint8_t packet_mesh_net_ivi_get( packet_mesh_net_packet_t *p_pkt)
{
    return ((p_pkt->pdu[PACKET_MESH_NET_IVI_OFFSET] & PACKET_MESH_NET_IVI_MASK) > 0);
}

static inline uint8_t packet_mesh_net_nid_get( packet_mesh_net_packet_t *p_pkt)
{
    return (p_pkt->pdu[PACKET_MESH_NET_NID_OFFSET] & PACKET_MESH_NET_NID_MASK);
}

#endif

#ifdef __cplusplus
};
#endif
#endif /* __MESH_PKT_MESH_H__ */
