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
/* PURPOSE: Smart Plug device configuration
*/

#ifndef SP_CONFIG_H
#define SP_CONFIG_H 1

#define SP_INIT_BASIC_HW_VERSION        2
#define SP_INIT_BASIC_MANUF_NAME	  "DSR"
#define SP_NVRAM_ERASE_AT_START ZB_FALSE

#define SP_DONT_CLOSE_NETWORK_AT_REBOOT

/* Use SP_MAC_ADDR if it is needed to setup IEEE addr manually. */
#define SP_MAC_ADDR {0x96, 0x39, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
//#define SP_OTA

#endif /* SP_CONFIG_H */
