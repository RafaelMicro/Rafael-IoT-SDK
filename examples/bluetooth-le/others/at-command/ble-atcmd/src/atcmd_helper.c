/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

#include "atcmd_helper.h"

bool jump_to_main(void)
{
    bool check = app_request_set(0xFF, APP_REQUEST_AT_CMD_PROCESS, false);
    return check;
}

bool jump_to_main_isr(void)
{
    bool check = app_request_set(0xFF, APP_REQUEST_AT_CMD_PROCESS, true);
    return check;
}
