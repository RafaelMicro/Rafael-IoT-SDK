
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/timer-freerun-downcount/include)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/timer-freerun-downcount/main.c)

setup_project(timer-freerun-downcount)