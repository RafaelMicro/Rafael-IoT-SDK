sdk_add_subdirectory_ifdef(CONFIG_FREERTOS ${CMAKE_CURRENT_LIST_DIR}/rtos)
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/slow-timer-freerun-downcount/include)
sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/slow-timer-freerun-downcount/main.c)
setup_project(slow-timer-freerun-downcount)
