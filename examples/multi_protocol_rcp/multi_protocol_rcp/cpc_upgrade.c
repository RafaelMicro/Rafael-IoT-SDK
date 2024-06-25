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
#include <stream_buffer.h>
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

//============================================================================
//                  Global Data Definition
//=============================================================================
static cpc_upg_event_t g_cpc_upg_evt_var;
static cpc_endpoint_handle_t upgrade_endpoint_handle;
static TaskHandle_t upgrade_cpc_taskHandle;
static TaskHandle_t upg_op_flash_taskHandle;
static upg_img_info_t gt_img_info;
static uint8_t *gp_ota_imgae_cache = NULL;
static fota_information_t t_bootloader_ota_info = {0};
static uint32_t g_download_complete = 0;
static StreamBufferHandle_t xStreamBuffer;
static volatile uint32_t g_last_pkt = 0, g_file_idx = 0, g_op_flash = 0;
static uint32_t g_op_flash_addr = FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS;
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
    if(arg)
        vPortFree(arg);

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
    uint32_t idx = 0;
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

        status = cpc_write(&upgrade_endpoint_handle, gateway_cmd_pkt, pkt_len, 0, gateway_cmd_pkt);

        if(status != STATUS_OK)
        {
            log_error("UPG Tx fail (%X)!\n", status);

            vPortFree(gateway_cmd_pkt);
        }
        else
        {
            //ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
            log_debug("------------------------      UPG >>>> ------------------------");
            log_debug_hexdump("UTX", gateway_cmd_pkt, pkt_len);        
        }
    } while (0);

}
#if (CONFIG_CPC_ENABLE_ZIGBEE_NCP == 1) 
static void upgrade_cmd_debug_handle(uint32_t cmd_id, uint8_t *pBuf)
{
    static uint32_t start_address = 0;
    uint8_t *p_tmp_buf = NULL;
    static uint32_t recv_cnt = 0;
    static uint32_t flash_addr  = 0;
    static uint32_t write_size = 0;
    uint32_t i = 0;
    uint32_t status = 0;
    
    if(cmd_id == 0xE0000000)
    {

        flash_addr = pBuf[0] | (pBuf[1] << 8)| (pBuf[2] << 16) |(pBuf[3] << 24);
        write_size = pBuf[4] | (pBuf[5] << 8)| (pBuf[6] << 16) |(pBuf[7] << 24);
        start_address = flash_addr;

        log_info("flash_addr %08x, size %x", flash_addr, write_size);
        log_info("15p4 MAC Address : %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", 
                        pBuf[15], pBuf[14], pBuf[13], pBuf[12],
                        pBuf[11], pBuf[10], pBuf[9], pBuf[8]);

        p_tmp_buf = pvPortMalloc(0x100);

        if(p_tmp_buf)
        {
            flash_read_sec_register((uint32_t)p_tmp_buf, 0x1100);
            enter_critical_section();
            while (flash_check_busy());
            flash_erase(FLASH_ERASE_SECURE, 0x1100);
            while (flash_check_busy());
            leave_critical_section();        

            memcpy(p_tmp_buf + 8, &pBuf[8], 8);
            flash_write_sec_register((uint32_t)p_tmp_buf, 0x1100);
            vPortFree(p_tmp_buf);
        }

        upgrade_cmd_send(0xE0008000, 0, 0, 0, (uint8_t *)&status, 8);
    }
    // Write
    else if (cmd_id == 0xE0000001)
    {
        if (!gp_ota_imgae_cache)
        {
            gp_ota_imgae_cache = pvPortMalloc(0x1000);
        }

        if (gp_ota_imgae_cache)
        {
            if (0x100 + recv_cnt >= 0x1000)
            {
                memcpy(&gp_ota_imgae_cache[recv_cnt], &pBuf[0], 0x1000 - recv_cnt);

                log_info("erase addr %08X", flash_addr);
                // // Page erase (4096 bytes)
                enter_critical_section();
                while (flash_check_busy());
                flash_erase(FLASH_ERASE_SECTOR, flash_addr);
                while (flash_check_busy());
                leave_critical_section();
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
            }
            else
            {
                memcpy(&gp_ota_imgae_cache[recv_cnt], &pBuf[0], 0x100);
                recv_cnt += 0x100;
            }
        }

        if(flash_addr >= (start_address + write_size))
        {
            if (gp_ota_imgae_cache)
            {
                vPortFree(gp_ota_imgae_cache);
                gp_ota_imgae_cache = NULL;
            }
        }
        status = 0;
        upgrade_cmd_send(0xE0008001, 0, 0, 0, (uint8_t *)&status, 4);
    }
    // Page Read
    else if (cmd_id == 0xE0000002)
    {
        start_address = pBuf[0] | (pBuf[1] << 8)| (pBuf[2] << 16) |(pBuf[3] << 24);

        log_info("Page Read %08X", start_address);

        p_tmp_buf = pvPortMalloc(0x100);

        if(p_tmp_buf)
        {
            memcpy(p_tmp_buf, start_address, 0x100);
            upgrade_cmd_send(0xE0008002, 0, 0, 0, p_tmp_buf, 0x100);
            vPortFree(p_tmp_buf);
        }
    }
    else if(cmd_id == 0xE0000003)
    {
        p_tmp_buf = pvPortMalloc(8);

        if(p_tmp_buf)
        {
            zigbee_app_read_otp_mac_addr(p_tmp_buf);
            if((p_tmp_buf[0] == 0xFF) && (p_tmp_buf[1] == 0xFF) && (p_tmp_buf[2] == 0xFF) && (p_tmp_buf[3] == 0xFF) && 
            (p_tmp_buf[4] == 0xFF) && (p_tmp_buf[5] == 0xFF) && (p_tmp_buf[6] == 0xFF) && (p_tmp_buf[7] == 0xFF))
            {
                flash_get_unique_id((uint32_t)p_tmp_buf, 8);
            }        
            upgrade_cmd_send(0xE0008003, 0, 0, 0, (uint8_t *)p_tmp_buf, 8);
            vPortFree(p_tmp_buf);
        }
    }    
}
#endif

static void upg_op_flash_task(void *parameters_ptr)
{

    size_t xReceivedBytes = 0;
    uint32_t i;
    for (;;)
    {
        xReceivedBytes = xStreamBufferReceive(xStreamBuffer, 
                                            &gp_ota_imgae_cache[g_file_idx], 
                                            (0x1000 - g_file_idx), 
                                            10);
        if(xReceivedBytes || g_file_idx == 0x1000)
        {
            g_file_idx += xReceivedBytes;
            if((g_last_pkt == 1 || g_file_idx == 0x1000))
            {
                log_info("erase addr %08X", g_op_flash_addr);

                enter_critical_section();
                while (flash_check_busy());
                flash_erase(FLASH_ERASE_SECTOR, g_op_flash_addr);
                while (flash_check_busy());
                leave_critical_section();

                log_info("write addr %08X", g_op_flash_addr);

                enter_critical_section();
                for (i = 0; i < 0x10; i++)
                {
                    while (flash_check_busy());
                    flash_write_page((uint32_t) & ((uint8_t *)gp_ota_imgae_cache)[i * 0x100], g_op_flash_addr);
                    while (flash_check_busy());
                    g_op_flash_addr += 0x100;
                }
                leave_critical_section();
                upgrade_cmd_send(0xF0008001, 0, 0, 0, (uint8_t *)&g_op_flash, 4);
                g_op_flash = 0;
                g_file_idx = 0;
            }
        }
    }
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
    uint32_t xWriteBytes = 0;

    // erase
    if (cmd_id == 0xF0000000)
    {
        if(xStreamBuffer != NULL)
        {
            vStreamBufferDelete(xStreamBuffer);
            xStreamBuffer = NULL;
        }
        if(upg_op_flash_taskHandle == NULL)
        {
            xTaskCreate(upg_op_flash_task, "op_flash", 128, NULL, 20, &upg_op_flash_taskHandle);
        }

        if (!gp_ota_imgae_cache)
        {
            gp_ota_imgae_cache = pvPortMalloc(0x1000);
        }

        xStreamBuffer = xStreamBufferCreate(CPC_RX_PAYLOAD_MAX_LENGTH * 10, 1);

        g_file_idx = 0;
        g_op_flash_addr = FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS;

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
        xWriteBytes = xStreamBufferSend(xStreamBuffer, upg_data->pkt, upg_data->pkt_len, 0);
        xWriteBytes = upg_data->pkt_len;
        if(xWriteBytes == upg_data->pkt_len)
        {
            recv_cnt += upg_data->pkt_len;
            status = upg_data->cur_pkt;

            if (upg_data->cur_pkt == (gt_img_info.total_pkt - 1))
            {
                g_last_pkt = 1;
            }

            if(recv_cnt >= 0x1000 || g_last_pkt)
            {
                g_op_flash = upg_data->cur_pkt;
                recv_cnt -= 0x1000;
            }

            if(g_op_flash == 0)
            {
                vTaskDelay(1);
                upgrade_cmd_send(0xF0008001, 0, 0, 0, (uint8_t *)&status, 4);
            }
        }
        else
        {
            log_error("Strem send failed %d ", xWriteBytes);
            status = 0xFFFFFFFF;
            upgrade_cmd_send(0xF0008001, 0, 0, 0, (uint8_t *)&status, 4);
        }
    }
    else if(cmd_id == 0xF0000002)
    {
        //flush_cache();
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
#if (CONFIG_CPC_ENABLE_ZIGBEE_NCP == 1) 
    else if((cmd_index >= 0xE0000000) )
    {
        upgrade_cmd_debug_handle(cmd_index, pt_pd->parameter);
    }
#endif
}

static void __cpc_upg_ep_proc(cpc_upg_event_t evt)
{
    uint8_t ep_state;
    uint32_t rval = 0;
    uint8_t *read_buf;
    uint16_t len;

    if(CPC_UPG_EVENT_CPC_ERROR & evt)
    {
        // ep_state = cpc_get_endpoint_state(&upgrade_endpoint_handle);

        // log_error("cpc upg ep error %d", ep_state);

        // if(ep_state == CPC_STATE_ERROR_DESTINATION_UNREACHABLE)
        //     cpc_system_reset(0);

        cpc_set_state(&upgrade_endpoint_handle, CPC_STATE_OPEN);

        upgrade_endpoint_handle.ref_count = 1u;
    }

    if ((CPC_UPG_EVENT_CPC_READ & evt)) 
    {
        do
        {
            rval = cpc_read(&upgrade_endpoint_handle, (void **) &read_buf, &len, 0, 1);
            if(rval == 0)
            {
                upgrade_cmd_proc(read_buf, len);
                cpc_free_rx_buffer(read_buf);
            }
            /* code */
        } while (rval == 0);

    }
    if ((CPC_UPG_EVENT_CPC_WRITE_DONE & evt)) 
    {      

        if(g_download_complete)
            cpc_system_reset(0);
    } 

    if(!gp_ota_imgae_cache)
        return;
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
    uint8_t state;
    state = cpc_open_user_endpoint(&upgrade_endpoint_handle, 
                            CPC_ENDPOINT_USER_ID_0, 0, 10);

    if(state == 0)
        log_info("Opened Firmware upgrade EP");

    cpc_set_endpoint_option(&upgrade_endpoint_handle, 
                            CPC_ENDPOINT_ON_IFRAME_WRITE_COMPLETED, 
                            (void *)upgrade_cpc_tx_callback);
    cpc_set_endpoint_option(&upgrade_endpoint_handle, 
                            CPC_ENDPOINT_ON_IFRAME_RECEIVE, 
                            (void *)upgrade_cpc_rx_callback);
    cpc_set_endpoint_option(&upgrade_endpoint_handle, 
                            CPC_ENDPOINT_ON_ERROR, 
                            (void*)upgrade_cpc_error_callback);

    state = cpc_get_endpoint_state(&upgrade_endpoint_handle);

    enter_critical_section();
    while (flash_check_busy());
    if(flash_erase(FLASH_ERASE_SECTOR, FOTA_UPDATE_BANK_INFO_ADDRESS) != STATUS_SUCCESS)
        log_error("erase fail");
    while (flash_check_busy());
    leave_critical_section(); 

    xTaskCreate(upgrade_cpc_task, "TASK_UPG", 512, NULL, 20, &upgrade_cpc_taskHandle);    
}