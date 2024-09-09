/**
 * @file cli.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <FreeRTOS.h>
#include <stdio.h>
#include <string.h>
#include <task.h>
#include "mcu.h"
#include "uart_stdio.h"

#include "cli.h"
#include "shell.h"
#ifdef CONFIG_BUILD_COMPONENT_SW_TIMER
#include "sw_timer.h"
#endif // CONFIG_BUILD_COMPONENT_SW_TIMER

#define SYS_CLI_TASK_STACK_SIZE 2048
#define SYS_CLI_TASK_PRIORITY   (28)

#define CLI_CMD_DEEP     (8)
#define CLI_LINE_SIZE    (256)
#define CLI_HISTORY_SIZE SHELL_CALC_HISTORY_BUFFER(CLI_LINE_SIZE, CLI_CMD_DEEP)

#define UART_CACHE_SIZE 256

typedef struct uart_io {
    volatile uint32_t wr_idx;
    volatile uint32_t rd_idx;
    uint8_t uart_cache[UART_CACHE_SIZE];
} uart_io_t;

static uart_io_t g_uart_rx_io = {
    .wr_idx = 0,
    .rd_idx = 0,
};

static char g_line_buf[CLI_LINE_SIZE] = {0};
static char g_history[CLI_HISTORY_SIZE] = {0};
static sh_args_t g_sh_args = {0};

static int _sh_io_read(uint8_t* pBuf, uint32_t length, void* pExtra) {
    uint32_t byte_cnt = 0;

    if (g_uart_rx_io.rd_idx != g_uart_rx_io.wr_idx) {
        pBuf[byte_cnt] = g_uart_rx_io.uart_cache[g_uart_rx_io.rd_idx];

        g_uart_rx_io.rd_idx = (g_uart_rx_io.rd_idx + 1) % UART_CACHE_SIZE;
        return 1;
    }

    return 0;
}

static int _sh_io_write(uint8_t* pBuf, uint32_t length, void* pExtra) {
    return uart_stdio_write(pBuf, length);
}

static const sh_io_desc_t g_sh_std_io = {
    .cb_read = _sh_io_read,
    .cb_write = _sh_io_write,
};

static void cli_task(void* pvParameters) {
    uint8_t cnt = 0;
    sh_set_t sh_set = {
        .pLine_buf = g_line_buf,
        .line_buf_len = sizeof(g_line_buf),
        .pHistory_buf = g_history,
        .line_size = sizeof(g_line_buf),
        .cmd_deep = CLI_CMD_DEEP,
        .history_buf_size = SHELL_CALC_HISTORY_BUFFER(sh_set.line_size,
                                                      sh_set.cmd_deep),
    };
    shell_init((sh_io_desc_t*)&g_sh_std_io, &sh_set);
    g_sh_args.is_blocking = 0;
    while (1) {
        cnt = uart_stdio_read(g_uart_rx_io.uart_cache, 0);
        if (cnt) {
            g_uart_rx_io.wr_idx = (g_uart_rx_io.wr_idx + cnt) % UART_CACHE_SIZE;
            while (cnt--)
                shell_proc(&g_sh_args);
            g_uart_rx_io.rd_idx = 0;
            g_uart_rx_io.wr_idx = 0;
        }
        vTaskDelay(10);
    }
}

static int _cli_cmd_ps(int argc, char** argv, cb_shell_out_t log_out,
                       void* pExtra) {
    char* pbuffer = (char*)pvPortCalloc(1, 2048);

    log_out("--------------- heap:%u ---------------------\r\n",
            xPortGetFreeHeapSize());
    log_out("Task Name\tState\tPrio\tStack\tNum\r\n");
    vTaskList(pbuffer);
    log_out("%s", pbuffer);
    log_out("----------------------------------------------\r\n");

    // log_out("Task Name\tCount\t\tRate\r\n");
    // vTaskGetRunTimeStats((char *)pbuffer);
    // log_out("%s", pbuffer);
    // log_out("----------------------------------------------\r\n");

    // log_out("System boot time : ");
    // if(Timer_25us_Tick > 3456000000)
    //     log_out("%u D, ", (Timer_25us_Tick/3456000000));

    // if(Timer_25us_Tick > 144000000)
    //     log_out("%u h, ", (Timer_25us_Tick/144000000) % 24);

    // if(Timer_25us_Tick > 2400000)
    //     log_out("%u m, ", (Timer_25us_Tick/2400000) % 60);

    // log_out("%u.%u s \r\n", ((Timer_25us_Tick/40000) % 60), ((Timer_25us_Tick/40) % 1000));
    vPortFree(pbuffer);

    return 0;
}
#if (CONFIG_BUILD_COMPONENT_SW_TIMER == 1)                                     \
    && (CONFIG_SW_TIMER_ENABLE_DEBUG == 1)
static int _cli_cmd_swtimer(int argc, char** argv, cb_shell_out_t log_out,
                            void* pExtra) {
    sw_timer_state_print();
    return 0;
}
#endif // CONFIG_BUILD_COMPONENT_SW_TIMER
int cli_init(void) {
    xTaskCreate(cli_task, (char*)"cli",
                SYS_CLI_TASK_STACK_SIZE / sizeof(StackType_t), NULL,
                E_TASK_PRIORITY_CLI, NULL);

    return 0;
}
#if (CONFIG_BUILD_COMPONENT_SW_TIMER == 1)                                     \
    && (CONFIG_SW_TIMER_ENABLE_DEBUG == 1)
const sh_cmd_t g_cli_cmd_swtimer STATIC_CLI_CMD_ATTRIBUTE = {
    .pCmd_name = "swtimer",
    .pDescription = "Show software timer",
    .cmd_exec = _cli_cmd_swtimer,
};
#endif
const sh_cmd_t g_cli_cmd_ps STATIC_CLI_CMD_ATTRIBUTE = {
    .pCmd_name = "ps",
    .pDescription = "Show task and memory usage",
    .cmd_exec = _cli_cmd_ps,
};