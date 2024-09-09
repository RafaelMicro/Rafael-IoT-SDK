#ifndef _COMMON_CMD_H_
#define _COMMON_CMD_H_

#include "atcmd_parser.h"

#define CHECK_PARAM(param_convert_result)    \
    if(!(param_convert_result))              \
    {                                        \
        return BLE_ERR_INVALID_PARAMETER; \
    }

// PUBLIC CLASS DECLARE
typedef struct cmd_info_s
{
    char *cmd_name;
    char *description;
    bool is_high_level_cmd;
    void (*init)(struct cmd_info_s *this);
    void (*do_cmd)(struct cmd_info_s *this, atcmd_item_t *item);
    ble_err_t (*set_cmd)(atcmd_item_t *item);
    ble_err_t (*read_cmd)(atcmd_item_t *item);
    void (*test_cmd)(atcmd_item_t *item);
} cmd_info_t;

// PUBLIC FUNCTION DECLARE
void cmd_info_init(cmd_info_t *this);

#endif //_COMMON_CMD_H_
