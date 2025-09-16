#ifndef __MAIN_H
#define __MAIN_H

#include <miu_port.h>
#include <openthread/cli.h>
#include <openthread/coap.h>
#include <openthread/icmp6.h>
#include <openthread/ncp.h>
#include <openthread/thread.h>
#include <openthread/thread_ftd.h>

/*app_task.c*/
void app_task(void);

/*app_led.c*/
void app_set_led0_on(void);
void app_set_led0_off(void);
void app_set_led0_toggle(void);
void app_set_led0_flash(void);

void app_set_led1_on(void);
void app_set_led1_off(void);
void app_set_led1_toggle(void);
void app_led_pin_init(void);

/*app_udp.c*/
uint8_t app_sockInit(otInstance* instance,
                     void (*handler)(otMessage*, const otMessageInfo*),
                     uint16_t udp_port);

void app_udpSend(otIp6Address dstaddr, uint8_t* p, uint16_t len);

#endif // __DEMO_GPIO_H