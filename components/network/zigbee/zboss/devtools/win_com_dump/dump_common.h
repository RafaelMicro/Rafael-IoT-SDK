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
/* PURPOSE: Console Windows utility which reads serial port and writes trace and
traffic dump to files.
*/
#ifndef DUMP_COMMON_H
#define DUMP_COMMON_H 1

#define BAUD_RATE CBR_115200
#define BAUD_RATE_ITM 4000000u
#define NORDIC_BAUD_RATE 1000000
//#define XAP_BAUD_RATE CBR_38400
#define XAP_BAUD_RATE CBR_115200

/*  Changed program so it would be able to accept 2 parameters: one for compiler and one for working mode
    Needed for getting trace from cc2530: IAR51 and JTAGLOGSBIN
*/
typedef enum dump_mode_s
{
  MODE_KEIL             = 1,
  MODE_IAR51            = (1 << 1),
  MODE_IARARM           = (1 << 2),
  MODE_SDKLOGS          = (1 << 3),
  MODE_JTAGLOGS         = (1 << 4),
  MODE_JTAGLOGSBIN      = (1 << 5),
  MODE_RAWSERIAL        = (1 << 6),
  MODE_USARTLOGSBIN     = (1 << 7),
  MODE_USARTLOGSBIN_XAP = (1 << 8),
  MODE_USARTLOGSBIN_SDK = (1 << 9),
  MODE_ARM_BINFILE      = (1 << 10),
  MODE_SIFLOGSBIN_XAP   = (1 << 11),
  MODE_CONSOLE_SCRIPT   = (1 << 12),
  MODE_NORDIC_LOG       = (1 << 13),
  MODE_NORDIC_LOG_115200= (1 << 14),
  MODE_STANDARD_INPUT   = (1 << 15),
  MODE_HEX              = (1 << 16),
  MODE_ITM              = (1 << 17)
} dump_mode_t;

#define IS_USING_SERIAL(mode) (mode & (MODE_RAWSERIAL | MODE_USARTLOGSBIN_XAP | MODE_USARTLOGSBIN | MODE_CONSOLE_SCRIPT | MODE_ITM))
#define IS_BE(mode) (mode & MODE_KEIL)
#define IS_IARARM(mode) \
    (mode & (MODE_IARARM | MODE_SDKLOGS | MODE_JTAGLOGS | MODE_JTAGLOGSBIN | MODE_USARTLOGSBIN | MODE_ARM_BINFILE | MODE_USARTLOGSBIN_SDK) && !(mode & (MODE_KEIL | MODE_IAR51)) && !IS_XAP(mode))
#define IS_XAP(mode) (mode & (MODE_USARTLOGSBIN_XAP | MODE_SIFLOGSBIN_XAP))
#define IS_BINARY(mode)(mode & (MODE_JTAGLOGSBIN | MODE_USARTLOGSBIN | MODE_USARTLOGSBIN_XAP | MODE_ARM_BINFILE | MODE_USARTLOGSBIN_SDK | MODE_SIFLOGSBIN_XAP | MODE_ITM))
#define IS_SCRIPTING(mode) (mode & MODE_CONSOLE_SCRIPT)
#define TWO_BYTES_H(mode) (IS_XAP(mode) || (mode & MODE_IAR51))

extern dump_mode_t g_mode;

void parse_trace_line(char *str, int line_len, FILE *outf);


#endif /* DUMP_COMMON_H */
