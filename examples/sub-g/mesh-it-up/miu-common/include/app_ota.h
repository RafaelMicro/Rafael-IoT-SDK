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
#define OTA_EVENT_DATA_MAX_SIZE 512
#define OTA_SEGMENTS_MAX_SIZE   256
#define OTA_REQUEST_TABLE_SIZE  10
#define OTA_RESPONSE_TABLE_SIZE 40
#define OTA_REQUEST_TRY_MAX     30
#define OTA_RESPONESE_TIMEOUT   (2000)
#define OTA_DONE_TIMEOUT        (3 * 60 * 1000)
#define OTA_DEPORT_TIMEOUT      (30 * 1000)
#define OTA_REBOOT_TIMEOUT      (1 * 60 * 1000)

#define OTA_FLASH_SECTOR_SIZE  0x1000 // 4KB
#define OTA_MAX_IMAGE_SECTORS  (SIZE_OF_FOTA_BANK_1MB / OTA_FLASH_SECTOR_SIZE)
#define OTA_INFO_HEADER_OFFSET 0x20   // Firmware offset

typedef enum {
    OTA_IDLE = 0x10,
    OTA_DATA_SENDING,
    OTA_DATA_RECEIVING,
    OTA_UNICAST_RECEIVING,
    OTA_REQUEST_SENDING,
    OTA_DONE,
    OTA_REBOOT
} ota_state_t;

typedef enum {
    OTA_PAYLOAD_TYPE_DATA = 0x20,
    OTA_PAYLOAD_TYPE_DATA_ACK,
    OTA_PAYLOAD_TYPE_REQUEST,
    OTA_PAYLOAD_TYPE_RESPONSE,
    OTA_PAYLOAD_TYPE_RXMODE
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
    uint8_t* data;
    uint16_t data_lens;
} __attribute__((packed)) ota_event_data_t;

typedef struct {
    uint32_t version;
    uint32_t size;
    uint32_t crc;
    uint16_t seq;
    uint16_t segments;
    uint16_t intervel;
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
} __attribute__((packed)) ota_rxmode_t;

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
    otIp6Address dstipv6;
} __attribute__((packed)) ota_status_report_t;

#define OTA_EXECUTE_REBOOT 0x61
#define OTA_EXECUTE_STOP   0x62

typedef struct {
    uint8_t flag;          //61:reboot/62:stop
    uint16_t time;         // millisecond
    uint8_t bin_type[12];  // 0x00*12 = all devices (backward compat)
} __attribute__((packed)) ota_execute_t;


otError ota_init(otInstance* aInstance,
                 void (*ota_state_change_cb)(uint8_t state));
void ota_stop();
void ota_send_rxmode(bool OnWhenIdle);
void ota_start(uint16_t segments_size, uint16_t intervel);
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