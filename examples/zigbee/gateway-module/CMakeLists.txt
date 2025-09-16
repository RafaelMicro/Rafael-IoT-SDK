sdk_add_subdirectory_ifdef(CONFIG_FREERTOS ${CMAKE_CURRENT_LIST_DIR}/rtos)
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
app_git_version(APP_PACKAGE_VERSION)
message(STATUS "${CONFIG_BUILD_PORJECT} version : ${APP_PACKAGE_VERSION}")
add_compile_options(-DCONFIG_PROJECT_VERSION="${APP_PACKAGE_VERSION}")
sdk_use_app_lib()
target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/zigbee_api.c
    ${CMAKE_CURRENT_LIST_DIR}/src/zcl_construction.c
    ${CMAKE_CURRENT_LIST_DIR}/src/zigbee_cmd_nwk.c
    ${CMAKE_CURRENT_LIST_DIR}/src/zigbee_cmd_app.c
    ${CMAKE_CURRENT_LIST_DIR}/src/zigbee_cmd_ota.c
    ${CMAKE_CURRENT_LIST_DIR}/src/zigbee_cli.c
    ${CMAKE_CURRENT_LIST_DIR}/src/uart_handler.c
    ${CMAKE_CURRENT_LIST_DIR}/src/zigbee_zcl_msg_handler.c
)
if (CONFIG_BUILD_COMPONENT_ENHANCED_FLASH_DATASET)
    sdk_add_compile_options(
        -DEFD_START_ADDR=0xF0000
        -DENV_AREA_SIZE=0x4000
    )
endif()
sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/src/main.c)
setup_project(gateway-module)
