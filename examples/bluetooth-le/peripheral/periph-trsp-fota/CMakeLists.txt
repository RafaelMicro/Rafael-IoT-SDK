sdk_add_subdirectory_ifdef(CONFIG_FREERTOS ${CMAKE_CURRENT_LIST_DIR}/rtos)
sdk_add_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/periph-trsp-fota/include
    ${CMAKE_CURRENT_LIST_DIR}/ble-app-profile/include
)
sdk_use_app_lib()
target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/ble-app-profile/src/ble_profile_app.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-app-profile/src/ble_profile_def.c
)
sdk_set_main_file(
    ${CMAKE_CURRENT_LIST_DIR}/periph-trsp-fota/src/main.c
)
sdk_add_compile_options(-DBLE_SUPPORT_NUM_CONN_MAX=${CONFIG_BLE_SUPPORT_NUM_CONN_MAX} -DBLE_PAWR_SELECT=${CONFIG_SUPPORT_PAWR})
sdk_add_compile_options_ifdef(CONFIG_BLE_DEMO_FOTA_TEST_1 -DBLE_VERSION="sys ver 0001")
sdk_add_compile_options_ifdef(CONFIG_BLE_DEMO_FOTA_TEST_2 -DBLE_VERSION="sys ver 0002")
sdk_add_compile_options(-DSET_SYS_CLK=1) #0=32Mhz, 1=48Mhz, 2=64Mhz
setup_project(periph-trsp-fota)
