
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/uart1_dma_it_loopback/Include)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/uart1_dma_it_loopback/main.c)
setup_project(uart1_dma_it_loopback)