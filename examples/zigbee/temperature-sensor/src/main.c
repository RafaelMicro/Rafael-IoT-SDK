#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zigbee_platform.h"
#include "zigbee_zcl_msg_handler.h"
#include "zigbee_api.h"
#include "device_api.h"

#include "hosal_rf.h"
#include "hosal_uart.h"

#include "FreeRTOS.h"
#include "task.h"

#include "log.h"
#include "app_hooks.h"
#include "uart_stdio.h"


#define PHY_PIB_TURNAROUND_TIMER    192
#define PHY_PIB_CCA_DETECTED_TIME   128 // 8 symbols
#define PHY_PIB_CCA_DETECT_MODE     0
#define PHY_PIB_CCA_THRESHOLD       75
#define MAC_PIB_UNIT_BACKOFF_PERIOD 320
#define MAC_PIB_MAC_ACK_WAIT_DURATION                                          \
    544 // non-beacon mode; 864 for beacon mode
#define MAC_PIB_MAC_MAX_BE                    5
#define MAC_PIB_MAC_MAX_FRAME_TOTAL_WAIT_TIME 16416
#define MAC_PIB_MAC_MAX_FRAME_RETRIES         4
#define MAC_PIB_MAC_MAX_CSMACA_BACKOFFS       5
#define MAC_PIB_MAC_MIN_BE                    2

static uint8_t g_joined_network = 0;
static uint16_t g_panid = 0xFFFF;
static uint16_t g_short_addr = 0xFFFF;
static uint8_t g_joined_channel = 0xFF;

int main(void) {
    uart_stdio_init();
    vHeapRegionsInt();
    log_info("%s version : %s", CONFIG_BUILD_PORJECT, CONFIG_PROJECT_VERSION);

    hosal_rf_init(HOSAL_RF_MODE_RUCI_CMD);
    zigbee_app_init();
    zbStart();
    vTaskStartScheduler();
    while(1) {;}
}

void app_main_loop(void* parameters_ptr) {
    zb_app_event_t sevent = ZB_APP_EVENT_NONE;

    ZB_THREAD_SAFE(
        ZB_AF_REGISTER_DEVICE_CTX(&simple_desc_temperature_sensor_ctx);
        for (int i = 0; i < simple_desc_temperature_sensor_ctx.ep_count; i++) {
            ZB_AF_SET_ENDPOINT_HANDLER(
                simple_desc_temperature_sensor_ctx.ep_desc_list[i]->ep_id,
                zigbee_zcl_msg_handler);
        }
    )
    ZIGBEE_APP_NOTIFY(ZB_APP_EVENT_INIT);

    for (;;) {
        if (ulTaskNotifyTake(pdFALSE, portMAX_DELAY) != 0) {
            ZIGBEE_APP_GET_NOTIFY(sevent);

            switch (sevent) {
                case ZB_APP_EVENT_INIT: {
                    zigbee_app_nwk_start(ZIGBEE_CHANNEL_ALL_MASK(), 32, 0);
                    log_info("ZigBee APP init");
                    start_sensor_timer();
                    set_led_onoff(LED_BLUE,1);
                } break;

                case ZB_APP_EVENT_NOT_JOINED: {

                    ZB_THREAD_SAFE(bdb_start_top_level_commissioning(
                        ZB_BDB_NETWORK_STEERING));
                    log_info("ZigBee APP not joined");
                } break;

                case ZB_APP_EVENT_JOINED: {
                    log_info("ZigBee APP joined");
                    g_joined_network = 1;
                    set_led_onoff(LED_BLUE,0);

                    ZB_THREAD_SAFE(g_panid = zb_get_pan_id();
                                   g_short_addr = zb_get_short_address();
                                   g_joined_channel = zb_get_current_channel();)

                    log_info("PAN ID: %04X, Short Addr: %04X, Channel: %d",
                             g_panid, g_short_addr, g_joined_channel);

                } break;

                case ZB_APP_EVENT_FACTORY_RESET: {
                    zigbee_do_factory_reset();
                } break;
                default: break;
            }
        }
    }
}