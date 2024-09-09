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

#ifndef ZB_TRACE_SIGNAL_PARAMS_H
#define ZB_TRACE_SIGNAL_PARAMS_H 1

#include "zb_common.h"

void zb_trace_signal_params(zb_zdo_app_signal_type_t sig_type, zb_zdo_app_signal_hdr_t *sg_p);

#endif /* ZB_TRACE_SIGNAL_PARAMS_H */
