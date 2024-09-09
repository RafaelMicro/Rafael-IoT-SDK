
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/slow-timer-periodic/include)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/slow-timer-periodic/main.c)
setup_project(slow-timer-periodic)