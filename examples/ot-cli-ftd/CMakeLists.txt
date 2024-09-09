sdk_use_app_lib()

if(CONFIG_OT_DEVICE_TYPE_FTD)
    add_definitions(-DOPENTHREAD_FTD)
endif()

sdk_add_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/ot-cli-ftd/Include
    )
target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/ot-cli-ftd/app_task.c
    ${CMAKE_CURRENT_LIST_DIR}/ot-cli-ftd/app_udp.c
    ${CMAKE_CURRENT_LIST_DIR}/ot-cli-ftd/cli_uart.cpp
)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/ot-cli-ftd/main.c)
setup_project(ot-cli-ftd)