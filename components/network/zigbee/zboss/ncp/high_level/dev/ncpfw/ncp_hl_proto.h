/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * http://www.dsr-zboss.com
 * http://www.dsr-corporation.com
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
/*  PURPOSE: NCP High level protocol definitions
*/
#ifndef NCP_HL_PROTO_H
#define NCP_HL_PROTO_H 1

#include "ncp_hl_proto_common.h"

/* indication header == ncp_hl_header_t */

typedef enum ncp_hl_reset_opt_e
{
  NO_OPTIONS = 0,
  NVRAM_ERASE,
  FACTORY_RESET
}
ncp_hl_reset_opt_t;

#endif /* NCP_HL_PROTO_H */
