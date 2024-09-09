#ifndef  __MAIN_H
#define  __MAIN_H

#include <openthread/thread.h>
#include <openthread/thread_ftd.h>
#include <openthread/icmp6.h>
#include <openthread/cli.h>
#include <openthread/ncp.h>
#include <openthread/coap.h>
#include <openthread_port.h>

#include "cpc_system_common.h"

#define THREAD_CHANNEL      11
#define THREAD_PANID        0x6767
#define THREAD_UDP_PORT     5678
#define THREAD_COAP_PORT    (THREAD_UDP_PORT + 2)

#define DEMO_UDP            1
#define DEMO_COAP           2

void cpc_upgrade_init(void);
void cpc_uart_init(void);
void cpc_subg_cmd_init(void);
void cpc_system_reset(cpc_system_reboot_mode_t reboot_mode);

uint8_t app_sockInit(otInstance *instance, void (*handler)(otMessage *, const otMessageInfo *), uint16_t udp_port);
uint8_t app_udpSend(otInstance *instance, otIp6Address *addr, uint8_t *p, uint16_t len, uint16_t port);
void cp_sub_cmd_send(uint8_t *pdata, uint16_t len);
void at_cmd_proc(uint8_t *pData, uint16_t len);
void app_task (void) ;
#endif