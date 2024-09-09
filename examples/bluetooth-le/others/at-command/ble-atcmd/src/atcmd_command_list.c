#include "atcmd_command_list.h"

// PRIVATE VARIABLE DECLARE
static cmd_info_t *cmd_list[] =
{
    //Other
    &reset,
    &help,
    &role,
    &all_param,
    &sleep,
    &wake_up,
    &accept_list_size,
    &add_accept_list,
    &clear_accept_list,
    &remove_accept_list,
    //Advertising
    &enable_adv,
    &disable_adv,
    &adv_interval,
    &adv_channel_map,
    &adv_type,
    &adv_data,
    &scan_rsp,
    //Connect
    &disconnect,
    &con_interval,
    &con_latency,
    &con_timeout,
    &con_param,
    //Scanning
    &enable_scan,
    &disable_scan,
    &scan_type,
    &scan_interval,
    &scan_window,
    //Address
    &dev_addr,
    &dev_addr_type,
    &default_addr,
    //Connect_Other
    &data_length,
    &preferred_data_length,
    &preferred_mtu_size,
    &read_mtu_size,
    &phy,
    &read_phy,
    &read_rssi,
    &bonding_flag,
    &security_request,
    &restore_cccd,
    &init_bonding_info,
    //Master
    &exchange_mtu_size,
    &read,
    &write,
    &write_without_rsp,
    &read_cccd,
    &write_cccd,
    //Create_Connect
    &create_connect,
    &cancel_create_connect,
    &create_connect_interval,
    &create_connect_latency,
    &create_connect_timeout,
    //Slave
    &gap_appearance,
    &gap_device_name,
    &gap_pre_con_param,
    &notify,
    &indicate,
    &read_rsp,
};

static const int cmd_list_len = sizeof(cmd_list) / sizeof(cmd_info_t *);
static cmd_info_t *high_level_cmd_list[10];
static int high_level_cmd_list_len;

// PRIVATE FUNCTION DECLARE
void is_high_level_cmd_list_init(void);

// PUBLIC FUNCTION IMPLEMENT
void cmd_list_init(void)
{
    for (int i = 0; i < cmd_list_len; i++)
    {
        // init cmd
        cmd_list[i]->init(cmd_list[i]);
    }
    is_high_level_cmd_list_init();
}

bool cmd_assign(atcmd_item_t *item)
{
    for (int i = 0; i < cmd_list_len; i++)
    {
        if (strcmp(item->cmd_str, cmd_list[i]->cmd_name) == 0)
        {
            item->cmd_info = cmd_list[i];
            return true;
        }
    }
    return false;
}

bool high_level_cmd_check(char *cmd_str)
{
    for (int i = 0; i < high_level_cmd_list_len; i++)
    {
        char *hl_cmd_name = high_level_cmd_list[i]->cmd_name;
        if (strncmp(cmd_str, hl_cmd_name, strlen(hl_cmd_name)) == 0)
        {
            return true;
        }
    }
    return false;
}

void cmd_list_print(void)
{
    printf("The Cmd List :\n");
    for (int i = 0; i < cmd_list_len; i++)
    {
        printf("AT%s: %s\n", cmd_list[i]->cmd_name, cmd_list[i]->description);
    }
}

// PRIVATE FUNCTION IMPLEMENT
void is_high_level_cmd_list_init(void)
{
    int high_level_cmd_idx = 0;
    for (int i = 0; i < cmd_list_len; i++)
    {
        if (cmd_list[i]->is_high_level_cmd)
        {
            high_level_cmd_list[high_level_cmd_idx] = cmd_list[i];
            high_level_cmd_idx++;
        }
    }
    high_level_cmd_list_len = high_level_cmd_idx;
}
