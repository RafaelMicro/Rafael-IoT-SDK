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
/* PURPOSE: SGW macsplit device header
*/

#ifndef MACSPLIT_DEVICE_H
#define MACSPLIT_DEVICE_H 1

#include "zb_config.h"

#if ! defined ZBOSS_MAJOR
#warning ZBOSS_MAJOR is undefined. Default value is used
#define ZBOSS_MAJOR 3
#endif /* ! defined ZBOSS_MAJOR */

#if ! defined ZBOSS_MINOR
#warning ZBOSS_MINOR is nudefined. Default value is used
#define ZBOSS_MINOR 4
#endif /* ! defined ZBOSS_MINOR */

#if ! defined MACSPLIT_APP_REVISION
#warning "MACSPLIT_APP_REVISION isn't defined, default value is used"
#define MACSPLIT_APP_REVISION 1
#endif /* ! defined MACSPLIT_APP_REVISION */

#define MACSPLIT_APP_VERSION (ZBOSS_MAJOR << 24 | ZBOSS_MINOR << 16 | MACSPLIT_APP_REVISION)

#endif /* MACSPLIT_DEVICE_H */
