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
#ifndef __AES128_H__
#define __AES128_H__

#include "types.h"

/**
 * AES-128 encryption interface
 *
 * @param key    128-bit secret key
 * @param data   128-bit plaintext/ciphertext
 * @param c      128-bit encrypted data
 */
void aes_block_enc(uint8 *key, uint8 *msg, uint8 *c);

#endif  /* __AES_H__ */
