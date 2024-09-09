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
#ifndef ITM_TRACE_H
#define ITM_TRACE_H 1

#ifndef ZB_PLATFORM_LINUX_PC32
#include <process.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <stdint.h>
#else
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "lin_com_dump.h"
#endif

#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

/* #define DEBUG */

#ifdef DEBUG
#define DBG_PRINT(...) fprintf(stderr, "" __VA_ARGS__)
#else
#define DBG_PRINT(...)
#endif

#ifndef ZB_PLATFORM_LINUX_PC32
  #define PACKED_STRUCT
  #define PACKET_STRUCT_PRE 1
#else
  #define PACKED_STRUCT __attribute__ ((packed))
#endif

/* signature is dump and NCP has different endianness */
#define ZB_PREAMBLE_1ST     0xDE
#define ZB_PREAMBLE_2ND     0xAD
#define ZB_PREAMBLE_SIZE    0x02
#define ZB_TRACE_TYPE       0x02

#define MCU_CLOCK     48000000u
#define TS_PRESCALER  16u
#define TICKS_PER_US  3u /* MCU_CLOCK/(TS_PRESCALER*1000000) */

typedef enum pkt_state_e
{
  WAIT_PREAMBLE_1ST = 0,
  WAIT_PREAMBLE_2ND,
  WAIT_LEN_1ST,
  WAIT_LEN_2ND,
  WAIT_TYPE,
  WAIT_CRC,
  WAIT_BODY
}
pkt_state_t;

typedef struct ring_buf_s
{
  size_t rd;
  size_t wr;
  size_t cnt;
  size_t max;
  unsigned char * buf;
}
ring_buf_t;

typedef struct trace_pkt_s
{
  size_t size;
  size_t offset;
  unsigned char * buf;
}
trace_pkt_t;

typedef struct itm_ctx_s
{
  size_t body_size;
  size_t body_pos;
  size_t max;
  pkt_state_t state;
}
itm_ctx_t;

typedef struct trace_s
{
  ring_buf_t rb;
  trace_pkt_t trace_pkt;
  itm_ctx_t itm;
}
trace_t;

void read_itm_trace();

#endif /* ITM_TRACE_H */
