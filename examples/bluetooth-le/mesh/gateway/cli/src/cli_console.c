
/** @file cli_console.c
 *
 * @license
 * @description
 */


#if (SUPPORT_DEBUG_CONSOLE_CLI == 1)
#include "types.h"
#include "uart_stdio.h"

#include "shell.h"


#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
//=============================================================================
//                  Constant Definition
//=============================================================================
#define CLI_STACK_SIZE      2048

//=============================================================================
//                  Macro Definition
//=============================================================================
#define CLI_CMD_DEEP        (8)
#define CLI_LINE_SIZE       (256)
#define CLI_HISTORY_SIZE    SHELL_CALC_HISTORY_BUFFER(CLI_LINE_SIZE, CLI_CMD_DEEP)
//=============================================================================
//                  Structure Definition
//=============================================================================

//=============================================================================
//                  Global Data Definition
//=============================================================================
static char         g_line_buf[CLI_LINE_SIZE] = {0};
static char         g_history[CLI_HISTORY_SIZE] = {0};
static sh_args_t    g_sh_args = {0};

static TaskHandle_t cli_task_handle;

#define UART_CACHE_SIZE 1024

typedef struct uart_io {
    volatile uint32_t wr_idx;
    volatile uint32_t rd_idx;
    uint8_t uart_cache[UART_CACHE_SIZE];
} uart_io_t;

static uart_io_t g_uart_rx_io = {
    .wr_idx = 0,
    .rd_idx = 0,
};

extern int stdout_string(char *str, int length);
extern int stdin_str(char *pBuf, int length);
//=============================================================================
//                  Private Function Definition
//=============================================================================
static int
_sh_io_init(sh_set_t *pSet_info)
{
    return 0;
}

static int _sh_io_read(uint8_t *pBuf, uint32_t length, void *pExtra)
{
    uint32_t byte_cnt = 0;

    byte_cnt = uart_stdio_read((char *)&g_uart_rx_io.uart_cache[g_uart_rx_io.wr_idx], length);
    if (byte_cnt)
    {
        g_uart_rx_io.wr_idx = (g_uart_rx_io.wr_idx + byte_cnt) % UART_CACHE_SIZE;
    }

    if (g_uart_rx_io.rd_idx != g_uart_rx_io.wr_idx) {
        *pBuf = g_uart_rx_io.uart_cache[g_uart_rx_io.rd_idx];
        g_uart_rx_io.rd_idx = (g_uart_rx_io.rd_idx + 1) % UART_CACHE_SIZE;

        if (g_uart_rx_io.rd_idx == g_uart_rx_io.wr_idx)
        {
            g_uart_rx_io.wr_idx = 0;
            g_uart_rx_io.rd_idx = 0;
        }
        return 1;
    }

    return 0;
}

static int _sh_io_write(uint8_t *pBuf, uint32_t length, void *pExtra)
{
    return uart_stdio_write(pBuf, length);
}


static const sh_io_desc_t   g_sh_std_io =
{
    //.cb_init   = _sh_io_init,
    .cb_read   = _sh_io_read,
    .cb_write  = _sh_io_write,
    //.cb_flush  = _sh_io_flush,
};

static int _sh_cb_alarm(struct sh_args *pArg)
{
    vTaskDelay(pArg->delay_time_ms);
    return 0;
}

static void _task_console(void *pArgv)
{
    sh_set_t    sh_set = {0};

    sh_set.pLine_buf    = g_line_buf;
    sh_set.line_buf_len = sizeof(g_line_buf);
    sh_set.pHistory_buf = g_history;
    sh_set.line_size    = sizeof(g_line_buf);
    sh_set.cmd_deep     = CLI_CMD_DEEP;
    sh_set.history_buf_size = SHELL_CALC_HISTORY_BUFFER(sh_set.line_size, sh_set.cmd_deep);
    shell_init((sh_io_desc_t *)&g_sh_std_io, &sh_set);

    g_sh_args.is_blocking = 1;
    g_sh_args.delay_time_ms = 40;
    g_sh_args.cb_regular_alarm = _sh_cb_alarm;

    shell_proc(&g_sh_args);

    return;
}
//=============================================================================
//                  Public Function Definition
//=============================================================================
int
cli_console_init(void)
{
    xTaskCreate(_task_console, "cli", CLI_STACK_SIZE, NULL, configMAX_PRIORITIES-10, &cli_task_handle);

    return 0;
}

#endif
