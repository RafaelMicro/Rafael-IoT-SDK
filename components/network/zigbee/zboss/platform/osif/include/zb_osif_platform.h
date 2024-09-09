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
/* PURPOSE: Header to combine osif dependent files with stack
*/

#ifndef ZB_PLATFORM_OSIF_H
#define ZB_PLATFORM_OSIF_H 1

#ifndef DOXYGEN

#define MAIN() int main(int argc, char *argv[])
#define FAKE_ARGV
#define ARGV_UNUSED ZVUNUSED(argc) ; ZVUNUSED(argv)
#define MAIN_RETURN(v) return (v)

#endif /* ndef DOXYGEN */

#ifdef RT570
#include "zb_osif_rt570.h"
#include "uart_drv.h"
#elif defined RT569
#include "zb_osif_rt569.h"
#else
#include "zigbee_platform.h"
#endif

#endif /* ZB_PLATFORM_OSIF_H */
