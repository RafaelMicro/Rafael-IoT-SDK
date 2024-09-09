
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/dma_mem_polling/Include)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/dma_mem_polling/main.c)
setup_project(dma_mem_polling)