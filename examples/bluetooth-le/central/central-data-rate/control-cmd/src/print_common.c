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


/**************************************************************************
* Public Functions
**************************************************************************/

/** Show Data Rate Tool Version. */
void print_app_tool_version(void)
{
    printf("%s\r\n", DATA_RATE_TOOL_VER);
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
    printf("[CONN] interval(ms) =%0.1f\n", (connect_info.conn_interval * 1.25));
}


/** Show RF PHY Settings. */
void print_app_phy_param(void)
{
    if (connect_info.phy == BLE_PHY_1M)
    {
        printf("[PHY] phy = BLE_PHY_1M\n");
    }
    else if (connect_info.phy == BLE_PHY_2M)
    {
        printf("[PHY] phy = BLE_PHY_2M\n");
    }
    else if (connect_info.phy == BLE_PHY_CODED)
    {
        printf("[PHY] phy = BLE_EVT_PHY_CODED\n");
    }
    else
    {
        printf("[PHY] error\n");
    }
}

/** Show Connection Parameters. */
void print_app_data_len_param(void)
{
    printf("[Data Length] %d\n", connect_info.data_length);
}

/** Show Test Mode. */
void print_app_test_mode_param(void)
{
    if (app_DR_test_info.test_mode == DATARATE_MODE_TX)
    {
        printf("[Test Mode] TX\n");
    }
    else
    {
        printf("[Test Mode] RX\n");
    }
}

/** Show Test TX Total Length. */
void print_app_test_total_len_param(void)
{
    printf("[Total TX Test Len] %ld\n", app_DR_test_info.total_Tx_test_len);
}

/** Show All Application Parameters. */
void print_app_param(void)
{
    // show local device addr
    print_localAddr_param();

    // show target device
    print_targetAddr_param();

    // connection param
    print_app_conn_param();

    // rf phy
    print_app_phy_param();

    // Data Length
    print_app_data_len_param();

    // Test TX Total Length
    print_app_test_total_len_param();

    // Test Mode
    print_app_test_mode_param();
}

/** Show Help Message. */
void print_ctrl_cmd_help(void)
{
    uint8_t i = 0;
    int commandSize = (sizeof(ctrl_cmd_table) / sizeof(ctrl_cmd_table[0]) - 1);

    // show BLE row
    printf("Role: Central\n");

    // show local device addr
    print_localAddr_param();

    // show target device
    print_targetAddr_param();

    // banner
    printf("---------------------------------------------\n");
    printf("Control Command: \n");
    printf("---------------------------------------------\n");

    // show AT command
    for (i = 0; i < commandSize; i++)
    {
        printf("%-10s:", ctrl_cmd_table[i]);
        printf("%s\r\n", ctrl_cmd_description_table[i]);
    }

    printf("\n\n");
}



