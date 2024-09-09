#ifndef  __MAIN_H
#define  __MAIN_H

#include <openthread/thread.h>
#include <openthread/thread_ftd.h>
#include <openthread/icmp6.h>
#include <openthread/cli.h>
#include <openthread/ncp.h>
#include <openthread/coap.h>
#include <openthread_port.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include <time.h>

#if OPENTHREAD_CONFIG_RADIO_2P4GHZ_OQPSK_SUPPORT
#define THREAD_CHANNEL      11
#else
#define THREAD_CHANNEL      1
#endif
#define THREAD_PANID        0xff01
#define THREAD_UDP_PORT     5888
#define THREAD_COAP_PORT    (THREAD_UDP_PORT + 2)

typedef enum
{
    EVENT_NONE                       = 0,

    EVENT_UART1_UART_IN                    = 0x00000002,
    EVENT_UART2_UART_IN                    = 0x00000004,
    EVENT_UART1_UART_OUT_DONE              = 0x00000008,
    EVENT_UART2_UART_OUT_DONE              = 0x00000010,

    EVENT_UDP_RECEIVED                     = 0x00000020,

    EVENT_ALL                              = 0xffffffff,
} app_task_event_t;

extern app_task_event_t g_app_task_evt_var;

void __app_task_signal(void);

#define APP_EVENT_NOTIFY_ISR(ebit)                 (g_app_task_evt_var |= ebit); __app_task_signal()
#define APP_EVENT_NOTIFY(ebit)                     enter_critical_section(); g_app_task_evt_var |= ebit; leave_critical_section(); __app_task_signal()
#define APP_EVENT_GET_NOTIFY(ebit)                 enter_critical_section(); ebit = g_app_task_evt_var; g_app_task_evt_var = EVENT_NONE; ; leave_critical_section()

void app_task (void) ;
int8_t app_get_parent_rssi();

/*network_management*/
otError nwk_mgm_init(otInstance *aInstance);
void nwk_mgm_neighbor_Change_Callback(otNeighborTableEvent aEvent, const otNeighborTableEntryInfo *aEntryInfo);
void nwk_mgm_child_register_post();

/*app uart*/
void app_uart_init(void);
int app_uart_data_send(uint8_t u_port, uint8_t *p_data, uint16_t data_len);
void __uart_task(app_task_event_t sevent);

/*app udp*/
uint8_t app_sockInit(otInstance *instance, void (*handler)(otMessage *, const otMessageInfo *), uint16_t udp_port);
otError app_udpSend(uint16_t PeerPort, otIp6Address PeerAddr, uint8_t *data, uint16_t data_lens);
void app_udp_received_queue_push(uint8_t *data, uint16_t data_lens);
void __udp_task(app_task_event_t sevent);

#endif // __DEMO_GPIO_H
