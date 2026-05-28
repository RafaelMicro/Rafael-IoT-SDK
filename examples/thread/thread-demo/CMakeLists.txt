sdk_add_subdirectory_ifdef(CONFIG_FREERTOS ${CMAKE_CURRENT_LIST_DIR}/rtos)
sdk_use_app_lib()
sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/main.c)
sdk_add_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/Include
    )
if(CONFIG_APP_DEVICE_TYPE_FTD)    
    add_definitions(-DOPENTHREAD_FTD)
    target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/ftd/app_task.c
    ${CMAKE_CURRENT_LIST_DIR}/ftd/app_udp.c
    ${CMAKE_CURRENT_LIST_DIR}/ftd/cli_uart.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ftd/uartConfig.c
)
    setup_project(thread-demo-ftd)
elseif(CONFIG_APP_DEVICE_TYPE_MTD)
    add_definitions(-DOPENTHREAD_MTD)
    target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/mtd/app_task.c
    ${CMAKE_CURRENT_LIST_DIR}/ftd/app_udp.c
    ${CMAKE_CURRENT_LIST_DIR}/mtd/cli_uart.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mtd/uartConfig.c
)
    setup_project(thread-demo-mtd)
endif()
