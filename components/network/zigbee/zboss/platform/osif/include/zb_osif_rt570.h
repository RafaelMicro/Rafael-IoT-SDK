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
/* PURPOSE: Platform specific
*/


#include <string.h>             /* memcpy */
#include <stdio.h>
#include "mcu.h"


#include "zb_types.h"
#include "zb_stm_serial.h"

#ifdef ZB_HAVE_SERIAL
#define ZB_HAVE_IOCTX
#endif

#define ZB_VOLATILE
#define ZB_SDCC_XDATA
#define ZB_CALLBACK
#define ZB_SDCC_BANKED
#define ZB_KEIL_REENTRANT

/* At ARM all types from 1 to 4 bytes are passed to vararg with casting to 4 bytes */
typedef zb_uint32_t zb_minimal_vararg_t;

/* use macros to be able to redefine */
#define ZB_MEMCPY   memcpy
#define ZB_MEMMOVE  memmove
#define ZB_MEMSET   memset
#define ZB_MEMCMP   memcmp

#define ZB_BZERO(s,l) ZB_MEMSET((char*)(s), 0, (l))
#define ZB_BZERO2(s) ZB_BZERO(s, 2)


#ifdef ZB_IAR
#define NULL _NULL
#else
#ifndef NULL
#define NULL ((void*)0)
#endif
#endif

#define ZVUNUSED(v) (void)v


void random_init(unsigned short seed);
uint32_t random_rand(void);

#define ZB_RANDOM_INIT() random_init(0)
#define ZB_RANDOM_RAND() random_rand()

#define ZB_PLATFORM_INIT() zb_rt570_init()

//extern uart_handle_t  m_uart_handle[MAX_NUMBER_OF_UART];
//void rt_uart_transmit_chars(uint32_t id);
//void rt_uart_receive_chars(uint32_t id);

uint32_t timer_status(uint32_t timer_id);
void timer_set(void);
void zb_rt570_init(void);
void zb_disable_int(void);
void zb_enable_int(void);
void zb_stop_timer(void);
void zb_start_timer(void);
zb_uint32_t zb_timer_status(void);

#if defined ZB_TRACE_OVER_USART && defined ZB_TRACE_LEVEL
#define ZB_SERIAL_FOR_TRACE
#endif

void SysTickIntDisable(void);
void SysTickIntEnable(void);
uint32_t SysTickValueGet(void);

#define ZB_CHECK_TIMER_IS_ON()  zb_timer_status()

#define ZB_START_HW_TIMER()   zb_start_timer()
#define ZB_STOP_HW_TIMER()    zb_stop_timer()

#define ZB_TRANSPORT_NONBLOCK_ITERATION() 0
#define ZB_ENABLE_ALL_INTER()  zb_enable_int() //leave_critical_section() //__enable_irq() //CPUcpsie()
#define ZB_DISABLE_ALL_INTER() zb_disable_int()//enter_critical_section() //__disable_irq() //CPUcpsid()


#define ZB_OSIF_GLOBAL_LOCK()         ZB_DISABLE_ALL_INTER()
#define ZB_OSIF_GLOBAL_UNLOCK()       ZB_ENABLE_ALL_INTER()

#define ZB_ABORT 
//#define ZB_GO_IDLE() CPUwfi()
/* Have problems with sleep when working without a debugger. Switch it iff temporary */
#define ZB_GO_IDLE()

#if (BOARD==A047)
#define LED0         18
#define LED1         19
#define LED2         20
#define LED3         21

#define KEY0         14
#define KEY1         15
#define KEY2         30
#define KEY3         31

#define DBG0         10
#define DBG1         11
#define DBG2         12
#define DBG3         13

#define DBGON        2

#elif  (BOARD==A048)
#define LED0         20
#define LED1         21
#define LED2         22
#define LED3         23

#define KEY0         0
#define KEY1         1
#define KEY2         2
#define KEY3         3
#else

#define LED0         8
#define LED1         9
#define LED2         10
#define LED3         11
#define LED4         12
#define LED5         13
#define LED6         14
#define LED7         15

#define KEY0         0
#define KEY1         1
#define KEY2         2
#define KEY3         3
#define KEY4         4
#define KEY5         5
#define KEY6         6
#define KEY7         7

#endif


