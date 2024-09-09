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
/*  PURPOSE: MAC split API definitions
*/

#ifndef ZB_MACSPLIT_TRANSPORT_H
#define ZB_MACSPLIT_TRANSPORT_H 1

#include "zb_common.h"
#include "zb_types.h"
#include "zb_ringbuffer.h"
#include "zb_config.h"
#include "zb_macsplit_internal.h"
#include "zb_mac_transport.h"
#include "zb_ota_transport.h"

/****************************** MACSPLIT TRANSPORT API ******************************/

/**
 * Type for callback which is used by upper layer for non-call packets handler establishment
 */
typedef void (*recv_data_cb_t)(zb_uint8_t);

/**
 * Macsplit transport initializer,
 * should be called before sending any data
 *
 * @return nothing
 */
void zb_macsplit_transport_init(void);

/**
 * Macsplit transport re-initializer,
 *
 * @return nothing
 */
void zb_macsplit_transport_reinit(void);

#if defined ZB_MACSPLIT_TRACE_DUMP_TO_FILE
/**
 * Provides macsplit trace file path.
 *
 * Weak default implementation provided. Re-implement for custom logic.
 */
void zb_macsplit_trace_dump_file_path_get(char* file_name);
#endif /* defined ZB_MACSPLIT_TRACE_DUMP_TO_FILE */

/**
 * Function for non-call packets handler establishment,
 * callback can be set at any time,
 * if no callback is set then all non-call packets will be discarded
 *
 * @param cb - non-call packets handler
 *
 * @return nothing
 */
void zb_macsplit_set_cb_recv_data(recv_data_cb_t cb);

/**
 * Function for non-call packet sending,
 * on receipt such a packet, handler set by upper layer is invoked
 * with buffer as function parameter,
 * if no handler set, this packet is discarded
 *
 * @param param - bufid of buffer to be transmitted
 *
 * @return nothing
 */
void zb_macsplit_transport_send_data(zb_bufid_t param);

/**
 * Function for call packet sending,
 * on receipt of this packet, an appropriate handler is invoked (usually, MAC primitive)
 * with buffer as function parameter
 *
 * @param param - bufid of buffer to be transmitted
 * @param call_type - call type (for possible call types, see MACSPLIT TRANSPORT PROTOCOL section)
 *
 * @return nothing
 */
void zb_macsplit_transport_send_data_with_type(zb_bufid_t param, zb_transport_call_type_t call_type);


void zb_macsplit_send_ota_msg(zb_bufid_t param);

#ifdef ZB_MACSPLIT_DEVICE
/**
 * Type for callback which is used by upper layer for non-call packets handler establishment
 */
typedef zb_uint32_t (*get_mac_device_version_cb_t)();

/**
 * Function for getting device version from app layer
 * Be sure your app setup this cb before mac initialization
 * if no callback is set then default version 1.0.0.0 will be applied
 *
 */
void zb_macsplit_set_cb_dev_version(get_mac_device_version_cb_t cb);
#endif  /* ZB_MACSPLIT_DEVICE */

/**
 * Function for handling specific incoming call.
 *
 * @param param - ref to buffer to be handling
 *
 * @return nothing
 */
#if defined ZB_MACSPLIT_HOST
void zb_macsplit_transport_handle_data_confirm_call(zb_bufid_t param);
void zb_macsplit_transport_handle_set_get_confirm_call(zb_bufid_t param);
#endif /* ZB_MACSPLIT_HOST */

/**
 * Enumeration for transport types
 */
typedef enum
{
  ZB_MACSPLIT_TRANSPORT_TYPE_SPI = 0,
  ZB_MACSPLIT_TRANSPORT_TYPE_SERIAL,
  ZB_MACSPLIT_TRANSPORT_TYPES_N
}
zb_macsplit_transport_type_e;

/****************************** MACSPLIT TRANSPORT CONFIG ******************************/
/* ES: ZB_MACSPLIT_USE_IO_BUFFERS is disabled for a few platforms actually */
#if !defined ZB_MACSPLIT_DISABLE_IO_BUFFERS
#define ZB_MACSPLIT_USE_IO_BUFFERS
#endif

#if defined ZB_MACSPLIT_USE_IO_BUFFERS

void zb_macsplit_transport_recv_bytes(void);

#ifndef ZB_MACSPLIT_TRANSPORT_BUFFER_CAPACITY
#define ZB_MACSPLIT_TRANSPORT_BUFFER_CAPACITY 255U
#endif /* ZB_MACSPLIT_TRANSPORT_BUFFER_CAPACITY */
ZB_RING_BUFFER_DECLARE(zb_macsplit_transport_buffer, zb_uint8_t, ZB_MACSPLIT_TRANSPORT_BUFFER_CAPACITY);
#endif /* defined ZB_MACSPLIT_USE_RX_BUFFER */

#define ZB_MACSPLIT_TRANSPORT_TX_QUEUE_SIZE   ZB_IOBUF_POOL_SIZE
ZB_RING_BUFFER_DECLARE(zb_macsplit_transport_tx_queue, zb_uint8_t, ZB_MACSPLIT_TRANSPORT_TX_QUEUE_SIZE);

#define ZB_CRC16_INITIAL_VALUE 0U
#define ZB_CRC8_INITIAL_VALUE  0U

#if defined ZB_PLATFORM_LINUX
#define ZB_HAVE_IOCTX
#define TRANSPORT_BUFFER_SIZE 1024U
ZB_RING_BUFFER_DECLARE(zb_transport_buffer, zb_uint8_t, TRANSPORT_BUFFER_SIZE);

/* name of file where trace and dump from device is stored */
#define DUMP_FILENAME "device.dump"

#endif /* defined ZB_PLATFORM_LINUX */


/****************************** MACSPLIT TRANSPORT PROTOCOL ******************************/

/**
 * Description of protocol for MAC split architecture
 *
 * This protocol is used for transmitting stack calls between host and device,
 * transmitting dump and trace from device to host.
 *
 * Packet structure:
 *   header:
 *     length of the packet: 1 byte - length includes header and body, doesn't include signature
 *     type of the packet: 1 byte - dump, trace or macsplit data
 *     flags: 1 byte - is_ack, should_retransmit, packet number and call type  | in case of non-macsplit packet
 *     crc: 1 byte - crc, if MAC split data                                    | these 2 bytes are used for time field
 *   body:
 *     data : ZB_TRANSPORT_DATA_SIZE bytes
 *     crc: 2 bytes
 *
 */
#ifdef ZB_TH_ENABLED
#define ZB_MACSPLIT_RECV_DATA_TIMEOUT    400U    /* in msec. */
#else
#define ZB_MACSPLIT_RECV_DATA_TIMEOUT    200U    /* in msec. Increase to 200. */
#endif
/**
 *  Retransmit timeout (in milliseconds)
 *
 *  Attention: retransmit timeout is related to maximum count of retransmit
 *  attempts. If you decrease retransmit timeout you should increase maximum
 *  count of retransmit attempts.
 */
#define ZB_MACSPLIT_RETRANSMIT_TIMEOUT   500U

/**
 *  Maximum count of retransmit attempts
 *
 *  Attention: set bigger count of retransmit attempts for the Host side to be
 *  sure that there is real OOM state (or buffers leakage) on the Radio side
 */
#if defined ZB_MACSPLIT_HOST
#define ZB_MACSPLIT_RETRANSMIT_MAX_COUNT 50U
#else
#define ZB_MACSPLIT_RETRANSMIT_MAX_COUNT 12U
#endif /* ZB_MACSPLIT_HOST */

#if defined ZB_MACSPLIT_HOST
#define ZB_MACSPLIT_CONFIRM_TIMEOUT      10000U  /* in msec */
#endif /* ZB_MACSPLIT_HOST */

#if defined ZB_MACSPLIT_DEVICE
#define ZB_MACSPLIT_BOOT_INDICATION_RETRANSMIT_TIMEOUT  3U * 60U /*  in sec */
#endif  /* ZB_MACSPLIT_DEVICE */

#define SIGNATURE_SIZE                   2U
#define SIGNATURE_FIRST_BYTE             0xDEU
#define SIGNATURE_SECOND_BYTE            0xADU

#define ZB_TRANSPORT_MAX_PACKET_NUMBER   3U
#define ZB_TRANSPORT_MIN_PACKET_NUMBER   0U

#define ZB_TRANSPORT_BODY_CRC_SIZE       2U
/* TODO: adjust size */
#define ZB_TRANSPORT_DATA_SIZE           200U
#define ZB_TRANSPORT_PKT_MAX_SIZE \
  (SIGNATURE_SIZE + sizeof(zb_macsplit_packet_t))

#ifndef ZB_MACSPLIT_TRACE_BUF_SIZE
#define ZB_MACSPLIT_TRACE_BUF_SIZE ZB_TRANSPORT_PKT_MAX_SIZE
#endif /* ZB_MACSPLIT_TRACE_BUF_SIZE */

/* 08/22/2018 EE CR:MINOR Better put trace file name size and names into the vendor file. */
#define ZB_TRACE_DUMP_FILE_PATH_SIZE     30U

typedef ZB_PACKED_PRE struct zb_transport_flags_s
{
  zb_bitfield_t               is_ack            : 1;
  zb_bitfield_t               should_retransmit : 1;
  zb_bitfield_t               packet_number     : 2;
  zb_bitfield_t               call_type         : 4; /* actual for sgw, else reserved */
} ZB_PACKED_STRUCT
zb_transport_flags_t;


typedef ZB_PACKED_PRE struct zb_macsplit_transport_hdr_s
{
  zb_uint8_t           len;
  zb_uint8_t           type;
  zb_transport_flags_t flags;
  zb_uint8_t           crc;
} ZB_PACKED_STRUCT
zb_macsplit_transport_hdr_t;

/* Ensure that transport headers are always of the same size */
ZB_ASSERT_COMPILE_DECL((sizeof(zb_macsplit_transport_hdr_t) == sizeof(zb_mac_transport_hdr_t)));

typedef ZB_PACKED_PRE struct zb_macsplit_transport_body_s
{
  zb_uint8_t msdu_handle;
  zb_uint16_t call_type;
  zb_uint8_t data[ZB_TRANSPORT_DATA_SIZE];
  zb_uint16_t crc;
} ZB_PACKED_STRUCT
  zb_macsplit_transport_body_t;

typedef struct zb_macsplit_packet_s
{
  zb_macsplit_transport_hdr_t hdr;
  zb_macsplit_transport_body_t body;
}
  zb_macsplit_packet_t;

typedef enum zb_transport_state_e
{
  /* signature */
  RECEIVING_SIGNATURE,

  /* header/common */
  RECEIVING_LENGTH,
  RECEIVING_TYPE,

  /* header/mac_data */
  RECEIVING_FLAGS,
  RECEIVING_HDR_CRC,

  /* header/trace */
  RECEIVING_TIME,

  /* body/mac_data */
  RECEIVING_MSDU_HANDLE,
  RECEIVING_CALL_TYPE,


  /* body/common */
  RECEIVING_BODY_DATA,
  RECEIVING_BODY_CRC

} zb_transport_state_t;

#define ZB_RECV_CFM_NBYTES ((ZB_IOBUF_POOL_SIZE + 7U) / 8U)

typedef struct zb_macsplit_transport_specific_s
{
#if defined ZB_MACSPLIT_DEVICE
  zb_uint8_t msdu_handles[ZB_N_BUF_IDS];  /*!< store msdu_handle from host */
#else
  zb_callback_t confirm_cb[ZB_N_BUF_IDS]; /*!< store callbacks by host */
  zb_uint8_t    recv_cfm[ZB_RECV_CFM_NBYTES]; /*!< store received confirm packets buf id */
#endif /* ZB_MACSPLIT_DEVICE */
} zb_macsplit_transport_specific_t;

#if defined ZB_MACSPLIT_HOST
typedef zb_ret_t (*zb_macsplit_device_trace_cb_t)(zb_uint8_t *buf, zb_uint8_t len);
void zb_macsplit_set_device_trace_cb(zb_macsplit_device_trace_cb_t cb);
#endif

#ifdef ZB_MACSPLIT_HANDLE_DATA_BY_APP
typedef void (*zb_macsplit_handle_data_by_app)(zb_uint8_t byte);
#endif

typedef struct zb_macsplit_transport_context_s
{
#if defined ZB_MACSPLIT_USE_IO_BUFFERS
  ZB_VOLATILE zb_macsplit_transport_buffer_t   rx_buffer; /*!< buffer for incoming data used by osif layer */
  ZB_VOLATILE zb_macsplit_transport_buffer_t   tx_buffer; /*!< buffer for outcoming data used by osif layer */
#endif
#if defined ZB_MACSPLIT_DEVICE && (defined ZB_SERIAL_FOR_TRACE || defined ZB_TRACE_OVER_USART)
  zb_uint8_t                       trace_buffer[ZB_MACSPLIT_TRACE_BUF_SIZE]; /*!< inner buffer for sending trace to HW */
  zb_short_t                       trace_buffer_data;
#endif
  zb_uint8_t                       tx_inner_buffer[ZB_TRANSPORT_PKT_MAX_SIZE]; /*!< inner buffer for sending data to HW driver */
  zb_uint8_t                       tx_calls_table[ZB_N_BUF_IDS];         /*!< translation table buffer to call-type */
  zb_macsplit_transport_tx_queue_t tx_queue;            /*!< queue for outcoming calls */
  recv_data_cb_t                   recv_data_cb;        /*!< callback for non-call packet handler */
  zb_callback_t                    ethernet_cb;         /*!< just to compile, used in zb_scheduler.c */
  zb_uint16_t                      received_bytes;      /*!< number of bytes received in current FSM state */
  zb_transport_state_t             transport_state;     /*!< current FSM state */
  zb_macsplit_packet_t             rx_pkt;              /*!< last (or is currently being) received packet */
  zb_macsplit_packet_t             tx_pkt;              /*!< last (or is currently being) sent packet */
  zb_macsplit_transport_hdr_t      ack_pkt;             /*!< ack packet */
  zb_bool_t                        is_waiting_for_ack;  /*!< if an ack to sent packet was received  */
  zb_uint8_t                       curr_pkt_number;     /*!< number of next outcoming packet */
  zb_uint8_t                       last_rx_pkt_number;  /*!< number of last received packet */
  zb_macsplit_transport_type_e     transport_type;      /*!< transport type: serial or spi, should be set before init */
  zb_uint8_t                       retransmit_count;    /*!< number of packet retransmit */
  zb_macsplit_transport_specific_t specific_ctx;        /*!< specific context for Host or Device */

  zb_ota_protocol_context_t ota_context;
#if defined ZB_MACSPLIT_TRACE_DUMP_TO_FILE
  zb_osif_file_t                   *trace_file;          /*!< dump file for trace packets */
#endif
#if defined ZB_MACSPLIT_HOST
  zb_uint8_t forced_device_reset;
#ifdef ZB_MACSPLIT_HANDLE_DATA_BY_APP
  zb_uint8_t                       handle_data_by_app;
  zb_uint8_t                       handle_data_by_app_after_last_ack;
  zb_macsplit_handle_data_by_app   handle_data_by_app_cb;
#endif
#endif

  zb_bufid_t                         operation_buf;       /*!< Buffer for future use (some internal data etc) */
#ifdef ZB_MAC_CONFIGURABLE_TX_POWER
  zb_int8_t                        mac_tx_power[ZB_PROD_CFG_APS_CHANNEL_LIST_SIZE][ZB_PROD_CFG_MAC_TX_POWER_CHANNEL_N];
  zb_bool_t                        tx_power_exist;
#endif /* ZB_MAC_CONFIGURABLE_TX_POWER */
#if defined ZB_MACSPLIT_HOST
  zb_macsplit_device_trace_cb_t    device_trace_cb;
#endif
} zb_macsplit_transport_context_t;

typedef ZB_PACKED_PRE struct macsplit_device_ver_s
{
  zb_uint32_t    val;
#ifdef USE_HW_LONG_ADDR
  zb_ieee_addr_t extended_address;          /*!< The 64-bit (IEEE) address assigned to the device. */
#endif
} ZB_PACKED_STRUCT macsplit_device_ver_t;

extern zb_macsplit_transport_context_t g_macsplit;
#define MACSPLIT_CTX() g_macsplit

#if defined ZB_MACSPLIT_HOST
#define MAC_CTX() MACSPLIT_CTX()
#endif

#define OTA_CTX() MACSPLIT_CTX().ota_context

#if defined ZB_MACSPLIT_HOST
void zb_macsplit_mlme_mark_radio_reset(void);
#endif /* defined ZB_MACSPLIT_HOST */

#ifdef ZB_PLATFORM_LINUX
#include "zb_macsplit_transport_linux.h"
#elif defined ZB_WINDOWS
#include "zb_macsplit_transport_windows.h"
#endif

#ifdef ZB_MACSPLIT_HANDLE_DATA_BY_APP
void zb_mac_transport_handle_data_by_app_set(zb_uint8_t flag, zb_uint8_t after_ack);
void zb_mac_transport_handle_data_by_app_set_cb(zb_macsplit_handle_data_by_app cb);
int zb_mac_transport_get_fd();
#endif

/* MACSPLIT Radio Failure error indication code */
#define ZB_ERROR_MACSPLIT_RADIO_FAILURE 1u

#endif /* ZB_MACSPLIT_TRANSPORT_H */
