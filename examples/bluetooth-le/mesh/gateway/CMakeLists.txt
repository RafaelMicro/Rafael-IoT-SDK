sdk_add_subdirectory_ifdef(CONFIG_FREERTOS ${CMAKE_CURRENT_LIST_DIR}/rtos)
sdk_add_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/gateway/include
    ${CMAKE_CURRENT_LIST_DIR}/ble-app-profile/include
)
sdk_use_app_lib()
target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/gateway/src/ble_mesh_gateway_cmd_handle.c
    ${CMAKE_CURRENT_LIST_DIR}/gateway/src/uart_handler.c
    ${CMAKE_CURRENT_LIST_DIR}/cli/src/cli_cmd_gateway.c
    ${CMAKE_CURRENT_LIST_DIR}/cli/src/cli_cmd_sys.c
    ${CMAKE_CURRENT_LIST_DIR}/cli/src/cli_console.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-app-profile/src/ble_profile_def.c
)
sdk_set_main_file(
    ${CMAKE_CURRENT_LIST_DIR}/gateway/src/main.c
)
sdk_add_compile_options(-DBLE_SUPPORT_NUM_CONN_MAX=${CONFIG_BLE_SUPPORT_NUM_CONN_MAX} -DSUPPORT_DEBUG_CONSOLE_CLI=1 -DBLE_PAWR_SELECT=${CONFIG_SUPPORT_PAWR})
sdk_add_compile_options_ifdef(CONFIG_SUPPORT_MESH_CLIENT -DMESH_MODEL_CLIENT=1)
sdk_add_compile_options(-DSET_SYS_CLK=1) #0=32Mhz, 1=48Mhz, 2=64Mhz
setup_project(gateway)
