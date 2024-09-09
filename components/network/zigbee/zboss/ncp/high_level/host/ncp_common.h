/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * http://www.dsr-zboss.com
 * http://www.dsr-corporation.com
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
/*  PURPOSE: NCP Low level protocol definitions for the host side
*/
#ifndef NCP_COMMON_H
#define NCP_COMMON_H 1

#ifndef ZB_NCP_HOST_RPI
#define ZB_NCP_HOST_RPI 0
#endif /* ZB_NCP_HOST_RPI */

#if ZB_NCP_HOST_RPI

#include <stdint.h>
/*cstat -MISRAC* */
#include <time.h>
/*cstat +MISRAC* */

/**
 *  @mainpage ZBOSS API License
 *
 * @par ZBOSS Zigbee 3.0 stack
 * Zigbee 3.0 stack, also known as ZBOSS v3.0 (R) Zigbee stack is available
 * under the terms of the DSR Commercial License.
 *
 * ZBOSS is a registered trademark of DSR Corporation AKA Data Storage
 * Research LLC.
 *
 * @par Commercial Usage
 * Licensees holding valid DSR Commercial licenses may use
 * this file in accordance with the DSR Commercial License
 * Agreement provided with the Software or, alternatively, in accordance
 * with the terms contained in a written agreement between you and DSR.
 */

/**
 *  @defgroup NCP_HOST_TYPES ZBOSS API compatibility layer for RPI NCP Host-side library
 *
 *  @par Purpose
 *  This layer allows to keep the code portable between ZBOSS NCP firmware and
 *  standalone dynamic library for Raspberry PI. It contains dummy implementation
 *  of some ZBOSS API.
 * @{
 */

/**
   Dummy implementation of ZBOSS internal trace macro.

   @param level - unused
   @param fmt   - unused
   @param args  - unused

   @return nothing
 */
#define TRACE_MSG(level, fmt, args) ((void) 0)

/** Definition of ZBOSS unsigned 8-bit integer type. */
typedef uint8_t zb_uint8_t;
/** Definition of ZBOSS unsigned 16-bit integer type. */
typedef uint16_t zb_uint16_t;
/** Definition of ZBOSS unsigned 32-bit integer type. */
typedef uint32_t zb_uint32_t;
/** Definition of ZBOSS unsigned short integer type. */
typedef uint16_t zb_ushort_t;
/** Definition of ZBOSS unsigned integer type. */
typedef unsigned int zb_uint_t;
/** Definition of ZBOSS time representation type. */
typedef unsigned long int zb_time_t;
/** Definition of ZBOSS generic callback type. */
typedef void (*callback_t)(zb_uint8_t);

/**
   Implementation of ZBOSS macro for unused arguments/variables.

   @param x - unused argumant/variable name
*/
#define ZVUNUSED(x) ((void)(x))

/**
   Implementation of ZBOSS function obtaining current transceiver time.

   @return current time value in usec
*/
static inline zb_time_t osif_transceiver_time_get(void)
{
  long long time;
  struct timespec ts;

  (void)clock_gettime(CLOCK_MONOTONIC, &ts);

  time = (long long)ts.tv_sec;
  time = time * 1000000000LL; /* Convert seconds to nanoseconds */
  time = time + ts.tv_nsec;
  time = time / 1000LL;       /* Convert nanoseconds to mcroseconds */

  return (zb_time_t)time;
}

/**
 * @}
 */
#else

#include "zboss_api_core.h"

#endif

#endif /* NCP_COMMON_H */
