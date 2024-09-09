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
/*  PURPOSE: ZBOSS NCP global definitions.
*/
#ifndef ZBNCP_INCLUDE_GUARD_DEFS_H
#define ZBNCP_INCLUDE_GUARD_DEFS_H 1

#ifndef ZBNCP_DEBUG
/**
 * @brief Configuration parameter defining whether to use debug
 * fascilities of the library (debug tracing and asserting).
 */
#define ZBNCP_DEBUG 0
#endif /* ZBNCP_DEBUG */

#ifndef ZBNCP_USE_ZBOSS_TRACE
/**
 * @brief Configuration parameter defining whether to use ZBOSS
 * debug tracing fascilities or implement custom ones.
 */
#define ZBNCP_USE_ZBOSS_TRACE 0
#endif /* ZBNCP_USE_ZBOSS_TRACE */

#ifndef ZBNCP_USE_STDTYPES
/**
 * @brief Configuration parameter defining whether to use types provided
 * by the C standard library, or to declare all the types ourselves.
 */
#define ZBNCP_USE_STDTYPES 0
#endif /* ZBNCP_USE_STDTYPES */

#ifndef ZBNCP_USE_STDMEM
/**
 * @brief Configuration parameter defining whether to use memory manipulation
 * routines from the standard library or to implement them ourselves.
 */
#define ZBNCP_USE_STDMEM 0
#endif /* ZBNCP_USE_STDMEM */

#ifndef ZBNCP_USE_STRUCT_PACKING
/**
 * @brief Configuration parameter defining whether to use compiler
 * extensions to define packed structures (i.e. structures with
 * fields aligned on a byte boundary without any padding). All the
 * structures requiring the packing are declared so that all the
 * fields are naturally aligned without any paddings. Nevertheless
 * it is very easy to break this behaviour unintentionally, so the
 * enforcement of the byte alignment rules can be desireable.
 */
#define ZBNCP_USE_STRUCT_PACKING 1
#endif /* ZBNCP_USE_STRUCT_PACKING */

#ifndef ZBNCP_USE_U8_FIFO_INDEX
/**
 * @brief Configuration parameter defining whether to use 8-bit integers
 * or platform native-sized integers for indexing FIFO buffers.
 */
#define ZBNCP_USE_U8_FIFO_INDEX 1
#endif /* ZBNCP_USE_U8_FIFO_INDEX */

#if ZBNCP_USE_STRUCT_PACKING

#if (defined __IAR_SYSTEMS_ICC__ || defined __ARMCC_VERSION) && !defined ZB8051
  /* IAR or Keil ARM CPU */
  #define ZBNCP_PACKED_PRE
  #define ZBNCP_PACKED_POST
#endif

#if defined __GNUC__ || defined __TI_COMPILER_VERSION__
  #define ZBNCP_PACKED_STRUCT __attribute__((packed))
#else
  #define ZBNCP_PACKED_STRUCT
#endif

#else

#define ZBNCP_PACKED_STRUCT

#endif

#if defined __GNUC__ && defined __x86_64__
/** @brief Definition of pointer size for 64-bit platform. */
#define ZBNCP_PTR_SIZE 8u
#else
/** @brief Definition of pointer size for 32-bit platform. */
#define ZBNCP_PTR_SIZE 4u
#endif

/**
 * @brief Macro to show to the compiler that we intentionally want
 * to have unused function argument or variable.
 */
#define ZBNCP_UNUSED(x) ((void)(x))

/** @brief Macro to determine a count of an array elements. */
#define ZBNCP_COUNTOF(x) (sizeof(x) / sizeof((x)[0]))

#endif /* ZBNCP_INCLUDE_GUARD_DEFS_H */
