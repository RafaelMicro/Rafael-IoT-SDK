/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */
#include <openthread/coap.h>
#include <openthread/logging.h>
#include <openthread/platform/misc.h>
#include <openthread/random_noncrypto.h>
#include <openthread/thread_ftd.h>
#include "uart.h"

#include "FreeRTOS.h"
#include "app_ota.h"
#include "app_uart.h"
#include "cli.h"
#include "fota_define.h"
#include "hosal_flash.h"
#include "log.h"
#include "main.h"
#include "miu_bin_version.h"
#include "miu_ext_mem.h"
#include "queue.h"
#include "task.h"
#include "timers.h"
#include "util_string.h"

#include <string.h>

#if CONFIG_APP_TASK_OTA_ENABLE
// ota_data_t (packed, data[OTA_SEGMENTS_MAX_SIZE]) must fit inside ota_event_data_t.data
_Static_assert(sizeof(ota_data_t) <= OTA_EVENT_DATA_MAX_SIZE,
               "ota_data_t too large for event queue buffer; reduce "
               "OTA_SEGMENTS_MAX_SIZE or increase OTA_EVENT_DATA_MAX_SIZE");

unsigned int ota_debug_flags = 0;
#define ota_printf(args...)                                                    \
    do {                                                                       \
        if (ota_debug_flags > 0)                                               \
            log_info(args);                                                    \
    } while (0);

#define IS_VALID_PTR(ptr)                                                      \
    ((ptr) != NULL && (ptr) != (void*)0xFFFFFFFF                               \
     && (((uint32_t)(ptr) & 0x3) == 0))

static void ota_change_state_and_timer(uint8_t state, uint32_t timeout);
static void ota_request_start(void);
static void ota_bootinfo_ready(void);
static void ota_rxmode_sended_event_handler(otIp6Address dst_ipaddr,
                                            uint8_t* data, uint16_t lens);
static void ota_execute_sended_event_handler(otIp6Address dst_ipaddr,
                                             uint8_t* data, uint16_t lens);
static void (*ota_state_change_cb_signal)(uint8_t state) = NULL;
static void ota_status_sended_event_handler(otIp6Address dst_ipaddr,
                                            uint8_t* data, uint16_t lens);
static void ota_push_status(void);
static void ota_push_status_to(otIp6Address dst);
static void ota_execute_phase2_respond(otIp6Address leader_addr);
static void ota_execute_phase3_child_respond(otIp6Address router_addr,
                                             const ota_execute_t* exec);
#if CONFIG_MIU_DEVICE_TYPE_FTD
static void ota_phase2_start(void);
static void ota_phase2_send_trigger(void);
static void ota_phase2_on_loop_done_impl(void* arg);
static void ota_phase2_on_loop_done(void);
static void ota_phase2_grace_timer_post(void* arg);
static void ota_phase2_grace_timer_cb(TimerHandle_t xTimer);
static void ota_phase3_start(void);
static void ota_prewake_sleeping_children(const uint8_t img_bin_type[12],
                                          uint32_t round_version);
static void ota_phase0_prewake_broadcast(void);
static void ota_phase3_send_trigger(void);
static void ota_phase3_finish_ftd(void);
static void ota_phase3_on_loop_done_impl(void* arg);
static void ota_phase3_on_loop_done(void);
static void ota_phase3_grace_timer_post(void* arg);
static void ota_phase3_grace_timer_cb(TimerHandle_t xTimer);
#endif
static xQueueHandle ota_event_queue;

// Timer callbacks run on the FreeRTOS timer-service task; these _post bodies do the real OT work and are marshalled onto the OT app task (runs under otrLock), same pattern as the OT ms-alarm and the grace timers. Calling OT/radio APIs straight from a timer callback races the OT task's radio/TX path.
static void ota_timer_post(void* arg);
static void ota_response_timer_post(void* arg);
static void ota_report_timer_post(void* arg);
static void ota_sleep_timer_post(void* arg);

static TimerHandle_t ota_timer;
static TimerHandle_t ota_resp_timer;
static TimerHandle_t ota_report_timer;
static TimerHandle_t
    ota_sleep_timer; // deferred sleepy Child Update (staggered), replaces a vTaskDelay that froze the OT ms-alarm on the timer-service task
static otIp6Address ota_invalid_addr;
otIp6Address ota_sender_addr;
otCoapResource ota_data_resource;
otCoapResource ota_req_resource;
otCoapResource ota_resp_resource;
otCoapResource ota_rxmode_resource;
otCoapResource ota_status_resource;
otCoapResource ota_execute_resource;
const char ota_data_Uri_Path[] = RAFAEL_OTA_URL_DATA;
const char ota_req_Uri_Path[] = RAFAEL_OTA_URL_REQ;
const char ota_resp_Uri_Path[] = RAFAEL_OTA_URL_RESP;
const char ota_rxmode_Uri_Path[] = RAFAEL_OTA_URL_RXMODE;
const char ota_status_Uri_Path[] = RAFAEL_OTA_URL_STATUS;
const char ota_execute_Uri_Path[] = RAFAEL_OTA_URL_EXECUTE;
static volatile uint8_t g_ota_state = OTA_IDLE;
static volatile uint32_t g_state_enter_tick =
    0; // raw tick stamped on each state entry; reported for per-state timing stats
static uint32_t* g_ota_bitmap = NULL;
static uint8_t g_ota_request_cnt = 0;
static uint8_t g_ota_unicast_try_cnt = 0;
static uint32_t g_ota_request_last_remain = 0;
static uint16_t g_ota_segments_size = 0;
static uint16_t g_ota_data_interval = 0;
static uint32_t g_ota_image_version = 0;
static uint32_t g_ota_image_size = 0;
static uint32_t g_ota_image_crc = 0;
static uint32_t g_ota_total_num = 0;
static uint32_t g_ota_start_index = 0;
static uint32_t g_ota_last_index = 0;
static bool g_have_data_ack = false;
#if CONFIG_MIU_DEVICE_TYPE_FTD
// Sender-side: the currently-served child's bitmap (bit==1 has, bit==0 missing); push walks its zeros. Freed per child and on OTA_IDLE.
static uint32_t* g_child_bitmap = NULL;
static uint16_t g_child_bitmap_total = 0;
// Batch-push round bookkeeping per target: one round = push every missing seg once, then re-ask for a fresh bitmap.
static uint16_t g_push_round_rloc = 0xFFFF;
static uint8_t g_push_round = 0;
static uint16_t g_push_last_missing =
    0xFFFF; // current target's missing count last round; a round that shrinks it resets g_push_round (progress refresh), so only stalled rounds count toward giving up
#endif
static bool bin_type_matched = true;
// Highest Phase1 DATA seq seen this round; tracks max (not latest) so an out-of-order dup can't shorten the recomputed pre-wake window
static uint16_t g_phase1_max_seq_seen = 0;
static otIp6Address g_ota_push_target;
// Phase 2/3 unicast redirection: g_unicast_req_dst (receiver's REQUEST target), g_unicast_resp_dst (sender's RESPONSE target), in place of ff02::1
static otIp6Address g_unicast_req_dst;
static otIp6Address g_unicast_resp_dst;
// Whether this device was actually rx-off (SED) when woken; only set true by a real wake observation, so rxmode is only restored if we know it was truly asleep
static bool g_mtd_was_sleepy = false;
// Distinguishes the two callers that arm OTA_PHASE0_PREWAKE (round initiator vs. a woken receiver), since both look identically awake by the time the timer fires
static bool g_ota_prewake_woken = false;
#if CONFIG_MIU_DEVICE_TYPE_FTD
// Leader-only: how many pre-wake rounds still to send before Phase 1. Reset in ota_start(), counted down in the OTA_PHASE0_PREWAKE timer branch. 11-hop MPL floods drop packets, so one broadcast isn't a safe bet; resend OTA_PHASE0_PREWAKE_ROUNDS times before opening the multicast.
static uint8_t g_prewake_rounds_left = 0;
#endif

#if CONFIG_MIU_DEVICE_TYPE_FTD
// Phase 2: Leader confirms each FTD Router via unicast after multicast Phase 1
typedef struct {
    bool active;
    uint8_t count;
    uint8_t current;
    uint8_t
        try_count; // confirm retries spent on the current Router (see OTA_PHASE2_TRY_MAX)
    uint16_t rlocs[OTA_PHASE2_ROUTER_MAX];
    uint32_t image_version;
    uint32_t image_size;
    uint32_t image_crc;
} ota_phase2_ctx_t;

static ota_phase2_ctx_t g_phase2;

// Phase 3: Router confirms each child via unicast; `pending`/`done` are bitmaps over live child-table index, not a fixed rloc array
typedef struct {
    bool active;
    uint16_t max_children; // child-table capacity this round's bitmaps cover
    uint16_t total;        // children snapshotted this round (logging only)
    uint16_t remaining;    // pending && !done count
    uint16_t current;      // child-table index currently targeted
    uint16_t
        current_rloc; // rloc at trigger-send time; confirm matches this, not a re-query, since the child-table index can shift before the reply arrives
    uint8_t try_count;
    uint32_t* pending;
    uint32_t* done;
    uint32_t image_version;
    uint32_t image_size;
    uint32_t image_crc;
} ota_phase3_ctx_t;

static ota_phase3_ctx_t g_phase3;

// Grace Re-scan: after the Phase2/Phase3 confirm loop finishes, wait then sweep the target table once more (Phase2: Routers that attached after the snapshot; Phase3: the whole child table again — already-done children reply 100% and cost nothing)
#define OTA_GRACE_RESCAN_MAX      3     // max wait-and-rescan rounds
#define OTA_GRACE_RESCAN_DELAY_MS 12000 // ms to wait before each rescan

// Sentinel for g_phase3.current_rloc when no live target rloc is cached (0xfffe unused in Thread RLOC16 allocation)
#define OTA_INVALID_RLOC 0xfffe

// OTA_EXECUTE_REBOOT is NON_CONFIRMABLE with no ack, so send it a few times per Router; duplicates are harmless (receiver reboots once)
#define OTA_EXECUTE_REBOOT_RETRY 3
// Fix A: Leader re-blasts the Router reboot batch a bounded number of rounds so a
// Router that dropped the first copy still reboots. Window = ROUNDS * GAP (6*10s=60s).
#define OTA_REBOOT_RESEND_ROUNDS 6
#define OTA_REBOOT_RESEND_GAP_MS (10 * 1000)
static TimerHandle_t g_reboot_resend_timer = NULL;
static uint8_t g_reboot_resend_left = 0;
static TimerHandle_t g_phase2_grace_timer = NULL;
static uint8_t g_phase2_grace_round = 0;
static TimerHandle_t g_phase3_grace_timer = NULL;
static uint8_t g_phase3_grace_round = 0;
// True from the moment the main loop ends until the last grace re-scan settles; the round isn't over yet even though g_phase3.active is false during the wait
static bool g_phase3_grace_pending = false;
// bin_type_matched snapshot at this round's start; OTA_IDLE resets the live global to true, so a dip through Idle would corrupt it if read live
static bool g_phase3_bin_type_matched_for_round = false;
// EXECUTE_REBOOT arrived while a grace re-scan was still pending; remember it and honor it once Phase3 really settles
static bool g_phase3_reboot_pending = false;
static uint16_t g_phase3_reboot_pending_time_ms = 0;
#endif
static const uint8_t g_ota_response_k = 2;
static uint8_t g_ota_response_c = 0;
static uint16_t g_resp_table[OTA_RESPONSE_TABLE_SIZE];
ota_status_report_t g_ota_status_report;

uint32_t crc32checksum(uint32_t flash_offset, uint32_t data_len) {
    uint32_t chkSum = ~0;
    uint8_t page_buf[256] __attribute__((aligned(4)));
    uint32_t processed_len = 0;

    while (processed_len < data_len) {
        // flash_read_page reads a 256-byte-aligned page; derive page base and in-page offset from the address
        uint32_t cur_addr = flash_offset + processed_len;
        uint32_t page_base = cur_addr & ~0xFFu;
        uint32_t page_off = cur_addr - page_base;
        uint32_t available = 256u - page_off;
        uint32_t read_len = ((data_len - processed_len) > available)
                                ? available
                                : (data_len - processed_len);

        if (hosal_flash_read(HOSAL_FLASH_READ_PAGE, page_base, page_buf) != 0) {
            return 0;
        }

        for (uint32_t i = 0; i < read_len; i++) {
            chkSum ^= page_buf[page_off + i];
            for (uint16_t k = 0; k < 8; k++) {
                chkSum = (chkSum & 1) ? (chkSum >> 1) ^ 0xedb88320
                                      : (chkSum >> 1);
            }
        }
        processed_len += read_len;
    }

    return ~chkSum;
}

/*bit map*/
uint32_t* ota_bitmap_init(uint32_t lens) {
    uint16_t bitmap_size;
    uint32_t* bitmap = NULL;
    bitmap_size = (lens >> 5);
    if (lens % 32) {
        ++bitmap_size;
    }

    ota_printf("lens %d bitmap_size %d ", lens, bitmap_size);
    bitmap_size *= sizeof(uint32_t);

    bitmap = xMalloc(bitmap_size);
    if (!bitmap) {
        log_error("[FOTA] bitmap xMalloc failed, size=%u", bitmap_size);
        return NULL;
    }
    memset(bitmap, 0x0, bitmap_size);
    return bitmap;
}

void ota_bitmap_set(uint32_t* bitmap, uint32_t index) {
    if (!IS_VALID_PTR(bitmap)) {
        log_error("[FOTA] Invalid bitmap pointer in set: %p", bitmap);
        return;
    }
    enter_critical_section();
    uint32_t tmp_index = 0, bit = 0;
    tmp_index = index >> 5;
    bit = index % 32;

    bitmap[tmp_index] |= 0x1 << bit;
    leave_critical_section();
}

// Zero an existing bitmap in place (keeps the allocation) — used by the Phase3 grace re-scan to rebuild its target set without churning the heap.
void ota_bitmap_clear_all(uint32_t* bitmap, uint32_t lens) {
    if (!IS_VALID_PTR(bitmap)) {
        log_error("[FOTA] Invalid bitmap pointer in clear_all: %p", bitmap);
        return;
    }
    uint16_t words = (lens >> 5) + ((lens % 32) ? 1 : 0);
    enter_critical_section();
    memset(bitmap, 0x0, words * sizeof(uint32_t));
    leave_critical_section();
}

void ota_bitmap_delete(uint32_t* bitmap) {
    if (bitmap == NULL) {
        return;
    }
    if (!IS_VALID_PTR(bitmap)) {
        log_error("[FOTA] Invalid bitmap pointer in delete: %p", bitmap);
        return;
    }
    xFree(bitmap);
}

uint32_t ota_bitmap_get_bit(uint32_t* bitmap, uint32_t index) {
    if (!IS_VALID_PTR(bitmap)) {
        log_error("[FOTA] Invalid bitmap pointer in get_bit: %p", bitmap);
        ota_change_state_and_timer(OTA_IDLE, 0);
        return 0;
    }
    uint32_t tmp_index = index >> 5, bit = 0;

    bit = index % 32;
    return (bitmap[tmp_index] & (0x1 << bit));
}

uint32_t ota_bitmap_get_remain(uint32_t* bitmap, uint32_t lens) {
    if (!IS_VALID_PTR(bitmap)) {
        log_error("[FOTA] Invalid bitmap pointer in get_remain: %p", bitmap);
        ota_change_state_and_timer(OTA_IDLE, 0);
        return 0;
    }
    uint32_t tmp_lens = lens >> 5, i = 0, j = 0, remain = 0;
    if (lens % 32) {
        ++tmp_lens;
    }

    for (i = 0; i < tmp_lens; i++) {
        if ((i == (tmp_lens - 1) && (lens % 32) != 0)) {
            for (j = 0; j < (lens % 32); j++) {
                if ((bitmap[i] & (0x1 << j)) == 0) {
                    ++remain;
                }
            }
        } else {
            for (j = 0; j < 32; j++) {
                if ((bitmap[i] & (0x1 << j)) == 0) {
                    ++remain;
                }
            }
        }
    }
    return remain;
}

void ota_bitmap_print(uint32_t* bitmap, uint32_t lens) {
    if (!IS_VALID_PTR(bitmap)) {
        log_error("[FOTA] Invalid bitmap pointer in print: %p", bitmap);
        return;
    }

    uint32_t tmp_lens = lens >> 5, i = 0;
    if (lens % 32) {
        ++tmp_lens;
    }

    for (i = 0; i < tmp_lens; i++) {
        ota_printf("%X ", bitmap[i]);
    }
    ota_printf("");
}

static void ota_change_state_and_timer(uint8_t state, uint32_t timeout) {
    xTimerStop(ota_timer, 0);
    if (state == OTA_IDLE) {
        ota_bitmap_delete(g_ota_bitmap);
        g_ota_bitmap = NULL;
        g_ota_segments_size = 0;
        g_ota_data_interval = 0;
        g_ota_total_num = 0;
        g_ota_start_index = 0;
        g_ota_request_cnt = 0;
        g_ota_unicast_try_cnt = 0;
        g_ota_request_last_remain = 0;
        g_ota_last_index = 0;
        g_have_data_ack = false;
        bin_type_matched = true;
        g_phase1_max_seq_seen = 0;
        // Central reset so a stale "woken" state from a previous round can't leak into this one, regardless of which call site starts it.
        g_ota_prewake_woken = false;
        // Same reasoning: a stale "was sleepy" flag must not survive into a fresh round — only this round's own wake observation may set it true.
        g_mtd_was_sleepy = false;
        memset(&g_unicast_req_dst, 0, sizeof(g_unicast_req_dst));
        memset(&g_unicast_resp_dst, 0, sizeof(g_unicast_resp_dst));
#if CONFIG_MIU_DEVICE_TYPE_FTD
        // Stop pending FTD-side one-shot timers so a manual/timeout transition to OTA_IDLE can't be resurrected by a stray callback resuming the confirm loop.
        if (g_phase2_grace_timer)
            xTimerStop(g_phase2_grace_timer, 0);
        if (g_phase3_grace_timer)
            xTimerStop(g_phase3_grace_timer, 0);
        g_phase2.active = false;
        g_phase3.active = false;
        g_phase3_grace_pending = false;
        ota_bitmap_delete(g_phase3.pending);
        ota_bitmap_delete(g_phase3.done);
        g_phase3.pending = NULL;
        g_phase3.done = NULL;
        ota_bitmap_delete(g_child_bitmap);
        g_child_bitmap = NULL;
        g_child_bitmap_total = 0;
        g_push_round_rloc = 0xFFFF;
        g_push_round = 0;
#endif
    }
    if (timeout > 0) {
        xTimerChangePeriod(ota_timer, pdMS_TO_TICKS(timeout), 0);
    }

    if (g_ota_state != state) {
        g_ota_state = state;
        g_state_enter_tick = xTaskGetTickCount();
        if (ota_state_change_cb_signal) {
            ota_state_change_cb_signal(g_ota_state);
        }
#if CONFIG_MIU_DEVICE_TYPE_MTD
        // MTD: push status to Leader on every state change, regardless of prior poll target
        {
            otIp6Address leader_rloc;
            if (otThreadGetLeaderRloc(otrGetInstance(), &leader_rloc)
                == OT_ERROR_NONE) {
                ota_push_status_to(leader_rloc);
            }
        }
#endif
        if (state == OTA_DONE || state == OTA_REBOOT) {
            ota_push_status();
            // Phase 2/3: notify the Leader/Router that triggered us
            {
                static const uint8_t zero[OT_IP6_ADDRESS_SIZE] = {0};
                if (memcmp(g_unicast_req_dst.mFields.m8, zero,
                           OT_IP6_ADDRESS_SIZE)
                    != 0) {
                    ota_push_status_to(g_unicast_req_dst);
                    memset(&g_unicast_req_dst, 0, sizeof(g_unicast_req_dst));
                }
            }
        }
    }
}

// How many ms remain before ota_timer fires (next state transition/retry/give-up); 0 if not armed.
static uint32_t ota_get_timer_remain_ms(void) {
    if (ota_timer == NULL || xTimerIsTimerActive(ota_timer) == pdFALSE) {
        return 0;
    }
    TickType_t now = xTaskGetTickCount();
    TickType_t expiry = xTimerGetExpiryTime(ota_timer);
    TickType_t remain = (expiry > now) ? (expiry - now) : 0;
    return (uint32_t)(remain * portTICK_PERIOD_MS);
}

// Real completion percentage from the live bitmap, same formula as the OTA_STATUS_GET poll path; DONE/REBOOT report 100 outright since the bitmap may already be torn down.
static uint8_t ota_compute_progress_bar(void) {
    if (g_ota_state == OTA_DONE || g_ota_state == OTA_REBOOT) {
        return 100;
    }
    if (g_ota_state == OTA_IDLE || g_ota_segments_size == 0
        || !IS_VALID_PTR(g_ota_bitmap)) {
        return 0;
    }
    uint32_t toatol_num = g_ota_total_num;
    uint32_t remain = ota_bitmap_get_remain(g_ota_bitmap, toatol_num);
    // Denominator is toatol_num (NOT -1): only remain==0 is真100%. The old -1 made remain==1
    // also read 100%, so a 1-seg-short child reported done -> parent advanced & stopped pushing
    // the last seg -> child never hit remain==0 -> ota_check_bin_type()/self-reboot never ran ->
    // parked forever in PHASE3_WAITING showing "100%".
    return (toatol_num == 0)
               ? 0
               : (uint8_t)(((toatol_num - remain) * 100) / toatol_num);
}

// Unicast one OTA_STATUS_REPORT to the tool (g_ota_push_target); only fires once the tool has polled at least once (target non-zero).
static void ota_push_status(void) {
    static const uint8_t zero_addr[OT_IP6_ADDRESS_SIZE] = {0};
    if (memcmp(g_ota_push_target.mFields.m8, zero_addr, OT_IP6_ADDRESS_SIZE)
        == 0) {
        return;
    }
    ota_status_report_t report;
    report.flag = OTA_STATUS_REPORT;
    report.version = (g_ota_state == OTA_IDLE)
                         ? GET_BIN_VERSION(systeminfo.sysinfo)
                         : g_ota_image_version;
    report.rxmode = otThreadGetLinkMode(otrGetInstance()).mRxOnWhenIdle;
    report.state = g_ota_state;
    report.progress_bar = ota_compute_progress_bar();
    memcpy(report.bin_type, GET_BIN_TYPE_PTR(systeminfo.sysinfo), 12);
    memcpy(&report.dstipv6, &g_ota_push_target, sizeof(otIp6Address));
    memcpy(&report.src_mleid, otThreadGetMeshLocalEid(otrGetInstance()),
           sizeof(otIp6Address));
    report.state_remain_ms = ota_get_timer_remain_ms();
    report.state_enter_tick = g_state_enter_tick;
    report.rloc16 = otThreadGetRloc16(
        otrGetInstance()); // was unset here -> tool saw uninitialized rloc
    ota_status_sended_event_handler(g_ota_push_target, (uint8_t*)&report,
                                    sizeof(report));
}

static void ota_push_status_to(otIp6Address dst) {
    ota_status_report_t report;
    report.flag = OTA_STATUS_REPORT;
    report.version = (g_ota_state == OTA_IDLE)
                         ? GET_BIN_VERSION(systeminfo.sysinfo)
                         : g_ota_image_version;
    report.rxmode = otThreadGetLinkMode(otrGetInstance()).mRxOnWhenIdle;
    report.state = g_ota_state;
    report.progress_bar = ota_compute_progress_bar();
    memcpy(report.bin_type, GET_BIN_TYPE_PTR(systeminfo.sysinfo), 12);
    memcpy(&report.dstipv6, &dst, sizeof(otIp6Address));
    memcpy(&report.src_mleid, otThreadGetMeshLocalEid(otrGetInstance()),
           sizeof(otIp6Address));
    report.state_remain_ms = ota_get_timer_remain_ms();
    report.state_enter_tick = g_state_enter_tick;
    report.rloc16 = otThreadGetRloc16(
        otrGetInstance()); // was unset here -> tool saw uninitialized rloc
    ota_status_sended_event_handler(dst, (uint8_t*)&report, sizeof(report));
}

// Erase the whole OTA image region before any (re)fill from an empty/reset bitmap — NOR flash writes only clear bits, so untouched bytes from a prior attempt would otherwise produce the same deterministic CRC mismatch on every retry.
static void ota_erase_image_flash(void) {
    // Erase outside critical section: each sector takes ~10-20 ms; holding all IRQs for OTA_MAX_IMAGE_SECTORS sectors would break Thread connectivity.
    for (uint32_t sector = 0; sector < OTA_MAX_IMAGE_SECTORS; sector++) {
        while (flash_check_busy())
            ;
        flash_erase(FLASH_ERASE_SECTOR, OTA_FLASH_START + (0x1000 * sector));
        while (flash_check_busy())
            ;
    }
}

// Phase 2: called when Router/MTD receives EXECUTE_PHASE2/3C trigger. Checks local bitmap: 100% -> push immediately; <100% -> enter REQUEST_SENDING unicast to src.
static void ota_execute_phase2_respond(otIp6Address leader_addr) {
    g_unicast_req_dst = leader_addr;
    if (g_ota_bitmap == NULL
        || ota_bitmap_get_remain(g_ota_bitmap, g_ota_total_num) == 0) {
        ota_push_status_to(leader_addr);
        memset(&g_unicast_req_dst, 0, sizeof(g_unicast_req_dst));
        return;
    }
    // Has missing segments — REQUEST will go unicast to leader_addr
    ota_request_start();
}

static void ota_execute_phase3_child_respond(otIp6Address router_addr,
                                             const ota_execute_t* exec) {
    // Shared by any sleepy child (MTD or GPIO-sleeping FTD), not gated to !FTD; checks bin_type match, rejecting (back to sleep) on mismatch.
    if (exec != NULL) {
        static const uint8_t zero_bin[12] = {0};
        const uint8_t* self_bin = GET_BIN_TYPE_PTR(systeminfo.sysinfo);
        if (memcmp(exec->img_bin_type, zero_bin, 12) != 0
            && memcmp(exec->img_bin_type, self_bin, 12) != 0) {
            log_info("[Rafael] Phase3: img_bin_type mismatch, reporting Idle "
                     "and going back to sleep");
            // Router's Phase3 confirm loop only checks that a STATUS report arrived from this child's rloc, not the state — without this a mismatched child stays silent and the Router burns 3 blind retries x 30s per child.
            ota_change_state_and_timer(OTA_IDLE, 0);
            ota_push_status_to(router_addr);
            if (g_mtd_was_sleepy) {
                otLinkModeConfig cfg = {.mRxOnWhenIdle = false,
                                        .mNetworkData = false,
                                        .mDeviceType = false};
                otThreadSetLinkMode(otrGetInstance(), cfg);
            }
            return;
        }
    }

    // Already running the target version (pre-flashed, re-OTA same ver, or finished a prior round) — nothing to fetch. Checked BEFORE touching link mode: just report our current state and stay in whatever low-power state we're already in. Do NOT churn Idle→Done here: the parent's grace re-scan re-triggers the WHOLE child table every round, and forcing Done would needlessly wake a finished SED and flip it Idle→Done→Idle each round. The router accepts this Idle+matching-bin_type+target-version report as "done, advance" (see ota_status_received_event_handler / match_idle_done).
    if (exec != NULL && exec->time > 0) {
        const uint32_t* meta = (const uint32_t*)exec->bin_type;
        uint32_t target_ver = meta[0];
        if (target_ver != 0
            && target_ver == GET_BIN_VERSION(systeminfo.sysinfo)) {
            log_info("[Rafael] Phase3 MTD: already at v0x%08x, report success, "
                     "stay asleep",
                     target_ver);
            g_ota_image_version = target_ver;
            bin_type_matched = false; // no reboot
            ota_push_status_to(
                router_addr); // reports current state (Idle=success at target, or Done if still transiently there); never forces a wake
            return;
        }
    }

    // bin_type confirmed and image NOT yet at target — safe to switch to rx-on so REQUEST/RESPONSE can complete quickly. Kept after the already-at-target skip above so a finished/idle child is never woken. (Was unconditional in the caller; moved here so a mismatched child never touches its link mode.)
    {
        otLinkModeConfig cfg;
        cfg.mRxOnWhenIdle = true;
        cfg.mNetworkData = false;
        cfg.mDeviceType = false;
        otThreadSetLinkMode(otrGetInstance(), cfg);
    }

    // If the MTD has no bitmap yet (first trigger seen), cold-start from metadata embedded in the EXECUTE_PHASE3_CHILD packet: time=total_num, bin_type=version/size/crc
    if (g_ota_bitmap == NULL && exec != NULL) {
        uint16_t total_num = exec->time;
        if (total_num > 0) {
            const uint32_t* meta = (const uint32_t*)exec->bin_type;
            uint32_t version = meta[0];
            uint32_t size = meta[1];
            uint32_t crc = meta[2];
            if (version != 0 && size != 0) {
                log_info("[Rafael] Phase3 MTD: cold start — bitmap NULL, init "
                         "%u segs from router metadata",
                         total_num);
                g_ota_total_num = total_num;
                g_ota_image_version = version;
                g_ota_image_size = size;
                g_ota_image_crc = crc;
                g_ota_segments_size = (uint16_t)((size + total_num - 1)
                                                 / total_num);
                ota_bootinfo_reset();
                ota_erase_image_flash();
                g_ota_bitmap = ota_bitmap_init(total_num);
                if (!g_ota_bitmap) {
                    log_error("[Rafael] Phase3: bitmap init failed");
                    return;
                }
                // All bits remain 0 (all segments missing); g_unicast_req_dst non-zero signals was_phase3 in ota_check_bin_type()
                g_unicast_req_dst = router_addr;
                ota_request_start();
                return;
            }
        }
    }

    g_unicast_req_dst = router_addr;
    if (g_ota_bitmap != NULL
        && ota_bitmap_get_remain(g_ota_bitmap, g_ota_total_num) == 0) {
        // All segments already received — verify CRC then reboot, no need to request anything
        uint32_t crc32 = crc32checksum(
            (OTA_FLASH_START + OTA_INFO_HEADER_OFFSET),
            (g_ota_image_size - OTA_INFO_HEADER_OFFSET));
        if (crc32 == g_ota_image_crc) {
            log_info("[Rafael] Phase3 EXECUTE: MTD already has all segments, "
                     "CRC OK → reboot");
            ota_push_status_to(router_addr);
            ota_bootinfo_ready();
            otLinkModeConfig cfg = {.mRxOnWhenIdle = false};
            otThreadSetLinkMode(otrGetInstance(), cfg);
            ota_change_state_and_timer(OTA_REBOOT, OTA_REBOOT_TIMEOUT);
        } else {
            log_info("[Rafael] Phase3 EXECUTE: MTD all segs but CRC fail, "
                     "reset bitmap");
            ota_bitmap_delete(g_ota_bitmap);
            g_ota_bitmap = NULL;
            ota_erase_image_flash();
            g_ota_bitmap = ota_bitmap_init(g_ota_total_num);
            if (!g_ota_bitmap) {
                log_error("[Rafael] Phase3: bitmap init failed");
                return;
            }
            // All bits remain 0 (all segments missing) — matches the erase above
            ota_request_start();
        }
        return;
    }
    if (g_ota_bitmap == NULL) {
        // No bitmap at all — handled above in cold-start block
        ota_push_status_to(router_addr);
        memset(&g_unicast_req_dst, 0, sizeof(g_unicast_req_dst));
        return;
    }
    ota_request_start();
}

#if CONFIG_MIU_DEVICE_TYPE_FTD
// ── Phase 2: Leader confirms each Router via unicast ───────────────────────

static void ota_phase2_start(void) {
    memset(&g_phase2, 0, sizeof(g_phase2));
    g_phase2_grace_round = 0;
    g_phase2.image_version = g_ota_image_version;
    g_phase2.image_size = g_ota_image_size;
    g_phase2.image_crc = g_ota_image_crc;

    uint16_t self_rloc = otThreadGetRloc16(otrGetInstance());
    uint8_t maxRouterId = otThreadGetMaxRouterId(otrGetInstance());

    for (uint8_t rid = 0; rid <= maxRouterId; rid++) {
        otRouterInfo ri;
        if (otThreadGetRouterInfo(otrGetInstance(), rid, &ri) != OT_ERROR_NONE)
            continue;
        if (!ri.mAllocated || ri.mRloc16 == self_rloc)
            continue;
        if (g_phase2.count >= OTA_PHASE2_ROUTER_MAX)
            break;
        g_phase2.rlocs[g_phase2.count++] = ri.mRloc16;
    }

    log_info("[Rafael] Phase2: found %u Routers to confirm", g_phase2.count);
    if (g_phase2.count == 0) {
        // No other Routers — Leader goes straight to its own Phase 3
        {
            const uint8_t* self_bin = GET_BIN_TYPE_PTR(systeminfo.sysinfo);
            uint8_t img_bin[12];
            uint8_t zero_bin[12] = {0};
            ota_flash_read_bin_type(img_bin);
            bin_type_matched = (memcmp(img_bin, self_bin, 12) == 0
                                || memcmp(img_bin, zero_bin, 12) == 0);
            log_info("[Rafael] Phase2(skip): bin_type_matched=%d",
                     (int)bin_type_matched);
        }
        ota_phase3_start();
        return;
    }
    g_phase2.current = 0;
    g_phase2.active = true;
    ota_phase2_send_trigger();
}

static void ota_phase2_send_trigger(void) {
    // Clear resp state from the previous Router before targeting the next one.
    xTimerStop(ota_resp_timer, 0);
    memset(g_resp_table, 0xFF, sizeof(g_resp_table));
    memset(&g_unicast_resp_dst, 0, sizeof(g_unicast_resp_dst));

    if (g_phase2.current >= g_phase2.count) {
        g_phase2.active = false;
        ota_phase2_on_loop_done();
        return;
    }

    uint16_t rloc = g_phase2.rlocs[g_phase2.current];
    otIp6Address dst = *otThreadGetRloc(otrGetInstance());
    dst.mFields.m8[14] = (rloc >> 8) & 0xff;
    dst.mFields.m8[15] = rloc & 0xff;

    log_info("[Rafael] Phase2: trigger Router[%u/%u] rloc=0x%04x",
             g_phase2.current + 1, g_phase2.count, rloc);

    ota_send_execute(dst, OTA_EXECUTE_PHASE2, 0);
    ota_change_state_and_timer(OTA_PHASE2_CONFIRMING, OTA_PHASE2_CONFIRM_MS);
}

// Grace Re-scan: mirror Phase3 — wipe the snapshot and re-sweep the WHOLE live router table from scratch, re-triggering everyone from index 0. An already-confirmed Router replies 100% in one round-trip (cheap); one that was skipped/timed-out (e.g. a lost EXECUTE_PHASE2, or a Router still filling) gets another full attempt instead of being stranded forever. Returns false only if there is no other Router at all.
static bool ota_phase2_rescan_all_routers(void) {
    uint16_t self_rloc = otThreadGetRloc16(otrGetInstance());
    uint8_t maxRouterId = otThreadGetMaxRouterId(otrGetInstance());

    g_phase2.count = 0;
    for (uint8_t rid = 0; rid <= maxRouterId; rid++) {
        otRouterInfo ri;
        if (otThreadGetRouterInfo(otrGetInstance(), rid, &ri) != OT_ERROR_NONE)
            continue;
        if (!ri.mAllocated || ri.mRloc16 == self_rloc)
            continue;
        if (g_phase2.count >= OTA_PHASE2_ROUTER_MAX)
            break;
        g_phase2.rlocs[g_phase2.count++] = ri.mRloc16;
    }
    g_phase2.current = 0;
    g_phase2.try_count = 0;
    return (g_phase2.count > 0);
}

// Send OTA_EXECUTE_REBOOT to every g_phase2 Router once (x RETRY each, NON_CONFIRMABLE).
static void ota_reboot_blast_once(void) {
    for (uint8_t i = 0; i < g_phase2.count; i++) {
        uint16_t rloc = g_phase2.rlocs[i];
        otIp6Address dst = *otThreadGetRloc(otrGetInstance());
        dst.mFields.m8[14] = (rloc >> 8) & 0xff;
        dst.mFields.m8[15] = rloc & 0xff;
        for (uint8_t r = 0; r < OTA_EXECUTE_REBOOT_RETRY; r++) {
            ota_send_execute(dst, OTA_EXECUTE_REBOOT, OTA_UNICAST_WAIT_MS);
        }
    }
}

// Fix A: one resend round on the OT app task; re-arms until g_reboot_resend_left hits 0.
static void ota_reboot_resend_post(void* arg) {
    (void)arg;
    if (g_reboot_resend_left == 0) {
        return;
    }
    g_reboot_resend_left--;
    log_info("[Rafael] Phase3: Router reboot resend (%u round(s) left)",
             g_reboot_resend_left);
    ota_reboot_blast_once();
    if (g_reboot_resend_left > 0 && g_reboot_resend_timer) {
        xTimerStop(g_reboot_resend_timer, 0);
        xTimerChangePeriod(g_reboot_resend_timer,
                           pdMS_TO_TICKS(OTA_REBOOT_RESEND_GAP_MS), 0);
        xTimerStart(g_reboot_resend_timer, 0);
    }
}

static void ota_reboot_resend_timer_cb(TimerHandle_t xTimer) {
    (void)xTimer;
    ot_app_task_post(ota_reboot_resend_post, NULL);
}

// Leader -> all confirmed Routers: reboot together. Called only after the Leader's
// own Phase3 finishes (rebooting earlier churns the topology mid-fill). One immediate
// blast, then OTA_REBOOT_RESEND_ROUNDS more over ~60s (Fix A) for drops/late Routers.
static void ota_command_routers_reboot(void) {
    log_info("[Rafael] Phase3: Leader done, commanding %u Router(s) to reboot "
             "(x%u each), +%u resend round(s)",
             g_phase2.count, OTA_EXECUTE_REBOOT_RETRY,
             OTA_REBOOT_RESEND_ROUNDS);
    ota_reboot_blast_once();
    g_reboot_resend_left = OTA_REBOOT_RESEND_ROUNDS;
    if (g_reboot_resend_timer == NULL) {
        g_reboot_resend_timer = xTimerCreate(
            "ota_reboot_rs", pdMS_TO_TICKS(OTA_REBOOT_RESEND_GAP_MS), pdFALSE,
            NULL, ota_reboot_resend_timer_cb);
    }
    if (g_reboot_resend_timer) {
        xTimerStop(g_reboot_resend_timer, 0);
        xTimerChangePeriod(g_reboot_resend_timer,
                           pdMS_TO_TICKS(OTA_REBOOT_RESEND_GAP_MS), 0);
        xTimerStart(g_reboot_resend_timer, 0);
    } else {
        log_error("[Rafael] Phase3: reboot resend timer create failed "
                  "(one-shot blast only)");
    }
}

// Leader is the sender and never receives its own Trickle multicast, so ota_check_bin_type() was never called for it — compute bin_type_matched here before dispatching Phase 3.
static void ota_phase2_finish(void) {
    log_info("[Rafael] Phase2: all %u Routers confirmed, Leader running Phase3 "
             "for own children",
             g_phase2.count);
    {
        const uint8_t* self_bin = GET_BIN_TYPE_PTR(systeminfo.sysinfo);
        uint8_t img_bin[12];
        uint8_t zero_bin[12] = {0};
        ota_flash_read_bin_type(img_bin);
        bin_type_matched = (memcmp(img_bin, self_bin, 12) == 0
                            || memcmp(img_bin, zero_bin, 12) == 0);
        log_info("[Rafael] Phase2 done: bin_type_matched=%d",
                 (int)bin_type_matched);
    }
    // Router reboot command moved to ota_phase3_finish_ftd()'s Leader branch
    // (after the Leader's own Phase3), so Routers stay up until then.
    ota_phase3_start();
}

// Runs on the OT app task (CONFIG_MIU_OPENTHREAD_TASK_SIZE = 4096 words), never on the timer task — see ota_phase2_grace_timer_cb() below for why this is split out of the timer callback.
static void ota_phase2_on_loop_done_impl(void* arg) {
    (void)arg;
    if (g_phase2_grace_round >= OTA_GRACE_RESCAN_MAX) {
        log_info(
            "[Rafael] Phase2: grace re-scan exhausted (%u rounds), finishing",
            OTA_GRACE_RESCAN_MAX);
        ota_phase2_finish();
        return;
    }
    g_phase2_grace_round++;
    log_info("[Rafael] Phase2: loop done, grace re-scan round %u/%u in %ums",
             g_phase2_grace_round, OTA_GRACE_RESCAN_MAX,
             OTA_GRACE_RESCAN_DELAY_MS);
    xTimerStop(ota_timer,
               0); // no per-Router confirm in flight during grace wait
    if (g_phase2_grace_timer == NULL) {
        g_phase2_grace_timer = xTimerCreate(
            "ota_p2_grace", pdMS_TO_TICKS(OTA_GRACE_RESCAN_DELAY_MS), pdFALSE,
            NULL, ota_phase2_grace_timer_cb);
        if (!g_phase2_grace_timer) {
            log_error("[Rafael] Phase2: grace timer create failed, finishing "
                      "without re-scan");
            ota_phase2_finish();
            return;
        }
    }
    xTimerStop(g_phase2_grace_timer, 0);
    xTimerChangePeriod(g_phase2_grace_timer,
                       pdMS_TO_TICKS(OTA_GRACE_RESCAN_DELAY_MS), 0);
    xTimerStart(g_phase2_grace_timer, 0);
}

// Called whenever the Phase2 confirm loop runs out of targets (first pass or a resumed pass after grace re-scan). Waits OTA_GRACE_RESCAN_DELAY_MS then re-scans, up to OTA_GRACE_RESCAN_MAX rounds, before finishing. Marshals onto the OT app task instead of running inline: reachable from ota_phase2_send_trigger()'s CONFIRMING-timeout path, which runs on the shared FreeRTOS timer-service task (512-word stack) — ota_phase2_finish() chains into flash erase+write and otSysReset() setup, which must not run on that stack. ot_app_task_post() is the existing repo convention for this (see mmp_device.c, app_udp.c, app_uart.c).
static void ota_phase2_on_loop_done(void) {
    ot_app_task_post(ota_phase2_on_loop_done_impl, NULL);
}

// Timer callback proper: runs on the timer-service task, so it only marshals onto the OT app task. The actual rescan and any follow-on dispatch/finish happen in ota_phase2_grace_timer_post() on the bigger stack.
static void ota_phase2_grace_timer_post(void* arg) {
    (void)arg;
    if (ota_phase2_rescan_all_routers()) {
        log_info("[Rafael] Phase2: grace re-scan (round %u/%u) re-sweeping all "
                 "%u Router(s) from index 0",
                 g_phase2_grace_round, OTA_GRACE_RESCAN_MAX, g_phase2.count);
        g_phase2.active = true;
        ota_phase2_send_trigger();
        return;
    }
    ota_phase2_on_loop_done_impl(NULL);
}

static void ota_phase2_grace_timer_cb(TimerHandle_t xTimer) {
    (void)xTimer;
    ot_app_task_post(ota_phase2_grace_timer_post, NULL);
}

// Phase 3 is now self-triggered by each Router via ota_check_bin_type(); the receive-side OTA_EXECUTE_PHASE3 handler is kept for backward compatibility with old firmware nodes.

// ── Phase 0 pre-wake ─────────────────────────────────────────────────────
// Sends unicast RXMODE(ON) to every sleeping child so it wakes for the upcoming Phase 1 multicast; shared by the Leader and every relaying Router. round_version must be the round's real target version, not this device's possibly-stale g_ota_image_version, else a child could wrongly conclude it already has the image and get stranded.
static void ota_prewake_sleeping_children(const uint8_t img_bin_type[12],
                                          uint32_t round_version) {
    uint16_t max_children = otThreadGetMaxAllowedChildren(otrGetInstance());
    uint16_t woke = 0;
    for (uint16_t i = 0; i < max_children; i++) {
        otChildInfo ci;
        if (otThreadGetChildInfoByIndex(otrGetInstance(), i, &ci)
                != OT_ERROR_NONE
            || ci.mIsStateRestoring)
            continue;
        if (ci.mRxOnWhenIdle) // already awake
            continue;
        ota_rxmode_t rxmode;
        memset(&rxmode, 0, sizeof(rxmode));
        rxmode.OnWhenIdle = true;
        rxmode.version = round_version;
        memcpy(rxmode.img_bin_type, img_bin_type, sizeof(rxmode.img_bin_type));
        rxmode.total_segments = (uint16_t)g_ota_total_num;
        rxmode.interval = g_ota_data_interval;
        otIp6Address dst = *otThreadGetRloc(otrGetInstance());
        dst.mFields.m8[14] = (ci.mRloc16 >> 8) & 0xff;
        dst.mFields.m8[15] = ci.mRloc16 & 0xff;
        ota_rxmode_sended_event_handler(dst, (uint8_t*)&rxmode, sizeof(rxmode));
        ++woke;
    }
    log_info("[Rafael] Phase0: pre-wake RXMODE sent to %u sleeping children",
             woke);
}

// Leader entry point for Phase 0: broadcasts OTA_EXECUTE_PHASE3 on ff03::1 as an early "who's this image for" predicate (reusing the existing broadcast type), then pre-wakes its own sleeping children directly since it won't receive its own broadcast.
static void ota_phase0_prewake_broadcast(void) {
    otIp6Address dst;
    otIp6AddressFromString("ff03:0:0:0:0:0:0:1", &dst);
    ota_execute_t exec;
    memset(&exec, 0, sizeof(exec));
    exec.flag = OTA_EXECUTE_PHASE3;
    ota_flash_read_bin_type(exec.img_bin_type);
    // Carry the real round version/size/crc so relaying Routers can prewake their own sleeping children with it instead of falling back to their own stale g_ota_image_version.
    {
        uint32_t* meta = (uint32_t*)exec.bin_type;
        meta[0] = g_ota_image_version;
        meta[1] = g_ota_image_size;
        meta[2] = g_ota_image_crc;
    }
    ota_execute_sended_event_handler(dst, (uint8_t*)&exec, sizeof(exec));
    log_info("[Rafael] Phase0: pre-wake broadcast sent (img_bin_type read from "
             "own flash)");
    ota_prewake_sleeping_children(exec.img_bin_type, g_ota_image_version);
}

// ── Phase 3 (unified): live-query every child, wake sleeping ones, confirm one at a time via unicast. No FTD/MTD split or bin_type filtering here — every child is targeted and decides for itself in ota_execute_phase3_child_respond(). Target list = bitmap over child-table index.

static void ota_phase3_start(void) {
    ota_bitmap_delete(g_phase3.pending);
    ota_bitmap_delete(g_phase3.done);
    memset(&g_phase3, 0, sizeof(g_phase3));
    g_phase3_grace_round = 0;
    g_phase3_grace_pending = false;
    // Capture now: on_loop_done_impl() must not re-read the live bin_type_matched global later.
    g_phase3_bin_type_matched_for_round = bin_type_matched;
    g_phase3.image_version = g_ota_image_version;
    g_phase3.image_size = g_ota_image_size;
    g_phase3.image_crc = g_ota_image_crc;

    g_phase3.max_children = otThreadGetMaxAllowedChildren(otrGetInstance());
    g_phase3.pending = ota_bitmap_init(g_phase3.max_children);
    g_phase3.done = ota_bitmap_init(g_phase3.max_children);
    if (!g_phase3.pending || !g_phase3.done) {
        log_error("[Rafael] Phase3: bitmap init failed, aborting");
        ota_bitmap_delete(g_phase3.pending);
        g_phase3.pending = NULL;
        ota_bitmap_delete(g_phase3.done);
        g_phase3.done = NULL;
        ota_change_state_and_timer(OTA_IDLE, 0);
        return;
    }

    for (uint16_t i = 0; i < g_phase3.max_children; i++) {
        otChildInfo ci;
        if (otThreadGetChildInfoByIndex(otrGetInstance(), i, &ci)
                != OT_ERROR_NONE
            || ci.mIsStateRestoring)
            continue;
        ota_bitmap_set(g_phase3.pending, i);
        g_phase3.total++;
        g_phase3.remaining++;
    }

    log_info("[Rafael] Phase3: found %u children to confirm", g_phase3.total);
    if (g_phase3.remaining == 0) {
        if (g_phase3_bin_type_matched_for_round) {
            ota_phase3_finish_ftd();
        } else {
            ota_change_state_and_timer(OTA_IDLE, 0);
        }
        return;
    }
    g_phase3.current = 0;
    g_phase3.try_count = 0;
    g_phase3.active = true;
    ota_phase3_send_trigger();
}

// Phase3 Grace Re-scan: like Phase2's re-sweep, the parent re-scans its whole live child table — a child that already finished replies 100% immediately and costs one round-trip, while one that is still short gets its gaps filled. Returns false only if there is no live child at all.
static bool ota_phase3_rescan_children(void) {
    uint16_t found = 0;
    // Fresh push budget for the grace round — otherwise the node that exhausted OTA_PUSH_ROUND_MAX in the main loop (exactly the one this re-scan exists for) is skipped again on its first bitmap.
    g_push_round_rloc = 0xFFFF;
    g_push_round = 0;
    ota_bitmap_clear_all(g_phase3.pending, g_phase3.max_children);
    ota_bitmap_clear_all(g_phase3.done, g_phase3.max_children);
    for (uint16_t i = 0; i < g_phase3.max_children; i++) {
        otChildInfo ci;
        if (otThreadGetChildInfoByIndex(otrGetInstance(), i, &ci)
                != OT_ERROR_NONE
            || ci.mIsStateRestoring)
            continue;
        ota_bitmap_set(g_phase3.pending, i);
        found++;
    }
    g_phase3.total = found;
    g_phase3.remaining = found;
    g_phase3.current = 0;
    g_phase3.try_count = 0;
    return (found > 0);
}

static void ota_phase3_send_trigger(void) {
    // Clear leftover resp state from the previous child so we stop sending RESPONSE frames to an RLOC now asleep/done.
    xTimerStop(ota_resp_timer, 0);
    memset(g_resp_table, 0xFF, sizeof(g_resp_table));
    memset(&g_unicast_resp_dst, 0, sizeof(g_unicast_resp_dst));

    uint16_t idx = g_phase3.current;
    while (idx < g_phase3.max_children
           && (!ota_bitmap_get_bit(g_phase3.pending, idx)
               || ota_bitmap_get_bit(g_phase3.done, idx))) {
        idx++;
    }
    g_phase3.current = idx;
    // Invalidate until a live child-table lookup below re-populates it — otherwise a stray late report from the previous (already-done) child could still match here.
    g_phase3.current_rloc = OTA_INVALID_RLOC;

    if (idx >= g_phase3.max_children) {
        g_phase3.active = false;
        ota_phase3_on_loop_done();
        return;
    }

    // Live child-table query at confirm time — no cached rloc from the snapshot. Thread's own child timeout (OPENTHREAD_CONFIG_MLE_CHILD_TIMEOUT_DEFAULT, 240s) already governs how long a real disconnect takes to remove a child, so waiting here doesn't make it "come back" faster — skip immediately.
    otChildInfo ci;
    bool in_child_table = (otThreadGetChildInfoByIndex(otrGetInstance(), idx,
                                                       &ci)
                           == OT_ERROR_NONE)
                          && !ci.mIsStateRestoring;

    if (!in_child_table) {
        log_info("[Rafael] Phase3: skip child idx[%u] — not in child table "
                 "(%u remaining of %u)",
                 idx, g_phase3.remaining, g_phase3.total);
        ota_bitmap_set(g_phase3.done, idx);
        g_phase3.remaining--;
        g_phase3.current++;
        ota_phase3_send_trigger();
        return;
    }

    uint16_t rloc = ci.mRloc16;
    g_phase3.current_rloc =
        rloc; // cache for the confirm-time match (see struct comment)
    otIp6Address dst = *otThreadGetRloc(otrGetInstance());
    dst.mFields.m8[14] = (rloc >> 8) & 0xff;
    dst.mFields.m8[15] = rloc & 0xff;

    log_info("[Rafael] Phase3: trigger child idx[%u] rloc=0x%04x (%u remaining "
             "of %u)",
             idx, rloc, g_phase3.remaining, g_phase3.total);

    // No RXMODE wake step: EXECUTE_PHASE3_CHILD goes out directly and the child decides for itself whether to come awake (bin_type match, see ota_execute_phase3_child_respond()).
    // Embed image metadata so the child can cold-start Phase3 on its first trigger: time=total_num(u16), bin_type[0..3]=version, [4..7]=size, [8..11]=crc.
    {
        ota_execute_t exec;
        exec.flag = OTA_EXECUTE_PHASE3_CHILD;
        exec.time = (uint16_t)g_ota_total_num;
        uint32_t* meta = (uint32_t*)exec.bin_type;
        // Serve the version captured when THIS Phase3 round started, never the live global: a new round's Phase1 DATA can overwrite g_ota_image_version mid-Phase3, and a Router that never reboots must not hand a stale/other-round version to its children (parent-contamination guard).
        meta[0] = g_phase3.image_version;
        meta[1] = g_phase3.image_size;
        meta[2] = g_phase3.image_crc;
        ota_flash_read_bin_type(
            exec.img_bin_type); // tell child what binary type this is
        ota_execute_sended_event_handler(dst, (uint8_t*)&exec, sizeof(exec));
    }
    // Push: confirm only waits for the child's FIRST bitmap (fast), not a full fill. OTA_PHASE3_CONFIRM_MS x (OTA_PHASE3_TRY_MAX+1 sends) = 20s x 3 = 60s, aligned to the child's OTA_MTD_WAKE_WATCHDOG_TIMEOUT(60s) so a slow-to-wake / collided child isn't skipped before it can reply. A bitmap resets try_count and hands off to push.
    ota_change_state_and_timer(OTA_PHASE3_CONFIRMING, OTA_PHASE3_CONFIRM_MS);
}

static void ota_phase3_finish_ftd(void) {
    if (otThreadGetDeviceRole(otrGetInstance()) == OT_DEVICE_ROLE_LEADER) {
        // Leader must never arm its own apply-on-next-boot flag here: it isn't rebooting this round, and a later unrelated reboot must not flash it with the Router image it was distributing.
        // Leader's own children confirmed — now safe to reboot the Routers together
        // (moved here from ota_phase2_finish to avoid churning the mesh mid-fill).
        ota_command_routers_reboot();
        log_info("[Rafael] Phase3(FTD): Leader done, going IDLE");
        ota_change_state_and_timer(OTA_IDLE, 0);
    } else {
        // Router: own children confirmed; wait for Leader's synchronized EXECUTE_REBOOT instead of rebooting immediately (see ota_check_bin_type()).
        ota_bootinfo_ready();
        log_info("[Rafael] Phase3(FTD): %u children done, waiting for Leader "
                 "EXECUTE_REBOOT",
                 g_phase3.total);
        ota_change_state_and_timer(OTA_DONE, OTA_DONE_FALLBACK_TIMEOUT);
    }
}

// Runs on the OT app task (never the timer task, since it chains into a flash erase+write); re-verifies CRC like the immediate-reboot path, mirroring ota_execute_received_event_handler().
static void ota_phase3_execute_pending_reboot(void) {
    uint32_t crc32 = crc32checksum((OTA_FLASH_START + OTA_INFO_HEADER_OFFSET),
                                   (g_ota_image_size - OTA_INFO_HEADER_OFFSET));
    if (crc32 != g_ota_image_crc) {
        ota_printf("ota request upgrade fail %X %X", crc32, g_ota_image_crc);
        ota_change_state_and_timer(OTA_IDLE, 0);
    } else {
        ota_bootinfo_ready();
        ota_change_state_and_timer(OTA_REBOOT, g_phase3_reboot_pending_time_ms);
        ota_printf("%u ms after reboot", g_phase3_reboot_pending_time_ms);
    }
}

// Phase3 really is over: honor a deferred reboot if one was requested during the grace wait, else finalize on the bin_type captured at round start (never the live global).
static void ota_phase3_finalize(void) {
    g_phase3_grace_pending = false;
    if (g_phase3_reboot_pending) {
        log_info("[Rafael] Phase3: grace re-scan done, honoring deferred "
                 "EXECUTE_REBOOT");
        g_phase3_reboot_pending = false;
        ota_phase3_execute_pending_reboot();
        return;
    }
    if (g_phase3_bin_type_matched_for_round) {
        log_info("[Rafael] Phase3(FTD): confirm loop done, finishing");
        ota_phase3_finish_ftd();
    } else {
        log_info("[Rafael] Phase3(MTD): %u children done, Router going IDLE",
                 g_phase3.total);
        ota_change_state_and_timer(OTA_IDLE, 0);
    }
}

// Mirrors ota_phase2_on_loop_done_impl(): the main loop finishing is not the end of Phase3 — wait OTA_GRACE_RESCAN_DELAY_MS, then re-sweep the whole child table, up to OTA_GRACE_RESCAN_MAX rounds.
static void ota_phase3_on_loop_done_impl(void* arg) {
    (void)arg;
    g_phase3.active = false;
    if (g_phase3_grace_round >= OTA_GRACE_RESCAN_MAX) {
        log_info(
            "[Rafael] Phase3: grace re-scan exhausted (%u rounds), finishing",
            OTA_GRACE_RESCAN_MAX);
        ota_phase3_finalize();
        return;
    }
    g_phase3_grace_round++;
    g_phase3_grace_pending = true;
    log_info("[Rafael] Phase3: loop done, grace re-scan round %u/%u in %ums",
             g_phase3_grace_round, OTA_GRACE_RESCAN_MAX,
             OTA_GRACE_RESCAN_DELAY_MS);
    xTimerStop(ota_timer,
               0); // no per-child confirm in flight during the grace wait
    if (g_phase3_grace_timer == NULL) {
        g_phase3_grace_timer = xTimerCreate(
            "ota_p3_grace", pdMS_TO_TICKS(OTA_GRACE_RESCAN_DELAY_MS), pdFALSE,
            NULL, ota_phase3_grace_timer_cb);
        if (!g_phase3_grace_timer) {
            log_error("[Rafael] Phase3: grace timer create failed, finishing "
                      "without re-scan");
            ota_phase3_finalize();
            return;
        }
    }
    xTimerStop(g_phase3_grace_timer, 0);
    xTimerChangePeriod(g_phase3_grace_timer,
                       pdMS_TO_TICKS(OTA_GRACE_RESCAN_DELAY_MS), 0);
    xTimerStart(g_phase3_grace_timer, 0);
}

// Called whenever the Phase3 confirm loop runs out of targets (first pass or a resumed pass after grace re-scan); marshals onto the OT app task, see ota_phase2_on_loop_done() above.
static void ota_phase3_on_loop_done(void) {
    ot_app_task_post(ota_phase3_on_loop_done_impl, NULL);
}

// Runs on the OT app task (the re-sweep can chain into a push and, eventually, flash work); the timer callback below only marshals.
static void ota_phase3_grace_timer_post(void* arg) {
    (void)arg;
    if (g_phase3.pending == NULL || g_phase3.done == NULL) {
        // Round was torn down (OTA_IDLE / ota stop) while we waited. Don't run the normal finish path — that would resurrect the round we just abandoned — but a reboot deferred on this grace wait still has to be honored.
        g_phase3_grace_pending = false;
        if (g_phase3_reboot_pending) {
            g_phase3_reboot_pending = false;
            ota_phase3_execute_pending_reboot();
        }
        return;
    }
    if (ota_phase3_rescan_children()) {
        log_info("[Rafael] Phase3: grace re-scan (round %u/%u) sweeping %u "
                 "child(ren)",
                 g_phase3_grace_round, OTA_GRACE_RESCAN_MAX, g_phase3.total);
        g_phase3.active = true;
        ota_phase3_send_trigger();
        return;
    }
    ota_phase3_on_loop_done_impl(NULL);
}

static void ota_phase3_grace_timer_cb(TimerHandle_t xTimer) {
    (void)xTimer;
    ot_app_task_post(ota_phase3_grace_timer_post, NULL);
}
#endif

uint32_t ota_get_image_version() { return g_ota_image_version; }

void ota_set_image_version(uint32_t version) { g_ota_image_version = version; }

uint32_t ota_get_image_size() { return g_ota_image_size; }

void ota_set_image_size(uint32_t size) { g_ota_image_size = size; }

uint32_t ota_get_image_crc() { return g_ota_image_crc; }

void ota_set_image_crc(uint32_t crc) { g_ota_image_crc = crc; }

uint8_t ota_get_state() { return g_ota_state; }

uint8_t ota_reset() {
    uint8_t ret = 0;
    if (g_ota_state == OTA_IDLE) {
        ota_set_image_version(0);
        ota_set_image_size(0);
        ota_set_image_crc(0);
        for (int i = 0; i < OTA_MAX_IMAGE_SECTORS; i++) {
            // Page erase (4096 bytes)
            hosal_flash_erase(HOSAL_FLASH_ERASE_SECTOR,
                              (OTA_FLASH_START + (0x1000 * i)));
        }
    } else {
        ret = 1;
    }
    return ret;
}

const char* OtaStateToString(ota_state_t state) {
    static const char* const kOtaStateStrings[] = {
        "Idle",        "Ph0:Prewake",    "Ph1:Sending", "Ph1:Receiving",
        "Ph1:Filling", "Ph2:Confirming", "Ph2:Filling", "Ph3:Confirming",
        "Ph3:Filling", "Ph3:Waiting",    "UnicastRecv", "Done",
        "Reboot",      "Ph3:Repairing",
    };
    return ((state > OTA_PHASE3_REPAIRING) || (state < OTA_IDLE))
               ? "invalid"
               : kOtaStateStrings[state - OTA_IDLE];
}

static const char* fota_result_to_str(uint8_t r) {
    switch (r) {
        case 0xFF: return "no-update";
        case FOTA_RESULT_SUCCESS: return "success";
        case FOTA_RESULT_ERR_VERIFY_SIZE_NOT_FOUND: return "err:verify-size";
        case FOTA_RESULT_ERR_IMAGE_SIZE: return "err:img-overflow";
        case FOTA_RESULT_ERR_SECURE_MAGIC_PATTERN_MISMATCH:
            return "err:sig-magic";
        case FOTA_RESULT_ERR_SECURE_VERIFY_SIZE_MISMATCH: return "err:sig-size";
        case FOTA_RESULT_ERR_IMAGE_ECDSA_VERIFY_FAIL: return "err:ecdsa";
        case FOTA_RESULT_ERR_CHECK_IMAGE_CRC_FAIL: return "err:crc";
        case FOTA_RESULT_ERR_TARGET_ADDR_IS_ILLEGAL: return "err:bad-addr";
        case FOTA_RESULT_ERR_COMPRESS_DECODE_FAIL: return "err:lzma";
        default: return "err:unknown";
    }
}

void ota_bootloader_info_check() {
    uint32_t file_ver, crc32;
    fota_information_t t_bootloader_ota_info = {0};
    hosal_flash_read_n_bytes(FOTA_UPDATE_BANK_INFO_ADDRESS,
                             (uint8_t*)&t_bootloader_ota_info,
                             sizeof(t_bootloader_ota_info));
    file_ver = (uint32_t)t_bootloader_ota_info.reserved[0];

    bool bl_was_ready = (t_bootloader_ota_info.fotabank_ready
                         == FOTA_IMAGE_READY);
    log_info(
        "[FOTA] BL: ready=%s result=0x%02X(%s) ver=0x%X len=0x%X crc=0x%08X",
        bl_was_ready ? "Y" : "N", t_bootloader_ota_info.fota_result,
        fota_result_to_str(t_bootloader_ota_info.fota_result),
        (unsigned int)file_ver,
        (unsigned int)t_bootloader_ota_info.fotabank_datalen,
        (unsigned int)t_bootloader_ota_info.fotabank_crc);

    if (bl_was_ready && t_bootloader_ota_info.fota_result != FOTA_RESULT_SUCCESS
        && t_bootloader_ota_info.fota_result != 0xFF) {
        log_warn("[FOTA] BL FAILED: 0x%02X (%s) start=0x%X",
                 t_bootloader_ota_info.fota_result,
                 fota_result_to_str(t_bootloader_ota_info.fota_result),
                 (unsigned int)t_bootloader_ota_info.fotabank_startaddr);
    }

    do {
        if (t_bootloader_ota_info.fotabank_ready == FOTA_IMAGE_READY) {
            if (t_bootloader_ota_info.fota_result == FOTA_RESULT_SUCCESS) {
                log_info("[FOTA] BL applied OK ver=0x%X",
                         (unsigned int)file_ver);
                ota_set_image_version(file_ver);
                ota_set_image_size((t_bootloader_ota_info.fotabank_datalen
                                    + OTA_INFO_HEADER_OFFSET));
                ota_set_image_crc(t_bootloader_ota_info.fotabank_crc);
            } else {
                log_warn("[FOTA] OTA result: 0x%02X (%s)",
                         t_bootloader_ota_info.fota_result,
                         fota_result_to_str(t_bootloader_ota_info.fota_result));
            }
            t_bootloader_ota_info.fotabank_ready = FOTA_IMAGE_READY + 1;
            t_bootloader_ota_info.fotabank_startaddr = 0;
            t_bootloader_ota_info.fotabank_datalen = 0;
            t_bootloader_ota_info.fotabank_crc = 0;
        } else {
            if (t_bootloader_ota_info.fotabank_startaddr
                    == (OTA_FLASH_START + OTA_INFO_HEADER_OFFSET)
                && t_bootloader_ota_info.fotabank_datalen
                       <= SIZE_OF_FOTA_BANK_1MB) {
                uint32_t first_word = 0xFFFFFFFF;
                hosal_flash_read_n_bytes(
                    t_bootloader_ota_info.fotabank_startaddr,
                    (uint8_t*)&first_word, sizeof(first_word));
                if (first_word == 0xFFFFFFFF) {
                    /* OTA buffer was erased (e.g. ISP re-flash) — silent cleanup */
                    t_bootloader_ota_info.fotabank_startaddr = 0;
                    t_bootloader_ota_info.fotabank_datalen = 0;
                    t_bootloader_ota_info.fotabank_crc = 0;
                } else {
                    crc32 = crc32checksum(
                        t_bootloader_ota_info.fotabank_startaddr,
                        t_bootloader_ota_info.fotabank_datalen);
                    if (crc32 == t_bootloader_ota_info.fotabank_crc) {
                        ota_set_image_version(file_ver);
                        ota_set_image_size(
                            (t_bootloader_ota_info.fotabank_datalen
                             + OTA_INFO_HEADER_OFFSET));
                        ota_set_image_crc(t_bootloader_ota_info.fotabank_crc);
                    } else {
                        log_warn("ota crc fail %04X %04X", crc32,
                                 t_bootloader_ota_info.fotabank_crc);
                        ota_reset();
                        t_bootloader_ota_info.fotabank_startaddr = 0;
                        t_bootloader_ota_info.fotabank_datalen = 0;
                        t_bootloader_ota_info.fotabank_crc = 0;
                    }
                }
                t_bootloader_ota_info.fotabank_ready = FOTA_IMAGE_READY + 1;
            }
        }
        enter_critical_section();
        hosal_flash_erase(HOSAL_FLASH_ERASE_SECTOR,
                          FOTA_UPDATE_BANK_INFO_ADDRESS);
        hosal_flash_write(HOSAL_FLASH_WRITE_PAGE, FOTA_UPDATE_BANK_INFO_ADDRESS,
                          (uint8_t*)&t_bootloader_ota_info);
        leave_critical_section();
    } while (0);

    log_info("ota ver             : 0x%08x ", ota_get_image_version());
}

static void ota_reboot_handler() {
    ota_printf("Reboot");
    otPlatReset(NULL);
}

static void ota_bootinfo_ready() {
    uint32_t program_data_u32[256 / 4] __attribute__((aligned(4)));
    uint8_t* const program_data = (uint8_t*)program_data_u32;
    fota_information_t t_bootloader_ota_info = {0};

    hosal_flash_read_n_bytes(FOTA_UPDATE_BANK_INFO_ADDRESS,
                             (uint8_t*)&t_bootloader_ota_info,
                             sizeof(t_bootloader_ota_info));

    t_bootloader_ota_info.fotabank_ready = FOTA_IMAGE_READY;

    memset(program_data, 0xFF, 256);
    memcpy(program_data, (uint8_t*)&t_bootloader_ota_info,
           sizeof(fota_information_t));

    enter_critical_section();
    while (flash_check_busy())
        ;
    flash_erase(FLASH_ERASE_SECTOR, FOTA_UPDATE_BANK_INFO_ADDRESS);

    while (flash_check_busy())
        ;

    flash_write_page((uint32_t)program_data, FOTA_UPDATE_BANK_INFO_ADDRESS);
    while (flash_check_busy())
        ;
    leave_critical_section();

    ota_printf("bootinfo ready");
}

void ota_bootinfo_reset() {
    uint32_t program_data_u32[256 / 4] __attribute__((aligned(4)));
    uint8_t* const program_data = (uint8_t*)program_data_u32;
    fota_information_t t_bootloader_ota_info = {0};

    uint32_t img_size = ota_get_image_size();
    uint32_t img_ver = ota_get_image_version();
    uint32_t img_crc = ota_get_image_crc();

    if (img_size == 0xFFFFFFFF || img_ver == 0xFFFFFFFF || img_crc == 0xFFFFFFFF
        || img_size == 0) {
        log_error("[FOTA] Core param corrupt! Flash write rejected to prevent "
                  "bricking. Size:0x%X, Ver:0x%X",
                  (unsigned int)img_size, (unsigned int)img_ver);
        return;
    }

    t_bootloader_ota_info.fotabank_startaddr = (OTA_FLASH_START
                                                + OTA_INFO_HEADER_OFFSET);
    t_bootloader_ota_info.signature_len = 0;
    t_bootloader_ota_info.target_startaddr = APP_START_ADDRESS;
    t_bootloader_ota_info.fotabank_datalen = img_size - OTA_INFO_HEADER_OFFSET;
    t_bootloader_ota_info.fota_image_info = FOTA_IMAGE_INFO_COMPRESSED;
    t_bootloader_ota_info.reserved[0] = img_ver;
    t_bootloader_ota_info.fota_result = 0xFF;
    t_bootloader_ota_info.fotabank_crc = img_crc;

    memset(program_data, 0xFF, 256);
    memcpy(program_data, (uint8_t*)&t_bootloader_ota_info,
           sizeof(fota_information_t));

    enter_critical_section();
    while (flash_check_busy())
        ;
    flash_erase(FLASH_ERASE_SECTOR, FOTA_UPDATE_BANK_INFO_ADDRESS);

    while (flash_check_busy())
        ;
    flash_write_page((uint32_t)program_data, FOTA_UPDATE_BANK_INFO_ADDRESS);
    while (flash_check_busy())
        ;
    leave_critical_section();

    ota_printf("bootinfo reset status success");
}

void ota_flash_read_bin_type(uint8_t out_bin_type[12]) {
    /* Must be 4-byte aligned for DMA (same requirement as ota_download_cmd_handler). */
    static uint32_t read_buf_u32[OTA_FLASH_SECTOR_SIZE / 4];
    uint8_t* const read_buf = (uint8_t*)read_buf_u32;
    while (flash_check_busy())
        ;
    flash_read_page((uint32_t)(read_buf), FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB);
    while (flash_check_busy())
        ;
    memcpy(out_bin_type, read_buf + 4, 12);
}

static void ota_check_bin_type() {
    const uint8_t* self_bin_type = GET_BIN_TYPE_PTR(systeminfo.sysinfo);
    uint8_t img_bin_type[12];
    ota_flash_read_bin_type(img_bin_type);
    uint8_t zero_type[12] = {0};
    bool matched = (memcmp(img_bin_type, self_bin_type, 12) == 0
                    || memcmp(img_bin_type, zero_type, 12) == 0);

    // Check if we arrived here via Phase3 unicast (g_unicast_req_dst set); must be checked BEFORE ota_change_state_and_timer clears it.
    static const uint8_t zero_addr[OT_IP6_ADDRESS_SIZE] = {0};
    bool was_phase3 = memcmp(g_unicast_req_dst.mFields.m8, zero_addr,
                             OT_IP6_ADDRESS_SIZE)
                      != 0;

    if (matched) {
        bin_type_matched = true;
#if CONFIG_MIU_DEVICE_TYPE_FTD
        // Do NOT trigger the MTD-distribution Phase3 for children — MTD children need a separate MTD binary OTA round; triggering it here would waste time sending FTD binary to MTD children who will reject it.
        otDeviceRole role = otThreadGetDeviceRole(otrGetInstance());
        if (g_ota_state == OTA_UNICAST_RECEIVING
            || role == OT_DEVICE_ROLE_CHILD) {
            // Standalone unicast ("ota send"), or a leaf FED/REED child with no routing responsibility: no Leader Phase2 loop to desync from — reboot on our own as before.
            log_info("[Rafael] FTD binary matched, skip Phase3, rebooting");
            ota_bootinfo_ready();
            ota_change_state_and_timer(OTA_REBOOT, OTA_REBOOT_TIMEOUT);
        } else {
            // Router: first relay/confirm our own children via unicast (same unified Phase3 the Leader uses), otherwise those children never get contacted. ota_phase3_start() ends in ota_phase3_finish_ftd(), parking us in OTA_DONE waiting for the Leader's synchronized EXECUTE_REBOOT (sent only after every Router confirms Phase2+grace-rescan) instead of rebooting immediately — rebooting the moment our own verify finishes desyncs from Routers still mid-Phase2 and fragments the mesh topology. If the Leader never sends the command, the generic OTA_DONE fallback timeout still reboots us.
            log_info("[Rafael] FTD binary matched, relaying to own children");
            ota_phase3_start();
        }
#else
        // MTD device received MTD binary — one transition only, no intermediate Done
        otLinkModeConfig cfg;
        cfg.mRxOnWhenIdle = false;
        cfg.mNetworkData = false;
        cfg.mDeviceType = false;
        if (was_phase3 || g_ota_state == OTA_UNICAST_RECEIVING) {
            // Phase3 fill done (or a Tool `ota send` unicast): write bootinfo, rxmode off, go straight to Reboot
            log_info("[Rafael] Phase3 MTD: fill done, bootinfo armed, "
                     "rebooting in %ums",
                     OTA_REBOOT_TIMEOUT);
            ota_bootinfo_ready();
            otThreadSetLinkMode(otrGetInstance(), cfg);
            ota_change_state_and_timer(OTA_REBOOT, OTA_REBOOT_TIMEOUT);
        } else {
            // Rare edge case: MTD binary arrived directly via multicast (not through a Router's per-child trigger) — rxmode off, wait for the Router's EXECUTE_PHASE3_CHILD (8min fallback).
            log_info("[Rafael] Phase3 MTD: done, rxmode OFF, waiting "
                     "EXECUTE_PHASE3_CHILD");
            if (g_mtd_was_sleepy) {
                otThreadSetLinkMode(otrGetInstance(), cfg);
            }
            // Push Done status to Leader so the OTA tool can see this MTD
            otIp6Address leader_addr;
            if (otThreadGetLeaderRloc(otrGetInstance(), &leader_addr)
                == OT_ERROR_NONE) {
                ota_push_status_to(leader_addr);
            }
            ota_change_state_and_timer(OTA_DONE, OTA_DONE_FALLBACK_TIMEOUT);
        }
#endif
    } else {
        bin_type_matched = false;
        log_info(
            "[Rafael] bin type mismatch — keeping flash for MTD distribution");
        log_info_hexdump("self:", self_bin_type, 12);
        log_info_hexdump("img :", img_bin_type, 12);
#if CONFIG_MIU_DEVICE_TYPE_FTD
        if (g_ota_state == OTA_UNICAST_RECEIVING) {
            // Direct unicast from tool (ota send) with wrong binary — just idle
            log_info("[Rafael] ota send FTD: bin mismatch, returning to Idle");
            ota_change_state_and_timer(OTA_IDLE, 0);
        } else {
            // FTD device received MTD binary via Phase1/2: keep bitmap for children
            ota_change_state_and_timer(OTA_DONE, OTA_DONE_TIMEOUT);
            ota_phase3_start();
        }
#else
        // MTD device received FTD binary: nothing to do
        ota_change_state_and_timer(OTA_IDLE, 0);
#endif
    }
}

static void ota_event_queue_push(uint8_t event, uint8_t* data,
                                 uint16_t data_lens, otIp6Address ipv6) {
    if (data_lens > OTA_EVENT_DATA_MAX_SIZE) {
        ota_printf("ota_event_queue_push data_lens exceed %u ", data_lens);
        return;
    }
    ota_event_data_t push_event_data;
    push_event_data.event = event;
    push_event_data.data_lens = data_lens;
    memcpy(push_event_data.data, data, data_lens);
    memcpy(&push_event_data.ipv6, &ipv6, sizeof(otIp6Address));
    if (xQueueSend(ota_event_queue, &push_event_data, 0) == pdPASS) {
        ot_app_task_post(ota_event_handler, NULL);
    } else {
        log_warn("ota_event_queue full, drop packet \r\n");
    }
}

static uint16_t calc_payload_data_len(uint32_t seq, uint32_t size,
                                      uint16_t segments) {
    uint32_t total_num = (size + segments - 1) / segments;
    uint16_t len = (seq == total_num - 1) ? (uint16_t)(size % segments)
                                          : segments;
    return len ? len : segments;
}

static int ota_data_parse(uint8_t type, uint8_t* payload,
                          uint16_t payloadlength, void* data) {
    if (payload == NULL || data == NULL)
        return 1;

    uint8_t* tmp = payload;
    uint16_t expected_data_lens = 0;

    switch (type) {
        case OTA_PAYLOAD_TYPE_DATA: {
            ota_data_t* dest = (ota_data_t*)data;
            // 1. Define the header size (excluding the last data array).
            const uint16_t header_size = offsetof(ota_data_t, data);
            if (payloadlength < header_size)
                return 1;

            // 2. One-time copy of Header
            memcpy(dest, tmp, header_size);
            tmp += header_size;

            // 3. Calculate the required data length for this package.
            expected_data_lens = calc_payload_data_len(dest->seq, dest->size,
                                                       dest->segments);

            // 4. Copy the remaining payload data
            if (header_size + expected_data_lens > payloadlength)
                return 1;
            memcpy(dest->data, tmp, expected_data_lens);
            tmp += expected_data_lens;
            break;
        }

        case OTA_PAYLOAD_TYPE_DATA_ACK: {
            const uint16_t ack_size = sizeof(ota_data_ack_t); // 7 bytes
            if (payloadlength < ack_size)
                return 1;
            memcpy(data, tmp, ack_size);
            tmp += ack_size;
            break;
        }

        case OTA_PAYLOAD_TYPE_REQUEST: {
            ota_request_t* dest = (ota_request_t*)data;
            const uint16_t req_header_size = offsetof(
                ota_request_t, req_table); // version(4)+size(4)+segments(2)
            if (payloadlength
                < (req_header_size + (OTA_REQUEST_TABLE_SIZE * 2)))
                return 1;

            memcpy(dest, tmp, req_header_size);
            tmp += req_header_size;
            memcpy(dest->req_table, tmp, (OTA_REQUEST_TABLE_SIZE * 2));
            tmp += (OTA_REQUEST_TABLE_SIZE * 2);
            break;
        }

        case OTA_PAYLOAD_TYPE_RESPONSE: {
            ota_response_t* dest = (ota_response_t*)data;
            const uint16_t res_header_size = offsetof(ota_response_t,
                                                      data); // no data[0]
            if (payloadlength < res_header_size)
                return 1;

            memcpy(dest, tmp, res_header_size);
            tmp += res_header_size;

            expected_data_lens = calc_payload_data_len(dest->seq, dest->size,
                                                       dest->segments);

            if (res_header_size + expected_data_lens > payloadlength)
                return 1;
            memcpy(dest->data, tmp, expected_data_lens);
            tmp += expected_data_lens;
            break;
        }

        case OTA_PAYLOAD_TYPE_BITMAP: {
            ota_bitmap_report_t* dest = (ota_bitmap_report_t*)data;
            const uint16_t bm_header_size = offsetof(ota_bitmap_report_t, data);
            if (payloadlength < bm_header_size)
                return 1;
            memcpy(dest, tmp, bm_header_size);
            tmp += bm_header_size;
            if (dest->byte_len > OTA_BITMAP_CHUNK_MAX
                || (bm_header_size + dest->byte_len) > payloadlength)
                return 1;
            memcpy(dest->data, tmp, dest->byte_len);
            tmp += dest->byte_len;
            break;
        }

        default: ota_printf("unknown parse type %u", type); return 1;
    }

    // payloadlength check
    if ((uint16_t)(tmp - payload) != payloadlength) {
        ota_printf("parse len mismatch: %u/%u", (uint16_t)(tmp - payload),
                   payloadlength);
        return 1;
    }
    return 0;
}

static void ota_data_piece(uint8_t type, uint8_t* payload,
                           uint16_t* payloadlength, void* data) {
    if (payload == NULL || data == NULL || payloadlength == NULL)
        return;

    uint8_t* tmp = payload;
    uint16_t data_lens = 0;

    switch (type) {
        case OTA_PAYLOAD_TYPE_DATA: {
            ota_data_t* src = (ota_data_t*)data;
            const uint16_t header_size = offsetof(ota_data_t, data);

            memcpy(tmp, src, header_size);
            tmp += header_size;

            data_lens = calc_payload_data_len(src->seq, src->size,
                                              src->segments);
            memcpy(tmp, src->data, data_lens);
            tmp += data_lens;
            break;
        }

        case OTA_PAYLOAD_TYPE_DATA_ACK: {
            memcpy(tmp, data, sizeof(ota_data_ack_t));
            tmp += sizeof(ota_data_ack_t);
            break;
        }

        case OTA_PAYLOAD_TYPE_REQUEST: {
            ota_request_t* src = (ota_request_t*)data;
            const uint16_t req_header_size = offsetof(ota_request_t, req_table);
            memcpy(tmp, src, req_header_size);
            tmp += req_header_size;
            memcpy(tmp, src->req_table, (OTA_REQUEST_TABLE_SIZE * 2));
            tmp += (OTA_REQUEST_TABLE_SIZE * 2);
            break;
        }

        case OTA_PAYLOAD_TYPE_RESPONSE: {
            ota_response_t* src = (ota_response_t*)data;
            const uint16_t res_header_size = offsetof(ota_response_t, data);
            memcpy(tmp, src, res_header_size);
            tmp += res_header_size;

            data_lens = calc_payload_data_len(src->seq, src->size,
                                              src->segments);
            memcpy(tmp, src->data, data_lens);
            tmp += data_lens;
            break;
        }

        case OTA_PAYLOAD_TYPE_BITMAP: {
            ota_bitmap_report_t* src = (ota_bitmap_report_t*)data;
            const uint16_t bm_header_size = offsetof(ota_bitmap_report_t, data);
            memcpy(tmp, src, bm_header_size);
            tmp += bm_header_size;
            memcpy(tmp, src->data, src->byte_len);
            tmp += src->byte_len;
            break;
        }

        default: ota_printf("unknown piece type %u", type); break;
    }

    *payloadlength = (uint16_t)(tmp - payload);
}

/*coap proccess*/
static void ota_coap_data_proccess(void* aContext, otMessage* aMessage,
                                   const otMessageInfo* aMessageInfo) {
    char string[OT_IP6_ADDRESS_STRING_SIZE];
    static uint8_t s_coap_data_buf[OTA_EVENT_DATA_MAX_SIZE];
    static uint8_t s_coap_ack_payload[sizeof(ota_data_ack_t)];
    uint16_t length, payloadlength = 0;
    otMessage* responseMessage = NULL;
    otCoapCode responseCode = OT_COAP_CODE_EMPTY;
    ota_data_t ota_data;
    ota_data_ack_t ota_data_ack;
    otError error = OT_ERROR_NONE;

    otIp6AddressToString(&aMessageInfo->mPeerAddr, string, sizeof(string));
    length = otMessageGetLength(aMessage) - otMessageGetOffset(aMessage);
    do {
        if (length > 0 && length <= OTA_EVENT_DATA_MAX_SIZE) {
            uint8_t* buf = s_coap_data_buf;
            otMessageRead(aMessage, otMessageGetOffset(aMessage), buf, length);
            ota_event_queue_push(OTA_DATA_RECEIVE_EVENT, buf, length,
                                 aMessageInfo->mPeerAddr);
            if (otCoapMessageGetType(aMessage) == OT_COAP_TYPE_CONFIRMABLE) {
                if (ota_data_parse(OTA_PAYLOAD_TYPE_DATA, buf, length,
                                   &ota_data)) {
                    break;
                }
                /*do ack packet*/
                responseCode = OT_COAP_CODE_VALID;
                responseMessage = otCoapNewMessage(otrGetInstance(), NULL);
                if (responseMessage == NULL) {
                    error = OT_ERROR_NO_BUFS;
                    break;
                }
                error = otCoapMessageInitResponse(responseMessage, aMessage,
                                                  OT_COAP_TYPE_ACKNOWLEDGMENT,
                                                  responseCode);
                if (error != OT_ERROR_NONE) {
                    break;
                }
                ota_data_ack.data_type = OTA_PAYLOAD_TYPE_DATA_ACK;
                ota_data_ack.version = ota_data.version;
                ota_data_ack.seq = ota_data.seq;
                uint8_t* payload = s_coap_ack_payload;
                ota_data_piece(OTA_PAYLOAD_TYPE_DATA_ACK, payload,
                               &payloadlength, &ota_data_ack);
                error = otCoapMessageSetPayloadMarker(responseMessage);
                if (error != OT_ERROR_NONE) {
                    break;
                }
                error = otMessageAppend(responseMessage, payload,
                                        payloadlength);
                if (error != OT_ERROR_NONE) {
                    break;
                }
                error = otCoapSendResponseWithParameters(
                    otrGetInstance(), responseMessage, aMessageInfo, NULL);
                if (error != OT_ERROR_NONE) {
                    break;
                }
            }
        }
    } while (0);
    if (error != OT_ERROR_NONE && responseMessage != NULL) {
        otMessageFree(responseMessage);
    }
}

static void ota_coap_generic_process(otMessage* aMessage,
                                     const otMessageInfo* aMessageInfo,
                                     uint8_t event_type) {
    static uint8_t s_coap_buf[OTA_EVENT_DATA_MAX_SIZE];
    uint16_t length = otMessageGetLength(aMessage)
                      - otMessageGetOffset(aMessage);
    if (length > 0 && length <= OTA_EVENT_DATA_MAX_SIZE) {
        otMessageRead(aMessage, otMessageGetOffset(aMessage), s_coap_buf,
                      length);
        ota_event_queue_push(event_type, s_coap_buf, length,
                             aMessageInfo->mPeerAddr);
    }
}

static void ota_coap_request_proccess(void* aContext, otMessage* aMessage,
                                      const otMessageInfo* aMessageInfo) {
    ota_coap_generic_process(aMessage, aMessageInfo, OTA_REQUEST_RECEIVE_EVENT);
}

static void ota_coap_response_proccess(void* aContext, otMessage* aMessage,
                                       const otMessageInfo* aMessageInfo) {
    ota_coap_generic_process(aMessage, aMessageInfo,
                             OTA_RESPONSE_RECEIVE_EVENT);
}

static void ota_coap_rxmode_proccess(void* aContext, otMessage* aMessage,
                                     const otMessageInfo* aMessageInfo) {
    ota_coap_generic_process(aMessage, aMessageInfo, OTA_RXMODE_RECEIVE_EVENT);
}

static void ota_coap_status_proccess(void* aContext, otMessage* aMessage,
                                     const otMessageInfo* aMessageInfo) {
    ota_coap_generic_process(aMessage, aMessageInfo, OTA_STATUS_RECEIVE_EVENT);
}

static void ota_coap_execute_proccess(void* aContext, otMessage* aMessage,
                                      const otMessageInfo* aMessageInfo) {
    ota_coap_generic_process(aMessage, aMessageInfo, OTA_EXECUTE_RECEIVE_EVENT);
}

static void ota_coap_ack_process(void* aContext, otMessage* aMessage,
                                 const otMessageInfo* aMessageInfo,
                                 otError aResult) {
    char string[OT_IP6_ADDRESS_STRING_SIZE];
    uint8_t ack_buf[sizeof(ota_data_ack_t) + 8];
    uint8_t* buf = ack_buf;
    uint16_t length;
    ota_data_ack_t ota_data_ack;
    otIp6AddressToString(&aMessageInfo->mPeerAddr, string, sizeof(string));
    length = otMessageGetLength(aMessage) - otMessageGetOffset(aMessage);
    do {
        if (length > 0 && length <= sizeof(ack_buf)) {
            otMessageRead(aMessage, otMessageGetOffset(aMessage), buf, length);
            if (buf[0] == OTA_PAYLOAD_TYPE_DATA_ACK) {
                if (ota_data_parse(OTA_PAYLOAD_TYPE_DATA_ACK, buf, length,
                                   &ota_data_ack)) {
                    break;
                }
                if (ota_data_ack.version != ota_get_image_version()) {
                    ota_printf("ota_data_ack version %x %x",
                               ota_data_ack.version, ota_get_image_version());
                    break;
                }
                ota_printf("[r] ota ack : %d/%d ", ota_data_ack.seq,
                           g_ota_start_index);
                if (ota_data_ack.seq == g_ota_start_index) {
                    g_have_data_ack = true;
                    ota_change_state_and_timer(g_ota_state, 1);
                }
            }
        }
    } while (0);
}

otError ota_coap_request(otCoapCode aCoapCode, otIp6Address coapDestinationIp,
                         otCoapType coapType, uint8_t* payload,
                         uint16_t payloadLength, const char* coap_Path) {
    otError error = OT_ERROR_NONE;
    otMessage* message = NULL;
    otMessageInfo messageInfo;

    // Default parameters

    do {
        message = otCoapNewMessage(otrGetInstance(), NULL);
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
                otrGetInstance(), message, &messageInfo, &ota_coap_ack_process,
                NULL, NULL);
        } else {
            error = otCoapSendRequestWithParameters(
                otrGetInstance(), message, &messageInfo, NULL, NULL, NULL);
        }
    } while (0);

    if ((error != OT_ERROR_NONE) && (message != NULL)) {
        otMessageFree(message);
    }
    return error;
}

static void ota_data_send_handler() {
    ota_data_t ota_data;
    static uint8_t s_data_payload[sizeof(ota_data_t)];
    uint8_t* const payload = s_data_payload;
    uint16_t payloadlength = 0, ota_data_lens = 0, i = 0;
    uint32_t start_addr;
    bool is_unicst = false;
    do {
        if (g_ota_total_num == 0) {
            ota_printf("g_ota_total_num 0");
            ota_change_state_and_timer(OTA_IDLE, 0);
            break;
        }
        if (ota_sender_addr.mFields.m8[0] != 0xff
            && ota_sender_addr.mFields.m8[1] != 0x03) {
            is_unicst = true;
        }
        if (g_ota_start_index < (g_ota_total_num - 1)) {
            ota_data_lens = g_ota_segments_size;
        } else if (g_ota_start_index == (g_ota_total_num - 1)) {
            ota_data_lens = (ota_get_image_size() % g_ota_segments_size);
        } else {
            if (is_unicst == true) {
                ota_change_state_and_timer(OTA_IDLE, 0);
            } else {
#if CONFIG_MIU_DEVICE_TYPE_FTD
                // Phase 1 multicast complete -> Leader starts Phase 2
                if (otThreadGetDeviceRole(otrGetInstance())
                        == OT_DEVICE_ROLE_LEADER
                    && !g_phase2.active) {
                    ota_phase2_start();
                    break;
                }
#endif
                ota_printf("============wait for ota request============");
                ota_change_state_and_timer(OTA_DONE, OTA_DONE_TIMEOUT);
            }
            break;
        }

        if (is_unicst == true) {
            if (g_have_data_ack == true) {
                ++g_ota_start_index;
            }
        }

        if (g_ota_start_index == g_ota_last_index) {
            ++g_ota_unicast_try_cnt;
            if (g_ota_unicast_try_cnt >= 10) {
                ota_printf("try to max %d ", g_ota_unicast_try_cnt);
                ota_change_state_and_timer(OTA_IDLE, 0);
                break;
            }
        } else {
            g_ota_last_index = g_ota_start_index;
            g_ota_unicast_try_cnt = 0;
        }

        ota_data.version = ota_get_image_version();
        ota_data.size = ota_get_image_size();
        ota_data.crc = ota_get_image_crc();
        ota_data.seq = g_ota_start_index;
        ota_data.segments = g_ota_segments_size;
        ota_data.interval = g_ota_data_interval;
        ota_data.is_unicast = is_unicst;
        enter_critical_section();
        start_addr = (OTA_FLASH_START + (ota_data.seq * ota_data.segments));
        for (int i = 0; i < ota_data_lens; i++) {
            hosal_flash_read(HOSAL_FLASH_READ_BYTE, (start_addr + i),
                             &ota_data.data[i]);
        }
        leave_critical_section();

        memset(payload, 0x0, sizeof(ota_data_t));
        ota_data_piece(OTA_PAYLOAD_TYPE_DATA, payload, &payloadlength,
                       &ota_data);
        ota_event_queue_push(OTA_DATA_SEND_EVENT, payload, payloadlength,
                             ota_invalid_addr);
        ota_printf("[s] ota data: [%3d%%] [%u/%u] ",
                   ((ota_data.seq * 100) / (g_ota_total_num - 1)), ota_data.seq,
                   (g_ota_total_num - 1));
        if (is_unicst == true) {
            g_have_data_ack = false;
        } else {
            g_ota_start_index++;
        }
        // Segment 0 just went out: give MPL forwarders/neighbors time to settle before segment 1 so the multicast doesn't retransmit into a burst of still-queued duplicates; from segment 1 onward use the configured interval.
        if (!is_unicst && ota_data.seq == 0) {
            ota_change_state_and_timer(g_ota_state, OTA_PHASE1_FIRST_GAP_MS);
        } else {
            ota_change_state_and_timer(g_ota_state, g_ota_data_interval);
        }
    } while (0);
    return;
}

#if CONFIG_MIU_DEVICE_TYPE_FTD
// Next seq >= from that the served child still misses (bit==0), or -1 when the child has everything.
static int32_t ota_child_next_missing(uint32_t from) {
    for (uint32_t i = from; i < g_child_bitmap_total; i++) {
        if (ota_bitmap_get_bit(g_child_bitmap, i) == 0) {
            return (int32_t)i;
        }
    }
    return -1;
}

// Release the served child's bitmap between children.
static void ota_push_free_child_bitmap(void) {
    ota_bitmap_delete(g_child_bitmap);
    g_child_bitmap = NULL;
    g_child_bitmap_total = 0;
}

// Push abandoned for the current target: free its bitmap and hand back to the confirm loop. Do NOT self-advance here — the advance to the next target is driven by the receiver's 100% STATUS report (success) or the confirm timeout (drop), which is phase-aware (Phase2 vs Phase3, see the progress_bar==100 handler) and the single source of truth. Self-advancing here would both double-advance Phase3 and mis-advance the wrong loop in Phase2.
static void ota_push_end_target(void) {
    ota_push_free_child_bitmap();
    if (g_phase2.active) {
        ota_change_state_and_timer(OTA_PHASE2_CONFIRMING, 5000);
    } else if (g_phase3.active) {
        ota_change_state_and_timer(OTA_PHASE3_CONFIRMING,
                                   OTA_PHASE3_CONFIRM_MS);
    } else {
        ota_change_state_and_timer(OTA_IDLE, 0);
    }
}

// One batch is done: re-trigger the SAME target (current is not advanced) so it answers with a fresh bitmap, and the next batch fills only the diff. Loops until the target reports 100% or OTA_PUSH_ROUND_MAX is hit.
static void ota_push_batch_done(void) {
    ota_push_free_child_bitmap();
    if (g_phase2.active) {
        ota_phase2_send_trigger();
    } else if (g_phase3.active) {
        ota_phase3_send_trigger();
    } else {
        ota_change_state_and_timer(OTA_IDLE, 0);
    }
}

// Give up on the current target and move the confirm loop past it — used when the batch/re-ask loop stops converging.
static void ota_push_skip_target(void) {
    ota_push_free_child_bitmap();
    if (g_phase2.active) {
        g_phase2.current++;
        ota_phase2_send_trigger();
    } else if (g_phase3.active) {
        ota_bitmap_set(g_phase3.done, g_phase3.current);
        if (g_phase3.remaining) {
            g_phase3.remaining--;
        }
        g_phase3.current++;
        g_phase3.try_count = 0;
        ota_phase3_send_trigger();
    } else {
        ota_change_state_and_timer(OTA_IDLE, 0);
    }
}

// Phase 2/3 push: unicast (RESPONSE) one missing segment per tick and advance unconditionally — the whole batch is fire-and-forget, with no per-segment ack. Reliability comes from re-asking the target for a fresh bitmap once the batch drains (ota_push_batch_done) and pushing only the diff. RESPONSE stays NON — CON piles up and exhausts the 6lo buffers (see ota_response_sended_event_handler). Destination is g_unicast_resp_dst, set to the child when the push starts.
static void ota_push_send_handler(void) {
    ota_response_t ota_response;
    static uint8_t s_resp_payload[sizeof(ota_response_t)];
    uint8_t* const payload = s_resp_payload;
    uint16_t payloadlength = 0, ota_data_lens = 0, i = 0;
    uint32_t start_addr;
    int32_t seq;

    if (g_child_bitmap == NULL || g_child_bitmap_total == 0) {
        ota_change_state_and_timer(OTA_IDLE, 0);
        return;
    }

    seq = (int32_t)g_ota_start_index;
    if (seq < 0 || (uint32_t)seq >= g_child_bitmap_total
        || ota_bitmap_get_bit(g_child_bitmap, (uint32_t)seq)) {
        seq = ota_child_next_missing(g_ota_start_index);
    }
    if (seq < 0) {
        log_info(
            "[Rafael] push: batch done (round %u/%u), re-asking for bitmap",
            g_push_round, OTA_PUSH_ROUND_MAX);
        ota_push_batch_done();
        return;
    }
    g_ota_start_index = (uint32_t)seq;

    // Invariant: sender is complete before serving; if it somehow lacks the seg, skip it so the batch can't stall (the target re-requests it in the next bitmap).
    if (ota_bitmap_get_bit(g_ota_bitmap, g_ota_start_index) == 0) {
        log_info("[Rafael] push: sender missing seq %u, skip",
                 g_ota_start_index);
        ota_bitmap_set(
            g_child_bitmap,
            g_ota_start_index); // treat as served so the walk advances
        ota_change_state_and_timer(g_ota_state, 1);
        return;
    }

    ota_data_lens = calc_payload_data_len(g_ota_start_index, g_ota_image_size,
                                          g_ota_segments_size);
    ota_response.version = ota_get_image_version();
    ota_response.size = ota_get_image_size();
    ota_response.crc = ota_get_image_crc();
    ota_response.seq = (uint16_t)g_ota_start_index;
    ota_response.segments = g_ota_segments_size;
    enter_critical_section();
    start_addr = OTA_FLASH_START + (g_ota_start_index * g_ota_segments_size);
    for (i = 0; i < ota_data_lens; i++) {
        hosal_flash_read(HOSAL_FLASH_READ_BYTE, (start_addr + i),
                         &ota_response.data[i]);
    }
    leave_critical_section();

    memset(payload, 0x0, sizeof(ota_response_t));
    ota_data_piece(OTA_PAYLOAD_TYPE_RESPONSE, payload, &payloadlength,
                   &ota_response);
    ota_event_queue_push(OTA_RESPONSE_SEND_EVENT, payload, payloadlength,
                         ota_invalid_addr);
    ota_printf("[s] push resp seq %u", g_ota_start_index);
    // Mark served and step past it — no ack is awaited; whatever is lost comes back in the target's next bitmap.
    ota_bitmap_set(g_child_bitmap, g_ota_start_index);
    g_ota_start_index++;
    // Fixed pacing, independent of g_ota_data_interval (which can be 0 on a cold-started pusher and would arm no timer -> freeze).
    // Randomized pacing (OTA_PUSH_SEG_GAP_MIN_MS..MAX): fixed 100ms overran the child's absorb rate; a per-seg random gap gives flash-write + poll-retrieval time and de-correlates concurrent routers so their bursts don't repeatedly collide.
    ota_change_state_and_timer(
        g_ota_state, otRandomNonCryptoGetUint16InRange(
                         OTA_PUSH_SEG_GAP_MIN_MS, OTA_PUSH_SEG_GAP_MAX_MS + 1));
}
#endif

static void ota_response_send_handler() {
    static uint8_t s_resp_payload[sizeof(ota_response_t)];
    uint8_t* const payload = s_resp_payload;
    uint8_t resp_index = 0;
    uint16_t resp_table_num = 0, i = 0;
    ota_response_t ota_response;
    uint16_t payloadlength = 0, ota_response_data_lens = 0;
    uint32_t start_addr;
    uint32_t toatol_num = g_ota_total_num;

    do {
        for (resp_index = 0; resp_index < OTA_RESPONSE_TABLE_SIZE;
             resp_index++) {
            if (g_resp_table[resp_index] != 0xFFFF) {
                break;
            }
        }
        if (resp_index >= OTA_RESPONSE_TABLE_SIZE) {
            break; // nothing pending
        }

        if (0 == ota_bitmap_get_bit(g_ota_bitmap, g_resp_table[resp_index])) {
            ota_printf("doesn't have %u ", g_resp_table[resp_index]);
            break;
        }

        ota_response.version = g_ota_image_version;
        ota_response.size = g_ota_image_size;
        ota_response.seq = g_resp_table[resp_index];
        ota_response.segments = g_ota_segments_size;
        ota_response.crc = g_ota_image_crc;
        if (ota_response.seq != (toatol_num - 1)) {
            ota_response_data_lens = g_ota_segments_size;
        } else {
            ota_response_data_lens = g_ota_image_size % g_ota_segments_size;
        }
        enter_critical_section();
        start_addr = (OTA_FLASH_START
                      + (ota_response.seq * g_ota_segments_size));

        for (int i = 0; i < ota_response_data_lens; i++) {
            hosal_flash_read(HOSAL_FLASH_READ_BYTE, (start_addr + i),
                             &ota_response.data[i]);
        }
        leave_critical_section();

        memset(payload, 0x0, sizeof(ota_response_t));
        ota_data_piece(OTA_PAYLOAD_TYPE_RESPONSE, payload, &payloadlength,
                       &ota_response);
        ota_event_queue_push(OTA_RESPONSE_SEND_EVENT, payload, payloadlength,
                             ota_invalid_addr);
        ota_printf("[s] ota response %u %u %08x ", resp_index, ota_response.seq,
                   start_addr);
        g_resp_table[resp_index] = 0xFFFF;

    } while (0);

    resp_table_num = 0;
    for (i = 0; i < OTA_RESPONSE_TABLE_SIZE; i++) {
        if (g_resp_table[i] != 0xFFFF) {
            ++resp_table_num;
        }
    }
    if (!xTimerIsTimerActive(ota_resp_timer) && resp_table_num > 0) {
        // RESPONSE is always unicast back to the REQUEST sender (g_unicast_resp_dst set the moment a REQUEST is received) — no concurrent responder to desync from, fire the next one right away.
        xTimerChangePeriod(ota_resp_timer, pdMS_TO_TICKS(10), 0);
        xTimerStart(ota_resp_timer, 0);
        g_ota_response_c = 0;
    }
}

// Receiver: announce our gaps to the pushing parent (g_unicast_req_dst) by sending our whole bitmap on ota/req. Replaces the pull REQUEST — the parent pushes from this, we don't ask seg-by-seg.
static void ota_send_bitmap(void) {
    if (g_ota_bitmap == NULL || g_ota_total_num == 0) {
        return;
    }
    static uint8_t s_bm_payload[sizeof(ota_bitmap_report_t)];
    ota_bitmap_report_t rep;
    uint16_t payloadlength = 0;
    uint16_t nbytes = (uint16_t)(((g_ota_total_num + 31) / 32) * 4);
    if (nbytes > OTA_BITMAP_CHUNK_MAX) {
        nbytes =
            OTA_BITMAP_CHUNK_MAX; // huge-image chunking is a later refinement; realistic images fit one message
    }
    rep.version = g_ota_image_version;
    rep.total_num = (uint16_t)g_ota_total_num;
    rep.byte_offset = 0;
    rep.byte_len = nbytes;
    memcpy(rep.data, (uint8_t*)g_ota_bitmap, nbytes);
    memset(s_bm_payload, 0, sizeof(s_bm_payload));
    ota_data_piece(OTA_PAYLOAD_TYPE_BITMAP, s_bm_payload, &payloadlength, &rep);
    ota_event_queue_push(OTA_REQUEST_SEND_EVENT, s_bm_payload, payloadlength,
                         ota_invalid_addr);
}

static void ota_request_start() {
    // Push model: send our bitmap once, then wait to be pushed. The FILLING-state timer is now a no-data watchdog (ota_fill_watchdog_handler), not a pull trigger. Reset on each received RESPONSE.
    ota_send_bitmap();
    static const uint8_t zero[OT_IP6_ADDRESS_SIZE] = {0};
    bool unicast_req =
        (memcmp(g_unicast_req_dst.mFields.m8, zero, OT_IP6_ADDRESS_SIZE) != 0);
    uint32_t timeout = OTA_MTD_WAKE_WATCHDOG_TIMEOUT;
    // Router/Leader is fed by the Leader directly (Phase1/2); a Child is fed by its parent Router (Phase3). Role distinction, not FTD/MTD.
    otDeviceRole role = otThreadGetDeviceRole(otrGetInstance());
    if (role == OT_DEVICE_ROLE_ROUTER || role == OT_DEVICE_ROLE_LEADER) {
        ota_change_state_and_timer(
            unicast_req ? OTA_PHASE2_FILLING : OTA_PHASE1_FILLING, timeout);
    } else {
        ota_change_state_and_timer(OTA_PHASE3_FILLING, timeout);
    }
}

// Runs on the timer-service task; MUST NOT block and MUST NOT touch OT/radio directly. otThreadSetLinkMode flips the radio into sleep, which races the OT task's Child Update TX on single-core and can wedge the radio state machine -> child detaches. So marshal onto the OT app task (runs under otrLock) like the grace timers.
static void ota_sleep_timer_handler(TimerHandle_t xTimer) {
    (void)xTimer;
    ot_app_task_post(ota_sleep_timer_post, NULL);
}

// Runs on the OT app task under otrLock. Re-check state: a resumed DATA download leaves WAITING and this becomes a no-op (self-cancelling).
static void ota_sleep_timer_post(void* arg) {
    (void)arg;
    if (ota_get_state() == OTA_PHASE3_WAITING
        && otThreadGetLinkMode(otrGetInstance()).mRxOnWhenIdle == true) {
        // [DEBUG] Capture the going-sleepy -> (possible) detach/buffer-stall window; runtime level starts at NONE and resets to NONE on reboot, so a recovering device stops logging while a stuck one keeps printing MeshForwarder/MLE activity. Remove (or gate) for customer builds.
        // otLoggingSetLevel(OT_LOG_LEVEL_INFO);
        otLinkModeConfig cfg = {.mRxOnWhenIdle = false,
                                .mNetworkData = false,
                                .mDeviceType = false};
        otThreadSetLinkMode(otrGetInstance(), cfg);
    }
}

// Arm the deferred sleepy Child Update stagger_ms from now without blocking (period >0 required, so floor to 1 tick).
static void ota_arm_deferred_sleep(uint32_t stagger_ms) {
    xTimerChangePeriod(ota_sleep_timer,
                       pdMS_TO_TICKS(stagger_ms ? stagger_ms : 1), 0);
}

// FILLING states are push receivers: if no RESPONSE arrives within the watchdog window, keep the bitmap and park in OTA_PHASE3_WAITING — terminal static, no timer and no polling. Recovery is a Leader Phase1 re-run or a Tool `ota send <ipv6>` unicast. Reset by ota_response_received_event_handler on each received seg.
static void ota_fill_watchdog_handler(void) {
    log_info(
        "[Rafael] fill watchdog: no push, parking to WAITING (bitmap kept)");
    // Report WAITING now while the radio is still on (poll/push captures it), then defer the sleepy mode-change.
    ota_change_state_and_timer(OTA_PHASE3_WAITING, 0);
    if (g_mtd_was_sleepy) {
        // Only genuinely-sleepy devices re-sleep; a Router (g_mtd_was_sleepy=false) never arms this, so it no longer eats the stagger delay. Stagger by rloc bucket so children under one parent don't all hit Child Update at once.
        uint16_t rloc16 = otThreadGetRloc16(otrGetInstance());
        uint16_t mixed = rloc16 ^ (rloc16 >> 8);
        ota_arm_deferred_sleep((mixed % 100) * 100);
    }
}

static void ota_image_received_handler() {
    uint32_t crc32 = 0, remain = 0;
    uint32_t toatol_num = g_ota_total_num;

    remain = ota_bitmap_get_remain(g_ota_bitmap, toatol_num);

    ota_printf("ota data remain %u (%3d%%) ", remain,
               (toatol_num == 0)
                   ? 0
                   : (int)(((toatol_num - remain) * 100) / toatol_num));

    if (remain == 0) {
        crc32 = crc32checksum((OTA_FLASH_START + OTA_INFO_HEADER_OFFSET),
                              (g_ota_image_size - OTA_INFO_HEADER_OFFSET));
        if (crc32 != g_ota_image_crc) {
            ota_printf("ota data upgrade fail ");
            ota_change_state_and_timer(OTA_IDLE, 0);
            //restart ota request
        }
        //wait request packet (FTD: waits for EXECUTE_PHASE2 from Leader)
    } else {
#if CONFIG_MIU_DEVICE_TYPE_FTD
        // Phase1 window elapsed, bitmap still incomplete. Router cannot self-heal here: ota_request_start()'s REQUEST always goes to g_unicast_req_dst, only set when the Leader reaches out first via EXECUTE_PHASE2 — calling it here with no destination silently drops every REQUEST and burns retries into a premature OTA_IDLE, abandoning the bitmap before the Leader's Phase2 loop reaches us. So stay put and wait for EXECUTE_PHASE2, same structure as the MTD child waiting for EXECUTE_PHASE3_CHILD one layer down.
        log_info("[Rafael] Phase1: window elapsed, remain=%u, waiting for "
                 "Leader EXECUTE_PHASE2",
                 remain);
        ota_change_state_and_timer(OTA_PHASE3_WAITING, 0);
#else
        // Phase1 window elapsed, bitmap incomplete: park in OTA_PHASE3_WAITING and defer going back to sleep (keeps bitmap; Phase 3 fills gaps later). Report WAITING now (radio still on), then arm the staggered mode-change (XOR router-ID into RLOC16, 100 buckets) so devices under different Routers don't all hit Child Update Request at once and overload a parent into forcing re-attach.
        uint16_t rloc16 = otThreadGetRloc16(otrGetInstance());
        uint16_t mixed = rloc16 ^ (rloc16 >> 8);
        uint32_t stagger_ms = (mixed % 100) * 100;
        log_info("[Rafael] Phase1: pre-wake window elapsed, remain=%u, "
                 "deferring sleep (stagger=%ums)",
                 remain, stagger_ms);
        ota_change_state_and_timer(OTA_PHASE3_WAITING, 0);
        if (otThreadGetLinkMode(otrGetInstance()).mRxOnWhenIdle == true) {
            ota_arm_deferred_sleep(stagger_ms);
        }
#endif
    }
}

static void ota_unicast_received_handler() {
    uint32_t crc32 = 0, remain = 0;
    uint32_t toatol_num = g_ota_total_num;

    remain = ota_bitmap_get_remain(g_ota_bitmap, toatol_num);

    ota_printf("ota data remain %u (%3d%%) ", remain,
               ((remain * 100) / (toatol_num - 1)));

    if (remain == 0) {
        crc32 = crc32checksum((OTA_FLASH_START + OTA_INFO_HEADER_OFFSET),
                              (g_ota_image_size - OTA_INFO_HEADER_OFFSET));
        if (crc32 != g_ota_image_crc) {
            ota_printf("ota data upgrade fail ");
            ota_change_state_and_timer(OTA_IDLE, 0);
        }
        // CRC OK: handled in the data-receive path (calls ota_check_bin_type); this timer path fires only if remain==0 but CRC was already good, safe no-op.
    } else {
        // Timeout with missing packets: the manual unicast sender pushes DATA and resends unacked segs itself, so a receiver-side pull is no longer used here (would collide with ota/req = bitmap). Give up cleanly; re-run `ota send` to retry.
        ota_printf("unicast timeout remain=%u, give up (re-run ota send)",
                   remain);
        ota_change_state_and_timer(OTA_IDLE, 0);
    }
}

// Timer-service task: marshal onto the OT app task so the CoAP send runs on the OT task under otrLock (same reason as ota_sleep_timer_handler / ota_event_queue_push).
static void ota_response_timer_handler(TimerHandle_t xTimer) {
    (void)xTimer;
    ot_app_task_post(ota_response_timer_post, NULL);
}

static void ota_response_timer_post(void* arg) {
    (void)arg;
    ota_response_send_handler();
}

// Timer-service task: read the timer ID here (cheap, no OT call), then marshal the CoAP status send onto the OT app task. The ota_status_report_t pointed at is stable (it is the timer ID), so passing it as the post arg is safe.
static void ota_report_timer_handler(TimerHandle_t xTimer) {
    ot_app_task_post(ota_report_timer_post, pvTimerGetTimerID(xTimer));
    // One-shot: leave the timer allocated (dormant) so xTimerIsTimerActive gates the next poll; do not delete/NULL it.
}

static void ota_report_timer_post(void* arg) {
    ota_status_report_t* ota_status_report = (ota_status_report_t*)arg;
    ota_status_sended_event_handler(ota_status_report->dstipv6,
                                    (uint8_t*)ota_status_report,
                                    sizeof(ota_status_report_t));
}

// Timer-service task: marshal onto the OT app task; the state-machine tick below sends CoAP / flips link mode, all of which must run on the OT task under otrLock.
static void ota_timer_handler(TimerHandle_t xTimer) {
    (void)xTimer;
    ot_app_task_post(ota_timer_post, NULL);
}

static void ota_timer_post(void* arg) {
    (void)arg;
    switch (g_ota_state) {
        case OTA_PHASE1_SENDING: ota_data_send_handler(); break;
        case OTA_PHASE1_RECEIVING: ota_image_received_handler(); break;
        case OTA_UNICAST_RECEIVING: ota_unicast_received_handler(); break;
        case OTA_PHASE1_FILLING:
        case OTA_PHASE2_FILLING:
        case OTA_PHASE3_FILLING: ota_fill_watchdog_handler(); break;
#if CONFIG_MIU_DEVICE_TYPE_FTD
        case OTA_PHASE3_REPAIRING: ota_push_send_handler(); break;
#endif
        case OTA_PHASE0_PREWAKE:
            if (!g_ota_prewake_woken) {
#if CONFIG_MIU_DEVICE_TYPE_FTD
                // Leader only: ota_phase0_prewake_broadcast()/g_prewake_rounds_left live under the FTD guard, so the multi-round logic must too. A woken MTD never reaches here (it arms this state with g_ota_prewake_woken=true) and stays 0 rounds regardless.
                if (g_prewake_rounds_left > 0) {
                    // Another pre-wake round: re-run the full broadcast (already-awake children are skipped inside ota_prewake_sleeping_children), wait one more gap, then Phase 1.
                    g_prewake_rounds_left--;
                    log_info("[Rafael] Phase0: pre-wake round %u/%u",
                             OTA_PHASE0_PREWAKE_ROUNDS - g_prewake_rounds_left,
                             OTA_PHASE0_PREWAKE_ROUNDS);
                    ota_phase0_prewake_broadcast();
                    ota_change_state_and_timer(OTA_PHASE0_PREWAKE,
                                               OTA_PHASE0_PREWAKE_GAP_MS);
                } else
#endif
                {
                    // Last pre-wake round's window elapsed, start broadcasting.
                    ota_change_state_and_timer(OTA_PHASE1_SENDING,
                                               g_ota_data_interval);
                }
            } else {
                // Wake watchdog expired: woke on RXMODE ON but no EXECUTE_PHASE3_CHILD/DATA followed. Go back to sleep but keep the bitmap and park in OTA_PHASE3_WAITING (terminal static, no polling) — a Leader Phase1 re-run or a Tool unicast is what brings us back.
                log_info("[Rafael] wake watchdog expired, going back to sleep "
                         "(bitmap kept)");
                if (g_mtd_was_sleepy) {
                    otLinkModeConfig cfg = {.mRxOnWhenIdle = false,
                                            .mNetworkData = false,
                                            .mDeviceType = false};
                    otThreadSetLinkMode(otrGetInstance(), cfg);
                }
                ota_change_state_and_timer(OTA_PHASE3_WAITING, 0);
            }
            break;
#if CONFIG_MIU_DEVICE_TYPE_FTD
        case OTA_PHASE2_CONFIRMING:
            if (g_phase2.try_count < OTA_PHASE2_TRY_MAX) {
                g_phase2.try_count++;
                log_info("[Rafael] Phase2: retry Router[%u/%u] attempt %u/%u",
                         g_phase2.current + 1, g_phase2.count,
                         g_phase2.try_count, OTA_PHASE2_TRY_MAX);
                ota_phase2_send_trigger(); // re-send to same Router (current unchanged): recovers a lost EXECUTE_PHASE2 under congestion
            } else {
                log_info("[Rafael] Phase2: skip Router[%u/%u] after %u retries",
                         g_phase2.current + 1, g_phase2.count,
                         OTA_PHASE2_TRY_MAX);
                g_phase2.current++;
                g_phase2.try_count = 0;
                ota_phase2_send_trigger();
            }
            break;
        case OTA_PHASE3_CONFIRMING:
            // A round can end naturally without stopping this timer — it was armed for the last child's trigger and keeps counting down in the background. If it fires afterward, g_phase3 is already zeroed (pending/done == NULL, active == false); retrying against it would scan with NULL bitmaps and flood "Invalid bitmap pointer" errors. Recognize that and settle to Idle instead.
            if (!g_phase3.active) {
                log_info("[Rafael] Phase3: confirm timeout with no active "
                         "round, going Idle");
                ota_change_state_and_timer(OTA_IDLE, 0);
                break;
            }
            if (g_phase3.try_count < OTA_PHASE3_TRY_MAX) {
                g_phase3.try_count++;
                log_info("[Rafael] Phase3: retry child idx[%u] attempt %u/%u "
                         "(%u remaining of %u)",
                         g_phase3.current, g_phase3.try_count,
                         OTA_PHASE3_TRY_MAX, g_phase3.remaining,
                         g_phase3.total);
                ota_phase3_send_trigger();
            } else {
                log_info("[Rafael] Phase3: skip child idx[%u] after %u retries",
                         g_phase3.current, OTA_PHASE3_TRY_MAX);
                ota_bitmap_set(g_phase3.done, g_phase3.current);
                g_phase3.remaining--;
                g_phase3.current++;
                g_phase3.try_count = 0;
                ota_phase3_send_trigger();
            }
            break;
#endif
        case OTA_DONE:
#if CONFIG_MIU_DEVICE_TYPE_FTD
            // Don't reboot out from under a Phase3 grace re-scan that is still pending — record the decision and let ota_phase3_finalize() commit it once the re-scan settles, instead of looping back through finish_ftd() into another full OTA_DONE_FALLBACK_TIMEOUT wait (this timeout already fired once).
            if (g_phase3.active || g_phase3_grace_pending) {
                log_info("[Rafael] OTA done timeout: Phase3 still running, "
                         "deferring decision");
                if (bin_type_matched) {
                    g_phase3_reboot_pending = true;
                    g_phase3_reboot_pending_time_ms = 10;
                }
                break;
            }
#endif
            // Fallback: bin_type_matched=true -> bootinfo+reboot (idempotent, covers devices skipped in Phase 3); false -> IDLE.
            if (bin_type_matched) {
                log_info("OTA done timeout: arming bootinfo + rebooting");
                ota_bootinfo_ready();
                ota_change_state_and_timer(OTA_REBOOT, 10);
            } else {
                log_info("OTA done timeout: returning to IDLE");
                ota_change_state_and_timer(OTA_IDLE, 0);
                if (g_mtd_was_sleepy) {
                    otLinkModeConfig cfg = {.mRxOnWhenIdle = false,
                                            .mNetworkData = false,
                                            .mDeviceType = false};
                    otThreadSetLinkMode(otrGetInstance(), cfg);
                }
            }
            break;
        case OTA_REBOOT: ota_reboot_handler(); break;
        default: break;
    }
}

static void ota_data_sended_event_handler(uint8_t* data, uint16_t lens) {
    otError error = OT_ERROR_NONE;
    otCoapCode CoapCode = OT_COAP_CODE_POST;
    // Use ota_sender_addr directly so multicast destinations are respected (multicast = first byte 0xff, any scope).
    otIp6Address coapDestinationIp = ota_sender_addr;
    otCoapType coapType = (ota_sender_addr.mFields.m8[0] == 0xff)
                              ? OT_COAP_TYPE_NON_CONFIRMABLE
                              : OT_COAP_TYPE_CONFIRMABLE;
    uint8_t* payload = (uint8_t*)data;
    uint16_t payloadLength = lens;
    error = ota_coap_request(CoapCode, coapDestinationIp, coapType, payload,
                             payloadLength, RAFAEL_OTA_URL_DATA);
}

static void ota_request_sended_event_handler(uint8_t* data, uint16_t lens) {
    static const uint8_t zero[OT_IP6_ADDRESS_SIZE] = {0};
    if (memcmp(g_unicast_req_dst.mFields.m8, zero, OT_IP6_ADDRESS_SIZE) == 0) {
        ota_printf("REQUEST: no unicast dst, skip");
        return;
    }
    ota_coap_request(OT_COAP_CODE_POST, g_unicast_req_dst,
                     OT_COAP_TYPE_NON_CONFIRMABLE, (uint8_t*)data, lens,
                     RAFAEL_OTA_URL_REQ);
}

static void ota_response_sended_event_handler(uint8_t* data, uint16_t lens) {
    otError error = OT_ERROR_NONE;
    otCoapCode CoapCode = OT_COAP_CODE_POST;
    otIp6Address coapDestinationIp;
    // Phase 2/3: send RESPONSE unicast to requester instead of ff02::1
    static const uint8_t zero[OT_IP6_ADDRESS_SIZE] = {0};
    otCoapType coapType;
    if (memcmp(g_unicast_resp_dst.mFields.m8, zero, OT_IP6_ADDRESS_SIZE) != 0) {
        coapDestinationIp = g_unicast_resp_dst;
        // NON_CONFIRMABLE: if requester is unreachable (e.g. different partition after network split), CON responses accumulate in the 6lo send queue and exhaust all 64 message buffers, deadlocking the Thread stack. Phase1 fill-in already retransmits at the REQUEST level, so delivery guarantee isn't needed here.
        coapType = OT_COAP_TYPE_NON_CONFIRMABLE;
    } else {
        char peer_addr[] = "ff02:0:0:0:0:0:0:1";
        error = otIp6AddressFromString(peer_addr, &coapDestinationIp);
        coapType = OT_COAP_TYPE_NON_CONFIRMABLE;
    }
    uint8_t* payload = (uint8_t*)data;
    uint16_t payloadLength = lens;
    error = ota_coap_request(CoapCode, coapDestinationIp, coapType, payload,
                             payloadLength, RAFAEL_OTA_URL_RESP);
}

static void ota_rxmode_sended_event_handler(otIp6Address dst_ipaddr,
                                            uint8_t* data, uint16_t lens) {
    otError error = OT_ERROR_NONE;
    otCoapCode CoapCode = OT_COAP_CODE_POST;
    otCoapType coapType = OT_COAP_TYPE_NON_CONFIRMABLE;
    error = ota_coap_request(CoapCode, dst_ipaddr, coapType, data, lens,
                             RAFAEL_OTA_URL_RXMODE);
}

static void ota_status_sended_event_handler(otIp6Address dst_ipaddr,
                                            uint8_t* data, uint16_t lens) {
    otError error = OT_ERROR_NONE;
    otCoapCode CoapCode = OT_COAP_CODE_POST;
    otCoapType coapType = OT_COAP_TYPE_NON_CONFIRMABLE;
    uint8_t* payload = (uint8_t*)data;
    uint16_t payloadLength = lens;
    error = ota_coap_request(CoapCode, dst_ipaddr, coapType, payload,
                             payloadLength, RAFAEL_OTA_URL_STATUS);
}

static void ota_execute_sended_event_handler(otIp6Address dst_ipaddr,
                                             uint8_t* data, uint16_t lens) {
    otError error = OT_ERROR_NONE;
    otCoapCode CoapCode = OT_COAP_CODE_POST;
    otCoapType coapType = OT_COAP_TYPE_NON_CONFIRMABLE;
    uint8_t* payload = (uint8_t*)data;
    uint16_t payloadLength = lens;
    error = ota_coap_request(CoapCode, dst_ipaddr, coapType, payload,
                             payloadLength, RAFAEL_OTA_URL_EXECUTE);
    ota_execute_t* exec = (ota_execute_t*)data;
    char dst_str[OT_IP6_ADDRESS_STRING_SIZE];
    otIp6AddressToString(&dst_ipaddr, dst_str, sizeof(dst_str));
    ota_printf("[Rafael] execute flag=%u sent to %s, coap error=%d",
               (lens >= sizeof(ota_execute_t)) ? exec->flag : 0xFF, dst_str,
               (int)error);
}

static void ota_data_received_event_handler(uint8_t* data, uint16_t lens,
                                            otIp6Address ipv6) {
    // Ignore all incoming data once we are committed to rebooting.
    if (g_ota_state == OTA_REBOOT) {
        return;
    }
    ota_data_t ota_data;
    uint32_t i = 0;
    uint32_t toatol_num = 0, timeout = 0, ota_data_lens = 0, tmp_addr = 0,
             crc32 = 0;

    do {
        if (ota_data_parse(OTA_PAYLOAD_TYPE_DATA, data, lens, &ota_data)) {
            break;
        }
        toatol_num = (ota_data.size / ota_data.segments);
        if (ota_data.size % ota_data.segments) {
            ++toatol_num;
        }

        if (ota_get_state() == OTA_IDLE
            && ota_get_image_version() == ota_data.version) {
            crc32 = crc32checksum((OTA_FLASH_START + OTA_INFO_HEADER_OFFSET),
                                  (ota_data.size - OTA_INFO_HEADER_OFFSET));
            if (crc32 == ota_get_image_crc()) {
                // Initiator bitmap set all
                if (g_ota_bitmap) {
                    ota_bitmap_delete(g_ota_bitmap);
                    g_ota_bitmap = NULL;
                }
                g_ota_bitmap = ota_bitmap_init(toatol_num);
                if (!g_ota_bitmap) {
                    log_error("[FOTA] bitmap init failed (line ~1429)");
                    ota_change_state_and_timer(OTA_IDLE, 0);
                    break;
                }
                for (i = 0; i < toatol_num; i++) {
                    ota_bitmap_set(g_ota_bitmap, i);
                }
                g_ota_total_num = toatol_num;
                g_ota_segments_size = ota_data.segments;
                ota_change_state_and_timer(OTA_DONE, OTA_DONE_TIMEOUT);
                bin_type_matched = false;
            }
        } else if (ota_get_state() == OTA_IDLE
                   && GET_BIN_VERSION(systeminfo.sysinfo) == ota_data.version) {
            // Running firmware already matches OTA target — skip download
            log_info("[Rafael] OTA skip: running v0x%08x matches incoming, "
                     "going Done",
                     ota_data.version);
            g_ota_image_version = ota_data.version;
            g_ota_image_size = ota_data.size;
            g_ota_image_crc = ota_data.crc;
            g_ota_total_num = toatol_num;
            g_ota_segments_size = ota_data.segments;
            ota_change_state_and_timer(OTA_DONE, OTA_DONE_TIMEOUT);
            bin_type_matched = false;
#if CONFIG_MIU_DEVICE_TYPE_MTD
            // Notify Leader so the OTA tool can see this MTD (already at target version)
            {
                otIp6Address leader_addr;
                if (otThreadGetLeaderRloc(otrGetInstance(), &leader_addr)
                    == OT_ERROR_NONE) {
                    ota_push_status_to(leader_addr);
                }
            }
#endif
        }

        // Phase1 restart: a multicast DATA arriving while we are NOT already mid-Phase1 means the Leader started a fresh round, so whatever we were holding is superseded — drop it and re-download onto erased flash, same version or new. OTA_PHASE1_RECEIVING is the only non-interruptible state; the one other exception is already holding this exact image complete (the same-version shortcut above), which must stay a no-op.
        if (ota_data.is_unicast == false && g_ota_bitmap != NULL
            && ota_get_state() != OTA_IDLE
            && ota_get_state() != OTA_PHASE1_RECEIVING
            && !(ota_get_image_version() == ota_data.version
                 && ota_bitmap_get_remain(g_ota_bitmap, g_ota_total_num)
                        == 0)) {
            log_info(
                "[Rafael] Phase1 restart from %s, erasing and re-downloading",
                OtaStateToString(ota_get_state()));
            g_ota_image_version =
                0; // let the version guard below fall through to the cold-start path
            bool was_sleepy = g_mtd_was_sleepy;
            bool was_woken = g_ota_prewake_woken;
            ota_change_state_and_timer(OTA_IDLE, 0);
            // OTA_IDLE wipes both, but they describe this device's radio, not the round we just dropped — a SED that woke for the previous round must still know to go back to sleep.
            g_mtd_was_sleepy = was_sleepy;
            g_ota_prewake_woken = was_woken;
        }

        // Reject an off-version packet only while we actually hold a download in progress (bitmap != NULL); a just-woken straggler with no bitmap is NOT downloading, so a different version means a fresh round it must start receiving, not be blocked from (the mass-0% straggler bug).
        if (NULL != g_ota_bitmap && OTA_IDLE != ota_get_state()
            && 0 != ota_get_image_version()
            && ota_get_image_version() != ota_data.version) {
            ota_printf("different version %08X %08X ", ota_data.version,
                       ota_get_image_version());
            break;
        }

        {
            uint8_t _cur = ota_get_state();
            if (_cur == OTA_DONE) {
                ota_change_state_and_timer(OTA_DONE, OTA_DONE_TIMEOUT);
                break;
            }
#if CONFIG_MIU_DEVICE_TYPE_FTD
            if (_cur == OTA_PHASE2_CONFIRMING
                || _cur == OTA_PHASE3_CONFIRMING) {
                break; // Ignore incoming data in post-Phase1 FTD states
            }
#endif
        }

        // Not FTD/MTD-specific: a device that never went through the wake path this round (g_ota_prewake_woken still false) can use the fixed formula; any woken/sleepy participant needs the dynamic sleep-aware one.
        if (!g_ota_prewake_woken) {
            timeout = 1 + (ota_data.interval * 2 * (toatol_num - ota_data.seq));
        } else {
            // Phase0 pre-wake dynamic sleep calc: remaining wait = (total_segments - max_seq_seen + 5) * interval.
            if (ota_data.seq > g_phase1_max_seq_seen) {
                g_phase1_max_seq_seen = ota_data.seq;
            }
            timeout = 1
                      + ((uint32_t)(toatol_num - g_phase1_max_seq_seen + 5)
                         * ota_data.interval);
        }
        ota_data_lens = ota_data.segments;
        if (ota_data.seq == (toatol_num - 1)) {
            ota_data_lens = ota_data.size % ota_data.segments;
        }

        if (NULL != g_ota_bitmap) {
            if (g_ota_state == OTA_IDLE) {
                ota_printf("ota data OTA_IDLE");
                break;
            }
            if (ota_bitmap_get_bit(g_ota_bitmap, ota_data.seq)) {
                ota_printf("ota upgrade data same %u", ota_data.seq);
                break;
            }
            // Resuming a retained bitmap via unicast (same-version WAITING straggler): remember the sender so completion counts as was_phase3 and reboots, even when only one segment was missing. No erase — keep the bits we already have.
            if (ota_data.is_unicast == true) {
                g_unicast_req_dst = ipv6;
            }
        } else {
            if (ota_data.is_unicast == true && ota_data.seq != 0) {
                ota_printf("is not seq 0 ");
                break;
            }
            g_ota_image_version = ota_data.version;
            g_ota_image_size = ota_data.size;
            g_ota_image_crc = ota_data.crc;
            g_ota_segments_size = ota_data.segments;
            g_ota_data_interval = ota_data.interval;
            g_ota_total_num = toatol_num;
            ota_bootinfo_reset();
            g_ota_bitmap = ota_bitmap_init(toatol_num);
            if (!g_ota_bitmap) {
                log_error("[FOTA] bitmap init failed");
                ota_change_state_and_timer(OTA_IDLE, 0);
                break;
            }
            if (ota_data.is_unicast == true) {
                // Direct single-device unicast download: remember the sender so a later gap-fill REQUEST has a destination instead of silently no-opping — same bug class as the Phase1 Router self-trigger.
                g_unicast_req_dst = ipv6;
                // Fixed (not the shrinking Phase1 formula): a scattered gap-fill must survive normal inter-round push gaps; each received segment re-arms it below.
                ota_change_state_and_timer(OTA_UNICAST_RECEIVING,
                                           OTA_UNICAST_RECV_WATCHDOG_TIMEOUT);
            } else {
                ota_change_state_and_timer(OTA_PHASE1_RECEIVING, timeout);
            }
            ota_erase_image_flash();
        }

        ota_printf("[R] ota data seq %u remain %u", ota_data.seq,
                   ota_bitmap_get_remain(g_ota_bitmap, toatol_num));

        if (ota_data.seq >= toatol_num) {
            ota_printf("[R] seq %u out of range (total %u), drop", ota_data.seq,
                       toatol_num);
            break;
        }
        ota_bitmap_set(g_ota_bitmap, ota_data.seq);

        enter_critical_section();
        tmp_addr = OTA_FLASH_START + (ota_data.segments * ota_data.seq);
        for (i = 0; i < ota_data_lens; i++) {
            while (flash_check_busy())
                ;
            flash_write_byte(tmp_addr + i, ota_data.data[i]);
            while (flash_check_busy())
                ;
        }
        leave_critical_section();

        if (0 == ota_bitmap_get_remain(g_ota_bitmap, toatol_num)) {
            crc32 = crc32checksum((OTA_FLASH_START + OTA_INFO_HEADER_OFFSET),
                                  (ota_data.size - OTA_INFO_HEADER_OFFSET));
            if (crc32 != ota_data.crc) {
                ota_printf("ota data upgrade fail %X %X", crc32, ota_data.crc);
                ota_change_state_and_timer(OTA_IDLE, 0);
            } else {
                // Enter OTA_DONE quiet period; bootinfo armed only if bin_type matches.
                ota_check_bin_type();
            }
            break;
        } else {
            if (ota_data.is_unicast == true) {
                // Re-arm the fixed unicast watchdog on every segment; do NOT use the shrinking Phase1 timeout here.
                ota_change_state_and_timer(OTA_UNICAST_RECEIVING,
                                           OTA_UNICAST_RECV_WATCHDOG_TIMEOUT);
            } else {
                ota_change_state_and_timer(OTA_PHASE1_RECEIVING, timeout);
            }
        }

    } while (0);
}

#if CONFIG_MIU_DEVICE_TYPE_FTD
// Parent-side truth for direct-vs-indirect tx: OT pushes DIRECT only when this child's registered mode is rxOn. A child that just sent its bitmap (uplink TX) proves it is awake, NOT that the parent's table already flipped to rxOn — the MLE Child Update carrying that mode change is a separate exchange that can still be in flight (or lost). While the table says rxOff the RESPONSE goes indirect-queue (shallow, mostly overwritten before a poll retrieves it), which matches "27 pushed, 5 landed". Returns 1=rxOn, 0=rxOff, -1=not in child table.
static int8_t ota_child_rxon_in_table(uint16_t rloc16) {
    otChildInfo ci;
    uint16_t maxc = otThreadGetMaxAllowedChildren(otrGetInstance());
    for (uint16_t i = 0; i < maxc; i++) {
        if (otThreadGetChildInfoByIndex(otrGetInstance(), i, &ci)
                != OT_ERROR_NONE
            || ci.mIsStateRestoring) {
            continue;
        }
        if (ci.mRloc16 == rloc16) {
            return ci.mRxOnWhenIdle ? 1 : 0;
        }
    }
    return -1;
}

// Parent (pusher): a child sent its bitmap on ota/req -> store it, aim RESPONSE at this child, and start pushing the segs it still misses. Replaces the old req_table responder.
static void ota_request_received_event_handler(uint8_t* data, uint16_t lens,
                                               otIp6Address src_ipv6) {
    ota_bitmap_report_t rep;
    if (ota_data_parse(OTA_PAYLOAD_TYPE_BITMAP, data, lens, &rep)) {
        return;
    }
    if (rep.version != ota_get_image_version()) {
        ota_printf("push: bitmap ver mismatch %x %x", rep.version,
                   ota_get_image_version());
        return;
    }
    // Invariant: only serve once we hold the complete image.
    if (g_ota_bitmap == NULL
        || ota_bitmap_get_remain(g_ota_bitmap, g_ota_total_num) != 0) {
        ota_printf("push: self image incomplete, can't serve bitmap");
        return;
    }
    uint16_t nbytes = (uint16_t)(((rep.total_num + 31) / 32) * 4);
    if (rep.byte_len < nbytes) {
        nbytes = rep.byte_len; // single-chunk assumed; trust only what arrived
    }
    ota_bitmap_delete(g_child_bitmap);
    g_child_bitmap = ota_bitmap_init(rep.total_num);
    if (!g_child_bitmap) {
        log_error("[Rafael] child bitmap init failed");
        return;
    }
    memcpy((uint8_t*)g_child_bitmap, rep.data, nbytes);
    g_child_bitmap_total = rep.total_num;
    g_unicast_resp_dst = src_ipv6;

    uint16_t src_rloc = ((uint16_t)src_ipv6.mFields.m8[14] << 8)
                        | src_ipv6.mFields.m8[15];
    // Only the node the confirm loop is currently pointing at may move that loop. Anyone else (duplicate trigger, straggler that woke late) still gets served, but must never advance or skip the slot belonging to a different node.
    bool is_current_target =
        (g_phase2.active && g_phase2.current < g_phase2.count
         && src_rloc == g_phase2.rlocs[g_phase2.current])
        || (g_phase3.active && src_rloc == g_phase3.current_rloc);

    if (is_current_target) {
        // A bitmap is proof of life, so the confirm retries this target already burned don't count against it.
        g_phase3.try_count = 0;
    }
    uint32_t miss_now = ota_bitmap_get_remain(g_child_bitmap, rep.total_num);
    if (src_rloc != g_push_round_rloc) {
        g_push_round_rloc = src_rloc;
        g_push_round = 0;
        g_push_last_missing = 0xFFFF;
    }
    // Progress-based give-up: a round that shrank the target's missing count doesn't count against it (reset the no-progress counter). A steadily-filling child (each bitmap shows fewer gaps) is never abandoned mid-fill; only a genuinely stalled one — missing not dropping — accumulates rounds and bails after OTA_PUSH_ROUND_MAX dry rounds.
    if (miss_now < g_push_last_missing) {
        g_push_round = 0;
    } else if (++g_push_round > OTA_PUSH_ROUND_MAX) {
        log_info("[Rafael] push: rloc=0x%04x no progress for %u rounds "
                 "(missing=%u), giving up on it",
                 src_rloc, OTA_PUSH_ROUND_MAX, (unsigned)miss_now);
        if (is_current_target) {
            ota_push_skip_target();
        } else {
            ota_push_free_child_bitmap();
        }
        return;
    }
    g_push_last_missing = (uint16_t)miss_now;

    int32_t first = ota_child_next_missing(0);
    if (first < 0) {
        if (is_current_target) {
            ota_push_end_target(); // child already complete
        } else {
            ota_push_free_child_bitmap();
        }
        return;
    }
    g_ota_start_index = (uint32_t)first;
    g_ota_unicast_try_cnt = 0;
    // 3rd-layer probe: the child's registered mode in OUR table decides direct vs indirect. rxon=1 -> direct (loss should now be flash/absorb only); rxon<=0 -> parent still has it sleepy so this whole batch goes indirect-queue and most gets dropped regardless of pacing. Log-only for now: this rxon value is what disambiguates the next test's root cause; we don't defer (deferring risks missing the child's short awake window and there is no proof the parent's table would settle before it sleeps).
    int8_t rxon = ota_child_rxon_in_table(src_rloc);
    // Only rxon==0 is actionable (child IS in our table but registered sleepy -> push goes indirect-queue and mostly drops). rxon==-1 just means the target is not our child (router / multi-hop / behind another parent) which is normal, not a drop cause -> do not WARN on it.
    if (rxon == 0) {
        log_info("[Rafael] push WARN: rloc=0x%04x rxon=0 (parent table not "
                 "rxOn -> push goes INDIRECT, expect heavy drop)",
                 src_rloc);
    }
    log_info("[Rafael] push start: rloc=0x%04x round=%u/%u total=%u missing=%u "
             "first=%u rxon=%d",
             src_rloc, g_push_round, OTA_PUSH_ROUND_MAX, rep.total_num,
             (unsigned)miss_now, (unsigned)first, rxon);
    ota_change_state_and_timer(OTA_PHASE3_REPAIRING, 1);
}
#endif

static void ota_response_received_event_handler(uint8_t* data, uint16_t lens) {
    ota_response_t ota_response;
    uint16_t i = 0, ota_data_lens = 0;
    uint32_t toatol_num = 0, tmp_addr = 0, crc32 = 0;
    do {
        if (ota_data_parse(OTA_PAYLOAD_TYPE_RESPONSE, data, lens,
                           &ota_response)) {
            break;
        }
        toatol_num = ota_response.size / ota_response.segments;

        if (ota_response.size % ota_response.segments) {
            ++toatol_num;
        }

        if (g_ota_state == OTA_IDLE) {
            ota_printf("ota response %s", OtaStateToString(g_ota_state));
            if (ota_get_image_version() == ota_response.version) {
                crc32 = crc32checksum(
                    (OTA_FLASH_START + OTA_INFO_HEADER_OFFSET),
                    (ota_response.size - OTA_INFO_HEADER_OFFSET));
                if (crc32 == ota_get_image_crc()) {
                    // Initiator bitmap set all
                    ota_printf("response same version and idle ");
                    if (g_ota_bitmap) {
                        ota_bitmap_delete(g_ota_bitmap);
                        g_ota_bitmap = NULL;
                    }
                    g_ota_bitmap = ota_bitmap_init(toatol_num);
                    if (!g_ota_bitmap) {
                        log_error("[FOTA] bitmap init failed");
                        ota_change_state_and_timer(OTA_IDLE, 0);
                        break;
                    }
                    for (i = 0; i < toatol_num; i++) {
                        ota_bitmap_set(g_ota_bitmap, i);
                    }
                    g_ota_total_num = toatol_num;
                    g_ota_segments_size = ota_response.segments;
                    ota_change_state_and_timer(OTA_DONE, OTA_DONE_TIMEOUT);
                    bin_type_matched = false;
                } else {
                    ota_printf("crc fail %x %x ", crc32, ota_get_image_crc());
                    if (g_ota_bitmap) {
                        ota_bitmap_delete(g_ota_bitmap);
                        g_ota_bitmap = NULL;
                    }
                    g_ota_bitmap = ota_bitmap_init(toatol_num);
                    if (!g_ota_bitmap) {
                        log_error("[FOTA] bitmap init failed");
                        ota_change_state_and_timer(OTA_IDLE, 0);
                        break;
                    }
                    g_ota_total_num = toatol_num;
                    g_ota_segments_size = ota_response.segments;
                    ota_bootinfo_reset();
                    ota_erase_image_flash();
                    ota_change_state_and_timer(
                        g_ota_state,
                        (OTA_REQUEST_TABLE_SIZE * OTA_UNICAST_WAIT_MS));
                    break;
                }
            } else {
                break;
            }
        }

        if (ota_response.segments != g_ota_segments_size) {
            ota_printf("ota response segments fail %u %u ",
                       ota_response.segments, g_ota_segments_size);
            break;
        }

        if (NULL == g_ota_bitmap) {
            ota_printf("ota response bitmap NULL");
            break;
        }

        if (ota_response.version != g_ota_image_version) {
            ota_printf("ota response differnt version %x %x ",
                       ota_response.version, g_ota_image_version);
            break;
        }
        //have other node not upgrade, need wait — re-arm current confirming state
        if (g_ota_state == OTA_DONE
#if CONFIG_MIU_DEVICE_TYPE_FTD
            || g_ota_state == OTA_PHASE2_CONFIRMING
            || g_ota_state == OTA_PHASE3_CONFIRMING
#endif
        ) {
#if CONFIG_MIU_DEVICE_TYPE_FTD
            ota_change_state_and_timer(g_ota_state,
                                       (g_phase2.active || g_phase3.active)
                                           ? 30000
                                           : OTA_DONE_TIMEOUT);
#else
            ota_change_state_and_timer(OTA_DONE, OTA_DONE_TIMEOUT);
#endif
        }
        enter_critical_section();
        for (i = 0; i < OTA_RESPONSE_TABLE_SIZE; i++) {
            if (g_resp_table[i] == ota_response.seq) {
                if (xTimerIsTimerActive(ota_resp_timer)) {
                    if (++g_ota_response_c >= g_ota_response_k) {
                        g_ota_response_c = 0;
                        xTimerStop(ota_resp_timer, 0);
                    }
                }
                g_resp_table[i] = 0xFFFF;
                break;
            }
        }
        g_ota_request_cnt = 0;
        leave_critical_section();
        if (ota_bitmap_get_bit(g_ota_bitmap, ota_response.seq)) {
            // ota_printf("ota response seq same %u",ota_response_header->seq);
            break;
        } else {
            // Reuse the sender-side length helper (has the `size % segments == 0 -> segments` guard); the old inline `size % segments` wrote 0 bytes for the last seg when size was an exact multiple, dropping a whole packet.
            ota_data_lens = calc_payload_data_len(
                ota_response.seq, ota_response.size, ota_response.segments);

            ota_bitmap_set(g_ota_bitmap, ota_response.seq);
            ota_printf("[R] ota response %u remain %u", ota_response.seq,
                       ota_bitmap_get_remain(g_ota_bitmap, toatol_num));

            enter_critical_section();
            tmp_addr = OTA_FLASH_START
                       + (ota_response.segments * ota_response.seq);
            // ota_bitmap_print(ota_bitmap,toatol_num);

            for (i = 0; i < ota_data_lens; i++) {
                while (flash_check_busy())
                    ;
                flash_write_byte(tmp_addr + i, ota_response.data[i]);
                while (flash_check_busy())
                    ;
            }
            leave_critical_section();
            // A pushed seg landed -> reset the no-data watchdog; the batch is unacked, the sender re-asks for our bitmap when it ends.
            if (g_ota_state == OTA_PHASE1_FILLING
                || g_ota_state == OTA_PHASE2_FILLING
                || g_ota_state == OTA_PHASE3_FILLING) {
                xTimerChangePeriod(
                    ota_timer, pdMS_TO_TICKS(OTA_MTD_WAKE_WATCHDOG_TIMEOUT), 0);
            }
            if (0 == ota_bitmap_get_remain(g_ota_bitmap, toatol_num)) {
                crc32 = crc32checksum(
                    (OTA_FLASH_START + OTA_INFO_HEADER_OFFSET),
                    (ota_response.size - OTA_INFO_HEADER_OFFSET));
                if (crc32 != ota_response.crc) {
                    ota_printf("ota response upgrade fail %X %X", crc32,
                               ota_response.crc);
                    ota_change_state_and_timer(OTA_IDLE, 0);
                    // Erase right here too (belt-and-suspenders alongside the cold-start erase) — don't leave this failed attempt's bytes in flash until the next trigger re-enters the cold-start path.
                    ota_erase_image_flash();
                } else {
                    ota_check_bin_type();
                }
                break;
            }
        }
    } while (0);
}

static void ota_rxmode_received_event_handler(uint8_t* data, uint16_t lens) {
    /*data is null*/
    otDeviceRole changeRole = otThreadGetDeviceRole(otrGetInstance());
    ota_rxmode_t* rxmode = (ota_rxmode_t*)data;
#if CONFIG_MIU_DEVICE_TYPE_FTD
    if (changeRole == OT_DEVICE_ROLE_ROUTER
        || changeRole == OT_DEVICE_ROLE_LEADER) {
        uint16_t maxChildrens = 0, sleep_node = 0;
        otChildInfo childInfo;
        maxChildrens = otThreadGetMaxAllowedChildren(otrGetInstance());
        for (uint16_t i = 0; i < maxChildrens; i++) {
            if ((otThreadGetChildInfoByIndex(otrGetInstance(), i, &childInfo)
                 != OT_ERROR_NONE)
                || childInfo.mIsStateRestoring) {
                continue;
            }
            if (childInfo.mRxOnWhenIdle == false) {
                otIp6Address dst_ipaddr = *otThreadGetRloc(otrGetInstance());
                dst_ipaddr.mFields.m8[14] = (childInfo.mRloc16 >> 8) & 0xff;
                dst_ipaddr.mFields.m8[15] = childInfo.mRloc16 & 0xff;
                ota_rxmode_sended_event_handler(dst_ipaddr, data, lens);
                ++sleep_node;
            }
        }
        ota_printf("sleep node %u ", sleep_node);
    } else
#endif
    {
        // OTA_PHASE3_WAITING is the parked, timer-less state a give-up lands in; a SED sitting there with rx off can only be brought back by this pre-wake, so it must not be filtered out with the busy states.
        if (ota_get_state() != OTA_IDLE
            && ota_get_state() != OTA_PHASE3_WAITING) {
            ota_printf("ota_rxmode_received state %s ",
                       OtaStateToString(ota_get_state()));
            return;
        }
        // Wake/sleep the radio first regardless of version; the same-version check only applies to actual data download
        if (rxmode->OnWhenIdle == true) {
            // Phase0 pre-wake: img_bin_type tells us which binary this round is for. A mismatched, non-zero type means this image isn't for this device — stay asleep and skip the rest of this message (0 = unknown/legacy sender, treated as wildcard match).
            {
                static const uint8_t zero_bin[12] = {0};
                const uint8_t* self_bin = GET_BIN_TYPE_PTR(systeminfo.sysinfo);
                if (memcmp(rxmode->img_bin_type, zero_bin, 12) != 0
                    && memcmp(rxmode->img_bin_type, self_bin, 12) != 0) {
                    ota_printf("[Rafael] RXMODE: img_bin_type mismatch, "
                               "staying asleep");
                    return;
                }
            }
            // "Stay asleep, we already hold this" is a conclusion ONLY OTA_IDLE may draw: a PHASE3_WAITING device parked here precisely because its image is INCOMPLETE, so it must always wake to keep filling (same round = finish it, different version = restart) — skipping it on a version match would strand a straggler forever (the 54%-park failure). And in IDLE compare the real running firmware (systeminfo), NOT g_ota_image_version: that staged copy is 0 after any plain power-cycle, which would let an already-updated SED get re-woken. Mirrors the DATA-path same-version skip (GET_BIN_VERSION == ota_data.version).
            if (ota_get_state() == OTA_IDLE) {
                const uint8_t* self_bin = GET_BIN_TYPE_PTR(systeminfo.sysinfo);
                if (rxmode->version != 0
                    && GET_BIN_VERSION(systeminfo.sysinfo) == rxmode->version
                    && memcmp(rxmode->img_bin_type, self_bin, 12) == 0) {
                    ota_printf("ota_rxmode same ver+type, stay asleep %x",
                               rxmode->version);
                    return;
                }
            }
            bool _was_sleeping =
                (otThreadGetLinkMode(otrGetInstance()).mRxOnWhenIdle == false);
            // Record whether we were genuinely asleep the first time a wakeup arrives (gated to OTA_IDLE above, fires once per session) — later re-sleeps only apply if this was true.
            g_mtd_was_sleepy = _was_sleeping;
            if (_was_sleeping) {
                ota_printf("wake up ");
                otLinkModeConfig config;
                config.mRxOnWhenIdle = true;
                config.mNetworkData = false;
                config.mDeviceType = false;

                if (otThreadSetLinkMode(otrGetInstance(), config)
                    != OT_ERROR_NONE) {
                    ota_printf("wake up set fail");
                }
            }
            // MTD wake watchdog: we're now awake waiting for EXECUTE_PHASE3_CHILD/DATA, but nothing guarantees it arrives — Router may give up (retry exhaustion) without notifying us, or the trigger packet may be lost. Without a self-timer we'd stay RxOnWhenIdle=true forever, draining the SED's battery. Arm OTA_PHASE0_PREWAKE purely as a watchdog; any subsequent DATA/EXECUTE_PHASE3_CHILD reception transitions state away before it fires, so this only fires when truly nothing follows. This device is a wake recipient, not the round's initiator — mark it so the timer handler goes back to sleep on expiry instead of starting Phase1 SENDING.
            g_ota_prewake_woken = true;
            // Wake watchdog = same Phase1 dynamic sleep calc (stay awake the whole round + 5-interval buffer, from the total_segments/interval RXMODE carried); a fixed timeout sleeps before Phase1 data even starts.
            uint32_t wake_timeout = OTA_MTD_WAKE_WATCHDOG_TIMEOUT;
            if (rxmode->total_segments > 0 && rxmode->interval > 0) {
                wake_timeout = 1
                               + ((uint32_t)(rxmode->total_segments
                                             - g_phase1_max_seq_seen + 5)
                                  * rxmode->interval);
            }
            ota_change_state_and_timer(OTA_PHASE0_PREWAKE, wake_timeout);
        } else {
            if (otThreadGetLinkMode(otrGetInstance()).mRxOnWhenIdle == true) {
                ota_printf("go to sleep ");
                otLinkModeConfig config;
                config.mRxOnWhenIdle = false;
                config.mNetworkData = false;
                config.mDeviceType = false;

                if (otThreadSetLinkMode(otrGetInstance(), config)
                    != OT_ERROR_NONE) {
                    ota_printf("go to sleep set fail");
                }
            }
        }
    }
}

static void ota_status_received_event_handler(uint8_t* data, uint16_t lens,
                                              otIp6Address src_ipv6) {
    uint8_t flag = data[0];
    if (flag == OTA_STATUS_GET) {
        // Record tool IPv6 for proactive push on DONE/REBOOT
        memcpy(&g_ota_push_target, &src_ipv6, sizeof(otIp6Address));
        ota_status_get_t* ota_status_get = (ota_status_get_t*)data;
        bool need_send_version = false;
#if CONFIG_MIU_DEVICE_TYPE_FTD
        if (ota_status_get->device_type == OTA_STATUS_TYPE_FTD) {
            need_send_version = true;
        }
#endif
#if CONFIG_MIU_DEVICE_TYPE_MTD
        if (ota_status_get->device_type == OTA_STATUS_TYPE_MTD) {
            need_send_version = true;
        }
#endif
        else if (ota_status_get->device_type == OTA_STATUS_TYPE_ALL) {
            need_send_version = true;
        } else {
            ota_printf("unknow ota staus device type");
        }
        if (need_send_version == true) {
            if (xTimerIsTimerActive(ota_report_timer) == pdFALSE) {
                enter_critical_section();
                g_ota_status_report.flag = OTA_STATUS_REPORT;
                if (ota_get_state() == OTA_IDLE) {
                    g_ota_status_report.version = GET_BIN_VERSION(
                        systeminfo.sysinfo);
                } else {
                    g_ota_status_report.version = ota_get_image_version();
                }
                g_ota_status_report.rxmode =
                    otThreadGetLinkMode(otrGetInstance()).mRxOnWhenIdle;
                g_ota_status_report.state = ota_get_state();
                memcpy(g_ota_status_report.bin_type,
                       GET_BIN_TYPE_PTR(systeminfo.sysinfo), 12);
                memcpy(&g_ota_status_report.dstipv6.mFields.m8,
                       &src_ipv6.mFields.m8, OT_IP6_ADDRESS_SIZE);
                memcpy(&g_ota_status_report.src_mleid,
                       otThreadGetMeshLocalEid(otrGetInstance()),
                       sizeof(otIp6Address));
                g_ota_status_report.state_remain_ms = ota_get_timer_remain_ms();
                g_ota_status_report.state_enter_tick = g_state_enter_tick;
                g_ota_status_report.progress_bar = ota_compute_progress_bar();
                g_ota_status_report.rloc16 = otThreadGetRloc16(
                    otrGetInstance());
                leave_critical_section();
                uint16_t random_time = otRandomNonCryptoGetUint16InRange(
                    1, ota_status_get->report_time);

                ota_printf("send staus report %d ms", random_time);
                xTimerChangePeriod(
                    ota_report_timer, random_time,
                    0); // arm+start the pre-created timer; no heap alloc on this path
            } else {
                ota_printf("ota_report_timer exist");
            }
        }
    } else if (flag == OTA_STATUS_REPORT) {
        ota_status_report_t* ota_status_report = (ota_status_report_t*)data;
        char mleid_str[OT_IP6_ADDRESS_STRING_SIZE];
        char src_str[OT_IP6_ADDRESS_STRING_SIZE];
        otIp6AddressToString(&src_ipv6, src_str, sizeof(src_str));
        uint32_t remain_ms = 0;
        uint32_t enter_tick = 0;
        uint16_t rloc16 =
            0xFFFF; // 0xFFFF = unknown (old firmware, no rloc16 in payload)
        if (lens >= sizeof(ota_status_report_t)) {
            otIp6AddressToString(&ota_status_report->src_mleid, mleid_str,
                                 sizeof(mleid_str));
            remain_ms = ota_status_report->state_remain_ms;
            enter_tick = ota_status_report->state_enter_tick;
            rloc16 = ota_status_report->rloc16;
        } else {
            // old firmware: no mleid/remain_ms/rloc16 in payload, fall back to CoAP source
            memcpy(mleid_str, src_str, sizeof(src_str));
        }
        log_info("OTA_STATUS|%s|%08x|%s|%u|%u%%|%.12s|%ums|%utk|rloc=0x%04x",
                 mleid_str, ota_status_report->version,
                 OtaStateToString(ota_status_report->state),
                 ota_status_report->rxmode, ota_status_report->progress_bar,
                 ota_status_report->bin_type, (unsigned)remain_ms,
                 (unsigned)enter_tick, (unsigned)rloc16);
#if CONFIG_MIU_DEVICE_TYPE_FTD
        // Phase 2/3: current target reports done (100%) OR reports Idle with a bin_type that doesn't match ours (mismatch/not participating this round) -> advance to next. Without the Idle case, a mismatched child's proactive report never reaches the rloc-match check below, so the Router still burns the full 3x30s retry per child. The bin_type comparison (vs. accepting any Idle report) avoids false-triggering on an unrelated Idle status (e.g. a bystander's proactive report).
        uint8_t self_round_bin_type[12];
        ota_flash_read_bin_type(self_round_bin_type);
        bool mismatch_idle_report = (ota_status_report->state == OTA_IDLE)
                                    && (memcmp(ota_status_report->bin_type,
                                               self_round_bin_type, 12)
                                        != 0);
        // Symmetric to mismatch_idle_report: a child already running THIS round's target (matching bin_type + version) now reports Idle instead of churning to Done. Treat that Idle as "confirmed, advance" — otherwise the grace re-scan burns the full ~20s confirm timeout on every already-finished child.
        bool match_idle_done =
            (ota_status_report->state == OTA_IDLE) && (g_ota_image_version != 0)
            && (ota_status_report->version == g_ota_image_version)
            && (memcmp(ota_status_report->bin_type, self_round_bin_type, 12)
                == 0);
        if (ota_status_report->progress_bar == 100 || mismatch_idle_report
            || match_idle_done) {
            uint16_t src_rloc = ((uint16_t)src_ipv6.mFields.m8[14] << 8)
                                | src_ipv6.mFields.m8[15];
            if (g_phase2.active && g_phase2.current < g_phase2.count
                && src_rloc == g_phase2.rlocs[g_phase2.current]) {
                log_info("[Rafael] Phase2: Router[%u/%u] rloc=0x%04x confirmed",
                         g_phase2.current + 1, g_phase2.count, src_rloc);
                g_phase2.current++;
                g_phase2.try_count = 0;
                ota_phase2_send_trigger();
            } else if (g_phase3.active
                       && g_phase3.current < g_phase3.max_children) {
                // Match against the rloc captured when the trigger was sent (g_phase3.current_rloc), not a fresh child-table re-query by index — the child table can reorder between trigger and reply (e.g. an unrelated Router reboot churns the mesh), which would silently break an index-based re-lookup even though the reply is legitimately from the target child.
                if (src_rloc == g_phase3.current_rloc) {
                    log_info("[Rafael] Phase3: child idx[%u] rloc=0x%04x "
                             "confirmed (%u remaining of %u)",
                             g_phase3.current, src_rloc, g_phase3.remaining - 1,
                             g_phase3.total);
                    ota_bitmap_set(g_phase3.done, g_phase3.current);
                    g_phase3.remaining--;
                    g_phase3.current++;
                    g_phase3.try_count = 0;
                    ota_phase3_send_trigger();
                }
            }
        }
#endif
    } else {
        ota_printf("unknow ota staus flag");
    }
}

static void ota_execute_received_event_handler(uint8_t* data, uint16_t lens,
                                               otIp6Address src_ipv6) {
    ota_execute_t* ota_execute = (ota_execute_t*)data;
    if (lens < sizeof(ota_execute_t)) {
        return;
    }
    {
        char src_str[OT_IP6_ADDRESS_STRING_SIZE];
        otIp6AddressToString(&src_ipv6, src_str, sizeof(src_str));
        ota_printf("[Rafael] execute flag=%u received from %s, my state=%s",
                   ota_execute->flag, src_str,
                   OtaStateToString(ota_get_state()));
    }
    if (ota_execute->flag == OTA_EXECUTE_REBOOT) {
#if CONFIG_MIU_DEVICE_TYPE_FTD
        // Fix A': check BEFORE the OTA_DONE gate. A Router still serving its own
        // Phase3 isn't OTA_DONE yet, so the old code hit "state is not done" and
        // DISCARDED the command — it then rode the 480s fallback (the "stuck ~400s"
        // symptom). Remember it here and honor it in ota_phase3_finalize().
        if (g_phase3.active || g_phase3_grace_pending) {
            log_info("[Rafael] EXECUTE_REBOOT deferred: Phase3 still running "
                     "(state=%s)",
                     OtaStateToString(ota_get_state()));
            g_phase3_reboot_pending = true;
            g_phase3_reboot_pending_time_ms = ota_execute->time;
            return;
        }
#endif
        if (ota_get_state() == OTA_DONE) {
            uint32_t crc32 = crc32checksum(
                (OTA_FLASH_START + OTA_INFO_HEADER_OFFSET),
                (g_ota_image_size - OTA_INFO_HEADER_OFFSET));
            if (crc32 != g_ota_image_crc) {
                ota_printf("ota request upgrade fail %X %X", crc32,
                           g_ota_image_crc);
                ota_change_state_and_timer(OTA_IDLE, 0);
            } else {
                ota_bootinfo_ready();
                ota_change_state_and_timer(OTA_REBOOT, ota_execute->time);
                ota_printf("%u ms after reboot", ota_execute->time);
            }
        } else {
            ota_printf("state is not done");
        }
    } else if (ota_execute->flag == OTA_EXECUTE_PHASE3) {
        // Reused for two purposes: (a) Phase 0 pre-wake predicate — Router relays a pre-wake RXMODE to every sleeping child, unconditionally, safe regardless of state since it only wakes radios; (b) legacy Phase 2-done trigger, kept for backward compat, gated on state==OTA_DONE which can't be true during Phase 0.
#if CONFIG_MIU_DEVICE_TYPE_FTD
        // Use the round version carried in this broadcast, not our own g_ota_image_version — Phase1 hasn't reached us yet either, so our own copy is still whatever was left from our last completed round.
        const uint32_t* meta = (const uint32_t*)ota_execute->bin_type;
        ota_prewake_sleeping_children(ota_execute->img_bin_type, meta[0]);
        // Legacy Phase2-done re-trigger: only re-start Phase3 if our staged version matches THIS round's broadcast. A Router stuck in DONE from the previous round still holds last round's g_ota_image_version, and re-triggering here would flood its children with that stale version; when they differ, wait for this round's Phase1 to refresh us instead.
        if (ota_get_state() == OTA_DONE && bin_type_matched
            && g_ota_image_version == meta[0]) {
            log_info(
                "[Rafael] Phase3 trigger received, starting MTD distribution");
            ota_phase3_start();
        } else {
            ota_printf("[Rafael] Phase3 trigger ignored for legacy path "
                       "(state=%s bin_type_matched=%d)",
                       OtaStateToString(ota_get_state()), bin_type_matched);
        }
#endif
    } else if (ota_execute->flag == OTA_EXECUTE_PHASE2) {
        // Leader->Router: check bitmap, fill gaps via unicast REQUEST/RESPONSE
        log_info("[Rafael] EXECUTE_PHASE2 received from Leader");
        ota_execute_phase2_respond(src_ipv6);
    } else if (ota_execute->flag == OTA_EXECUTE_PHASE3_CHILD) {
        // Router->MTD child: wake up, check bitmap, fill gaps via unicast
        log_info("[Rafael] EXECUTE_PHASE3_CHILD received from Router");
        // rx-on wake (if bin_type matches) now happens inside ota_execute_phase3_child_respond(), after the mismatch check.
        ota_execute_phase3_child_respond(src_ipv6, ota_execute);
    } else if (ota_execute->flag == OTA_EXECUTE_STOP) {
        // Always erase OTA flash on stop, even if already Idle, or a device that timed out from OTA_DONE keeps a stale non-zero version and the next TC's same-version pre-check wrongly skips OTA
        ota_change_state_and_timer(OTA_IDLE, 0);
        ota_reset();
        ota_printf("stop+erase done");
    } else {
        ota_printf("unknow ota staus flag");
    }
}

void ota_event_handler() {
    ota_event_data_t event_data;
    // Drain all queued events per call: if ot_app_task_post was dropped (task queue full), the old while(0) left event data malloc'd but never freed.
    while (xQueueReceive(ota_event_queue, &event_data, 0) == pdPASS) {
        switch (event_data.event) {
            case OTA_DATA_SEND_EVENT:
                ota_data_sended_event_handler(event_data.data,
                                              event_data.data_lens);
                break;
            case OTA_REQUEST_SEND_EVENT:
                ota_request_sended_event_handler(event_data.data,
                                                 event_data.data_lens);
                break;
            case OTA_RESPONSE_SEND_EVENT:
                ota_response_sended_event_handler(event_data.data,
                                                  event_data.data_lens);
                break;
            case OTA_DATA_RECEIVE_EVENT:
                ota_data_received_event_handler(
                    event_data.data, event_data.data_lens, event_data.ipv6);
                break;
            case OTA_REQUEST_RECEIVE_EVENT:
#if CONFIG_MIU_DEVICE_TYPE_FTD
                ota_request_received_event_handler(
                    event_data.data, event_data.data_lens, event_data.ipv6);
#endif
                break;
            case OTA_RESPONSE_RECEIVE_EVENT:
                ota_response_received_event_handler(event_data.data,
                                                    event_data.data_lens);
                break;
            case OTA_RXMODE_RECEIVE_EVENT:
                ota_rxmode_received_event_handler(event_data.data,
                                                  event_data.data_lens);
                break;
            case OTA_STATUS_RECEIVE_EVENT:
                ota_status_received_event_handler(
                    event_data.data, event_data.data_lens, event_data.ipv6);
                break;
            case OTA_EXECUTE_RECEIVE_EVENT:
                ota_execute_received_event_handler(
                    event_data.data, event_data.data_lens, event_data.ipv6);
                break;
            default: ota_printf("unknow event %u", event_data.event); break;
        }
    }
}

void ota_start(uint16_t segments_size, uint16_t interval) {
    otError error = OT_ERROR_NONE;
    if (g_ota_state == OTA_DONE) {
        // Allow restarting from DONE without broadcasting stop to network
        ota_change_state_and_timer(OTA_IDLE, 0);
    }
    if (g_ota_state != OTA_IDLE) {
        log_info("ota in progress %u ", g_ota_state);
        return;
    }
    if (segments_size > OTA_SEGMENTS_MAX_SIZE) {
        log_info("ota segments_size size can't big than 256 (%u) ",
                 segments_size);
        return;
    }
    // Initiator send parameter
    g_ota_segments_size = 255;
    g_ota_data_interval = 1000;
    if (segments_size != 0) {
        g_ota_segments_size = segments_size;
    }
    if (interval != 0) {
        g_ota_data_interval = interval;
    }

    if (ota_get_image_size() == 0) {
        log_info("ota size is 0 ");
        return;
    }

    g_ota_total_num = ota_get_image_size() / g_ota_segments_size;
    g_ota_start_index = 0;
    if (ota_get_image_size() % g_ota_segments_size) {
        ++g_ota_total_num;
    }
    // Initiator bitmap set all
    g_ota_bitmap = ota_bitmap_init(g_ota_total_num);
    if (!g_ota_bitmap) {
        log_error("[FOTA] bitmap init failed");
        ota_change_state_and_timer(OTA_IDLE, 0);
        return;
    }
    for (uint32_t i = 0; i < g_ota_total_num; i++) {
        ota_bitmap_set(g_ota_bitmap, i);
    }

    ota_printf("ota_toatol_num %u ", g_ota_total_num);
    uint32_t crc32 = crc32checksum(
        (OTA_FLASH_START + OTA_INFO_HEADER_OFFSET),
        (ota_get_image_size() - OTA_INFO_HEADER_OFFSET));
    if (crc32 != ota_get_image_crc()) {
        log_info("crc fail %x %x ", crc32, ota_get_image_crc());
        return;
    } else {
        ota_printf("crc %x %x ", crc32, ota_get_image_crc());
    }
    char peer_addr[] = "ff03:0:0:0:0:0:0:1";
    error = otIp6AddressFromString(peer_addr, &ota_sender_addr);

    bin_type_matched = false;
#if CONFIG_MIU_DEVICE_TYPE_FTD
    // This device is the round's initiator, not a wake recipient. g_ota_prewake_woken is already reset to false whenever the state machine returns to OTA_IDLE, so no extra reset is needed here.
    // First of OTA_PHASE0_PREWAKE_ROUNDS rounds; the rest fire from the timer branch below. Each round re-runs the full broadcast (multicast + own children) so a pre-wake lost mid-mesh gets another chance at every hop, not just at the Leader's own child table.
    g_prewake_rounds_left = OTA_PHASE0_PREWAKE_ROUNDS - 1;
    ota_phase0_prewake_broadcast();
    ota_change_state_and_timer(OTA_PHASE0_PREWAKE, OTA_PHASE0_PREWAKE_GAP_MS);
#else
    ota_change_state_and_timer(OTA_PHASE1_SENDING, g_ota_data_interval);
#endif
}

void ota_send(char* ipaddr_str) {
    otError error = OT_ERROR_NONE;
    if (g_ota_state == OTA_DONE) {
        ota_change_state_and_timer(OTA_IDLE, 0);
    }
    if (g_ota_state != OTA_IDLE) {
        log_info("ota in progress %u ", g_ota_state);
        return;
    }
    error = otIp6AddressFromString(ipaddr_str, &ota_sender_addr);
    if (OT_ERROR_NONE == error) {
        g_ota_segments_size = 255;
        g_ota_data_interval = 3000;
        g_ota_total_num = ota_get_image_size() / g_ota_segments_size;
        g_ota_start_index = 0;
        g_have_data_ack = false;
        if (ota_get_image_size() % g_ota_segments_size) {
            ++g_ota_total_num;
        }
        if (ota_get_image_size() == 0) {
            log_info("ota size is 0 ");
            return;
        }
        ota_printf("ota_toatol_num %u ", g_ota_total_num);
        uint32_t crc32 = crc32checksum(
            (OTA_FLASH_START + OTA_INFO_HEADER_OFFSET),
            (ota_get_image_size() - OTA_INFO_HEADER_OFFSET));
        if (crc32 != ota_get_image_crc()) {
            log_info("crc fail %x %x ", crc32, ota_get_image_crc());
            return;
        } else {
            ota_printf("crc %x %x ", crc32, ota_get_image_crc());
        }
        ota_change_state_and_timer(OTA_PHASE1_SENDING, g_ota_data_interval);
        bin_type_matched = false;
    } else {
        log_info("ip error(%u) %s ", error, ipaddr_str);
    }
}

static void ota_build_rxmode_packet(bool OnWhenIdle, ota_rxmode_t* out) {
    memset(out, 0, sizeof(*out));
    out->OnWhenIdle = OnWhenIdle; //set rx on mode
    out->version = ota_get_image_version();
    ota_flash_read_bin_type(out->img_bin_type);
    out->total_segments = (uint16_t)g_ota_total_num;
    out->interval = g_ota_data_interval;
}

void ota_send_rxmode(bool OnWhenIdle) {
    otError error = OT_ERROR_NONE;
    otIp6Address dst_ipaddr;
    char dst_ipaddr_str[] = "ff03:0:0:0:0:0:0:1";
    error = otIp6AddressFromString(dst_ipaddr_str, &dst_ipaddr);
    ota_rxmode_t ota_rxmode;
    ota_build_rxmode_packet(OnWhenIdle, &ota_rxmode);
    ota_rxmode_sended_event_handler(dst_ipaddr, (uint8_t*)&ota_rxmode,
                                    sizeof(ota_rxmode_t));
#if CONFIG_MIU_DEVICE_TYPE_FTD
    otDeviceRole changeRole = otThreadGetDeviceRole(otrGetInstance());
    if (changeRole == OT_DEVICE_ROLE_ROUTER
        || changeRole == OT_DEVICE_ROLE_LEADER) {
        uint16_t maxChildrens = 0, sleep_node = 0;
        otChildInfo childInfo;
        maxChildrens = otThreadGetMaxAllowedChildren(otrGetInstance());
        for (uint16_t i = 0; i < maxChildrens; i++) {
            if ((otThreadGetChildInfoByIndex(otrGetInstance(), i, &childInfo)
                 != OT_ERROR_NONE)
                || childInfo.mIsStateRestoring) {
                continue;
            }
            if (childInfo.mRxOnWhenIdle == false) {
                dst_ipaddr = *otThreadGetRloc(otrGetInstance());
                dst_ipaddr.mFields.m8[14] = (childInfo.mRloc16 >> 8) & 0xff;
                dst_ipaddr.mFields.m8[15] = childInfo.mRloc16 & 0xff;
                ota_rxmode_sended_event_handler(
                    dst_ipaddr, (uint8_t*)&ota_rxmode, sizeof(ota_rxmode_t));
                ++sleep_node;
            }
        }
        ota_printf("sleep node %u ", sleep_node);
    }
#endif
}

// Unicast rxmode wake for the single-device (unicast) OTA flow: sends only to dst_ipaddr, unlike ota_send_rxmode() which multicasts to ff03::1 plus every local sleepy child, so selecting one device for unicast OTA no longer wakes the entire mesh.
void ota_send_rxmode_unicast(bool OnWhenIdle, otIp6Address dst_ipaddr) {
    ota_rxmode_t ota_rxmode;
    ota_build_rxmode_packet(OnWhenIdle, &ota_rxmode);
    ota_rxmode_sended_event_handler(dst_ipaddr, (uint8_t*)&ota_rxmode,
                                    sizeof(ota_rxmode_t));
}

void ota_send_status_get(otIp6Address dst_ipaddr, uint8_t status_type,
                         uint16_t report_time) {
    otError error = OT_ERROR_NONE;
    ota_status_get_t ota_stauts_get;
    ota_stauts_get.flag = OTA_STATUS_GET;
    ota_stauts_get.device_type = status_type;
    ota_stauts_get.report_time = report_time;

    ota_status_sended_event_handler(dst_ipaddr, (uint8_t*)&ota_stauts_get,
                                    sizeof(ota_stauts_get));
}

void ota_send_execute(otIp6Address dst_ipaddr, uint8_t execute_flag,
                      uint16_t time) {
    ota_execute_t ota_execute;
    ota_execute.flag = execute_flag;
    ota_execute.time = time;
    memset(ota_execute.bin_type, 0, sizeof(ota_execute.bin_type));

    ota_execute_sended_event_handler(dst_ipaddr, (uint8_t*)&ota_execute,
                                     sizeof(ota_execute_t));
}

void ota_stop() { ota_change_state_and_timer(OTA_IDLE, 0); }

void ota_update_self() {
    uint32_t crc32 = crc32checksum((OTA_FLASH_START + OTA_INFO_HEADER_OFFSET),
                                   (g_ota_image_size - OTA_INFO_HEADER_OFFSET));
    if (crc32 != g_ota_image_crc) {
        ota_printf("ota request upgrade fail %X %X", crc32, g_ota_image_crc);
        ota_change_state_and_timer(OTA_IDLE, 0);
    } else {
        ota_bootinfo_ready();
        ota_change_state_and_timer(OTA_REBOOT, 10);
    }
}

void ota_debug_level(unsigned int level) { ota_debug_flags = level; }

uint32_t ota_debug_level_get() { return ota_debug_flags; }

otError ota_init(otInstance* aInstance,
                 void (*ota_state_change_cb)(uint8_t state)) {
    otError error = OT_ERROR_NONE;

    do {
        error = otCoapStart(aInstance, OT_DEFAULT_COAP_PORT);
        if (error != OT_ERROR_NONE) {
            break;
        }

        memset(&ota_data_resource, 0, sizeof(ota_data_resource));
        memset(&ota_req_resource, 0, sizeof(ota_req_resource));
        memset(&ota_resp_resource, 0, sizeof(ota_resp_resource));
        memset(&ota_rxmode_resource, 0, sizeof(ota_rxmode_resource));
        memset(&ota_status_resource, 0, sizeof(ota_status_resource));
        memset(&ota_execute_resource, 0, sizeof(ota_execute_resource));

        ota_data_resource.mUriPath = RAFAEL_OTA_URL_DATA;
        ota_data_resource.mContext = aInstance;
        ota_data_resource.mHandler = &ota_coap_data_proccess;
        ota_data_resource.mNext = NULL;

        ota_req_resource.mUriPath = RAFAEL_OTA_URL_REQ;
        ota_req_resource.mContext = aInstance;
        ota_req_resource.mHandler = &ota_coap_request_proccess;
        ota_req_resource.mNext = NULL;

        ota_resp_resource.mUriPath = RAFAEL_OTA_URL_RESP;
        ota_resp_resource.mContext = aInstance;
        ota_resp_resource.mHandler = &ota_coap_response_proccess;
        ota_resp_resource.mNext = NULL;

        ota_rxmode_resource.mUriPath = RAFAEL_OTA_URL_RXMODE;
        ota_rxmode_resource.mContext = aInstance;
        ota_rxmode_resource.mHandler = &ota_coap_rxmode_proccess;
        ota_rxmode_resource.mNext = NULL;

        ota_status_resource.mUriPath = RAFAEL_OTA_URL_STATUS;
        ota_status_resource.mContext = aInstance;
        ota_status_resource.mHandler = &ota_coap_status_proccess;
        ota_status_resource.mNext = NULL;

        ota_execute_resource.mUriPath = RAFAEL_OTA_URL_EXECUTE;
        ota_execute_resource.mContext = aInstance;
        ota_execute_resource.mHandler = &ota_coap_execute_proccess;
        ota_execute_resource.mNext = NULL;

        otCoapAddResource(aInstance, &ota_data_resource);
        otCoapAddResource(aInstance, &ota_req_resource);
        otCoapAddResource(aInstance, &ota_resp_resource);
        otCoapAddResource(aInstance, &ota_rxmode_resource);
        otCoapAddResource(aInstance, &ota_status_resource);
        otCoapAddResource(aInstance, &ota_execute_resource);

        if (NULL == ota_timer) {
            ota_timer = xTimerCreate("ota_timer_t", portMAX_DELAY, false, NULL,
                                     ota_timer_handler);
        } else {
            ota_printf("ota_timer exist");
        }

        if (NULL == ota_resp_timer) {
            ota_resp_timer = xTimerCreate("ota_resp_timer", portMAX_DELAY,
                                          false, NULL,
                                          ota_response_timer_handler);
        } else {
            ota_printf("ota_resp_timer exist");
        }
        // Pre-create here (not per-poll): allocating a timer on the poll hot path fails exactly when heap is tight under heavy OTA, dropping the report the tool most needs. ID = the static report buffer; period re-armed each poll.
        if (NULL == ota_report_timer) {
            ota_report_timer = xTimerCreate("ota_report_timer", portMAX_DELAY,
                                            false, &g_ota_status_report,
                                            ota_report_timer_handler);
        } else {
            ota_printf("ota_report_timer exist");
        }
        if (NULL == ota_sleep_timer) {
            ota_sleep_timer = xTimerCreate("ota_sleep_timer", portMAX_DELAY,
                                           false, NULL,
                                           ota_sleep_timer_handler);
        } else {
            ota_printf("ota_sleep_timer exist");
        }
        char peer_addr[] = "ff03:0:0:0:0:0:0:1";
        error = otIp6AddressFromString(peer_addr, &ota_sender_addr);

        /* Init rx done queue*/
        ota_event_queue = xQueueCreate(
            5,
            sizeof(
                ota_event_data_t)); // reduced from 10: handler now drains all events per call

        ota_state_change_cb_signal = ota_state_change_cb;

    } while (0);

    return error;
}

#endif // CONFIG_APP_TASK_OTA_ENABLE