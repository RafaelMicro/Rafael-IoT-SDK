sdk_add_subdirectory_ifdef(CONFIG_FREERTOS ${CMAKE_CURRENT_LIST_DIR}/rtos)
sdk_add_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/periph-esl/include
    ${CMAKE_CURRENT_LIST_DIR}/ble-app-profile/include
)
sdk_use_app_lib()
target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/ble-app-profile/src/ble_profile_app.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-app-profile/src/ble_profile_def.c
    ${CMAKE_CURRENT_LIST_DIR}/periph-esl/src/epd_driver.c
)
sdk_set_main_file(
    ${CMAKE_CURRENT_LIST_DIR}/periph-esl/src/main.c
)
sdk_add_compile_options(-DBLE_SUPPORT_NUM_CONN_MAX=${CONFIG_BLE_SUPPORT_NUM_CONN_MAX} -D__ARM_FEATURE_CMSE=3 -DBLE_PAWR_SELECT=${CONFIG_SUPPORT_PAWR})
sdk_add_compile_options(-DSET_SYS_CLK=1) #0=32Mhz, 1=48Mhz, 2=64Mhz
setup_project(periph-esl)
