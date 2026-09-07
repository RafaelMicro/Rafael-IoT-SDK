#ifndef __APP_OTA_H
#define __APP_OTA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <openthread/ip6.h>
#include <stdio.h>
#include "fota_define.h"

#define OTA_FLASH_START         FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB
#define RAFAEL_OTA_URL_DATA     "ota/data"
#define RAFAEL_OTA_URL_REQ      "ota/req"
#define RAFAEL_OTA_URL_RESP     "ota/resp"
#define RAFAEL_OTA_URL_RXMODE   "ota/rxmode"
#define RAFAEL_OTA_URL_STATUS   "ota/status"
#define RAFAEL_OTA_URL_EXECUTE  "ota/execute"
#define OTA_EVENT_DATA_MAX_SIZE 300
#define OTA_SEGMENTS_MAX_SIZE   256
#define OTA_REQUEST_TABLE_SIZE  10
#define OTA_RESPONSE_TABLE_SIZE 40
#define OTA_REQUEST_TRY_MAX                                                    \
    10 // ~20s worst case (10 * OTA_UNICAST_WAIT_MS) of zero-progress before giving up
#define OTA_UNICAST_WAIT_MS                                                    \
    (2000) // how long requester waits for a batch of 10 unicast responses before \
        // retrying — was 5000ms (5min for 1000 packets), tightened to 2000ms   \
        // (~3.3min for 1000 packets) to match observed round-trip
#define OTA_PHASE1_FIRST_GAP_MS                                                \
    (2500) // extra settle time between segment 0 and segment 1 (multicast only)
#define OTA_PHASE3_TRY_MAX                                                     \
    2 // retries per child before skipping in Phase 3. (TRY_MAX+1)*OTA_PHASE3_CONFIRM_MS = 3*20s = 60s, aligned to the child's OTA_MTD_WAKE_WATCHDOG_TIMEOUT(60s) so a sleepy child that wakes/answers slowly (indirect-queue latency, collided bitmap) is not skipped while still willing to reply. Only governs the wait for the FIRST bitmap; a bitmap resets try_count (proof of life) and hands off to push's OTA_PUSH_ROUND_MAX progress guard. Few long windows over many short ones: the confirm timeout never cuts off an in-flight bitmap, so frequent resends don't recover more children — they just add EXECUTE traffic that collides with the very bitmap reply we wait for.
#define OTA_PHASE2_TRY_MAX                                                     \
    3 // retries per Router before skipping in Phase 2 (Routers are always-on, no sleep watchdog to align with; one lost EXECUTE_PHASE2 must not strand a Router's whole child subtree)
#define OTA_PHASE2_CONFIRM_MS                                                  \
    10000 // per-try confirm window in Phase 2 (was a single 30s shot with no retry)
#define OTA_PHASE3_CONFIRM_MS                                                  \
    20000 // per-try confirm window in Phase 3: one full sleepy-child poll cycle to wake and return its bitmap; count set by OTA_PHASE3_TRY_MAX
#define OTA_PUSH_SEG_GAP_MIN_MS                                                \
    200 // sender: min gap between two pushed segments (randomized MIN..MAX). Fixed 100ms overran the child's absorb rate (measured ~560ms/seg: 27 pushed, 5 landed) and let concurrent routers' cadence re-collide; a random gap de-correlates them and gives flash-write + poll retrieval time.
#define OTA_PUSH_SEG_GAP_MAX_MS                                                \
    500 // sender: max gap between two pushed segments (randomized MIN..MAX)
#define OTA_PUSH_ROUND_MAX                                                     \
    4 // sender: batch-push + re-ask-bitmap rounds per target before giving up on it
#define OTA_BITMAP_CHUNK_MAX                                                   \
    256 // max bitmap bytes carried in one ota_bitmap_report_t (fits OTA_EVENT_DATA_MAX_SIZE)
#define OTA_DONE_TIMEOUT   (3 * 60 * 1000)
#define OTA_DEPORT_TIMEOUT (30 * 1000)
#define OTA_REBOOT_TIMEOUT (1 * 60 * 1000)
#define OTA_DONE_FALLBACK_TIMEOUT                                              \
    (8 * 60 * 1000) // 8min: MTD wait window for EXECUTE_PHASE3_CHILD
#define OTA_PHASE0_PREWAKE_ROUNDS                                              \
    3 // Leader: pre-wake broadcast rounds before Phase 1 (11-hop MPL floods drop packets; one send isn't a safe bet)
#define OTA_PHASE0_PREWAKE_GAP_MS                                              \
    (10                                                                        \
     * 1000) // wait between pre-wake rounds / before Phase 1; must be < the child wake watchdog so a child woken on round 1 doesn't sleep before the multicast opens
#define OTA_MTD_WAKE_WATCHDOG_TIMEOUT                                          \
    (60                                                                        \
     * 1000) // fallback only: woken MTD normally arms a dynamic watchdog = (total_segments - max_seq + 2) * interval so it stays awake the whole Phase1 round; this fixed value is used only if the RXMODE didn't carry total_segments/interval
#define OTA_UNICAST_RECV_WATCHDOG_TIMEOUT                                      \
    (120                                                                       \
     * 1000) // OTA_UNICAST_RECEIVING (tool `ota send` gap-fill): FIXED, reset per received segment. Must NOT reuse the Phase1 dynamic formula (it shrinks as max_seq grows -> ~9s -> a normal inter-round push gap fires it and the device gives up mid-fill). Generous but bounded so an abandoned SED still eventually re-sleeps.

#define OTA_FLASH_SECTOR_SIZE  0x1000 // 4KB
#define OTA_MAX_IMAGE_SECTORS  (SIZE_OF_FOTA_BANK_1MB / OTA_FLASH_SECTOR_SIZE)
#define OTA_INFO_HEADER_OFFSET 0x20 // Firmware offset

typedef enum {
    OTA_IDLE = 0x10,
    OTA_PHASE0_PREWAKE, // Leader: pre-wake broadcast (OTA_PHASE0_PREWAKE_ROUNDS rounds, OTA_PHASE0_PREWAKE_GAP_MS apart) before Phase 1
    OTA_PHASE1_SENDING, // Leader: ff03::1 multicast → all FTD Routers
    OTA_PHASE1_RECEIVING, // FTD Router: receiving Phase 1 data
    OTA_PHASE1_FILLING,   // FTD Router: REQUEST to recover missing Phase 1 segs
    OTA_PHASE2_CONFIRMING, // Leader: per-Router EXECUTE_PHASE2 confirm loop
    OTA_PHASE2_FILLING, // FTD Router: REQUEST/RESPONSE to fill gaps toward Leader
    OTA_PHASE3_CONFIRMING, // FTD Router/Leader: per-child wake + EXECUTE_PHASE3_CHILD confirm loop
    OTA_PHASE3_FILLING, // MTD child: REQUEST/RESPONSE to fill gaps toward Router
    OTA_PHASE3_WAITING, // Router/child: bitmap incomplete, terminal static — no timer, no
    // polling. Only a Leader Phase1 re-run or a Tool `ota send <ipv6>`
    // unicast brings it back in
    OTA_UNICAST_RECEIVING, // Single-device: receiving unicast OTA data
    OTA_DONE,              // Image ready, waiting for Phase 3 or reboot signal
    OTA_REBOOT,            // Rebooting to apply new firmware
    OTA_PHASE3_REPAIRING // Router/Leader: bitmap-driven repair (push of missing segs) of one child; pairs with OTA_PHASE3_FILLING (the child being filled). Appended last to keep wire values stable for the Tool.
} ota_state_t;

typedef enum {
    OTA_PAYLOAD_TYPE_DATA = 0x20,
    OTA_PAYLOAD_TYPE_DATA_ACK,
    OTA_PAYLOAD_TYPE_REQUEST,
    OTA_PAYLOAD_TYPE_RESPONSE,
    OTA_PAYLOAD_TYPE_RXMODE,
    OTA_PAYLOAD_TYPE_BITMAP
} ota_payload_type_t;

typedef enum {
    OTA_DATA_SEND_EVENT = 0x30,
    OTA_REQUEST_SEND_EVENT,
    OTA_RESPONSE_SEND_EVENT,
    OTA_DATA_RECEIVE_EVENT,
    OTA_REQUEST_RECEIVE_EVENT,
    OTA_RESPONSE_RECEIVE_EVENT,
    OTA_RXMODE_RECEIVE_EVENT,
    OTA_STATUS_RECEIVE_EVENT,
    OTA_EXECUTE_RECEIVE_EVENT,
} ota_event_state_t;

typedef uint32_t fotabank_status;
typedef uint8_t fotaimage_info;

typedef struct {
    uint8_t event;
    otIp6Address ipv6;
    uint16_t data_lens;
    uint8_t data[OTA_EVENT_DATA_MAX_SIZE];
} __attribute__((packed)) ota_event_data_t;

typedef struct {
    uint32_t version;
    uint32_t size;
    uint32_t crc;
    uint16_t seq;
    uint16_t segments;
    uint16_t interval;
    bool is_unicast;
    uint8_t data[OTA_SEGMENTS_MAX_SIZE];
} __attribute__((packed)) ota_data_t;

typedef struct {
    uint8_t data_type;
    uint32_t version;
    uint16_t seq;
} __attribute__((packed)) ota_data_ack_t;

typedef struct {
    uint32_t version;
    uint32_t size;
    uint16_t segments;
    uint16_t req_table[OTA_REQUEST_TABLE_SIZE];
} __attribute__((packed)) ota_request_t;

typedef struct {
    uint32_t version;
    uint32_t size;
    uint32_t crc;
    uint16_t seq;
    uint16_t segments;
    uint8_t data[OTA_SEGMENTS_MAX_SIZE];
} __attribute__((packed)) ota_response_t;

typedef struct {
    bool OnWhenIdle;
    uint32_t version;
    uint8_t img_bin_type
        [12]; // this round's image bin_type; child self-filters against it
    uint16_t
        total_segments; // Phase1 total segment count, for child's dynamic sleep calc
    uint16_t
        interval; // Phase1 multicast interval (ms), for child's dynamic sleep calc
} __attribute__((packed)) ota_rxmode_t;

typedef struct {
    uint32_t
        version; // round guard: sender ignores a bitmap from a different image version
    uint16_t total_num; // segment count this bitmap describes
    uint16_t
        byte_offset; // byte offset into the full bitmap (supports chunking when it exceeds one message)
    uint16_t byte_len; // bitmap bytes carried here
    uint8_t data[OTA_BITMAP_CHUNK_MAX];
} __attribute__((packed)) ota_bitmap_report_t;

#define OTA_STATUS_GET      0x40
#define OTA_STATUS_REPORT   0x41
#define OTA_STATUS_TYPE_FTD 0x50
#define OTA_STATUS_TYPE_MTD 0x51
#define OTA_STATUS_TYPE_ALL 0x52

typedef struct {
    uint8_t flag;         //40:get/41:report
    uint8_t device_type;  //50:ftd/51:mtd/52:all
    uint16_t report_time; // millisecond
} __attribute__((packed)) ota_status_get_t;

typedef struct {
    uint8_t flag; //40:get/41:report
    uint32_t version;
    bool rxmode;
    uint8_t state;
    uint8_t progress_bar;
    uint8_t bin_type[12];
    otIp6Address dstipv6; // routing destination (used by timer handler)
    otIp6Address
        src_mleid; // sender's Mesh-Local EID: stable device key + poll address
    uint32_t
        state_remain_ms; // ms left on ota_timer before next state transition (0 = no timer armed)
    uint32_t
        state_enter_tick; // raw xTaskGetTickCount() when this state was entered; Tool ×portTICK_PERIOD_MS, diffs adjacent reports for per-state dwell
    uint16_t
        rloc16; // otThreadGetRloc16(): router id (upper 6 bits) + child id (lower 10 bits) -> reveals parent/child topology
} __attribute__((packed)) ota_status_report_t;

#define OTA_EXECUTE_REBOOT 0x61
#define OTA_EXECUTE_STOP   0x62
#define OTA_EXECUTE_PHASE3                                                     \
    0x63 // [Rafael] Leader→Routers: start Phase 3 (MTD distribution)
#define OTA_EXECUTE_PHASE2                                                     \
    0x64 // [Rafael] Leader→Router: send bitmap, fill missing segs
#define OTA_EXECUTE_PHASE3_CHILD                                               \
    0x65 // [Rafael] Router→MTD child: send bitmap, fill missing segs

#define OTA_PHASE2_ROUTER_MAX 40 // max FTD routers to confirm in Phase 2

typedef struct {
    uint8_t flag;  // 61:reboot / 62:stop / 63-65:phase triggers
    uint16_t time; // millisecond or total_num (Phase3)
    uint8_t
        bin_type[12]; // version(4)+size(4)+crc(4) metadata — Phase3 cold-start
    uint8_t img_bin_type
        [12]; // actual binary bin_type string (e.g. "miu-mtd-r581\0")
} __attribute__((packed)) ota_execute_t;

otError ota_init(otInstance* aInstance,
                 void (*ota_state_change_cb)(uint8_t state));
void ota_stop();
void ota_send_rxmode(bool OnWhenIdle);
void ota_send_rxmode_unicast(bool OnWhenIdle, otIp6Address dst_ipaddr);
void ota_start(uint16_t segments_size, uint16_t interval);
uint8_t ota_get_state();
void ota_bootloader_info_check();
void ota_event_handler();
void ota_bootinfo_reset();
uint8_t ota_reset();
uint32_t ota_get_image_version();
uint32_t ota_get_image_size();
uint32_t ota_get_image_crc();
void ota_flash_read_bin_type(uint8_t out_bin_type[12]);
void ota_set_image_version(uint32_t version);
void ota_set_image_size(uint32_t size);
void ota_set_image_crc(uint32_t crc);
void ota_send(char* ipaddr_str);
void ota_debug_level(unsigned int level);
const char* OtaStateToString(ota_state_t state);
uint32_t crc32checksum(uint32_t flash_addr, uint32_t data_len);
void ota_update_self();
void ota_send_execute(otIp6Address dst_ipaddr, uint8_t execute_flag,
                      uint16_t time);
uint32_t ota_debug_level_get();
void ota_send_status_get(otIp6Address dst_ipaddr, uint8_t status_type,
                         uint16_t report_time);

#ifdef __cplusplus
};
#endif

#endif /* __APP_OTA_H */