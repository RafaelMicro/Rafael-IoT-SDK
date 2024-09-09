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

/** Show Data Rate Tool Version. */
void print_app_tool_version(void)
{
    printf("%s\r\n", POWER_MEASUREMENT_TOOL_VER);
}

/** Show Target Device Address. */
void print_targetAddr_param(void)
{
    printf("Target Addr: %02x:%02x:%02x:%02x:%02x:%02x\n",  g_target_addr.addr[5], g_target_addr.addr[4],
           g_target_addr.addr[3], g_target_addr.addr[2],
           g_target_addr.addr[1], g_target_addr.addr[0]);
}

void print_localAddr_param(void)
{
    ble_gap_addr_t addr;

    ble_cmd_device_addr_get(&addr);
    printf("Local Addr: %02x:%02x:%02x:%02x:%02x:%02x\n",  addr.addr[5], addr.addr[4], addr.addr[3], addr.addr[2], addr.addr[1], addr.addr[0]);
}


/** Show Connection Parameters. */
void print_app_conn_param(void)
{
    printf("[CONN] interval(ms) =%0.1f, latency = %d\n", (appParam.conn_param.ble_conn_param.max_conn_interval * 1.25), appParam.conn_param.ble_conn_param.periph_latency);
}

/** Show Advertisement Parameters. */
void print_app_adv_param(void)
{
    if (appParam.app_param_p.adv_param.adv_type == ADV_TYPE_ADV_IND)
    {
        printf("[ADV]  type: ADV_TYPE_ADV_IND, interval(ms) = %d\n", (appParam.app_param_p.adv_param.adv_interval_max * 5) >> 3);
    }
    else
    {
        printf("[ADV]  type: ADV_TYPE_ADV_NONCONN_IND, interval(ms) = %d\n", (appParam.app_param_p.adv_param.adv_interval_max * 5) >> 3);
    }
}

/** Show Scan Parameters. */
void print_app_scan_param(void)
{
    if (appParam.app_param_c.scan_param.scan_type == SCAN_TYPE_ACTIVE)
    {
        printf("[SCAN] type: SCAN_TYPE_ACTIVE, interval(ms) = %d, window(ms) = %d\n", (appParam.app_param_c.scan_param.scan_interval * 5) >> 3, (appParam.app_param_c.scan_param.scan_window * 5) >> 3);
    }
    else
    {
        printf("[SCAN] type: SCAN_TYPE_PASSIVE, interval(ms) = %d, window(ms) = %d\n", (appParam.app_param_c.scan_param.scan_interval * 5) >> 3, (appParam.app_param_c.scan_param.scan_window * 5) >> 3);
    }
}

/** Show RF PHY Settings. */
void print_app_phy_param(void)
{
    if (appParam.rf_phy == BLE_PHY_1M)
    {
        printf("[PHY]  phy = BLE_PHY_1M\n");
    }
    else if (appParam.rf_phy == BLE_PHY_2M)
    {
        printf("[PHY]  phy = BLE_PHY_2M\n");
    }
    else if (appParam.rf_phy == BLE_PHY_CODED)
    {
        if (appParam.phy_option == BLE_CODED_PHY_S2)
        {
            printf("[PHY]  phy = BLE_PHY_CODED_S2\n");
        }
        else if (appParam.phy_option == BLE_CODED_PHY_S8)
        {
            printf("[PHY]  phy = BLE_PHY_CODED_S8\n");
        }
    }
}

/** Show All Application Parameters. */
void print_app_param(void)
{
    // adv param
    print_app_adv_param();

    // scan param
    print_app_scan_param();

    // connection param
    print_app_conn_param();

    // rf phy
    print_app_phy_param();
}

/** Show Help Message. */
void print_ctrl_cmd_help(void)
{
    uint8_t i = 0;
    int commandSize = (sizeof(ctrl_cmd_table) / sizeof(ctrl_cmd_table[0]) - 1); // removed AT_COMMAND_NONE

    // show target device
    print_targetAddr_param();

    // show AT command
    for (i = 0; i < commandSize; i++)
    {
        printf("%-15s:", ctrl_cmd_table[i]);
        printf("%s\r\n", ctrl_cmd_description_table[i]);
    }
}



