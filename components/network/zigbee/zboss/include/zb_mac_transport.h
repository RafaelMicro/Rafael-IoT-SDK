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
/* PURPOSE: MAC and trace transport API

*/

#ifndef MAC_TRANSPORT_H
#define MAC_TRANSPORT_H 1

/*! @cond internals_doc */
/*! \addtogroup ZB_MAC_TRANSPORT */
/*! @{ */


/*
   That file is mostly historical.
   Initially it described some serial protocol.
   Now it describes traffic dump format.
*/

/**
   Packet types.

   This enum describes possible packet types to be transfered throught pipes.
 */
typedef enum zb_mac_transport_type_e
{
  ZB_MAC_TRANSPORT_TYPE_CONSOLE  = 0x00, /*!< Data packet type */
  ZB_MAC_TRANSPORT_TYPE_DUMP  = 0x01, /*!< Dump packet type */
  ZB_MAC_TRANSPORT_TYPE_TRACE = 0x02, /*!< Trace packet type */
  ZB_MAC_TRANSPORT_TYPE_CRASH_DATA = 0x03, /*!< Crash dump */
  ZB_MAC_TRANSPORT_TYPE_MAC_SPLIT_DATA = 0x04, /*!< MAC split data */
  ZB_MAC_TRANSPORT_TYPE_OTA_PROTOCOL = 0x05, /*!< OTA protocol used over MACSPLIT */
  ZB_MAC_TRANSPORT_TYPE_NUM
}
zb_mac_transport_type_t;


/**
   Legacy packet header. Now used in ZBOSS internal traffic dump.

 */
typedef ZB_PACKED_PRE struct zb_mac_transport_hdr_s
{
  zb_uint8_t len;  /*!< Packet length, including hdr */
  zb_uint8_t type; /*!< Packet type, @see zb_mac_transport_type_t */
  zb_uint16_t  time;             /*!< time label */
} ZB_PACKED_STRUCT
zb_mac_transport_hdr_t;


typedef ZB_PACKED_PRE struct zb_dump_hdr_v3_s
{
  zb_uint8_t version;
  zb_uint8_t len;
  zb_uint8_t page;
  zb_uint8_t channel;
  zb_uint32_t trace_cnt;
} ZB_PACKED_STRUCT
zb_dump_hdr_v3_t;

typedef ZB_PACKED_PRE struct zb_dump_hdr_s
{
  zb_uint32_t trace_cnt;
  zb_uint8_t channel;
  zb_uint8_t len;
} ZB_PACKED_STRUCT
zb_dump_hdr_t;

#if defined ZB_TRAFFIC_DUMP_ON
void zb_dump_put_buf(zb_uint8_t *buf, zb_uint_t len, zb_bool_t is_w);

void zb_dump_put_2buf(zb_uint8_t *buf1, zb_uint_t len1, zb_uint8_t *buf2, zb_uint_t len2, zb_bool_t is_w);
#endif /* ZB_TRACE_LEVEL */


/*! @} */
/*! @endcond */

#endif /* MAC_TRANSPORT_H */
