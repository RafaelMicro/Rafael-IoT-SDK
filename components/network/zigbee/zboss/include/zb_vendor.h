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
/* PURPOSE:
*/
#ifndef ZB_VENDOR_RAFAEL_H
#define ZB_VENDOR_RAFAEL_H 1

#include <FreeRTOS.h>
#include <task.h>

#define A015  0U
#define A047  2U
#define A048  1U

#ifndef BOARD
//#define BOARD   A047
#define BOARD   A048
#endif
/* Number of buttons supported */
#if (BOARD==A047)
#define ZB_N_BUTTONS 4
#else
#define ZB_N_BUTTONS 5
#endif

#ifndef ZB_MAC_TESTING_MODE
#define ZB_ENABLE_HA
#define ZB_ENABLE_ZCL
//#define ZB_ENABLE_SE
#define ZB_ENABLE_ZGP
#endif

/* Enable Rejoin Backoff (used in Smart Plug and IAS Zone samples). */
#define ZB_REJOIN_BACKOFF

#ifdef ZB_USE_SLEEP
//JJ mark the definition ZB_MEMORY_COMPACT for end device to support more binding and group list
// #define ZB_MEMORY_COMPACT
#endif

/* ZB3.0 BDB mode */
#define ZB_BDB_MODE
#define ZB_BDB_ENABLE_FINDING_BINDING
#define ZB_DISTRIBUTED_SECURITY_ON
#define ZB_SECURITY_INSTALLCODES

#ifndef ZB_ED_ROLE
#define ZB_IOBUF_POOL_SIZE  40
#define ZB_SCHEDULER_Q_SIZE 40
#else
#define ZB_IOBUF_POOL_SIZE 30
#define ZB_SCHEDULER_Q_SIZE 30
#endif

/* ZGP */

#ifdef  ZB_ENABLE_ZGP
#define ZB_ENABLE_ZGP_GPCB
#define ZB_ZGP_TRANSL_CMD_PLD_MAX_SIZE 3
#define ZGP_CLUSTER_TEST
#define ZB_ZGP_SKIP_GPDF_ON_NWK_LAYER
#define ZB_ZGP_RUNTIME_WORK_MODE_WITH_PROXIES
#define ZB_ZGP_SINK_TBL_SIZE 10
#define ZB_ZGP_PROXY_TBL_SIZE 4
#endif

/* Device support */
#define ZB_ALL_DEVICE_SUPPORT

/* OOM detection */
//JJ
#define ZB_CHECK_OOM_STATUS
#define ZB_SEND_OOM_STATUS

#define ZB_NO_NVRAM_VER_MIGRATION

#endif /* ZB_VENDOR_RAFAEL_H */
