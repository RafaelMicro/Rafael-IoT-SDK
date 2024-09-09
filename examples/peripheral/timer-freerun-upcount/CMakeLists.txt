sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/timer-freerun-upcount/include)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/timer-freerun-upcount/main.c)

if(NOT CONFIG_CHIP STREQUAL "RT584" )
    message(FATAL_ERROR "please choose chip type as rt584")
endif()

setup_project(timer-freerun-upcount)