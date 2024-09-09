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

#ifndef SECUR_AES128_H
#define SECUR_AES128_H 1

#ifndef ZB_WINDOWS
#include "zb_common.h"
#else
#include "zb_types.h"
#endif /* !ZB_WINDOWS */

/*
  XAP5 platform has HW aes128 but no aes128 decryption.
  aes128 decrypt is necessary for ZLL only, so can use HW aes128 for HA and
  exclyde soft AES implementation.
 */
#if !defined ZB_HW_ZB_AES128 || defined ZB_SOFT_SECURITY
#define NEED_SOFT_AES128
#endif

#if defined ZB_NEED_AES128_DEC && (!defined ZB_HW_ZB_AES128_DEC || defined ZB_SOFT_SECURITY)
#define NEED_SOFT_AES128_DEC
#endif

#ifdef NEED_SOFT_AES128
void zb_aes128(zb_uint8_t *key, zb_uint8_t *msg, zb_uint8_t *c);
#endif

#endif /* SECUR_AES128_H */
