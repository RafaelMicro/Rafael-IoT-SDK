sdk_add_subdirectory_ifdef(CONFIG_FREERTOS ${CMAKE_CURRENT_LIST_DIR}/rtos)
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/rf-labtest-tool/Include)
sdk_use_app_lib()
target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/rf-labtest-tool/rfet.c
    ${CMAKE_CURRENT_LIST_DIR}/rf-labtest-tool/rfet_cli.c
    ${CMAKE_CURRENT_LIST_DIR}/rf-labtest-tool/rfet_gui.c
    ${CMAKE_CURRENT_LIST_DIR}/rf-labtest-tool/rfet_uart_bypass.c
)
sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/rf-labtest-tool/main.c)
setup_project(rf-labtest-tool)
