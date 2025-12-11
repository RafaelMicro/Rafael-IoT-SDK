sdk_add_subdirectory_ifdef(CONFIG_FREERTOS ${CMAKE_CURRENT_LIST_DIR}/rtos)
sdk_add_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/multi-1c1p/include
    ${CMAKE_CURRENT_LIST_DIR}/ble-app-profile/include
    ${CMAKE_CURRENT_LIST_DIR}/control-cmd/include
)
sdk_use_app_lib()
target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/ble-app-profile/src/ble_profile_app.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-app-profile/src/ble_profile_def.c
    ${CMAKE_CURRENT_LIST_DIR}/control-cmd/src/ctrl_cmd.c
    ${CMAKE_CURRENT_LIST_DIR}/control-cmd/src/print_common.c
)
sdk_set_main_file(
    ${CMAKE_CURRENT_LIST_DIR}/multi-1c1p/src/main.c
)
sdk_add_compile_options(-DBLE_SUPPORT_NUM_CONN_MAX=${CONFIG_BLE_SUPPORT_NUM_CONN_MAX} -DBLE_PAWR_SELECT=${CONFIG_SUPPORT_PAWR})
sdk_add_compile_options(-DSET_SYS_CLK=1) #0=32Mhz, 1=48Mhz, 2=64Mhz
setup_project(multi-1c1p)
