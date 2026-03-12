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

#define NET_MGM_NODE_MLE_MAX_ROUTERS OPENTHREAD_CONFIG_MLE_MAX_ROUTERS
#define NET_MGM_NODE_MLE_MAX_CHILD   300
#define NET_MGM_NODE_TABLE_MAX_SIZE                                            \
    (NET_MGM_NODE_MLE_MAX_ROUTERS + NET_MGM_NODE_MLE_MAX_CHILD)
#define NET_MGM_NODE_ENROLL_REQ_TIMEOUT 20

static net_mgm_node_table_t* net_mgm_node_table = NULL;

static TimerHandle_t sNetSurvivalTimer = NULL;

bool enroll_done = false;
static uint16_t leader_child_enroll_update_time = 0;

uint16_t enroll_update_timeout = 0;
uint8_t enroll_send_try = 0;
uint8_t enroll_send_time = 0;

bool net_mgm_check_leader_pin_state() {
    uint32_t pin_value;
    hosal_gpio_pin_get(22, &pin_value);
    return (pin_value == 0);
}

int net_mgm_node_table_add(net_mgm_node_table_t* net_mgm_node_info) {
    uint16_t i;
    uint16_t freeRouterIdx = NET_MGM_NODE_MLE_MAX_ROUTERS;
    uint16_t freeChildIdx = NET_MGM_NODE_TABLE_MAX_SIZE;

    if (net_mgm_node_table == NULL) {
        log_info("net_mgm_node_table is NULL ");
        return -1;
    }

    // Check if the entry already exists
    for (i = 0; i < NET_MGM_NODE_TABLE_MAX_SIZE; i++) {
        if (net_mgm_node_table[i].used
            && memcmp(net_mgm_node_table[i].extaddr, net_mgm_node_info->extaddr,
                      OT_EXT_ADDRESS_SIZE)
                   == 0) {
            bool isRouterZone = (i < NET_MGM_NODE_MLE_MAX_ROUTERS);
            bool shouldBeRouter = (net_mgm_node_info->role
                                   == OT_DEVICE_ROLE_ROUTER);

            if ((isRouterZone && !shouldBeRouter)
                || (!isRouterZone && shouldBeRouter)) {
                log_info("Node role changed (from %s at %d to %s), re-adding.",
                         otThreadDeviceRoleToString(net_mgm_node_table[i].role),
                         i,
                         otThreadDeviceRoleToString(net_mgm_node_info->role));
                net_mgm_node_table[i].used = 0;
            } else {
                enter_critical_section();
                net_mgm_node_table[i].parent = net_mgm_node_info->parent;
                net_mgm_node_table[i].role = net_mgm_node_info->role;
                net_mgm_node_table[i].rloc = net_mgm_node_info->rloc;
                net_mgm_node_table[i].rssi = net_mgm_node_info->rssi;

                if (net_mgm_node_info->version != 0xFFFFFFFF) {
                    net_mgm_node_table[i].version = net_mgm_node_info->version;
                }
                net_mgm_node_table[i].survivaltime = NET_MGM_NODE_SURVIVAL_TIME;
                leave_critical_section();
                log_info("Update %02X%02X%02X%02X%02X%02X%02X%02X ",
                         net_mgm_node_table[i].extaddr[0],
                         net_mgm_node_table[i].extaddr[1],
                         net_mgm_node_table[i].extaddr[2],
                         net_mgm_node_table[i].extaddr[3],
                         net_mgm_node_table[i].extaddr[4],
                         net_mgm_node_table[i].extaddr[5],
                         net_mgm_node_table[i].extaddr[6],
                         net_mgm_node_table[i].extaddr[7]);
                return 0;
            }
        }
    }

    // Search for an available slot
    for (i = 0; i < NET_MGM_NODE_TABLE_MAX_SIZE; i++) {
        if (net_mgm_node_table[i].used == 0) {
            if (i < NET_MGM_NODE_MLE_MAX_ROUTERS) {
                if (freeRouterIdx == NET_MGM_NODE_MLE_MAX_ROUTERS) {
                    freeRouterIdx = i; //Router slot is found
                }
            } else {
                if (freeChildIdx == NET_MGM_NODE_TABLE_MAX_SIZE) {
                    freeChildIdx = i; // Record the first available child slot
                }
            }
        }
    }

    // Determine the insertion index
    uint16_t insertIdx = (net_mgm_node_info->role == OT_DEVICE_ROLE_ROUTER)
                             ? freeRouterIdx
                             : freeChildIdx;
    if (insertIdx == NET_MGM_NODE_TABLE_MAX_SIZE) {
        log_warn("No available slot ");
        return -1; // No available slot
    }
    if (net_mgm_node_table[insertIdx].used) {
        log_warn("No available slot for role %d ", net_mgm_node_info->role);
        return -1; // No available slot
    }
    enter_critical_section();
    net_mgm_node_table[insertIdx].used = 1;
    net_mgm_node_table[insertIdx].parent = net_mgm_node_info->parent;
    net_mgm_node_table[insertIdx].role = net_mgm_node_info->role;
    net_mgm_node_table[insertIdx].rloc = net_mgm_node_info->rloc;
    memcpy(net_mgm_node_table[insertIdx].extaddr, net_mgm_node_info->extaddr,
           OT_EXT_ADDRESS_SIZE);
    net_mgm_node_table[insertIdx].rssi = net_mgm_node_info->rssi;
    net_mgm_node_table[insertIdx].version = net_mgm_node_info->version;
    net_mgm_node_table[insertIdx].survivaltime = NET_MGM_NODE_SURVIVAL_TIME;
    leave_critical_section();
    log_info("add [%d] %s %04X %02X%02X%02X%02X%02X%02X%02X%02X %d %08x ",
             insertIdx,
             otThreadDeviceRoleToString(net_mgm_node_table[insertIdx].role),
             net_mgm_node_table[insertIdx].rloc,
             net_mgm_node_table[insertIdx].extaddr[0],
             net_mgm_node_table[insertIdx].extaddr[1],
             net_mgm_node_table[insertIdx].extaddr[2],
             net_mgm_node_table[insertIdx].extaddr[3],
             net_mgm_node_table[insertIdx].extaddr[4],
             net_mgm_node_table[insertIdx].extaddr[5],
             net_mgm_node_table[insertIdx].extaddr[6],
             net_mgm_node_table[insertIdx].extaddr[7],
             net_mgm_node_table[insertIdx].rssi,
             net_mgm_node_table[insertIdx].version);
    return 0;
}

void net_mgm_node_table_display() {
    uint16_t i = 0, count = 0;
    if (net_mgm_node_table) {
        printf("index role parent rloc extaddr rssi version survivaltime \r\n");
        printf("===============================================\r\n");
        for (i = 0; i < NET_MGM_NODE_TABLE_MAX_SIZE; i++) {
            if (net_mgm_node_table[i].used
                && (net_mgm_node_table[i].role == OT_DEVICE_ROLE_ROUTER)) {
                ++count;
                printf("[%u] %s %04X %04X %02X%02X%02X%02X%02X%02X%02X%02X %d "
                       "0x%08x %u \r\n",
                       count,
                       otThreadDeviceRoleToString(net_mgm_node_table[i].role),
                       net_mgm_node_table[i].parent, net_mgm_node_table[i].rloc,
                       net_mgm_node_table[i].extaddr[0],
                       net_mgm_node_table[i].extaddr[1],
                       net_mgm_node_table[i].extaddr[2],
                       net_mgm_node_table[i].extaddr[3],
                       net_mgm_node_table[i].extaddr[4],
                       net_mgm_node_table[i].extaddr[5],
                       net_mgm_node_table[i].extaddr[6],
                       net_mgm_node_table[i].extaddr[7],
                       net_mgm_node_table[i].rssi,
                       net_mgm_node_table[i].version,
                       net_mgm_node_table[i].survivaltime);
            }
        }
        for (i = 0; i < NET_MGM_NODE_TABLE_MAX_SIZE; i++) {
            if (net_mgm_node_table[i].used) {
                if (net_mgm_node_table[i].role == OT_DEVICE_ROLE_CHILD) {
                    ++count;
                    printf(
                        "[%u] %s %04X %04X %02X%02X%02X%02X%02X%02X%02X%02X "
                        "%d 0x%08x %u \r\n",
                        count,
                        otThreadDeviceRoleToString(net_mgm_node_table[i].role),
                        net_mgm_node_table[i].parent,
                        net_mgm_node_table[i].rloc,
                        net_mgm_node_table[i].extaddr[0],
                        net_mgm_node_table[i].extaddr[1],
                        net_mgm_node_table[i].extaddr[2],
                        net_mgm_node_table[i].extaddr[3],
                        net_mgm_node_table[i].extaddr[4],
                        net_mgm_node_table[i].extaddr[5],
                        net_mgm_node_table[i].extaddr[6],
                        net_mgm_node_table[i].extaddr[7],
                        net_mgm_node_table[i].rssi,
                        net_mgm_node_table[i].version,
                        net_mgm_node_table[i].survivaltime);
                }
            }
        }
        for (i = 0; i < NET_MGM_NODE_TABLE_MAX_SIZE; i++) {
            if (net_mgm_node_table[i].used) {

                if ((net_mgm_node_table[i].role != OT_DEVICE_ROLE_CHILD)
                    && (net_mgm_node_table[i].role != OT_DEVICE_ROLE_ROUTER)) {
                    ++count;
                    printf(
                        "[%u] %s %04X %04X %02X%02X%02X%02X%02X%02X%02X%02X "
                        "%d 0x%08x %u \r\n",
                        count,
                        otThreadDeviceRoleToString(net_mgm_node_table[i].role),
                        net_mgm_node_table[i].parent,
                        net_mgm_node_table[i].rloc,
                        net_mgm_node_table[i].extaddr[0],
                        net_mgm_node_table[i].extaddr[1],
                        net_mgm_node_table[i].extaddr[2],
                        net_mgm_node_table[i].extaddr[3],
                        net_mgm_node_table[i].extaddr[4],
                        net_mgm_node_table[i].extaddr[5],
                        net_mgm_node_table[i].extaddr[6],
                        net_mgm_node_table[i].extaddr[7],
                        net_mgm_node_table[i].rssi,
                        net_mgm_node_table[i].version,
                        net_mgm_node_table[i].survivaltime);
                }
            }
        }
        printf("=============================================== \r\n");
        printf("total num %u \r\n", count);
    } else {
        printf("net_mgm_node_table is NULL \r\n");
    }
}

void net_mgm_node_table_num() {
    uint16_t i = 0, unuse_cnt = 0, router_cnt = 0, child_cnt = 0,
             detached_cnt = 0;
    if (net_mgm_node_table) {
        for (i = 0; i < NET_MGM_NODE_TABLE_MAX_SIZE; i++) {
            if (net_mgm_node_table[i].used) {
                if (net_mgm_node_table[i].role == OT_DEVICE_ROLE_ROUTER) {
                    router_cnt++;
                } else if (net_mgm_node_table[i].role == OT_DEVICE_ROLE_CHILD) {
                    child_cnt++;
                } else if (net_mgm_node_table[i].role
                           == OT_DEVICE_ROLE_DETACHED) {
                    detached_cnt++;
                }
            } else {
                unuse_cnt++;
            }
        }
        printf("total num (router/child/detached)/unused (%u/%u/%u)/%u \r\n",
               router_cnt, child_cnt, detached_cnt, unuse_cnt);
    } else {
        printf("net_mgm_node_table is NULL \r\n");
    }
}

void net_mgm_node_survivaltime_update(uint8_t role, uint16_t parent_rloc,
                                      uint16_t self_rloc, int8_t rssi) {
    uint16_t i = 0;
    if (net_mgm_node_table) {
        for (i = 0; i < NET_MGM_NODE_TABLE_MAX_SIZE; i++) {
            if (net_mgm_node_table[i].used) {
                if (net_mgm_node_table[i].rloc == self_rloc) {
                    net_mgm_node_table[i].role = role;
                    net_mgm_node_table[i].parent = parent_rloc;
                    net_mgm_node_table[i].rssi = rssi;
                    net_mgm_node_table[i].survivaltime =
                        NET_MGM_NODE_SURVIVAL_TIME;
                    // log_info("Update survivaltime %u for "
                    //          "%02X%02X%02X%02X%02X%02X%02X%02X ",
                    //          net_mgm_node_table[i].survivaltime,
                    //          net_mgm_node_table[i].extaddr[0],
                    //          net_mgm_node_table[i].extaddr[1],
                    //          net_mgm_node_table[i].extaddr[2],
                    //          net_mgm_node_table[i].extaddr[3],
                    //          net_mgm_node_table[i].extaddr[4],
                    //          net_mgm_node_table[i].extaddr[5],
                    //          net_mgm_node_table[i].extaddr[6],
                    //          net_mgm_node_table[i].extaddr[7]);
                    break;
                }
            }
        }
        if (i == NET_MGM_NODE_TABLE_MAX_SIZE) {
            // log_info("No such node rloc %04X to update survivaltime",
            //          self_rloc);
        }
    } else {
        log_info("net_mgm_node_table is NULL ");
    }
}

void net_mgm_enroll_req_received_done(int status, uint16_t provision_time) {
    if (status < 0) {
        otInstanceFactoryReset(otrGetInstance());
    } else {
        enroll_done = true;
        if (otThreadGetDeviceRole(otrGetInstance()) == OT_DEVICE_ROLE_ROUTER)
            if (provision_time) {
                app_set_provisioning_mode(true, provision_time);
            } else {
                app_set_provisioning_mode(false, 0);
            }
    }
}

int net_mgm_enroll_req_send(otInstance* instance) {
    otExtAddress aExtAddress;
    otRouterInfo parentInfo;
    otNeighborInfo neighborInfo;
    otNeighborInfoIterator iterator = OT_NEIGHBOR_INFO_ITERATOR_INIT;
    int8_t rssi = -127;
    uint32_t role = otThreadGetDeviceRole(instance);
    if (role == OT_DEVICE_ROLE_ROUTER || role == OT_DEVICE_ROLE_CHILD) {
        if (otThreadGetParentInfo(instance, &parentInfo) != OT_ERROR_NONE) {
            log_info("get parent fail ");
        }

        while (otThreadGetNextNeighborInfo(instance, &iterator, &neighborInfo)
               == OT_ERROR_NONE) {
            if (neighborInfo.mRloc16 == parentInfo.mRloc16) {
                rssi = neighborInfo.mAverageRssi;
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
        char string[OT_IP6_ADDRESS_STRING_SIZE];
        otIp6AddressToString(&leader_addr, string, sizeof(string));
        log_info("[Network] >> Enroll Request (%d) %s [%s,rssi=%d, "
                 "ver=%d]",
                 enroll_send_try, string,
                 otThreadDeviceRoleToString(enroll_req.role), enroll_req.rssi,
                 enroll_req.version);
    }
    return 0;
}

int net_mgm_enroll_update_send(otInstance* instance) {
    otExtAddress aExtAddress;
    otLeaderData leaderData;
    otRouterInfo routerInfo;
    uint16_t destRloc16;
    uint16_t nextHopRloc16;
    uint8_t pathCost;
    uint16_t maxChildren;
    otChildInfo childInfo;
    otNeighborInfo neighborInfo;
    otNeighborInfoIterator iterator = OT_NEIGHBOR_INFO_ITERATOR_INIT;
    int8_t rssi = -127;
    uint32_t role = otThreadGetDeviceRole(instance);
    if (role == OT_DEVICE_ROLE_ROUTER) {

        otThreadGetLeaderData(instance, &leaderData);

        otThreadGetRouterInfo(instance, leaderData.mLeaderRouterId,
                              &routerInfo);
        otThreadGetNextHopAndPathCost(instance, routerInfo.mRloc16,
                                      &nextHopRloc16, &pathCost);

        while (otThreadGetNextNeighborInfo(instance, &iterator, &neighborInfo)
               == OT_ERROR_NONE) {
            if (neighborInfo.mRloc16 == nextHopRloc16) {
                rssi = neighborInfo.mAverageRssi;
                break;
            }
        }
        // generate enrollment update
        net_mgm_enroll_update_t enroll_update;
        enroll_update.parent = nextHopRloc16;
        enroll_update.self_rloc = otThreadGetRloc16(instance);
        enroll_update.self_rssi_from_parent = rssi;
        enroll_update.child_cnt = 0;

        maxChildren = otThreadGetMaxAllowedChildren(instance);

        for (uint16_t i = 0; i < maxChildren; i++) {
            if ((otThreadGetChildInfoByIndex(instance, i, &childInfo)
                 == OT_ERROR_NONE)
                && !childInfo.mIsStateRestoring) {
                enroll_update.children[enroll_update.child_cnt++] =
                    (net_mgm_child_update_info_t){childInfo.mRloc16,
                                                  childInfo.mAverageRssi};
            }
        }

        // Send enrollment update
        const size_t kEnrollUpdateHeaderSize = offsetof(net_mgm_enroll_update_t,
                                                        children);
        size_t children_list_size = enroll_update.child_cnt
                                    * sizeof(net_mgm_child_update_info_t);
        size_t total_payload_size = kEnrollUpdateHeaderSize
                                    + children_list_size;
        /*setting leader ip*/
        otIp6Address leader_addr = *otThreadGetRloc(instance);
        leader_addr.mFields.m8[14] = 0xFC;
        leader_addr.mFields.m8[15] = 0x00;
        if (app_ctrl_send_cmd(
                (uint8_t*)&leader_addr, CMD_ID_NETWORK_ENROLL_UPDATE, FLAG_UDP,
                (uint8_t*)&enroll_update, total_payload_size, 0, 0)) {
            log_info("app_ctrl_send_cmd fail");
        }
        char string[OT_IP6_ADDRESS_STRING_SIZE];
        otIp6AddressToString(&leader_addr, string, sizeof(string));
        log_info("[Network] >> Enroll Update %s [%d]", string,
                 enroll_update.child_cnt);
    }
    return 0;
}

static void net_mgm_node_table_survive_time_handler() {
    if (net_mgm_node_table) {
        for (uint16_t i = 0; i < NET_MGM_NODE_TABLE_MAX_SIZE; i++) {
            if (net_mgm_node_table[i].used) {
                if (net_mgm_node_table[i].survivaltime > 0
                    && --net_mgm_node_table[i].survivaltime == 0) {
                    net_mgm_node_table[i].role = OT_DEVICE_ROLE_DETACHED;
                    log_info(
                        "timeout %s %04X %02X%02X%02X%02X%02X%02X%02X%02X",
                        otThreadDeviceRoleToString(net_mgm_node_table[i].role),
                        net_mgm_node_table[i].rloc,
                        net_mgm_node_table[i].extaddr[0],
                        net_mgm_node_table[i].extaddr[1],
                        net_mgm_node_table[i].extaddr[2],
                        net_mgm_node_table[i].extaddr[3],
                        net_mgm_node_table[i].extaddr[4],
                        net_mgm_node_table[i].extaddr[5],
                        net_mgm_node_table[i].extaddr[6],
                        net_mgm_node_table[i].extaddr[7]);
                }
            }
        }
    }
}

void net_mgm_survival_timeout_callback(TimerHandle_t xTimer) {
    if (net_mgm_check_leader_pin_state() == true) {
        net_mgm_node_table_survive_time_handler();
        app_set_led0_toggle();
        app_set_led1_toggle();
        if (leader_child_enroll_update_time++ > 30) {
            otChildInfo childInfo;
            uint16_t maxChildren = otThreadGetMaxAllowedChildren(
                otrGetInstance());
            uint16_t self_rloc = otThreadGetRloc16(otrGetInstance());
            for (uint16_t i = 0; i < maxChildren; i++) {
                if ((otThreadGetChildInfoByIndex(otrGetInstance(), i,
                                                 &childInfo)
                     == OT_ERROR_NONE)
                    && !childInfo.mIsStateRestoring) {
                    net_mgm_node_survivaltime_update(
                        OT_DEVICE_ROLE_CHILD, self_rloc, childInfo.mRloc16,
                        childInfo.mAverageRssi);
                }
            }
            leader_child_enroll_update_time = 0;
        }
    } else {
        if (otThreadGetDeviceRole(otrGetInstance())
            <= OT_DEVICE_ROLE_DETACHED) {
            /*debug use*/
            app_set_led0_toggle();
        } else {
            if (enroll_done == false) {
                if (enroll_send_time == 0 || enroll_send_time > 20) {
                    if (enroll_send_try < 5) {
                        enroll_send_try++;
                        app_evt_single(NULL, APP_EVENT_NET_MGM_ENROLL_REQ_SEND);

                    } else {
                        enroll_send_try = 0;
                        app_evt_single(
                            NULL, APP_EVENT_NET_MGM_ENROLL_REQ_SEND_TRYOUT);
                    }
                    enroll_send_time = 1;
                } else {
                    enroll_send_time++;
                }
                app_set_led0_flash();
            } else {
                if (enroll_update_timeout > 0) {
                    --enroll_update_timeout;
                    if (enroll_update_timeout == 0) {
                        if (otThreadGetDeviceRole(otrGetInstance())
                            == OT_DEVICE_ROLE_ROUTER) {
                            app_evt_single(
                                NULL, APP_EVENT_NET_MGM_ENROLL_UPDATE_SEND);
                            enroll_update_timeout = 30;
                        }
                    }
                }
                app_set_led0_off();
                app_set_led1_toggle();
            }
        }
    }
}

void net_mgm_init(otInstance* instance) {
    /*network management*/
    /*leader pin state use*/
    hosal_gpio_input_config_t pin_cfg;
    pin_cfg.param = NULL;
    pin_cfg.pin_int_mode = HOSAL_GPIO_PIN_NOINT;
    pin_cfg.usr_cb = NULL;
    hosal_gpio_cfg_input(22, pin_cfg);

    if (net_mgm_check_leader_pin_state() == true
        && NULL == net_mgm_node_table) {
        log_info("malloc net_mgm_node_table %p ", net_mgm_node_table);
        net_mgm_node_table = xMalloc(sizeof(net_mgm_node_table_t)
                                     * NET_MGM_NODE_TABLE_MAX_SIZE);
        log_info("net_mgm_node_table %p ", net_mgm_node_table);
        if (net_mgm_node_table) {
            memset(net_mgm_node_table, 0x0,
                   sizeof(net_mgm_node_table_t) * NET_MGM_NODE_TABLE_MAX_SIZE);
        } else {
            log_info("net_mgm_node_table malloc fail ");
            return;
        }
        char key[] = "partitionid";
        uint32_t partition_id = 0xFFFFFFFF;
        size_t len = 0;
        efd_get_env_blob(key, &partition_id, sizeof(partition_id),
                         (size_t*)&len);
        if (efd_get_env_blob(key, &partition_id, sizeof(partition_id),
                             (size_t*)&len)
            > 0) {
            log_info("get partition_id 0x%08X ", partition_id);
            partition_id++;
        }
        otThreadSetPreferredLeaderPartitionId(instance, partition_id);
        efd_set_env_blob(key, &partition_id, sizeof(partition_id));
        log_info("set partition_id 0x%08X ", partition_id);

        otThreadSetLocalLeaderWeight(instance, 128);

        if (otThreadBecomeLeader(instance) != OT_ERROR_NONE) {
            log_info("otThreadBecomeLeader fail ");
        }

        otIp6Address multicast_addr;
        static uint8_t is_need_erase = 0;
        static uint8_t reboot_time = 10;
        uint8_t payload[2];
        payload[0] = is_need_erase;
        payload[1] = reboot_time;
        char multicast_ip[] = "ff03::1";
        otIp6AddressFromString(multicast_ip, &multicast_addr);
        for (uint8_t i = 0; i < 5; i++) {
            if (app_ctrl_send_cmd((uint8_t*)&multicast_addr, CMD_ID_REBOOT,
                                  FLAG_UDP, payload, sizeof(payload), 0, 0)) {
                log_info("app_ctrl_send_cmd fail");
            }
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
    if (sNetSurvivalTimer == NULL) {
        sNetSurvivalTimer = xTimerCreate(
            "NetSurvivalTimer", pdMS_TO_TICKS(1000), pdTRUE, NULL,
            (TimerCallbackFunction_t)net_mgm_survival_timeout_callback);
        xTimerStart(sNetSurvivalTimer, 0);
    }
}
#endif