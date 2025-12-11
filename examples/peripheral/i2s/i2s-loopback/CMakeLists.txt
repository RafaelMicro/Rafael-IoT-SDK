sdk_add_subdirectory_ifdef(CONFIG_FREERTOS ${CMAKE_CURRENT_LIST_DIR}/rtos)
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/i2s-loopback/include)
sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/i2s-loopback/main.c)
setup_project(i2s-loopback)
