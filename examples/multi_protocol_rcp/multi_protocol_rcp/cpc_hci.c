/**
 * @file cpc_hci.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include <stdlib.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#include <timers.h>

#include <log.h>

#include <cpc.h>
#include <cpc_api.h>

#include <hci_bridge.h>
#include <hosal_rf.h>

#include "main.h"
#include "log.h"
/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/
#define CPC_HCI_NOTIFY_ISR(ebit)                 (g_cpc_hci_evt_var |= ebit); __cpc_hci_signal()
#define CPC_HCI_NOTIFY(ebit)                     enter_critical_section(); g_cpc_hci_evt_var |= ebit; leave_critical_section(); __cpc_hci_signal()
#define CPC_HCI_GET_NOTIFY(ebit)                 enter_critical_section(); ebit = g_cpc_hci_evt_var; g_cpc_hci_evt_var = CPC_HCI_EVENT_NONE; leave_critical_section()
/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/

/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/
typedef enum  {
    CPC_HCI_EVENT_NONE                       = 0,

    CPC_HCI_EVENT_CPC_READ                   = 0x00000002,
    CPC_HCI_EVENT_HCI_IN                     = 0x00000004,
    CPC_HCI_EVENT_CPC_WRITE_DONE             = 0x00000008,
    CPC_HCI_EVENT_CPC_ERROR                  = 0x00000010,

    CPC_HCI_EVENT_ALL                        = 0xffffffff,
} cpc_hci_event_t;

typedef struct 
{
    uint16_t dlen;
    uint8_t pdata[];
} _cpc_hci_data_t ;


typedef struct
{
    uint8_t addr_type;                                 /**<Address type.*/
    uint8_t Reserved;                                  /**<Reserved byte.*/
    uint8_t ble_addr[6];                               /**<BLE unique mac address code.*/
    uint8_t zigbee_addr[8];                            /**<ZigBee unique mac address code.*/
    uint8_t ble_irk[16];                               /**<BLE Identity Resolving Key code.*/
} ble_unique_code_format_t;
/**************************************************************************************************
 *    GLOBAL VARIABLES
 *************************************************************************************************/
static cpc_hci_event_t g_cpc_hci_evt_var;
static TaskHandle_t cpc_hci_taskHandle;
static QueueHandle_t cpc_hci_handle;
static cpc_endpoint_handle_t cpc_hci_ep_handle;
static uint32_t cpc_hci_le_adv_report_event_cnt = 0;
static uint32_t cpc_hci_le_adv_report_sent = 0;
static uint32_t cpc_hci_host_conn = 0;
static uint16_t cpc_hci_scan_interval = 0x20;
static uint16_t cpc_hci_scan_window = 0x10;
static uint32_t g_hci_tx_done = 1;
/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
static void __cpc_hci_signal(void)
{
    if (xPortIsInsideInterrupt())
    {
        BaseType_t pxHigherPriorityTaskWoken = pdTRUE;
        vTaskNotifyGiveFromISR( cpc_hci_taskHandle, &pxHigherPriorityTaskWoken);
    }
    else
    {
        xTaskNotifyGive(cpc_hci_taskHandle);
    }
}

void __cpc_hci_write_done_evt(cpc_user_endpoint_id_t endpoint_id, void *buffer, void *arg, status_t status)
{
    (void)endpoint_id;
    (void)buffer;

    if(arg)
        vPortFree(arg);
    g_hci_tx_done = 1;
    CPC_HCI_NOTIFY(CPC_HCI_EVENT_CPC_WRITE_DONE);
}
void __cpc_hci_read_evt(uint8_t endpoint_id, void *arg)
{
    (void)endpoint_id;
    (void)arg;
    cpc_hci_host_conn = 1;
    CPC_HCI_NOTIFY(CPC_HCI_EVENT_CPC_READ);
}
void __cpc_hci_error_evt(uint8_t endpoint_id, void *arg)
{
    (void)endpoint_id;
    (void)arg;
    CPC_HCI_NOTIFY(CPC_HCI_EVENT_CPC_ERROR);
}

static void __cpc_hci_ep_init(void)
{
    uint32_t  status;
    status = cpc_open_service_endpoint(&cpc_hci_ep_handle, CPC_ENDPOINT_BLUETOOTH_RCP, 0, 10);

    status = cpc_set_endpoint_option(&cpc_hci_ep_handle, CPC_ENDPOINT_ON_IFRAME_WRITE_COMPLETED, (void *)__cpc_hci_write_done_evt);

    status = cpc_set_endpoint_option(&cpc_hci_ep_handle, CPC_ENDPOINT_ON_IFRAME_RECEIVE, (void *)__cpc_hci_read_evt);

    status = cpc_set_endpoint_option(&cpc_hci_ep_handle, CPC_ENDPOINT_ON_ERROR, (void*)__cpc_hci_error_evt);
}

static void __cpc_hci_ep_proc(cpc_hci_event_t evt)
{
    hosal_rf_pta_ctrl_t pta_ctrl;
    uint32_t rval = 0;
    uint16_t data_len;
    uint8_t *p_cpc_buffer;
    uint8_t ep_state;
    _cpc_hci_data_t *cpc_data = NULL;

    static uint8_t tmp_buffer[CPC_RX_PAYLOAD_MAX_LENGTH];
    static uint16_t idx = 0;
    uint8_t *ptr = NULL;
    uint8_t *p_tmp_buf = NULL;
    uint8_t i;

    if(CPC_HCI_EVENT_CPC_ERROR & evt)
    {
        ep_state = cpc_get_endpoint_state(&cpc_hci_ep_handle);

        log_error("cpc hci ep error %d", ep_state);

        // if(ep_state == CPC_STATE_ERROR_DESTINATION_UNREACHABLE)
        //     cpc_system_reset(0);

        cpc_close_endpoint(&cpc_hci_ep_handle);
        cpc_set_state(&cpc_hci_ep_handle, CPC_STATE_OPEN);

        cpc_hci_ep_handle.ref_count = 1u;
    }


    if ((CPC_HCI_EVENT_CPC_READ & evt)) 
    {
        rval = cpc_read(&cpc_hci_ep_handle, (void **) &p_cpc_buffer, &data_len, 0, 1);

        if(rval)
        {
            log_error("cpc read error %04lX", rval);
        }
        else
        {
            if(p_cpc_buffer[0] == 0x01 && p_cpc_buffer[1] == 0x0c &&
            p_cpc_buffer[2] == 0x20 && p_cpc_buffer[3] == 0x02)
            {
                if(p_cpc_buffer[4] == 0x01)
                {
                    cpc_hci_le_adv_report_event_cnt = 0;
                    cpc_hci_le_adv_report_sent = 0;
                    log_info("Scan Enable !");
                }
                else
                {
                    log_info("Scan Disable scan event: %d %d!", cpc_hci_le_adv_report_event_cnt, cpc_hci_le_adv_report_sent);              
                }
            }

            if(p_cpc_buffer[0] == 0x01 && p_cpc_buffer[1] == 0x0b &&
            p_cpc_buffer[2] == 0x20 && p_cpc_buffer[3] == 0x07)
            {
            
                p_cpc_buffer[5] = (cpc_hci_scan_interval & 0xFF);
                p_cpc_buffer[6] = ((cpc_hci_scan_interval >> 8) & 0xFF);

                p_cpc_buffer[7] = (cpc_hci_scan_window & 0xFF);
                p_cpc_buffer[8] = ((cpc_hci_scan_window >> 8) & 0xFF);                       

        
                log_info("LE Set Scan Parameters %04lX %04lX", cpc_hci_scan_window, cpc_hci_scan_interval);
            }

            if(p_cpc_buffer[0] == 0x01 && p_cpc_buffer[2] == 0xFC)
            {
                log_info_hexdump("VSC", p_cpc_buffer, data_len);

                if(p_cpc_buffer[1] == 0x02) // PTA
                {
                    pta_ctrl.enable = p_cpc_buffer[4];
                    pta_ctrl.inverse = p_cpc_buffer[5];

                    hosal_rf_ioctl(HOSAL_RF_IOCTL_PTA_CTRL_SET, (void*) &pta_ctrl);
                    cpc_data = pvPortMalloc(sizeof(_cpc_hci_data_t) + 7);

                    if(cpc_data)
                    {
                        cpc_data->dlen = 7;
                        cpc_data->pdata[0] = 0x04;
                        cpc_data->pdata[1] = 0x0E;
                        cpc_data->pdata[2] = 0x04;
                        cpc_data->pdata[3] = 0x01;
                        cpc_data->pdata[4] = 0x02;
                        cpc_data->pdata[5] = 0xFC;
                        cpc_data->pdata[6] = 0x00;
                        if(xQueueSend(cpc_hci_handle, (void *) &cpc_data, 0) != pdPASS)
                        {
                            log_error("q full!");
                            vPortFree(cpc_data);
                        }
                        CPC_HCI_NOTIFY(CPC_HCI_EVENT_HCI_IN);  
                    }
                }

                if(p_cpc_buffer[1] == 0x03) //Scan interval/window
                {
                    cpc_data = pvPortMalloc(sizeof(_cpc_hci_data_t) + 7);

                    cpc_hci_scan_interval = p_cpc_buffer[4] | (p_cpc_buffer[5] << 8);  
                    cpc_hci_scan_window = p_cpc_buffer[6] | (p_cpc_buffer[7] << 8);

                    if(cpc_hci_scan_window > cpc_hci_scan_interval)
                    {
                        cpc_hci_scan_window = 0x10;
                        cpc_hci_scan_interval = 0x20;
                    }

                    if(cpc_data)
                    {
                        cpc_data->dlen = 7;
                        cpc_data->pdata[0] = 0x04;
                        cpc_data->pdata[1] = 0x0E;
                        cpc_data->pdata[2] = 0x04;
                        cpc_data->pdata[3] = 0x01;
                        cpc_data->pdata[4] = 0x03;
                        cpc_data->pdata[5] = 0xFC;
                        cpc_data->pdata[6] = 0x00;
                        if(xQueueSend(cpc_hci_handle, (void *) &cpc_data, 0) != pdPASS)
                        {
                            log_error("q full!");
                            vPortFree(cpc_data);
                        }
                        CPC_HCI_NOTIFY(CPC_HCI_EVENT_HCI_IN);
                    }
                }

                if(p_cpc_buffer[1] == 0x04) //Set MAC address
                {
                    cpc_data = pvPortMalloc(sizeof(_cpc_hci_data_t) + 7);

                    log_info("MAC Address : %02X:%02X:%02X:%02X:%02X:%02X", 
                                    p_cpc_buffer[4], p_cpc_buffer[5], 
                                    p_cpc_buffer[6], p_cpc_buffer[7],
                                    p_cpc_buffer[8], p_cpc_buffer[9]);

                    p_tmp_buf = pvPortMalloc(0x100);

                    if(p_tmp_buf)
                    {
                        flash_read_sec_register((uint32_t)p_tmp_buf, 0x1100);
                        enter_critical_section();
                        while (flash_check_busy());
                        flash_erase(FLASH_ERASE_SECURE, 0x1100);
                        while (flash_check_busy());
                        leave_critical_section();        

                        for(i=0;i<6;i++)                        
                            memcpy((p_tmp_buf + 2 + i), &p_cpc_buffer[9-i], 6);
                        for(i=0;i<3;i++)
                        {
                            memcpy((p_tmp_buf + 8 + i), &p_cpc_buffer[9-i], 3);
                            memcpy((p_tmp_buf + 13 + i), &p_cpc_buffer[6-i], 3);
                        }
                        p_tmp_buf[11] = 0xFF;
                        p_tmp_buf[12] = 0xFF;

                        flash_write_sec_register((uint32_t)p_tmp_buf, 0x1100);
                        vPortFree(p_tmp_buf);
                    }

                    if(cpc_hci_scan_window > cpc_hci_scan_interval)
                    {
                        cpc_hci_scan_window = 0x10;
                        cpc_hci_scan_interval = 0x20;
                    }

                    if(cpc_data)
                    {
                        cpc_data->dlen = 7;
                        cpc_data->pdata[0] = 0x04;
                        cpc_data->pdata[1] = 0x0E;
                        cpc_data->pdata[2] = 0x04;
                        cpc_data->pdata[3] = 0x01;
                        cpc_data->pdata[4] = 0x04;
                        cpc_data->pdata[5] = 0xFC;
                        cpc_data->pdata[6] = 0x00;
                        if(xQueueSend(cpc_hci_handle, (void *) &cpc_data, 0) != pdPASS)
                        {
                            log_error("q full!");
                            vPortFree(cpc_data);
                        }
                        CPC_HCI_NOTIFY(CPC_HCI_EVENT_HCI_IN);
                    }
                }                
            }
            else
            {
                hci_bridge_message_write((ble_hci_message_t *)p_cpc_buffer);
                log_debug_hexdump("HCI Rx", p_cpc_buffer, data_len);
            }
            cpc_free_rx_buffer(p_cpc_buffer);
        }
    }

    if(idx < (CPC_RX_PAYLOAD_MAX_LENGTH-120))
    {
        while(xQueueReceive(cpc_hci_handle, (void*)&cpc_data, 0) == pdPASS)
        {
            if(cpc_data->pdata[0] == 0x04 && cpc_data->pdata[1] == 0x3E)
            {
                cpc_hci_le_adv_report_event_cnt ++;
            }

            if(cpc_data->pdata[0] == 0x04 && cpc_data->pdata[5] == 0xFC)
            {
                log_info_hexdump("VSE", cpc_data->pdata, cpc_data->dlen);
            }
            if((cpc_data->dlen + idx) < CPC_RX_PAYLOAD_MAX_LENGTH)
            {
                memcpy(&tmp_buffer[idx], cpc_data->pdata, cpc_data->dlen);
                idx += cpc_data->dlen;
            }
            vPortFree(cpc_data);
        }
    }
    if((g_hci_tx_done == 1) && (idx > 0))
    {
        ptr = pvPortMalloc(idx);

        if(ptr == NULL)
            return;

        memcpy(ptr, tmp_buffer, idx);
        rval = cpc_write(&cpc_hci_ep_handle, ptr, idx, 0, ptr);

        if(rval != 0)
        {
            log_error("cpc hci write error %04lX", rval);
            vPortFree(ptr);
        }
        else
        {
            if(ptr[0] == 0x04 && ptr[1] == 0x3E)
            {
                cpc_hci_le_adv_report_sent++;
            }

            log_debug_hexdump("HCI Tx", ptr, idx);
            g_hci_tx_done = 0;
            idx = 0;
        }
    }
}

static int __hci_evt_cb(uint8_t *p_data, uint16_t data_len)
{    
    _cpc_hci_data_t *cpc_data = NULL;

    if( cpc_hci_host_conn == 0)
    {
        return 0;
    }

    cpc_data = pvPortMalloc(sizeof(_cpc_hci_data_t) + data_len);

    if(cpc_data)
    {
        memcpy(cpc_data->pdata, p_data, data_len);
        cpc_data->dlen = data_len;
        if(xQueueSend(cpc_hci_handle, (void *) &cpc_data, 0) != pdPASS)
        {
            log_error("q full!");
            vPortFree(cpc_data);
        }
        CPC_HCI_NOTIFY(CPC_HCI_EVENT_HCI_IN);  
    }
    else
    {
        log_error("No memory!");
    }
    return 0;
}

static int __hci_data_cb(uint8_t *p_data, uint16_t data_len)
{
    _cpc_hci_data_t *cpc_data = NULL;
    cpc_data = pvPortMalloc(sizeof(_cpc_hci_data_t) + data_len);

    if(cpc_data)
    {
        memcpy(cpc_data->pdata, p_data, data_len);
        cpc_data->dlen = data_len;
        if(xQueueSend(cpc_hci_handle, (void *) &cpc_data, 0) != pdPASS)
        {
            log_error("q full!");
            vPortFree(cpc_data);
        }
    }
    else
    {
        log_error("No memory!");
    }

    CPC_HCI_NOTIFY(CPC_HCI_EVENT_HCI_IN);    
    return 0;
}


static void __cpc_hci_task(void *parameters_ptr)
{
    cpc_hci_event_t sevent = CPC_HCI_EVENT_NONE;

    for (;;)
    {
        CPC_HCI_GET_NOTIFY(sevent);
        __cpc_hci_ep_proc(sevent);

        ulTaskNotifyTake(pdTRUE, 2);
    }
}

// static void scan_100msTimerCallback( TimerHandle_t xTimer ) 
// {
//     log_warn("500 ms no adv report !");
// }

/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/
void cpc_hci_init(void)
{
    BaseType_t xReturned;
    uint8_t   *p_read_param;
    uint8_t   *p_cmp;
    uint8_t   _waddr = 1;
    uint8_t hci_set_addr[] = {0x01, 0x01, 0xFC, 0x09, 0x00, 0xF7, 0x00, 0x00, 0x4C, 0xC1, 0x0B, 0x64, 0x08};

    hci_bridge_init();

    hci_bridge_callback_set(HIC_BRIDGE_CALLBACK_TYPE_EVENT, __hci_evt_cb);
    hci_bridge_callback_set(HIC_BRIDGE_CALLBACK_TYPE_DATA, __hci_data_cb);    

    cpc_hci_handle = xQueueCreate(64, sizeof(_cpc_hci_data_t *));

    xReturned = xTaskCreate(__cpc_hci_task, "HCI_CPC", 512, NULL, configMAX_PRIORITIES - 6, &cpc_hci_taskHandle);
    if( xReturned != pdPASS )
    {
        log_error("HCI_CPC create fail");
    }

    __cpc_hci_ep_init();

    p_read_param = pvPortMalloc(256);

    if (p_read_param != NULL)
    {
        p_cmp = pvPortMalloc(sizeof(ble_unique_code_format_t));

        if (p_cmp != NULL)
        {
            flash_read_sec_register((uint32_t)p_read_param, 0x1100);
            while (flash_check_busy());

            memset(p_cmp, 0, sizeof(ble_unique_code_format_t));
            if (memcmp(p_read_param, p_cmp, sizeof(ble_unique_code_format_t)) == 0)
            {
                _waddr = 0;
            }
            memset(p_cmp, 0xFF, sizeof(ble_unique_code_format_t));
            if (memcmp(p_read_param, p_cmp, sizeof(ble_unique_code_format_t)) == 0)
            {
                _waddr = 0;
            }
            vPortFree(p_cmp);

            if(_waddr)
            {
                for(int i =0;i<6;i++)
                    hci_set_addr[4+i] = p_read_param[2+i];
                hci_bridge_message_write((ble_hci_message_t *)&hci_set_addr);
            }
        }
        vPortFree(p_read_param);     
    }
}