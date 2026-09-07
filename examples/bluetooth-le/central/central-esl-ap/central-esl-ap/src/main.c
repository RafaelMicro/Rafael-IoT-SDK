/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

/** @file
 *
 * @brief BLE ESL Central AP role demo.
 *
 */

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "ble_profile.h"
#include "ble_app.h"
#include "ble_event.h"
#include "ble_l2cap.h"
#include "ble_profile.h"
#include "hosal_rf.h"
#include "ble_api.h"
#include "ble_host_cmd.h"
#include "hosal_uart.h"
#include "hosal_gpio.h"
#include "hosal_lpm.h"
#include "hosal_sysctrl.h"
#include "uart_stdio.h"
#include "rt_aes.h"
#include "ble_l2cap.h"
#include "ble_extended_advertising.h"

#include "hosal_sysctrl.h"
#include "app_hooks.h"
#include "dump_boot_info.h"
#include "uart_handler.h"
#include "ble_esl_ap_cmd.h"
#include "ble_fota.h"

/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/


/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
#define BLE_APP_CB_QUEUE_SIZE           32
#define APP_ISR_QUEUE_SIZE              2
#define APP_REQ_QUEUE_SIZE              6
#define SET_SUBEVENT_DATA_QUEUE_SIZE    (MAX_SUBEVENT_NUM)
#define UART_MSG_QUEUE_SIZE             8
#define APP_QUEUE_SIZE                  (BLE_APP_CB_QUEUE_SIZE + APP_ISR_QUEUE_SIZE + APP_REQ_QUEUE_SIZE + SET_SUBEVENT_DATA_QUEUE_SIZE + UART_MSG_QUEUE_SIZE)

#define APP_FOTA_CMD_QUEUE_SIZE         2
// LINK please refer to "ble_profile_def.c" --> att_db_mapping

#define APP_L2CAP_BUFFER_SIZE           4000

#define ESL_DEVICE_TABLE_SIZE           20
#define ESL_ENADV_MIC_SIZE              0x04
#define ESL_ENADV_CCM_HEADER            0xEA

#define ESL_ENADV_FILL_ENC_HEADER(len, p)  p->length = len - 1;\
                                           p->type = GAP_AD_TYPE_ENCRYPTED_ADVERTISING_DATA;\
                                           p->randomizer[0] = g_app_rand_value;\
                                          *((uint32_t*)(p->randomizer+1)) = xTaskGetTickCount()

#define ESL_ENADV_FILL_ENC_DATA(group, cmd_len, p_cmd, p_enc_data)  p_enc_data->group_id = group;\
                                                                    p_enc_data->len = (cmd_len + sizeof(esls_padv_enc_data_param_t) - 1);\
                                                                    p_enc_data->esl_tag = GAP_AD_TYPE_ELECTRONIC_SHELF_LABEL;\
                                                                    p_enc_data->rfu = 0;\
                                                                    memcpy(p_enc_data->cmd, p_cmd, cmd_len);
// MTU size 
#define DEFAULT_MTU                      BLE_GATT_ATT_MTU_MAX
#define U16_HIGHBYTE(x)                  (uint8_t)((x >> 8) & 0xFF) 
#define U16_LOWBYTE(x)                   (uint8_t)(x & 0xFF)

/**
 * @brief Generic BLE advertising data (AD) structure header.
 * @details Overlaid on each AD structure inside a scan report's raw data so the length and type
 *          can be read while walking the report looking for the ESL 16-bit service UUID.
 */
typedef struct __attribute__((packed)) ble_adv_header_s
{
    uint8_t len;          /**< Length in bytes of type + adv_data (excludes this len field itself).*/
    uint8_t type;         /**< AD type (e.g. GAP_AD_TYPE_SERVICE_CPLT_16B_UUID).*/
    uint8_t  adv_data[];  /**< AD structure payload, len - 1 bytes.*/
} ble_adv_header_t;

/**
 * @brief Parameters describing one AES-CCM "Encrypted Advertising Data" payload to decrypt.
 * @details Passed to @ref app_decrypt_adv_data to decrypt and verify one ESL response payload
 *          received in a PAwR response report.
 */
typedef struct __attribute__((packed)) decrypt_eadv_param_s
{
    uint8_t *p_rand;        /**< Pointer to the 5-byte randomizer used to build the CCM nonce.*/
    uint8_t *p_enc_data;    /**< Pointer to the ciphertext (including the trailing MIC) to decrypt.*/
    uint8_t enc_data_len;   /**< Length in bytes of p_enc_data (ciphertext + MIC).*/
    uint8_t *p_dec_data;    /**< Output buffer that receives the decrypted plaintext.*/
    uint8_t *p_dec_key;     /**< Pointer to the 16-byte AES key used to decrypt.*/
    uint8_t *p_dec_nonce;   /**< Pointer to the 8-byte nonce appended after the randomizer.*/
} decrypt_eadv_param_t;

/**
 * @brief One entry of the subevent ID-list ring buffer.
 * @details Records, for one queued PAwR subevent, which ESL IDs were assigned to its response
 *          slots so a later response report can map a response slot back to its ESL ID.
 */
typedef struct __attribute__((packed)) esl_subevent_data_s
{
    uint8_t group_id;      /**< Subevent (ESL group) index this entry applies to.*/
    uint8_t esl_id_num;    /**< Number of ESL IDs stored in p_esl_id.*/
    uint8_t *p_esl_id;     /**< Pointer to a heap-allocated array of ESL IDs, indexed by response slot.*/
} esl_subevent_data_t;

/**
 * @brief Ring buffer of pending subevent ID-list entries.
 * @details The single global instance (subevent_data_list) is written by
 *          @ref esl_add_control_point_esl_id_list, read by @ref esl_get_control_point_esl_id_list,
 *          and retired by @ref esl_update_control_point_esl_id_list as PAwR subevents are sent.
 */
typedef struct __attribute__((packed)) esl_subevent_data_list_s
{
    uint8_t write_index;                             /**< Index of the most recently added entry.*/
    uint8_t read_index;                              /**< Index of the most recently retired entry.*/
    esl_subevent_data_t subevent[MAX_SUBEVENT_NUM];  /**< Ring buffer storage, one slot per subevent.*/
} esl_subevent_data_list_t;



/**************************************************************************************************
 *    LOCAL VARIABLES
 *************************************************************************************************/
// Device BLE Address
const ble_gap_addr_t  DEVICE_ADDR = {.addr_type = RANDOM_STATIC_ADDR,
                                     .addr = {0x77, 0x42, 0x43, 0x44, 0x45, 0xCC }
                                    };


static ble_gap_peer_addr_t esl_device_table[ESL_DEVICE_TABLE_SIZE];

static QueueHandle_t g_app_msg_q;

static SemaphoreHandle_t semaphore_cb;
static SemaphoreHandle_t semaphore_isr;
static SemaphoreHandle_t semaphore_app;
static SemaphoreHandle_t semaphore_subevent;
static SemaphoreHandle_t semaphore_uart_msg;
static SemaphoreHandle_t semaphore_l2cap_credit = NULL;
static uint8_t g_app_rand_value = 0;

static esl_subevent_data_list_t subevent_data_list;
static uint16_t g_esl_control_point_cmd_dest_id = 0xFFFF;

/**************************************************************************************************
 *    GLOBAL VARIABLES
 *************************************************************************************************/

l2cap_data_tx_param_t g_l2cap_data =
{
    .p_data_buffer = NULL,
    .curr_credit = 0,
    .dest_id = 0,
    .curr_tx_len = 0,
    .total_tx_len = 0,
    .mtu_size = 0
};

uint8_t esl_ap_sync_key[16] = 
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};

uint8_t esl_ap_nonce[8] = 
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
};

uint8_t esl_esl_rsp_key[16] = 
{
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
};

uint8_t esl_rsp_nonce[8] = 
{
   0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00
};

QueueHandle_t g_app_fota_cmd_q;
/**************************************************************************************************
 *    FUNCTION DECLARATION
 *************************************************************************************************/
static void ble_app_main(app_req_param_t *p_param);
static ble_err_t ble_init(void);
static void svcs_gatts_data_init(ble_svcs_gatts_data_t *p_data);

/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
/* ------------------------------
 *  Handler
 * ------------------------------
 */

/**
 * @brief Copies a buffer while reversing its byte order.
 *
 * Copies len bytes from pSrc to pDst such that pDst[len-1-i] = pSrc[i], i.e. it performs an
 * endian swap. Used to reverse the AP sync / ESL response key bytes before they are written to
 * the connected ESL device.
 *
 * @param pDst Destination buffer, must be at least len bytes.
 * @param pSrc Source buffer, must be at least len bytes.
 * @param len Number of bytes to copy.
 */
static void _memcpy_inv(uint8_t *pDst, uint8_t *pSrc, uint16_t len)
{
    uint16_t i;

    if (len)
    {
        i = 0;
        do
        {
            len--;
            *(pDst + len) = *(pSrc + i);
            i++;
        } while (len != 0);
    }
}

/**
 * @brief Parses an ESL Control Point response and forwards it to the host.
 *
 * Interprets the opcode of the decoded response payload (error, LED state, basic state, display
 * state, sensor value, or vendor-specific), prints the relevant fields, and (for all but the
 * vendor-specific case) builds the matching AP event structure and forwards it via
 * ble_esl_ap_event_send(). Called both from GATT ESL Control Point notifications and from
 * decrypted PAwR response reports.
 *
 * @param group Subevent (ESL group) index the response belongs to.
 * @param esl_id ESL ID (response slot) the response came from.
 * @param p_data Pointer to the decoded esls_ctrl_point_rsp_param_t response payload.
 */
static void app_esl_rsp_data_parsing(uint8_t group, uint8_t esl_id, uint8_t *p_data)
{
    esls_ctrl_point_rsp_param_t *rsp_param;
    uint8_t i;
    ble_err_t err;

    rsp_param = (esls_ctrl_point_rsp_param_t*)p_data;

    switch (rsp_param->opcode)
    {
        case ESL_RSP_ERROR:
        {
            event_err_rsp_param_t *p_err_rsp;
            printf("Error - error_code: %d\n", rsp_param->err.error_code);

            p_err_rsp = pvPortMalloc(sizeof(event_err_rsp_param_t));
            if (p_err_rsp != NULL)
            {
                p_err_rsp->group_id = group;
                p_err_rsp->esl_id = esl_id;
                p_err_rsp->error_code = rsp_param->err.error_code;
                ble_esl_ap_event_send(AP_EVENT_ERROR_RESPONSE, (uint8_t *)p_err_rsp, sizeof(event_err_rsp_param_t));
                vPortFree(p_err_rsp);
            }
        }
        break;

        case ESL_RSP_LED_STATE:
        {
            event_led_state_rsp_param_t *p_led_state_rsp;
            printf("Led state - led_index: %d\n", rsp_param->led_state.led_index);
            p_led_state_rsp = pvPortMalloc(sizeof(event_led_state_rsp_param_t));
            if (p_led_state_rsp != NULL)
            {
                p_led_state_rsp->group_id = group;
                p_led_state_rsp->esl_id = esl_id;
                p_led_state_rsp->led_index = rsp_param->led_state.led_index;
                ble_esl_ap_event_send(AP_EVENT_LED_STATE_RESPONSE, (uint8_t *)p_led_state_rsp, sizeof(event_led_state_rsp_param_t));
                vPortFree(p_led_state_rsp);
            }
        }
        break;

        case ESL_RSP_BASIC_STATE:
        {
            event_basic_state_rsp_param_t *p_basic_state_rsp;
            printf("Basic state - service_need: %d\n", rsp_param->basic_state.service_need);
            printf("Basic state - synchronized: %d\n", rsp_param->basic_state.synchronized);
            printf("Basic state - active_led: %d\n", rsp_param->basic_state.active_led);
            printf("Basic state - pending_led_update: %d\n", rsp_param->basic_state.pending_led_update);
            printf("Basic state - pending_display_update: %d\n", rsp_param->basic_state.pending_display_update);
            p_basic_state_rsp = pvPortMalloc(sizeof(event_basic_state_rsp_param_t));
            if (p_basic_state_rsp != NULL)
            {
                p_basic_state_rsp->group_id = group;
                p_basic_state_rsp->esl_id = esl_id;
                p_basic_state_rsp->service_need = rsp_param->basic_state.service_need;
                p_basic_state_rsp->synchronized = rsp_param->basic_state.synchronized;
                p_basic_state_rsp->active_led = rsp_param->basic_state.active_led;
                p_basic_state_rsp->pending_led_update = rsp_param->basic_state.pending_led_update;
                p_basic_state_rsp->pending_display_update = rsp_param->basic_state.pending_display_update;
                p_basic_state_rsp->rfu = 0;
                ble_esl_ap_event_send(AP_EVENT_BASIC_RESPONSE, (uint8_t *)p_basic_state_rsp, sizeof(event_basic_state_rsp_param_t));
                vPortFree(p_basic_state_rsp);
            }
        }
        break;

        case ESL_RSP_DISPLAY_STATE:
        {
            event_display_state_rsp_param_t *p_display_state_rsp;

            printf("Display state - display_index: %d\n", rsp_param->display_state.display_index);
            printf("Display state - image_index: %d\n", rsp_param->display_state.image_index);
            p_display_state_rsp = pvPortMalloc(sizeof(event_display_state_rsp_param_t));
            if (p_display_state_rsp != NULL)
            {
                p_display_state_rsp->group_id = group;
                p_display_state_rsp->esl_id = esl_id;
                p_display_state_rsp->display_index = rsp_param->display_state.display_index;
                p_display_state_rsp->image_index = rsp_param->display_state.image_index;
                ble_esl_ap_event_send(AP_EVENT_DISPLAY_STATE_RESPONSE, (uint8_t *)p_display_state_rsp, sizeof(event_display_state_rsp_param_t));
                vPortFree(p_display_state_rsp);
            }
        }
        break;

        default:

            if ((rsp_param->opcode & 0x0F) == ESL_RSP_SENSOR_VALUE)
            {
                uint8_t sensor_value_len = ((rsp_param->opcode >> 4) & 0x0F);
                event_sensor_value_rsp_param_t *p_sensor_value_rsp;

                printf("Sensor state - sensor_index: %d sensor_value_len %d\n", rsp_param->sensor_state.sensor_index, sensor_value_len);
                printf("Sensor state - sensor_value: ");
                for (i = 0; i < sensor_value_len; i++)
                {
                    printf("%02x ", rsp_param->sensor_state.sensor_data[i]);
                }
                printf("\n");

                p_sensor_value_rsp = pvPortMalloc(sizeof(event_sensor_value_rsp_param_t) + sensor_value_len);
                if (p_sensor_value_rsp != NULL)
                {
                    p_sensor_value_rsp->group_id = group;
                    p_sensor_value_rsp->esl_id = esl_id;
                    p_sensor_value_rsp->sensor_index = rsp_param->sensor_state.sensor_index;
                    memcpy(p_sensor_value_rsp->sensor_data, rsp_param->sensor_state.sensor_data, sensor_value_len);
                    ble_esl_ap_event_send(AP_EVENT_SENSOR_DATA_RESPONSE, (uint8_t *)p_sensor_value_rsp, sizeof(event_sensor_value_rsp_param_t) + sensor_value_len);
                    vPortFree(p_sensor_value_rsp);
                }
            }
            else if ((rsp_param->opcode & 0x0F) == ESL_RSP_VENDOR_SPECIFIC_RSP)
            {
                uint8_t vendor_data_len = ((rsp_param->opcode >> 4) & 0x0F);
                printf("Vendor state - vendor data: ");
                for(i = 0; i < vendor_data_len; i++)
                {
                    printf("%02x ", rsp_param->vendor_state.vendor_data[i]);
                }
                printf("\n");
            }
            break;
    }
}

/**
 * @brief Decrypts and verifies an AES-CCM encrypted advertising data payload.
 *
 * Builds the 13-byte CCM nonce from p_decrypt_parm->p_rand and p_decrypt_parm->p_dec_nonce,
 * prepends the fixed CCM header byte, and runs aes_ccm_decryption_verification() using
 * p_decrypt_parm->p_dec_key to decrypt p_decrypt_parm->p_enc_data into p_decrypt_parm->p_dec_data.
 *
 * @param p_decrypt_parm Pointer to the randomizer, ciphertext, key, nonce, and output buffer
 *                        describing the decryption to perform.
 * @return uint32_t Length in bytes of the decrypted output data.
 * @retval 0 Decryption/verification failed, or the scratch buffer could not be allocated.
 */
static uint32_t app_decrypt_adv_data(decrypt_eadv_param_t *p_decrypt_parm)
{
    struct aes_ctx ctx;
    struct aes_ccm_decryption_packet  ccm_payload_packet;
    uint8_t data_len,  nonce[13];
    uint32_t status, outbuf_length;

    aes_acquire(&ctx);

    aes_key_init(&ctx, p_decrypt_parm->p_dec_key, AES_KEY128);

    /*load key to secure engine*/
    aes_load_round_key(&ctx);

    ccm_payload_packet.payload_buf = pvPortMalloc(p_decrypt_parm->enc_data_len+1);

    if (ccm_payload_packet.payload_buf != NULL)
    {
        ccm_payload_packet.payload_buf[0] = ESL_ENADV_CCM_HEADER;
        memcpy(ccm_payload_packet.payload_buf + 1, p_decrypt_parm->p_enc_data, p_decrypt_parm->enc_data_len);
        memcpy(nonce, p_decrypt_parm->p_rand, 5);
        memcpy(nonce + 5, p_decrypt_parm->p_dec_nonce, 8);
        ccm_payload_packet.nonce = (uint8_t *) nonce;
        ccm_payload_packet.hdr_len = 1;
        ccm_payload_packet.mlen = ESL_ENADV_MIC_SIZE;
        ccm_payload_packet.data_len = (p_decrypt_parm->enc_data_len - ccm_payload_packet.mlen);   /*Notice: we don't count mlen for data length!*/

        outbuf_length = ccm_payload_packet.data_len;
        ccm_payload_packet.out_buf = p_decrypt_parm->p_dec_data;
        ccm_payload_packet.out_buf_len = &outbuf_length;

        status = aes_ccm_decryption_verification( &ccm_payload_packet);
        vPortFree(ccm_payload_packet.payload_buf);
    }
    else
    {
        status = STATUS_ERROR;
    }

    aes_release(&ctx);

    if (status == STATUS_SUCCESS)
    {
        return outbuf_length;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief Processes a Periodic Advertising with Responses (PAwR) response report.
 *
 * Looks up the ESL ID list recorded for the report's subevent, then for each response slot that
 * carries a valid "Encrypted Advertising Data" AD structure, decrypts it with the ESL response
 * key/nonce (app_decrypt_adv_data), validates the decrypted ESL tag/length, maps the response
 * slot to its ESL ID, and forwards the decoded response to app_esl_rsp_data_parsing().
 *
 * @param p_rsp_report_param Pointer to the PAwR response report event data (subevent, response
 *                            slots, and their raw payloads).
 */
static void app_process_padv_response_report(ble_evt_padv_padv_rsp_report_t *p_rsp_report_param)
{
    decrypt_eadv_param_t decrypt_param;
    esls_padv_rsp_param_t *p_padv_rsp_param;
    esls_padv_enc_hdr_param_t *p_enc_hdr;
    uint8_t i, *p_dec_buff, dec_length, *p_esl_id_list, esl_id_num;
    ble_err_t err;

    p_esl_id_list = pvPortMalloc(sizeof(uint8_t) * MAX_RSP_SLOT);
    if (p_esl_id_list != NULL)
    {
        err = esl_get_control_point_esl_id_list(p_rsp_report_param->subevent, &esl_id_num, p_esl_id_list);
        if (err != BLE_ERR_OK)
        {
            printf("get esl id list - error: %d\n", err);
        }
        else
        {
            for (i = 0; i < p_rsp_report_param->num_responses; i++)
            {
                printf("slot %d\n", p_rsp_report_param->rsp_info[i].response_slot);

                p_enc_hdr = (esls_padv_enc_hdr_param_t*)p_rsp_report_param->rsp_info[i].data;
                if ((p_enc_hdr->length > 0) && 
                    (p_enc_hdr->type == GAP_AD_TYPE_ENCRYPTED_ADVERTISING_DATA))
                {
                    dec_length = (p_enc_hdr->length - sizeof(p_enc_hdr->type) - sizeof(p_enc_hdr->randomizer) - ESL_ENADV_MIC_SIZE);
                    if (dec_length == 0)
                    {
                        printf("error length %d\n", p_enc_hdr->length);
                    }
                    else
                    {
                        p_dec_buff = pvPortMalloc(dec_length);
                        if (p_dec_buff != NULL)
                        {
                            decrypt_param.p_rand = p_enc_hdr->randomizer;
                            decrypt_param.p_enc_data = p_enc_hdr->enc_data;
                            decrypt_param.enc_data_len = p_enc_hdr->length - sizeof(p_enc_hdr->type) - sizeof(p_enc_hdr->randomizer);
                            decrypt_param.p_dec_data = p_dec_buff;
                            decrypt_param.p_dec_key = esl_esl_rsp_key;
                            decrypt_param.p_dec_nonce = esl_rsp_nonce;
                            dec_length = app_decrypt_adv_data(&decrypt_param);
                            p_padv_rsp_param = (esls_padv_rsp_param_t*)p_dec_buff;

                            if (dec_length == 0)
                            {
                                printf("decryption fail\n");
                            }
                            else if ((p_padv_rsp_param->len == (dec_length - 1)) && 
                                (p_padv_rsp_param->esl_tag == GAP_AD_TYPE_ELECTRONIC_SHELF_LABEL))
                            {
                                if (p_rsp_report_param->rsp_info[i].response_slot > esl_id_num)
                                {
                                    printf("p_esl_id_list err, esl_id_num %d\n", esl_id_num);
                                }
                                else
                                {
                                    app_esl_rsp_data_parsing(p_rsp_report_param->subevent, p_esl_id_list[p_rsp_report_param->rsp_info[i].response_slot], 
                                        p_padv_rsp_param->rsp);
                                }
                            }
                            vPortFree(p_dec_buff);
                        }
                    }
                }
            }
        }
        vPortFree(p_esl_id_list);
    }
}

/**
 * @brief Builds and forwards an OTA data-channel notification event to the host.
 *
 * Depending on p_fota_notify->notify_code (periodic progress, unexpected address, RX length
 * error, total length error, timeout, address error, or not-started), allocates an
 * event_ota_download_rsp_param_t of the matching size, copies in the notify-code-specific
 * payload, and sends it via ble_esl_ap_event_send(AP_EVENT_OTA_DOWNLOAD_RESPONSE, ...).
 *
 * @param p_fota_notify Pointer to the FOTA data notification received from the FOTAS service.
 */
static void app_process_fota_data_response(fota_notify_param_t *p_fota_notify)
{
    event_ota_download_rsp_param_t  *p_ota_data_notify = NULL;
    uint8_t app_event_len = 0;

    switch (p_fota_notify->notify_code)
    {
        case OTA_DATA_NOTIFY_PERIODIC:
            app_event_len = sizeof(event_ota_download_rsp_param_t) + sizeof(fota_notify_periodic_t);
            p_ota_data_notify = pvPortMalloc(app_event_len);

            if (p_ota_data_notify != NULL)
            {
                p_ota_data_notify->notify_data[0] = ((p_fota_notify->param.periodic.expect_addr >> 16) & 0xFF);
                p_ota_data_notify->notify_data[1] = ((p_fota_notify->param.periodic.expect_addr >> 8) & 0xFF);
                p_ota_data_notify->notify_data[2] = (p_fota_notify->param.periodic.expect_addr & 0xFF);
            }
            break;

        case OTA_DATA_NOTIFY_ADDRESS_UNEXPECTED:
            app_event_len = sizeof(event_ota_download_rsp_param_t) + sizeof(fota_notify_addr_unexpect_t);
            p_ota_data_notify = pvPortMalloc(app_event_len);

            if (p_ota_data_notify != NULL)
            {
                p_ota_data_notify->notify_data[0] = ((p_fota_notify->param.addr_unexpect.expect_addr >> 16) & 0xFF);
                p_ota_data_notify->notify_data[1] = ((p_fota_notify->param.addr_unexpect.expect_addr >> 8) & 0xFF);
                p_ota_data_notify->notify_data[2] = (p_fota_notify->param.addr_unexpect.expect_addr & 0xFF);
            }
            break;

        case OTA_DATA_NOTIFY_LEN_ERROR:
            app_event_len = sizeof(event_ota_download_rsp_param_t) + sizeof(fota_notify_rx_data_len_err_t);
            p_ota_data_notify = pvPortMalloc(app_event_len);

            if (p_ota_data_notify != NULL)
            {
                p_ota_data_notify->notify_data[0] = p_fota_notify->param.data_len_err.rx_data_len;
            }
            break;

        case OTA_DATA_NOTIFY_TOTAL_LEN_ERR:
            app_event_len = sizeof(event_ota_download_rsp_param_t) + sizeof(fota_notify_total_data_len_err_t);
            p_ota_data_notify = pvPortMalloc(app_event_len);

            if (p_ota_data_notify != NULL)
            {
                memcpy(p_ota_data_notify->notify_data, (uint8_t *)&p_fota_notify->param.total_len_err.total_data_len, sizeof(fota_notify_total_data_len_err_t));
            }
            break;

        case OTA_DATA_NOTIFY_TIMEOUT:
        case OTA_DATA_NOTIFY_ADDRESS_ERR:
        case OTA_DATA_NOTIFY_NOT_START:
            
        default:
            app_event_len = sizeof(event_ota_download_rsp_param_t);
            break;
    }

    if (p_ota_data_notify != NULL)
    {
        p_ota_data_notify->notify_code = p_fota_notify->notify_code;
        ble_esl_ap_event_send(AP_EVENT_OTA_DOWNLOAD_RESPONSE, (uint8_t *)p_ota_data_notify, app_event_len);
        vPortFree(p_ota_data_notify);
    }
}

/**
 * @brief Builds and forwards an OTA control-command response event to the host.
 *
 * Allocates an event_ota_control_rsp_param_t sized for cmd_id and p_fota_idc->err_code: on
 * success, the OTA_CMD_QUERY command carries the returned system-info payload while other
 * commands carry no extra payload; on failure, an OTA_CMD_START failure additionally carries the
 * "already started" or "out of bank" detail payload when applicable. Sends the result via
 * ble_esl_ap_event_send(AP_EVENT_OTA_CONTROL_RESPONSE, ...).
 *
 * @param cmd_id OTA command opcode the indication is responding to (e.g. OTA_CMD_QUERY/START/ERASE/APPLY).
 * @param p_fota_idc Pointer to the command indication payload (error code and command-specific data).
 */
static void app_process_fota_cmd_response(uint8_t cmd_id, fota_idc_param_t *p_fota_idc)
{
    event_ota_control_rsp_param_t  *p_ota_control_rsp = NULL;
    uint8_t app_event_len = 0;

    if (p_fota_idc->err_code == OTA_ERR_CODE_NO_ERR)
    {
        switch (cmd_id)
        {
            case OTA_CMD_QUERY:
                app_event_len = sizeof(event_ota_control_rsp_param_t) + sizeof(fota_idc_sys_info_t);
                p_ota_control_rsp = pvPortMalloc(app_event_len);

                if (p_ota_control_rsp != NULL)
                {
                    memcpy(p_ota_control_rsp->rsp_data, (uint8_t *)&p_fota_idc->param.sys_info, sizeof(fota_idc_sys_info_t));
                }
                break;

            case OTA_CMD_START:
            case OTA_CMD_ERASE:
            case OTA_CMD_APPLY:
            default:
                app_event_len = sizeof(event_ota_control_rsp_param_t);
                p_ota_control_rsp = pvPortMalloc(app_event_len);
                break;
        }
    }
    else
    {
        switch (cmd_id)
        {
            case OTA_CMD_START:
                if (p_fota_idc->err_code == OTA_ERR_CODE_ALREADY_START)
                {
                    app_event_len = sizeof(event_ota_control_rsp_param_t) + sizeof(fota_idc_already_start_t);
                    p_ota_control_rsp = pvPortMalloc(app_event_len);
                    if (p_ota_control_rsp != NULL)
                    {
                        memcpy(p_ota_control_rsp->rsp_data, (uint8_t *)&p_fota_idc->param.already_start, sizeof(fota_idc_already_start_t));
                    }
                }
                else if (p_fota_idc->err_code == OTA_ERR_CODE_OUT_OF_BANK_SIZE)
                {
                    app_event_len = sizeof(event_ota_control_rsp_param_t) + sizeof(fota_idc_out_of_bank_t);
                    p_ota_control_rsp = pvPortMalloc(app_event_len);
                    if (p_ota_control_rsp != NULL)
                    {
                        memcpy(p_ota_control_rsp->rsp_data, (uint8_t *)&p_fota_idc->param.out_of_bank, sizeof(fota_idc_out_of_bank_t));
                    }
                    
                }
                else
                {
                    app_event_len = sizeof(event_ota_control_rsp_param_t);
                    p_ota_control_rsp = pvPortMalloc(app_event_len);
                }
                break;

            case OTA_CMD_QUERY:
            case OTA_CMD_ERASE:
            case OTA_CMD_APPLY:
            default:
                app_event_len = sizeof(event_ota_control_rsp_param_t);
                p_ota_control_rsp = pvPortMalloc(app_event_len);
                break;
        }
    }

    if (p_ota_control_rsp != NULL)
    {
        p_ota_control_rsp->ota_command = cmd_id;
        p_ota_control_rsp->command_error_code = p_fota_idc->err_code;
        ble_esl_ap_event_send(AP_EVENT_OTA_CONTROL_RESPONSE, (uint8_t *)p_ota_control_rsp, app_event_len);
        vPortFree(p_ota_control_rsp);
    }
}

/**
 * @brief Event handler for the Device Information Service (DIS) client role.
 *
 * On a manufacturer-name or firmware-revision read response, copies the returned string into a
 * NUL-terminated buffer and prints it. Ignores events when the service is not acting as a client.
 *
 * @param p_param Pointer to the attribute event data (role, event code, and returned data).
 */
static void ble_svcs_dis_evt_handler(ble_evt_att_param_t *p_param)
{
    uint8_t *p_data;

    if (p_param->gatt_role == BLE_GATT_ROLE_CLIENT)
    {
        /* ----------------- Handle event from client ----------------- */
        switch (p_param->event)
        {
        case BLESERVICE_DIS_MANUFACTURER_NAME_STRING_READ_RSP_EVENT:
            p_data = pvPortMalloc(p_param->length + 1);
            if (p_data != NULL)
            {
                memcpy(p_data, p_param->data, p_param->length);
                *(p_data + p_param->length) = '\0';
                printf("Manufacturer name: %s\n", p_data);
                vPortFree(p_data);
            }
            break;

        case BLESERVICE_DIS_FIRMWARE_REVISION_STRING_READ_RSP_EVENT:
            p_data = pvPortMalloc(p_param->length + 1);
            if (p_data != NULL)
            {
                memcpy(p_data, p_param->data, p_param->length);
                *(p_data + p_param->length) = '\0';
                printf("FW rev.: %s\n", p_data);
                vPortFree(p_data);
            }
            break;

        default:
            break;
        }
    }
}

/**
 * @brief Event handler for the Electronic Shelf Label Service (ESLS) client role.
 *
 * On an ESL Control Point notification, retrieves the pending destination group/ESL id via
 * esl_get_control_point_esl_id() and forwards the notification payload to
 * app_esl_rsp_data_parsing() for decoding. Ignores events when the service is not acting as a
 * client.
 *
 * @param p_param Pointer to the attribute event data (role, event code, and notification data).
 */
static void ble_svcs_esls_evt_handler(ble_evt_att_param_t *p_param)
{
    if (p_param->gatt_role == BLE_GATT_ROLE_CLIENT)
    {
        /* ----------------- Handle event from client ----------------- */
        switch (p_param->event)
        {
        case BLESERVICE_ESLS_ESL_CONTROL_POINT_NOTIFY_EVENT:
        {
            uint8_t group_id, esl_id;

            esl_get_control_point_esl_id(&group_id, &esl_id);
            app_esl_rsp_data_parsing(group_id, esl_id, p_param->data);
        }
        break;

        default:
            break;
        }
    }
}

/**
 * @brief Event handler for the Object Transfer Service (OTS) client role.
 *
 * Prints the request opcode and result code carried by Object Action Control Point (OACP) and
 * Object List Control Point (OLCP) indication responses. Ignores events when the service is not
 * acting as a client.
 *
 * @param p_param Pointer to the attribute event data (role, event code, and indication data).
 */
static void ble_svcs_ots_evt_handler(ble_evt_att_param_t *p_param)
{

    if (p_param->gatt_role == BLE_GATT_ROLE_CLIENT)
    {
        /* ----------------- Handle event from client ----------------- */
        switch (p_param->event)
        {
        case BLESERVICE_OTS_OBJECT_ACTION_CONTROL_POINT_INDICATE_EVENT:
        {
            ots_oacp_response_t *p_oacp = (ots_oacp_response_t*)p_param->data; 

            if (p_oacp->op_code == OACP_OPCODE_RESPONSE)
            {
                printf("OACP response, req_op_code %d result_code %d\n", p_oacp->req_op_code, p_oacp->result_code);
            }
        }
        break;

        case BLESERVICE_OTS_OBJECT_LIST_CONTROL_POINT_INDICATE_EVENT:
        {
            ots_olcp_response_t *p_olcp = (ots_olcp_response_t*)p_param->data; 

            if (p_olcp->op_code == OLCP_OPCODE_RESPONSE)
            {
                printf("OLCP response, req_op_code %d result_code %d\n", p_olcp->req_op_code, p_olcp->result_code);
            }
        }
        break;
        default:
            break;
        }
    }
}

/**
 * @brief Event handler for the FOTA Service (FOTAS) client role.
 *
 * Handles the command-write response (no-op), the data-notify event (forwarded to
 * app_process_fota_data_response()), and the command-indicate event (dequeues the pending OTA
 * command id from g_app_fota_cmd_q and forwards it with the indication payload to
 * app_process_fota_cmd_response()). Ignores events when the service is not acting as a client.
 *
 * @param p_param Pointer to the attribute event data (role, event code, and event data).
 */
static void ble_svcs_fotas_evt_handler(ble_evt_att_param_t *p_param)
{
    if (p_param->gatt_role == BLE_GATT_ROLE_CLIENT)
    {
        /* ----------------- Handle event from server ----------------- */
        switch (p_param->event)
        {
        case BLESERVICE_FOTAS_COMMAND_WRITE_RSP_EVENT:
        {
 
        }
        break;

        case BLESERVICE_FOTAS_DATA_NOTIFY_EVENT:
        {
            fota_notify_param_t *p_fota_notify;

            p_fota_notify = (fota_notify_param_t*)p_param->data;
            app_process_fota_data_response(p_fota_notify);

        }
        break;

        case BLESERVICE_FOTAS_COMMAND_INDICATE_EVENT:
        {
            fota_idc_param_t *p_fota_idc;
            app_fota_cmd_queue_t fota_cmd_q;

            p_fota_idc = (fota_idc_param_t*)p_param->data;
            if (xQueueReceive(g_app_fota_cmd_q, &fota_cmd_q, 2) == pdTRUE)
            {
                app_process_fota_cmd_response(fota_cmd_q.cmd_id, p_fota_idc);
            }

        }
        break;
        default:
            break;
        }
    }
}

/**
 * @brief Sends one fragment of the pending outbound L2CAP data transfer.
 *
 * If an L2CAP CoC credit is available (g_l2cap_data.curr_credit > 0), sends up to mtu_size bytes
 * of g_l2cap_data.p_data_buffer; the very first fragment is prefixed with the 2-byte total length.
 * Updates curr_credit/curr_tx_len, and frees the data buffer once curr_tx_len reaches
 * total_tx_len.
 *
 * @param host_id Link (host) identifier the L2CAP data is sent on.
 * @return ble_err_t Result of the operation.
 * @retval BLE_ERR_UNKNOW_TYPE No L2CAP credit is currently available.
 * @retval BLE_ERR_ALLOC_MEMORY_FAIL Failed to allocate the TX packet buffer.
 * @retval BLE_BUSY The fragment was sent but more data remains to be transmitted.
 * @retval BLE_ERR_OK The final fragment was sent and the transfer is complete.
 * @retval (other) ble_err_t code returned by ble_cmd_l2cap_data_send() if the underlying send
 *         itself fails; credit/length bookkeeping is left untouched in that case.
 */
static ble_err_t svcs_esl_send_l2cap_data_handler(uint8_t host_id)
{
    ble_err_t status = BLE_ERR_UNKNOW_TYPE;
    ble_l2cap_data_send_t *p_l2cap_data_send;
    uint8_t tx_len;

    if (g_l2cap_data.curr_credit > 0)
    {
        if (g_l2cap_data.mtu_size < ( g_l2cap_data.total_tx_len - g_l2cap_data.curr_tx_len))
        {
            tx_len = g_l2cap_data.mtu_size;
        }
        else
        {
            tx_len = g_l2cap_data.total_tx_len - g_l2cap_data.curr_tx_len;
        }

        p_l2cap_data_send = pvPortMalloc(tx_len + sizeof(ble_l2cap_data_send_t));
        if (p_l2cap_data_send != NULL)
        {
            p_l2cap_data_send->host_id = host_id;
            p_l2cap_data_send->dest_id = g_l2cap_data.dest_id;
            p_l2cap_data_send->length = tx_len;

            if (g_l2cap_data.curr_tx_len == 0)
            {
                //first two byte contains total length
                memcpy(p_l2cap_data_send->data,  (uint8_t*)&g_l2cap_data.total_tx_len, sizeof(g_l2cap_data.total_tx_len));

                tx_len -= sizeof(g_l2cap_data.total_tx_len);
                memcpy(p_l2cap_data_send->data + sizeof(g_l2cap_data.total_tx_len), g_l2cap_data.p_data_buffer + g_l2cap_data.curr_tx_len, tx_len);
            }
            else
            {
                memcpy(p_l2cap_data_send->data, g_l2cap_data.p_data_buffer + g_l2cap_data.curr_tx_len, tx_len);
            }
            
            //test_image;    
            status = ble_cmd_l2cap_data_send(p_l2cap_data_send);
            //printf("g_l2cap_data.total_tx_len %d curr_tx_len 0x%04x, status %d tx_len %d \n", g_l2cap_data.total_tx_len, g_l2cap_data.curr_tx_len, status, tx_len);
            vPortFree(p_l2cap_data_send);
        }
        else
        {
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
        }

        if (status == BLE_ERR_OK)
        {
            g_l2cap_data.curr_credit--;
            g_l2cap_data.curr_tx_len += tx_len;

            if (g_l2cap_data.curr_tx_len == g_l2cap_data.total_tx_len)
            {
                vPortFree(g_l2cap_data.p_data_buffer);
                g_l2cap_data.p_data_buffer = NULL;
                g_l2cap_data.curr_tx_len = 0;
                status = BLE_ERR_OK;
            }
            else
            {
                status = BLE_BUSY;
            }
        }
    }
    return status;
}

/**
 * @brief Drives the post-connection GATT configuration sequence for the central link.
 *
 * Steps (tracked by a static cmd_index) through: MTU exchange, data length update, enabling the
 * ESL/OTS/FOTAS CCCDs for notifications/indications, reading the OTS/ESLS/DIS characteristic
 * values used by the application, and finally opening the L2CAP CoC channel. Advances cmd_index
 * only when the current step's command was accepted (BLE_ERR_OK); resets cmd_index if the link is
 * no longer connected.
 *
 * @param host_id Link (host) identifier the sequence is run on.
 * @return ble_err_t Progress/result of the sequence.
 * @retval BLE_ERR_OK All configuration steps have completed.
 * @retval BLE_BUSY The sequence is still in progress (caller should re-invoke via app_request_set()).
 */
static ble_err_t svcs_esl_multi_cmd_handler(uint8_t host_id)
{
    ble_err_t status;
    ble_info_link0_t *p_profile_info;
    static uint8_t cmd_index = 0;

    if (ble_app_link_info[host_id].state == STATE_CONNECTED)
    {
        p_profile_info = (ble_info_link0_t *)ble_app_link_info[host_id].profile_info;
        switch (cmd_index)
        {
        case 0:
            // send MTU exchange to server
            status = ble_cmd_mtu_size_update(host_id, DEFAULT_MTU);
            break;

        case 1:
            // send data length updated to server
            status = ble_cmd_data_len_update(host_id, (DEFAULT_MTU + 4)); // 4 bytes header
            break;

        case 2:
            // set GATT cccd
            p_profile_info->svcs_info_esls.client_info.data.esl_control_point_cccd = BLEGATT_CCCD_NOTIFICATION;

            // send config GATT cccd to server to enable to receive indication from server
            status = ble_svcs_cccd_set(host_id,
                                       p_profile_info->svcs_info_esls.client_info.handles.hdl_esl_control_point_cccd,
                                       p_profile_info->svcs_info_esls.client_info.data.esl_control_point_cccd);
            break;

        case 3:
            // set GATT cccd
            p_profile_info->svcs_info_ots.client_info.data.object_action_control_point_cccd = BLEGATT_CCCD_INDICATION;

            // send config GATT cccd to server to enable to receive indication from server
            status = ble_svcs_cccd_set(host_id,
                                       p_profile_info->svcs_info_ots.client_info.handles.hdl_object_action_control_point_cccd,
                                       p_profile_info->svcs_info_ots.client_info.data.object_action_control_point_cccd);
            break;

        case 4:
        {
            p_profile_info->svcs_info_ots.client_info.data.object_list_control_point_cccd = BLEGATT_CCCD_INDICATION;

            // send config GATT cccd to server to enable to receive indication from server
            status = ble_svcs_cccd_set(host_id,
                                       p_profile_info->svcs_info_ots.client_info.handles.hdl_object_list_control_point_cccd,
                                       p_profile_info->svcs_info_ots.client_info.data.object_list_control_point_cccd);
        }
        break;

        case 5:
        {
            p_profile_info->svcs_info_ots.client_info.data.object_changed_cccd = BLEGATT_CCCD_INDICATION;

            // send config GATT cccd to server to enable to receive indication from server
            status = ble_svcs_cccd_set(host_id,
                                       p_profile_info->svcs_info_ots.client_info.handles.hdl_object_changed_cccd,
                                       p_profile_info->svcs_info_ots.client_info.data.object_changed_cccd);
        }
        break;

        case 6:
        {
            p_profile_info->svcs_info_fotas.client_info.data.data_cccd = BLEGATT_CCCD_NOTIFICATION;

            // send config GATT cccd to server to enable to receive indication from server
            status = ble_svcs_cccd_set(host_id,
                                       p_profile_info->svcs_info_fotas.client_info.handles.hdl_data_cccd,
                                       p_profile_info->svcs_info_fotas.client_info.data.data_cccd);
        }
        break;

        case 7:
        {
            p_profile_info->svcs_info_fotas.client_info.data.command_cccd = BLEGATT_CCCD_INDICATION;

            // send config GATT cccd to server to enable to receive indication from server
            status = ble_svcs_cccd_set(host_id,
                                       p_profile_info->svcs_info_fotas.client_info.handles.hdl_command_cccd,
                                       p_profile_info->svcs_info_fotas.client_info.data.command_cccd);
        }
        break;

        case 8:
        {
            ble_gatt_read_req_param_t read_param;

            read_param.host_id = host_id;
            read_param.handle_num = p_profile_info->svcs_info_ots.client_info.handles.hdl_ots_feature;
            status = ble_cmd_gatt_read_req(&read_param);
        }
        break;

        case 9:
        {
            ble_gatt_read_req_param_t read_param;

            read_param.host_id = host_id;
            read_param.handle_num = p_profile_info->svcs_info_esls.client_info.handles.hdl_esl_display_information;
            status = ble_cmd_gatt_read_req(&read_param);
        }
        break;

        case 10:
        {
            ble_gatt_read_req_param_t read_param;

            read_param.host_id = host_id;
            read_param.handle_num = p_profile_info->svcs_info_esls.client_info.handles.hdl_esl_image_information;
            status = ble_cmd_gatt_read_req(&read_param);
        }
        break;

        case 11:
        {
            ble_gatt_read_req_param_t read_param;

            read_param.host_id = host_id;
            read_param.handle_num = p_profile_info->svcs_info_esls.client_info.handles.hdl_esl_sensor_information;
            status = ble_cmd_gatt_read_req(&read_param);
        }
        break;

        case 12:
        {
            ble_gatt_read_req_param_t read_param;

            read_param.host_id = host_id;
            read_param.handle_num = p_profile_info->svcs_info_esls.client_info.handles.hdl_esl_led_information;
            status = ble_cmd_gatt_read_req(&read_param);
        }
        break;

        case 13:
        {
            ble_gatt_read_req_param_t read_param;

            read_param.host_id = host_id;
            read_param.handle_num = p_profile_info->svcs_info_dis.client_info.handles.hdl_pnp_id;
            status = ble_cmd_gatt_read_req(&read_param);
        }
        break;

        case 14:
        {
            ble_l2cap_chan_connect_t l2cap_ch_connect;

            l2cap_ch_connect.host_id = host_id;
            l2cap_ch_connect.spsm = 0x0025;
            l2cap_ch_connect.mtu = APP_L2CAP_BUFFER_SIZE;
            status = ble_cmd_l2cap_chan_connect(&l2cap_ch_connect);
        }
        break;

        default:
            // process commands are done.
            cmd_index = 0;
            return BLE_ERR_OK;
        }

        // if command status is BLE_STATUS_SUCCESS then go to do next command (index++)
        if (status == BLE_ERR_OK)
        {
            cmd_index++;
        }
    }
    else
    {
        cmd_index = 0;
    }

    return BLE_BUSY;
}

// ESL AP Central
/**
 * @brief Handles application-level requests for the ESL AP central link.
 *
 * On APP_REQUEST_ESL_AP_MULTI_CMD, drives svcs_esl_multi_cmd_handler() and re-queues the request
 * (via app_request_set()) until the GATT configuration sequence completes or the link
 * disconnects. On APP_REQUEST_SEND_L2CAP_DATA, drives svcs_esl_send_l2cap_data_handler()
 * similarly to progress the pending L2CAP data transfer. Other requests are ignored.
 *
 * @param p_param Pointer to the request parameters (host id and requested event).
 */
static void app_central_handler(app_req_param_t *p_param)
{
    ble_err_t status;
    uint8_t host_id;
    ble_info_link0_t *p_profile_info;

    status = BLE_ERR_OK;
    host_id = p_param->host_id;
    p_profile_info = (ble_info_link0_t *)ble_app_link_info[host_id].profile_info;

    switch (p_param->app_req)
    {
    case APP_REQUEST_ESL_AP_MULTI_CMD:
        if (svcs_esl_multi_cmd_handler(host_id) == BLE_ERR_OK)
        {
            // Multiple commands set finished
            printf("Ready to TX/RX data to/from the connected server. \n");
        }
        else
        {
            if (ble_app_link_info[host_id].state == STATE_CONNECTED)
            {
                if (app_request_set(host_id, APP_REQUEST_ESL_AP_MULTI_CMD, false) == false)
                {
                    // No application queue buffer. Error.
                }
            }
            else
            {
                printf("connection terminated, reconnect server to restart TX/RX data. \n");
            }
        }
        break;

    case APP_REQUEST_SEND_L2CAP_DATA:
        if (svcs_esl_send_l2cap_data_handler(host_id) == BLE_ERR_OK)
        {
            printf("L2CAP data TX finish. \n");
        }
        else
        {
            if (ble_app_link_info[host_id].state == STATE_CONNECTED)
            {
                if (app_request_set(host_id, APP_REQUEST_SEND_L2CAP_DATA, false) == false)
                {
                    // No application queue buffer. Error.
                }
            }
            else
            {
                printf("connection terminated, reconnect server to restart TX L2CAP data. \n");
            }
        }
        break;

    default:
        break;
    }
}

/**
 * @brief Dispatches generic BLE stack events for the ESL AP central link.
 *
 * Handles scan-enable status, advertising reports (filters for the ESL 16-bit service UUID and
 * forwards new devices via ble_esl_ap_event_send()), connection complete/parameter update, PHY
 * read/update, MTU exchange, data length change, disconnection complete, ATT/GATT database parse
 * complete (fetches service handles and kicks off the GATT configuration sequence), SM
 * authentication status (restores CCCDs), PAwR subevent data request (retires the corresponding
 * ID-list entry), PAwR response report (forwards to app_process_padv_response_report()), and
 * extended advertising set terminated. Updates ble_app_link_info[].state and prints diagnostic
 * logs as appropriate for each event.
 *
 * @param p_param Pointer to the BLE event data (event code and event-specific parameters).
 */
static void ble_evt_handler(ble_evt_param_t *p_param)
{
    switch (p_param->event)
    {
    case BLE_SCAN_EVT_SET_ENABLE:
    {
        ble_evt_scan_set_scan_enable_t *p_scan_enable = (ble_evt_scan_set_scan_enable_t *)&p_param->event_param.ble_evt_scan.param.evt_set_scan_enable;

        if (p_scan_enable->status == BLE_HCI_ERR_CODE_SUCCESS)
        {
            if (p_scan_enable->scan_enabled == true)
            {
                ble_app_link_info[APP_ESL_AP_C_HOST_ID].state = STATE_SCANNING;
                printf("Scanning...\n");
                memset(&esl_device_table, 0xFF, sizeof(esl_device_table));
            }
            else
            {
                if (ble_app_link_info[APP_ESL_AP_C_HOST_ID].state == STATE_INITIATING)
                {

                }
                else
                {
                    ble_app_link_info[APP_ESL_AP_C_HOST_ID].state = STATE_STANDBY;
                }
                printf("Idle.\n");
            }
        }
        else
        {
            printf("Scan enable failed.\n");
        }
    }
    break;

    case BLE_SCAN_EVT_ADV_REPORT:
    {
        ble_evt_scan_adv_report_t *p_adv_report = (ble_evt_scan_adv_report_t *)&p_param->event_param.ble_evt_scan.param.evt_adv_report;
        ble_adv_header_t *p_adv_header;
        event_advertising_report_param_t *p_adv_report_event;
        uint8_t idx = 0, i;

        while (idx < p_adv_report->length)
        {
            p_adv_header = (ble_adv_header_t *)(p_adv_report->data + idx);
            if ((p_adv_header->type == GAP_AD_TYPE_SERVICE_CPLT_16B_UUID) &&
                (p_adv_header->adv_data[0] == U16_LOWBYTE(GATT_SERVICES_ELECTRONIC_SHELF_LABEL)) &&
                (p_adv_header->adv_data[1] == U16_HIGHBYTE(GATT_SERVICES_ELECTRONIC_SHELF_LABEL)))
            {
                for (i = 0; i < ESL_DEVICE_TABLE_SIZE; i++)
                {
                    if ((esl_device_table[i].addr_type == p_adv_report->peer_addr.addr_type) &&
                        (memcmp(esl_device_table[i].addr, p_adv_report->peer_addr.addr, BLE_ADDR_LEN) == 0))
                    {
                        break;
                    }
                }

                if (i != ESL_DEVICE_TABLE_SIZE)
                {
                    //duplicate device
                    break;
                }

                p_adv_report_event = pvPortMalloc(sizeof(event_advertising_report_param_t));
                if (p_adv_report_event != NULL)
                {
                    p_adv_report_event->address_type = p_adv_report->peer_addr.addr_type;
                    memcpy(p_adv_report_event->address, p_adv_report->peer_addr.addr, BLE_ADDR_LEN);
                    ble_esl_ap_event_send(AP_EVENT_ADVERTISING_REPORT, (uint8_t *)p_adv_report_event, sizeof(event_advertising_report_param_t));
                    vPortFree(p_adv_report_event);
                }

                printf("ESL device found, addr type %d, addr: %02x:%02x:%02x:%02x:%02x:%02x\n", p_adv_report->peer_addr.addr_type,
                                                                           p_adv_report->peer_addr.addr[5], p_adv_report->peer_addr.addr[4], 
                                                                           p_adv_report->peer_addr.addr[3], p_adv_report->peer_addr.addr[2], 
                                                                           p_adv_report->peer_addr.addr[1], p_adv_report->peer_addr.addr[0]);
                for (i = 0; i < ESL_DEVICE_TABLE_SIZE; i++)
                {
                    if(esl_device_table[i].addr_type == 0xFF)
                    {
                        esl_device_table[i].addr_type = p_adv_report->peer_addr.addr_type;
                        memcpy(esl_device_table[i].addr, p_adv_report->peer_addr.addr, BLE_ADDR_LEN);
                        break;
                    }
                }
                break;
            }
            idx += (p_adv_header->len + sizeof(p_adv_header->len));
        }
    }
    break;

    case BLE_GAP_EVT_CONN_COMPLETE:
    {
        ble_evt_gap_conn_complete_t *p_conn_param = (ble_evt_gap_conn_complete_t *)&p_param->event_param.ble_evt_gap.param.evt_conn_complete;
        event_connection_complete_param_t *p_conn_complete;
        uint8_t event_len = 0;

        if (p_conn_param->status == BLE_HCI_ERR_CODE_SUCCESS)
        {
            event_len = sizeof(event_connection_complete_param_t);
        }
        else
        {
            event_len = sizeof(uint8_t);
        }

        p_conn_complete = pvPortMalloc(event_len);
        if (p_conn_complete != NULL)
        {
            p_conn_complete->status = p_conn_param->status;
            if (p_conn_param->status == BLE_HCI_ERR_CODE_SUCCESS)
            {
                p_conn_complete->address_type = p_conn_param->peer_addr.addr_type;
                memcpy(p_conn_complete->address, p_conn_param->peer_addr.addr, BLE_ADDR_LEN);
                p_conn_complete->interval = p_conn_param->conn_interval;
                p_conn_complete->lantency = p_conn_param->periph_latency;
                p_conn_complete->timeout = p_conn_param->supv_timeout;
            }

            ble_esl_ap_event_send(AP_EVENT_CONNECTION_COMPLETE, (uint8_t *)p_conn_complete, event_len);
            vPortFree(p_conn_complete);
        }

        if (p_conn_param->status != BLE_HCI_ERR_CODE_SUCCESS)
        {
            printf("Connect failed, error code = 0x%02x\n", p_conn_param->status);
        }
        else
        {
            ble_app_link_info[p_conn_param->host_id].state = STATE_CONNECTED;
            printf("Connected, ID=%d, Connected to %02x:%02x:%02x:%02x:%02x:%02x\n",
                       p_conn_param->host_id,
                       p_conn_param->peer_addr.addr[5],
                       p_conn_param->peer_addr.addr[4],
                       p_conn_param->peer_addr.addr[3],
                       p_conn_param->peer_addr.addr[2],
                       p_conn_param->peer_addr.addr[1],
                       p_conn_param->peer_addr.addr[0]);
        }
    }
    break;

    case BLE_GAP_EVT_CONN_PARAM_UPDATE:
    {
        ble_evt_gap_conn_param_update_t *p_conn_param = (ble_evt_gap_conn_param_update_t *)&p_param->event_param.ble_evt_gap.param.evt_conn_param_update;

        if (p_conn_param->status != BLE_HCI_ERR_CODE_SUCCESS)
        {
            printf("Connection update failed, error code = 0x%02x\n", p_conn_param->status);
        }
        else
        {
            printf("Connection updated\n");
            printf("ID: %d, ", p_conn_param->host_id);
            printf("Interval: %d, ", p_conn_param->conn_interval);
            printf("Latency: %d, ", p_conn_param->periph_latency);
            printf("Supervision Timeout: %d\n", p_conn_param->supv_timeout);
        }
    }
    break;

    case BLE_GAP_EVT_PHY_READ:
    case BLE_GAP_EVT_PHY_UPDATE:
    {
        ble_evt_gap_phy_t *p_phy_param = (ble_evt_gap_phy_t *)&p_param->event_param.ble_evt_gap.param.evt_phy;
        if (p_phy_param->status != BLE_HCI_ERR_CODE_SUCCESS)
        {
            printf("PHY update/read failed, error code = 0x%02x\n", p_phy_param->status);
        }
        else
        {
            printf("PHY updated/read, ID: %d, TX PHY: %d, RX PHY: %d\n", p_phy_param->host_id, p_phy_param->tx_phy, p_phy_param->rx_phy);
        }
    }
    break;

    case BLE_ATT_GATT_EVT_MTU_EXCHANGE:
    {
        ble_evt_mtu_t *p_mtu_param = (ble_evt_mtu_t *)&p_param->event_param.ble_evt_att_gatt.param.ble_evt_mtu;
        printf("MTU changed, ID: %d\n", p_mtu_param->host_id);
        printf("MTU size: %d\n", p_mtu_param->mtu);

    }
    break;

    case BLE_ATT_GATT_EVT_DATA_LENGTH_CHANGE:
    {
        ble_evt_data_length_change_t *p_data_len_param = (ble_evt_data_length_change_t *)&p_param->event_param.ble_evt_att_gatt.param.ble_evt_data_length_change;
        printf("Data length changed, ID: %d\n", p_data_len_param->host_id);
        printf("MaxTxOctets: %d  MaxTxTime:%d\n", p_data_len_param->max_tx_octets, p_data_len_param->max_tx_time);
        printf("MaxRxOctets: %d  MaxRxTime:%d\n", p_data_len_param->max_rx_octets, p_data_len_param->max_rx_time);
    }
    break;

    case BLE_GAP_EVT_DISCONN_COMPLETE:
    {
        ble_evt_gap_disconn_complete_t *p_disconn_param = (ble_evt_gap_disconn_complete_t *)&p_param->event_param.ble_evt_gap.param.evt_disconn_complete;
        event_disconnection_complete_param_t *p_disconn_complete;
        uint8_t event_len = 0;

        if (p_disconn_param->status == BLE_HCI_ERR_CODE_SUCCESS)
        {
            event_len = sizeof(event_disconnection_complete_param_t);
        }
        else
        {
            event_len = sizeof(uint8_t);
        }

        p_disconn_complete = pvPortMalloc(event_len);
        if (p_disconn_complete != NULL)
        {
            p_disconn_complete->status = p_disconn_param->status;
            if (p_disconn_param->status == BLE_HCI_ERR_CODE_SUCCESS)
            {
                p_disconn_param->reason = p_disconn_param->reason;
            }
            ble_esl_ap_event_send(AP_EVENT_DISCONNECTION_COMPLETE, (uint8_t *)p_disconn_complete, event_len);
            vPortFree(p_disconn_complete);
        }

        if (p_disconn_param->status != BLE_HCI_ERR_CODE_SUCCESS)
        {
            printf("Disconnect failed, error code = 0x%02x\n", p_disconn_param->status);
        }
        else
        {
            ble_app_link_info[p_disconn_param->host_id].state = STATE_STANDBY;

            printf("Disconnect, ID:%d, Reason:0x%02x\n", p_disconn_param->host_id, p_disconn_param->reason);
        }

        if (semaphore_l2cap_credit != NULL)
        {
            vSemaphoreDelete(semaphore_l2cap_credit);
            semaphore_l2cap_credit = NULL;
        }
        g_esl_control_point_cmd_dest_id = 0xFFFF;

    }
    break;

    case BLE_ATT_GATT_EVT_DB_PARSE_COMPLETE:
    {
        ble_err_t status;
        ble_evt_att_db_parse_complete_t *p_parsing_param = (ble_evt_att_db_parse_complete_t *)&p_param->event_param.ble_evt_att_gatt.param.ble_evt_att_db_parse_complete;

        if (p_parsing_param->host_id == APP_ESL_AP_C_HOST_ID)
        {
            printf("DB Parsing completed, ID:%d status:0x%02x\n", p_parsing_param->host_id, p_parsing_param->result);

            // Get LINK 0 all service handles and related information
            status = link0_svcs_handles_get(p_parsing_param->host_id);

            if (status != BLE_ERR_OK)
            {
                printf("Get handle fail, ID:%d status:%d\n", p_parsing_param->host_id, status);
            }

            // Do GATT commands
            if (app_request_set(APP_ESL_AP_C_HOST_ID, APP_REQUEST_ESL_AP_MULTI_CMD, false) == false)
            {
                // No application queue buffer. Error.
            }
        }
    }
    break;

    case BLE_SM_EVT_AUTH_STATUS:
    {
        ble_evt_sm_auth_status_t *p_auth_param = (ble_evt_sm_auth_status_t *)&p_param->event_param.ble_evt_sm.param.evt_auth_status;

        if (p_auth_param->status == BLE_HCI_ERR_CODE_SUCCESS)
        {
            ble_cmd_cccd_restore(p_auth_param->host_id);
        }

        printf("BLE authentication status = %d\n", p_auth_param->status);
    }
    break;

    case BLE_PADV_EVT_PADV_SUBEVENT_DATA_REQ:
    {
        ble_evt_padv_padv_subevent_data_req_t *p_data_req_param = (ble_evt_padv_padv_subevent_data_req_t *)&p_param->event_param.ble_evt_padv.param.evt_padv_subevent_data_req;
        
        //indicate that controller ready to transmit subevent data -> TX finish
        esl_update_control_point_esl_id_list(p_data_req_param->subevent_start);

        printf("adv_handle %d subevent[%d] req %d data\n", p_data_req_param->adv_handle, p_data_req_param->subevent_start, p_data_req_param->subevent_data_cnt);
    }
    break;
    
    case BLE_PADV_EVT_PADV_RESPONSE_REPORT:
    {
        ble_evt_padv_padv_rsp_report_t *p_rsp_report_param = (ble_evt_padv_padv_rsp_report_t *)&p_param->event_param.ble_evt_padv.param.evt_padv_rsp_report; 

        printf("adv handle[%d] rsp report, group:%d, tx_status:%d, rsp num:%d\n", p_rsp_report_param->adv_handle, p_rsp_report_param->subevent, p_rsp_report_param->tx_status, p_rsp_report_param->num_responses); 
        app_process_padv_response_report(p_rsp_report_param);
 
    }
    break;

    case BLE_EADV_EVT_ADV_SET_TERMINATED:
    {
        ble_evt_adv_set_terminated_t *p_adv_set_terminated = (ble_evt_adv_set_terminated_t *)&p_param->event_param.ble_evt_padv.param.evt_adv_set_terminated;

        printf("ADV set %d terminated status %d\n", p_adv_set_terminated->status, p_adv_set_terminated->adv_handle);
    }
    break;

    default:
        break;
    }
}


/* ------------------------------
 *  Application Task
 * ------------------------------
 */
/**
 * @brief BLE host callback for APP_GENERAL_EVENT.
 *
 * Copies the raw BLE event (p_param, sized by its extended_length) into a heap-allocated TLV and
 * posts it (QUEUE_TYPE_OTHERS / APP_GENERAL_EVENT) to the application message queue for later
 * dispatch by app_main_task()/ble_evt_handler(). Bounded by the semaphore_cb counting semaphore.
 *
 * @param p_param Pointer to the raw ble_evt_param_t event data supplied by the BLE host stack.
 * @return ble_err_t Result of queuing the event.
 * @retval BLE_ERR_OK Event queued successfully.
 * @retval BLE_BUSY No free semaphore_cb slot was available, or the application queue was full.
 * @retval BLE_ERR_DATA_MALLOC_FAIL Failed to allocate the TLV buffer.
 */
static ble_err_t ble_app_event_cb(void *p_param)
{
    ble_err_t status;
    app_queue_t p_app_q;
    ble_tlv_t *p_tlv;

    status = BLE_ERR_OK;
    if (xSemaphoreTake(semaphore_cb, 0) == pdTRUE)
    {
        p_tlv = pvPortMalloc(sizeof(ble_tlv_t) + sizeof(ble_evt_param_t) + ((ble_evt_param_t *)p_param)->extended_length);
        if (p_tlv != NULL)
        {
            p_app_q.param_type = QUEUE_TYPE_OTHERS;
            p_app_q.param.pt_tlv = p_tlv;
            p_app_q.param.pt_tlv->type = APP_GENERAL_EVENT;
            memcpy(p_tlv->value, p_param, sizeof(ble_evt_param_t) + ((ble_evt_param_t *)p_param)->extended_length);

            if (xQueueSendToBack(g_app_msg_q, &p_app_q, 1) != pdTRUE)
            {
                status = BLE_BUSY;
                vPortFree(p_tlv);
                xSemaphoreGive(semaphore_cb);
            }
        }
        else
        {
            status = BLE_ERR_DATA_MALLOC_FAIL;
            xSemaphoreGive(semaphore_cb);
        }
    }
    else
    {
        status = BLE_BUSY;
    }

    return status;
}

/**
 * @brief BLE host callback for APP_SERVICE_EVENT.
 *
 * Copies the raw GATT/service attribute event (p_param, sized by its length) into a
 * heap-allocated TLV and posts it (QUEUE_TYPE_OTHERS / APP_SERVICE_EVENT) to the application
 * message queue for later dispatch by app_main_task() to the matching client/server ATT handler.
 * Bounded by the semaphore_cb counting semaphore.
 *
 * @param p_param Pointer to the raw ble_evt_att_param_t event data supplied by the BLE host stack.
 * @return ble_err_t Result of queuing the event.
 * @retval BLE_ERR_OK Event queued successfully.
 * @retval BLE_BUSY No free semaphore_cb slot was available, or the application queue was full.
 * @retval BLE_ERR_DATA_MALLOC_FAIL Failed to allocate the TLV buffer.
 */
static ble_err_t ble_service_data_cb(void *p_param)
{
    ble_err_t status;
    app_queue_t p_app_q;
    ble_tlv_t *p_tlv;
    ble_evt_att_param_t *p_evt_att;

    status = BLE_ERR_OK;
    do {
        if (xSemaphoreTake(semaphore_cb, 0) == pdTRUE)
        {
            p_evt_att = p_param;
            p_tlv = pvPortMalloc(sizeof(ble_tlv_t) + sizeof(ble_evt_att_param_t) + p_evt_att->length);
            if (p_tlv == NULL)
            {
                status = BLE_ERR_DATA_MALLOC_FAIL;
                xSemaphoreGive(semaphore_cb);
                break;
            }

            p_app_q.param_type = QUEUE_TYPE_OTHERS;
            p_app_q.param.pt_tlv = p_tlv;
            p_app_q.param.pt_tlv->type = APP_SERVICE_EVENT;
            memcpy(p_tlv->value, p_param, sizeof(ble_evt_att_param_t) + p_evt_att->length);

            if (xQueueSendToBack(g_app_msg_q, &p_app_q, 1) != pdTRUE)
            {
                status = BLE_BUSY;
                vPortFree(p_tlv);
                xSemaphoreGive(semaphore_cb);
            }
        }
        else
        {
            status = BLE_BUSY;
        }
    } while (0);

    return status;
}

/**
 * @brief BLE host callback for APP_L2CAP_DATA_EVENT.
 *
 * Copies the raw L2CAP event (p_param, sized by its length) into a heap-allocated TLV and posts
 * it (QUEUE_TYPE_OTHERS / APP_L2CAP_DATA_EVENT) to the application message queue for later
 * dispatch by app_main_task() (connection response credit/MTU setup, flow-control credit
 * indications). Bounded by the semaphore_cb counting semaphore.
 *
 * @param p_param Pointer to the raw ble_l2cap_evt_param_t event data supplied by the BLE host stack.
 * @return ble_err_t Result of queuing the event.
 * @retval BLE_ERR_OK Event queued successfully.
 * @retval BLE_BUSY No free semaphore_cb slot was available, or the application queue was full.
 * @retval BLE_ERR_DATA_MALLOC_FAIL Failed to allocate the TLV buffer.
 */
static ble_err_t ble_l2cap_data_cb(void *p_param)
{
    ble_err_t status;
    app_queue_t p_app_q;
    ble_tlv_t *p_tlv;
    ble_l2cap_evt_param_t *p_evt_l2cap;

    status = BLE_ERR_OK;
    do {
        if (xSemaphoreTake(semaphore_cb, 0) == pdTRUE)
        {
            p_evt_l2cap = p_param;
            p_tlv = pvPortMalloc(sizeof(ble_tlv_t) + sizeof(ble_l2cap_evt_param_t) + p_evt_l2cap->length);
            if (p_tlv == NULL)
            {
                status = BLE_ERR_DATA_MALLOC_FAIL;
                xSemaphoreGive(semaphore_cb);
                break;
            }

            p_app_q.param_type = QUEUE_TYPE_OTHERS;
            p_app_q.param.pt_tlv = p_tlv;
            p_app_q.param.pt_tlv->type = APP_L2CAP_DATA_EVENT;
            memcpy(p_tlv->value, p_param, sizeof(ble_l2cap_evt_param_t) + p_evt_l2cap->length);

            if (xQueueSendToBack(g_app_msg_q, &p_app_q, 1) != pdTRUE)
            {
                status = BLE_BUSY;
                vPortFree(p_tlv);
                xSemaphoreGive(semaphore_cb);
            }
        }
        else
        {
            status = BLE_BUSY;
        }
    } while (0);

    return status;
}

/**
 * @brief Main application task loop.
 *
 * Performs one-time BLE initialization (ble_init()), halting the task on failure, then clears the
 * subevent ID-list table and loops forever waiting on the application message queue. Each entry
 * is dispatched by its param_type: QUEUE_TYPE_APP_REQ releases the app/ISR request semaphore and
 * calls ble_app_main(); QUEUE_TYPE_SUBEVENT_DATA_SET issues the periodic advertising subevent
 * data set command (re-queued at the front of the queue if the controller rejects it);
 * QUEUE_TYPE_OTHERS dispatches general BLE events, service/attribute events, or L2CAP events;
 * QUEUE_TYPE_UART_MSG forwards the message to ble_esl_ap_cmd_proc(). Frees the queued TLV and
 * releases the corresponding semaphore once each entry has been handled.
 *
 * @note This function never returns; it must be called once from the application entry task.
 */
static void app_main_task(void)
{
    ble_err_t status;
    app_queue_t p_app_q;

    // BLE default setting and profile init
    status = ble_init();
    if (status != BLE_ERR_OK)
    {
        printf("[DEBUG_ERR] ble_init() fail: %d\n", status);
        while (1);
    }

    memset(&subevent_data_list, 0x00, sizeof(subevent_data_list));
    for (;;)
    {
        g_app_rand_value++;
        if (xQueueReceive(g_app_msg_q, &p_app_q, portMAX_DELAY) == pdTRUE)
        {
            switch (p_app_q.param_type)
            {
            case QUEUE_TYPE_APP_REQ:
            {
                if (p_app_q.from_isr == true)
                {
                    xSemaphoreGive(semaphore_isr);
                }
                else
                {
                    xSemaphoreGive(semaphore_app);
                }
                ble_app_main(&p_app_q.param.app_req);
            }
            break;

            case QUEUE_TYPE_SUBEVENT_DATA_SET:
            {
                if (ble_cmd_periodic_adv_subevent_data_set((ble_periodic_adv_subevent_data_set_param_t *)p_app_q.param.pt_tlv) == BLE_ERR_OK)
                {
                    vPortFree(p_app_q.param.pt_tlv);
                    xSemaphoreGive(semaphore_subevent);
                }
                else
                {
                    if (xQueueSendToFront(g_app_msg_q, &p_app_q, 2) != pdTRUE)
                    {
                        // send error
                        vPortFree(p_app_q.param.pt_tlv);
                        xSemaphoreGive(semaphore_subevent);
                    }
                }
                
            }
            break;

            case QUEUE_TYPE_OTHERS:
            {
                if (p_app_q.param.pt_tlv != NULL)
                {
                    switch (p_app_q.param.pt_tlv->type)
                    {
                    case APP_GENERAL_EVENT:
                        ble_evt_handler((ble_evt_param_t *)p_app_q.param.pt_tlv->value);
                        break;

                    case APP_SERVICE_EVENT:
                    {
                        ble_evt_att_param_t *p_svcs_param = (ble_evt_att_param_t *)p_app_q.param.pt_tlv->value;
                        
                        switch (p_svcs_param->gatt_role)
                        {
                        case BLE_GATT_ROLE_CLIENT:
                            att_db_link[p_svcs_param->host_id].p_client_db[p_svcs_param->cb_index]->att_handler(p_svcs_param);
                            break;

                        case BLE_GATT_ROLE_SERVER:
                            att_db_link[p_svcs_param->host_id].p_server_db[p_svcs_param->cb_index]->att_handler(p_svcs_param);
                            break;

                        default:
                            break;
                        }
                    }
                    break;

                    case APP_L2CAP_DATA_EVENT:
                    {
                        ble_l2cap_evt_param_t *p_l2cap_event = (ble_l2cap_evt_param_t*)(p_app_q.param.pt_tlv->value);
                        //printf("APP GET L2CAP event %d\n", p_l2cap_event->event);
                        switch (p_l2cap_event->event)
                        {
                            case LE_L2CAP_EVT_CREDIT_BASED_CONNECT_REQ_RSP:
                                //printf("l2cap conn rsp, result %d, dest_id %d\n", p_l2cap_event->evt_param.l2cap_conn_req_rsp.result, 
                                                   //p_l2cap_event->evt_param.l2cap_conn_req_rsp.dest_id);
                                g_l2cap_data.dest_id = p_l2cap_event->evt_param.l2cap_conn_req_rsp.dest_id;
                                g_l2cap_data.mtu_size = p_l2cap_event->evt_param.l2cap_conn_req_rsp.mps;

                                if (semaphore_l2cap_credit != NULL)
                                {
                                    vSemaphoreDelete(semaphore_l2cap_credit);
                                }
                                semaphore_l2cap_credit = xSemaphoreCreateCounting(p_l2cap_event->evt_param.l2cap_conn_req_rsp.init_credits, p_l2cap_event->evt_param.l2cap_conn_req_rsp.init_credits);
                            break;

                            case LE_L2CAP_EVT_FLOW_CTRL_CREDIT_IND:
                            {
                                uint16_t return_credits;
                                event_image_transfer_complete_event_param_t *p_image_transfer_complete;
                                //printf("l2cap credit ind, cid %d, credits %d\n", p_l2cap_event->evt_param.l2cap_flow_ctrl_credit_ind.cid, 
                                                   //p_l2cap_event->evt_param.l2cap_flow_ctrl_credit_ind.credits);

                                return_credits = p_l2cap_event->evt_param.l2cap_flow_ctrl_credit_ind.credits;

                                p_image_transfer_complete = pvPortMalloc(sizeof(event_image_transfer_complete_event_param_t));
                                if (p_image_transfer_complete != NULL)
                                {
                                    p_image_transfer_complete->complete_num = return_credits;
                                    ble_esl_ap_event_send(AP_EVENT_ESL_IMAGE_TRANSFER_COMPLETE, (uint8_t *)p_image_transfer_complete, sizeof(event_image_transfer_complete_event_param_t));
                                    vPortFree(p_image_transfer_complete);
                                }

                                while (return_credits > 0)
                                {
                                    xSemaphoreGive(semaphore_l2cap_credit);
                                    return_credits --;
                                }
                            }
                            break;
                        }
                    }
                    break;

                    default:
                        break;
                    }
                    // free
                    vPortFree(p_app_q.param.pt_tlv);
                    xSemaphoreGive(semaphore_cb);
                }
            }
            break;

            case QUEUE_TYPE_UART_MSG:
            {
                if (p_app_q.param.pt_tlv != NULL)
                {
                    ble_esl_ap_cmd_proc(p_app_q.param.pt_tlv);
                    // free
                    vPortFree(p_app_q.param.pt_tlv);
                    xSemaphoreGive(semaphore_uart_msg);
                }
            }
            break;

            default:
                break;
            }
        }
    }
}

/**
 * @brief Top-level application-request dispatcher.
 *
 * Forwards the application request to the handler for the (single) central link (host id 0).
 *
 * @param p_param Pointer to the request parameters (host id and requested event).
 */
static void ble_app_main(app_req_param_t *p_param)
{
    // Link 0 (Central)
    app_central_handler(p_param);
}

/* ------------------------------
 *  Application Initializations
 * ------------------------------
 */
/**
 * @brief Initializes the GATT service (GATTS) profile data.
 *
 * Clears the Service Changed CCCD value before the GATTS profile is registered.
 *
 * @param p_data Pointer to the GATTS profile data to initialize.
 */
static void svcs_gatts_data_init(ble_svcs_gatts_data_t *p_data)
{
    p_data->service_changed_cccd = 0;
}

/**
 * @brief Initializes all client-role BLE service profiles for a link.
 *
 * Sets the link's state to STATE_STANDBY, then initializes the GAP, GATT, DIS, ESLS, OTS, and
 * FOTAS client profiles for host_id, registering the DIS/ESLS/OTS/FOTAS event handlers (GAP and
 * GATT are initialized with no event handler).
 *
 * @param host_id Link (host) identifier the profiles are initialized for.
 * @return ble_err_t Result of the initialization.
 * @retval BLE_ERR_OK All service profiles were initialized successfully.
 * @retval (other) ble_err_t code returned by whichever service init call failed first.
 */
static ble_err_t client_profile_init(uint8_t host_id)
{
    ble_err_t status = BLE_ERR_OK;
    ble_info_link0_t *p_profile_info = (ble_info_link0_t *)ble_app_link_info[host_id].profile_info;

    // set link's state
    ble_app_link_info[host_id].state = STATE_STANDBY;
    do
    {
        // GAP Related
        // -------------------------------------
        status = ble_svcs_gaps_init(host_id, BLE_GATT_ROLE_CLIENT, &(p_profile_info->svcs_info_gaps), NULL);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // GATT Related
        // -------------------------------------
        status = ble_svcs_gatts_init(host_id, BLE_GATT_ROLE_CLIENT, &(p_profile_info->svcs_info_gatts), NULL);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // DIS Related
        // -------------------------------------
        status = ble_svcs_dis_init(host_id, BLE_GATT_ROLE_CLIENT, &(p_profile_info->svcs_info_dis), ble_svcs_dis_evt_handler);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // ESL Related
        // -------------------------------------
        status = ble_svcs_esls_init(host_id, BLE_GATT_ROLE_CLIENT, &(p_profile_info->svcs_info_esls), ble_svcs_esls_evt_handler);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // OTS Related
        // -------------------------------------
        status = ble_svcs_ots_init(host_id, BLE_GATT_ROLE_CLIENT, &(p_profile_info->svcs_info_ots), ble_svcs_ots_evt_handler);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // FOTAS Related
        // -------------------------------------
        status = ble_svcs_fotas_init(host_id, BLE_GATT_ROLE_CLIENT, &(p_profile_info->svcs_info_fotas), ble_svcs_fotas_evt_handler);
        if (status != BLE_ERR_OK)
        {
            break;
        }
    } while (0);

    return status;
}

/**
 * @brief One-time BLE stack and application initialization.
 *
 * Registers the general/service/L2CAP host event callbacks, initializes the PHY controller, sets
 * the device address (using the factory-programmed unique code and IRK if available, otherwise
 * the fixed DEVICE_ADDR), initializes resolvable private address generation, sets the suggested
 * data length, and initializes the client profile for the (single) central link.
 *
 * @return ble_err_t Result of the initialization.
 * @retval BLE_ERR_OK Initialization completed successfully.
 * @retval (other) ble_err_t code returned by whichever initialization step failed first.
 */
static ble_err_t ble_init(void)
{
    ble_err_t status;
    ble_unique_code_format_t unique_code_param;
    ble_gap_addr_t device_addr_param;

    status = BLE_ERR_OK;

    do
    {
        status = ble_host_callback_set(APP_GENERAL_EVENT, ble_app_event_cb);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        status = ble_host_callback_set(APP_SERVICE_EVENT, ble_service_data_cb);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        status = ble_host_callback_set(APP_L2CAP_DATA_EVENT, ble_l2cap_data_cb);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        status = ble_cmd_phy_controller_init();
        if (status != BLE_ERR_OK)
        {
            break;
        }

        status = ble_cmd_read_unique_code(&unique_code_param);
        if (status == BLE_ERR_OK)
        {
            device_addr_param.addr_type = unique_code_param.addr_type;
            memcpy(&device_addr_param.addr, &unique_code_param.ble_addr, 6);
            status = ble_cmd_device_addr_set((ble_gap_addr_t *)&device_addr_param);
            if (status != BLE_ERR_OK)
            {
                break;
            }
            status = ble_cmd_write_identity_resolving_key((ble_sm_irk_param_t *)&unique_code_param.ble_irk[0]);
            if (status != BLE_ERR_OK)
            {
                break;
            }
        }
        else
        {
            status = ble_cmd_device_addr_set((ble_gap_addr_t *)&DEVICE_ADDR);
            if (status != BLE_ERR_OK)
            {
                break;
            }
        }

        status = ble_cmd_resolvable_address_init();
        if (status != BLE_ERR_OK)
        {
            break;
        }

        status = ble_cmd_suggest_data_len_set(BLE_GATT_DATA_LENGTH_MAX);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // BLE profile init --> only 1 link (host id = 0)
        status = client_profile_init(APP_ESL_AP_C_HOST_ID);
        if (status != BLE_ERR_OK)
        {
            break;
        }

    } while (0);

    return status;
}

/**
 * @brief UART receive callback that queues a parsed UART command for the application task.
 *
 * Wraps pt_tlv into an app_queue_t (QUEUE_TYPE_UART_MSG) and posts it to the application message
 * queue for app_main_task() to forward to ble_esl_ap_cmd_proc(). Bounded by the
 * semaphore_uart_msg counting semaphore; frees pt_tlv (and releases the semaphore if it had been
 * taken) if the queue is full or no semaphore slot is available.
 *
 * @param pt_tlv Pointer to the parsed UART command TLV to queue; ownership is transferred to this
 *               function (it is freed on failure or by app_main_task() on success).
 */
static void app_uart_msg_recv(ble_tlv_t *pt_tlv)
{
    app_queue_t p_app_q;

    p_app_q.event = 0; // from BLE
    p_app_q.param_type = QUEUE_TYPE_UART_MSG;
    p_app_q.param.pt_tlv = (ble_tlv_t *)pt_tlv;

    if (xSemaphoreTake(semaphore_uart_msg, 0) == pdTRUE)
    {
        p_app_q.from_isr = false;
        if (xQueueSendToBack(g_app_msg_q, &p_app_q, 0) != pdTRUE)
        {
            // send error
            xSemaphoreGive(semaphore_uart_msg);
            vPortFree(pt_tlv);
        }
    }
    else
    {
        printf("semaphore_uart_msg xSemaphoreTake fail\n");
        vPortFree(pt_tlv);

    }
}

/**
 * @brief Performs application-level startup before the main task loop runs.
 *
 * Prints the startup banner, creates the application message queue and its counting semaphores
 * (callback, ISR request, app request, subevent, UART message) plus the FOTA command queue,
 * initializes the BLE host stack with the configured task priorities, wires up the UART command
 * parser/receive callbacks, and (if enabled) starts the debug CLI console.
 */
static void app_init(void)
{
    ble_task_priority_t ble_task_level;
    hosal_gpio_input_config_t input_cfg;
    uart_handler_parm_t uart_handler_param = {0};

    // banner
    printf("------------------------------------------\n");
    printf("  BLE ESL AP (C) demo: start...\n");
    printf("------------------------------------------\n");

    // application queue & semaphore
    g_app_msg_q = xQueueCreate(APP_QUEUE_SIZE, sizeof(app_queue_t));
    semaphore_cb = xSemaphoreCreateCounting(BLE_APP_CB_QUEUE_SIZE, BLE_APP_CB_QUEUE_SIZE);
    semaphore_isr = xSemaphoreCreateCounting(APP_ISR_QUEUE_SIZE, APP_ISR_QUEUE_SIZE);
    semaphore_app = xSemaphoreCreateCounting(APP_REQ_QUEUE_SIZE, APP_REQ_QUEUE_SIZE);
    semaphore_subevent = xSemaphoreCreateCounting(SET_SUBEVENT_DATA_QUEUE_SIZE, SET_SUBEVENT_DATA_QUEUE_SIZE);
    semaphore_uart_msg = xSemaphoreCreateCounting(UART_MSG_QUEUE_SIZE, UART_MSG_QUEUE_SIZE);

    g_app_fota_cmd_q = xQueueCreate(APP_FOTA_CMD_QUEUE_SIZE, sizeof(app_fota_cmd_queue_t));
    // BLE Stack init  
    ble_task_level.ble_host_level = configMAX_PRIORITIES - 7;
    ble_task_level.hci_tx_level = configMAX_PRIORITIES - 6;
    if (ble_host_stack_init(&ble_task_level) == 0) {
        printf("BLE stack initial success...\n");
    }
    else {
        printf("BLE stack initial fail...\n");
    }

    uart_handler_param.UartParserCB[0] = ble_esl_ap_cmd_parser;
    uart_handler_param.UartRecvCB[0] = app_uart_msg_recv;

    uart_handler_init(&uart_handler_param, configMAX_PRIORITIES - 10);
#if (SUPPORT_DEBUG_CONSOLE_CLI == 1)
    extern int cli_console_init(void);
    cli_console_init();
#endif

}

/**
 * @brief Encrypts data for an encrypted periodic advertising payload.
 *
 * Runs AES-CCM encryption (aes_ccm_encryption()) over p_data using the AP synchronization key
 * (esl_ap_sync_key) and a nonce built from p_rand and esl_ap_nonce, writing the header byte,
 * ciphertext, and MIC into p_enc_data.
 *
 * @param p_rand Pointer to the 5-byte randomizer used to build the CCM nonce.
 * @param p_data Pointer to the plaintext ESL Control Point command data to encrypt.
 * @param data_len Length in bytes of the plaintext data.
 * @param p_enc_data Output buffer that receives the header byte, ciphertext, and MIC.
 * @return uint32_t Total length in bytes written to p_enc_data (header + ciphertext + MIC).
 * @retval 0 Encryption failed.
 */
static uint32_t app_enc_adv_data(uint8_t *p_rand, uint8_t *p_data, uint8_t data_len, uint8_t *p_enc_data)
{
    struct aes_ctx ctx;
    struct aes_ccm_encryption_packet  ccm_packet;
    uint8_t nonce[13], headr;
    uint32_t status, outbuf_length;

    aes_acquire(&ctx);
    aes_key_init(&ctx, esl_ap_sync_key, AES_KEY128);

    /*load key to secure engine*/
    aes_load_round_key(&ctx);

    memcpy(nonce, p_rand, 5);
    memcpy(nonce + 5, esl_ap_nonce, 8);
    ccm_packet.nonce = (uint8_t *) nonce;
    ccm_packet.hdr_len = 1;

    headr = ESL_ENADV_CCM_HEADER;
    ccm_packet.hdr = &headr;
    ccm_packet.data_len = data_len;
    ccm_packet.data = p_data;
    ccm_packet.mlen = ESL_ENADV_MIC_SIZE;
    ccm_packet.out_buf = p_enc_data;

    outbuf_length = data_len + ccm_packet.mlen + ccm_packet.hdr_len;
    ccm_packet.out_buf_len = &outbuf_length;

    status = aes_ccm_encryption( &ccm_packet);
    aes_release(&ctx);

    if (status == STATUS_SUCCESS)
    {
        return outbuf_length;
    }
    else
    {
        return 0;
    }
}

/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/
/**
 * @brief Queues an application-level request for the given link.
 *
 * Builds an app_queue_t entry of type QUEUE_TYPE_APP_REQ and posts it to the application message
 * queue so it will be handled by app_main_task()/ble_app_main(). Uses the semaphore_app counting
 * semaphore in normal (task) context or the semaphore_isr counting semaphore and the ISR-safe
 * FreeRTOS APIs when from_isr is true; the semaphore is released again if the queue send fails.
 *
 * @param host_id Link (host) identifier the request applies to.
 * @param request Application request event to queue.
 * @param from_isr true if called from interrupt context, false for normal task context.
 * @return true if the request was successfully queued.
 * @retval false No free request semaphore slot was available, or the application queue was full.
 */
bool app_request_set(uint8_t host_id, app_request_t request, bool from_isr)
{
    app_queue_t p_app_q;

    p_app_q.event = 0; // from BLE
    p_app_q.param_type = QUEUE_TYPE_APP_REQ;
    p_app_q.param.app_req.host_id = host_id;
    p_app_q.param.app_req.app_req = request;

    if (from_isr == false)
    {
        if (xSemaphoreTake(semaphore_app, 0) == pdTRUE)
        {
            p_app_q.from_isr = false;
            if (xQueueSendToBack(g_app_msg_q, &p_app_q, 0) != pdTRUE)
            {
                // send error
                xSemaphoreGive(semaphore_app);
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        BaseType_t context_switch = pdFALSE;

        if (xSemaphoreTakeFromISR(semaphore_isr, &context_switch) == pdTRUE)
        {
            p_app_q.from_isr = true;
            context_switch = pdFALSE;
            if (xQueueSendToBackFromISR(g_app_msg_q, &p_app_q, &context_switch) != pdTRUE)
            {
                context_switch = pdFALSE;
                xSemaphoreGiveFromISR(semaphore_isr, &context_switch);
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief Resets the outbound L2CAP transfer's length bookkeeping without touching the data buffer.
 *
 * Unlike esl_update_l2cap_data(), this does not allocate or copy a data buffer; it only resets
 * g_l2cap_data.total_tx_len/curr_tx_len, assuming the buffer is already populated by the caller.
 *
 * @param data_len Total length in bytes of the data that will be transmitted.
 * @return ble_err_t Result of the operation.
 * @retval BLE_ERR_OK Length bookkeeping reset successfully.
 * @retval BLE_ERR_INVALID_PARAMETER data_len exceeds APP_L2CAP_BUFFER_SIZE.
 */
ble_err_t esl_update_l2cap_data_info(uint16_t data_len)
{
    ble_err_t status = BLE_ERR_OK;

    if (data_len > APP_L2CAP_BUFFER_SIZE)
    {
        status = BLE_ERR_INVALID_PARAMETER;
    }
    else
    {
        g_l2cap_data.total_tx_len = data_len;
        g_l2cap_data.curr_tx_len = 0;
    }
    return status;
}

ble_err_t esl_update_l2cap_data(uint8_t *p_l2cap_data, uint16_t data_len)
{
    ble_err_t status;

    if (data_len > APP_L2CAP_BUFFER_SIZE)
    {
        status = BLE_ERR_INVALID_PARAMETER;
    }
    else if (g_l2cap_data.p_data_buffer != NULL)
    {
        status = BLE_BUSY;
    }
    else
    {
        g_l2cap_data.total_tx_len = data_len;
        g_l2cap_data.curr_tx_len = 0;
        g_l2cap_data.p_data_buffer = pvPortMalloc(data_len);
        if (g_l2cap_data.p_data_buffer != NULL)
        {
            memcpy(g_l2cap_data.p_data_buffer, p_l2cap_data, data_len);
            status = BLE_ERR_OK;
        }
        else
        {
            status = BLE_ERR_DATA_MALLOC_FAIL;
        }
    }
    return status;
}

ble_err_t esl_control_point_cmd_send(uint8_t *p_control_point_cmd, uint8_t cmd_len)
{
    ble_gatt_data_param_t data_param;
    ble_info_link0_t *p_profile_info;
    ble_err_t status;

    p_profile_info = (ble_info_link0_t *)ble_app_link_info[0].profile_info;
    data_param.handle_num = p_profile_info->svcs_info_esls.client_info.handles.hdl_esl_control_point;
    data_param.host_id = APP_ESL_AP_C_HOST_ID;
        
    data_param.length = cmd_len;
    data_param.p_data = (uint8_t*)p_control_point_cmd;

    status = ble_svcs_data_send(TYPE_BLE_GATT_WRITE_REQ, &data_param);
    if (status != BLE_ERR_OK)
    {
        //printf("write control point cmd fail, opcode 0x%02x esl id %d\n", data_param.p_data[0], data_param.p_data[1]);
    }
    return status;
}

ble_err_t ap_sync_stop(void)
{
    ble_err_t status = BLE_ERR_UNKNOW_TYPE;
    ble_periodic_adv_disable_set_param_t *p_padv_disable;

    p_padv_disable = pvPortMalloc(sizeof(ble_periodic_adv_disable_set_param_t));
    if (p_padv_disable == NULL)
    {
        printf("p_padv_disable malloc fail\n");
        status = BLE_ERR_ALLOC_MEMORY_FAIL;
    }
    else
    {
        p_padv_disable->adv_handle = ESL_AP_ADV_HANDLE;
        status = ble_cmd_periodic_adv_disable_set(p_padv_disable);
        vPortFree(p_padv_disable);

         if (status != BLE_ERR_OK)
        {
            printf("periodic ADV disable fail %d\n", status);
        }
    }
    return status;
}

ble_err_t ap_sync_start(ap_sync_start_t *p_sync_param)
{
    ble_err_t status = BLE_ERR_UNKNOW_TYPE;
    ble_extended_adv_param_set_t *p_extend_adv_param;
    ble_adv_set_random_address_set_t *p_adv_set_addr;
    ble_periodic_adv_param_set_param_t *p_padv_param;
    ble_periodic_adv_enable_set_param_t *p_padv_eanble; 
    ble_extended_adv_enable_set_t *p_eadv_enable;
    ble_gap_addr_t own_addr;

    do
    {
        p_extend_adv_param = pvPortMalloc(sizeof(ble_extended_adv_param_set_t));
        if (p_extend_adv_param == NULL)
        {
            printf("p_extend_adv_param mallc fail\n");
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
            break;
        }
        p_extend_adv_param->adv_handle = ESL_AP_ADV_HANDLE;
        p_extend_adv_param->adv_event_properties = 0;
        p_extend_adv_param->primary_adv_interval_min = 0x280;
        p_extend_adv_param->primary_adv_interval_max = 0x280;
        p_extend_adv_param->primary_adv_ch_map = ADV_CHANNEL_ALL;

        ble_gap_device_address_get(&own_addr);
        p_extend_adv_param->own_address_type = own_addr.addr_type;
        p_extend_adv_param->peer_address_type = 0x01;
        p_extend_adv_param->peer_address[0] = 0x88;
        p_extend_adv_param->peer_address[1] = 0x88;
        p_extend_adv_param->peer_address[2] = 0x88;
        p_extend_adv_param->peer_address[3] = 0x88;
        p_extend_adv_param->peer_address[4] = 0x88;
        p_extend_adv_param->peer_address[5] = 0xCC;
        p_extend_adv_param->adv_filter_policy = ADV_FILTER_POLICY_ACCEPT_ALL;
        p_extend_adv_param->primary_adv_phy = BLE_PHY_1M;
        p_extend_adv_param->secondary_adv_max_skip = 0x00;
        p_extend_adv_param->secondary_adv_phy = BLE_PHY_1M;
        p_extend_adv_param->adv_sid = 0x00;
        p_extend_adv_param->scan_req_notification_enable = 0x01;

        status = ble_cmd_extended_adv_parameter_set(p_extend_adv_param);
        vPortFree(p_extend_adv_param);
        if (status != BLE_ERR_OK)
        {
            printf("extended ADV parameter set fail %d\n", status); 
            break;
        }

        p_adv_set_addr = pvPortMalloc(sizeof(ble_adv_set_random_address_set_t));
        if (p_adv_set_addr == NULL)
        {
            printf("p_adv_set_addr mallc fail\n");
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
            break;
        }

        p_adv_set_addr->adv_handle = ESL_AP_ADV_HANDLE;
        memcpy(p_adv_set_addr->addr, DEVICE_ADDR.addr, BLE_ADDR_LEN);
        status = ble_cmd_adv_set_random_addr_set(p_adv_set_addr);
        vPortFree(p_adv_set_addr);
        if (status != BLE_ERR_OK)
        {
            printf("adv set random address set fail %d\n", status); 
            break;
        }

        p_padv_param = pvPortMalloc(sizeof(ble_periodic_adv_param_set_param_t));
        if (p_padv_param == NULL)
        {
            printf("p_padv_param malloc fail\n");
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
            break;
        }
        p_padv_param->adv_handle = ESL_AP_ADV_HANDLE;
        p_padv_param->padv_interval_min = p_sync_param->padv_interval;// padv_interval*1.25ms  
        p_padv_param->padv_interval_max = p_sync_param->padv_interval;// padv_interval*1.25ms
        p_padv_param->padv_properties = 0;   
        p_padv_param->num_subevents = p_sync_param->num_subevents;// 15;
        p_padv_param->subevent_interval =  p_sync_param->subevent_interval;//subevent_interval*1.25ms
        p_padv_param->rsp_slot_delay =  p_sync_param->rsp_slot_delay;//rsp_slot_delay*1.25ms
        p_padv_param->rsp_slot_spacing = p_sync_param->rsp_slot_spacing;//rsp_slot_spacing* 0.125ms
        p_padv_param->num_rsp_slot = p_sync_param->num_rsp_slot;
        status = ble_cmd_periodic_adv_parameter_set(p_padv_param);
        vPortFree(p_padv_param);

        if (status != BLE_ERR_OK)
        {
            printf("periodic ADV parameter set fail %d\n", status);
            break;
        }

        p_padv_eanble = pvPortMalloc(sizeof(ble_periodic_adv_enable_set_param_t));
        if (p_padv_eanble == NULL)
        {
            printf("p_padv_eanble mallc fail\n");
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
            break;
        }

        p_padv_eanble->adv_handle = ESL_AP_ADV_HANDLE;
        status = ble_cmd_periodic_adv_enable_set(p_padv_eanble);
        vPortFree(p_padv_eanble);

        if (status != BLE_ERR_OK)
        {
            printf("periodic ADV enable fail %d\n", status);
            break;
        }

        p_eadv_enable = pvPortMalloc(sizeof(ble_extended_adv_enable_set_t));
        if (p_eadv_enable == NULL)
        {
            printf("p_eadv_enable malloc fail\n");
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
            break;
        }
        p_eadv_enable->enable = 0x01;
        p_eadv_enable->adv_handle = ESL_AP_ADV_HANDLE;
        p_eadv_enable->duration = 100;
        p_eadv_enable->max_extended_adv_events = 0;
        status = ble_cmd_extended_adv_enable_set(BLE_HOSTID_RESERVED, p_eadv_enable);
        vPortFree(p_eadv_enable);
        if (status != BLE_ERR_OK)
        {
            printf("extended ADV enable fail %d\n", status);
        }

    } while (0);

    return status;
}

ble_err_t ap_config_esl_device(ap_config_esl_device_t *p_config_device)
{
    uint16_t esl_id, retry_cnt = 6000, curr_cnt = 0;
    ble_gatt_data_param_t data_param;
    ble_info_link0_t *p_profile_info;
    uint8_t *p_data;
    ble_err_t status;

    p_profile_info = (ble_info_link0_t *)ble_app_link_info[0].profile_info;
    data_param.handle_num = p_profile_info->svcs_info_esls.client_info.handles.hdl_esl_address;
    data_param.host_id = 0;
    data_param.length = sizeof(p_config_device->esl_id);

    do
    {
        p_data = pvPortMalloc(sizeof(p_config_device->esl_id));

        if (p_data != NULL)
        {
            p_data[0] = (p_config_device->esl_id & 0xff);
            p_data[1] = ((p_config_device->esl_id >> 8) & 0xff);
            data_param.p_data = p_data;

            status = ble_svcs_data_send(TYPE_BLE_GATT_WRITE_REQ, &data_param);
            vPortFree(p_data);
            if (status != BLE_ERR_OK)
            {
                printf("write ESL ID fail %d\n", status);
                break;
            }
        }
        else
        {
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
            break;
        }

        data_param.handle_num = p_profile_info->svcs_info_esls.client_info.handles.hdl_ap_sync_key_material;
        data_param.host_id = 0;
        data_param.length = sizeof(uint8_t)*24;
        p_data = pvPortMalloc(data_param.length);
        if (p_data != NULL)
        {
            _memcpy_inv(p_data, p_config_device->p_ap_sync_key, 16);
            memcpy(p_data + 16,  p_config_device->p_ap_sync_key + 16, 8);

            data_param.p_data = p_data;
            curr_cnt = 0;
            while (ble_svcs_data_send(TYPE_BLE_GATT_WRITE_REQ, &data_param) != BLE_ERR_OK)
            {
                curr_cnt++;
                if(curr_cnt > retry_cnt)
                {
                    printf("write AP sync key fail\n");
                    status = BLE_BUSY;
                    break;
                }
                vTaskDelay(2);
            }
            vPortFree(p_data);
            if (status == BLE_BUSY)
            {
                break;
            }
        }
        else
        {
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
            break;
        }

        data_param.handle_num = p_profile_info->svcs_info_esls.client_info.handles.hdl_esl_rsp_key_material;
        data_param.host_id = 0;
        data_param.length = sizeof(uint8_t)*24;
        p_data = pvPortMalloc(data_param.length);
        if (p_data != NULL)
        {
            _memcpy_inv(p_data, p_config_device->p_esl_rsp_key, 16);
            memcpy(p_data + 16, p_config_device->p_esl_rsp_key + 16, 8);

            data_param.p_data = p_data;

            curr_cnt = 0;
            while (ble_svcs_data_send(TYPE_BLE_GATT_WRITE_REQ, &data_param) != BLE_ERR_OK)
            {
                curr_cnt++;
                if(curr_cnt > retry_cnt)
                {
                    printf("write esl response key fail\n");
                    status = BLE_BUSY;
                    break;
                }
                vTaskDelay(2);
            }
            vPortFree(p_data);
            if (status == BLE_BUSY)
            {
                break;
            }
        }
        else
        {
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
            break;
        }

        data_param.handle_num = p_profile_info->svcs_info_esls.client_info.handles.hdl_esl_current_absolute_time;
        data_param.host_id = 0;
        data_param.length = sizeof(uint32_t);
        p_data = pvPortMalloc(sizeof(uint32_t));
        if (p_data != NULL)
        {
            //p_config_device->current_time =  xTaskGetTickCount();
            p_data[0] = (p_config_device->current_time & 0xff);
            p_data[1] = ((p_config_device->current_time >> 8) & 0xff);
            p_data[2] = ((p_config_device->current_time >> 16) & 0xff);
            p_data[3] = ((p_config_device->current_time >> 24) & 0xff);
            data_param.p_data = p_data;

            curr_cnt = 0;
            while (ble_svcs_data_send(TYPE_BLE_GATT_WRITE_REQ, &data_param) != BLE_ERR_OK)
            {
                curr_cnt++;
                if(curr_cnt > retry_cnt)
                {
                    printf("write absolute time fail\n");
                    status = BLE_BUSY;
                    break;
                }
                vTaskDelay(2);
            }

            vPortFree(p_data);
            if (status == BLE_BUSY)
            {
                break;
            }
        }
        else
        {
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
            break;
        }

    } while (0);

    return status;
}

ble_err_t object_action_control_point_cmd_send(uint8_t *p_control_point_cmd, uint8_t cmd_len)
{
    ble_gatt_data_param_t data_param;
    ble_info_link0_t *p_profile_info;
    ble_err_t status;

    p_profile_info = (ble_info_link0_t *)ble_app_link_info[APP_ESL_AP_C_HOST_ID].profile_info;
    data_param.handle_num = p_profile_info->svcs_info_ots.client_info.handles.hdl_object_action_control_point;
    data_param.host_id = APP_ESL_AP_C_HOST_ID;

    data_param.length = cmd_len;
    data_param.p_data = (uint8_t*)p_control_point_cmd;

    status = ble_svcs_data_send(TYPE_BLE_GATT_WRITE_REQ, &data_param);
    if (status != BLE_ERR_OK)
    {
        //printf("write control point cmd fail, opcode 0x%02x esl id %d\n", data_param.p_data[0], data_param.p_data[1]);
    }
    return status;
}

ble_err_t object_list_control_point_cmd_send(uint8_t *p_control_point_cmd, uint8_t cmd_len)
{
    ble_gatt_data_param_t data_param;
    ble_info_link0_t *p_profile_info;
    ble_err_t status;

    p_profile_info = (ble_info_link0_t *)ble_app_link_info[APP_ESL_AP_C_HOST_ID].profile_info;
    data_param.handle_num = p_profile_info->svcs_info_ots.client_info.handles.hdl_object_list_control_point;
    data_param.host_id = APP_ESL_AP_C_HOST_ID;

    data_param.length = cmd_len;
    data_param.p_data = (uint8_t*)p_control_point_cmd;

    status = ble_svcs_data_send(TYPE_BLE_GATT_WRITE_REQ, &data_param);
    if (status != BLE_ERR_OK)
    {
        //printf("write control point cmd fail, opcode 0x%02x esl id %d\n", data_param.p_data[0], data_param.p_data[1]);
    }
    return status;
}

ble_err_t esl_control_point_cmd_send_over_encrypt_adv(esl_control_point_cmd_send_t *p_esl_cmd)
{
    ble_periodic_adv_subevent_data_set_param_t *p_subevent_data;
    periodic_adv_subevent_param_t *p_subevent_parm;
    esls_padv_enc_hdr_param_t *p_enc_header;
    esls_padv_enc_data_param_t *p_enc_data;
    ble_err_t status = BLE_ERR_OK;
    uint8_t temp_len = 0, i, *p_enc_payload, enc_len;

    if (p_esl_cmd->group_num > MAX_ESL_CTRL_POINT_GROUP_NUM)
    {
        status = BLE_ERR_INVALID_PARAMETER;
    }
    else
    {
        temp_len = sizeof(ble_periodic_adv_subevent_data_set_param_t);
        for (i = 0; i < p_esl_cmd->group_num; i++)
        {
            temp_len += sizeof(periodic_adv_subevent_param_t);
            temp_len += (p_esl_cmd->group_cmd[i].cmd_len + ESL_ENADV_MIC_SIZE + sizeof(esls_padv_enc_hdr_param_t) + sizeof(esls_padv_enc_data_param_t));
        }
        p_subevent_data = pvPortMalloc(temp_len);

        if (p_subevent_data != NULL)
        {
            p_subevent_data->adv_handle = ESL_AP_ADV_HANDLE;
            p_subevent_data->num_subevents = p_esl_cmd->group_num;
            temp_len = 0;

            for (i = 0; i < p_esl_cmd->group_num; i++)
            {
                p_subevent_parm = (periodic_adv_subevent_param_t*)(p_subevent_data->subevents + temp_len);
                p_subevent_parm->subevent = p_esl_cmd->group_cmd[i].group_id;
                p_subevent_parm->rsp_slot_start = 0;
                p_subevent_parm->rsp_slot_count = p_esl_cmd->group_cmd[i].cmd_num;
                p_subevent_parm->subevent_data_len = p_esl_cmd->group_cmd[i].cmd_len + ESL_ENADV_MIC_SIZE + sizeof(esls_padv_enc_hdr_param_t) + sizeof(esls_padv_enc_data_param_t);
                p_enc_header = (esls_padv_enc_hdr_param_t*)(p_subevent_parm->subevent_data);
                ESL_ENADV_FILL_ENC_HEADER(p_subevent_parm->subevent_data_len, p_enc_header);

                p_enc_data = (esls_padv_enc_data_param_t*)p_enc_header->enc_data;
                ESL_ENADV_FILL_ENC_DATA(p_esl_cmd->group_cmd[i].group_id, p_esl_cmd->group_cmd[i].cmd_len, p_esl_cmd->group_cmd[i].p_cmd, p_enc_data);

                p_enc_payload = pvPortMalloc(p_esl_cmd->group_cmd[i].cmd_len + ESL_ENADV_MIC_SIZE + 0x01/*addition data "0xEA"*/+ sizeof(esls_padv_enc_data_param_t));
                if (p_enc_payload != NULL)
                {
                    enc_len = app_enc_adv_data(p_enc_header->randomizer, p_enc_header->enc_data, p_esl_cmd->group_cmd[i].cmd_len + sizeof(esls_padv_enc_data_param_t), p_enc_payload);
                    memcpy(p_enc_header->enc_data, p_enc_payload+1, enc_len - 1);  //shift one byte addition data "0xEA"
                    vPortFree(p_enc_payload);
                }
                else
                {
                    status = BLE_ERR_ALLOC_MEMORY_FAIL;
                    break;
                }
                temp_len += (sizeof(periodic_adv_subevent_param_t) + p_subevent_parm->subevent_data_len);
            }

            if(status == BLE_ERR_OK)
            {
                app_queue_t p_app_q;

                p_app_q.event = 0; // from BLE
                p_app_q.param_type = QUEUE_TYPE_SUBEVENT_DATA_SET;
                p_app_q.param.pt_tlv = (ble_tlv_t *)p_subevent_data;

                if (xSemaphoreTake(semaphore_subevent, 0) == pdTRUE)
                {
                    p_app_q.from_isr = false;
                    if (xQueueSendToBack(g_app_msg_q, &p_app_q, 0) != pdTRUE)
                    {
                        // send error
                        xSemaphoreGive(semaphore_subevent);
                        vPortFree(p_subevent_data);
                        status = BLE_BUSY;
                    }
                }
                else
                {
                    vPortFree(p_subevent_data);
                    status = BLE_ERR_SEMAPHORE_MALLOC_FAIL;
                }
            }
            else
            {
                vPortFree(p_subevent_data);
            }
        }
        else
        {
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
        }
    }
    return status;
}

/**
 * @brief Initializes the pin multiplexing.
 *
 * This function sets all GPIO pins to GPIO mode, except for GPIO16 and GPIO17,
 * which are reserved for specific functions.
 *
 * @return void This function does not return a value.
 */
static void pin_mux_init(void) 
{
    /*set all pin to gpio, except GPIO16, GPIO17 */
    for (int i = 0; i < 32; i++) {
        if (i == 16 || i == 17) {
            continue; // Skip GPIO16 and GPIO17
        }
        hosal_pin_set_mode(i, HOSAL_MODE_GPIO);
    }
}

/**
 * @brief Main entry point for the application.
 *
 * This function initializes the RF module, starts the application initialization,
 * and enters an infinite loop to keep the application running.
 *
 * @param pvParameters Pointer to parameters passed to the task (not used).
 * @return void This function does not return a value.
 */
static void app_main_entry(void* pvParameters)
{
    hosal_lpm_init();
    hosal_rf_init(HOSAL_RF_MODE_BLE_CONTROLLER);

    /* application init */
    app_init();
    app_main_task();

    while (1) {
    }
}

/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/
void esl_record_control_point_esl_id(uint8_t group_id, uint8_t esl_id)
{
    g_esl_control_point_cmd_dest_id = ((group_id << 8) | esl_id);
}

void esl_get_control_point_esl_id(uint8_t *p_group_id, uint8_t *p_esl_id)
{
    *p_group_id = ((g_esl_control_point_cmd_dest_id >> 8) & 0xFF);
    *p_esl_id = (g_esl_control_point_cmd_dest_id & 0xFF);

    g_esl_control_point_cmd_dest_id = 0xFFFF;
}

uint8_t esl_control_point_cmd_processing(void)
{
    return (g_esl_control_point_cmd_dest_id == 0xFFFF)? FALSE:TRUE;
}

ble_err_t esl_add_control_point_esl_id_list(uint8_t group_id, uint8_t esl_cmd_num, uint8_t *p_esl_id)
{
    ble_err_t err_code = BLE_ERR_UNKNOW_TYPE;
    uint8_t i, j;

    do
    {
        if (esl_cmd_num >= MAX_RSP_SLOT)
        {
            err_code = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        i = ((subevent_data_list.write_index + 1) % MAX_SUBEVENT_NUM);
        if (i == subevent_data_list.read_index)
        {
            err_code = BLE_BUSY;
            break;
        }
        subevent_data_list.write_index = i;
        subevent_data_list.subevent[i].group_id = group_id;
        subevent_data_list.subevent[i].esl_id_num = esl_cmd_num;

        if (subevent_data_list.subevent[i].p_esl_id != NULL)
        {
            vPortFree(subevent_data_list.subevent[i].p_esl_id);
            subevent_data_list.subevent[i].p_esl_id = NULL;
        }
        subevent_data_list.subevent[i].p_esl_id = pvPortMalloc(sizeof(uint8_t) * esl_cmd_num);

        if (subevent_data_list.subevent[i].p_esl_id == NULL)
        {
            err_code = BLE_ERR_ALLOC_MEMORY_FAIL;
            break;
        }

        j = 0;
        do
        {
            subevent_data_list.subevent[i].p_esl_id[j] = p_esl_id[j];
            j++;
        } while (j < esl_cmd_num);
        err_code = BLE_ERR_OK;

    } while (0);

    return err_code;
}

ble_err_t esl_update_control_point_esl_id_list(uint8_t group_id)
{
    ble_err_t err_code = BLE_ERR_UNKNOW_TYPE;
    uint8_t i, j;

    do
    {
        if (subevent_data_list.write_index == subevent_data_list.read_index)
        {
            break;
        }
        i = ((subevent_data_list.read_index + 1) % MAX_SUBEVENT_NUM);

        if (subevent_data_list.subevent[i].group_id != group_id)
        {
            err_code = BLE_ERR_INVALID_PARAMETER;
            break;
        }
        subevent_data_list.read_index = i;
        err_code = BLE_ERR_OK;

    } while (0);

    return err_code;
}

ble_err_t esl_get_control_point_esl_id_list(uint8_t group_id, uint8_t *p_esl_cmd_num, uint8_t *p_esl_id)
{
    ble_err_t err_code = BLE_ERR_UNKNOW_TYPE;
    uint8_t i, j;

    do
    {
        i = subevent_data_list.read_index;

        if (subevent_data_list.subevent[i].group_id != group_id)
        {
            err_code = BLE_ERR_INVALID_PARAMETER;
            break;
        }

        err_code = BLE_ERR_OK;

        j = 0;
        do
        {
            p_esl_id[j] = subevent_data_list.subevent[i].p_esl_id[j];
            j++;
        } while (j < subevent_data_list.subevent[i].esl_id_num);

        *p_esl_cmd_num = subevent_data_list.subevent[i].esl_id_num;

    } while (0);

    return err_code;
}

/**
 * @brief Sends a single L2CAP CoC data packet, consuming one flow-control credit.
 *
 * Unlike esl_update_l2cap_data()/svcs_esl_send_l2cap_data_handler() (which fragment a large
 * buffer across multiple credits as they become available), this sends p_l2cap_data as a single
 * packet no larger than the negotiated MTU: it takes one credit from semaphore_l2cap_credit
 * before sending, and gives it back again if any step after that fails.
 *
 * @param p_l2cap_data Pointer to the data to send.
 * @param l2cap_data_len Length in bytes of the data to send; must not exceed the negotiated
 *                        L2CAP MTU (g_l2cap_data.mtu_size).
 * @return ble_err_t Result of the operation.
 * @retval BLE_ERR_OK Data sent successfully.
 * @retval BLE_ERR_INVALID_STATE The link is not connected, or the L2CAP channel has not been
 *         established yet (semaphore_l2cap_credit has not been created).
 * @retval BLE_ERR_SEMAPHORE_MALLOC_FAIL No L2CAP flow-control credit is currently available.
 * @retval BLE_ERR_INVALID_PARAMETER l2cap_data_len exceeds the negotiated MTU size.
 * @retval BLE_ERR_ALLOC_MEMORY_FAIL Failed to allocate the TX packet buffer.
 * @retval (other) ble_err_t code returned by ble_cmd_l2cap_data_send() on failure.
 */
ble_err_t esl_send_l2cap_data(uint8_t *p_l2cap_data, uint16_t l2cap_data_len)
{
    ble_err_t status = BLE_ERR_UNKNOW_TYPE;
    ble_l2cap_data_send_t *p_l2cap_data_send;
    uint8_t tx_len;

    do
    {
        if (ble_app_link_info[APP_ESL_AP_C_HOST_ID].state != STATE_CONNECTED)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        if (semaphore_l2cap_credit == NULL)
        {
            status = BLE_ERR_INVALID_STATE;
            break;            
        }

        if (xSemaphoreTake(semaphore_l2cap_credit, 0) == pdFALSE)
        {
            status = BLE_ERR_SEMAPHORE_MALLOC_FAIL;
            break;
        }

        if (l2cap_data_len > g_l2cap_data.mtu_size)
        {
            status = BLE_ERR_INVALID_PARAMETER;
            xSemaphoreGive(semaphore_l2cap_credit);
            break;
        }

        p_l2cap_data_send = pvPortMalloc(l2cap_data_len + sizeof(ble_l2cap_data_send_t));
        if (p_l2cap_data_send == NULL)
        {
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
            xSemaphoreGive(semaphore_l2cap_credit);
            break;
        }
        p_l2cap_data_send->host_id = APP_ESL_AP_C_HOST_ID;
        p_l2cap_data_send->dest_id = g_l2cap_data.dest_id;   
        p_l2cap_data_send->length = l2cap_data_len;
        memcpy(p_l2cap_data_send->data, p_l2cap_data, l2cap_data_len);
  
        status = ble_cmd_l2cap_data_send(p_l2cap_data_send);
        //printf("g_l2cap_data.total_tx_len %d curr_tx_len 0x%04x, status %d tx_len %d \n", g_l2cap_data.total_tx_len, g_l2cap_data.curr_tx_len, status, tx_len);
        vPortFree(p_l2cap_data_send);

        if (status != BLE_ERR_OK)
        {
            xSemaphoreGive(semaphore_l2cap_credit);
            break;
        }

    } while (0);

    return status;
}

/**
 * @brief Application entry point.
 *
 * Initializes hardware, configures peripherals, and starts the main application task.
 *
 * @return int Not used.
 */
int main(void)
{
    pin_mux_init();
    uart_stdio_init();
    vHeapRegionsInt();
    _dump_boot_info();

    if (xTaskCreate(app_main_entry, (char*)"main",
                    CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE, NULL,
                    E_TASK_PRIORITY_APP, NULL)
        != pdPASS) {
        puts("Task create fail....\r\n");
    }
    puts("[OS] Starting OS Scheduler...\r\n");
    puts("\r\n");
    vTaskStartScheduler();
    while (1) {
    }

    return 0;
}
