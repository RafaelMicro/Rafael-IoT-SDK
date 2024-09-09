
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/rtc-event/include)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/rtc-event/main.c)

setup_project(rtc-event)