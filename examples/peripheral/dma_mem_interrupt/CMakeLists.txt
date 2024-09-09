
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/dma_mem_interrupt/Include)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/dma_mem_interrupt/main.c)
setup_project(dma_mem_interrupt)