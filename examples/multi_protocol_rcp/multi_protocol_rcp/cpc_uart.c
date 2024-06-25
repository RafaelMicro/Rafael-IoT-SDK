/**
 * @file cpc_uart.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-02
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
#include <timers.h>
#include <stream_buffer.h>
#include "queue.h"
#include <hosal_uart.h>
#include "log.h"

#include <cpc.h>
#include "cpc_hdlc.h"
#include "cpc_system_common.h"
#include "cpc_crc.h"
#include "cpc_api.h"

#include "shell.h"

#include "hosal_rf.h"
#include "lmac15p4.h"
#include "util_string.h"
//=============================================================================
//                  Constant Definition
//=============================================================================
#ifndef CONFIG_OPERATION_UART_PORT
#define CONFIG_OPERATION_UART_PORT 1
#endif // !CONFIG_OPERATION_UART_PORT

#ifndef CPC_OPERARION_UART_PIN_1
#define CPC_OPERARION_UART_PIN_1 28
#endif // !CONFIG_OPERATION_UART_PORT

#ifndef CPC_OPERARION_UART_PIN_2
#define CPC_OPERARION_UART_PIN_2 29
#endif // !CPC_OPERARION_UART_PIN_1

#ifndef CPC_OPERATION_BAUDRATE
#define CPC_OPERATION_BAUDRATE 2000000
#endif // !CPC_OPERARION_UART_PIN_1

#if (CPC_OPERATION_BAUDRATE == 115200)
HOSAL_UART_DEV_DECL(cpc_uart_dev, CONFIG_OPERATION_UART_PORT, CPC_OPERARION_UART_PIN_1, CPC_OPERARION_UART_PIN_2, UART_BAUDRATE_115200)
#elif (CPC_OPERATION_BAUDRATE == 500000)
HOSAL_UART_DEV_DECL(cpc_uart_dev, CONFIG_OPERATION_UART_PORT, CPC_OPERARION_UART_PIN_1, CPC_OPERARION_UART_PIN_2, UART_BAUDRATE_500000)
#elif (CPC_OPERATION_BAUDRATE == 1000000)
HOSAL_UART_DEV_DECL(cpc_uart_dev, CONFIG_OPERATION_UART_PORT, CPC_OPERARION_UART_PIN_1, CPC_OPERARION_UART_PIN_2, UART_BAUDRATE_1000000)
#elif (CPC_OPERATION_BAUDRATE == 2000000)
HOSAL_UART_DEV_DECL(cpc_uart_dev, CONFIG_OPERATION_UART_PORT, CPC_OPERARION_UART_PIN_1, CPC_OPERARION_UART_PIN_2, UART_BAUDRATE_2000000)
#endif // 0

#define CPC_UART_NOTIFY_ISR(ebit)                 (g_cpc_uart_evt_var |= ebit); __cpc_uart_signal()
#define CPC_UART_NOTIFY(ebit)                     enter_critical_section(); g_cpc_uart_evt_var |= ebit; leave_critical_section(); __cpc_uart_signal()
#define CPC_UART_GET_NOTIFY(ebit)                 enter_critical_section(); ebit = g_cpc_uart_evt_var; g_cpc_uart_evt_var = CPC_UART_EVENT_NONE; leave_critical_section()
//=============================================================================
//                  Macro Definition
//=============================================================================
#define CPC_UART_RAW_BUFFER_SIZE        (CPC_RX_PAYLOAD_MAX_LENGTH+9)
#define CPC_UART_CACHE_BUFFER_SIZE      (CPC_UART_RAW_BUFFER_SIZE*2)
//=============================================================================
//                  Structure Definition
//=============================================================================
typedef struct __attribute__((packed))
{
    uint8_t tag;
    uint8_t ep;
    uint16_t len;
    uint8_t ctrl;
    uint16_t hcs;
    uint8_t data[];
} cpc_uart_pkt_hdr;

typedef struct __attribute__((packed))
{
    uint16_t fcs;
} cpc_uart_pkt_end;

typedef enum  {
    CPC_UART_EVENT_NONE                       = 0,
    CPC_UART_EVENT_TRIGGER                     = 0x00000001,
    CPC_UART_EVENT_UART_IN                    = 0x00000002,
    CPC_UART_EVENT_UART_TIMEOUT               = 0x00000010,

    CPC_UART_EVENT_ALL                        = 0xffffffff,
} cpc_uart_event_t;

typedef enum
{
    CPC_UART_DATA_VALID = 0,
    CPC_UART_DATA_INVALID,
    CPC_UART_DATA_CS_ERROR,
    CPC_UART_DATA_NO_TAG,
} cpc_uart_parser_data_sts_t;

typedef struct 
{
    uint16_t dlen;
    void *pdata;
} _cpc_uart_data_t ;

//=============================================================================
//                  Global Data Definition
//=============================================================================
static int _cli_15p4_tx(int argc, char **argv, cb_shell_out_t log_out, void *pExtra);
static int _cli_2chscan_en(int argc, char **argv, cb_shell_out_t log_out, void *pExtra);
extern void cpc_drv_uart_loop(void);
extern void cpc_drv_trnsmit_complete(void);
TaskHandle_t uart_taskHandle = NULL;
TaskHandle_t cpc_sys_taskHandle = NULL;
static QueueHandle_t cpc_uart_handle;
static uint8_t g_tx_buf[CPC_UART_RAW_BUFFER_SIZE];
static uint8_t g_rx_buf[32];
static cpc_uart_event_t g_cpc_uart_evt_var;

static TimerHandle_t s15p4tx_timer;
static volatile uint32_t g15p4_tx_count = 0, g15p4_tx_delay = 0;
static StreamBufferHandle_t xStreamBuffer;

static volatile uint32_t g_uart_tx_compelete = 1, g_uart_tx_start = 0;

static hosal_uart_dma_cfg_t txdam_cfg = {
    .dma_buf = g_tx_buf,
    .dma_buf_size = sizeof(g_tx_buf),
};

static sh_cmd_t  g_cli_cmd_15p4_tx =
{
    .pCmd_name      = "15p4tx",
    .cmd_exec       = _cli_15p4_tx,
    .pDescription   = "15p4tx\n"
    "  usage: 15p4tx [freq] [count]\n"
    "    e.g. 15p4tx 2405 10",
};

// static sh_cmd_t  g_cli_cmd_2chscan_en =
// {
//     .pCmd_name      = "2chscan",
//     .cmd_exec       = _cli_2chscan_en,
//     .pDescription   = "2chscan\n"
//     "  usage: 2chscan [ch1] [ch2] [enable] \n"
//     "    e.g. 2chscan 11 ~ 25 11 ~  25 1/0",
// };


//=============================================================================
//                  Private Function Definition
//=============================================================================
// static int _cli_2chscan_en(int argc, char **argv, cb_shell_out_t log_out, void *pExtra)
// {
//     uint8_t ch1, ch2, en;
//     do
//     {
//         if ((argc < 3))
//         {
//             break;
//         }

//         ch1 = utility_strtol(argv[1], 0);
//         ch2 = utility_strtol(argv[2], 0);
//         en = utility_strtol(argv[3], 0);

//         log_info("%s 2 channel scan(%d, %d)", (en? "enable":"disable"), ch1, ch2);

//         lmac15p4_2ch_scan_set(en, ch1-11, ch2-11);
//     } while (0);
//     return 0;
// }
static int _cli_15p4_tx(int argc, char **argv, cb_shell_out_t log_out, void *pExtra)
{
    uint32_t freq ; 
    do
    {
        if ((argc < 2))
        {
            break;
        }

        g15p4_tx_count = utility_strtol(argv[2], 0);

        freq = utility_strtol(argv[1], 0);

        if(freq < 2405 || freq > 2480)
            break;

        hosal_rf_ioctl(HOSAL_RF_IOCTL_FREQUENCY_SET, (void*)freq);
        xTimerStart(s15p4tx_timer, 0);

        printf("\r\n");

        while(g15p4_tx_count);

    } while(0);

    return 0;
}

static void __15p4_tx_cb(TimerHandle_t xTimer)
{
    uint8_t beacon[] = {0x00, 0xc0, 0x00, 0xFF, 0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    uint8_t tx_control = 0;

    if(g15p4_tx_count -- )
    {
        tx_control ^= (1 << 1);
        lmac15p4_tx_data_send(0, beacon, sizeof(beacon), tx_control, 0);
        printf(".");
    }

    if(g15p4_tx_count == 0)
    {
        printf("\r\n");
        xTimerStop(s15p4tx_timer, 0);
    }
}

static void __cpc_uart_signal(void)
{
    if (xPortIsInsideInterrupt())
    {
        BaseType_t pxHigherPriorityTaskWoken = pdTRUE;
        vTaskNotifyGiveFromISR( uart_taskHandle, &pxHigherPriorityTaskWoken);
    }
    else
    {
        xTaskNotifyGive(uart_taskHandle);
    }
}
static void _uart_tx_done_pend_cb(void *pvParameter1, uint32_t ulParameter2)
{
    // vTaskResume(cpc_sys_taskHandle);
    g_uart_tx_compelete = 1;
    // enter_critical_section();

    // leave_critical_section();
    gpio_pin_set(20);
    CPC_UART_NOTIFY(CPC_UART_EVENT_TRIGGER);
}
static int __uart_tx_callback(void *p_arg)
{
    BaseType_t  xHigherPriorityTaskWoken;

    xHigherPriorityTaskWoken = pdFALSE;

    xTimerPendFunctionCallFromISR( _uart_tx_done_pend_cb,
                               NULL,
                               NULL,
                               &xHigherPriorityTaskWoken );
    return 0;
}

static int __reloc __uart_rx_callback(void *p_arg)
{
    int cnt = 0;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    cnt = hosal_uart_receive(p_arg, g_rx_buf, sizeof(g_rx_buf));
    gpio_pin_toggle(21);
    if(cnt > 0)
    {
        xStreamBufferSendFromISR(xStreamBuffer, g_rx_buf, cnt, &xHigherPriorityTaskWoken);
    }
    //CPC_UART_NOTIFY_ISR(CPC_UART_EVENT_TRIGGER);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

    return 0;
}
static void _tx_power_setting_fcc(void)
{
    hosal_rf_modem_t modem_type;
    hosal_rf_tx_power_ch_comp_t tx_pwr_ch_comp_ctrl;
    hosal_rf_tx_power_comp_seg_t tx_pwr_comp_seg_ctrl;    
    // BLE
    modem_type = HOSAL_RF_MODEM_BLE;
    hosal_rf_ioctl(HOSAL_RF_IOCTL_TX_PWR_COMP_SET, (void*) &modem_type);

    tx_pwr_ch_comp_ctrl.tx_power_stage0 = 31;
    tx_pwr_ch_comp_ctrl.tx_power_stage1 = 28;
    tx_pwr_ch_comp_ctrl.tx_power_stage2 = 25;
    tx_pwr_ch_comp_ctrl.tx_power_stage3 = 19;
    tx_pwr_ch_comp_ctrl.modem_type = modem_type;
    hosal_rf_ioctl(HOSAL_RF_IOCTL_TX_PWR_CH_COMP_SET, (void*) &tx_pwr_ch_comp_ctrl);
    
    // 0~segmentA-1, segmentA~segmentB-1, segmentB~segmentC-1, segmentC~39
    // 0=2402MHz ~ 39=2480MHz
    tx_pwr_comp_seg_ctrl.segmentA = 35;
    tx_pwr_comp_seg_ctrl.segmentB = 38;
    tx_pwr_comp_seg_ctrl.segmentC = 39;
    tx_pwr_comp_seg_ctrl.modem_type = modem_type;
    hosal_rf_ioctl(HOSAL_RF_IOCTL_COMP_SEG_SET, (void *) &tx_pwr_comp_seg_ctrl);

    // ZIGBEE
    modem_type = HOSAL_RF_MODEM_2P4G_OQPSK;
    hosal_rf_ioctl(HOSAL_RF_IOCTL_TX_PWR_COMP_SET, (void*) &modem_type);

    tx_pwr_ch_comp_ctrl.tx_power_stage0 = 31;
    tx_pwr_ch_comp_ctrl.tx_power_stage1 = 31;
    tx_pwr_ch_comp_ctrl.tx_power_stage2 = 31;
    tx_pwr_ch_comp_ctrl.tx_power_stage3 = 10;
    tx_pwr_ch_comp_ctrl.modem_type = modem_type;
    hosal_rf_ioctl(HOSAL_RF_IOCTL_TX_PWR_CH_COMP_SET, (void*) &tx_pwr_ch_comp_ctrl);
    
    // 0~segmentA-1, segmentA~segmentB-1, segmentB~segmentC-1, segmentC~39
    // 0=2402MHz ~ 39=2480MHz
    tx_pwr_comp_seg_ctrl.segmentA = 13;
    tx_pwr_comp_seg_ctrl.segmentB = 26;
    tx_pwr_comp_seg_ctrl.segmentC = 34;
    tx_pwr_comp_seg_ctrl.modem_type = modem_type;
    hosal_rf_ioctl(HOSAL_RF_IOCTL_COMP_SEG_SET, (void *) &tx_pwr_comp_seg_ctrl);
}
static void _tx_power_setting_ce(void)
{
    hosal_rf_modem_t modem_type;
    hosal_rf_tx_power_ch_comp_t tx_pwr_ch_comp_ctrl;
    hosal_rf_tx_power_comp_seg_t tx_pwr_comp_seg_ctrl;    
    // BLE
    modem_type = HOSAL_RF_MODEM_BLE;
    hosal_rf_ioctl(HOSAL_RF_IOCTL_TX_PWR_COMP_SET, (void*) &modem_type);

    tx_pwr_ch_comp_ctrl.tx_power_stage0 = 29;
    tx_pwr_ch_comp_ctrl.tx_power_stage1 = 30;
    tx_pwr_ch_comp_ctrl.tx_power_stage2 = 30;
    tx_pwr_ch_comp_ctrl.tx_power_stage3 = 30;
    tx_pwr_ch_comp_ctrl.modem_type = modem_type;
    hosal_rf_ioctl(HOSAL_RF_IOCTL_TX_PWR_CH_COMP_SET, (void*) &tx_pwr_ch_comp_ctrl);
    
    // 0~segmentA-1, segmentA~segmentB-1, segmentB~segmentC-1, segmentC~39
    // 0=2402MHz ~ 39=2480MHz
    tx_pwr_comp_seg_ctrl.segmentA = 1;
    tx_pwr_comp_seg_ctrl.segmentB = 38;
    tx_pwr_comp_seg_ctrl.segmentC = 39;
    tx_pwr_comp_seg_ctrl.modem_type = modem_type;
    hosal_rf_ioctl(HOSAL_RF_IOCTL_COMP_SEG_SET, (void *) &tx_pwr_comp_seg_ctrl);

    // ZIGBEE
    modem_type = HOSAL_RF_MODEM_2P4G_OQPSK;
    hosal_rf_ioctl(HOSAL_RF_IOCTL_TX_PWR_COMP_SET, (void*) &modem_type);

    tx_pwr_ch_comp_ctrl.tx_power_stage0 = 28;
    tx_pwr_ch_comp_ctrl.tx_power_stage1 = 29;
    tx_pwr_ch_comp_ctrl.tx_power_stage2 = 29;
    tx_pwr_ch_comp_ctrl.tx_power_stage3 = 29;
    tx_pwr_ch_comp_ctrl.modem_type = modem_type;
    hosal_rf_ioctl(HOSAL_RF_IOCTL_TX_PWR_CH_COMP_SET, (void*) &tx_pwr_ch_comp_ctrl);
    
    // 0~segmentA-1, segmentA~segmentB-1, segmentB~segmentC-1, segmentC~39
    // 0=2402MHz ~ 39=2480MHz
    tx_pwr_comp_seg_ctrl.segmentA = 8;
    tx_pwr_comp_seg_ctrl.segmentB = 26;
    tx_pwr_comp_seg_ctrl.segmentC = 34;
    tx_pwr_comp_seg_ctrl.modem_type = modem_type;
    hosal_rf_ioctl(HOSAL_RF_IOCTL_COMP_SEG_SET, (void *) &tx_pwr_comp_seg_ctrl);
}

void cpc_set_secondary_rf_cert_band(uint32_t band)
{   
    log_info("cert band %d", band);
    if(band == 1)
        _tx_power_setting_fcc();
    else if (band == 2)
        _tx_power_setting_ce();
}

extern uint32_t cpc_drv_uart_push_header(uint8_t *pHeader);
extern void cpc_drv_uart_push_data(uint8_t *p_data, uint16_t length);

static uint32_t __cpc_hdlc_pkt_parser(uint8_t *pBuf, uint16_t plen)
{
    uint32_t u32_len = 0;
    uint16_t i;
    cpc_uart_pkt_hdr *hdr = NULL;
    cpc_uart_pkt_end *end = NULL;

    /* Check input parameters */
    if (!pBuf || plen < CPC_HDLC_HEADER_RAW_SIZE) 
    {
        goto exit;
    }
    for (i = 0; i < plen; i++) 
    {
        /* Check HDLC header flag */
        if (pBuf[i] == CPC_HDLC_FLAG_VAL) 
        {
            if ((plen - i) < CPC_HDLC_HEADER_RAW_SIZE) 
            {
                u32_len = i;
                goto exit;
            }

            hdr = (cpc_uart_pkt_hdr *)(pBuf + i);
            if(hdr->len > CPC_RX_PAYLOAD_MAX_LENGTH || hdr->ep > 100)
            {
                //log_warn("epkt");
                continue;
            }

            /* Check header crc */
            if (!cpc_validate_crc_sw((pBuf + i), CPC_HDLC_HEADER_SIZE, hdr->hcs))
            {
                //log_warn("ehcrc");
                continue;
            }

            if ((plen - i) < (CPC_HDLC_HEADER_RAW_SIZE + hdr->len)) 
            {
                /* Wait HDLC data */
                u32_len = i;
                goto exit;
            }
            if(hdr->len == 0)
            {
                /* Notfiy cpc header with no data */
                vTaskSuspend(cpc_sys_taskHandle);
                cpc_drv_uart_push_header((uint8_t *)hdr);
                vTaskResume(cpc_sys_taskHandle);
            }
            else
            {
                end = (cpc_uart_pkt_end *)&pBuf[i + CPC_HDLC_HEADER_RAW_SIZE + (hdr->len - 2)];

                /* Check data crc */
                if (!cpc_validate_crc_sw(hdr->data, (hdr->len-2), end->fcs))
                {
                    continue;
                }
                else
                {
                    /* Notfiy cpc header with data */
                    vTaskSuspend(cpc_sys_taskHandle);
                    cpc_drv_uart_push_header((uint8_t *)hdr);
                    cpc_drv_uart_push_data(hdr->data , hdr->len);
                    vTaskResume(cpc_sys_taskHandle);
                }
            }
            u32_len = i + CPC_HDLC_HEADER_RAW_SIZE + hdr->len;
            i +=  CPC_HDLC_HEADER_RAW_SIZE + hdr->len;
        }
        else
        {
            u32_len=i;
        }
    }

    exit:
    return u32_len;
}

static void _uart_proces(uint8_t *packet, uint16_t *packet_length)
{
    _cpc_uart_data_t uart_data;
    cpc_buffer_handle_t *buffer_handle;
    uint16_t offset = 0;
    size_t xReceivedBytes = 0;

    /* TX proc */
    if(g_uart_tx_compelete == 1)
    {
        if(g_uart_tx_start == 1)
        {
            vTaskSuspend(cpc_sys_taskHandle);
            cpc_drv_trnsmit_complete();
            vTaskResume(cpc_sys_taskHandle);        
            g_uart_tx_start = 0;
        }
        if(xQueueReceive(cpc_uart_handle, (void*)&uart_data, 0) == pdPASS)
        {
            g_uart_tx_compelete = 0;
            buffer_handle = (cpc_buffer_handle_t *)uart_data.pdata;
            if (uart_data.dlen > 0)
            {
                txdam_cfg.dma_buf_size = CPC_HDLC_HEADER_RAW_SIZE + uart_data.dlen + 2;

                memcpy(txdam_cfg.dma_buf, buffer_handle->hdlc_header, CPC_HDLC_HEADER_RAW_SIZE);
                memcpy(&txdam_cfg.dma_buf[7], buffer_handle->data, uart_data.dlen);
                memcpy(&txdam_cfg.dma_buf[7 + uart_data.dlen], buffer_handle->fcs, 2);

                log_debug_hexdump("U Tx", txdam_cfg.dma_buf, txdam_cfg.dma_buf_size);
            }
            else
            {
                txdam_cfg.dma_buf_size = CPC_HDLC_HEADER_RAW_SIZE;
                memcpy(txdam_cfg.dma_buf, buffer_handle->hdlc_header, CPC_HDLC_HEADER_RAW_SIZE);
            }
            // vTaskSuspend(cpc_sys_taskHandle);
            hosal_uart_ioctl(&cpc_uart_dev, HOSAL_UART_DMA_TX_START, &txdam_cfg);
            g_uart_tx_start = 1;
            gpio_pin_clear(20);
        }
    }


    /* RX proc */
    xReceivedBytes = xStreamBufferReceive(xStreamBuffer, 
                                          &packet[*packet_length], 
                                          (CPC_UART_CACHE_BUFFER_SIZE - *packet_length), 
                                          1);
                                          

    if(xReceivedBytes > 0)
    {
        // vTaskSuspend(cpc_sys_taskHandle);
        // log_info_hexdump("",  &packet[*packet_length], xReceivedBytes);
        *packet_length += xReceivedBytes;

        /* Sepcial pattern for reset */
        // uint8_t isp_pattern[] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA };
        // if(*packet_length >= 7)
        // {
        //     if(memcmp(packet, isp_pattern, 7) == 0)
        //     {
        //         cpc_system_reset(NULL);
        //     }
        // }
        offset = __cpc_hdlc_pkt_parser(packet, *packet_length);

        if(offset > 0 )
        {
            if(*packet_length > offset)
            {
                memmove(packet, (packet + offset), (*packet_length - offset));
            }
            else
            {
                offset = *packet_length;
            }
            *packet_length -= offset;
        }
        // vTaskResume(cpc_sys_taskHandle);
    }
}

static void _uart_task(void *parameters_ptr)
{
    cpc_uart_event_t sevent = CPC_UART_EVENT_NONE;
    uint16_t uart_packet_length = 0;
    uint8_t uart_packet[CPC_UART_CACHE_BUFFER_SIZE];

    /* Configure UART Rx interrupt callback function */
    hosal_uart_callback_set(&cpc_uart_dev, HOSAL_UART_RX_CALLBACK, __uart_rx_callback, &cpc_uart_dev);
    hosal_uart_callback_set(&cpc_uart_dev, HOSAL_UART_TX_DMA_CALLBACK, __uart_tx_callback, &cpc_uart_dev);

    /* Configure UART to interrupt mode */
    hosal_uart_ioctl(&cpc_uart_dev, HOSAL_UART_MODE_SET, (void *)HOSAL_UART_MODE_INT_RX);

    NVIC_SetPriority(Uart1_IRQn, 2);
    NVIC_SetPriority(Uart0_IRQn, 2);

    for(;;)
    {
        CPC_UART_GET_NOTIFY(sevent);

        _uart_proces(uart_packet, &uart_packet_length);
        // ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        ulTaskNotifyTake(pdTRUE, 1);
    }
}

static void __cpc_sys_task(void *parameters_ptr)
{

    if(cpc_init() != 0)
    {
        log_error("cpc init fail!");
    }
    else
    {
        log_info("cpc_init success");
    }

	for(;;)
    {
        cpc_process_action();
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }    
}
const char *cpc_secondary_app_version(void)
{
    return RAFAEL_SDK_VER;
    // return "3.6.1";
}
int cpc_uart_data_send(cpc_buffer_handle_t *buffer_handle, uint16_t payload_tx_len)
{
    _cpc_uart_data_t uart_data;
    uint8_t *pdata = (uint8_t *)buffer_handle->data;

    uart_data.pdata = buffer_handle;
    uart_data.dlen = payload_tx_len;

    if(payload_tx_len > CPC_RX_PAYLOAD_MAX_LENGTH)
    {
        /* Fake uart tx down*/
        enter_critical_section();
        cpc_drv_trnsmit_complete();
        leave_critical_section();

        return;
    }

    if(xQueueSend(cpc_uart_handle, (void *)&uart_data, portMAX_DELAY) != pdPASS)
    {
        /* Fake uart tx down*/
        enter_critical_section();
        cpc_drv_trnsmit_complete();
        leave_critical_section();        
    }

    return 0;
}

void cpc_sys_signal(void)
{
    if (xPortIsInsideInterrupt())
    {
        BaseType_t pxHigherPriorityTaskWoken = pdTRUE;
        vTaskNotifyGiveFromISR( cpc_sys_taskHandle, &pxHigherPriorityTaskWoken);
    }
    else
    {
        xTaskNotifyGive(cpc_sys_taskHandle);
    }
}
void cpc_uart_init(void)
{
    BaseType_t xReturned;

    /*Init UART In the first place*/
    hosal_uart_init(&cpc_uart_dev);

    shell_register_cmd(&g_cli_cmd_15p4_tx);
    // shell_register_cmd(&g_cli_cmd_2chscan_en);

    s15p4tx_timer = xTimerCreate("15p4_T", 10, true, NULL, __15p4_tx_cb);

    gpio_cfg_output(20);
    gpio_cfg_output(21);

    xStreamBuffer = xStreamBufferCreate(CPC_UART_RAW_BUFFER_SIZE, 1);

    cpc_uart_handle = xQueueCreate(16, sizeof(_cpc_uart_data_t));

    xReturned = xTaskCreate(_uart_task, 
                            "cpc_uart", 512, NULL, 
                            (configMAX_PRIORITIES - 2),
                            &uart_taskHandle);
    if( xReturned != pdPASS )
    {
        log_error("task create fail\n");
    }

    xReturned = xTaskCreate(__cpc_sys_task, "cpc_sys", 512, NULL, configMAX_PRIORITIES - 5, &cpc_sys_taskHandle);
    if( xReturned != pdPASS )
    {
        log_error("task create fail\n");
    }
}

int cpc_drv_get_bus_speed(void)
{
    int baud = 0;

    if(cpc_uart_dev.config.baud_rate == UART_BAUDRATE_500000)
        baud = 500000;

    else if(cpc_uart_dev.config.baud_rate == UART_BAUDRATE_115200)
        baud = 115200;

    else if(cpc_uart_dev.config.baud_rate == UART_BAUDRATE_1000000)
        baud = 1000000;    

    else if(cpc_uart_dev.config.baud_rate == UART_BAUDRATE_2000000)
        baud = 2000000;        

    return baud;
}