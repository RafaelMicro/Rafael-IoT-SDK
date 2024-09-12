#ifndef CONFIG_BOOTLOADER
#include <FreeRTOS.h>
#include <task.h>
#endif
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hosal_uart.h"
#include "mcu.h"

#ifndef CONFIG_UART_STDIO_PORT
#define CONFIG_UART_STDIO_PORT 0
#endif // !CONFIG_UART_STDIO_POR

#ifndef CONFIG_UART_STDIO_BAUDRATE
#define CONFIG_UART_STDIO_BAUDRATE 2000000
#endif // !CONFIG_UART_STDIO_BAUDRATE

#ifndef CONFIG_UART_STDIO_TX_PIN
#define CONFIG_UART_STDIO_TX_PIN 16
#endif // !CONFIG_UART_STDIO_TX_PIN

#ifndef CONFIG_UART_STDIO_RX_PIN
#define CONFIG_UART_STDIO_RX_PIN 17
#endif // !CONFIG_UART_STDIO_RX_PIN

typedef struct execption_ctxt {
    uint32_t exp_vect;
    uint32_t exp_addr;
    uint32_t spsr;
    uint32_t cpsr;
    uint32_t r[16];
} exception_ctxt_t;

#if (CONFIG_UART_STDIO_BAUDRATE == 115200)
HOSAL_UART_DEV_DECL(uartstdio, CONFIG_UART_STDIO_PORT, CONFIG_UART_STDIO_TX_PIN,
                    CONFIG_UART_STDIO_RX_PIN, UART_BAUDRATE_115200)
#elif (CONFIG_UART_STDIO_BAUDRATE == 500000)
HOSAL_UART_DEV_DECL(uartstdio, CONFIG_UART_STDIO_PORT, CONFIG_UART_STDIO_TX_PIN,
                    CONFIG_UART_STDIO_RX_PIN, UART_BAUDRATE_500000)
#elif (CONFIG_UART_STDIO_BAUDRATE == 1000000)
HOSAL_UART_DEV_DECL(uartstdio, CONFIG_UART_STDIO_PORT, CONFIG_UART_STDIO_TX_PIN,
                    CONFIG_UART_STDIO_RX_PIN, UART_BAUDRATE_1000000)
#elif (CONFIG_UART_STDIO_BAUDRATE == 2000000)
HOSAL_UART_DEV_DECL(uartstdio, CONFIG_UART_STDIO_PORT, CONFIG_UART_STDIO_TX_PIN,
                    CONFIG_UART_STDIO_RX_PIN, UART_BAUDRATE_2000000)
#endif // 0

#define STDIO_UART_BUFF_SIZE 128

typedef struct uart_io {
    uint16_t start;
    uint16_t end;
    uint32_t recvLen;
    uint8_t uart_cache[STDIO_UART_BUFF_SIZE];
} uart_io_t;

static uart_io_t g_uart_rx_io = {
    .start = 0,
    .end = 0,
};
static char bsp_c_g_msg[64] __attribute__((aligned(4))) = {0};
exception_ctxt_t __exi_ctxt;

static int __uartstdio_rx_callback(void* p_arg) {
    uint32_t len = 0;
    if (g_uart_rx_io.start >= g_uart_rx_io.end) {
        g_uart_rx_io.start += hosal_uart_receive(
            p_arg, g_uart_rx_io.uart_cache + g_uart_rx_io.start,
            STDIO_UART_BUFF_SIZE - g_uart_rx_io.start - 1);
        if (g_uart_rx_io.start == (STDIO_UART_BUFF_SIZE - 1)) {
            g_uart_rx_io.start = hosal_uart_receive(
                p_arg, g_uart_rx_io.uart_cache,
                (STDIO_UART_BUFF_SIZE + g_uart_rx_io.end - 1)
                    % STDIO_UART_BUFF_SIZE);
        }
    } else if (((g_uart_rx_io.start + 1) % STDIO_UART_BUFF_SIZE)
               != g_uart_rx_io.end) {
        g_uart_rx_io.start += hosal_uart_receive(
            p_arg, g_uart_rx_io.uart_cache,
            g_uart_rx_io.end - g_uart_rx_io.start - 1);
    }

    if (g_uart_rx_io.start != g_uart_rx_io.end) {

        len = (g_uart_rx_io.start + STDIO_UART_BUFF_SIZE - g_uart_rx_io.end)
              % STDIO_UART_BUFF_SIZE;
        if (g_uart_rx_io.recvLen != len)
            g_uart_rx_io.recvLen = len;
    }
    return 0;
}

static int __uartstdio_line_status_callback(void* p_arg) {

    hosal_uart_get_lsr((hosal_uart_dev_t*)p_arg);
}

int uart_stdio_write(uint8_t* p_data, uint32_t length) {
    return hosal_uart_send(&uartstdio, p_data, length);
}

int uart_stdio_read(uint8_t* p_data, uint32_t length) {
    uint32_t byte_cnt = 0;
    enter_critical_section();

    if (g_uart_rx_io.start != g_uart_rx_io.end) {
        if (g_uart_rx_io.start > g_uart_rx_io.end) {
            memcpy(p_data, g_uart_rx_io.uart_cache + g_uart_rx_io.end,
                   g_uart_rx_io.start - g_uart_rx_io.end);
            g_uart_rx_io.end = g_uart_rx_io.start;
        } else {
            memcpy(p_data, g_uart_rx_io.uart_cache + g_uart_rx_io.end,
                   STDIO_UART_BUFF_SIZE - g_uart_rx_io.end);
            g_uart_rx_io.end = STDIO_UART_BUFF_SIZE - 1;
            if (g_uart_rx_io.start) {
                memcpy(p_data, g_uart_rx_io.uart_cache, g_uart_rx_io.start);
                g_uart_rx_io.end = (STDIO_UART_BUFF_SIZE + g_uart_rx_io.start
                                    - 1)
                                   % STDIO_UART_BUFF_SIZE;
            }
        }
    }

    byte_cnt = g_uart_rx_io.recvLen;

    g_uart_rx_io.start = g_uart_rx_io.end = 0;
    g_uart_rx_io.recvLen = 0;
    leave_critical_section();
    return byte_cnt;
}

void uart_stdio_en_int_mode(void) {
    hosal_uart_ioctl(&uartstdio, HOSAL_UART_MODE_SET,
                     (void*)HOSAL_UART_MODE_INT_RX);
}

int uart_stdio_deinit(void) {
    hosal_uart_send_complete(&uartstdio);
    hosal_uart_finalize(&uartstdio);
    return 0;
}

int uart_stdio_init(void) {
    /*Init UART In the first place*/
    hosal_uart_init(&uartstdio);

    /* Configure UART Rx interrupt callback function */
    hosal_uart_callback_set(&uartstdio, HOSAL_UART_RX_CALLBACK,
                            __uartstdio_rx_callback, &uartstdio);

    /* Configure UART break interrupt callback function */
    hosal_uart_callback_set(&uartstdio, HOSAL_UART_RECEIVE_LINE_STATUS_CALLBACK,
                            __uartstdio_line_status_callback, &uartstdio);

    /* Configure UART to interrupt mode */
    hosal_uart_ioctl(&uartstdio, HOSAL_UART_MODE_SET,
                     (void*)HOSAL_UART_MODE_INT_RX);

    return 0;
}

static void _uint2strhex(char* pStr, unsigned int number,
                         const char nibbles_to_print) {
#define MAX_NIBBLES (8)
    int nibble = 0;
    char nibbles = (nibbles_to_print > MAX_NIBBLES) ? nibbles_to_print
                                                    : MAX_NIBBLES;

    while (nibbles > 0) {
        nibbles--;
        nibble = (int)(number >> (nibbles * 4)) & 0x0F;
        pStr[strlen(pStr)] = (nibble <= 9) ? (char)('0' + nibble)
                                           : (char)('A' - 10 + nibble);
    }
    return;
}

static int _exp_dump_out(char* pMsg, int len) {

    UART_T* table[] = {UART0, UART1, UART2};
    UART_T* pCSR = table[CONFIG_UART_STDIO_PORT];
    while (len) {
#ifdef CONFIG_RT584
        while ((pCSR->lsr & 0x20) == 0) {}
        pCSR->thr = *pMsg++;
#else
        while ((pCSR->LSR & 0x20) == 0) {}
        pCSR->THR = *pMsg++;
#endif
        len--;
    }

    return 0;
}

static void _exp_log_out(const char* format, ...) {
    char* pch = (char*)format;
    va_list va;
    va_start(va, format);

    if (!pch) {
        va_end(va);
        return;
    }

    while (*pch) {
        /* format identification character */
        if (*pch != '%') {
            _exp_dump_out(pch, 1);
            pch++;
            continue;
        }

        pch++;
        if (!pch) {
            pch++;
            continue;
        }

        switch (*pch) {
            case 'x': {
                const unsigned int number = va_arg(va, unsigned int);
                for (int j = 0; j < sizeof(bsp_c_g_msg); j += 4)
                    *(uint32_t*)&bsp_c_g_msg[j] = 0;
                strcpy(&bsp_c_g_msg[strlen(bsp_c_g_msg)], "0x");
                _uint2strhex(bsp_c_g_msg, number, 8);
                _exp_dump_out(bsp_c_g_msg, strlen(bsp_c_g_msg));
            } break;
            case 's': {
                char* string = va_arg(va, char*);
                _exp_dump_out(string, strlen(string));
            } break;
            default: break;
        }
        pch++;
    }

    va_end(va);
    return;
}

static int _exp_dump_init(void) {
    UART_T* table[] = {UART0, UART1, UART2};
    UART_T* pCSR = table[CONFIG_UART_STDIO_PORT];
#ifdef CONFIG_RT584
    pCSR->dlx = UART_BAUDRATE_Baud2000000 & 0xFFFF;
    pCSR->fdl = (UART_BAUDRATE_Baud2000000 >> UART_BR_FRCT_SHIFT) & 0xFF;
    pCSR->lsm = 0;
    pCSR->lcr = 0x3;
#else
    pCSR->LCR |= (0x1 << 7);
    pCSR->DLL = UART_BAUDRATE_Baud2000000 & 0xFF;
    pCSR->DLM = UART_BAUDRATE_Baud2000000 >> 8;
    pCSR->LCR &= ~(0x1 << 7);
    pCSR->LCR = 0x3;
#endif
    return 0;
}

void my_fault_handler_c(uint32_t* sp, uint32_t lr_value) {
    _exp_dump_init();
    _exp_log_out("\r\n[HardFaultHandler]\r\n");
    _exp_log_out("R0= %x, R1= %x R2= %x R3= %x\r\n", sp[0], sp[1], sp[2],
                 sp[3]);

    _exp_log_out("R12= %x, LR= %x, PC= %x, PSR= %x\r\n", sp[4], sp[5], sp[6],
                 sp[7]);

    _exp_log_out("LR Value= %x\r\n", lr_value);

    while (1) {}
    NVIC_SystemReset(); //while need change to reset
}

void HardFault_Handler(void) {
    __asm volatile("   movs r0, #4\n"
                   "   mov r1, LR\n"
                   "   tst r0, r1\n"
                   "   mrs r0, psp\n"
                   "   mrs r0, msp\n"
                   "   mov r1, LR\n"
                   "   b my_fault_handler_c\n");
}
