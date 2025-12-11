sdk_add_subdirectory_ifdef(CONFIG_FREERTOS ${CMAKE_CURRENT_LIST_DIR}/rtos)
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/timer-periodic/include)
sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/timer-periodic/main.c)
setup_project(timer-periodic)
