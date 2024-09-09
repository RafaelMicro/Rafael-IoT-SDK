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

#include <stdlib.h>
#include <stdint.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#include <log.h>

#include <cpc.h>
#include <cpc_api.h>

#include "main.h"
#include "fota_define.h"
//=============================================================================
//                  Constant Definition
//=============================================================================

//=============================================================================
//                  Macro Definition
//=============================================================================
#define CPC_UPG_NOTIFY_ISR(ebit)                 (g_cpc_upg_evt_var |= ebit); __cpc_upg_signal()
#define CPC_UPG_NOTIFY(ebit)                     enter_critical_section(); g_cpc_upg_evt_var |= ebit; leave_critical_section(); __cpc_upg_signal()
#define CPC_UPG_GET_NOTIFY(ebit)                 enter_critical_section(); ebit = g_cpc_upg_evt_var; g_cpc_upg_evt_var = CPC_UPG_EVENT_NONE; leave_critical_section()
//=============================================================================
//                  Structure Definition
//=============================================================================
typedef enum  {
    CPC_UPG_EVENT_NONE                       = 0,

    CPC_UPG_EVENT_CPC_READ                   = 0x00000001,
    CPC_UPG_EVENT_CPC_WRITE_DONE             = 0x00000002,
    CPC_UPG_EVENT_CPC_ERROR                  = 0x00000004,

    CPC_UPG_EVENT_ALL                        = 0xffffffff,
} cpc_upg_event_t;

typedef struct __attribute__((packed))
{
    uint8_t header[4];
    uint8_t len;
} gateway_cmd_hdr;
typedef struct __attribute__((packed))
{
    uint32_t command_id;
    uint16_t address;
    uint8_t address_mode;
    uint8_t parameter[];
} gateway_cmd_pd;

typedef struct __attribute__((packed))
{
    uint8_t cs;
} gateway_cmd_end;

typedef struct __attribute__((packed))
{
    uint16_t image_type;
    uint16_t manufacturer_code;
    uint32_t file_version;
    uint32_t image_size;
    uint32_t total_pkt;
    uint32_t cur_pkt;
    uint16_t pkt_len;
    uint8_t pkt[];
} upg_img_info_t;
//=============================================================================
//                  Global Data Definition
//=============================================================================
static cpc_upg_event_t g_cpc_upg_evt_var;
static cpc_endpoint_handle_t upgrade_endpoint_handle;
static TaskHandle_t upgrade_cpc_taskHandle;
static upg_img_info_t gt_img_info;
static uint8_t *gp_ota_imgae_cache = NULL;
static fota_information_t t_bootloader_ota_info = {0};
static uint32_t g_download_complete = 0;
//=============================================================================
//                  Private Function Definition
//=============================================================================
static void __cpc_upg_signal(void)
{
    if (xPortIsInsideInterrupt())
    {
        BaseType_t pxHigherPriorityTaskWoken = pdTRUE;
        vTaskNotifyGiveFromISR( upgrade_cpc_taskHandle, &pxHigherPriorityTaskWoken);
    }
    else
    {
        xTaskNotifyGive(upgrade_cpc_taskHandle);
    }
}

static void upgrade_cpc_tx_callback(cpc_user_endpoint_id_t endpoint_id, void *buffer, void *arg, status_t status)
{
    (void)endpoint_id;
    (void)buffer;
    (void)arg;

    CPC_UPG_NOTIFY(CPC_UPG_EVENT_CPC_WRITE_DONE);
}

static void upgrade_cpc_rx_callback(uint8_t endpoint_id, void *arg)
{
    (void)endpoint_id;
    (void)arg;

    CPC_UPG_NOTIFY(CPC_UPG_EVENT_CPC_READ);
}

static void upgrade_cpc_error_callback(uint8_t endpoint_id, void *arg)
{
    (void)endpoint_id;
    (void)arg;

    CPC_UPG_NOTIFY(CPC_UPG_EVENT_CPC_ERROR);
}

static uint32_t crc32checksum(uint32_t flash_addr, uint32_t data_len)
{
    uint16_t k;
    uint32_t i;
    uint8_t *buf = ((uint8_t *)flash_addr);
    uint32_t chkSum = ~0, len = data_len;

    for (i = 0; i < len; i ++ )
    {
        chkSum ^= *buf++;
        for (k = 0; k < 8; k ++)
        {
            chkSum = chkSum & 1 ? (chkSum >> 1) ^ 0xedb88320 : chkSum >> 1;
        }
    }
    return ~chkSum;
}

static uint8_t upgrade_cmd_checksum_calc(uint8_t *pBuf, uint8_t len)
{
    uint8_t cs = 0;

    for (int i = 0; i < len; i++)
    {
        cs += pBuf[i];
    }
    return (~cs);
}

void upgrade_cmd_send(uint32_t cmd_id, uint16_t addr, uint8_t addr_mode, uint8_t src_endp, uint8_t *pParam, uint32_t len)
{
    uint8_t *gateway_cmd_pkt;
    uint32_t pkt_len;
    uint8_t idx = 0;
    status_t status;

    do
    {
        pkt_len = sizeof(gateway_cmd_hdr) + sizeof(gateway_cmd_pd) + len + sizeof(gateway_cmd_end);

        if (src_endp != 0)
        {
            pkt_len += 1;
        }

        gateway_cmd_pkt = pvPortMalloc(pkt_len);

        if (gateway_cmd_pkt == NULL)
        {
            break;
        }

        ((gateway_cmd_hdr *)(gateway_cmd_pkt))->header[0] = 0xFF;
        ((gateway_cmd_hdr *)(gateway_cmd_pkt))->header[1] = 0xFC;
        ((gateway_cmd_hdr *)(gateway_cmd_pkt))->header[2] = 0xFC;
        ((gateway_cmd_hdr *)(gateway_cmd_pkt))->header[3] = 0xFF;
        ((gateway_cmd_hdr *)(gateway_cmd_pkt))->len = sizeof(gateway_cmd_pd) + len;

        if (src_endp != 0)
        {
            ((gateway_cmd_hdr *)(gateway_cmd_pkt))->len += 1;
        }

        idx += sizeof(gateway_cmd_hdr);

        ((gateway_cmd_pd *)(&gateway_cmd_pkt[idx]))->command_id = cmd_id;
        ((gateway_cmd_pd *)(&gateway_cmd_pkt[idx]))->address = addr;
        ((gateway_cmd_pd *)(&gateway_cmd_pkt[idx]))->address_mode = addr_mode;

        if (src_endp != 0)
        {
            ((gateway_cmd_pd *)(&gateway_cmd_pkt[idx]))->parameter[0] = src_endp;
            memcpy(((gateway_cmd_pd *)(&gateway_cmd_pkt[idx]))->parameter + 1, pParam, len);
        }
        else
        {
            memcpy(((gateway_cmd_pd *)(&gateway_cmd_pkt[idx]))->parameter, pParam, len);
        }

        idx += sizeof(gateway_cmd_pd) + len;

        if (src_endp != 0)
        {
            idx += 1;
        }
        ((gateway_cmd_end *)(&gateway_cmd_pkt[idx]))->cs = upgrade_cmd_checksum_calc((uint8_t *) & (((gateway_cmd_hdr *)(gateway_cmd_pkt))->len),
                ((gateway_cmd_hdr *)(gateway_cmd_pkt))->len + 1);

        status = cpc_write(&upgrade_endpoint_handle, gateway_cmd_pkt, pkt_len, 0, NULL);

        if(status != STATUS_OK)
        {
            log_error("UPG Tx fail (%X)!\n", status);
        }
        else
        {
            //ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
            log_debug("------------------------      UPG >>>> ------------------------");
            log_debug_hexdump("UTX", gateway_cmd_pkt, pkt_len);        
        }
        vPortFree(gateway_cmd_pkt);

    } while (0);

}

static void upgrade_cmd_handle(uint32_t cmd_id, uint8_t *pBuf)
{
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

    int i;
    uint32_t status = 0;
    static uint8_t *p_tmp_buf;
    static uint32_t recv_cnt = 0;
    static uint32_t flash_addr = FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS, tmp_len = 0;
    upg_img_info_t *upg_data;

    // erase
    if (cmd_id == 0xF0000000)
    {
        enter_critical_section();
        while (flash_check_busy());
        flash_erase(FLASH_ERASE_SECTOR, FOTA_UPDATE_BANK_INFO_ADDRESS);
        while (flash_check_busy());
        leave_critical_section();

        portYIELD();

        for (i = 0; i < 0x74; i++)
        {
            log_info("erase addr %08X", FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS + (0x1000 * i));
            // // Page erase (4096 bytes)
            enter_critical_section();
            while (flash_check_busy());
            flash_erase(FLASH_ERASE_SECTOR, FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS + (0x1000 * i));
            while (flash_check_busy());
            leave_critical_section();
            vTaskDelay(2);
            //portYIELD();
        }
        upgrade_cmd_send(0xF0008000, 0, 0, 0, (uint8_t *)&status, 4);

        flash_addr = FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS;
        recv_cnt = 0;
        tmp_len = 0;
    }
    else if (cmd_id == 0xF0000001)
    {
        upg_data = (upg_img_info_t *)pBuf;
        if (upg_data->cur_pkt == 0)
        {
            memcpy((uint8_t *)&gt_img_info, pBuf, sizeof(gt_img_info));

            log_info("File Type: 0x%X", gt_img_info.image_type);
            log_info("Manufacturer Code: 0x%X", gt_img_info.manufacturer_code);
            log_info("File Version: 0x%X", gt_img_info.file_version);
            log_info("File Size: 0x%X", gt_img_info.image_size);
        }

        if (!gp_ota_imgae_cache)
        {
            gp_ota_imgae_cache = pvPortMalloc(0x1000);
        }

        if (gp_ota_imgae_cache)
        {
            if (upg_data->pkt_len + recv_cnt >= 0x1000)
            {
                memcpy(&gp_ota_imgae_cache[recv_cnt], upg_data->pkt, 0x1000 - recv_cnt);
                tmp_len = upg_data->pkt_len - (0x1000 - recv_cnt);

                if(tmp_len!=0)
                {
                    p_tmp_buf = pvPortMalloc(tmp_len);
                    memcpy(p_tmp_buf, &upg_data->pkt[0x1000 - recv_cnt], tmp_len);
                }
                else
                {
                    p_tmp_buf = NULL;
                }

                log_info("write addr %08X", flash_addr);
                for (i = 0; i < 0x10; i++)
                {
                    vTaskSuspendAll();
                    while (flash_check_busy());
                    flash_write_page((uint32_t) & ((uint8_t *)gp_ota_imgae_cache)[i * 0x100], flash_addr);
                    while (flash_check_busy());
                    xTaskResumeAll();
                    portYIELD();
                    flash_addr += 0x100;
                }
                recv_cnt = 0;

                if(tmp_len!=0)
                {
                    memcpy(&gp_ota_imgae_cache[recv_cnt], p_tmp_buf, tmp_len);
                    recv_cnt += tmp_len;
                }

                if (p_tmp_buf)
                {
                    vPortFree(p_tmp_buf);
                }
            }
            else
            {
                memcpy(&gp_ota_imgae_cache[recv_cnt], upg_data->pkt, upg_data->pkt_len);
                recv_cnt += upg_data->pkt_len;
            }
            if (upg_data->cur_pkt == (gt_img_info.total_pkt - 1))
            {

                log_info("erase addr %08X", flash_addr);
                enter_critical_section();
                while (flash_check_busy());
                flash_erase(FLASH_ERASE_SECTOR, flash_addr);
                while (flash_check_busy());
                leave_critical_section();
                portYIELD();

                log_info("write addr %08X", flash_addr);

                for (i = 0; i < 0x10; i++)
                {
                    vTaskSuspendAll();
                    while (flash_check_busy());
                    flash_write_page((uint32_t) & ((uint8_t *)gp_ota_imgae_cache)[i * 0x100], flash_addr);
                    while (flash_check_busy());
                    xTaskResumeAll();
                    portYIELD();
                    flash_addr += 0x100;
                }

                if (gp_ota_imgae_cache)
                {
                    vPortFree(gp_ota_imgae_cache);
                    gp_ota_imgae_cache = NULL;
                }
            }
            status = upg_data->cur_pkt;
        }
        else
        {
            status = 0xFFFFFFFF;
        }

        upgrade_cmd_send(0xF0008001, 0, 0, 0, (uint8_t *)&status, 4);
    }
    else if(cmd_id == 0xF0000002)
    {
        flush_cache();
        memset(&t_bootloader_ota_info, 0xFF, sizeof(t_bootloader_ota_info));
        flash_addr = FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS;
        t_bootloader_ota_info.fotabank_ready = FOTA_IMAGE_READY;
        t_bootloader_ota_info.fotabank_startaddr = FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS;
        t_bootloader_ota_info.target_startaddr = APP_START_ADDRESS;
        t_bootloader_ota_info.fotabank_datalen = gt_img_info.image_size;
        t_bootloader_ota_info.fota_result = 0xFF;
        t_bootloader_ota_info.fota_image_info = 0;
        t_bootloader_ota_info.fotabank_crc = crc32checksum(t_bootloader_ota_info.fotabank_startaddr, t_bootloader_ota_info.fotabank_datalen);

        enter_critical_section();

        while (flash_check_busy());
        flash_erase(FLASH_ERASE_SECTOR, FOTA_UPDATE_BANK_INFO_ADDRESS);
        while (flash_check_busy());

        flash_write_page((uint32_t)&t_bootloader_ota_info, FOTA_UPDATE_BANK_INFO_ADDRESS);
        while (flash_check_busy());

        flush_cache();
        leave_critical_section();
        status = 0;

        upgrade_cmd_send(0xF0008002, 0, 0, 0, (uint8_t *)&status, 4);

        g_download_complete = 1;
    }
}

static void upgrade_cmd_proc(uint8_t *pBuf, uint32_t len)
{
    uint32_t cmd_index;

    log_debug("------------------------ >>>> UPG      ------------------------");
    log_debug_hexdump("URX", pBuf, len);

    cmd_index = ((gateway_cmd_pd *)(&pBuf[5]))->command_id;
    gateway_cmd_pd *pt_pd = (gateway_cmd_pd *)&pBuf[5];


    if ((cmd_index >= 0xF0000000) && (cmd_index < 0xF0000003))
    {
        upgrade_cmd_handle(cmd_index, pt_pd->parameter);
    }
}

static void __cpc_upg_ep_proc(cpc_upg_event_t evt)
{
    uint8_t ep_state;
    uint32_t rval = 0;
    uint8_t *read_buf;
    uint16_t len;

    if(CPC_UPG_EVENT_CPC_ERROR & evt)
    {
        ep_state = cpc_get_endpoint_state(&upgrade_endpoint_handle);

        log_error("cpc upg ep error %d", ep_state);

        if(ep_state == CPC_STATE_ERROR_DESTINATION_UNREACHABLE)
            cpc_system_reset(0);

        cpc_close_endpoint(&upgrade_endpoint_handle);
    }

    if ((CPC_UPG_EVENT_CPC_READ & evt)) 
    {
        rval = cpc_read(&upgrade_endpoint_handle, (void **) &read_buf, &len, 0, 1);

        if(rval)
        {
            log_error("cpc read error %04lX", rval);
        }
        else
        {
            upgrade_cmd_proc(read_buf, len);
            cpc_free_rx_buffer(read_buf);
        }
    }
    if ((CPC_UPG_EVENT_CPC_WRITE_DONE & evt)) 
    {        
        if(g_download_complete)
            cpc_system_reset(0);
    }    
}


static void upgrade_cpc_task(void *parameters_ptr)
{
    cpc_upg_event_t sevent = CPC_UPG_EVENT_NONE;
    for (;;)
    {
        CPC_UPG_GET_NOTIFY(sevent);
        __cpc_upg_ep_proc(sevent);

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }
}



void cpc_upgrade_init(void)
{

    cpc_open_user_endpoint(&upgrade_endpoint_handle, CPC_ENDPOINT_USER_ID_0, 0, 1);

    cpc_set_endpoint_option(&upgrade_endpoint_handle, CPC_ENDPOINT_ON_IFRAME_WRITE_COMPLETED, (void *)upgrade_cpc_tx_callback);

    cpc_set_endpoint_option(&upgrade_endpoint_handle, CPC_ENDPOINT_ON_IFRAME_RECEIVE, (void *)upgrade_cpc_rx_callback);

    cpc_set_endpoint_option(&upgrade_endpoint_handle, CPC_ENDPOINT_ON_ERROR, (void*)upgrade_cpc_error_callback);


    uint8_t state = cpc_get_endpoint_state(&upgrade_endpoint_handle);

    log_info("upgrade ep state %X", state);


    enter_critical_section();
    while (flash_check_busy());
    if(flash_erase(FLASH_ERASE_SECTOR, FOTA_UPDATE_BANK_INFO_ADDRESS) != STATUS_SUCCESS)
        log_error("erase fail");
    while (flash_check_busy());
    leave_critical_section(); 

    xTaskCreate(upgrade_cpc_task, "TASK_UPG", 512, NULL, 8, &upgrade_cpc_taskHandle);
}