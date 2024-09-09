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
/* PURPOSE:
*/
#ifndef __TYPES_H__
#define __TYPES_H__

#define N 8

#ifndef _WIN32
#include <inttypes.h>
#endif

/**
 * Define data type
 */
typedef unsigned char uint8;          /* Unsigned 8  bit value */
typedef unsigned short uint16;        /* Unsigned 16 bit value */
typedef unsigned long uint32;         /* Unsigned 32 bit value */
#ifndef _WIN32
typedef uint64_t uint64;
#else
typedef unsigned __int64 uint64;      /* Unsigned 64 bit value */
#endif
typedef long int sint32;              /* Signed 32 bit value */


#endif  /* __TYPES_H__ */
