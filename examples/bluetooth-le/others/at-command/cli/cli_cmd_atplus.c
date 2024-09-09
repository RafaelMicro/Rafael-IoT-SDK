/** @file cli_cmd_rd.c
 *
 * @author Rex
 * @version 0.1
 * @date 2021/08/04
 * @license
 * @description
 */

#include "project_config.h"
#include "ble_app.h"
#include "string.h"
#if (MODULE_ENABLE(SUPPORT_DEBUG_CONSOLE_CLI))
#include "types.h"
#include "util_string.h"

#include "shell.h"
#include "bsp_console.h"

//=============================================================================
//                  Private Function Declare
//=============================================================================
static int _cli_cmd_atplus(int argc, char **argv, cb_shell_out_t log_out, void *pExtra);

//=============================================================================
//                  Global Data Definition
//=============================================================================
sh_cmd_t  g_cli_cmd_atplus =
{
    .pCmd_name      = "AT+",
    .cmd_exec       = _cli_cmd_atplus,
    .pDescription   = "AT+cmd\n"
    "    e.g. AT+HELP",
};
//=============================================================================
//                  Private Function Definition
//=============================================================================
static int _cli_cmd_atplus(int argc, char **argv, cb_shell_out_t log_out, void *pExtra)
{
    uint8_t cmd[300] = {0};
    uint32_t tmplen = 0, argv_len = 0;

    for (int i = 0; i < argc; i++)
    {
        argv_len = utility_strlen(argv[i]);
        memcpy((char *)&cmd[tmplen], argv[i], argv_len);
        tmplen += argv_len;
    }
    uart_rx_data_handle(cmd, tmplen + 1);

    return 0;
}

#endif
