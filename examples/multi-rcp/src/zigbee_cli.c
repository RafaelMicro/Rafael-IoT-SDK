
/**
 * @file zigbee_cli.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifdef CONFIG_APP_MULTI_RCP_ZB_GW
//=============================================================================
//                Include
//=============================================================================
#include "mcu.h"

#include "FreeRTOS.h"

#include "zb_common.h"
#include "zb_mac_globals.h"
#include "zb_zdo_globals.h"
#include "zboss_api.h"
#include "zboss_api_nwk.h"

#include <cpc_user_interface.h>
#include "cli.h"
#include "log.h"
#include "util_string.h"
#include "zigbee_app.h"
#include "zigbee_cmd.h"
#include "zigbee_gw.h"
//=============================================================================
//                Private Definitions of const value
//=============================================================================
#define ZB_TRACE_FILE_ID        294
#define ZIGBEE_DEFAULT_ENDPOINT 0x01

//=============================================================================
//                Private ENUM
//=============================================================================

//=============================================================================
//                Private Struct
//=============================================================================

//=============================================================================
//                Private Global Variables
//=============================================================================

//=============================================================================
//                Functions
//=============================================================================

static void _permit_join_response(zb_uint8_t param) {
    zb_bufid_t buf = param;
    uint8_t p_fail_pd;

    do {
        p_fail_pd = zb_buf_get_status(param);

        zb_buf_free(param);
    } while (0);

    zigbee_gw_cmd_send((ZIGBEE_CMD_PERMIT_JOIN_REQUEST | 0x8000), 0x0000, 0, 0,
                       &p_fail_pd, sizeof(p_fail_pd));
}

static int _cli_cmd_pj(int argc, char** argv, cb_shell_out_t log_out,
                       void* pExtra) {
    zb_zdo_mgmt_permit_joining_req_param_t* req_param;
    zb_bufid_t buf;
    do {
        buf = zb_buf_get_out();
        if (!buf) {
            break;
        }

        req_param = ZB_BUF_GET_PARAM(buf,
                                     zb_zdo_mgmt_permit_joining_req_param_t);

        req_param->dest_addr = ZB_PIBCACHE_NETWORK_ADDRESS();
        req_param->permit_duration = 0xFF;
        req_param->tc_significance = 1;

        zb_zdo_mgmt_permit_joining_req(buf, _permit_join_response);

    } while (0);
    return 0;
}

const sh_cmd_t g_cli_cmd_pj STATIC_CLI_CMD_ATTRIBUTE = {
    .pCmd_name = "pj",
    .cmd_exec = _cli_cmd_pj,
    .pDescription = "pj\n"
                    "  usage: pj [on/off]\n"
                    "    e.g. pj on",
};
#endif