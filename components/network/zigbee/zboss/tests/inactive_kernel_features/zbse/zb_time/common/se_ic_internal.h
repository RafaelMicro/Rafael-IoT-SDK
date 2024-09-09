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
/* PURPOSE: Installcodes for sample devices.
*/

#ifndef SE_INSTALLCODES_H
#define SE_INSTALLCODES_H 1

#if !defined SE_CERT_H
#define IHD_DEV_ADDR          {0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb}
#define EL_METERING_DEV_ADDR  {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11}
#define GAS_METERING_DEV_ADDR {0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22}
#define PCT_DEV_ADDR          {0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33}
#endif


/* -------------------------- IHD device ----------------------------------- */

zb_ieee_addr_t ihd_dev_addr = IHD_DEV_ADDR;
char ihd_installcode[]= "966b9f3ef98ae605 9708";

/* -------------------------- Electric Metering device --------------------- */

zb_ieee_addr_t el_metering_dev_addr = EL_METERING_DEV_ADDR;
char el_metering_installcode[]= "966b9f3ef98ae605 9708";

/* -------------------------- Gas Metering device -------------------------- */

zb_ieee_addr_t gas_metering_dev_addr = GAS_METERING_DEV_ADDR;
char gas_metering_installcode[]= "966b9f3ef98ae605 9708";

/* -------------- Programmable Controlled Thermostat device ---------------- */

zb_ieee_addr_t pct_dev_addr = PCT_DEV_ADDR;
char pct_installcode[]= "966b9f3ef98ae605 9708";


#endif /* SE_INSTALLCODES_H */
