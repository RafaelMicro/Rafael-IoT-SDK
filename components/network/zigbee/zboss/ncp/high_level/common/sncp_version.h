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
/*  PURPOSE: SNCP version header
*/
#ifndef SNCP_VERSION_H
#define SNCP_VERSION_H 1

#include "zb_version.h"

/* TODO: change to ZBOSS_MINOR in future */
#define SNCP_ZBOSS_MINOR 2u
/* ZBOSS version */
#define SNCP_STACK_VERSION ((ZBOSS_MAJOR << 24u) | (SNCP_ZBOSS_MINOR << 16u))

/* SDK version */
#define SNCP_SDK_TYPE 1u
#define SNCP_SDK_VERSION 18u
#define SNCP_FW_VERSION ((SNCP_STACK_VERSION) | (SNCP_SDK_TYPE << 8u) | (SNCP_SDK_VERSION))

/* Serial Protocol document version */
#define SNCP_PROTOCOL_MAJOR 1u
#define SNCP_PROTOCOL_MINOR 19u
#define SNCP_PROTOCOL_VERSION ((SNCP_PROTOCOL_MAJOR << 8u) | (SNCP_PROTOCOL_MINOR))

/* redefine common NCP versions with SNCP versions */
#ifdef NCP_STACK_VERSION
#undef NCP_STACK_VERSION
#endif
#ifdef NCP_FW_VERSION
#undef NCP_FW_VERSION
#endif
#ifdef NCP_PROTOCOL_VERSION
#undef NCP_PROTOCOL_VERSION
#endif

#define NCP_STACK_VERSION    SNCP_STACK_VERSION
#define NCP_FW_VERSION       SNCP_FW_VERSION
#define NCP_PROTOCOL_VERSION SNCP_PROTOCOL_VERSION

#endif /* SNCP_VERSION_H */
