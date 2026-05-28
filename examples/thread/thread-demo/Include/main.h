/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

#ifndef __MAIN_H
#define __MAIN_H

#include <openthread/cli.h>
#include <openthread/coap.h>
#include <openthread/icmp6.h>
#include <openthread/ncp.h>
#include <openthread/thread.h>
#include <openthread/thread_ftd.h>
#include <openthread_port.h>

uint8_t app_sockInit(otInstance* instance,
                     void (*handler)(otMessage*, const otMessageInfo*),
                     uint16_t udp_port);
void app_udpSend(otInstance* instance, uint8_t* p, uint16_t len);
void app_task(void);
#endif // __DEMO_GPIO_H