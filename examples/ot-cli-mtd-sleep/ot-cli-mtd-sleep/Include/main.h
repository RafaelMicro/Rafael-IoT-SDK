#ifndef  __MAIN_H
#define  __MAIN_H

#include <openthread/thread.h>
#include <openthread/thread_ftd.h>
#include <openthread/icmp6.h>
#include <openthread/cli.h>
#include <openthread/ncp.h>
#include <openthread/coap.h>
#include <openthread_port.h>

#define THREAD_CHANNEL      20
#define THREAD_PANID        0x3107
#define THREAD_UDP_PORT     5678
#define THREAD_COAP_PORT    (THREAD_UDP_PORT + 2)

#define DEMO_UDP            1
#define DEMO_COAP           2

uint8_t app_sockInit(otInstance *instance, void (*handler)(otMessage *, const otMessageInfo *), uint16_t udp_port);
void app_udpSend(otInstance *instance, uint8_t *p, uint16_t len);
void app_task (void) ;
#endif // __DEMO_GPIO_H