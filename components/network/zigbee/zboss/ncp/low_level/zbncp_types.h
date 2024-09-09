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
/*  PURPOSE:  ZBOSS NCP global type definitions.
*/
#ifndef ZBNCP_INCLUDE_GUARD_TYPES_H
#define ZBNCP_INCLUDE_GUARD_TYPES_H 1

#include "zbncp_defs.h"

/** @par Basic type definitions used across the library. */

#if ZBNCP_USE_STDTYPES

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>


typedef int8_t      zbncp_int8_t;
typedef int16_t     zbncp_int16_t;
typedef int32_t     zbncp_int32_t;
typedef int64_t     zbncp_int64_t;
typedef uint8_t     zbncp_uint8_t;
typedef uint16_t    zbncp_uint16_t;
typedef uint32_t    zbncp_uint32_t;
typedef uint64_t    zbncp_uint64_t;
typedef intptr_t    zbncp_intptr_t;
typedef uintptr_t   zbncp_uintptr_t;
typedef ptrdiff_t   zbncp_ptrdiff_t;
typedef size_t      zbncp_size_t;
typedef bool        zbncp_bool_t;

#define ZBNCP_FALSE false
#define ZBNCP_TRUE  true

#else /* ZBNCP_USE_STDTYPES */

typedef char                   zbncp_int8_t;
typedef short int              zbncp_int16_t;
typedef int                    zbncp_int32_t;
typedef long long int          zbncp_int64_t;
typedef unsigned char          zbncp_uint8_t;
typedef unsigned short int     zbncp_uint16_t;
typedef unsigned int           zbncp_uint32_t;
typedef unsigned long long int zbncp_uint64_t;

#if (ZBNCP_PTR_SIZE == 4u)
typedef zbncp_int32_t          zbncp_intptr_t;
typedef zbncp_uint32_t         zbncp_uintptr_t;
typedef zbncp_intptr_t         zbncp_ptrdiff_t;
typedef zbncp_uint32_t         zbncp_size_t;
#elif (ZBNCP_PTR_SIZE == 8u)
typedef zbncp_int64_t          zbncp_intptr_t;
typedef zbncp_uint64_t         zbncp_uintptr_t;
typedef zbncp_intptr_t         zbncp_ptrdiff_t;
typedef zbncp_uint64_t         zbncp_size_t;
#else
#error ZBNCP_PTR_SIZE should be defined either as 4u or as 8u.
#endif

typedef _Bool zbncp_bool_t;

#define ZBNCP_FALSE   ((zbncp_bool_t)0)
#define ZBNCP_TRUE    (!ZBNCP_FALSE)

#endif /* ZBNCP_USE_STDTYPES */

#define  ZBNCP_RET_OK         (0)
#define  ZBNCP_RET_ERROR      (-1)
#define  ZBNCP_RET_BLOCKED    (-2)
#define  ZBNCP_RET_EXIT       (-3)
#define  ZBNCP_RET_BUSY       (-4)
#define  ZBNCP_RET_TX_FAILED  (-5)
#define  ZBNCP_RET_NO_ACK     (-6)

#endif /* ZBNCP_INCLUDE_GUARD_TYPES_H */
