
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/qspi_dma_master/Include)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/qspi_dma_master/main.c)
setup_project(qspi_dma_master)