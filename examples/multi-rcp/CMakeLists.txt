
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

app_git_version(APP_PACKAGE_VERSION)
message(STATUS "${CONFIG_BUILD_PORJECT} version : ${APP_PACKAGE_VERSION}")
add_compile_options(-DCONFIG_PROJECT_VERSION="${APP_PACKAGE_VERSION}")
sdk_use_app_lib()
target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/cpc_hci.c
    ${CMAKE_CURRENT_LIST_DIR}/src/cpc_uart.c
    ${CMAKE_CURRENT_LIST_DIR}/src/cpc_upgrade.c
    ${CMAKE_CURRENT_LIST_DIR}/src/cli_uart.cpp
)
if(CONFIG_APP_MULTI_RCP_ZB_GW)
target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/zigbee_app.c
    ${CMAKE_CURRENT_LIST_DIR}/src/zigbee_cmd.c
    ${CMAKE_CURRENT_LIST_DIR}/src/zigbee_gw.c
    ${CMAKE_CURRENT_LIST_DIR}/src/zigbee_cli.c
)
endif()

if(CONFIG_BUILD_COMPONENT_SYSLOG)
target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/app_syslog.c
)
endif()

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/src/main.c)
setup_project(multi-rcp)