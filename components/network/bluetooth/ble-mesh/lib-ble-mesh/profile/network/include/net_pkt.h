// ---------------------------------------------------------------------------
// Copyright (c) 2021
// All rights reserved.
//
// ---------------------------------------------------------------------------
#ifndef __NET_PKT_H__
#define __NET_PKT_H__

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                Include (Better to prevent)
//=============================================================================

/** Offset of the start of the encrypted part of the network packet. */
#define NET_PACKET_ENCRYPTION_START_OFFSET          7
/** Offset of the start of the obfuscated part of the network packet. */
#define NET_PACKET_OBFUSCATION_START_OFFSET         1
/** Number of bytes before the payload that are being encrypted. */
#define NET_PACKET_ENCRYPTION_START_PAYLOAD_OVERHEAD (9 - NET_PACKET_ENCRYPTION_START_OFFSET)

#define PACKET_MESH_NET_PDU_OFFSET (9)          /**< Offset of net packet PDU. */
#define PACKET_MESH_NET_MAX_SIZE (29)           /**< Max size of net packet. */
#define PACKET_MESH_NET_PDU_MAX_SIZE (20)       /**< Max size of net packet PDU. */

#define PACKET_MESH_NET_IVI_OFFSET   (0)        /**< Offset to the net_ivi field.*/
#define PACKET_MESH_NET_IVI_MASK     (0x80)     /**< Mask for net_ivi field. */
#define PACKET_MESH_NET_IVI_MASK_INV (0x7F)     /**< Inverse mask for net_ivi field. */

#define PACKET_MESH_NET_NID_OFFSET   (0)        /**< Offset to the net_nid field.*/
#define PACKET_MESH_NET_NID_MASK     (0x7F)     /**< Mask for net_nid field. */
#define PACKET_MESH_NET_NID_MASK_INV (0x80)     /**< Inverse mask for net_nid field. */

#define PACKET_MESH_NET_CTL_OFFSET   (1)        /**< Offset to the net_ctl field.*/
#define PACKET_MESH_NET_CTL_MASK     (0x80)     /**< Mask for net_ctl field. */
#define PACKET_MESH_NET_CTL_MASK_INV (0x7F)     /**< Inverse mask for net_ctl field. */

#define PACKET_MESH_NET_TTL_OFFSET   (1)        /**< Offset to the net_ttl field.*/
#define PACKET_MESH_NET_TTL_MASK     (0x7F)     /**< Mask for net_ttl field. */
#define PACKET_MESH_NET_TTL_MASK_INV (0x80)     /**< Inverse mask for net_ttl field. */

#define PACKET_MESH_NET_SEQ0_OFFSET   (2)       /**< Offset to the net_seq field (0).*/
#define PACKET_MESH_NET_SEQ1_OFFSET   (3)       /**< Offset to the net_seq field (1).*/
#define PACKET_MESH_NET_SEQ2_OFFSET   (4)       /**< Offset to the net_seq field (2).*/

#define PACKET_MESH_NET_SRC0_OFFSET   (5)       /**< Offset to the net_src field (0).*/
#define PACKET_MESH_NET_SRC1_OFFSET   (6)       /**< Offset to the net_src field (1).*/

#define PACKET_MESH_NET_DST0_OFFSET   (7)       /**< Offset to the net_dst field (0).*/
#define PACKET_MESH_NET_DST1_OFFSET   (8)       /**< Offset to the net_dst field (1).*/

/** Redefinition of header_transfuscate() for clarity. */
#define header_obfuscate(p_net_metadata, p_net_packet_in, p_net_packet_out)    \
    header_transfuscate(p_net_metadata, p_net_packet_in, p_net_packet_out)
/** Redefinition of header_transfuscate() for clarity. */
#define header_deobfuscate(p_net_metadata, p_net_packet_in, p_net_packet_out)  \
    header_transfuscate(p_net_metadata, p_net_packet_in, p_net_packet_out)

#define PRIVACY_RANDOM_SIZE 7
#define PECB_SIZE           6

typedef struct __attribute__((packed))
{
    uint8_t          zero_padding[5];                     /**< Zero padding. */
    uint32_t         iv_index_be;                         /**< Current IV index (big endian). */
    uint8_t          privacy_random[PRIVACY_RANDOM_SIZE]; /**< The 7 LSBs of the privacy random value. */
}
pecb_data_t;


typedef enum
{
    NET_PACKET_KIND_TRANSPORT, /**< Network packet containing Transport layer data */
    NET_PACKET_KIND_PROXY_CONFIG, /**< Network packet containing proxy configuration data. */
} net_packet_kind_t;

typedef struct
{
    uint8_t pdu[29];
} network_packet_t;


uint32_t net_packet_payload_len_get(network_meta_t *p_net_metadata, uint32_t net_packet_len);
network_packet_t *net_packet_from_payload(uint8_t *p_net_payload);
uint8_t *net_packet_enc_start_get(network_packet_t *p_net_packet);
uint8_t *net_packet_payload_get(network_packet_t *p_net_packet);
uint8_t *net_packet_obfuscation_start_get(network_packet_t *p_net_packet);
network_status_t net_packet_decrypt(network_meta_t *p_data_meta, uint8_t datalen, uint8_t *p_data, net_packet_kind_t packet_kind);
void net_packet_encrypt(network_meta_t *p_net_metadata,
                        uint32_t net_packet_len,
                        network_packet_t *p_net_packet,
                        net_packet_kind_t packet_kind);
void net_packet_header_set(network_packet_t *p_net_packet, network_meta_t *p_metadata);

#ifdef __cplusplus
};
#endif
#endif /* __NET_PKT_H__ */
