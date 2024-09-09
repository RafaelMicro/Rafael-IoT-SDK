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
#include <stdbool.h>
/*cstat +MISRAC* */

/**
 *  @addtogroup NCP_HOST_TYPES
 *  @{
 *  @page ncp_host_types ZBOSS Types definitions for NCP Host
 *  @{
 *  This layer allows to keep
 *  the code portable between ZBOSS NCP firmware and standalone library
 *  containing ZBOSS NCL LL protocol implementation for a Host. It contains typedefs and
 *  dummy implementation of some ZBOSS API.
 *
 *  @}
 *
 */


/**
   Dummy implementation of ZBOSS internal trace macro.

   @param level - unused
   @param fmt   - unused
   @param args  - unused

   @return nothing
 */
#define TRACE_MSG(level, fmt, args) ((void) 0)

/**
   Internal ZBOSS macro checking for trace level enabled.

   Empty (zero) for standalone NCP build
 */
#define TRACE_ENABLED(x) 0

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
/** Definition of ZBOSS return code */
typedef int zb_ret_t;

/** Definition of ZBOSS generic boolean type. */
typedef _Bool zb_bool_t;
/** False value literal. */
#define ZB_FALSE false
/** True value literal. */
#define ZB_TRUE true

/**
   Implementation of ZBOSS macro for unused arguments/variables.

   @param x - unused argumant/variable name
*/
#define ZVUNUSED(x) ((void)(x))

/**
   Internal ZBOSS assert macro.

   Empty for standalone NCP host lib
 */
#define ZB_ASSERT(x) (ZVUNUSED(x))

/**
   Internal ZBOSS memcpy definition.
   Let's cast return value to (void), we don't use it anywhere, and this casting let us avoid MISRA Rule 17.7 violation
 */
#define ZB_MEMCPY (void)memcpy
/**
 * @}
 */
#else

#include "zboss_api_core.h"

#endif

#endif /* NCP_COMMON_H */
