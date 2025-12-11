#ifndef APP_CONTROL_CMD_H
#define APP_CONTROL_CMD_H

#include <openthread/ip6.h>
#include <openthread/udp.h>
#include <stdint.h>

/* --- packet format --- */
/* Start | Seq | Flags | CommandID | Length | Data... | CRC */

/* Flags */
#define FLAG_MAC 0x01
#define FLAG_UDP 0x02

#define PKT_START_BYTE 0xAA
#define PKT_MIN_LEN                                                            \
    6 // Start(1)+Seq(1)+Flags(1)+Cmd(1)+Len(1)+CRC(1) when Length==0
#define MAX_DATA_LEN 1200

/* Command IDs (Example, can be expanded as needed) */
typedef enum {
    CMD_ID_LED_ON = 0x01,
    CMD_ID_LED_OFF = 0x02,
    CMD_ID_LED_TOGGLE = 0x03,
    CMD_ID_LED_FLASH = 0x04, /* data: times(1) */
    CMD_ID_PATH_REQUEST = 0x10,
    CMD_ID_PATH_RESPONSE = 0x11,
    CMD_ID_NETWORK_ENROLL_REQ = 0x20,
    CMD_ID_NETWORK_ENROLL_RESP = 0x21,
    CMD_ID_NETWORK_ENROLL_UPDATE = 0x22,
    CMD_ID_NETWORK_PROVISIONING_SET = 0x23,
    CMD_ID_NETWORK_JOIN_REQUEST = 0x24,
    CMD_ID_NETWORK_JOIN_RESPONSE = 0x25,
    CMD_ID_REBOOT = 0x31,
    CMD_ID_GET_VERSION_REQ = 0x32,
    CMD_ID_GET_VERSION_RESP = 0x33,
} command_id_t;

/* packet structure used in handlers */
typedef struct {
    uint8_t start; /* always 0xAA */
    uint8_t seq;
    uint8_t flags;
    uint8_t cmd;
    uint16_t len; /* payload length */
    uint8_t data[MAX_DATA_LEN];
    uint8_t crc;
} __attribute__((packed)) ctrl_packet_t;

#if CONFIG_APP_TASK_CENTRAL_ENABLE
typedef struct {
    uint8_t role;
    uint16_t parent;
    uint16_t self_rloc;
    uint8_t self_extaddr[OT_EXT_ADDRESS_SIZE];
    int8_t rssi;
    uint32_t version;
} __attribute__((packed)) net_mgm_enroll_req_t;

typedef struct {
    int status;
    uint16_t provision_time;
} __attribute__((packed)) net_mgm_enroll_resp_t;

typedef struct {
    uint16_t rloc;
    int8_t rssi;
} __attribute__((packed)) net_mgm_child_update_info_t;

/*only router use */
typedef struct {
    uint16_t parent;
    uint16_t self_rloc;
    int8_t self_rssi_from_parent;
    uint16_t child_cnt;
    net_mgm_child_update_info_t
        children[64]; //follow OPENTHREAD_CONFIG_MLE_MAX_CHILDREN
} __attribute__((packed)) net_mgm_enroll_update_t;
#endif

typedef struct {
    uint16_t panid;
    uint8_t netkey[16];
} __attribute__((packed)) net_join_response_t;

/* handler prototype - srcAddr type is left generic (replace with your otIp6Address type) */
typedef int (*ctrl_cmd_handler_t)(const void* srcAddr,
                                  const ctrl_packet_t* pkt);

/* register table entry */
typedef struct {
    uint8_t cmd_id;
    ctrl_cmd_handler_t handler;
} ctrl_cmd_entry_t;

/* public API */
void app_control_cmd_init(void);

/* send helper: uses your existing low-level app_udpSend(dstAddr, data, len)
   - dstAddr: pointer to address struct (use your otIp6Address *)
   - cmd: CommandID
   - flags: Flags (use FLAG_SET if you want a response)
   - data: pointer to payload (can be NULL if len==0)
   - len: payload length (<= MAX_DATA_LEN)
   - panid: only used if flags is FLAG_MAC, else ignored
   - channel: only used if flags is FLAG_MAC, else ignored
*/
int app_ctrl_send_cmd(uint8_t* dstAddr, uint8_t cmd, uint8_t flags,
                      const uint8_t* data, uint16_t len, uint16_t panid,
                      uint8_t channel);

/* called by your UDP and MAC receive path when UDP datagram arrives
   - srcAddr: pointer to address struct (use your otIp6Address * or uint8_t *)
   - data: received bytes
   - len: received length
*/
int app_ctrl_received(const void* srcAddr, const uint8_t* data, uint16_t len);

#endif // APP_CONTROL_CMD_H
