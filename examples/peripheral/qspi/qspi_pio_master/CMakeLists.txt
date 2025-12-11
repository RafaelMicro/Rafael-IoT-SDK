sdk_add_subdirectory_ifdef(CONFIG_FREERTOS ${CMAKE_CURRENT_LIST_DIR}/rtos)
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/qspi_pio_master/Include)
sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/qspi_pio_master/main.c)
setup_project(qspi_pio_master)
