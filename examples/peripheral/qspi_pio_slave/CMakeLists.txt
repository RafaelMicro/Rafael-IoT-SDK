
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/qspi_pio_slave/Include)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/qspi_pio_slave/main.c)
setup_project(qspi_pio_slave)