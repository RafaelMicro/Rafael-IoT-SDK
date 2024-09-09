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
/*  PURPOSE:
*/

#ifndef ZB_MULTI_TEST_MEM_CONF_H
#define ZB_MULTI_TEST_MEM_CONF_H 1

#ifdef ZB_CONFIGURABLE_MEM

#define ZB_MEM_CONFIG_DEFAULT 0
#define ZB_MEM_CONFIG_MIN 1
#define ZB_MEM_CONFIG_MED 2
#define ZB_MEM_CONFIG_MAX 4

#define ZB_MEM_CONFIG ZB_MEM_CONFIG_MAX

#if (ZB_MEM_CONFIG & ZB_MEM_CONFIG_MIN)
#include "zb_mem_config_min.h"
#elif (ZB_MEM_CONFIG & ZB_MEM_CONFIG_MED)
#include "zb_mem_config_med.h"
#elif (ZB_MEM_CONFIG & ZB_MEM_CONFIG_MAX)
#include "zb_mem_config_max.h"
#elif (ZB_MEM_CONFIG == ZB_MEM_CONFIG_DEFAULT)
/* there are no *.h files for the default mem-config */
#endif

#endif /* ZB_CONFIGURABLE_MEM */

#endif /* ZB_MULTI_TEST_MEM_CONF_H */
