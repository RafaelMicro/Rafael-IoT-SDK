sdk_add_subdirectory_ifdef(CONFIG_FREERTOS ${CMAKE_CURRENT_LIST_DIR}/rtos)

sdk_use_app_lib()

if(CONFIG_MIU_DEVICE_TYPE_FTD)
    add_definitions(-DOPENTHREAD_FTD)
endif()

add_definitions(-DCONFIG_APP_TASK_UDP_LISTEN_PORT=5678)

sdk_add_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/miu-router/Include
    )

target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/miu-router/app_task.c
    ${CMAKE_CURRENT_LIST_DIR}/miu-router/app_led.c
    ${CMAKE_CURRENT_LIST_DIR}/miu-router/app_udp.c
    ${CMAKE_CURRENT_LIST_DIR}/miu-router/cli_uart.cpp
)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/miu-router/main.c)

setup_project(miu-router)
