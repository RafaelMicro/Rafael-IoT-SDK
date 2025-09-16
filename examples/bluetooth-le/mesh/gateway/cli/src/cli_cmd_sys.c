/** @file cli_cmd_rd.c
 *
 * @author Rex
 * @version 0.1
 * @date 2021/08/04
 * @license
 * @description
 */

#if (SUPPORT_DEBUG_CONSOLE_CLI == 1)
#include <string.h>
#include "cli.h"
#include "shell.h"
#include "types.h"
#include "util_string.h"
#include "sysfun.h"
#include "FreeRTOS.h"
#include "task.h"

//=============================================================================
//                  Constant Definition
//=============================================================================

//=============================================================================
//                  Macro Definition
//=============================================================================

//=============================================================================
//                  Structure Definition
//=============================================================================

//=============================================================================
//                  Global Data Definition
//=============================================================================

//=============================================================================
//                  Private Function Definition
//=============================================================================
static int _cli_cmd_ps(int argc, char** argv, cb_shell_out_t log_out,
                       void* pExtra) {
    do {
#if 0
        uint8_t *CPU_RunInfo = pvPortMalloc(2048);
        uint32_t u32_tick = xTaskGetTickCount();

        memset(CPU_RunInfo, 0, 2048);
        vTaskList((char *)CPU_RunInfo);
        log_out("---------------------------------------------\n");
        log_out("Task Name\tState\tPrio\tStack\tNum\n");
        log_out("%s", CPU_RunInfo);
        log_out("\n");
        log_out("---------------------------------------------\n");
        memset(CPU_RunInfo, 0, 2048);
#if 0
        vTaskGetRunTimeStats((char *)CPU_RunInfo);
        log_out("---------------------------------------------\n");
        log_out("Task Name\tCount\t\tRate\n");
        log_out("%s", CPU_RunInfo);
#endif
        log_out("---------------------------------------------\n");
        log_out("System boot time :");
        if (u32_tick > 86400000)
        {
            log_out(" %u D", u32_tick / 86400000);
        }
        if (u32_tick > 3600000)
        {
            log_out(" %u h", (u32_tick / 3600000) % 24);
        }
        if (u32_tick > 60000)
        {
            log_out(" %u m", (u32_tick / 60000) % 60);
        }
        log_out(" %u.%u s\n", (u32_tick / 1000) % 60, u32_tick % 1000);
        vPortFree(CPU_RunInfo);
#endif
    } while (0);
    return 0;
}

static int _cli_cmd_sys_reset(int argc, char** argv, cb_shell_out_t log_out,
                              void* pExtra) {
    sys_software_reset();

    return 0;
}

//=============================================================================
//                  Public Function Definition
//=============================================================================

const sh_cmd_t g_cli_cmd_mem STATIC_CLI_CMD_ATTRIBUTE = {
    .pCmd_name = "ps",
    .cmd_exec = _cli_cmd_ps,
    .pDescription = "Show task counts\n"
                    "  usage: ps\n"
                    "    e.g. ps",
};

const sh_cmd_t g_cli_cmd_sys_reset STATIC_CLI_CMD_ATTRIBUTE = {
    .pCmd_name = "reset",
    .cmd_exec = _cli_cmd_sys_reset,
    .pDescription = "reset device\n"
                    "  usage: reset\n"
                    "    e.g. reset",
};

#endif
