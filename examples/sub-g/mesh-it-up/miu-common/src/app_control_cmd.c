#include "app_control_cmd.h"
#include <FreeRTOS.h>
#include <stdio.h>
#include <string.h>
#include <timers.h>
#include "app_led.h"
#include "app_mac_raw.h"
#include "app_net_mgm.h"
#include "app_task.h"
#include "app_udp.h"
#include "log.h"
#include "main.h"

#include <openthread/thread.h>

#if CONFIG_APP_TASK_CONTROL_CMD_ENABLE
/* Sequence generator */
static uint8_t s_seq = 0;

static uint8_t next_seq(void) { return ++s_seq; }

/* CRC = XOR of bytes (from Start up to last data byte) */
static uint8_t calc_crc(const uint8_t* buf, uint16_t len_no_crc) {
    uint8_t crc = 0;
    for (uint16_t i = 0; i < len_no_crc; ++i)
        crc ^= buf[i];
    return crc;
}

/* Build and send raw packet */
int app_ctrl_send_cmd(uint8_t* dstAddr, uint8_t cmd, uint8_t flags,
                      const uint8_t* data, uint16_t len, uint16_t panid,
                      uint8_t channel) {
    if (len > MAX_DATA_LEN)
        return -1;
    static uint8_t
        buf[7 + MAX_DATA_LEN]; // Start+Seq+Flags+Cmd+Len(2) + data + CRC
    uint16_t idx = 0;
    buf[idx++] = PKT_START_BYTE;
    buf[idx++] = next_seq();
    buf[idx++] = flags;
    buf[idx++] = cmd;
    buf[idx++] = (len >> 8) & 0xFF;
    buf[idx++] = len & 0xFF;
    if (len && data) {
        memcpy(&buf[idx], data, len);
        idx += len;
    }
    /* compute CRC over all except CRC byte */
    uint8_t crc = calc_crc(buf, idx);
    buf[idx++] = crc;
    int rc = 0;
    if (flags == FLAG_MAC) {
        /* call lower-level MAC raw send */
        rc = app_macRawSend(dstAddr, buf, idx, panid, channel, false);
    } else {
        /* call lower-level UDP send */
        /* NOTE: app_udpSend must be implemented by you to send the raw bytes to dstAddr */
        rc = app_udpSend(*(otIp6Address*)dstAddr, buf, idx, false);
    }
    return rc;
}

// === Handler control commands ===
static int cmd_led_on_handler(const void* srcAddr, const ctrl_packet_t* pkt) {
    app_set_led0_on();
    log_info("LED ON\r\n");
}

static int cmd_led_off_handler(const void* srcAddr, const ctrl_packet_t* pkt) {
    app_set_led0_off();
    log_info("LED OFF\r\n");
}

static int cmd_led_toggle_handler(const void* srcAddr,
                                  const ctrl_packet_t* pkt) {
    app_set_led0_toggle();
    log_info("LED TOGGLE\r\n");
}

static int cmd_led_flash_handler(const void* srcAddr,
                                 const ctrl_packet_t* pkt) {
    app_set_led0_flash();
    log_info("LED FLASH\r\n");
}

static int cmd_path_request_handler(const void* srcAddr,
                                    const ctrl_packet_t* pkt) {}

static int cmd_path_response_handler(const void* srcAddr,
                                     const ctrl_packet_t* pkt) {}

static int cmd_network_enroll_request_handler(const void* srcAddr,
                                              const ctrl_packet_t* pkt) {
#if CONFIG_MIU_DEVICE_TYPE_FTD
    if (pkt->len != sizeof(net_mgm_enroll_req_t)) {
        log_info("Invalid network enroll request length %u\r\n", pkt->len);
        return -1;
    }
    otIp6Address dst_addr = *(otIp6Address*)srcAddr;
    net_mgm_enroll_req_t* enroll_req = (net_mgm_enroll_req_t*)pkt->data;
    char string[OT_IP6_ADDRESS_STRING_SIZE];
    otIp6AddressToString(&dst_addr, string, sizeof(string));
    log_info("[Network] << Enroll Request %s [%s,rssi=%d, "
             "ver=%x]",
             string, otThreadDeviceRoleToString(enroll_req->role),
             enroll_req->rssi, enroll_req->version);
    net_mgm_node_table_t net_mgm_node_info;
    memset(&net_mgm_node_info, 0, sizeof(net_mgm_node_info));
    net_mgm_node_info.role = enroll_req->role;
    net_mgm_node_info.parent = enroll_req->parent;
    net_mgm_node_info.rloc = enroll_req->self_rloc;
    memcpy(net_mgm_node_info.extaddr, enroll_req->self_extaddr,
           OT_EXT_ADDRESS_SIZE);
    net_mgm_node_info.rssi = enroll_req->rssi;
    net_mgm_node_info.version = enroll_req->version;
    int status = net_mgm_node_table_add(&net_mgm_node_info);

    // Send the enrollment response
    net_mgm_enroll_resp_t enroll_resp;
    enroll_resp.status = status;
    enroll_resp.provision_time = app_provision_get_remain_time();
    if (app_ctrl_send_cmd((uint8_t*)&dst_addr, CMD_ID_NETWORK_ENROLL_RESP,
                          FLAG_UDP, (uint8_t*)&enroll_resp, sizeof(enroll_resp),
                          0, 0)) {
        log_info("app_ctrl_send_cmd enrollment response fail");
    }
    log_info("[Network] >> Enroll Response %s [%s]", string,
             enroll_resp.status == 0 ? "success" : "fail");
#endif
    return 0;
}

static int cmd_network_enroll_response_handler(const void* srcAddr,
                                               const ctrl_packet_t* pkt) {
    if (pkt->len != sizeof(net_mgm_enroll_resp_t)) {
        log_info("Invalid network enroll response length %u\r\n", pkt->len);
        return -1;
    }
    net_mgm_enroll_resp_t* enroll_resp = (net_mgm_enroll_resp_t*)pkt->data;
    char string[OT_IP6_ADDRESS_STRING_SIZE];
    otIp6AddressToString((otIp6Address*)srcAddr, string, sizeof(string));
    log_info("[Network] << Enroll Response %s [%s]", string,
             enroll_resp->status == 0 ? "success" : "fail");
    net_mgm_enroll_req_received_done(enroll_resp->status,
                                     enroll_resp->provision_time);
}

static int cmd_network_enroll_update_handler(const void* srcAddr,
                                             const ctrl_packet_t* pkt) {
#if CONFIG_MIU_DEVICE_TYPE_FTD
    const size_t kHeaderSize = offsetof(net_mgm_enroll_update_t, children);
    if (pkt->len < kHeaderSize) {
        log_info("Invalid network enroll update length %u %u\r\n", pkt->len,
                 kHeaderSize);
        return -1;
    }
    otIp6Address dst_addr = *(otIp6Address*)srcAddr;
    const uint8_t* payload_ptr = (const uint8_t*)pkt->data;
    uint16_t parent_rloc;
    uint16_t self_rloc;
    int8_t self_rssi_from_parent;
    uint8_t child_count;
    char string[OT_IP6_ADDRESS_STRING_SIZE];

    memcpy(&parent_rloc, payload_ptr, sizeof(uint16_t));
    memcpy(&self_rloc, payload_ptr + sizeof(uint16_t), sizeof(uint16_t));
    memcpy(&self_rssi_from_parent, payload_ptr + (2 * sizeof(uint16_t)),
           sizeof(int8_t));
    memcpy(&child_count, payload_ptr + (2 * sizeof(uint16_t) + sizeof(int8_t)),
           sizeof(uint8_t));

    size_t children_list_size = child_count
                                * sizeof(net_mgm_child_update_info_t);
    size_t expected_len = kHeaderSize + children_list_size;

    if (pkt->len != expected_len) {
        log_info("Invalid update length %u: Expected %zu for %u children.\r\n",
                 pkt->len, expected_len, child_count);
        return -1;
    }
    otIp6AddressToString(&dst_addr, string, sizeof(string));
    log_info("[Network] << Enroll Update %s [%d] ", string, child_count);

    /*update router survival time*/
    net_mgm_node_survivaltime_update(OT_DEVICE_ROLE_ROUTER, parent_rloc,
                                     self_rloc, self_rssi_from_parent);
    const net_mgm_child_update_info_t* children_ptr =
        (const net_mgm_child_update_info_t*)(payload_ptr + kHeaderSize);

    for (uint8_t i = 0; i < child_count; i++) {
        uint16_t child_rloc = children_ptr[i].rloc;
        int8_t child_rssi = children_ptr[i].rssi;
        net_mgm_node_survivaltime_update(OT_DEVICE_ROLE_CHILD, self_rloc,
                                         child_rloc, child_rssi);
    }

    // Send the enrollment response
    net_mgm_enroll_resp_t enroll_resp;
    enroll_resp.status = 0;
    enroll_resp.provision_time = app_provision_get_remain_time();
    if (app_ctrl_send_cmd((uint8_t*)&dst_addr, CMD_ID_NETWORK_ENROLL_RESP,
                          FLAG_UDP, (uint8_t*)&enroll_resp, sizeof(enroll_resp),
                          0, 0)) {
        log_info("app_ctrl_send_cmd enrollment response fail");
    }
    log_info("[Network] >> Enroll Response %s [%s]", string,
             enroll_resp.status == 0 ? "success" : "fail");
#endif
    return 0;
}

static int cmd_network_provisioning_set_handler(const void* srcAddr,
                                                const ctrl_packet_t* pkt) {
#if CONFIG_MIU_DEVICE_TYPE_FTD
    if (pkt->len != sizeof(uint32_t)) {
        log_info("Invalid network provisioning set length %u\r\n", pkt->len);
        return -1;
    }
    uint32_t provision_time = *(uint32_t*)pkt->data;
    log_info("Set network provisioning time %u seconds\r\n", provision_time);

    if (provision_time > 0) {
        app_set_provisioning_mode(true, provision_time);
    } else {
        app_set_provisioning_mode(false, 0);
    }
#endif
    return 0;
}

static int cmd_network_join_request_handler(const void* srcAddr,
                                            const ctrl_packet_t* pkt) {
#if CONFIG_MIU_DEVICE_TYPE_FTD
    uint8_t mac_addr[OT_EXT_ADDRESS_SIZE];
    memcpy(mac_addr, srcAddr, OT_EXT_ADDRESS_SIZE);
    log_info("[Network] << Join Request [%02X%02X%02X%02X%02X%02X%02X%02X] ",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4],
             mac_addr[5], mac_addr[6], mac_addr[7]);
    app_join_request_handler();
#endif
    return 0;
}

static int cmd_network_join_response_handler(const void* srcAddr,
                                             const ctrl_packet_t* pkt) {
    if (pkt->len != sizeof(net_join_response_t)) {
        log_info("Invalid network join response length %u\r\n", pkt->len);
        return -1;
    }
    uint8_t mac_addr[OT_EXT_ADDRESS_SIZE];
    memcpy(mac_addr, srcAddr, OT_EXT_ADDRESS_SIZE);
    log_info("[Network] << Join Response [%02X%02X%02X%02X%02X%02X%02X%02X] ",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4],
             mac_addr[5], mac_addr[6], mac_addr[7]);
    net_join_response_t* join_response = (net_join_response_t*)pkt->data;
    app_join_response_handler(join_response->panid, join_response->netkey);
    return 0;
}

static TimerHandle_t kick_levave_timer;

static void kick_levave_timer_cb(TimerHandle_t xTimer) {
    otInstanceFactoryReset(otrGetInstance());
    xTimerDelete(kick_levave_timer, 0);
}

static int cmd_network_kick_handler(const void* srcAddr,
                                    const ctrl_packet_t* pkt) {
    if (pkt->len != sizeof(uint32_t)) {
        log_info("Invalid network kick length %u\r\n", pkt->len);
        return -1;
    }
    uint32_t leave_time = *(uint32_t*)pkt->data;
    char string[OT_IP6_ADDRESS_STRING_SIZE];
    otIp6AddressToString((otIp6Address*)srcAddr, string, sizeof(string));
    log_info("[Network] << kick %s [%d]", string, leave_time);

    /*chage to disable*/
    otThreadSetEnabled(otrGetInstance(), false);

    if (kick_levave_timer == NULL) {
        kick_levave_timer = xTimerCreate("kick_levave_timer",
                                         pdMS_TO_TICKS(leave_time * 1000),
                                         pdFALSE, // one-shot
                                         NULL, kick_levave_timer_cb);
    }

    if (kick_levave_timer != NULL) {
        xTimerChangePeriod(kick_levave_timer, pdMS_TO_TICKS(leave_time * 1000),
                           0);
        xTimerStart(kick_levave_timer, 0);
    }
    return 0;
}

static int cmd_get_version_request_handler(const void* srcAddr,
                                           const ctrl_packet_t* pkt) {}

static int cmd_get_version_response_handler(const void* srcAddr,
                                            const ctrl_packet_t* pkt) {}

static TimerHandle_t reboot_timer;

static void reboot_timer_cb(TimerHandle_t xTimer) {
    uint8_t is_erase = (uint8_t)(uintptr_t)pvTimerGetTimerID(xTimer);

    if (is_erase == 1) {
        otInstanceFactoryReset(otrGetInstance());
        log_info("Executing factory reset (otInstanceFactoryReset)...\r\n");
    } else {
        // Perform a normal reset/reboot
        otInstanceReset(otrGetInstance());
        log_info("Executing normal reboot (otInstanceReset)...\r\n");
    }
}

static int cmd_reboot_handler(const void* srcAddr, const ctrl_packet_t* pkt) {
    if (pkt->len != 2) {
        log_info("Invalid reboot command length %u\r\n", pkt->len);
        return -1;
    }
    uint8_t is_erase = pkt->data[0];
    uint8_t delay_sec = pkt->data[1];
    log_info("Rebooting in %u seconds...\r\n", delay_sec);
    if (delay_sec == 0) {
        delay_sec = 1; // minimum delay 1 second
    }
    if (reboot_timer == NULL) {
        reboot_timer = xTimerCreate(
            "rebootTimer", pdMS_TO_TICKS(delay_sec * 1000),
            pdFALSE, // one-shot
            (void*)(uintptr_t)is_erase, reboot_timer_cb);
    }

    if (reboot_timer != NULL) {
        vTimerSetTimerID(reboot_timer, (void*)(uintptr_t)is_erase);
        xTimerChangePeriod(reboot_timer, pdMS_TO_TICKS(delay_sec * 1000), 0);
        xTimerStart(reboot_timer, 0);
    }

    return 0;
}

// === Command Table ===
static const ctrl_cmd_entry_t s_cmd_table[] = {
    {CMD_ID_LED_ON, cmd_led_on_handler},
    {CMD_ID_LED_OFF, cmd_led_off_handler},
    {CMD_ID_LED_TOGGLE, cmd_led_toggle_handler},
    {CMD_ID_LED_FLASH, cmd_led_flash_handler},
    {CMD_ID_PATH_REQUEST, cmd_path_request_handler},
    {CMD_ID_PATH_RESPONSE, cmd_path_response_handler},
    {CMD_ID_NETWORK_ENROLL_REQ, cmd_network_enroll_request_handler},
    {CMD_ID_NETWORK_ENROLL_RESP, cmd_network_enroll_response_handler},
    {CMD_ID_NETWORK_ENROLL_UPDATE, cmd_network_enroll_update_handler},
    {CMD_ID_NETWORK_PROVISIONING_SET, cmd_network_provisioning_set_handler},
    {CMD_ID_NETWORK_JOIN_REQUEST, cmd_network_join_request_handler},
    {CMD_ID_NETWORK_JOIN_RESPONSE, cmd_network_join_response_handler},
    {CMD_ID_NETWORK_NODE_KICK, cmd_network_kick_handler},
    {CMD_ID_GET_VERSION_REQ, cmd_get_version_request_handler},
    {CMD_ID_GET_VERSION_RESP, cmd_get_version_response_handler},
    {CMD_ID_REBOOT, cmd_reboot_handler},
};

/* helper to dispatch */
static void dispatch_cmd(const void* srcAddr, const ctrl_packet_t* pkt) {
    for (size_t i = 0; i < sizeof(s_cmd_table) / sizeof(s_cmd_table[0]); ++i) {
        if (s_cmd_table[i].cmd_id == pkt->cmd) {
            s_cmd_table[i].handler(srcAddr, pkt);
            return;
        }
    }
    log_info("[dispatch_cmd] unknown cmd: 0x%02X\n", pkt->cmd);
}

/* public init */
void app_control_cmd_init(void) { s_seq = 0; }

/* parse incoming UDP And MAC Raw bytes and dispatch */
int app_ctrl_received(const void* srcAddr, const uint8_t* data, uint16_t len) {
    if (len < PKT_MIN_LEN) {
        log_info("[app_ctrl_received] short pkt len=%u\n", len);
        return -1;
    }

    /* basic parse */
    if (data[0] != PKT_START_BYTE) {
        log_info("[app_ctrl_received] bad start: 0x%02X\n", data[0]);
        return -1;
    }

    uint8_t recv_crc = data[len - 1];
    uint8_t calc = calc_crc(data, len - 1);
    if (recv_crc != calc) {
        log_info("[app_ctrl_received] crc mismatch (recv=%02X calc=%02X)\n",
                 recv_crc, calc);
        return -1;
    }

    static ctrl_packet_t pkt;
    memset(&pkt, 0, sizeof(pkt));
    pkt.start = data[0];
    pkt.seq = data[1];
    pkt.flags = data[2];
    pkt.cmd = data[3];
    pkt.len = ((data[4] << 8) & 0xFF00) | data[5];

    if (pkt.len > MAX_DATA_LEN) {
        log_info("[app_ctrl_received] payload too big %u\n", pkt.len);
        return -1;
    }
    if (pkt.len > 0) {
        memcpy(pkt.data, &data[6], pkt.len);
    }
    pkt.crc = recv_crc;

    /* dispatch normal command */
    dispatch_cmd(srcAddr, &pkt);
    return 0;
}

#endif // CONFIG_APP_TASK_CONTROL_CMD_ENABLE