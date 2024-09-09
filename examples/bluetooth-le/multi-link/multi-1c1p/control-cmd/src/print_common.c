/************************************************************************
 *
 * File Name  : printf_common.c
 * Description: This file contains the functions of BLE AT Commands print related for application.
 *
 *******************************************************************/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "ctrl_cmd.h"
#include "print_common.h"
#include "ble_profile.h"
#include "ble_scan.h"
#include "ble_app.h"

/**************************************************************************
* Public Functions
**************************************************************************/

/** Show Help Message. */
void print_ctrl_cmd_help(void)
{
    uint8_t i = 0;
    int commandSize = (sizeof(ctrl_cmd_table) / sizeof(ctrl_cmd_table[0]) - 1); // removed AT_COMMAND_NONE

    // show AT command
    for (i = 0; i < commandSize; i++)
    {
        printf("%-15s:", ctrl_cmd_table[i]);
        printf("%s\n", ctrl_cmd_description_table[i]);
    }
}



