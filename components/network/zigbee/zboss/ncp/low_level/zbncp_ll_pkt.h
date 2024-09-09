/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
 *
 * This is unpublished proprietary source code of DSR Corporation
 * The copyright notice does not evidence any actual or intended
 * publication of such source code.
 *
 * ZBOSS is a registered trademark of Data Storage Research LLC d/b/a DSR
 * Corporation
 *
 * Commercial Usage
 * Licensees holding valid DSR Commercial licenses may use
 * this file in accordance with the DSR Commercial License
 * Agreement provided with the Software or, alternatively, in accordance
 * with the terms contained in a written agreement between you and
 * DSR.
 */
/*  PURPOSE: NCP Low level protocol packet structure definition.
*/
#ifndef ZBNCP_INCLUDE_GUARD_LL_PKT_H
#define ZBNCP_INCLUDE_GUARD_LL_PKT_H 1

#include "zbncp_defs.h"
#include "zbncp_types.h"
#include "zbncp_debug.h"
#include "zbncp_utils.h"
#include "zbncp_mem.h"

#ifdef ZB_BIG_ENDIAN
#define ZBNCP_LL_SIGN             0xdeadu                   /**< LL packet signature */
#else
#define ZBNCP_LL_SIGN             0xaddeu                   /**< LL packet signature */
#endif
#define ZBNCP_LL_TYPE_NCP         0x06u                     /**< LL packet type */
#define ZBNCP_LL_FLAGS_NONE       0u                        /**< LL packet header flags value showing that the packet is a regular packet */
#define ZBNCP_LL_FLAG_ACK         1u                        /**< LL packet header flag showing that the packet is an acknowledge packet */
#define ZBNCP_LL_FLAG_RETRANSMIT  2u                        /**< LL packet header flag showing that the sender should retransmit last packet */
#define ZBNCP_LL_FLAGS_MASK       0xC3u                     /**< Mask for the LL packet header flags */
#define ZBNCP_LL_PKT_NUM_SHIFT    2u                        /**< Starting bit position of the packet frame number in the LL packet header flags */
#define ZBNCP_LL_ACK_NUM_SHIFT    4u                        /**< Starting bit position of the acknowledge frame number in the LL packet header flags */
#define ZBNCP_LL_NUM_MASK         3u                        /**< Bit mask for extracting frame numbers from the LL packet header flags */
#define ZBNCP_LL_NUM_BOOT         0u                        /**< Frame number of the BOOT LL packet */
#define ZBNCP_LL_NUM_RX_INIT      (ZBNCP_LL_NUM_MASK + 1u)  /**< Value that can't match any packet # */
#define ZBNCP_LL_NUM_TX_INIT      (ZBNCP_LL_NUM_BOOT)       /**< Only the first TX packet can be a bootstrap packet */
#define ZBNCP_LL_PKT_LIM_SHIFT    6u                        /**< Starting bit position of the packet limits in the LL packet header flags */
#define ZBNCP_LL_PKT_LIM_MASK     0xC0u                     /**< Mask for the LL packet header flag showing first and last fragment */
#define ZBNCP_LL_PKT_START        1u                        /**< Marker showing first fragment of a packet */
#define ZBNCP_LL_PKT_END          2u                        /**< Marker showing last fragment of a packet */

/**
@addtogroup NCP_TRANSPORT_API
@{
*/

/**
   Maximum size of LL packet at physical level.
   This is also size of i/o buffer in NCP LL transport level
   Single packet can hold a single message fragment, or the whole message if it is not fragmented.

   Maximum fragmented message size is defined by @ref ZBNCP_BIG_BUF_SIZE
   @see NCP_HOST_MEM
*/
#define ZBNCP_LL_PKT_SIZE_MAX     256u

/**
}@
*/


#define ZBNCP_LL_SIGN_SIZE        2u                        /**< Size of the LL packet signature */
#define ZBNCP_LL_HDR_SIZE         5u                        /**< Size of the LL packet header */
#define ZBNCP_LL_HDR_CRC_SIZE     1u                        /**< Size of the LL packet header CRC */
#define ZBNCP_LL_BODY_CRC_OFFSET  (ZBNCP_LL_SIGN_SIZE + ZBNCP_LL_HDR_SIZE)
                                                            /**< Offset of the LL packet body CRC */
#define ZBNCP_LL_BODY_CRC_SIZE    2u                        /**< Size of the LL packet body CRC */
#define ZBNCP_LL_BODY_OFFSET      (ZBNCP_LL_BODY_CRC_OFFSET + ZBNCP_LL_BODY_CRC_SIZE)
                                                            /**< Offset of the LL packet body data */
#define ZBNCP_LL_BODY_SIZE_MAX    (ZBNCP_LL_PKT_SIZE_MAX - ZBNCP_LL_BODY_OFFSET)
                                                            /**< Maximum size of LL packet body data */
#define ZBNCP_LL_HDR_LEN_MIN      (ZBNCP_LL_HDR_SIZE)       /**< Minimum value of LL packet length in the header */
#define ZBNCP_LL_HDR_LEN_BCRC     (ZBNCP_LL_HDR_LEN_MIN + ZBNCP_LL_BODY_CRC_SIZE)
                                                            /**< Minimum value of LL packet length in the header plus a body CRC */
#define ZBNCP_LL_HDR_LEN_MAX      (ZBNCP_LL_HDR_LEN_BCRC + ZBNCP_LL_BODY_SIZE_MAX)
                                                            /**< Maximum value of LL packet length in the header */

/* @brief Structure of the NCP low-level packet header */
#if defined ZBNCP_PACKED_PRE
#pragma pack(1)
#endif
typedef struct zbncp_ll_hdr_s
{
  zbncp_uint16_t len;      /**< Packet length not including packet signature */
  zbncp_uint8_t type;     /**< Packet type */
  zbncp_uint8_t flags;    /**< Packet flags and frame number */
  zbncp_uint8_t crc;      /**< Packet header CRC */
} ZBNCP_PACKED_STRUCT
zbncp_ll_hdr_t;

/* @brief Structure of the NCP low-level packet body */
typedef struct zbncp_ll_body_s
{
  zbncp_uint16_t crc;     /**< Packet body CRC */
  zbncp_uint8_t data[ZBNCP_LL_BODY_SIZE_MAX];
                          /**< Packet body data */
} ZBNCP_PACKED_STRUCT
zbncp_ll_body_t;

typedef struct zbncp_ll_pkt_wo_bodys
{
  zbncp_uint16_t sign;    /**< Packet signature */
  zbncp_ll_hdr_t hdr;     /**< Packet header */
} ZBNCP_PACKED_STRUCT
zbncp_ll_pkt_wo_body_t;

/* @brief Structure of the whole NCP low-level packet */
typedef struct zbncp_ll_pkt_s
{
  zbncp_uint16_t sign;    /**< Packet signature */
  zbncp_ll_hdr_t hdr;     /**< Packet header */
  zbncp_ll_body_t body;   /**< Packet body (if present) */
} ZBNCP_PACKED_STRUCT
zbncp_ll_pkt_t;
#if defined ZBNCP_PACKED_POST
#pragma pack()
#endif

ZBNCP_DBG_STATIC_ASSERT(sizeof(zbncp_ll_hdr_t ) == ZBNCP_LL_HDR_SIZE)
ZBNCP_DBG_STATIC_ASSERT(sizeof(zbncp_ll_body_t) == (ZBNCP_LL_BODY_CRC_SIZE + ZBNCP_LL_BODY_SIZE_MAX))
ZBNCP_DBG_STATIC_ASSERT(sizeof(zbncp_ll_pkt_t ) == ZBNCP_LL_PKT_SIZE_MAX)


/**
 * @brief Extract acknowledge number from the packet header flags.
 */
static inline zbncp_uint8_t zbncp_ll_flags_ack_num(zbncp_uint8_t flags)
{
  return ((flags >> ZBNCP_LL_ACK_NUM_SHIFT) & ZBNCP_LL_NUM_MASK);
}

/**
 * @brief Extract packet number from the packet header flags.
 */
static inline zbncp_uint8_t zbncp_ll_flags_pkt_num(zbncp_uint8_t flags)
{
  return ((flags >> ZBNCP_LL_PKT_NUM_SHIFT) & ZBNCP_LL_NUM_MASK);
}

/**
 * @brief Calculate packet body data size based on the packet header.
 */
static inline zbncp_size_t zbncp_ll_hdr_body_size(const zbncp_ll_hdr_t *hdr)
{
  zbncp_size_t hlen = (zbncp_size_t) hdr->len;
  return ((hlen >= ZBNCP_LL_HDR_LEN_BCRC) ? (hlen - ZBNCP_LL_HDR_LEN_BCRC) : 0u);
}

/**
 * @brief Calculate CRC of the packet header.
 */
static inline zbncp_uint8_t zbncp_ll_hdr_calc_crc(const zbncp_ll_hdr_t *hdr)
{
  zbncp_size_t len = (ZBNCP_LL_HDR_SIZE - ZBNCP_LL_HDR_CRC_SIZE);
  return zbncp_crc8(ZBNCP_CRC8_INIT_VAL, hdr, len);
}

/**
 * @brief Predicate to check whether the packet header has a valid CRC.
 */
static inline zbncp_bool_t zbncp_ll_hdr_crc_is_valid(const zbncp_ll_hdr_t *hdr)
{
  zbncp_uint8_t hcrc = zbncp_ll_hdr_calc_crc(hdr);
  return (hdr->crc == hcrc);
}

/**
 * @brief Predicate to check whether the packet type is known to the NCP
 * low-level protocol implementation.
 */
static inline zbncp_bool_t zbncp_ll_hdr_type_is_supported(const zbncp_ll_hdr_t *hdr)
{
  return (hdr->type == ZBNCP_LL_TYPE_NCP);
}

/**
 * @brief Predicate to check whether the packet header contains a valid packet size.
 */
static inline zbncp_bool_t zbncp_ll_hdr_size_is_valid(const zbncp_ll_hdr_t *hdr)
{
  return ((hdr->len >= ZBNCP_LL_HDR_LEN_MIN) && (hdr->len <= ZBNCP_LL_HDR_LEN_MAX));
}

/**
 * @brief Calculate the packet length to be stored in the packed header based
 * on the size of the packet body data.
 */
static inline zbncp_uint16_t zbncp_ll_calc_hdr_len(zbncp_size_t bsize)
{
  zbncp_size_t hlen = 0u;
  if (bsize <= ZBNCP_LL_BODY_SIZE_MAX)
  {
    hlen = ZBNCP_LL_HDR_SIZE;
    if (bsize > 0u)
    {
      hlen = (hlen + ZBNCP_LL_BODY_CRC_SIZE + bsize);
    }
  }
  return (zbncp_uint16_t)hlen;
}

/**
 * @brief Calculate the total packet size based on the size of the packet body data.
 */
static inline zbncp_size_t zbncp_ll_calc_total_pkt_size(zbncp_size_t bsize)
{
  zbncp_size_t hlen = zbncp_ll_calc_hdr_len(bsize);
  return (ZBNCP_LL_SIGN_SIZE + hlen);
}

/**
 * @brief Calculate the packet body CRC.
 */
static inline zbncp_uint16_t zbncp_ll_calc_body_crc(const zbncp_ll_body_t *body, zbncp_size_t bsize)
{
  return zbncp_crc16(ZBNCP_CRC16_INIT_VAL, body->data, bsize);
}

#endif /* ZBNCP_INCLUDE_GUARD_LL_PKT_H */
