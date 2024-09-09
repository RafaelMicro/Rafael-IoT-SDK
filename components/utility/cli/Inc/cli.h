/**
 * @file cli.h
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CLI_H__
#define __CLI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "shell.h"

#define STATIC_CLI_CMD_ATTRIBUTE __attribute__((used, section("cli_cmd_pool")))
#define __cli_cmd_pool          __attribute__ ((used, section("cli_cmd_pool")))



int cli_init(void);
/**
 * @}
 */
#ifdef __cplusplus
}
#endif

#endif