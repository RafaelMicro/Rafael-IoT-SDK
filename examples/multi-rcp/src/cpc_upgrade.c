/**
 * @file cpc_upgrade.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <sys/errno.h>

#include <stdint.h>
#include <stdlib.h>

#include <FreeRTOS.h>
#include <log.h>
#include <queue.h>
#include <semphr.h>
#include <stream_buffer.h>
#include <string.h>
#include <task.h>

#include <cpc.h>
#include <cpc_api.h>
#include <cpc_user_interface.h>

#include "fota_define.h"
#include "zigbee_app.h"

#define CPC_UPG_NOTIFY_HANDEL upg_mgr.event
#define CPC_UPG_NOTIFY_SIGNAL __cpc_upg_signal()
#define CPC_UPG_NOTIFY_ISR(ebit)                                               \
    (CPC_UPG_NOTIFY_HANDEL |= ebit);                                           \
    CPC_UPG_NOTIFY_SIGNAL;

#define CPC_UPG_NOTIFY(ebit)                                                   \
    MCU_ENTER_CRITICAL();                                                      \
    CPC_UPG_NOTIFY_HANDEL |= ebit;                                             \
    MCU_EXIT_CRITICAL();                                                       \
    CPC_UPG_NOTIFY_SIGNAL;

#define CPC_UPG_GET_NOTIFY(ebit)                                               \
    MCU_ATOMIC_SECTION({                                                       \
        ebit = CPC_UPG_NOTIFY_HANDEL;                                          \
        CPC_UPG_NOTIFY_HANDEL = CPC_UPG_EVENT_NONE;                            \
    });

#define WRITE_CACHE(pkt, len)                                                  \
    {                                                                          \
        memcpy(&upg_mgr.cache[upg_mgr.cache_idx], pkt, len);                   \
        upg_mgr.cache_idx += len;                                              \
    }

static void cpc_upg_write_image();
#define WRITE_INAGE() cpc_upg_write_image()

#define WRITE_INAGE_COND(cond)                                                 \
    {                                                                          \
        if (cond)                                                              \
            cpc_upg_write_image();                                             \
    }

#define CPC_UPG_MALLOC(ptr, len)                                               \
    {                                                                          \
        ptr = pvPortMalloc(len);                                               \
        if (ptr == NULL)                                                       \
            return;                                                            \
    }

#define CPC_UPG_MALLOC_COND(cond, ptr, len)                                    \
    {                                                                          \
        if (cond) {                                                            \
            ptr = pvPortMalloc(len);                                           \
            if (ptr == NULL)                                                   \
                return;                                                        \
        }                                                                      \
    }

//=============================================================================
//                  Structure Definition
//=============================================================================
SENUM_GENERIC(cpc_upg_command_t, uint32_t){
    CPC_UPG_CMD_ERASE = 0xF0000000,
    CPC_UPG_CMD_WRITE = 0xF0000001,
    CPC_UPG_CMD_COMPLETE = 0xF0000002,
    CPC_UPG_CMD_END = 0xF0000003,

    CPC_UPG_CMD_SEND_ERASE = 0xF0008000,
    CPC_UPG_CMD_SEND_WRITE = 0xF0008001,
    CPC_UPG_CMD_SEND_COMPLETE = 0xF0008002,
    CPC_UPG_CMD_SEND_END = 0xF0008003,

    CPC_ZB_GW_CMD_ERASE = 0xE0000000,
    CPC_ZB_GW_CMD_WRITE = 0xE0000001,
    CPC_ZB_GW_CMD_READ = 0xE0000002,
    CPC_ZB_GW_CMD_COMPLETE = 0xE0000003,
    CPC_ZB_GW_CMD_END = 0xE0000004,

    CPC_ZB_GW_CMD_SEND_ERASE = 0xE0008000,
    CPC_ZB_GW_CMD_SEND_WRITE = 0xE0008001,
    CPC_ZB_GW_CMD_SEND_READ = 0xE0008002,
    CPC_ZB_GW_CMD_SEND_COMPLETE = 0xE0008003,
    CPC_ZB_GW_CMD_SEND_END = 0xE0008003,
};

SENUM_GENERIC(cpc_upg_event_t, uint32_t){
    CPC_UPG_EVENT_NONE = 0,

    CPC_UPG_EVENT_CPC_READ = 0x00000001,
    CPC_UPG_EVENT_CPC_WRITE_DONE = 0x00000002,
    CPC_UPG_EVENT_CPC_ERROR = 0x00000004,
    CPC_UPG_EVENT_ALL = 0xffffffff,
};

typedef struct __attribute__((packed)) {
    uint8_t header[4];
    uint8_t len;
} gateway_cmd_hdr;

typedef struct __attribute__((packed)) {
    uint32_t command_id;
    uint16_t address;
    uint8_t address_mode;
    uint8_t parameter[];
} gateway_cmd_pd;

typedef struct __attribute__((packed)) {
    uint8_t cs;
} gateway_cmd_end;

typedef struct __attribute__((packed)) {
    uint16_t image_type;
    uint16_t manufacturer_code;
    uint32_t file_version;
    uint32_t image_size;
    uint32_t total_pkt;
    uint32_t cur_pkt;
    uint16_t pkt_len;
    uint8_t pkt[];
} upg_img_info_t;

typedef struct {
    cpc_upg_event_t event;
    cpc_endpoint_handle_t ep;
    TaskHandle_t task;
    fota_information_t fota;
    upg_img_info_t image;
    uint8_t state;
    uint8_t* cache;
    volatile uint32_t cache_idx;
    volatile uint32_t flash_addr;
} g_cpc_upg_t;

static g_cpc_upg_t upg_mgr = {
    .fota = {0},
    .cache_idx = 0,
    .flash_addr = FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS,
};

static void cpc_upg_write_image() {
    log_debug("erase addr %08X", upg_mgr.flash_addr);
    __cpc_flash_erase(FLASH_ERASE_SECTOR, upg_mgr.flash_addr);
    log_debug("write addr %08X", upg_mgr.flash_addr);
    for (uint8_t i = 0; i < 0x10; i++) {
        __cpc_flash_write(upg_mgr.flash_addr,
                          (uint32_t) & (upg_mgr.cache)[i * 0x100]);
        upg_mgr.flash_addr += 0x100;
    }
    upg_mgr.cache_idx = 0;
}

static void __cpc_upg_signal(void) {
    if (!xPortIsInsideInterrupt()) {
        xTaskNotifyGive(upg_mgr.task);
        return;
    }
    BaseType_t pxHigherPriorityTaskWoken = pdTRUE;
    vTaskNotifyGiveFromISR(upg_mgr.task, &pxHigherPriorityTaskWoken);
}

static void upgrade_cpc_tx_callback(cpc_user_endpoint_id_t endpoint_id,
                                    void* buffer, void* arg, status_t status) {
    (void)endpoint_id;
    (void)buffer;
    (void)status;
    if (arg)
        vPortFree(arg);
    CPC_UPG_NOTIFY(CPC_UPG_EVENT_CPC_WRITE_DONE);
}

static void upgrade_cpc_rx_callback(uint8_t endpoint_id, void* arg) {
    (void)endpoint_id;
    (void)arg;
    CPC_UPG_NOTIFY(CPC_UPG_EVENT_CPC_READ);
}

static void upgrade_cpc_error_callback(uint8_t endpoint_id, void* arg) {
    (void)endpoint_id;
    (void)arg;
    CPC_UPG_NOTIFY(CPC_UPG_EVENT_CPC_ERROR);
}

static uint32_t crc32checksum(uint32_t flash_addr, uint32_t data_len) {
    uint16_t k;
    uint8_t* buf = ((uint8_t*)flash_addr);
    uint32_t chkSum = ~0;

    for (uint32_t i = 0; i < data_len; i++) {
        chkSum ^= *buf++;
        for (k = 0; k < 8; k++)
            chkSum = chkSum & 1 ? (chkSum >> 1) ^ 0xedb88320 : chkSum >> 1;
    }
    return ~chkSum;
}

static uint8_t upgrade_cmd_checksum_calc(uint8_t* pBuf, uint8_t len) {
    uint8_t cs = 0;
    for (int i = 0; i < len; i++)
        cs += pBuf[i];
    return (~cs);
}

void upgrade_cmd_send(uint32_t cmd_id, uint16_t addr, uint8_t addr_mode,
                      uint8_t endp, uint8_t* pParam, uint32_t len) {
    uint8_t* gateway_cmd_pkt;
    uint32_t pkt_len;
    uint32_t idx = 0;

    // Calculate packet length
    pkt_len = sizeof(gateway_cmd_hdr) + sizeof(gateway_cmd_pd) + len
              + sizeof(gateway_cmd_end) + (endp != 0);

    // Allocate memory for the packet
    CPC_UPG_MALLOC(gateway_cmd_pkt, pkt_len);

    // Fill gateway command header
    gateway_cmd_hdr* hdr = (gateway_cmd_hdr*)gateway_cmd_pkt;
    hdr->header[0] = 0xFF;
    hdr->header[1] = 0xFC;
    hdr->header[2] = 0xFC;
    hdr->header[3] = 0xFF;
    hdr->len = sizeof(gateway_cmd_pd) + len + (endp != 0);
    idx += sizeof(gateway_cmd_hdr);

    // Fill gateway command payload
    gateway_cmd_pd* pd = (gateway_cmd_pd*)&gateway_cmd_pkt[idx];
    pd->command_id = cmd_id;
    pd->address = addr;
    pd->address_mode = addr_mode;

    if (endp != 0)
        pd->parameter[0] = endp;

    memcpy(pd->parameter + (endp != 0), pParam, len);
    idx += sizeof(gateway_cmd_pd) + len + (endp != 0);

    // Calculate and fill checksum
    gateway_cmd_end* end = (gateway_cmd_end*)&gateway_cmd_pkt[idx];
    end->cs = upgrade_cmd_checksum_calc((uint8_t*)&hdr->len, hdr->len + 1);

    // Send the packet
    status_t status = cpc_write(&upg_mgr.ep, gateway_cmd_pkt, pkt_len, 0,
                                gateway_cmd_pkt);
    if (status != STATUS_OK) {
        log_error("UPG Tx fail (%X)!\n", status);
        vPortFree(gateway_cmd_pkt);
        return;
    }

    // Log the packet
    log_debug("------------ UPG >>>> ------------");
    log_debug_hexdump("UTX", gateway_cmd_pkt, pkt_len);
}

#ifdef CONFIG_APP_MULTI_RCP_ZB_GW

static inline void zbgw_cmd_erase(uint8_t* pBuf) {
    uint32_t flash_addr = pBuf[0] | (pBuf[1] << 8) | (pBuf[2] << 16)
                          | (pBuf[3] << 24);
    uint32_t write_size = pBuf[4] | (pBuf[5] << 8) | (pBuf[6] << 16)
                          | (pBuf[7] << 24);
    uint8_t* p_tmp_buf = NULL;

    log_info("flash_addr %08x, size %x", flash_addr, write_size);
    log_info("15p4 MAC Address : %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
             pBuf[15], pBuf[14], pBuf[13], pBuf[12], pBuf[11], pBuf[10],
             pBuf[9], pBuf[8]);
    CPC_UPG_MALLOC(p_tmp_buf, 0x100);

    // Read, erase, and write secure register
    flash_read_sec_register((uint32_t)p_tmp_buf, 0x1100);
    __cpc_flash_erase(FLASH_ERASE_SECURE, 0x1100);
    memcpy(p_tmp_buf + 8, &pBuf[8], 8);
    flash_write_sec_register((uint32_t)p_tmp_buf, 0x1100);
    vPortFree(p_tmp_buf);

    // Send erase command
    upgrade_cmd_send(CPC_ZB_GW_CMD_SEND_ERASE, 0, 0, 0, 0, 8);
}

static inline void zbgw_cmd_write(uint8_t* pBuf) {
    static uint32_t start_address = 0;
    static uint32_t flash_addr;
    static uint32_t write_size;
    static uint32_t recv_cnt = 0;
    CPC_UPG_MALLOC_COND(!upg_mgr.cache, upg_mgr.cache, 0x1000);

    if (upg_mgr.cache) {
        if (recv_cnt + 0x100 >= 0x1000) {
            // Copy remaining data to cache
            memcpy(&upg_mgr.cache[recv_cnt], &pBuf[0], 0x1000 - recv_cnt);

            log_info("erase addr %08X", flash_addr);
            __cpc_flash_erase(FLASH_ERASE_SECTOR, flash_addr);
            log_info("write addr %08X", flash_addr);

            // Write data in 256-byte chunks
            for (uint32_t i = 0; i < 0x10; i++) {
                __cpc_flash_write(flash_addr,
                                  (uint32_t)&upg_mgr.cache[i * 0x100]);
                portYIELD();
                flash_addr += 0x100;
            }
            recv_cnt = 0;
        } else {
            // Copy data to cache
            memcpy(&upg_mgr.cache[recv_cnt], &pBuf[0], 0x100);
            recv_cnt += 0x100;
        }
    }

    // Free cache if writing is complete
    if (flash_addr >= (start_address + write_size)) {
        if (upg_mgr.cache) {
            vPortFree(upg_mgr.cache);
            upg_mgr.cache = NULL;
        }
    }

    // Send write command
    upgrade_cmd_send(CPC_ZB_GW_CMD_SEND_WRITE, 0, 0, 0, 0, 4);
}

static inline void zbgw_cmd_read(uint8_t* pBuf) {
    uint32_t start_address = pBuf[0] | (pBuf[1] << 8) | (pBuf[2] << 16)
                             | (pBuf[3] << 24);
    uint8_t* p_tmp_buf = NULL;
    log_info("Page Read %08X", start_address);
    CPC_UPG_MALLOC(p_tmp_buf, 0x100);

    // Read data from flash and send it
    memcpy(p_tmp_buf, (uint8_t*)start_address, 0x100);
    upgrade_cmd_send(CPC_ZB_GW_CMD_SEND_READ, 0, 0, 0, p_tmp_buf, 0x100);
    vPortFree(p_tmp_buf);
}

static inline void zbgw_cmd_complete(uint8_t* pBuf) {
    uint8_t* p_tmp_buf = NULL;
    CPC_UPG_MALLOC(p_tmp_buf, 8);

    // Read MAC address from OTP
    zigbee_app_read_otp_mac_addr(p_tmp_buf);

    // If MAC address is all 0xFF, read unique ID from flash
    if (memcmp(p_tmp_buf, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", 8) == 0) {
        flash_get_unique_id((uint32_t)p_tmp_buf, 8);
    }

    // Send complete command with MAC address or unique ID
    upgrade_cmd_send(CPC_ZB_GW_CMD_SEND_COMPLETE, 0, 0, 0, p_tmp_buf, 8);
    vPortFree(p_tmp_buf);
}

static void upgrade_cmd_debug_handle(uint32_t cmd_id, uint8_t* pBuf) {
    switch (cmd_id) {
        case CPC_ZB_GW_CMD_ERASE: return zbgw_cmd_erase(pBuf);
        case CPC_ZB_GW_CMD_WRITE: return zbgw_cmd_write(pBuf);
        case CPC_ZB_GW_CMD_READ: return zbgw_cmd_read(pBuf);
        case CPC_ZB_GW_CMD_COMPLETE: return zbgw_cmd_complete(pBuf);
        default: return;
    }
}
#endif

static inline void upgrade_cmd_erase() {
    CPC_UPG_MALLOC_COND(!upg_mgr.cache, upg_mgr.cache, 0x1000);
    upg_mgr.cache_idx = 0;
    upg_mgr.flash_addr = FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS;
    upgrade_cmd_send(CPC_UPG_CMD_SEND_ERASE, 0, 0, 0, 0, 4);
}

static inline void upgrade_cmd_write(upg_img_info_t* data) {
    if (data->cur_pkt == 0) {
        memcpy((uint8_t*)&upg_mgr.image, data, sizeof(upg_mgr.image));
        log_info("File Type: 0x%X", upg_mgr.image.image_type);
        log_info("Manufacturer Code: 0x%X", upg_mgr.image.manufacturer_code);
        log_info("File Version: 0x%X", upg_mgr.image.file_version);
        log_info("File Size: 0x%X", upg_mgr.image.image_size);
    }

    if (upg_mgr.cache_idx + data->pkt_len <= 0x1000) {
        WRITE_CACHE(data->pkt, data->pkt_len);
        WRITE_INAGE_COND(upg_mgr.cache_idx == 0x1000);
    } else {
        uint16_t part = (0x1000 - upg_mgr.cache_idx);
        WRITE_CACHE(data->pkt, part);
        WRITE_INAGE();
        WRITE_CACHE(data->pkt + part, data->pkt_len - part);
    }
    WRITE_INAGE_COND(data->cur_pkt == (upg_mgr.image.total_pkt - 1));
    // vTaskDelay(1);
    // ***************
    upgrade_cmd_send(CPC_UPG_CMD_SEND_WRITE, 0, 0, 0, (uint8_t*)&data->cur_pkt,
                     4);
}

static inline void upgrade_cmd_complete() {
    memset(&upg_mgr.fota, 0xFF, sizeof(upg_mgr.fota));
    upg_mgr.fota.fotabank_ready = FOTA_IMAGE_READY;
    upg_mgr.fota.fotabank_startaddr =
        FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS;
    upg_mgr.fota.target_startaddr = APP_START_ADDRESS;
    upg_mgr.fota.fotabank_datalen = upg_mgr.image.image_size;
    upg_mgr.fota.fota_result = 0xFF;
    upg_mgr.fota.fota_image_info = 0;
    upg_mgr.fota.fotabank_crc = crc32checksum(upg_mgr.fota.fotabank_startaddr,
                                              upg_mgr.fota.fotabank_datalen);

    __cpc_flash_erase(FLASH_ERASE_SECTOR, FOTA_UPDATE_BANK_INFO_ADDRESS);
    __cpc_flash_write(FOTA_UPDATE_BANK_INFO_ADDRESS, (uint32_t)&upg_mgr.fota);
    upgrade_cmd_send(CPC_UPG_CMD_SEND_COMPLETE, 0, 0, 0, 0, 4);
    upg_mgr.state = 1;
}

//  +--------------------------+-------> 0x0000_0000
//  |     Bootloader (32K)     |
//  +--------------------------+-------> 0x0000_8000
//  |                          |
//  |     Application (464K)   |
//  |                          |
//  +--------------------------+-------> 0x0007_C000
//  |                          |
//  |     OTA Target  (464K)   |
//  |                          |
//  +--------------------------+-------> 0x000F_0000
//  |     Reserved    (16K)    |
//  +--------------------------+-------> 0x000F_4000
static void upgrade_cmd_handle(uint32_t cmd_id, uint8_t* pBuf) {
    switch (cmd_id) {
        case CPC_UPG_CMD_ERASE: return upgrade_cmd_erase();
        case CPC_UPG_CMD_WRITE: return upgrade_cmd_write((upg_img_info_t*)pBuf);
        case CPC_UPG_CMD_COMPLETE: return upgrade_cmd_complete();
        default: return;
    }
}

static void upgrade_cmd_proc(uint8_t* pBuf, uint32_t len) {
    uint32_t cmd_index;

    log_debug("------------ >>>> UPG ------------");
    log_debug_hexdump("URX", pBuf, len);

    cmd_index = ((gateway_cmd_pd*)(&pBuf[5]))->command_id;
    gateway_cmd_pd* pt_pd = (gateway_cmd_pd*)&pBuf[5];

    if ((cmd_index >= CPC_UPG_CMD_ERASE) && (cmd_index < CPC_UPG_CMD_END))
        upgrade_cmd_handle(cmd_index, pt_pd->parameter);
#ifdef CONFIG_APP_MULTI_RCP_ZB_GW
    else if ((cmd_index >= CPC_ZB_GW_CMD_ERASE)
             && (cmd_index < CPC_ZB_GW_CMD_END))
        upgrade_cmd_debug_handle(cmd_index, pt_pd->parameter);
#endif
}

static void __cpc_upg_ep_proc(cpc_upg_event_t evt) {
    if (CPC_UPG_EVENT_CPC_ERROR & evt) {
        if (cpc_get_endpoint_state(&upg_mgr.ep)
            == CPC_STATE_ERROR_DESTINATION_UNREACHABLE)
            cpc_system_reset(0);
        cpc_set_state(&upg_mgr.ep, CPC_STATE_OPEN);
        MCU_ATOMIC_LOAD(upg_mgr.ep.ref_count, 1);
    }

    if ((CPC_UPG_EVENT_CPC_READ & evt)) {
        uint8_t* read_buf;
        uint16_t len;
        while (cpc_read(&upg_mgr.ep, (void**)&read_buf, &len, 0, 1) == 0) {
            upgrade_cmd_proc(read_buf, len);
            cpc_free_rx_buffer(read_buf);
        }
    }
    if ((CPC_UPG_EVENT_CPC_WRITE_DONE & evt) && upg_mgr.state)
        cpc_system_reset(1);
}

static void upgrade_cpc_task(void* parameters_ptr) {
    cpc_upg_event_t sevent = CPC_UPG_EVENT_NONE;
    while (1) {
        CPC_UPG_GET_NOTIFY(sevent);
        __cpc_upg_ep_proc(sevent);
        wdt_kick();
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }
}

void cpc_upgrade_init(void) {
    if (cpc_open_user_endpoint(&upg_mgr.ep, CPC_ENDPOINT_USER_ID_0, 0, 1) == 0)
        log_info("Opened Firmware upgrade EP");
    cpc_set_endpoint_option(&upg_mgr.ep, CPC_ENDPOINT_ON_IFRAME_WRITE_COMPLETED,
                            (void*)upgrade_cpc_tx_callback);
    cpc_set_endpoint_option(&upg_mgr.ep, CPC_ENDPOINT_ON_IFRAME_RECEIVE,
                            (void*)upgrade_cpc_rx_callback);
    cpc_set_endpoint_option(&upg_mgr.ep, CPC_ENDPOINT_ON_ERROR,
                            (void*)upgrade_cpc_error_callback);
    cpc_get_endpoint_state(&upg_mgr.ep);
    wdt_kick();

    if (__cpc_flash_erase(FLASH_ERASE_SECTOR, FOTA_UPDATE_BANK_INFO_ADDRESS)
        != STATUS_SUCCESS)
        log_error("erase fail");
    CPC_TASK_CREATE(upgrade_cpc_task, "TASK_UPG", 512, NULL,
                    E_TASK_PRIORITY_NORMAL, &upg_mgr.task);
}