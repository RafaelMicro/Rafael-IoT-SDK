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
/* PURPOSE: Reporting configuration for DUT
*/
#ifndef __FB_PRE_TC_08_DUT_REPORTING_CONFIG_
#define __FB_PRE_TC_08_DUT_REPORTING_CONFIG_


#define DUT_TEMP_VALUE_MIN_INTERVAL 10
#define DUT_TEMP_VALUE_MAX_INTERVAL 70
#define DUT_TEMP_VALUE_CHANGE       1

#define DUT_TEMP_TOLERANCE_MIN_INTERVAL 12
#define DUT_TEMP_TOLERANCE_MAX_INTERVAL 72
#define DUT_TEMP_TOLERANCE_CHANGE       1

#define DUT_ILLUMINANCE_VALUE_MIN_INTERVAL 15
#define DUT_ILLUMINANCE_VALUE_MAX_INTERVAL 80
#define DUT_ILLUMINANCE_VALUE_CHANGE       1

#define DUT_REL_HUMIDITY_VALUE_MIN_INTERVAL 20
#define DUT_REL_HUMIDITY_VALUE_MAX_INTERVAL 90
#define DUT_REL_HUMIDITY_VALUE_CHANGE       1


#endif /* __FB_PRE_TC_08_DUT_REPORTING_CONFIG_ */
