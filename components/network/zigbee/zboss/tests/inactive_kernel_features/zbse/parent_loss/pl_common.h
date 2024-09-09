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
/*  PURPOSE: Common settings
*/

#ifndef PL_COMMON_H
#define PL_COMMON_H 1

#define PL_ZC_IEEE_ADDR  {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define PL_ZR1_IEEE_ADDR {0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb}
#define PL_ZR2_IEEE_ADDR {0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc}
#define PL_ZED_IEEE_ADDR {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}

#define TEST_CHANNEL_MASK ((1l<<11) | (1l<<25))

#endif /* PL_COMMON_H */
