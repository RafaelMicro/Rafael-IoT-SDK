#include <FreeRTOS.h>
#include <miu_port.h>
#include <openthread-core-config.h>
#include <openthread/config.h>
#include <openthread/thread.h>
#include <openthread/thread_ftd.h>
#include <string.h>
#include <task.h>
#include <timers.h>

#include "EnhancedFlashDataset.h"
#include "app_control_cmd.h"
#include "app_led.h"
#include "app_net_mgm.h"
#include "app_task.h"
#include "hosal_gpio.h"
#include "log.h"
#include "miu_ext_mem.h"

#if CONFIG_APP_TASK_CENTRAL_ENABLE
#define NET_MGM_NODE_ENROLL_REQ_TIMEOUT 20

static TimerHandle_t sNetSurvivalTimer = NULL;

bool enroll_done = false;

uint8_t enroll_send_try = 0;
uint8_t enroll_send_time = 0;
uint16_t inactivity_timer = 0;

void net_mgm_enroll_req_received_done(int status, uint16_t comission_time) {
    if (status < 0) {
        otInstanceFactoryReset(otrGetInstance());
    } else {
        enroll_done = true;
        enroll_send_time = 0;
    }
}

int net_mgm_enroll_req_send(otInstance* instance) {
    otExtAddress aExtAddress;
    otRouterInfo parentInfo;
    otNeighborInfo neighborInfo;
    otNeighborInfoIterator iterator = OT_NEIGHBOR_INFO_ITERATOR_INIT;
    uint32_t role = otThreadGetDeviceRole(instance);
    int8_t rssi = -127;
    if (role == OT_DEVICE_ROLE_ROUTER || role == OT_DEVICE_ROLE_CHILD) {
        if (otThreadGetParentInfo(instance, &parentInfo) != OT_ERROR_NONE) {
            log_info("get parent fail ");
        }

        while (otThreadGetNextNeighborInfo(instance, &iterator, &neighborInfo)
               == OT_ERROR_NONE) {
            if (neighborInfo.mRloc16 == parentInfo.mRloc16) {
                rssi = neighborInfo.mLastRssi;
                break;
            }
        }
        // Send enrollment request
        net_mgm_enroll_req_t enroll_req;
        enroll_req.role = role;
        enroll_req.parent = parentInfo.mRloc16;
        enroll_req.self_rloc = otThreadGetRloc16(instance);
        aExtAddress = *otLinkGetExtendedAddress(instance);
        memcpy(enroll_req.self_extaddr, aExtAddress.m8, OT_EXT_ADDRESS_SIZE);
        enroll_req.rssi = rssi;
        const char* verStr = otGetVersionString();
        const char* dash = strchr(verStr, '-');
        if (dash && strlen(dash) >= 9) {
            char timeHex[9] = {0};
            strncpy(timeHex, dash + 1, 8);
            enroll_req.version = (uint32_t)strtoul(timeHex, NULL, 16);
        }

        // Send the enrollment request
        /*setting leader ip*/
        otIp6Address leader_addr = *otThreadGetRloc(instance);
        leader_addr.mFields.m8[14] = 0xFC;
        leader_addr.mFields.m8[15] = 0x00;
        if (app_ctrl_send_cmd((uint8_t*)&leader_addr, CMD_ID_NETWORK_ENROLL_REQ,
                              FLAG_UDP, (uint8_t*)&enroll_req,
                              sizeof(net_mgm_enroll_req_t), 0, 0)) {
            log_info("app_ctrl_send_cmd fail");
        }
        log_info("Enroll Req sent, role=%d, parent=%x, rloc=%x, "
                 "extaddr=%02x%02x%02x%02x%02x%02x%02x%02x, rssi=%d, ver=%d",
                 enroll_req.role, enroll_req.parent, enroll_req.self_rloc,
                 enroll_req.self_extaddr[0], enroll_req.self_extaddr[1],
                 enroll_req.self_extaddr[2], enroll_req.self_extaddr[3],
                 enroll_req.self_extaddr[4], enroll_req.self_extaddr[5],
                 enroll_req.self_extaddr[6], enroll_req.self_extaddr[7],
                 enroll_req.rssi, enroll_req.version);
    }
    return 0;
}

void net_mgm_survival_timeout_callback(TimerHandle_t xTimer) {

    if (otThreadGetDeviceRole(otrGetInstance()) <= OT_DEVICE_ROLE_DETACHED) {
        /*debug use*/
        app_set_led0_toggle();
        inactivity_timer++;
        if (inactivity_timer >= 240) {
            otInstanceFactoryReset(otrGetInstance());
        }
    } else {
        if (inactivity_timer != 0) {
            inactivity_timer = 0;
        }
        if (enroll_done == false) {
            if (enroll_send_time == 0 || enroll_send_time > 20) {
                if (enroll_send_try < 5) {
                    enroll_send_try++;
                    app_evt_single(NULL, APP_EVENT_NET_MGM_ENROLL_REQ_SEND);
                } else {
                    enroll_send_try = 0;
                    app_evt_single(NULL,
                                   APP_EVENT_NET_MGM_ENROLL_REQ_SEND_TRYOUT);
                }
                enroll_send_time = 1;
            } else {
                enroll_send_time++;
            }
            app_set_led0_flash();
        } else {
            app_set_led0_off();
            app_set_led1_toggle();
        }
    }
}

void net_mgm_init(otInstance* instance) {
    if (sNetSurvivalTimer == NULL) {
        sNetSurvivalTimer = xTimerCreate(
            "NetSurvivalTimer", pdMS_TO_TICKS(1000), pdTRUE, NULL,
            (TimerCallbackFunction_t)net_mgm_survival_timeout_callback);
    }
    xTimerStart(sNetSurvivalTimer, 0);
}

#endif