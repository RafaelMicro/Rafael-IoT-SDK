
sdk_add_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/at-command/include
    ${CMAKE_CURRENT_LIST_DIR}/ble-app-profile/include
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/include
)
sdk_use_app_lib()
target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/ble-app-profile/src/ble_profile_app.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-app-profile/src/ble_profile_def.c

    ${CMAKE_CURRENT_LIST_DIR}/cli/src/cli_cmd_atplus.c
    ${CMAKE_CURRENT_LIST_DIR}/cli/src/cli_cosole.c

    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/atcmd_command_list.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/atcmd_command.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/atcmd_helper.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/atcmd_init.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/atcmd_parser.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/atcmd_queue.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/atcmd.c

    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/address/default_addr.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/address/dev_addr_type.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/address/dev_addr.c

    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/advertising/adv_channel_map.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/advertising/adv_data.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/advertising/adv_interval.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/advertising/adv_type.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/advertising/disable_adv.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/advertising/enable_adv.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/advertising/scan_rsp.c

    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/connect/con_interval.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/connect/con_latency.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/connect/con_param.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/connect/con_timeout.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/connect/disconnect.c

    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/connect_other/bonding_flag.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/connect_other/data_length.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/connect_other/init_bonding_info.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/connect_other/phy.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/connect_other/preferred_data_length.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/connect_other/preferred_mtu_size.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/connect_other/read_mtu_size.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/connect_other/read_phy.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/connect_other/restore_cccd.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/connect_other/rssi.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/connect_other/security.c

    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/other/add_accept_list.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/other/all_param.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/other/clear_filter_accept_list.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/other/help.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/other/read_filter_accept_list_size.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/other/remove_accept_list.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/other/reset.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/other/role.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/other/sleep.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/other/wake_up.c

    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/scanning/disable_scan.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/scanning/enable_scan.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/scanning/scan_interval.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/scanning/scan_type.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/common/scanning/scan_window.c

    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/master/create_connect/cancel_create_connect.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/master/create_connect/create_connect_interval.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/master/create_connect/create_connect_latency.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/master/create_connect/create_connect_timeout.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/master/create_connect/create_connect.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/master/exchange_mtu_size.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/master/read_cccd.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/master/read.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/master/write_cccd.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/master/write_without_rsp.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/master/write.c

    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/slave/gap_appearance.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/slave/gap_device_name.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/slave/gap_pre_con_param.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/slave/indicate.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/slave/notify.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-atcmd/src/command/slave/read_rsp.c
)
sdk_set_main_file(
    ${CMAKE_CURRENT_LIST_DIR}/at-command/src/main.c
)

sdk_add_compile_options(-DBLE_SUPPORT_NUM_CONN_MAX=${CONFIG_BLE_SUPPORT_NUM_CONN_MAX})
setup_project(at-command)