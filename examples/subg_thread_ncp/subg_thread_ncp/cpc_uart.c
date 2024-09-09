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
#include "queue.h"
#include <hosal_uart.h>
#include "log.h"

#include <cpc.h>
#include "cpc_hdlc.h"
#include "cpc_system_common.h"
#include "cpc_crc.h"
//=============================================================================
//                  Constant Definition
//=============================================================================
#ifndef CONFIG_OPERATION_UART_PORT
#define CONFIG_OPERATION_UART_PORT 1
#endif // !CONFIG_OPERATION_UART_PORT
#if (CONFIG_OPERATION_UART_PORT == 1)
#if (CONFIG_UART1_PIN_4_5 == 1)
HOSAL_UART_DEV_DECL(cpc_uart_dev, CONFIG_OPERATION_UART_PORT, 4, 5, UART_BAUDRATE_115200)
#else
HOSAL_UART_DEV_DECL(cpc_uart_dev, CONFIG_OPERATION_UART_PORT, 28, 29, UART_BAUDRATE_500000)
// HOSAL_UART_DEV_DECL(cpc_uart_dev, CONFIG_OPERATION_UART_PORT, 28, 29, UART_BAUDRATE_2000000)
#endif
#else
HOSAL_UART_DEV_DECL(cpc_uart_dev, CONFIG_OPERATION_UART_PORT, 16, 17, UART_BAUDRATE_115200)
//HOSAL_UART_DEV_DECL(cpc_uart_dev, CONFIG_OPERATION_UART_PORT, 16, 17, UART_BAUDRATE_115200)
#endif

#define CPC_UART_NOTIFY_ISR(ebit)                 (g_cpc_uart_evt_var |= ebit); __cpc_uart_signal()
#define CPC_UART_NOTIFY(ebit)                     enter_critical_section(); g_cpc_uart_evt_var |= ebit; leave_critical_section(); __cpc_uart_signal()
#define CPC_UART_GET_NOTIFY(ebit)                 enter_critical_section(); ebit = g_cpc_uart_evt_var; g_cpc_uart_evt_var = CPC_UART_EVENT_NONE; leave_critical_section()
//=============================================================================
//                  Macro Definition
//=============================================================================
#define TX_BUFF_SIZE                    484
#define TX_BUFF_MASK                    (TX_BUFF_SIZE -1)
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

    CPC_UART_EVENT_UART_IN                    = 0x00000002,
    CPC_UART_EVENT_CPC_IN                     = 0x00000004,
    CPC_UART_EVENT_UART_TX_DOWN               = 0x00000008,
    CPC_UART_EVENT_UART_TIMEOUT               = 0x00000010,

    CPC_UART_EVENT_ALL                        = 0xffffffff,
} cpc_uart_event_t;

typedef enum
{
    CPC_UART_DATA_VALID = 0,
    CPC_UART_DATA_INVALID,
    CPC_UART_DATA_CS_ERROR,
} cpc_uart_parser_data_sts_t;

typedef struct uart_io
{
    uint16_t start;
    uint16_t end;

    uint32_t recvLen;
    uint8_t uart_cache[TX_BUFF_SIZE];
} uart_io_t;

typedef struct 
{
    uint16_t dlen;
    uint8_t *pdata;
} _cpc_uart_data_t ;

//=============================================================================
//                  Global Data Definition
//=============================================================================
extern void cpc_drv_uart_loop(void);
extern void cpc_drv_trnsmit_complete(void);
TaskHandle_t uart_taskHandle = NULL;
TaskHandle_t cpc_sys_taskHandle = NULL;
static QueueHandle_t cpc_uart_handle;
static uart_io_t g_uart_rx_io = { .start = 0, .end = 0, };
static uint8_t g_tx_buf[TX_BUFF_SIZE];
static cpc_uart_event_t g_cpc_uart_evt_var;

static hosal_uart_dma_cfg_t txdam_cfg = {
    .dma_buf = g_tx_buf,
    .dma_buf_size = sizeof(g_tx_buf),
};

//=============================================================================
//                  Private Function Definition
//=============================================================================
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

static int __uart_read(uint8_t *p_data)
{
    uint32_t byte_cnt = 0;
    hosal_uart_ioctl(&cpc_uart_dev, HOSAL_UART_DISABLE_INTERRUPT, (void *)NULL);

    while (g_uart_rx_io.start != g_uart_rx_io.end) {
        memcpy(p_data + byte_cnt, g_uart_rx_io.uart_cache + g_uart_rx_io.end, 1);
        byte_cnt++;
        g_uart_rx_io.end = (g_uart_rx_io.end + 1) % TX_BUFF_SIZE;
    }

    g_uart_rx_io.start = g_uart_rx_io.end = 0;
    g_uart_rx_io.recvLen = 0;
    hosal_uart_ioctl(&cpc_uart_dev, HOSAL_UART_ENABLE_INTERRUPT, (void *)NULL);
    return byte_cnt;
}
static int __uart_tx_callback(void *p_arg)
{
    CPC_UART_NOTIFY_ISR(CPC_UART_EVENT_UART_TX_DOWN);
    return 0;
}

static int __uart_rx_callback(void *p_arg)
{
    uint32_t len = 0;
    
    g_uart_rx_io.start += hosal_uart_receive(p_arg, g_uart_rx_io.uart_cache + g_uart_rx_io.start, 
                            TX_BUFF_SIZE - g_uart_rx_io.start - 1);

    if (g_uart_rx_io.start == (TX_BUFF_SIZE - 1)) {
        g_uart_rx_io.start = hosal_uart_receive(p_arg, g_uart_rx_io.uart_cache,
                            (TX_BUFF_SIZE + g_uart_rx_io.end - 1) % TX_BUFF_SIZE);
    } else if (((g_uart_rx_io.start + 1) % TX_BUFF_SIZE) != g_uart_rx_io.end) {
        g_uart_rx_io.start += hosal_uart_receive(p_arg, g_uart_rx_io.uart_cache,
            g_uart_rx_io.end - g_uart_rx_io.start - 1);
    }

    if (g_uart_rx_io.start != g_uart_rx_io.end) {
        len = (g_uart_rx_io.start + TX_BUFF_SIZE - g_uart_rx_io.end) % TX_BUFF_SIZE;
        if (g_uart_rx_io.recvLen != len) {
            g_uart_rx_io.recvLen = len;
            CPC_UART_NOTIFY_ISR(CPC_UART_EVENT_UART_IN);
        }
    }

    return 0;
}
extern uint32_t cpc_drv_uart_push_header(uint8_t *pHeader);
extern void cpc_drv_uart_push_data(uint8_t *p_data, uint16_t length);

static cpc_uart_parser_data_sts_t __cpc_hdlc_pkt_parser(uint8_t *pBuf, uint16_t plen, uint16_t *pkt_len, uint16_t *offset)
{
    cpc_uart_parser_data_sts_t t_return = CPC_UART_DATA_INVALID;

    uint16_t i=0;
    uint16_t idx = 0;
    uint8_t find = 0;
    
    cpc_uart_pkt_hdr *hdr = NULL;
    cpc_uart_pkt_end *end = NULL;    

    do 
    {
        /* find tag */
        if (plen < CPC_HDLC_HEADER_RAW_SIZE)
        {
            break;
        }

        for(i=0; i<plen; i++)
        {
            if(pBuf[i] == CPC_HDLC_FLAG_VAL)
            {
                if(offset)
                {
                    idx = i;
                }
                find = 1;
                break;
            }
        }

        if(find == 0)
            break;

        /* Check header size*/
        if((plen - idx) < CPC_HDLC_HEADER_RAW_SIZE)
        {
            break;
        }

        hdr = (cpc_uart_pkt_hdr *)(pBuf + idx);
        /* Check header crc */
        if (!cpc_validate_crc_sw((pBuf + idx), CPC_HDLC_HEADER_SIZE, hdr->hcs))
        {
            log_error("hcs error");
            log_error_hexdump("Origin", pBuf, plen);
            log_error_hexdump("HDR", (pBuf + idx), CPC_HDLC_HEADER_RAW_SIZE);
            t_return = CPC_UART_DATA_CS_ERROR;
            break;
        }

        /* check pkt len */
        if(plen < (idx + CPC_HDLC_HEADER_RAW_SIZE + hdr->len))
        {
            break;
        }

        if(hdr->len == 0)
        {
            /* Notfiy cpc header with no data */
            cpc_drv_uart_push_header((uint8_t *)hdr);
            t_return = CPC_UART_DATA_VALID; 
            *pkt_len = CPC_HDLC_HEADER_RAW_SIZE;
            *offset = idx + *pkt_len;
            break;
        }

        end = (cpc_uart_pkt_end *)&pBuf[idx + CPC_HDLC_HEADER_RAW_SIZE + (hdr->len - 2)];

        /* Check data crc */
        if (!cpc_validate_crc_sw(hdr->data, (hdr->len-2), end->fcs))
        {
            log_error("fcs error %04X", end->fcs);
            log_error_hexdump("Origin", pBuf, plen);
            log_error_hexdump("Data", hdr->data, hdr->len);
            t_return = CPC_UART_DATA_CS_ERROR;
            break;
        }

        /* Notfiy cpc header with data */
        cpc_drv_uart_push_header((uint8_t *)hdr);
        cpc_drv_uart_push_data(hdr->data , hdr->len);

        *pkt_len = CPC_HDLC_HEADER_RAW_SIZE + hdr->len;
        *offset = idx + *pkt_len;
        t_return = CPC_UART_DATA_VALID;

    } while(0);

    return t_return;
}


static void __cpc_packet_parser_process(cpc_uart_event_t evt)
{
    static uint16_t __total_len = 0;
    static uint8_t __uart_buffer[CONFIG_CPC_RX_PAYLOAD_MAX_LENGTH*2];

    uint16_t len = 0;
    uint16_t pkt_len = 0;
    uint16_t __offset = 0;
    uint8_t tmp_buff[CONFIG_CPC_RX_PAYLOAD_MAX_LENGTH/2];

    cpc_uart_parser_data_sts_t parser_status;

    do
    {
        if (!(CPC_UART_EVENT_UART_IN & evt)) 
        {
            break;
        }

        if(__total_len > (CONFIG_CPC_RX_PAYLOAD_MAX_LENGTH*2))
        {
            __total_len = 0;
        }

        len = __uart_read(tmp_buff);

        if(len == 0)
            break;

        memcpy(__uart_buffer + __total_len, tmp_buff, len);

        __total_len += len;
__parser:
        parser_status = __cpc_hdlc_pkt_parser(__uart_buffer, __total_len, &pkt_len, &__offset);

        if (parser_status == CPC_UART_DATA_CS_ERROR)
        {

            __total_len = 0;
        }
        else if(parser_status == CPC_UART_DATA_VALID)
        {
            __total_len -= __offset;
            if(__total_len > 0)
            {
                memcpy(tmp_buff, __uart_buffer + __offset, __total_len);
                memcpy(__uart_buffer, tmp_buff, __total_len);
                if(__total_len >= CPC_HDLC_HEADER_RAW_SIZE)
                {
                    goto __parser;
                }
            }
        }
    } while (0);
}
static void __check_queue(cpc_uart_event_t evt)
{
    _cpc_uart_data_t uart_data;
    static uint32_t __tx_done = 1;

    if((CPC_UART_EVENT_UART_TX_DOWN & evt))
    {
        __tx_done = 1;
        gpio_pin_set(20);
    }

    if(__tx_done == 1)
    {
        if(xQueueReceive(cpc_uart_handle, (void*)&uart_data, 0) == pdPASS)
        {
            memcpy(txdam_cfg.dma_buf, uart_data.pdata, uart_data.dlen);
            txdam_cfg.dma_buf_size = uart_data.dlen;
            hosal_uart_ioctl(&cpc_uart_dev, HOSAL_UART_DMA_TX_START, &txdam_cfg);
            gpio_pin_clear(20);
            log_debug_hexdump("CPC Tx", uart_data.pdata, uart_data.dlen);
            vPortFree(uart_data.pdata);

            enter_critical_section();
            cpc_drv_trnsmit_complete();
            __tx_done = 0;
            leave_critical_section();
        }
    }
}  

static void __uart_task(void *parameters_ptr)
{
    cpc_uart_event_t sevent = CPC_UART_EVENT_NONE;
    /* Configure UART Rx interrupt callback function */
    hosal_uart_callback_set(&cpc_uart_dev, HOSAL_UART_RX_CALLBACK, __uart_rx_callback, &cpc_uart_dev);
    hosal_uart_callback_set(&cpc_uart_dev, HOSAL_UART_TX_DMA_CALLBACK, __uart_tx_callback, &cpc_uart_dev);

    /* Configure UART to interrupt mode */
    hosal_uart_ioctl(&cpc_uart_dev, HOSAL_UART_MODE_SET, (void *)HOSAL_UART_MODE_INT_RX);

    __NVIC_SetPriority(Uart1_IRQn, 2);

    for(;;)
    {
        CPC_UART_GET_NOTIFY(sevent);

        __cpc_packet_parser_process(sevent);
        __check_queue(sevent);

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
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
}
int cpc_uart_data_send(uint8_t *p_data, uint16_t data_len)
{
    _cpc_uart_data_t uart_data;

    uart_data.pdata = p_data;
    uart_data.dlen = data_len;

    while(xQueueSend(cpc_uart_handle, (void *)&uart_data, portMAX_DELAY) != pdPASS);

    CPC_UART_NOTIFY(CPC_UART_EVENT_CPC_IN);

    
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

    gpio_cfg_output(20);
    gpio_cfg_output(21);

    cpc_uart_handle = xQueueCreate(16, sizeof(_cpc_uart_data_t));

    xReturned = xTaskCreate(__uart_task, "cpc_uart", 512, NULL, configMAX_PRIORITIES - 2, &uart_taskHandle);
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

    else if(cpc_uart_dev.config.baud_rate == UART_BAUDRATE_2000000)
        baud = 2000000;        

    return baud;
}