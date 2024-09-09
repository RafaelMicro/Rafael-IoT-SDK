/**
 * @file cpc_subg_cmd.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-30
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

#include "util_string.h"
//=============================================================================
//                  Constant Definition
//=============================================================================

//=============================================================================
//                  Macro Definition
//=============================================================================
#define CPC_SUBG_CMD_NOTIFY_ISR(ebit)                 (g_cpc_cmd_evt_var |= ebit); __cpc_cmd_signal()
#define CPC_SUBG_CMD_NOTIFY(ebit)                     enter_critical_section(); g_cpc_cmd_evt_var |= ebit; leave_critical_section(); __cpc_cmd_signal()
#define CPC_SUBG_CMD_GET_NOTIFY(ebit)                 enter_critical_section(); ebit = g_cpc_cmd_evt_var; g_cpc_cmd_evt_var = CPC_SUBG_CMD_EVENT_NONE; leave_critical_section()
//=============================================================================
//                  Structure Definition
//=============================================================================
typedef enum 
{
    CMD_THREAD_NETWORK_START_REQUEST                         = 0x00000000,
    CMD_THREAD_UDP_CONFIG_SET_REQUEST,
    CMD_THREAD_FINISH,    
} thread_cmd_t;

typedef enum  
{
    CPC_SUBG_CMD_EVENT_NONE                       = 0,

    CPC_SUBG_CMD_EVENT_CPC_READ                   = 0x00000001,
    CPC_SUBG_CMD_EVENT_CPC_WRITE_DONE             = 0x00000002,
    CPC_SUBG_CMD_EVENT_ATCMD_OUT                  = 0x00000004,
    CPC_SUBG_CMD_EVENT_CPC_ERROR                  = 0x00000008,

    CPC_SUBG_CMD_EVENT_ALL                        = 0xffffffff,
} cpc_cmd_event_t;

typedef struct 
{
    uint16_t dlen;
    uint8_t pdata[];
} _cpc_at_data_t ;


//=============================================================================
//                  Global Data Definition
//=============================================================================
static cpc_cmd_event_t g_cpc_cmd_evt_var;
static cpc_endpoint_handle_t cmd_endpoint_handle;
static TaskHandle_t cmd_cpc_taskHandle;
static QueueHandle_t cpc_at_handle;

//=============================================================================
//                  Private Function Definition
//=============================================================================
static void __cpc_cmd_signal(void)
{
    if (xPortIsInsideInterrupt())
    {
        BaseType_t pxHigherPriorityTaskWoken = pdTRUE;
        vTaskNotifyGiveFromISR( cmd_cpc_taskHandle, &pxHigherPriorityTaskWoken);
    }
    else
    {
        xTaskNotifyGive(cmd_cpc_taskHandle);
    }
}

static void cmd_cpc_tx_callback(cpc_user_endpoint_id_t endpoint_id, void *buffer, void *arg, status_t status)
{
    (void)endpoint_id;
    (void)buffer;
    if(arg)
        vPortFree(arg);
    CPC_SUBG_CMD_NOTIFY(CPC_SUBG_CMD_EVENT_CPC_WRITE_DONE);
}

static void cmd_cpc_rx_callback(uint8_t endpoint_id, void *arg)
{
    (void)endpoint_id;
    (void)arg;

    CPC_SUBG_CMD_NOTIFY(CPC_SUBG_CMD_EVENT_CPC_READ);
}

static void cmd_cpc_error_callback(uint8_t endpoint_id, void *arg)
{
    (void)endpoint_id;
    (void)arg;

    CPC_SUBG_CMD_NOTIFY(CPC_SUBG_CMD_EVENT_CPC_ERROR);
}



static void cmd_proc(uint8_t *pBuf, uint32_t len)
{
    if(pBuf[0] == 'A' && pBuf[1] == 'T' && pBuf[2] == '+')
    {
        at_cmd_proc(pBuf, len);
    }
}

static void __cpc_cmd_ep_proc(cpc_cmd_event_t evt)
{
    uint8_t ep_state;
    uint32_t rval = 0;
    uint8_t *read_buf;
    uint16_t len;

    _cpc_at_data_t *cpc_data = NULL;
    uint32_t __tx_done = 1;
    uint8_t *ptr = NULL;
    static uint8_t tmp_buffer[512];
    static uint16_t idx = 0;

    if ((CPC_SUBG_CMD_EVENT_CPC_WRITE_DONE & evt)) 
    {
         __tx_done = 1;
    }    

    if(CPC_SUBG_CMD_EVENT_CPC_ERROR & evt)
    {
        ep_state = cpc_get_endpoint_state(&cmd_endpoint_handle);

        log_error("cpc cmd ep error %d", ep_state);

        if(ep_state == CPC_STATE_ERROR_DESTINATION_UNREACHABLE)
            cpc_system_reset(0);

        cpc_close_endpoint(&cmd_endpoint_handle);
    }

    if ((CPC_SUBG_CMD_EVENT_CPC_READ & evt)) 
    {
        rval = cpc_read(&cmd_endpoint_handle, (void **) &read_buf, &len, 0, 1);

        if(rval)
        {
            log_error("cpc read error %04lX", rval);
        }
        else
        {
            cmd_proc(read_buf, len);
            cpc_free_rx_buffer(read_buf);
        }
    }

    if((CPC_SUBG_CMD_EVENT_ATCMD_OUT & evt))
    {
        if(idx < 250)
        {
            while(xQueueReceive(cpc_at_handle, (void*)&cpc_data, 0) == pdPASS)
            {
                memcpy(&tmp_buffer[idx], cpc_data->pdata, cpc_data->dlen);
                idx += cpc_data->dlen;
                vPortFree(cpc_data);

                if(idx >= 380)
                {
                    CPC_SUBG_CMD_NOTIFY(CPC_SUBG_CMD_EVENT_ATCMD_OUT);
                    break;
                }
                vTaskDelay(5);
            }
        }
        else
        {
            CPC_SUBG_CMD_NOTIFY(CPC_SUBG_CMD_EVENT_ATCMD_OUT);
        }        
    }
    if((__tx_done == 1) && (idx > 0))
    {
        ptr = pvPortMalloc(idx);

        if(ptr != NULL)
        {
            memcpy(ptr, tmp_buffer, idx);
            rval = cpc_write(&cmd_endpoint_handle, ptr, idx, 0, (void *)ptr);

            if(rval != 0)
            {
                vPortFree(ptr);
                ptr = NULL;
            }
            else
            {
                log_info_hexdump("ATCMDO", ptr, idx);
                __tx_done = 0;
            }
        }

        idx = 0;
    }    
}


static void cmd_cpc_task(void *parameters_ptr)
{
    cpc_cmd_event_t sevent = CPC_SUBG_CMD_EVENT_NONE;
    for (;;)
    {
        CPC_SUBG_CMD_GET_NOTIFY(sevent);
        __cpc_cmd_ep_proc(sevent);

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }
}

void cp_sub_cmd_send(uint8_t *pdata, uint16_t len)
{
    _cpc_at_data_t *cpc_data = NULL;

    cpc_data = pvPortMalloc(sizeof(_cpc_at_data_t) + len);

    do
    {
        if(cpc_data == NULL)
            break;

        memcpy(cpc_data->pdata, pdata, len);
        cpc_data->dlen = len;
        if(xQueueSend(cpc_at_handle, (void *) &cpc_data, 0) != pdPASS)
        {
            log_error("q full!");
            vPortFree(cpc_data);
        }
        else
        {
            CPC_SUBG_CMD_NOTIFY(CPC_SUBG_CMD_EVENT_ATCMD_OUT);
        }
    } while (0);    
}


void cpc_subg_cmd_init(void)
{

    cpc_open_user_endpoint(&cmd_endpoint_handle, CPC_ENDPOINT_USER_ID_1, 0, 1);

    cpc_set_endpoint_option(&cmd_endpoint_handle, CPC_ENDPOINT_ON_IFRAME_WRITE_COMPLETED, (void *)cmd_cpc_tx_callback);

    cpc_set_endpoint_option(&cmd_endpoint_handle, CPC_ENDPOINT_ON_IFRAME_RECEIVE, (void *)cmd_cpc_rx_callback);

    cpc_set_endpoint_option(&cmd_endpoint_handle, CPC_ENDPOINT_ON_ERROR, (void*)cmd_cpc_error_callback);


    uint8_t state = cpc_get_endpoint_state(&cmd_endpoint_handle);

    log_info("subg thread ep state %X", state);

    xTaskCreate(cmd_cpc_task, "SUB_CMD", 512, NULL, 15, &cmd_cpc_taskHandle);


    cpc_at_handle = xQueueCreate(16, sizeof(_cpc_at_data_t *));
}