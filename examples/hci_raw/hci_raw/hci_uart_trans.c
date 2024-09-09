/**
 * @file hci_uart_trans.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "hci_bridge.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "queue.h"
#include "hosal_uart.h"
#include "log.h"

#ifndef CONFIG_OPERATION_UART_PORT
#define CONFIG_OPERATION_UART_PORT 1
#endif // !CONFIG_OPERATION_UART_PORT

HOSAL_UART_DEV_DECL(hci_uart_dev, CONFIG_OPERATION_UART_PORT, 4, 5, UART_BAUDRATE_115200)

#define STDIO_UART_BUFF_SIZE    512

typedef struct __attribute__((packed)) uart_io
{
    uint16_t start;
    uint16_t end;

    uint32_t recvLen;
    uint8_t uart_cache[STDIO_UART_BUFF_SIZE];
} uart_io_t;

typedef struct 
{
    uint16_t dlen;
    uint8_t pdata[];
} _hci_uart_data_t ;


typedef enum  {
    HIC_UART_EVENT_NONE                       = 0,

    HIC_UART_EVENT_UART_IN                    = 0x00000002,
    HIC_UART_EVENT_HCI_IN                     = 0x00000004,
    HCI_UART_EVENT_UART_TX_DOWN               = 0x00000008,
    HCI_UART_EVENT_UART_TIMEOUT               = 0x00000010,

    HIC_UART_EVENT_ALL                        = 0xffffffff,
} hci_uart_event_t;

#define HCI_UART_NOTIFY_ISR(ebit)                 (g_hci_uart_evt_var |= ebit); __hci_uart_signal()
#define HCI_UART_NOTIFY(ebit)                     enter_critical_section(); g_hci_uart_evt_var |= ebit; leave_critical_section(); __hci_uart_signal()
#define HCI_UART_GET_NOTIFY(ebit)                 enter_critical_section(); ebit = g_hci_uart_evt_var; g_hci_uart_evt_var = HIC_UART_EVENT_NONE; leave_critical_section()
#define TX_BUFF_SIZE                    4096

static uart_io_t g_uart_rx_io = { .start = 0, .end = 0, };
static hci_uart_event_t g_hci_uart_evt_var;
static TaskHandle_t uart_taskHandle = NULL;
static volatile uint32_t hci_cmd_timout = 0;
static QueueHandle_t hci_uart_handle;
static uint8_t g_temp_buf[TX_BUFF_SIZE];
static uint8_t g_tx_buf[TX_BUFF_SIZE];
static hosal_uart_dma_cfg_t txdam_cfg = {
    .dma_buf = g_tx_buf,
    .dma_buf_size = sizeof(g_tx_buf),
};

static void __hci_uart_signal(void)
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
    enter_critical_section();

    if (g_uart_rx_io.start != g_uart_rx_io.end) {
        if (g_uart_rx_io.start > g_uart_rx_io.end) {
            memcpy(p_data, g_uart_rx_io.uart_cache + g_uart_rx_io.end, g_uart_rx_io.start - g_uart_rx_io.end);
            g_uart_rx_io.end = g_uart_rx_io.start;
        }
        else {
            memcpy(p_data, g_uart_rx_io.uart_cache + g_uart_rx_io.end, STDIO_UART_BUFF_SIZE - g_uart_rx_io.end);
            g_uart_rx_io.end = STDIO_UART_BUFF_SIZE - 1;
            if (g_uart_rx_io.start) {
                memcpy(p_data, g_uart_rx_io.uart_cache, g_uart_rx_io.start);
                g_uart_rx_io.end = (STDIO_UART_BUFF_SIZE + g_uart_rx_io.start - 1) % STDIO_UART_BUFF_SIZE;
            }
        }
    }

    byte_cnt = g_uart_rx_io.recvLen;

    g_uart_rx_io.start = g_uart_rx_io.end = 0;
    g_uart_rx_io.recvLen = 0;
    leave_critical_section();
    return byte_cnt;
}


static void hci_packet_parser_process(hci_uart_event_t evt)
{
    uint8_t     *uart_buff;
    uint32_t    u32_byte_cnt = 0;
    static ble_hci_message_t hci_msg;
    uint16_t  len = 0;
    static uint16_t received_len = 0;
    static uint16_t idx = 0;

    if (!(HIC_UART_EVENT_UART_IN & evt)) 
    {
        return;
    }
    hci_cmd_timout = 0;

    uart_buff = hci_msg.hci_message.ble_hci_array;

    u32_byte_cnt = __uart_read(&uart_buff[idx]);
   
    if (u32_byte_cnt)
    {   
        received_len += u32_byte_cnt;
        do
        {
            if(received_len >= 4)
            {
                if(uart_buff[0] == 0x01)
                {
                    len = uart_buff[3] + 4;
                    if(received_len >= len)
                    {
                        log_info_hexdump("HCI CMD", uart_buff, len);

                        hci_bridge_message_write(&hci_msg);
                        idx = received_len - len;
                        received_len = idx;

                        if(idx)
                        {
                            memcpy(uart_buff, &uart_buff[len], idx);
                        }
                        break;
                    }
                }
                else if(uart_buff[0] == 0x02)
                {
                    if(received_len >= 5)
                    {
                        len = (uart_buff[3] | (uart_buff[4] << 8)) + 5;

                        if(received_len >= len)
                        {
                            log_info_hexdump("HCI DATA", uart_buff, len);
                            hci_bridge_message_write(&hci_msg);
                            idx = received_len - len;
                            received_len = idx;
                            if(idx)
                            {
                                memcpy(uart_buff, &uart_buff[len], idx);
                            }                            
                            break;
                        }
                    }
                }
                idx += u32_byte_cnt;
            }
        } while (0);
    }
}
static void __check_queue(hci_uart_event_t evt)
{
    _hci_uart_data_t *uart_data = NULL;
    static uint32_t __tx_done = 1;
    static uint32_t idx = 0;

    if((HIC_UART_EVENT_HCI_IN & evt))
    {
        if(idx >= 3600)
            idx = 0;
        while(xQueueReceive(hci_uart_handle, (void*)&uart_data, 0) == pdPASS)
        {
            memcpy(&g_temp_buf[idx], uart_data->pdata, uart_data->dlen);
            log_info_hexdump("HCI EVT", uart_data->pdata, uart_data->dlen);
            idx+= uart_data->dlen;
            vPortFree(uart_data);
        }
    }

    if((HCI_UART_EVENT_UART_TX_DOWN & evt))
    {
        __tx_done = 1;
    }

    if(__tx_done == 1 && idx != 0)
    {
        __tx_done = 0;
        memcpy(txdam_cfg.dma_buf, g_temp_buf, idx);
        txdam_cfg.dma_buf_size = idx;
        hosal_uart_ioctl(&hci_uart_dev, HOSAL_UART_DMA_TX_START, &txdam_cfg);
        idx = 0;
    }

}  

static void uart_task(void *parameters_ptr)
{
    hci_uart_event_t sevent = HIC_UART_EVENT_NONE;
    for(;;)
    {
        HCI_UART_GET_NOTIFY(sevent);

        hci_packet_parser_process(sevent);
        __check_queue(sevent);

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }
    
}

static int __uart_tx_callback(void *p_arg)
{
    HCI_UART_NOTIFY_ISR(HCI_UART_EVENT_UART_TX_DOWN);
    return 0;
}

static int __uart_rx_callback(void *p_arg)
{
    uint32_t len = 0;
    if(g_uart_rx_io.start >= g_uart_rx_io.end) 
    {
        g_uart_rx_io.start += hosal_uart_receive(p_arg, g_uart_rx_io.uart_cache + g_uart_rx_io.start, 
                                STDIO_UART_BUFF_SIZE - g_uart_rx_io.start - 1);
        if(g_uart_rx_io.start == (STDIO_UART_BUFF_SIZE-1))
        {
            g_uart_rx_io.start = hosal_uart_receive(p_arg, g_uart_rx_io.uart_cache, 
                                (STDIO_UART_BUFF_SIZE + g_uart_rx_io.end -1) % STDIO_UART_BUFF_SIZE);
        }
    }
    else if (((g_uart_rx_io.start + 1) % STDIO_UART_BUFF_SIZE) != g_uart_rx_io.end) 
    {
        g_uart_rx_io.start += hosal_uart_receive(p_arg, g_uart_rx_io.uart_cache, 
            g_uart_rx_io.end - g_uart_rx_io.start - 1);
    }

    if (g_uart_rx_io.start != g_uart_rx_io.end) 
    {

        len = (g_uart_rx_io.start + STDIO_UART_BUFF_SIZE - g_uart_rx_io.end) % STDIO_UART_BUFF_SIZE;
        if (g_uart_rx_io.recvLen != len) 
        {
            g_uart_rx_io.recvLen = len;
        }
    }

    HCI_UART_NOTIFY_ISR(HIC_UART_EVENT_UART_IN);

    return 0;
}

static int __hci_evt_cb(uint8_t *p_data, uint16_t data_len)
{    
    _hci_uart_data_t *uart_data = NULL;
    uart_data = pvPortMalloc(sizeof(_hci_uart_data_t) + data_len);

    if(uart_data)
    {
        memcpy(uart_data->pdata, p_data, data_len);
        uart_data->dlen = data_len;
        if(xQueueSend(hci_uart_handle, (void *) &uart_data, 0) != pdPASS)
        {
            log_error("q full!");
            vPortFree(uart_data);
        }
    }

    HCI_UART_NOTIFY(HIC_UART_EVENT_HCI_IN);
    return 0;
}

static int __hci_data_cb(uint8_t *p_data, uint16_t data_len)
{
    _hci_uart_data_t *uart_data = NULL;
    uart_data = pvPortMalloc(sizeof(_hci_uart_data_t) + data_len);

    if(uart_data)
    {
        memcpy(uart_data->pdata, p_data, data_len);
        uart_data->dlen = data_len;
        if(xQueueSend(hci_uart_handle, (void *) &uart_data, 0) != pdPASS)
        {
            log_error("q full!");
            vPortFree(uart_data);
        }
    }

    HCI_UART_NOTIFY(HIC_UART_EVENT_HCI_IN);    
    return 0;
}

void hci_uart_init(void)
{
    BaseType_t xReturned;

    /*Init UART In the first place*/
    hosal_uart_init(&hci_uart_dev);

    /* Configure UART Rx interrupt callback function */
    hosal_uart_callback_set(&hci_uart_dev, HOSAL_UART_RX_CALLBACK, __uart_rx_callback, &hci_uart_dev);
    hosal_uart_callback_set(&hci_uart_dev, HOSAL_UART_TX_DMA_CALLBACK, __uart_tx_callback, &hci_uart_dev);

    /* Configure UART to interrupt mode */
    hosal_uart_ioctl(&hci_uart_dev, HOSAL_UART_MODE_SET, (void *)HOSAL_UART_MODE_INT_RX);


    hci_bridge_callback_set(HIC_INTERFACE_CALLBACK_TYPE_EVENT, __hci_evt_cb);
    hci_bridge_callback_set(HIC_INTERFACE_CALLBACK_TYPE_DATA, __hci_data_cb);

    xReturned = xTaskCreate(uart_task, "TASK_UART", 1024, NULL, 1, &uart_taskHandle);
    if( xReturned != pdPASS )
    {
        log_error("task create fail\n");
    }

    hci_uart_handle = xQueueCreate(128, sizeof(_hci_uart_data_t *));
}
