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
/*  PURPOSE: Debug routines declaration.
*/
#ifndef ZBNCP_INCLUDE_GUARD_DEBUG_H
#define ZBNCP_INCLUDE_GUARD_DEBUG_H 1

#include "zbncp_types.h"

#if ZBNCP_DEBUG

typedef enum {
  ZBNCP_DBG_IGNORE,
  ZBNCP_DBG_BREAK,
  ZBNCP_DBG_ABORT,
} zbncp_dbg_res_t;

zbncp_dbg_res_t zbncp_dbg_assert(const char *file, unsigned int line, const char *cond);
void zbncp_dbg_break(void);
void zbncp_dbg_trace(const char *fmt, ...);

#define ZBNCP_DBG_ASSERT(cond)                          \
  do {                                                  \
    if (!(cond)) {                                      \
      zbncp_dbg_res_t res = zbncp_dbg_assert(__FILE__,  \
        __LINE__, #cond);                               \
      if (res == ZBNCP_DBG_BREAK) {                     \
        zbncp_dbg_break();                              \
      }                                                 \
    }                                                   \
  } while (0)

#define ZBNCP_DBG_STATIC_ASSERT(cond)                   \
  _Static_assert((cond), "Static assertion failed");

#define ZBNCP_DBG_TRACE(...)                            \
  do {                                                  \
    zbncp_dbg_trace("%24s: ", __func__);                \
    zbncp_dbg_trace(__VA_ARGS__);                       \
    zbncp_dbg_trace("\n");                              \
  } while (0)

#else /* ZBNCP_DEBUG */

#define ZBNCP_DBG_TRACE(...)          ((void) 0)
#define ZBNCP_DBG_ASSERT(cond)        ((void) 0)
#define ZBNCP_DBG_STATIC_ASSERT(cond)

#endif /* ZBNCP_DEBUG */

#endif /* ZBNCP_INCLUDE_GUARD_DEBUG_H */
