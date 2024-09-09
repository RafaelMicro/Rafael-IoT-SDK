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
#ifndef __AESMMO_H__
#define __AESMMO_H__

#define NO_BLOCK_CHAINING 0
#define WITH_BLOCK_CHAINING 1

/**
 * AES MMO construction
 *
 * @param M      an octet message M 
 * @param Mlen   the length of the message M in bits (Mlen < 2^64)
 * @param h      the hash value of the message M (128 bits)
 * succeed       return 1
 * fail          return 0
 */
uint32 AES_MMO(uint8 *M, uint32 Mlen, uint8 *h, uint8 DoBlockChaining);

#endif  /* __AESCCM_H__ */
