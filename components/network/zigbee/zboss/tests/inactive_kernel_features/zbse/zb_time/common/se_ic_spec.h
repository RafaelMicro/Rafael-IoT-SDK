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

/*  Now all devices have equal MAC Addresses because of equal certificates  */

#if !defined SE_CERT_H

#define EMBEDDED_MAC_FROM_SPEC_CERT_CS1 {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define EMBEDDED_MAC_FROM_SPEC_CERT_CS2 {0x12, 0x10, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a}

#if (defined SE_CRYPTOSUITE_1 && defined SE_CRYPTOSUITE_2)
#error Please define only one CRYPTOSUITE or use another se_ic.h
#endif

#if defined SE_CRYPTOSUITE_1
#define IHD_DEV_ADDR EMBEDDED_MAC_FROM_SPEC_CERT_CS1
#define EL_METERING_DEV_ADDR EMBEDDED_MAC_FROM_SPEC_CERT_CS1
#define GAS_METERING_DEV_ADDR EMBEDDED_MAC_FROM_SPEC_CERT_CS1
#define PCT_DEV_ADDR EMBEDDED_MAC_FROM_SPEC_CERT_CS1
#define ADDITIONAL_DEV_ADDR EMBEDDED_MAC_FROM_SPEC_CERT_CS2
#elif defined SE_CRYPTOSUITE_2
#define IHD_DEV_ADDR EMBEDDED_MAC_FROM_SPEC_CERT_CS2
#define EL_METERING_DEV_ADDR EMBEDDED_MAC_FROM_SPEC_CERT_CS2
#define GAS_METERING_DEV_ADDR EMBEDDED_MAC_FROM_SPEC_CERT_CS2
#define PCT_DEV_ADDR EMBEDDED_MAC_FROM_SPEC_CERT_CS2
#define ADDITIONAL_DEV_ADDR EMBEDDED_MAC_FROM_SPEC_CERT_CS1
#endif

#endif /* SE_CERT_H */
/* ------------------------- IHD device ----------------------------------- */
zb_ieee_addr_t ihd_dev_addr = IHD_DEV_ADDR;
char ihd_installcode[]= "966b9f3ef98ae605 9708";

/* ------------------------- Electric Metering device --------------------- */
zb_ieee_addr_t el_metering_dev_addr = EL_METERING_DEV_ADDR;
char el_metering_installcode[]= "966b9f3ef98ae605 9708";

/* ------------------------- Gas Metering device -------------------------- */
zb_ieee_addr_t gas_metering_dev_addr = GAS_METERING_DEV_ADDR;
char gas_metering_installcode[]= "966b9f3ef98ae605 9708";

/* -------------- Programmable Controlled Thermostat device --------------- */
zb_ieee_addr_t pct_dev_addr = PCT_DEV_ADDR;
char pct_installcode[]= "966b9f3ef98ae605 9708";

/* - Additional device for support of complementary MAC devices (for unified HEX) - */
#ifdef ADDITIONAL_DEV_ADDR
zb_ieee_addr_t add_dev_addr = ADDITIONAL_DEV_ADDR;
char add_installcode[]= "966b9f3ef98ae605 9708";
#endif

#endif /* SE_INSTALLCODES_H */
