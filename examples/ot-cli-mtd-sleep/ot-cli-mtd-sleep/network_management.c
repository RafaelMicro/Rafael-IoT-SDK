/**
 * @file nwtwork_management.c
 * @author
 * @brief
 * @version 0.1
 * @date 2022-03-31
 *
 * @copyright Copyright (c) 2022
 *
 */

//=============================================================================
//                Include
//=============================================================================
/* Utility Library APIs */
#include <openthread-core-config.h>
#include <openthread-system.h>
#include <openthread/coap.h>
#include <openthread/instance.h>
#include <openthread/link.h>
#include <openthread/thread_ftd.h>
#include "FreeRTOS.h"
#include "cli.h"
#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "log.h"
#include "main.h"
#include "task.h"
#include "timers.h"
//=============================================================================
//                Private Function Declaration
//=============================================================================
#define RAFAEL_NWK_MGM_URL "nwk"

#define NWK_MGM_CHILD_REQ_TATLE_SIZE (30)

#define NWK_MGM_CHILD_REQ_ENTRIES_TIME                                         \
    OPENTHREAD_CONFIG_MLE_CHILD_TIMEOUT_DEFAULT
#define NWK_MGM_ROUTER_KEEP_ALIVE_TIME                                         \
    ((OPENTHREAD_CONFIG_MLE_CHILD_TIMEOUT_DEFAULT / 3) + 1)

#define NWK_MGM_CHILD_INFO_SIZE (OPENTHREAD_CONFIG_MLE_MAX_CHILDREN + 1)

unsigned int nwk_mgm_debug_flags = 0;

// #define nwk_mgm_printf(args...)      \
//     do                               \
//     {                                \
//         if (nwk_mgm_debug_flags > 0) \
//         {                            \
//             log_info(args...);       \
//         }                            \
//     } while (0);

#define nwk_mgm_printf(args...)

//=============================================================================
//                Private ENUM
//=============================================================================
enum {
    NWK_MGM_TYPE_CHILD_REG,
    NWK_MGM_TYPE_CHILD_REG_ACK,
    NWK_MGM_TYPE_CHILD_KICK
};

//=============================================================================
//                Private Struct
//=============================================================================

typedef struct {
    uint16_t rloc;
    uint8_t extaddr[OT_EXT_ADDRESS_SIZE];
    int8_t rssi;
} nwk_mgm_child_info_t;

typedef struct {
    uint8_t data_type;
    uint16_t parent;
    uint16_t self_rloc;
    uint8_t self_extaddr[OT_EXT_ADDRESS_SIZE];
    uint16_t num;
    nwk_mgm_child_info_t child_info[NWK_MGM_CHILD_INFO_SIZE];
} nwk_mgm_child_reg_data_t;

typedef struct {
    uint8_t data_type;
    uint8_t state;
    uint16_t kick_num;
    nwk_mgm_child_info_t child_info[NWK_MGM_CHILD_INFO_SIZE];
} nwk_mgm_child_reg_ack_info_t;

typedef struct {
    uint8_t data_type;
    uint16_t rloc;
    uint16_t panid;
} nwk_mgm_kick_child_info_t;

typedef struct {
    uint8_t used;
    uint8_t role;
    uint16_t parent;
    uint16_t rloc;
    uint8_t extaddr[OT_EXT_ADDRESS_SIZE];
    int8_t rssi;
    uint16_t validtime;
} nwk_mgm_child_reg_table_t;

otCoapResource nwk_mgm_resource;

static nwk_mgm_child_reg_table_t* nwk_mgm_child_reg_table = NULL;

static TimerHandle_t nwk_mgm_timer = NULL;

static uint16_t mgm_reg_send_timer = 0;
static uint16_t mgm_router_up_timer = 0;
static bool reg_table_is_full = false;

//=============================================================================
//                Private Function Declaration
//=============================================================================
static void nwk_mgm_kick_child_post(uint16_t rloc, uint16_t panid,
                                    uint8_t* extaddr);

extern otError otLinkBlackListAddPanId(otInstance* aInstance,
                                       const uint16_t aPanId);
extern void otLinkRemoveChildren(otInstance* aInstance, const uint16_t aRloc16);

//=============================================================================
//                Functions
//=============================================================================

void nwk_mgm_debug_level(unsigned int level) { nwk_mgm_debug_flags = level; }

void nwk_mgm_reg_send_timer_set(uint16_t time) { mgm_reg_send_timer = time; }

void nwk_mgm_router_update_timer_set(uint16_t time) {
    mgm_router_up_timer = time;
}

static int nwk_mgm_child_reg_table_add(uint8_t role, uint16_t parent,
                                       uint16_t rloc, uint8_t* extaddr,
                                       int8_t rssi) {
    uint16_t i = 0;
    if (nwk_mgm_child_reg_table) {
        enter_critical_section();
        for (i = 0; i < NWK_MGM_CHILD_REQ_TATLE_SIZE; i++) {
            if (nwk_mgm_child_reg_table[i].used
                && memcmp(nwk_mgm_child_reg_table[i].extaddr, extaddr,
                          OT_EXT_ADDRESS_SIZE)
                       == 0) {
                nwk_mgm_printf("updata %02X%02X%02X%02X%02X%02X%02X%02X \n",
                               nwk_mgm_child_reg_table[i].extaddr[0],
                               nwk_mgm_child_reg_table[i].extaddr[1],
                               nwk_mgm_child_reg_table[i].extaddr[2],
                               nwk_mgm_child_reg_table[i].extaddr[3],
                               nwk_mgm_child_reg_table[i].extaddr[4],
                               nwk_mgm_child_reg_table[i].extaddr[5],
                               nwk_mgm_child_reg_table[i].extaddr[6],
                               nwk_mgm_child_reg_table[i].extaddr[7]);

                nwk_mgm_child_reg_table[i].parent = parent;
                nwk_mgm_child_reg_table[i].role = role;
                nwk_mgm_child_reg_table[i].rloc = rloc;
                nwk_mgm_child_reg_table[i].rssi = rssi;
                nwk_mgm_child_reg_table[i].validtime =
                    NWK_MGM_CHILD_REQ_ENTRIES_TIME;
                break;
            }
        }
        if (i >= NWK_MGM_CHILD_REQ_TATLE_SIZE) {
            for (i = 0; i < NWK_MGM_CHILD_REQ_TATLE_SIZE; i++) {
                if (0 == nwk_mgm_child_reg_table[i].used) {
                    nwk_mgm_child_reg_table[i].parent = parent;
                    nwk_mgm_child_reg_table[i].role = role;
                    nwk_mgm_child_reg_table[i].rloc = rloc;
                    memcpy(nwk_mgm_child_reg_table[i].extaddr, extaddr,
                           OT_EXT_ADDRESS_SIZE);
                    nwk_mgm_child_reg_table[i].rssi = rssi;
                    nwk_mgm_child_reg_table[i].used = 1;
                    nwk_mgm_child_reg_table[i].validtime =
                        NWK_MGM_CHILD_REQ_ENTRIES_TIME;
                    nwk_mgm_printf(
                        "add %s %04X %02X%02X%02X%02X%02X%02X%02X%02X %d \n",
                        otThreadDeviceRoleToString(
                            nwk_mgm_child_reg_table[i].role),
                        nwk_mgm_child_reg_table[i].rloc,
                        nwk_mgm_child_reg_table[i].extaddr[0],
                        nwk_mgm_child_reg_table[i].extaddr[1],
                        nwk_mgm_child_reg_table[i].extaddr[2],
                        nwk_mgm_child_reg_table[i].extaddr[3],
                        nwk_mgm_child_reg_table[i].extaddr[4],
                        nwk_mgm_child_reg_table[i].extaddr[5],
                        nwk_mgm_child_reg_table[i].extaddr[6],
                        nwk_mgm_child_reg_table[i].extaddr[7],
                        nwk_mgm_child_reg_table[i].rssi);
                    break;
                }
            }
        }
        leave_critical_section();
    }
    return (i >= NWK_MGM_CHILD_REQ_TATLE_SIZE);
}

static void nwk_mgm_child_reg_table_remove(uint8_t role, uint8_t* extaddr) {
    if (nwk_mgm_child_reg_table) {
        enter_critical_section();
        for (uint16_t i = 0; i < NWK_MGM_CHILD_REQ_TATLE_SIZE; i++) {
            if (nwk_mgm_child_reg_table[i].used
                && memcmp(nwk_mgm_child_reg_table[i].extaddr, extaddr,
                          OT_EXT_ADDRESS_SIZE)
                       == 0) {
                if (role == nwk_mgm_child_reg_table[i].role) {
                    nwk_mgm_printf(
                        "remove %s %04X %02X%02X%02X%02X%02X%02X%02X%02X %d \n",
                        otThreadDeviceRoleToString(
                            nwk_mgm_child_reg_table[i].role),
                        nwk_mgm_child_reg_table[i].rloc,
                        nwk_mgm_child_reg_table[i].extaddr[0],
                        nwk_mgm_child_reg_table[i].extaddr[1],
                        nwk_mgm_child_reg_table[i].extaddr[2],
                        nwk_mgm_child_reg_table[i].extaddr[3],
                        nwk_mgm_child_reg_table[i].extaddr[4],
                        nwk_mgm_child_reg_table[i].extaddr[5],
                        nwk_mgm_child_reg_table[i].extaddr[6],
                        nwk_mgm_child_reg_table[i].extaddr[7],
                        nwk_mgm_child_reg_table[i].rssi);
                    nwk_mgm_child_reg_table[i].role = OT_DEVICE_ROLE_DETACHED;
                }
            }
        }
        leave_critical_section();
    }
}

static void nwk_mgm_child_reg_table_time_handler() {
    if (nwk_mgm_child_reg_table) {
        enter_critical_section();
        for (uint16_t i = 0; i < NWK_MGM_CHILD_REQ_TATLE_SIZE; i++) {
            if (nwk_mgm_child_reg_table[i].used) {
                if (nwk_mgm_child_reg_table[i].validtime > 0
                    && --nwk_mgm_child_reg_table[i].validtime == 0) {
                    nwk_mgm_printf(
                        "timeout %s %04X %02X%02X%02X%02X%02X%02X%02X%02X \n",
                        otThreadDeviceRoleToString(
                            nwk_mgm_child_reg_table[i].role),
                        nwk_mgm_child_reg_table[i].rloc,
                        nwk_mgm_child_reg_table[i].extaddr[0],
                        nwk_mgm_child_reg_table[i].extaddr[1],
                        nwk_mgm_child_reg_table[i].extaddr[2],
                        nwk_mgm_child_reg_table[i].extaddr[3],
                        nwk_mgm_child_reg_table[i].extaddr[4],
                        nwk_mgm_child_reg_table[i].extaddr[5],
                        nwk_mgm_child_reg_table[i].extaddr[6],
                        nwk_mgm_child_reg_table[i].extaddr[7]);
                    memset(&nwk_mgm_child_reg_table[i], 0x0,
                           sizeof(nwk_mgm_child_reg_table_t));
                }
            }
        }
        leave_critical_section();
    }
}

bool otPlatFindNwkMgmChildRegTable(uint8_t* aExtAddress) {
    bool ret = false;
    uint16_t i = 0;
    if (nwk_mgm_child_reg_table) {
        for (i = 0; i < NWK_MGM_CHILD_REQ_TATLE_SIZE; i++) {
            if (nwk_mgm_child_reg_table[i].used
                && memcmp(nwk_mgm_child_reg_table[i].extaddr, aExtAddress,
                          OT_EXT_ADDRESS_SIZE)
                       == 0) {
                nwk_mgm_printf("fined %02X%02X%02X%02X%02X%02X%02X%02X \n",
                               aExtAddress[0], aExtAddress[1], aExtAddress[2],
                               aExtAddress[3], aExtAddress[4], aExtAddress[5],
                               aExtAddress[6], aExtAddress[7]);
                ret = true;
                break;
            }
        }
        if (ret == false) {
            nwk_mgm_printf("not find %02X%02X%02X%02X%02X%02X%02X%02X \n",
                           aExtAddress[0], aExtAddress[1], aExtAddress[2],
                           aExtAddress[3], aExtAddress[4], aExtAddress[5],
                           aExtAddress[6], aExtAddress[7]);
        }
    }
    return ret;
}

bool otPlatNwkMgmIsFull() { return reg_table_is_full; }

void nwk_mgm_child_reg_table_display() {
    uint16_t i = 0, count = 0;
    if (nwk_mgm_child_reg_table) {
        log_info("index role parent rloc extaddr rssi %u \n", count);
        log_info("\n =============================================== \n");
        for (i = 0; i < NWK_MGM_CHILD_REQ_TATLE_SIZE; i++) {
            if (nwk_mgm_child_reg_table[i].used
                && (nwk_mgm_child_reg_table[i].role == OT_DEVICE_ROLE_ROUTER)) {
                ++count;
                log_info(
                    "[%u] %s %04X %04X %02X%02X%02X%02X%02X%02X%02X%02X %d "
                    "%u\n",
                    count,
                    otThreadDeviceRoleToString(nwk_mgm_child_reg_table[i].role),
                    nwk_mgm_child_reg_table[i].parent,
                    nwk_mgm_child_reg_table[i].rloc,
                    nwk_mgm_child_reg_table[i].extaddr[0],
                    nwk_mgm_child_reg_table[i].extaddr[1],
                    nwk_mgm_child_reg_table[i].extaddr[2],
                    nwk_mgm_child_reg_table[i].extaddr[3],
                    nwk_mgm_child_reg_table[i].extaddr[4],
                    nwk_mgm_child_reg_table[i].extaddr[5],
                    nwk_mgm_child_reg_table[i].extaddr[6],
                    nwk_mgm_child_reg_table[i].extaddr[7],
                    nwk_mgm_child_reg_table[i].rssi,
                    nwk_mgm_child_reg_table[i].validtime);
            }
        }
        for (i = 0; i < NWK_MGM_CHILD_REQ_TATLE_SIZE; i++) {
            if (nwk_mgm_child_reg_table[i].used) {
                if (nwk_mgm_child_reg_table[i].role == OT_DEVICE_ROLE_CHILD) {
                    ++count;
                    log_info("[%u] %s %04X %04X "
                             "%02X%02X%02X%02X%02X%02X%02X%02X %d %u\n",
                             count,
                             otThreadDeviceRoleToString(
                                 nwk_mgm_child_reg_table[i].role),
                             nwk_mgm_child_reg_table[i].parent,
                             nwk_mgm_child_reg_table[i].rloc,
                             nwk_mgm_child_reg_table[i].extaddr[0],
                             nwk_mgm_child_reg_table[i].extaddr[1],
                             nwk_mgm_child_reg_table[i].extaddr[2],
                             nwk_mgm_child_reg_table[i].extaddr[3],
                             nwk_mgm_child_reg_table[i].extaddr[4],
                             nwk_mgm_child_reg_table[i].extaddr[5],
                             nwk_mgm_child_reg_table[i].extaddr[6],
                             nwk_mgm_child_reg_table[i].extaddr[7],
                             nwk_mgm_child_reg_table[i].rssi,
                             nwk_mgm_child_reg_table[i].validtime);
                }
            }
        }
        for (i = 0; i < NWK_MGM_CHILD_REQ_TATLE_SIZE; i++) {
            if (nwk_mgm_child_reg_table[i].used) {

                if ((nwk_mgm_child_reg_table[i].role != OT_DEVICE_ROLE_CHILD)
                    && (nwk_mgm_child_reg_table[i].role
                        != OT_DEVICE_ROLE_ROUTER)) {
                    ++count;
                    log_info("[%u] %s %04X %04X "
                             "%02X%02X%02X%02X%02X%02X%02X%02X %d %u\n",
                             count,
                             otThreadDeviceRoleToString(
                                 nwk_mgm_child_reg_table[i].role),
                             nwk_mgm_child_reg_table[i].parent,
                             nwk_mgm_child_reg_table[i].rloc,
                             nwk_mgm_child_reg_table[i].extaddr[0],
                             nwk_mgm_child_reg_table[i].extaddr[1],
                             nwk_mgm_child_reg_table[i].extaddr[2],
                             nwk_mgm_child_reg_table[i].extaddr[3],
                             nwk_mgm_child_reg_table[i].extaddr[4],
                             nwk_mgm_child_reg_table[i].extaddr[5],
                             nwk_mgm_child_reg_table[i].extaddr[6],
                             nwk_mgm_child_reg_table[i].extaddr[7],
                             nwk_mgm_child_reg_table[i].rssi,
                             nwk_mgm_child_reg_table[i].validtime);
                }
            }
        }
        log_info("=============================================== \n");
        log_info("total num %u \n", count);
    }
}

static void nwk_mgm_reset() {
    otInstance* instance = otrGetInstance();
    if (instance) {
        otDeviceRole mRole = otThreadGetDeviceRole(instance);
        if (OT_DEVICE_ROLE_ROUTER == mRole || OT_DEVICE_ROLE_CHILD == mRole) {
            otOperationalDataset dataset;
            // otOperationalDatasetTlvs sDatasetTlvs;

            memset(&dataset, 0, sizeof(otOperationalDataset));

            dataset.mActiveTimestamp.mSeconds = 0;
            dataset.mComponents.mIsActiveTimestampPresent = false;
            OT_ASSERT(otDatasetSetActive(instance, &dataset) == OT_ERROR_NONE);
            // OT_ASSERT(otDatasetUpdateTlvs(&dataset, &sDatasetTlvs) == OT_ERROR_NONE);
            // OT_ASSERT(otDatasetSetActiveTlvs(instance &sDatasetTlvs) == OT_ERROR_NONE);
            OT_ASSERT(otThreadBecomeDetached(instance) == OT_ERROR_NONE);
        }
    }
}

static int nwk_mgm_data_parse(uint8_t type, uint8_t* payload,
                              uint16_t payloadlength, void* data) {
    nwk_mgm_child_reg_data_t* child_reg_data = NULL;
    nwk_mgm_child_reg_ack_info_t* child_reg_data_ack = NULL;
    nwk_mgm_kick_child_info_t* kick_child_data = NULL;
    uint8_t* tmp = payload;

    if (type == NWK_MGM_TYPE_CHILD_REG) {
        child_reg_data = (nwk_mgm_child_reg_data_t*)data;
        child_reg_data->data_type = *tmp++;
        memcpy(&child_reg_data->parent, tmp, 2);
        tmp += 2;
        memcpy(&child_reg_data->self_rloc, tmp, 2);
        tmp += 2;
        memcpy(&child_reg_data->self_extaddr, tmp, OT_EXT_ADDRESS_SIZE);
        tmp += OT_EXT_ADDRESS_SIZE;
        memcpy(&child_reg_data->num, tmp, 2);
        tmp += 2;
        if (child_reg_data->num > 0) {
            memcpy(&child_reg_data->child_info, tmp,
                   (child_reg_data->num * sizeof(nwk_mgm_child_info_t)));
            tmp += (child_reg_data->num * sizeof(nwk_mgm_child_info_t));
        }
    } else if (type == NWK_MGM_TYPE_CHILD_REG_ACK) {
        child_reg_data_ack = (nwk_mgm_child_reg_ack_info_t*)data;
        child_reg_data_ack->data_type = *tmp++;
        child_reg_data_ack->state = *tmp++;
        memcpy(&child_reg_data_ack->kick_num, tmp, 2);
        tmp += 2;
        if (child_reg_data_ack->kick_num > 0) {
            memcpy(
                &child_reg_data_ack->child_info, tmp,
                (child_reg_data_ack->kick_num * sizeof(nwk_mgm_child_info_t)));
            tmp += (child_reg_data_ack->kick_num
                    * sizeof(nwk_mgm_child_info_t));
        }
    } else if (type == NWK_MGM_TYPE_CHILD_KICK) {
        kick_child_data = (nwk_mgm_kick_child_info_t*)data;
        kick_child_data->data_type = *tmp++;
        memcpy(&kick_child_data->rloc, tmp, 2);
        tmp += 2;
        memcpy(&kick_child_data->panid, tmp, 2);
        tmp += 2;
    } else {
        nwk_mgm_printf("unknow parse type %u \n", type);
    }
    if ((tmp - payload) != payloadlength) {
        nwk_mgm_printf("parse fail %u (%u/%u)\n", type, (tmp - payload),
                       payloadlength);
        return 1;
    }
    return 0;
}

static void nwk_mgm_data_piece(uint8_t type, uint8_t* payload,
                               uint16_t* payloadlength, void* data) {
    uint8_t* ptr = (uint8_t*)data;
    nwk_mgm_child_reg_data_t* child_reg_data = NULL;
    nwk_mgm_child_reg_ack_info_t* child_reg_data_ack = NULL;
    nwk_mgm_kick_child_info_t* kick_child_data = NULL;

    uint8_t* tmp = payload;
    *tmp++ = ptr[0];

    if (type == NWK_MGM_TYPE_CHILD_REG) {
        child_reg_data = (nwk_mgm_child_reg_data_t*)data;
        memcpy(tmp, &child_reg_data->parent, 2);
        tmp += 2;
        memcpy(tmp, &child_reg_data->self_rloc, 2);
        tmp += 2;
        memcpy(tmp, &child_reg_data->self_extaddr, OT_EXT_ADDRESS_SIZE);
        tmp += OT_EXT_ADDRESS_SIZE;
        memcpy(tmp, &child_reg_data->num, 2);
        tmp += 2;
        if (child_reg_data->num > 0) {
            memcpy(tmp, &child_reg_data->child_info,
                   (child_reg_data->num * sizeof(nwk_mgm_child_info_t)));
            tmp += (child_reg_data->num * sizeof(nwk_mgm_child_info_t));
        }
    } else if (type == NWK_MGM_TYPE_CHILD_REG_ACK) {
        child_reg_data_ack = (nwk_mgm_child_reg_ack_info_t*)data;

        *tmp++ = child_reg_data_ack->state;
        memcpy(tmp, &child_reg_data_ack->kick_num, 2);
        tmp += 2;
        if (child_reg_data_ack->kick_num > 0) {
            memcpy(
                tmp, &child_reg_data_ack->child_info,
                (child_reg_data_ack->kick_num * sizeof(nwk_mgm_child_info_t)));
            tmp += (child_reg_data_ack->kick_num
                    * sizeof(nwk_mgm_child_info_t));
        }
    } else if (type == NWK_MGM_TYPE_CHILD_KICK) {
        kick_child_data = (nwk_mgm_kick_child_info_t*)data;

        memcpy(tmp, &kick_child_data->rloc, 2);
        tmp += 2;
        memcpy(tmp, &kick_child_data->panid, 2);
        tmp += 2;
    } else {
        nwk_mgm_printf("unknow piece type %u \n", type);
    }
    *payloadlength = (tmp - payload);
}

static void nwk_mgm_childs_register_proccess(otMessage* aMessage,
                                             const otMessageInfo* aMessageInfo,
                                             uint8_t* buf, uint16_t length) {
    otDeviceRole mRole;
    otError error = OT_ERROR_NONE;
    otMessage* responseMessage = NULL;
    otCoapCode responseCode = OT_COAP_CODE_EMPTY;
    nwk_mgm_child_reg_data_t child_reg_data;
    nwk_mgm_child_reg_ack_info_t child_reg_data_ack;
    uint16_t* ack_rloc = NULL;
    uint8_t* payload = NULL;
    uint16_t i = 0, k = 0, payloadlength;

    otInstance* instance = otrGetInstance();
    if (instance) {
        mRole = otThreadGetDeviceRole(instance);
        do {
            if (length > 0) {
                if (NULL != buf) {
                    if (otCoapMessageGetType(aMessage)
                        == OT_COAP_TYPE_CONFIRMABLE) {
                        /*do ack packet*/
                        responseCode = OT_COAP_CODE_VALID;
                        responseMessage = otCoapNewMessage(instance, NULL);
                        if (responseMessage == NULL) {
                            error = OT_ERROR_NO_BUFS;
                            break;
                        }
                        error = otCoapMessageInitResponse(
                            responseMessage, aMessage,
                            OT_COAP_TYPE_ACKNOWLEDGMENT, responseCode);
                        if (error != OT_ERROR_NONE) {
                            break;
                        }
                        memset(&child_reg_data_ack, 0x0,
                               sizeof(nwk_mgm_child_reg_ack_info_t));
                        child_reg_data_ack.data_type =
                            NWK_MGM_TYPE_CHILD_REG_ACK;
                        child_reg_data_ack.state = 0; // success = 0, fail = 1
                        child_reg_data_ack.kick_num = 0;
                        reg_table_is_full = false;

                        if (mRole == OT_DEVICE_ROLE_LEADER) {
                            if (nwk_mgm_data_parse(NWK_MGM_TYPE_CHILD_REG, buf,
                                                   length, &child_reg_data)) {
                                break;
                            }
                            nwk_mgm_printf(
                                "self %04x,%04x, "
                                "%02X%02X%02X%02X%02X%02X%02X%02X \n",
                                child_reg_data.parent, child_reg_data.self_rloc,
                                child_reg_data.self_extaddr[0],
                                child_reg_data.self_extaddr[1],
                                child_reg_data.self_extaddr[2],
                                child_reg_data.self_extaddr[3],
                                child_reg_data.self_extaddr[4],
                                child_reg_data.self_extaddr[5],
                                child_reg_data.self_extaddr[6],
                                child_reg_data.self_extaddr[7]);

                            /*add my self*/
                            if (nwk_mgm_child_reg_table_add(
                                    OT_DEVICE_ROLE_ROUTER,
                                    child_reg_data.parent,
                                    child_reg_data.self_rloc,
                                    child_reg_data.self_extaddr, (-128))) {
                                nwk_mgm_printf("unexpected router add fail\n");
                                reg_table_is_full = true;
                                child_reg_data_ack.state =
                                    1; // success = 0, fail = 1
                                child_reg_data_ack
                                    .child_info[child_reg_data_ack.kick_num]
                                    .rloc = child_reg_data.self_rloc;
                                memcpy(
                                    child_reg_data_ack
                                        .child_info[child_reg_data_ack.kick_num]
                                        .extaddr,
                                    child_reg_data.self_extaddr,
                                    OT_EXT_ADDRESS_SIZE);
                                child_reg_data_ack
                                    .child_info[child_reg_data_ack.kick_num]
                                    .rssi = 0;
                                child_reg_data_ack.kick_num++;
                            }
                            /*check child table*/
                            for (i = 0; i < NWK_MGM_CHILD_REQ_TATLE_SIZE; i++) {
                                if (nwk_mgm_child_reg_table[i].used) {
                                    if (nwk_mgm_child_reg_table[i].parent
                                        == child_reg_data.self_rloc) {
                                        for (k = 0; k < child_reg_data.num;
                                             k++) {
                                            if (memcmp(
                                                    nwk_mgm_child_reg_table[i]
                                                        .extaddr,
                                                    child_reg_data.child_info[k]
                                                        .extaddr,
                                                    OT_EXT_ADDRESS_SIZE)
                                                == 0) {
                                                break;
                                            }
                                        }

                                        if (k >= child_reg_data.num) {
                                            nwk_mgm_printf(
                                                "data rm %04x %04x "
                                                "%02X%02X%02X%02X%02X%02X%02X%"
                                                "02X\n",
                                                child_reg_data.self_rloc,
                                                nwk_mgm_child_reg_table[i].rloc,
                                                nwk_mgm_child_reg_table[i]
                                                    .extaddr[0],
                                                nwk_mgm_child_reg_table[i]
                                                    .extaddr[1],
                                                nwk_mgm_child_reg_table[i]
                                                    .extaddr[2],
                                                nwk_mgm_child_reg_table[i]
                                                    .extaddr[3],
                                                nwk_mgm_child_reg_table[i]
                                                    .extaddr[4],
                                                nwk_mgm_child_reg_table[i]
                                                    .extaddr[5],
                                                nwk_mgm_child_reg_table[i]
                                                    .extaddr[6],
                                                nwk_mgm_child_reg_table[i]
                                                    .extaddr[7]);
                                            nwk_mgm_child_reg_table_remove(
                                                OT_DEVICE_ROLE_CHILD,
                                                (uint8_t*)
                                                    nwk_mgm_child_reg_table[i]
                                                        .extaddr);
                                        }
                                    }
                                }
                            }
                            /*updata child table*/
                            nwk_mgm_printf("rece %u \n", child_reg_data.num);
                            for (i = 0; i < child_reg_data.num; i++) {
                                nwk_mgm_printf(
                                    "child %04x,%04x, "
                                    "%02X%02X%02X%02X%02X%02X%02X%02X, %d \n",
                                    child_reg_data.self_rloc,
                                    child_reg_data.child_info[i].rloc,
                                    child_reg_data.child_info[i].extaddr[0],
                                    child_reg_data.child_info[i].extaddr[1],
                                    child_reg_data.child_info[i].extaddr[2],
                                    child_reg_data.child_info[i].extaddr[3],
                                    child_reg_data.child_info[i].extaddr[4],
                                    child_reg_data.child_info[i].extaddr[5],
                                    child_reg_data.child_info[i].extaddr[6],
                                    child_reg_data.child_info[i].extaddr[7],
                                    child_reg_data.child_info[i].rssi);
                                if (nwk_mgm_child_reg_table_add(
                                        OT_DEVICE_ROLE_CHILD,
                                        child_reg_data.self_rloc,
                                        child_reg_data.child_info[i].rloc,
                                        child_reg_data.child_info[i].extaddr,
                                        child_reg_data.child_info[i].rssi)) {
                                    reg_table_is_full = true;
                                    child_reg_data_ack.state =
                                        1; // success = 0, fail = 1
                                    child_reg_data_ack
                                        .child_info[child_reg_data_ack.kick_num]
                                        .rloc =
                                        child_reg_data.child_info[i].rloc;
                                    memcpy(child_reg_data_ack
                                               .child_info[child_reg_data_ack
                                                               .kick_num]
                                               .extaddr,
                                           child_reg_data.child_info[i].extaddr,
                                           OT_EXT_ADDRESS_SIZE);
                                    child_reg_data_ack
                                        .child_info[child_reg_data_ack.kick_num]
                                        .rssi =
                                        child_reg_data.child_info[i].rssi;
                                    child_reg_data_ack.kick_num++;
                                }
                            }
                        } else {
                            nwk_mgm_printf("isn't leader \n");
                        }
                        nwk_mgm_printf("ack send %u \n",
                                       child_reg_data_ack.kick_num);
                        payload = pvPortMalloc(
                            sizeof(nwk_mgm_child_reg_ack_info_t));
                        if (payload) {
                            nwk_mgm_data_piece(NWK_MGM_TYPE_CHILD_REG_ACK,
                                               payload, &payloadlength,
                                               &child_reg_data_ack);
                            error = otCoapMessageSetPayloadMarker(
                                responseMessage);
                            if (error != OT_ERROR_NONE) {
                                break;
                            }
                            error = otMessageAppend(responseMessage, payload,
                                                    payloadlength);
                            if (error != OT_ERROR_NONE) {
                                break;
                            }
                        }
                        error = otCoapSendResponseWithParameters(
                            instance, responseMessage, aMessageInfo, NULL);
                        if (error != OT_ERROR_NONE) {
                            break;
                        }
                    }
                }
            }
        } while (0);
        if (payload) {
            vPortFree(payload);
        }
        if (error != OT_ERROR_NONE && responseMessage != NULL) {
            otMessageFree(responseMessage);
        }
    }
}

static void nwk_mgm_childs_register_ack_proccess(uint8_t* buf,
                                                 uint16_t length) {

    otInstance* instance = otrGetInstance();
    if (instance) {
        otDeviceRole mRole = otThreadGetDeviceRole(instance);
        nwk_mgm_child_reg_ack_info_t child_reg_data_ack;
        otExtAddress aExtAddress;
        aExtAddress = *otLinkGetExtendedAddress(instance);
        do {
            if (length > 0) {
                if (NULL != buf) {
                    if (mRole == OT_DEVICE_ROLE_ROUTER) {
                        if (nwk_mgm_data_parse(NWK_MGM_TYPE_CHILD_REG_ACK, buf,
                                               length, &child_reg_data_ack)) {
                            break;
                        }
                        nwk_mgm_printf("ack state %u\n",
                                       child_reg_data_ack.state);
                        reg_table_is_full = child_reg_data_ack.state;
                        if (child_reg_data_ack.state) {
                            for (uint16_t i = 0;
                                 i < child_reg_data_ack.kick_num; i++) {
                                nwk_mgm_printf(
                                    "kick %04x, "
                                    "%02X%02X%02X%02X%02X%02X%02X%02X, %d \n",
                                    child_reg_data_ack.child_info[i].rloc,
                                    child_reg_data_ack.child_info[i].extaddr[0],
                                    child_reg_data_ack.child_info[i].extaddr[1],
                                    child_reg_data_ack.child_info[i].extaddr[2],
                                    child_reg_data_ack.child_info[i].extaddr[3],
                                    child_reg_data_ack.child_info[i].extaddr[4],
                                    child_reg_data_ack.child_info[i].extaddr[5],
                                    child_reg_data_ack.child_info[i].extaddr[6],
                                    child_reg_data_ack.child_info[i].extaddr[7],
                                    child_reg_data_ack.child_info[i].rssi);
                                if (memcmp(child_reg_data_ack.child_info[i]
                                               .extaddr,
                                           aExtAddress.m8, OT_EXT_ADDRESS_SIZE)
                                    == 0) {
                                    nwk_mgm_printf("kick self \n");
                                    if (OT_ERROR_NONE
                                        != otLinkBlackListAddPanId(
                                            instance,
                                            otLinkGetPanId(instance))) {
                                        nwk_mgm_printf("black add fill \n");
                                    }
                                    nwk_mgm_reset();
                                    break;
                                } else {
                                    nwk_mgm_kick_child_post(
                                        child_reg_data_ack.child_info[i].rloc,
                                        otLinkGetPanId(instance),
                                        (uint8_t*)child_reg_data_ack
                                            .child_info[i]
                                            .extaddr);
                                    otLinkRemoveChildren(
                                        instance,
                                        child_reg_data_ack.child_info[i].rloc);
                                }
                            }
                        }
                    }
                }
            }
        } while (0);
    }
}

static void nwk_mgm_kick_child_proccess(uint8_t* buf, uint16_t length) {
    nwk_mgm_kick_child_info_t kick_child_data;

    otInstance* instance = otrGetInstance();
    if (instance) {
        otDeviceRole mRole = otThreadGetDeviceRole(instance);

        do {
            if (length > 0) {
                if (NULL != buf) {
                    if (nwk_mgm_data_parse(NWK_MGM_TYPE_CHILD_KICK, buf, length,
                                           &kick_child_data)) {
                        break;
                    }
                    if (mRole == OT_DEVICE_ROLE_CHILD
                        || mRole == OT_DEVICE_ROLE_ROUTER) {
                        nwk_mgm_printf("kick %04x \n", kick_child_data.panid);

                        if (OT_ERROR_NONE
                            != otLinkBlackListAddPanId(instance,
                                                       kick_child_data.panid)) {
                            nwk_mgm_printf("black add fill \n");
                        }
                        nwk_mgm_reset();
                    }
                }
            }
        } while (0);
    }
}

static void nwk_mgm_request_proccess(void* aContext, otMessage* aMessage,
                                     const otMessageInfo* aMessageInfo) {
    char string[OT_IP6_ADDRESS_STRING_SIZE];
    uint8_t* buf = NULL;
    uint16_t length;
    uint8_t nwk_mgm_data_type = 0xff;
    otIp6AddressToString(&aMessageInfo->mPeerAddr, string, sizeof(string));
    length = otMessageGetLength(aMessage) - otMessageGetOffset(aMessage);

    do {
        if (length > 0) {
            buf = pvPortMalloc(length);
            if (NULL != buf) {
                otMessageRead(aMessage, otMessageGetOffset(aMessage), buf,
                              length);
                nwk_mgm_data_type = buf[0];
                switch (nwk_mgm_data_type) {
                    case NWK_MGM_TYPE_CHILD_REG:
                        nwk_mgm_childs_register_proccess(aMessage, aMessageInfo,
                                                         buf, length);
                        break;
                    case NWK_MGM_TYPE_CHILD_KICK:
                        nwk_mgm_kick_child_proccess(buf, length);
                        break;
                    default: break;
                }
            }
        }
    } while (0);

    if (buf) {
        vPortFree(buf);
    }
}

static void nwk_mgm_ack_process(void* aContext, otMessage* aMessage,
                                const otMessageInfo* aMessageInfo,
                                otError aResult) {
    char string[OT_IP6_ADDRESS_STRING_SIZE];
    uint8_t* buf = NULL;
    uint16_t length;
    uint8_t nwk_mgm_data_type = 0xff;
    otIp6AddressToString(&aMessageInfo->mPeerAddr, string, sizeof(string));
    length = otMessageGetLength(aMessage) - otMessageGetOffset(aMessage);
    do {
        if (length > 0) {
            buf = pvPortMalloc(length);
            if (NULL != buf) {
                otMessageRead(aMessage, otMessageGetOffset(aMessage), buf,
                              length);
                nwk_mgm_data_type = buf[0];
                switch (nwk_mgm_data_type) {
                    case NWK_MGM_TYPE_CHILD_REG_ACK:
                        nwk_mgm_childs_register_ack_proccess(buf, length);
                        break;
                    default: break;
                }
            }
        }
    } while (0);

    if (buf) {
        vPortFree(buf);
    }
}

otError nwk_mgm_coap_request(otCoapCode aCoapCode,
                             otIp6Address coapDestinationIp,
                             otCoapType coapType, uint8_t* payload,
                             uint16_t payloadLength, const char* coap_Path) {
    otError error = OT_ERROR_NONE;
    otMessage* message = NULL;
    otMessageInfo messageInfo;

    otInstance* instance = otrGetInstance();
    if (instance) {
        do {
            message = otCoapNewMessage(instance, NULL);
            if (NULL == message) {
                error = OT_ERROR_NO_BUFS;
                break;
            }
            otCoapMessageInit(message, coapType, aCoapCode);
            otCoapMessageGenerateToken(message, OT_COAP_DEFAULT_TOKEN_LENGTH);

            error = otCoapMessageAppendUriPathOptions(message, coap_Path);
            if (OT_ERROR_NONE != error) {
                break;
            }

            if (payloadLength > 0) {
                error = otCoapMessageSetPayloadMarker(message);
                if (OT_ERROR_NONE != error) {
                    break;
                }
            }

            // Embed content into message if given
            if (payloadLength > 0) {
                error = otMessageAppend(message, payload, payloadLength);
                if (OT_ERROR_NONE != error) {
                    break;
                }
            }

            memset(&messageInfo, 0, sizeof(messageInfo));
            messageInfo.mPeerAddr = coapDestinationIp;
            messageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;

            if ((coapType == OT_COAP_TYPE_CONFIRMABLE)
                || (aCoapCode == OT_COAP_CODE_GET)) {
                error = otCoapSendRequestWithParameters(
                    instance, message, &messageInfo, &nwk_mgm_ack_process, NULL,
                    NULL);
            } else {
                error = otCoapSendRequestWithParameters(
                    instance, message, &messageInfo, NULL, NULL, NULL);
            }
        } while (0);

        if ((error != OT_ERROR_NONE) && (message != NULL)) {
            otMessageFree(message);
        }
    }
    return error;
}

void nwk_mgm_child_register_post() {

    otInstance* instance = otrGetInstance();
    if (instance) {
        otError error = OT_ERROR_NONE;
        otCoapCode CoapCode = OT_COAP_CODE_POST;
        otIp6Address coapDestinationIp = *otThreadGetRloc(instance);
        coapDestinationIp.mFields.m8[14] = 0xfc;
        coapDestinationIp.mFields.m8[15] = 0x00;
        char string[OT_IP6_ADDRESS_STRING_SIZE];
        otIp6AddressToString(&coapDestinationIp, string, sizeof(string));

        otCoapType coapType = OT_COAP_TYPE_CONFIRMABLE;
        nwk_mgm_child_reg_data_t child_reg_data;
        uint8_t* payload = NULL;
        uint16_t payloadlength = 0;
        otRouterInfo parentInfo;
        otExtAddress aExtAddress;
        otChildInfo childInfo;

        memset(&child_reg_data, 0x0, sizeof(nwk_mgm_child_reg_data_t));
        child_reg_data.data_type = NWK_MGM_TYPE_CHILD_REG;
        otThreadGetParentInfo(instance, &parentInfo);
        child_reg_data.parent = parentInfo.mRloc16;
        child_reg_data.self_rloc = otThreadGetRloc16(instance);
        aExtAddress = *otLinkGetExtendedAddress(instance);
        memcpy(child_reg_data.self_extaddr, aExtAddress.m8,
               OT_EXT_ADDRESS_SIZE);

        child_reg_data.num = 0;
        uint16_t childernmax = otThreadGetMaxAllowedChildren(instance);

        for (uint16_t i = 0; i < childernmax; i++) {
            if ((otThreadGetChildInfoByIndex(instance, i, &childInfo)
                 != OT_ERROR_NONE)
                || childInfo.mIsStateRestoring) {
                continue;
            }

            child_reg_data.child_info[child_reg_data.num].rloc =
                childInfo.mRloc16;
            memcpy(child_reg_data.child_info[child_reg_data.num].extaddr,
                   &childInfo.mExtAddress, OT_EXT_ADDRESS_SIZE);
            child_reg_data.child_info[child_reg_data.num].rssi =
                childInfo.mAverageRssi;
            child_reg_data.num++;
        }

        payload = pvPortMalloc(sizeof(nwk_mgm_child_reg_data_t));
        if (payload) {
            nwk_mgm_data_piece(NWK_MGM_TYPE_CHILD_REG, payload, &payloadlength,
                               &child_reg_data);
            error = nwk_mgm_coap_request(CoapCode, coapDestinationIp, coapType,
                                         payload, payloadlength,
                                         RAFAEL_NWK_MGM_URL);
            nwk_mgm_printf("register %s %u %u \n", string, error,
                           payloadlength);
            vPortFree(payload);
            nwk_mgm_router_update_timer_set(NWK_MGM_ROUTER_KEEP_ALIVE_TIME);
        }
    }
    return;
}

static void nwk_mgm_kick_child_post(uint16_t rloc, uint16_t panid,
                                    uint8_t* extaddr) {

    otInstance* instance = otrGetInstance();
    if (instance) {
        otError error = OT_ERROR_NONE;
        otCoapCode CoapCode = OT_COAP_CODE_POST;
        otIp6Address coapDestinationIp;
        nwk_mgm_kick_child_info_t kick_child_data;
        uint8_t* payload = NULL;
        uint16_t payloadlength = 0;

        payload = pvPortMalloc(sizeof(nwk_mgm_kick_child_info_t));
        if (payload) {
            kick_child_data.data_type = NWK_MGM_TYPE_CHILD_KICK;
            kick_child_data.rloc = rloc;
            kick_child_data.panid = panid;
            nwk_mgm_data_piece(NWK_MGM_TYPE_CHILD_KICK, payload, &payloadlength,
                               &kick_child_data);

            coapDestinationIp = *otThreadGetLinkLocalIp6Address(instance);
            memcpy(&coapDestinationIp.mFields.m8[8], extaddr,
                   OT_EXT_ADDRESS_SIZE);
            coapDestinationIp.mFields.m8[8] ^= (1 << 1);

            otCoapType coapType = OT_COAP_TYPE_NON_CONFIRMABLE;

            char string[OT_IP6_ADDRESS_STRING_SIZE];
            otIp6AddressToString(&coapDestinationIp, string, sizeof(string));

            error = nwk_mgm_coap_request(CoapCode, coapDestinationIp, coapType,
                                         payload, payloadlength,
                                         RAFAEL_NWK_MGM_URL);
            nwk_mgm_printf("kick %s %u \n", string, error);
            if (payload) {
                vPortFree(payload);
            }
        }
    }
    return;
}

void nwk_mgm_neighbor_Change_Callback(
    otNeighborTableEvent aEvent, const otNeighborTableEntryInfo* aEntryInfo) {

    otInstance* instance = otrGetInstance();
    if (instance) {
        otDeviceRole mRole;

        mRole = otThreadGetDeviceRole(instance);

        if (mRole == OT_DEVICE_ROLE_LEADER) {
            if (NULL == nwk_mgm_child_reg_table) {
                printf("malloc table %u \r\n",
                       (sizeof(nwk_mgm_child_reg_table_t)
                        * NWK_MGM_CHILD_REQ_TATLE_SIZE));
                nwk_mgm_child_reg_table = pvPortMalloc(
                    sizeof(nwk_mgm_child_reg_table_t)
                    * NWK_MGM_CHILD_REQ_TATLE_SIZE);
                memset(nwk_mgm_child_reg_table, 0x0,
                       sizeof(nwk_mgm_child_reg_table_t)
                           * NWK_MGM_CHILD_REQ_TATLE_SIZE);
            }
            switch (aEvent) {
                case OT_NEIGHBOR_TABLE_EVENT_CHILD_ADDED:
                    if (nwk_mgm_child_reg_table_add(
                            OT_DEVICE_ROLE_CHILD, otThreadGetRloc16(instance),
                            aEntryInfo->mInfo.mChild.mRloc16,
                            (uint8_t*)aEntryInfo->mInfo.mChild.mExtAddress.m8,
                            aEntryInfo->mInfo.mChild.mAverageRssi)) {
                        nwk_mgm_printf("leader add fail \n");
                        nwk_mgm_kick_child_post(
                            aEntryInfo->mInfo.mChild.mRloc16,
                            otLinkGetPanId(instance),
                            (uint8_t*)aEntryInfo->mInfo.mChild.mExtAddress.m8);
                        otLinkRemoveChildren(instance,
                                             aEntryInfo->mInfo.mChild.mRloc16);
                    }
                    nwk_mgm_router_update_timer_set(
                        NWK_MGM_ROUTER_KEEP_ALIVE_TIME);
                    break;

                case OT_NEIGHBOR_TABLE_EVENT_CHILD_REMOVED:
                    nwk_mgm_printf("leader remove \r\n");
                    nwk_mgm_child_reg_table_remove(
                        OT_DEVICE_ROLE_CHILD,
                        (uint8_t*)aEntryInfo->mInfo.mChild.mExtAddress.m8);
                    break;

                default: break;
            }
        } else if (mRole == OT_DEVICE_ROLE_ROUTER) {
            if (aEvent == OT_NEIGHBOR_TABLE_EVENT_CHILD_ADDED
                || aEvent == OT_NEIGHBOR_TABLE_EVENT_CHILD_REMOVED) {
                nwk_mgm_reg_send_timer_set(10);
            }
            if (nwk_mgm_child_reg_table) {
                vPortFree(nwk_mgm_child_reg_table);
                nwk_mgm_child_reg_table = NULL;
            }
        } else {
            if (nwk_mgm_child_reg_table) {
                vPortFree(nwk_mgm_child_reg_table);
                nwk_mgm_child_reg_table = NULL;
            }
        }
    }
}

static void nwk_mgm_child_register_post_resend() {

    otInstance* instance = otrGetInstance();
    if (instance) {
        otDeviceRole mRole = otThreadGetDeviceRole(instance);

        if (mRole == OT_DEVICE_ROLE_ROUTER) {
            nwk_mgm_child_register_post();
        }
    }
}

static void mgm_router_update_timer_handler() {

    otInstance* instance = otrGetInstance();
    if (instance) {
        otDeviceRole mRole = otThreadGetDeviceRole(instance);

        if (mRole == OT_DEVICE_ROLE_ROUTER) {
            nwk_mgm_child_register_post();
            if (nwk_mgm_child_reg_table) {
                vPortFree(nwk_mgm_child_reg_table);
                nwk_mgm_child_reg_table = NULL;
            }
        } else if (mRole == OT_DEVICE_ROLE_LEADER) {
            otChildInfo childInfo;
            uint16_t childernmax = otThreadGetMaxAllowedChildren(instance);

            for (uint16_t i = 0; i < childernmax; i++) {
                if ((otThreadGetChildInfoByIndex(instance, i, &childInfo)
                     != OT_ERROR_NONE)
                    || childInfo.mIsStateRestoring) {
                    continue;
                }

                nwk_mgm_child_reg_table_add(
                    OT_DEVICE_ROLE_CHILD, otThreadGetRloc16(instance),
                    childInfo.mRloc16, childInfo.mExtAddress.m8,
                    childInfo.mAverageRssi);
            }
        } else {
            if (nwk_mgm_child_reg_table) {
                vPortFree(nwk_mgm_child_reg_table);
                nwk_mgm_child_reg_table = NULL;
            }
        }
        nwk_mgm_router_update_timer_set(NWK_MGM_ROUTER_KEEP_ALIVE_TIME);
    }
}

static void nwk_mgm_timer_handler(TimerHandle_t xTimer) {
    if ((mgm_reg_send_timer > 0 && (--mgm_reg_send_timer == 0))) {
        nwk_mgm_child_register_post_resend();
    }
    if (mgm_router_up_timer > 0 && (--mgm_router_up_timer == 0)) {
        mgm_router_update_timer_handler();
    }

    nwk_mgm_child_reg_table_time_handler();

    xTimerStart(nwk_mgm_timer, 0);
}

otError nwk_mgm_init(otInstance* aInstance) {
    otError error = OT_ERROR_NONE;
    printf("nwk_mgm_init \r\n");
    do {
        memset(&nwk_mgm_resource, 0, sizeof(nwk_mgm_resource));

        nwk_mgm_resource.mUriPath = RAFAEL_NWK_MGM_URL;
        nwk_mgm_resource.mContext = aInstance;
        nwk_mgm_resource.mHandler = &nwk_mgm_request_proccess;
        nwk_mgm_resource.mNext = NULL;

        otCoapAddResource(aInstance, &nwk_mgm_resource);

        if (NULL == nwk_mgm_timer) {
            nwk_mgm_timer = xTimerCreate("nwk_mgm_timer", (1000), false,
                                         (void*)0, nwk_mgm_timer_handler);

            xTimerStart(nwk_mgm_timer, 0);
        } else {
            log_info("nwk_mgm_timer exist\n");
        }
    } while (0);

    return error;
}

static int _cli_cmd_nwk(int argc, char** argv, cb_shell_out_t log_out,
                        void* pExtra) {

    nwk_mgm_child_reg_table_display();
    return 0;
}

const sh_cmd_t g_cli_cmd_nwk STATIC_CLI_CMD_ATTRIBUTE = {
    .pCmd_name = "nwk",
    .pDescription = "network management information",
    .cmd_exec = _cli_cmd_nwk,
};
