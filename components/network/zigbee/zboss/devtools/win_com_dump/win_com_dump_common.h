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
/* PURPOSE: Linux-specific win_com_dump header.
*/
#ifndef WIN_COM_DUMP_COMMON_H
#define WIN_COM_DUMP_COMMON_H 1

#ifndef ZB_PLATFORM_LINUX_PC32
#include <process.h>
#include <windows.h>
#else
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "lin_com_dump.h"
#endif

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include "dump_common.h"

#ifndef ZB_PLATFORM_LINUX_PC32
typedef HANDLE serial_handle_t;
#else
typedef int serial_handle_t;
#endif

typedef struct win_com_dump_ctx_s
{
  serial_handle_t comf;
  dump_mode_t g_mode;
  FILE *fraw;
  FILE *tracef;
  FILE *dumpf;
  FILE *scriptf;
#ifdef DEBUG_RESYNC
  FILE *fdump;
#endif
}
win_com_dump_ctx_t;

int port_read(serial_handle_t comf, void *buf, int n, FILE *outf);

#endif /* WIN_COM_DUMP_COMMON_H */
