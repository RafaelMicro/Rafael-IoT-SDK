/**
 * @file bootloader.c
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bootloader.h"
#include "hosal_uart.h"
#include "hosal_wdt.h"
#include "isp.h"
#include "mcu.h"
#include "slip.h"
#include "util_list.h"

typedef struct {
    /* data */
    volatile uint32_t wr_idx;
    volatile uint32_t rd_idx;
    uint8_t* pcache;
} blder_uart_io_t;

typedef struct {
    utils_dlist_t dlist;
    uint32_t length  : 16;
    uint32_t command : 8;
    uint32_t option  : 8;
    uint32_t address;
    uint8_t* pdata;
} blder_event_instance_t;

#define BLDER_EVENT_MAX_BUFFER_SIZE 256
#define BLDER_EVENT_BUFFER_NUM      64

#define ALIGNED_EVENT_SIZE ((sizeof(blder_event_instance_t) + 3) & 0xfffffffc)
#define TOTAL_EVENT_BUFFER_SIZE                                                \
    (ALIGNED_EVENT_SIZE + BLDER_EVENT_MAX_BUFFER_SIZE)

typedef struct {
    /* data */
    utils_dlist_t free_list;
    utils_dlist_t used_list;
    uint8_t buffer_pool[TOTAL_EVENT_BUFFER_SIZE * BLDER_EVENT_BUFFER_NUM];
} blder_event_item_t;

HOSAL_UART_DEV_DECL(blder_uart0_dev, 0, 16, 17, UART_BAUDRATE_2000000)
HOSAL_UART_DEV_DECL(blder_uart1_dev, 1, 28, 29, UART_BAUDRATE_2000000)
HOSAL_UART_DEV_DECL(blder_uart2_dev, 2, 30, 31, UART_BAUDRATE_2000000)

#define BLDER_NOTIFY_HANDEL g_blder_evt_var
#define BLDER_NOTIFY(ebit)  BLDER_NOTIFY_HANDEL |= ebit;
#define BLDER_GET_NOTIFY(ebit)                                                 \
    ebit = BLDER_NOTIFY_HANDEL;                                                \
    BLDER_NOTIFY_HANDEL = BLDER_EVENT_NONE;
#define BLDER_UART_CACHE_SIZE 0x400

typedef enum {
    BLDER_EVENT_NONE = 0,
    BLDER_EVENT_UART_TX_COMPELETE = 0x00000001,
    BLDER_EVENT_UART_RX_DATA_IN = 0x00000002,

    BLDER_EVENT_TIMEOUT = 0x00000010,

    BLDER_EVENT_OPERATION_FLASH = 0x00000100,
    BLDER_EVENT_ALL = 0xffffffff,
} blder_event_t;

typedef enum {

    WAIT_SLIP_DELIMITER_STATE = 0,
    RECV_HEADER_STATE = 1,
    RECV_DATA_CRC_STATE = 2,
    RECV_CHECK_PACKET = 3,
} bootloader_slip_state_t;

static utils_dlist_t g_event_buffer_list;
static utils_dlist_t g_event_list;
static blder_event_instance_t g_event_instance;
static blder_event_item_t g_event_item;
static uint32_t g_slip_state = WAIT_SLIP_DELIMITER_STATE;
static hosal_wdt_config_mode_t g_keep_alive_init_cfg;
static volatile uint8_t used_uart = 0;

// static uint8_t g_tx_buf[12];
static uint32_t g_blder_evt_var = 0;
static uint8_t g_uart_rx_cache[BLDER_UART_CACHE_SIZE];
// static hosal_uart_dma_cfg_t txdam_cfg = {
//     .dma_buf = g_tx_buf,
//     .dma_buf_size = 0,
// };

static blder_uart_io_t g_uart_io = {
    .pcache = g_uart_rx_cache,
    .wr_idx = 0,
    .rd_idx = 0,
};

void wdt_isr(void) { BLDER_NOTIFY(BLDER_EVENT_TIMEOUT); }

static void keep_alive_init(hosal_wdt_config_mode_t* p_keep_alive_dev) {

    p_keep_alive_dev->int_enable = 1;
    p_keep_alive_dev->reset_enable = 0;
    p_keep_alive_dev->lock_enable = 0;
    p_keep_alive_dev->prescale = HOSAL_WDT_PRESCALE_32;
}

static void keep_alive_set(hosal_wdt_config_mode_t* p_keep_alive_dev,
                           uint32_t timout) {
    hosal_wdt_config_tick_t wdt_cfg_ticks;
    wdt_cfg_ticks.wdt_ticks = timout * 1000;
    wdt_cfg_ticks.int_ticks = 10 * 1000;
    wdt_cfg_ticks.wdt_min_ticks = 0;
    hosal_wdt_stop();
    hosal_wdt_start(*p_keep_alive_dev, wdt_cfg_ticks, wdt_isr);
    NVIC_EnableIRQ(Wdt_IRQn);
}

static inline void keep_alive_flush(void) { wdt_kick(); }

static void keep_alive_stop(void) { 
    hosal_wdt_stop(); 
    NVIC_DisableIRQ(Wdt_IRQn);    
}

__STATIC_FORCEINLINE void bootloader_isr(uint8_t id) {
    uint8_t ch;
    UART_T* uart = UART1;
    hosal_uart_dev_t* blder_uart_dev = NULL;
    used_uart = id;
    uint8_t value[8] = {0};

    if (id == 1) {
        uart = UART1;
        blder_uart_dev = &blder_uart1_dev;
    } else if (id == 2) {
        uart = UART2;
        blder_uart_dev = &blder_uart2_dev;
    }

    if ((uart->xDMA_INT_STATUS & xDMA_ISR_TX) == xDMA_ISR_TX) {
        uart->xDMA_INT_STATUS = xDMA_ISR_TX;
        uart->xDMA_TX_ENABLE = xDMA_Stop;
    }

    uint32_t iir = uart->IIR & IIR_INTID_MSK;
    if ((iir != IIR_INTID_RDA) && (iir != IIR_INTID_CTI))
        return;
    if (!(uart->LSR & UART_LSR_DR))
        return;
    uint32_t wr_pos = g_uart_io.wr_idx;
    uint32_t rd_pos = g_uart_io.rd_idx;
    uint32_t cnt = hosal_uart_receive(blder_uart_dev, value, 8);

    uint32_t pos = (wr_pos + cnt) % BLDER_UART_CACHE_SIZE;

    if (pos == rd_pos) {
        DEBUG_PRINT("q full\r\n");
        return;
    }

    for (int i = 0; i < cnt; i++)
        g_uart_io.pcache[((wr_pos + i) % BLDER_UART_CACHE_SIZE)] = value[i];

    g_uart_io.wr_idx = pos;

    // BLDER_NOTIFY(BLDER_EVENT_UART_RX_DATA_IN);
}

static void blder_event_list_init(void) {
    blder_event_instance_t* pevent = NULL;
    memset(&g_event_item, 0x00, offsetof(blder_event_item_t, buffer_pool));

    utils_dlist_init(&g_event_item.free_list);
    utils_dlist_init(&g_event_item.used_list);

    for (int i = 0; i < BLDER_EVENT_BUFFER_NUM; i++) {
        pevent = (blder_event_instance_t*)(g_event_item.buffer_pool
                                           + (TOTAL_EVENT_BUFFER_SIZE * i));

        pevent->pdata = ((uint8_t*)pevent) + ALIGNED_EVENT_SIZE;

        utils_dlist_add_tail(&pevent->dlist, &g_event_item.free_list);
    }
}

static void bootloader_enter_isp_mode(void) {
    static uint16_t length = 0;
    static uint8_t isp_cmd_req[640];
    uint8_t ch, ack[4];

    uint32_t pos = 0;

    do {
        switch (g_slip_state) {
            case WAIT_SLIP_DELIMITER_STATE: {
                if (bootloader_uart_recv(&ch, 1)) {
                    if (ch == SLIP_DELIMITER) {
                        g_slip_state = RECV_HEADER_STATE;
                        keep_alive_flush();
                    }
                }
                break;
            }
            case RECV_HEADER_STATE: {
                if (slip_packet_decode(isp_cmd_req, 5) == 0xFF) {
                    g_slip_state = RECV_HEADER_STATE;
                    break;
                }
                /* Checksum */
                if (isp_header_check(isp_cmd_req, &length) == 1) {
                    g_slip_state = WAIT_SLIP_DELIMITER_STATE;
                    break;
                }
                g_slip_state = RECV_DATA_CRC_STATE;
                keep_alive_flush();

                break;
            }
            case RECV_DATA_CRC_STATE: {
                // DEBUG_PRINT("L: %d\r\n", length);
                if (slip_packet_decode(&isp_cmd_req[5], length + 2) == 0xFF) {
                    g_slip_state = RECV_HEADER_STATE;
                    break;
                }
                g_slip_state = RECV_CHECK_PACKET;
                keep_alive_flush();
                break;
            }
            case RECV_CHECK_PACKET: {
                g_slip_state = WAIT_SLIP_DELIMITER_STATE;
                while (1) {
                    if (bootloader_uart_recv(&ch, 1)) {
                        if (ch == 0xc0)
                            break;
                    }
                }
                if (isp_payload_check(isp_cmd_req) == 1) {
                    break;
                }
                isp_command_handler(isp_cmd_req);

                break;
            }
            default: break;
        }
    } while (0);
}

static void bootloader_isp_proc(void) {
    static uint32_t mstate_sync = 0;
    uint8_t ack[4];
    char ch;

    if (mstate_sync == 0) {
        if (bootloader_uart_peek(&ch, 1)) {
            if (ch == 0xAA) {
                memset(ack, 0x55, 4);
                bootloader_uart_send(ack, 1);
                bootloader_uart_recv(&ch, 1);
            } else if (ch == 0x69) {
                keep_alive_flush();
                memset(ack, 0x6A, 4);
                bootloader_uart_send(ack, 1);
                bootloader_uart_recv(&ch, 1);
            } else if (ch == 0xC0) {
                mstate_sync = 1;
                DEBUG_PRINT("[BOOT] Sync Success\r\n");
                // keep_alive_init(&g_keep_alive_init_cfg);
                // keep_alive_set(&g_keep_alive_init_cfg, 3000);
                keep_alive_stop();
            }
        }
    } else {
        bootloader_enter_isp_mode();
    }
}

static void execute_erase_flash_command(uint8_t command, uint32_t addr,
                                        uint8_t option) {
    // DEBUG_PRINT("E: %x\r\n", addr);

    if (option == 0x01) {
        FLASH->command = CMD_ERASESECTOR;
    } else if (option == 0x02) {
        FLASH->command = CMD_ERASE_BL32K;
    } else if (option == 0x03) {
        FLASH->command = CMD_ERASE_BL64K;
    }
    FLASH->flash_addr = addr;
    FLASH->pattern = FLASH_UNLOCK_PATTER;
    FLASH->start = STARTBIT;

    while ((FLASH->start & BUSYBIT) == BUSYBIT) {};
    return;
}

static void execute_write_and_verify_command(uint8_t command, uint32_t addr,
                                             uint8_t option, uint8_t* data) {
    // DEBUG_PRINT("W: %x\r\n", addr);

    FLASH->command = CMD_WRITEPAGE;
    FLASH->flash_addr = addr;
    FLASH->mem_addr = (uint32_t)data;
    FLASH->pattern = FLASH_UNLOCK_PATTER;
    FLASH->start = STARTBIT;

    while ((FLASH->start & BUSYBIT) == BUSYBIT) {};
    return;
}

void bootloader_check_flash_op_command(void) {
    blder_event_instance_t* pevent = NULL;

    if ((FLASH->start & BUSYBIT) != BUSYBIT) {

        if (!utils_dlist_empty(&g_event_item.used_list)) {
            pevent = (blder_event_instance_t*)g_event_item.used_list.next;
            utils_dlist_del(&pevent->dlist);
        }

        if (pevent != NULL) {
            // DEBUG_PRINT("O(%x, %x)\r\n", pevent->command, pevent->address);
            switch (pevent->command) {
                case 0x08:
                    execute_erase_flash_command(
                        pevent->command, pevent->address, pevent->option);
                    break;
                case 0x09:
                    execute_write_and_verify_command(
                        pevent->command, pevent->address, pevent->option,
                        pevent->pdata);
                    break;
                default: break;
            }
            if (pevent->pdata != NULL) {
                free(pevent->pdata);
            }
            utils_dlist_add_tail(&pevent->dlist, &g_event_item.free_list);
        }
    }
}

uint8_t bootloader_uart_peek(uint8_t* pbuf, uint16_t len) {
    size_t byte_cnt = 0;
    uint32_t rd_pos = g_uart_io.rd_idx;
    uint32_t wr_pos = g_uart_io.wr_idx;

    if (rd_pos == wr_pos)
        return 0;

    memcpy(pbuf, &g_uart_io.pcache[rd_pos], len);

    return 1;
}

size_t bootloader_uart_recv(uint8_t* pbuf, size_t len) {
    size_t byte_cnt = 0;
    uint32_t rd_pos = g_uart_io.rd_idx;
    uint32_t wr_pos = g_uart_io.wr_idx;

    while (1) {
        if (rd_pos == wr_pos || len == byte_cnt)
            break;

        pbuf[byte_cnt++] = g_uart_io.pcache[rd_pos];
        rd_pos = (rd_pos + 1) % BLDER_UART_CACHE_SIZE;
    }

    g_uart_io.rd_idx = rd_pos;

    return byte_cnt;
}

void bootloader_uart_send(uint8_t* buf, uint16_t len) {
    hosal_uart_dev_t* blder_uart_dev = NULL;
    if (used_uart == 1) {
        blder_uart_dev = &blder_uart1_dev;
    } else {
        blder_uart_dev = &blder_uart2_dev;
    }
    // txdam_cfg.dma_buf = buf;
    // txdam_cfg.dma_buf_size = len;
    hosal_uart_send(blder_uart_dev, buf, len);

    // hosal_uart_ioctl(&blder_uart_dev, HOSAL_UART_DMA_TX_START, &txdam_cfg);
}

uint32_t bootloader_flash_op_event_trigger(uint32_t address, uint8_t cmd,
                                           uint8_t op, uint8_t* pdata) {

    static uint8_t seq;
    uint32_t ret = 0;
    blder_event_instance_t* p = NULL;
    if (!utils_dlist_empty(&g_event_item.free_list)) {
        p = (blder_event_instance_t*)g_event_item.free_list.next;
        utils_dlist_del(&p->dlist);
    }
    if (p) {
        p->address = address;
        p->command = cmd;
        p->option = op;
        p->pdata = pdata;
        utils_dlist_add_tail(&p->dlist, &g_event_item.used_list);
        // DEBUG_PRINT("I(%x, %x)\r\n", cmd, address);
        // BLDER_NOTIFY(BLDER_EVENT_OPERATION_FLASH);
    } else
        ret = 1;
    return ret;
}

void bootloader_uart_init(void) {
    blder_event_list_init();

    hosal_uart_init(&blder_uart0_dev);
    hosal_uart_init(&blder_uart1_dev);
    hosal_uart_init(&blder_uart2_dev);
    hosal_uart_ioctl(&blder_uart1_dev, HOSAL_UART_MODE_SET,
                     (void*)HOSAL_UART_MODE_INT_RX);

    hosal_uart_ioctl(&blder_uart2_dev, HOSAL_UART_MODE_SET,
                     (void*)HOSAL_UART_MODE_INT_RX);
}

uint32_t bootloader_run_loop(void) {
    uint32_t evt = 0;

    keep_alive_init(&g_keep_alive_init_cfg);
    keep_alive_set(&g_keep_alive_init_cfg, 50);
    while (1) {
        BLDER_GET_NOTIFY(evt);

        bootloader_isp_proc();

        // if (evt & BLDER_EVENT_OPERATION_FLASH) {
        //     bootloader_check_flash_op_command();
        //     keep_alive_flush();
        // }
        if (evt & BLDER_EVENT_TIMEOUT) {
            DEBUG_PRINT("[BOOT] Timeout\r\n");
            break;
        }
    }

    hosal_uart_finalize(&blder_uart1_dev);
    hosal_uart_finalize(&blder_uart2_dev);
    return evt;
}
#pragma GCC pop_options

void uart1_handler(void) { bootloader_isr(1); }

void uart2_handler(void) { bootloader_isr(2); }
