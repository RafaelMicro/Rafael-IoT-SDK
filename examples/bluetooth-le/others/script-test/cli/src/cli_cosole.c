
/** @file cli_console.c
 *
 * @license
 * @description
 */

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "types.h"
#include "util_string.h"
#include "shell.h"
#include "uart_stdio.h"
#include "cli.h"

//=============================================================================
//                  Constant Definition
//=============================================================================
#define CLI_STACK_SIZE          1024
//=============================================================================
//                  Macro Definition
//=============================================================================
#define CLI_CMD_DEEP        (8)
#define CLI_LINE_SIZE       (256)
#define CLI_HISTORY_SIZE    SHELL_CALC_HISTORY_BUFFER(CLI_LINE_SIZE, CLI_CMD_DEEP)

#ifndef CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_PRIORITY
#define CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_PRIORITY 5
#endif
//=============================================================================
//                  Structure Definition
//=============================================================================

//=============================================================================
//                  Global Data Definition
//=============================================================================
static char         g_line_buf[CLI_LINE_SIZE] = {0};
static char         g_history[CLI_HISTORY_SIZE] = {0};
static sh_args_t    g_sh_args = {0};
#ifdef CONFIG_HOSAL_SOC_IDLE_SLEEP
static SemaphoreHandle_t    g_cli_sem;
static bool         g_cli_sleep_flag = true;
#endif

extern int stdout_string(char *str, int length);
extern int stdin_str(char *pBuf, int length);
//=============================================================================
//                  Private Function Definition
//=============================================================================
static int _sh_io_read(uint8_t *pBuf, uint32_t length, void *pExtra)
{
    uint32_t    byte_cnt = 0;

    byte_cnt = uart_stdio_read((char *)pBuf, length);
    return byte_cnt;
}

static int _sh_io_write(uint8_t *pBuf, uint32_t length, void *pExtra)
{
    return 0;
}

static const sh_io_desc_t   g_sh_std_io =
{
    .cb_read   = _sh_io_read,
    .cb_write  = _sh_io_write,
};

static int _sh_cb_alarm(struct sh_args *pArg)
{
#ifdef CONFIG_HOSAL_SOC_IDLE_SLEEP
    if (g_cli_sleep_flag)
    {
       while (xSemaphoreTake(g_cli_sem, portMAX_DELAY) != pdTRUE);
    }
#else
    vTaskDelay(pArg->delay_time_ms);
#endif
    return 0;
}

static void
_task_console(void *pArgv)
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
int cli_console_init(void)
{
#ifdef CONFIG_HOSAL_SOC_IDLE_SLEEP
    g_cli_sem = xSemaphoreCreateBinary();
    if (g_cli_sem == NULL)
    {
        printf("ERROR Happened! %s:%d status:%d\n", __FILE__, __LINE__, g_cli_sem);
    }
#endif
    xTaskCreate(_task_console, "cli", CLI_STACK_SIZE, NULL, CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_PRIORITY, NULL);

    return 0;
}

#ifdef CONFIG_HOSAL_SOC_IDLE_SLEEP
void wake_up_cli_console_isr(void)
{
    BaseType_t context_switch = pdFALSE;

    g_cli_sleep_flag = false;
    xSemaphoreGiveFromISR(g_cli_sem, &context_switch);
}

void sleep_cli_console(void)
{
    g_cli_sleep_flag = true;
}
#endif
